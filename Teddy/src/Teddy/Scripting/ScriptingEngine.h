#pragma once

#include "Teddy/Scene/Scene.h"
#include "Teddy/Scene/Entity.h"
#include "Teddy/Scene/Component.h"

#include <filesystem>
#include <string>


extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoClassField MonoClassField;
}

namespace Teddy {

	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};

	struct ScriptField
	{
		ScriptFieldType Type;
		std::string Name;
		MonoClassField* ClassField;

		// A buffer to store the default value of this field
		uint8_t m_DefaultValueBuffer[16] = { 0 };

		template<typename T>
		T GetDefaultValue() const
		{
			static_assert(sizeof(T) <= 16, "Type too large!");
			return *(T*)m_DefaultValueBuffer;
		}
	};

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr, MonoObject** exc = nullptr);

		const std::unordered_map<std::string, ScriptField>& GetFields() const { return m_Fields; }
	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;

		MonoClass* m_MonoClass = nullptr;

		std::unordered_map<std::string, ScriptField> m_Fields;

		friend class ScriptingEngine;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);
		~ScriptInstance();

		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);

		Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			static_assert(sizeof(T) <= 16, "Type too large");
			T buffer;
			GetFieldValueInternal(name, &buffer);
			return buffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, const T& value)
		{
			SetFieldValueInternal(name, &value);
		}
	
		void GetFieldValueInternal(const std::string& name, void* buffer);
		void SetFieldValueInternal(const std::string& name, const void* value);

	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		uint32_t m_GCHandle = 0;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;
	};

	class ScriptingEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::filesystem::path& filepath);

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static bool EntityClassExists(const std::string& fullClassName);
		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, Timestep ts);

		static Scene* GetSceneContext();
		static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityID);
		static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();

		static void PrintAssemblyTypes();

		static MonoImage* GetCoreAssemblyImage();
		static MonoDomain* GetAppDomain();
	private:
		static void InitMono();
		static void ShutdownMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		static void LoadAssemblyClasses(MonoAssembly* assembly);

		friend class ScriptClass;
		friend class ScriptGlue;
	};


}