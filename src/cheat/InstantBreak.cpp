#include <pch.h>
#include <intsafe.h>
#include "InstantBreak.h"
#include "debug.h"

namespace cheat::feature
{
	static bool MonsterToughnessInfo_DamageToToughness_Hook(app::MonsterToughnessInfo* __this, app::AdventureActor* from, int32_t damage, app::IList_1_System_Int32_* hitDamageTag, MethodInfo* method);

	InstantBreak& InstantBreak::GetInstance() {
		DbgMsg("[Feature] InstantBreak: executed1");
		static InstantBreak instance;
		return instance;
	}

	InstantBreak::InstantBreak() : Feature(),
		f_InstantBreak(false)
	{
		HookManager::install(app::MonsterToughnessInfo_DamageToToughness, MonsterToughnessInfo_DamageToToughness_Hook);
	}

	const FeatureGUIInfo& InstantBreak::GetGUIInfo() const {
		const static FeatureGUIInfo info { "", "Player", false };
		return info;
	}

	void InstantBreak::DrawMain() {
		ImGui::Checkbox("Instant Break", &f_InstantBreak);
	}

	static bool MonsterToughnessInfo_DamageToToughness_Hook(app::MonsterToughnessInfo* __this, app::AdventureActor* from, int32_t damage, app::IList_1_System_Int32_* hitDamageTag, MethodInfo* method)
	{
		InstantBreak& instance = InstantBreak::GetInstance();
		if (instance.f_InstantBreak)
		{
			DbgMsg("[Feature] InstantBreak: executed4");
			auto toughness = app::SpecialAttributeList_GetAttributeValue(__this->fields.specialAttributeList, app::GameEnum_specialAttributeType__Enum::TOUGHNESS_V, nullptr);
			auto fp = app::iFP_get_Value(&toughness, nullptr);
			auto realValue = HIDWORD(fp._serializedValue);
			if (realValue)
				damage = realValue;
		}

		return CALL_ORIGIN(MonsterToughnessInfo_DamageToToughness_Hook, __this, from, damage, hitDamageTag, method);
	}
}
