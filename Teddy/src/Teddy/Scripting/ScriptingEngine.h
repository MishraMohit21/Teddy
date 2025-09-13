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

		MonoClass* GetMonoClass() const { return m_MonoClass; }

		const std::unordered_map<std::string, ScriptField>& GetFields() const { return m_Fields; }

		const std::string& GetClassName() const { return m_ClassName; }
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

		static void LoadAssemblies(const std::filesystem::path& corePath, const std::filesystem::path& gamePath);
		static void ReloadGameAssembly();
		static void UnloadGameAssembly();
		static bool IsGameAssemblyLoaded();

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static bool EntityClassExists(const std::string& fullClassName);
		static void OnCreateEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, Timestep ts);

		static Scene* GetSceneContext();
		static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityID);
		static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();

		static MonoImage* GetCoreAssemblyImage();
		static MonoDomain* GetCoreAppDomain();
	private:
		static void InitMono();
		static void ShutdownMono();

		static MonoAssembly* LoadAssembly(const std::filesystem::path& filepath);
		static void LoadAssemblyClasses(MonoAssembly* assembly);

		static MonoObject* InstantiateClass(MonoClass* monoClass);

		friend class ScriptClass;
		friend class ScriptGlue;
	};


}

namespace Utils {
	inline const char* ScriptFieldTypeToString(Teddy::ScriptFieldType type)
	{
		switch (type)
		{
		case Teddy::ScriptFieldType::None:    return "None";
		case Teddy::ScriptFieldType::Float:   return "Float";
		case Teddy::ScriptFieldType::Double:  return "Double";
		case Teddy::ScriptFieldType::Bool:    return "Bool";
		case Teddy::ScriptFieldType::Char:    return "Char";
		case Teddy::ScriptFieldType::Byte:    return "Byte";
		case Teddy::ScriptFieldType::Short:   return "Short";
		case Teddy::ScriptFieldType::Int:     return "Int";
		case Teddy::ScriptFieldType::Long:    return "Long";
		case Teddy::ScriptFieldType::UByte:   return "UByte";
		case Teddy::ScriptFieldType::UShort:  return "UShort";
		case Teddy::ScriptFieldType::UInt:    return "UInt";
		case Teddy::ScriptFieldType::ULong:   return "ULong";
		case Teddy::ScriptFieldType::Vector2: return "Vector2";
		case Teddy::ScriptFieldType::Vector3: return "Vector3";
		case Teddy::ScriptFieldType::Vector4: return "Vector4";
		case Teddy::ScriptFieldType::Entity:  return "Entity";
		}
		return "Unknown";
	}

	inline Teddy::ScriptFieldType ScriptFieldTypeFromString(std::string_view str)
	{
		if (str == "None")    return Teddy::ScriptFieldType::None;
		if (str == "Float")   return Teddy::ScriptFieldType::Float;
		if (str == "Double")  return Teddy::ScriptFieldType::Double;
		if (str == "Bool")    return Teddy::ScriptFieldType::Bool;
		if (str == "Char")    return Teddy::ScriptFieldType::Char;
		if (str == "Byte")    return Teddy::ScriptFieldType::Byte;
		if (str == "Short")   return Teddy::ScriptFieldType::Short;
		if (str == "Int")     return Teddy::ScriptFieldType::Int;
		if (str == "Long")    return Teddy::ScriptFieldType::Long;
		if (str == "UByte")   return Teddy::ScriptFieldType::UByte;
		if (str == "UShort")  return Teddy::ScriptFieldType::UShort;
		if (str == "UInt")    return Teddy::ScriptFieldType::UInt;
		if (str == "ULong")   return Teddy::ScriptFieldType::ULong;
		if (str == "Vector2") return Teddy::ScriptFieldType::Vector2;
		if (str == "Vector3") return Teddy::ScriptFieldType::Vector3;
		if (str == "Vector4") return Teddy::ScriptFieldType::Vector4;
		if (str == "Entity")  return Teddy::ScriptFieldType::Entity;
		return Teddy::ScriptFieldType::None;
	}
}
