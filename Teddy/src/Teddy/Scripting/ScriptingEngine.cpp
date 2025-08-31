#include "tdpch.h"
#include "ScriptingEngine.h"
#include "ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/threads.h>

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

		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
				return nullptr;

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint64_t size = end - stream.tellg();

			if (size == 0)
				return nullptr;

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = (uint32_t)size;
			return buffer;
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath, bool loadPDB = false)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);

			if (fileData == nullptr)
				return nullptr;

			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				TD_CORE_ERROR("Failed to open mono image: {}", errorMessage);
				return nullptr;
			}

			if (loadPDB)
			{
				std::filesystem::path pdbPath = assemblyPath;
				pdbPath.replace_extension(".pdb");

				if (std::filesystem::exists(pdbPath))
				{
					uint32_t pdbFileSize = 0;
					char* pdbFileData = ReadBytes(pdbPath, &pdbFileSize);
					mono_debug_open_image_from_memory(image, (const mono_byte*)pdbFileData, pdbFileSize);
					delete[] pdbFileData;
				}
			}

			std::string pathString = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
			mono_image_close(image);

			delete[] fileData;

			return assembly;
		}

	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* CoreAppDomain = nullptr;
		MonoDomain* GameAppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		MonoAssembly* GameAssembly = nullptr;
		MonoImage* GameAssemblyImage = nullptr;
        std::filesystem::path GameAssemblyPath;

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
	}

	void ScriptingEngine::Shutdown()
	{
		ShutdownMono();
		s_Data.reset();
	}

	void ScriptingEngine::LoadAssemblies(const std::filesystem::path& corePath, const std::filesystem::path& gamePath)
	{
		s_Data->CoreAppDomain = mono_domain_create_appdomain("TeddyScriptRuntime", nullptr);
		mono_domain_set(s_Data->CoreAppDomain, true);

		s_Data->CoreAssembly = Utils::LoadMonoAssembly(corePath, true);
		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);

		s_Data->GameAssemblyPath = gamePath;
		if (std::filesystem::exists(s_Data->GameAssemblyPath))
		{
			s_Data->GameAppDomain = mono_domain_create_appdomain("TeddyGameRuntime", nullptr);
			mono_domain_set(s_Data->GameAppDomain, true);
			s_Data->GameAssembly = Utils::LoadMonoAssembly(gamePath, true);
			s_Data->GameAssemblyImage = mono_assembly_get_image(s_Data->GameAssembly);
			LoadAssemblyClasses(s_Data->GameAssembly);
			mono_domain_set(s_Data->CoreAppDomain, true);
		}

		LoadAssemblyClasses(s_Data->CoreAssembly);

		ScriptGlue::RegisterComponents();
		ScriptGlue::RegisterFunctions();

		s_Data->EntityClass = ScriptClass("Teddy", "Entity");
	}

	void ScriptingEngine::ReloadGameAssembly()
	{
		if (!IsGameAssemblyLoaded())
			return;

		UnloadGameAssembly();

		s_Data->GameAppDomain = mono_domain_create_appdomain("TeddyGameRuntime", nullptr);
		mono_domain_set(s_Data->GameAppDomain, true);
		s_Data->GameAssembly = Utils::LoadMonoAssembly(s_Data->GameAssemblyPath, true);
		s_Data->GameAssemblyImage = mono_assembly_get_image(s_Data->GameAssembly);
		LoadAssemblyClasses(s_Data->GameAssembly);
		mono_domain_set(s_Data->CoreAppDomain, true);
	}

	void ScriptingEngine::UnloadGameAssembly()
	{
		if (!IsGameAssemblyLoaded())
			return;

		s_Data->EntityClasses.clear();
		s_Data->EntityInstances.clear();

		mono_domain_set(s_Data->RootDomain, false);
		mono_domain_unload(s_Data->GameAppDomain);

		s_Data->GameAssembly = nullptr;
		s_Data->GameAssemblyImage = nullptr;
		s_Data->GameAppDomain = nullptr;
	}

	bool ScriptingEngine::IsGameAssemblyLoaded()
	{
		return s_Data->GameAssembly != nullptr;
	}

	void ScriptingEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		s_Data->RootDomain = mono_jit_init("TeddyJITRuntime");
	}

	void ScriptingEngine::ShutdownMono()
	{
		UnloadGameAssembly();

		if (s_Data->CoreAppDomain)
		{
			mono_domain_set(s_Data->RootDomain, false);
			mono_domain_unload(s_Data->CoreAppDomain);
		}

		if (s_Data->RootDomain)
			mono_jit_cleanup(s_Data->RootDomain);

		s_Data->CoreAppDomain = nullptr;
		s_Data->RootDomain = nullptr;
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

			for (const auto& [name, fieldInstance] : sc.FieldInstances)
			{
				instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
			}

			instance->InvokeOnCreate();
		}
	}

	void ScriptingEngine::OnUpdateEntity(Entity entity, Timestep ts)
	{
		UUID entityUUID = entity.GetComponent<UUIDComponent>().id;
		if (s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end())
		{
			Ref<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
			instance->InvokeOnUpdate((float)ts);
		}
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
		return s_Data->EntityClasses;
	}

	void ScriptingEngine::LoadAssemblyClasses(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Teddy", "Entity");

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
				continue;

			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (isEntity)
				s_Data->EntityClasses[fullName] = CreateRef<ScriptClass>(nameSpace, name);
		}
	}

	MonoImage* ScriptingEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
	}

	MonoDomain* ScriptingEngine::GetCoreAppDomain()
	{
		return s_Data->CoreAppDomain;
	}

	MonoObject* ScriptingEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_Data->CoreAppDomain, monoClass);
		if (!instance)
			instance = mono_object_new(s_Data->GameAppDomain, monoClass);

		mono_runtime_object_init(instance);
		return instance;
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(s_Data->GameAssemblyImage, classNamespace.c_str(), className.c_str());
		if (!m_MonoClass)
			m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());

		if (!m_MonoClass)
		{
			TD_CORE_ERROR("Could not find class {}.{}", classNamespace, className);
			return;
		}

		MonoObject* dummyInstance = Instantiate();

		void* iter = nullptr;
		while (MonoClassField* field = mono_class_get_fields(m_MonoClass, &iter))
		{
			const char* fieldName = mono_field_get_name(field);
			uint32_t flags = mono_field_get_flags(field);
			if (flags & MONO_FIELD_ATTR_PUBLIC)
			{
				MonoType* monoType = mono_field_get_type(field);
				ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(monoType);
						
				ScriptField& scriptField = m_Fields[fieldName];
				scriptField.Type = fieldType;
				scriptField.Name = fieldName;
				scriptField.ClassField = field;

				mono_field_get_value(dummyInstance, field, scriptField.m_DefaultValueBuffer);
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

		UUID entityID = entity.GetComponent<UUIDComponent>().id;
		void* param = &entityID;
		m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
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
				TD_CORE_ERROR("C# Exception: {}", c_str);
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
				TD_CORE_ERROR("C# Exception: {}", c_str);
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