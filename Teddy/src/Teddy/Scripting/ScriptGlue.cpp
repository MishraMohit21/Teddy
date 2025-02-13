#include "tdpch.h"
#include "ScriptGlue.h"
#include "ScriptingEngine.h"


#include "Teddy/Core/UUID.h"
#include "Teddy/Core/KeyCodes.h"
#include "Teddy/Core/Input.h"

#include "Teddy/Scene/Scene.h"
#include "Teddy/Scene/Entity.h"
#include "Teddy/Scene/Component.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "box2d/b2_body.h"

namespace Teddy {

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define TD_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Teddy.InternalCalls::" #Name, Name)

	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		TD_CORE_WARN("Value: {0}", *parameter);
		*outResult = glm::normalize(*parameter);
	}

	static float NativeLog_VectorDot(glm::vec3* parameter)
	{
		TD_CORE_WARN("Value: {0}", *parameter);
		return glm::dot(*parameter, *parameter);
	}

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		//(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		//(entity);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		//(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		//(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		//(entity);

		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		//(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		//(entity);

		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		//(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		//(entity);

		auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();
		b2Body* body = (b2Body*)rb2d.RunTimeBody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		//(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		//(entity);

		auto& rb2d = entity.GetComponent<Rigid2DBodyComponent>();
		b2Body* body = (b2Body*)rb2d.RunTimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
			{
				std::string_view typeName = typeid(Component).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypename = fmt::format("Teddy.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
				if (!managedType)
				{
					TD_CORE_ERROR("Could not find component type {}", managedTypename);
					return;
				}
				s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
			}(), ...);
	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		TD_ADD_INTERNAL_CALL(NativeLog);
		TD_ADD_INTERNAL_CALL(NativeLog_Vector);
		TD_ADD_INTERNAL_CALL(NativeLog_VectorDot);

		TD_ADD_INTERNAL_CALL(Entity_HasComponent);
		TD_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		TD_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		TD_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		TD_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

		TD_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}
	;
}
