#include "tdpch.h"
#include "ScriptingEngine.h"

#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include <mono/metadata/attrdefs.h>

namespace Teddy {

	namespace Utils {

		static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
		{
			{ "System.Single",  ScriptFieldType::Float },
			{ "System.Double",  ScriptFieldType::Double },
			{ "System.Boolean", ScriptFieldType::Bool },
			{ "System.Char",    ScriptFieldType::Char },
			{ "System.Int16",   ScriptFieldType::Short },
			{ "System.Int32",   ScriptFieldType::Int },
			{ "System.Int64",   ScriptFieldType::Long },
			{ "System.Byte",    ScriptFieldType::Byte },
			{ "System.UInt16",  ScriptFieldType::UShort },
			{ "System.UInt32",  ScriptFieldType::UInt },
			{ "System.UInt64",  ScriptFieldType::ULong },

			{ "Teddy.Vector2", ScriptFieldType::Vector2 },
			{ "Teddy.Vector3", ScriptFieldType::Vector3 },
			{ "Teddy.Vector4", ScriptFieldType::Vector4 },

			{ "Teddy.Entity", ScriptFieldType::Entity },
		};

		static ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
		{
			std::string typeName = mono_type_get_name(monoType);

			auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end())
				return ScriptFieldType::None;

			return it->second;
		}

		// TODO: move to FileSystem class
		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint64_t size = end - stream.tellg();

			if (size == 0)
			{
				// File is empty
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = (uint32_t)size;
			return buffer;
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			std::string pathString = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}

	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;

		// Runtime
		Scene* SceneContext = nullptr;
	};

	static std::unique_ptr<ScriptEngineData> s_Data;

	void ScriptingEngine::Init()
	{
		s_Data = std::make_unique<ScriptEngineData>();

		InitMono();
		LoadAssembly("Resources/Scripts/ScriptCore.dll");
		LoadAssemblyClasses(s_Data->CoreAssembly);

		ScriptGlue::RegisterComponents();
		ScriptGlue::RegisterFunctions();

		// Retrieve and instantiate class
		s_Data->EntityClass = ScriptClass("Teddy", "Entity");

		// Call ScriptRegistry::RegisterScripts()
		MonoClass* scriptRegistryClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Teddy", "ScriptRegistry");
		MonoMethod* registerScriptsMethod = mono_class_get_method_from_name(scriptRegistryClass, "RegisterScripts", 0);
		mono_runtime_invoke(registerScriptsMethod, nullptr, nullptr, nullptr);
#if 0

		MonoObject* instance = s_Data->EntityClass.Instantiate();

		// Call method
		MonoMethod* printMessageFunc = s_Data->EntityClass.GetMethod("PrintMessage", 0);
		s_Data->EntityClass.InvokeMethod(instance, printMessageFunc);

		// Call method with param
		MonoMethod* printIntFunc = s_Data->EntityClass.GetMethod("PrintInt", 1);

		int value = 5;
		void* param = &value;

		s_Data->EntityClass.InvokeMethod(instance, printIntFunc, &param);

		MonoMethod* printIntsFunc = s_Data->EntityClass.GetMethod("PrintInts", 2);
		int value2 = 508;
		void* params[2] =
		{
			&value,
			&value2
		};
		s_Data->EntityClass.InvokeMethod(instance, printIntsFunc, params);

		MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello World from C++!");
		MonoMethod* printCustomMessageFunc = s_Data->EntityClass.GetMethod("PrintCustomMessage", 1);
		void* stringParam = monoString;
		s_Data->EntityClass.InvokeMethod(instance, printCustomMessageFunc, &stringParam);

		TD_CORE_ASSERT(false);
#endif
	}

	void ScriptingEngine::Shutdown()
	{
		ShutdownMono();
		s_Data.reset();
	}

	void ScriptingEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("TeddyJITRuntime");
		//TD_CORE_ASSERT(rootDomain);

		// Store the root domain pointer
		s_Data->RootDomain = rootDomain;
	}

	void ScriptingEngine::ShutdownMono()
	{
		// NOTE(Yan): mono is a little confusing to shutdown, so maybe come back to this

		if (s_Data->AppDomain)
			mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		if (s_Data->RootDomain)
			mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}

	void ScriptingEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		// Create an App Domain
		s_Data->AppDomain = mono_domain_create_appdomain("TeddyScriptRuntime", nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		// Move this maybe
		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
		// Utils::PrintAssemblyTypes(s_Data->CoreAssembly);
	}

	void ScriptingEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->SceneContext = scene;
	}

	bool ScriptingEngine::EntityClassExists(const std::string& fullClassName)
	{
		return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
	}

	void ScriptingEngine::OnCreateEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();
		if (ScriptingEngine::EntityClassExists(sc.ClassName))
		{
			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);
			s_Data->EntityInstances[entity.GetComponent<UUIDComponent>().id] = instance;
			instance->InvokeOnCreate();
		}
	}

	void ScriptingEngine::OnUpdateEntity(Entity entity, Timestep ts)
	{
		UUID entityUUID = entity.GetComponent<UUIDComponent>().id;
		//TD_CORE_ASSERT(s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end());

		Ref<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
		instance->InvokeOnUpdate((float)ts);
	}

	Scene* ScriptingEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	void ScriptingEngine::OnRuntimeStop()
	{
		s_Data->SceneContext = nullptr;

		s_Data->EntityInstances.clear();
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptingEngine::GetEntityClasses()
	{
		std::unordered_map<std::string, Ref<ScriptClass>> sandboxClasses;
		for (auto const& [name, scriptClass] : s_Data->EntityClasses)
		{
			if (name.rfind("Sandbox.", 0) == 0) // Check if the name starts with "Sandbox."
			{
				sandboxClasses[name] = scriptClass;
			}
		}
		return sandboxClasses;
	}

	void ScriptingEngine::LoadAssemblyClasses(MonoAssembly* assembly)
	{
		s_Data->EntityClasses.clear();

		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = mono_class_from_name(image, "Teddy", "Entity");

		if (entityClass == nullptr)
		{
			TD_CORE_ERROR("Could not find Teddy.Entity class in ScriptCore assembly!");
			return;
		}

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, name);
			else
				fullName = name;

			MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);

			if (monoClass == nullptr)
			{
				TD_CORE_WARN("Could not get class from name: {}", fullName);
				continue;
			}

			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (isEntity)
				s_Data->EntityClasses[fullName] = CreateRef<ScriptClass>(nameSpace, name);
		}
	}

	void ScriptingEngine::PrintAssemblyTypes()
	{
		MonoImage* image = s_Data->CoreAssemblyImage;
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		TD_CORE_TRACE("C# Classes in ScriptCore.dll:");
		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			TD_CORE_TRACE("  - {}.{}", nameSpace, name);
		}
	}

	MonoImage* ScriptingEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
	}

	MonoDomain* ScriptingEngine::GetAppDomain()
	{
		return s_Data->AppDomain;
	}

	MonoObject* ScriptingEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());

		// Get ShowInEditor attribute class
		MonoClass* showInEditorAttr = mono_class_from_name(s_Data->CoreAssemblyImage, "Teddy", "ShowInEditorAttribute");

		void* iter = nullptr;
		while (MonoClassField* field = mono_class_get_fields(m_MonoClass, &iter))
		{
			const char* fieldName = mono_field_get_name(field);
			uint32_t flags = mono_field_get_flags(field);
			if (flags & MONO_FIELD_ATTR_PUBLIC)
			{
				MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_field(m_MonoClass, field);
				if (attrInfo)
				{
					bool hasAttribute = mono_custom_attrs_has_attr(attrInfo, showInEditorAttr);
					mono_custom_attrs_free(attrInfo);
					if (hasAttribute)
					{
						MonoType* monoType = mono_field_get_type(field);
						ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(monoType);
						m_Fields[fieldName] = { fieldType, fieldName, field };
					}
				}
			}
		}
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptingEngine::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params, MonoObject** exc)
	{
		return mono_runtime_invoke(method, instance, params, exc);
	}

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();
		m_GCHandle = mono_gchandle_new(m_Instance, false);

		m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

		// Call Entity constructor
		{
			UUID entityID = entity.GetComponent<UUIDComponent>().id;
			void* param = &entityID;
			m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
		}
	}

	ScriptInstance::~ScriptInstance()
	{
		mono_gchandle_free(m_GCHandle);
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_OnCreateMethod)
		{
			MonoObject* exc = nullptr;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod, nullptr, &exc);
			if (exc)
			{
				MonoString* exc_str = mono_object_to_string(exc, nullptr);
				char* c_str = mono_string_to_utf8(exc_str);
				TD_CORE_ERROR("C# Exception: {0}", c_str);
				mono_free(c_str);
			}
		}
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		if (m_OnUpdateMethod)
		{
			void* param = &ts;
			MonoObject* exc = nullptr;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param, &exc);
			if (exc)
			{
				MonoString* exc_str = mono_object_to_string(exc, nullptr);
				char* c_str = mono_string_to_utf8(exc_str);
				TD_CORE_ERROR("C# Exception: {0}", c_str);
				mono_free(c_str);
			}
		}
	}

	void ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return;

		const ScriptField& field = it->second;
		mono_field_get_value(m_Instance, field.ClassField, buffer);
	}

	void ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return;

		const ScriptField& field = it->second;
		mono_field_set_value(m_Instance, field.ClassField, (void*)value);
	}


	Ref<ScriptInstance> ScriptingEngine::GetEntityScriptInstance(UUID entityID)
	{
		auto it = s_Data->EntityInstances.find(entityID);
		if (it == s_Data->EntityInstances.end())
			return nullptr;

		return it->second;
	}

}