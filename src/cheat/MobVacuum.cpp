#include <pch.h>
#include <appdata/helpers.h>
#include "MobVacuum.h"
#include "main.h"
#include "debug.h"

namespace cheat::feature
{
	static void OnUpdate();

	MobVacuum& MobVacuum::GetInstance()
	{
		DbgMsg("[Feature] MobVacuum: executed");
		static MobVacuum instance;
		return instance;
	}

	MobVacuum::MobVacuum() : Feature(),
		f_MobVacuum(false),
		f_AggroOnly(false),
		f_Distance(3.f)
	{
		global::AdventureModuleController_Update.push_back(OnUpdate);
	}

	const FeatureGUIInfo& MobVacuum::GetGUIInfo() const
	{
		static FeatureGUIInfo info = { "Mob Vacuum", "World", true };
		return info;
	}

	void MobVacuum::DrawMain()
	{
		ImGui::Checkbox("Mob Vacuum", &f_MobVacuum);
		ImGui::Checkbox("Aggro Only", &f_AggroOnly);
		ImGui::DragFloat("Distance", &f_Distance, 0.1f, 1.f, 10.f, nullptr, ImGuiSliderFlags_AlwaysClamp);
	}

	static app::TSVector2 GetForwardPos(app::LogicEntity* actor, float scalar)
	{
		auto playerPos = app::TrueSyncTransform_get_Position(actor->fields._tsTransform_k__BackingField, nullptr);
		auto playerForward = app::TrueSyncTransform_get_Forward(actor->fields._tsTransform_k__BackingField, nullptr);

		auto convertedScale = (app::FP)(scalar * (1LL << 32));
		auto forwardScale = app::TSVector2_op_Multiply_1(playerForward, convertedScale, nullptr);
		auto forwardPos = app::TSVector2_op_Addition(playerPos, forwardScale, nullptr);
		return forwardPos;
	}

	static void SetActorPosition(app::LogicEntity* entity, app::TSVector2 pos)
	{
		auto actor = CastTo<app::AdventureActor>(entity, *app::AdventureActor__TypeInfo);
		if (!actor->fields.actorHealthInfo->fields._isAlive)
			return;

		app::TrueSyncTransform_set_Position(entity->fields._tsTransform_k__BackingField, pos, nullptr);
		auto newY = app::CommonHelper_GetTerrainHeight_1(pos, entity->fields._tsTransform_k__BackingField->fields._positionY, nullptr);
		app::TrueSyncTransform_set_PositionY(entity->fields._tsTransform_k__BackingField, newY, nullptr);
		app::AdventureActor_SyncMovementLocation_1(actor, true, nullptr);
	}

	static void OnUpdate()
	{
		DbgMsg("[Feature] MobVacuum: executed");
		MobVacuum& instance = MobVacuum::GetInstance();
		if (!instance.f_MobVacuum)
			return;

		auto adventureModuleController = GET_SINGLETON(AdventureModuleController);
		if (adventureModuleController == nullptr)
			return;

		auto adventurePlayerController = GET_SINGLETON(AdventurePlayerController);
		if (adventurePlayerController == nullptr)
			return;

		auto monsterActors = TO_UNI_LIST(app::AdventureModuleController_get_monsterActors(adventureModuleController, nullptr), app::LogicEntity*);
		if (monsterActors == nullptr)
			return;

		auto playerActor = adventurePlayerController->fields._activedPlayerActor;
		if (playerActor == nullptr)
			return;

		auto player = (app::LogicEntity*)playerActor;
		auto targetPos = GetForwardPos(player, instance.f_Distance);
		for (auto monster : *monsterActors)
		{
			auto monsterActor = CastTo<app::AdventureActor>(monster, *app::AdventureActor__TypeInfo);
			if (monsterActor == nullptr)
				continue;

			if (instance.f_AggroOnly && monsterActor->fields._monsterTarget != player)
				continue;

			SetActorPosition(monster, targetPos);
		}
	}
}
