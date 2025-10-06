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

	static std::unique_ptr<ScriptEngineData> s_ScriptingData;

	void ScriptingEngine::Init()
	{
		s_ScriptingData = std::make_unique<ScriptEngineData>();
		InitMono();
	}

	void ScriptingEngine::Shutdown()
	{
		ShutdownMono();
		s_ScriptingData.reset();
	}

	void ScriptingEngine::LoadAssemblies(const std::filesystem::path& corePath, const std::filesystem::path& gamePath)
	{
		TD_CORE_INFO("Game DLL: {}, ScriptCore: {}", gamePath.string(), corePath.string());
		char* runtimeName = (char*)"TeddyScriptRuntime";
		s_ScriptingData->CoreAppDomain = mono_domain_create_appdomain(runtimeName, nullptr);
		mono_domain_set(s_ScriptingData->CoreAppDomain, true);

		s_ScriptingData->CoreAssembly = Utils::LoadMonoAssembly(corePath, true);
		s_ScriptingData->CoreAssemblyImage = mono_assembly_get_image(s_ScriptingData->CoreAssembly);

		s_ScriptingData->GameAssemblyPath = gamePath;
		if (std::filesystem::exists(s_ScriptingData->GameAssemblyPath))
		{
			s_ScriptingData->GameAssembly = Utils::LoadMonoAssembly(gamePath, true);
			s_ScriptingData->GameAssemblyImage = mono_assembly_get_image(s_ScriptingData->GameAssembly);
			LoadAssemblyClasses(s_ScriptingData->GameAssembly);
		}

		LoadAssemblyClasses(s_ScriptingData->CoreAssembly);

		ScriptGlue::RegisterComponents();
		ScriptGlue::RegisterFunctions();

		s_ScriptingData->EntityClass = ScriptClass("Teddy", "Entity");
	}

	void ScriptingEngine::ReloadGameAssembly()
	{
		if (!IsGameAssemblyLoaded())
			return;

		UnloadGameAssembly();

		// Create a fresh Game domain for the new assembly
		s_ScriptingData->GameAppDomain = mono_domain_create_appdomain((char*)"TeddyGameRuntime", nullptr);
		mono_domain_set(s_ScriptingData->GameAppDomain, true);

		// Load the Game assembly inside the new domain
		s_ScriptingData->GameAssembly = Utils::LoadMonoAssembly(s_ScriptingData->GameAssemblyPath, true);
		s_ScriptingData->GameAssemblyImage = mono_assembly_get_image(s_ScriptingData->GameAssembly);

		// ?? Re-resolve EntityClass in the new domain before scanning for subclasses
		s_ScriptingData->EntityClass = ScriptClass("Teddy", "Entity");

		// Load classes while GameAppDomain is active
		LoadAssemblyClasses(s_ScriptingData->GameAssembly);

		// Re-register glue inside the Game domain if needed
		ScriptGlue::RegisterComponents();
		ScriptGlue::RegisterFunctions();

		// Switch back to Core domain for editor/runtime control
		if (s_ScriptingData->CoreAppDomain)
			mono_domain_set(s_ScriptingData->CoreAppDomain, true);

		TD_CORE_TRACE("Reloaded Game Assembly into new GameAppDomain");
	}




	void ScriptingEngine::UnloadGameAssembly()
	{
		if (!IsGameAssemblyLoaded())
			return;

		s_ScriptingData->EntityClasses.clear();
		s_ScriptingData->EntityInstances.clear();

		// Ensure we are not inside the GameAppDomain when unloading it
		if (s_ScriptingData->RootDomain)
			mono_domain_set(s_ScriptingData->RootDomain, false);

		if (s_ScriptingData->GameAppDomain)
		{
			mono_domain_unload(s_ScriptingData->GameAppDomain);
		}

		s_ScriptingData->GameAssembly = nullptr;
		s_ScriptingData->GameAssemblyImage = nullptr;
		s_ScriptingData->GameAppDomain = nullptr;
	}

	bool ScriptingEngine::IsGameAssemblyLoaded()
	{
		return s_ScriptingData->GameAssembly != nullptr;
	}

	void ScriptingEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		s_ScriptingData->RootDomain = mono_jit_init("TeddyJITRuntime");
	}

	void ScriptingEngine::ShutdownMono()
	{
		UnloadGameAssembly();

		if (s_ScriptingData->CoreAppDomain)
		{
			mono_domain_set(s_ScriptingData->RootDomain, false);
			mono_domain_unload(s_ScriptingData->CoreAppDomain);
		}

		if (s_ScriptingData->RootDomain)
			mono_jit_cleanup(s_ScriptingData->RootDomain);

		s_ScriptingData->CoreAppDomain = nullptr;
		s_ScriptingData->RootDomain = nullptr;
	}

	void ScriptingEngine::OnRuntimeStart(Scene* scene)
	{
		s_ScriptingData->SceneContext = scene;
	}

	bool ScriptingEngine::EntityClassExists(const std::string& fullClassName)
	{
		return s_ScriptingData->EntityClasses.find(fullClassName) != s_ScriptingData->EntityClasses.end();
	}

	void ScriptingEngine::OnCreateEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();
		if (ScriptingEngine::EntityClassExists(sc.ClassName))
		{
			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_ScriptingData->EntityClasses[sc.ClassName], entity);
			s_ScriptingData->EntityInstances[entity.GetComponent<UUIDComponent>().id] = instance;

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
		if (s_ScriptingData->EntityInstances.find(entityUUID) != s_ScriptingData->EntityInstances.end())
		{
			Ref<ScriptInstance> instance = s_ScriptingData->EntityInstances[entityUUID];
			instance->InvokeOnUpdate((float)ts);
		}
	}

	Scene* ScriptingEngine::GetSceneContext()
	{
		return s_ScriptingData->SceneContext;
	}

	void ScriptingEngine::OnRuntimeStop()
	{
		s_ScriptingData->SceneContext = nullptr;
		s_ScriptingData->EntityInstances.clear();
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptingEngine::GetEntityClasses()
	{
		return s_ScriptingData->EntityClasses;
	}

	void ScriptingEngine::LoadAssemblyClasses(MonoAssembly* assembly)
	{
		if (!assembly)
		{
			TD_CORE_ERROR("LoadAssemblyClasses called with null assembly");
			return;
		}

		MonoImage* image = mono_assembly_get_image(assembly);
		const char* assemblyName = mono_image_get_name(image);
		TD_CORE_INFO("Loading classes from assembly: {}", assemblyName);

		// Set domain to the domain that owns this assembly
		if (s_ScriptingData->GameAssembly && assembly == s_ScriptingData->GameAssembly)
		{
			// ensure we are in GameAppDomain while inspecting game assembly
			if (s_ScriptingData->GameAppDomain)
				mono_domain_set(s_ScriptingData->GameAppDomain, true);
		}
		else
		{
			// Core assembly (or any other) should use CoreAppDomain
			if (s_ScriptingData->CoreAppDomain)
				mono_domain_set(s_ScriptingData->CoreAppDomain, true);
		}

		// Re-fetch Teddy.Entity inside the currently active domain (important)
		MonoClass* entityClass = mono_class_from_name(s_ScriptingData->CoreAssemblyImage, "Teddy", "Entity");
		if (entityClass == nullptr)
		{
			TD_CORE_ERROR("Could not find Teddy.Entity class in ScriptCore assembly!");
			return;
		}

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		TD_CORE_INFO("Found {} types.", numTypes);

		int foundEntityClasses = 0;
		s_ScriptingData->EntityClasses.clear(); // rebuild registry

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			std::string fullName;
			if (nameSpace && strlen(nameSpace) != 0)
				fullName = std::format("{}.{}", nameSpace, name);
			else
				fullName = name;

			MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);
			if (monoClass == nullptr)
				continue;

			// ignore the base Entity itself
			if (monoClass == entityClass)
				continue;

			TD_CORE_INFO("  Checking inheritance for class: {}", fullName);
			bool isEntity = false;
            MonoClass* parentClass = mono_class_get_parent(monoClass);
            while (parentClass != nullptr)
            {
                const char* parentName = mono_class_get_name(parentClass);
                const char* parentNamespace = mono_class_get_namespace(parentClass);
                if (strcmp(parentName, "Entity") == 0 && strcmp(parentNamespace, "Teddy") == 0)
                {
                    isEntity = true;
                    break;
                }
                parentClass = mono_class_get_parent(parentClass);
            }

			if (isEntity)
			{
				TD_CORE_INFO("  Found Entity Class: {}", fullName);
				// create a ScriptClass pointing to the class *in the correct domain*.
				// Note: ScriptClass constructor will try the Game image first (if game loaded).
				s_ScriptingData->EntityClasses[fullName] = CreateRef<ScriptClass>(std::string(nameSpace ? nameSpace : ""), std::string(name ? name : ""));
				foundEntityClasses++;
			}
		}

		TD_CORE_INFO("Found and registered {} entity classes from {}.", foundEntityClasses, assemblyName);
	}


	MonoImage* ScriptingEngine::GetCoreAssemblyImage()
	{
		return s_ScriptingData->CoreAssemblyImage;
	}

	MonoDomain* ScriptingEngine::GetCoreAppDomain()
	{
		return s_ScriptingData->CoreAppDomain;
	}

	MonoObject* ScriptingEngine::InstantiateClass(MonoClass* monoClass)
	{
		if (!monoClass)
			return nullptr;

		// Determine which image the class belongs to and set the domain accordingly
		MonoImage* classImage = mono_class_get_image(monoClass);

		if (s_ScriptingData->GameAssemblyImage && classImage == s_ScriptingData->GameAssemblyImage)
		{
			if (s_ScriptingData->GameAppDomain)
				mono_domain_set(s_ScriptingData->GameAppDomain, true);
		}
		else
		{
			if (s_ScriptingData->CoreAppDomain)
				mono_domain_set(s_ScriptingData->CoreAppDomain, true);
		}

		MonoObject* instance = mono_object_new(monoClass ? mono_domain_get() : nullptr, monoClass);
		if (!instance)
		{
			// fallback: try creating in core domain explicitly
			if (s_ScriptingData->CoreAppDomain)
				mono_domain_set(s_ScriptingData->CoreAppDomain, true);
			instance = mono_object_new(s_ScriptingData->CoreAppDomain, monoClass);
		}

		mono_runtime_object_init(instance);
		return instance;
	}


	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		// Prefer Game assembly class if game assembly is loaded and its domain is active
		if (ScriptingEngine::IsGameAssemblyLoaded() && s_ScriptingData->GameAssemblyImage)
		{
			// ensure we are in game domain when trying to lookup game types
			if (s_ScriptingData->GameAppDomain)
				mono_domain_set(s_ScriptingData->GameAppDomain, true);

			m_MonoClass = mono_class_from_name(s_ScriptingData->GameAssemblyImage, classNamespace.c_str(), className.c_str());
		}

		// If not found in game assembly, try core assembly (switch to core domain)
		if (!m_MonoClass)
		{
			if (s_ScriptingData->CoreAppDomain)
				mono_domain_set(s_ScriptingData->CoreAppDomain, true);

			m_MonoClass = mono_class_from_name(s_ScriptingData->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
		}

		if (!m_MonoClass)
		{
			TD_CORE_ERROR("Could not find class {}.{}", classNamespace, className);
			return;
		}

		// Create a temporary instance to read default field values (instantiate in the domain owning the class)
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

				// read default value into buffer
				if (dummyInstance)
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

		MonoClass* parentClass = mono_class_get_parent(scriptClass->GetMonoClass());
		m_Constructor = mono_class_get_method_from_name(parentClass, ".ctor", 1);
		if (m_Constructor == nullptr)
			TD_CORE_ERROR("Failed to find Entity base constructor for script {}", scriptClass->GetClassName());
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
		auto it = s_ScriptingData->EntityInstances.find(entityID);
		if (it == s_ScriptingData->EntityInstances.end())
			return nullptr;

		return it->second;
	}

}