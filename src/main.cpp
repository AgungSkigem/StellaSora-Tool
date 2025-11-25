#include <pch.h>
#include "main.h"
#include <cstdio>
#include <appdata/il2cpp-init.h>
#include <appdata/helpers.h>

#include "CheatMenu.h"

#include "cheat/NoCD.h"
#include "cheat/HitEffect.h"
#include "cheat/InstantBreak.h"
#include "cheat/MobVacuum.h"
#include "cheat/StatModifier.h"

#include "cheat/misc/About.h"
#include "debug.h"
// di awal fungsi inisialisasi:

InitDebugConsole();
DbgMsg("[StellaSora] Main init started");

void OpenConsole()
{
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

	auto consoleWindow = GetConsoleWindow();
	SetForegroundWindow(consoleWindow);
	ShowWindow(consoleWindow, SW_RESTORE);
	ShowWindow(consoleWindow, SW_SHOW);
}

static void AdventureModuleController_Update_Hook(app::AdventureModuleController* __this, MethodInfo* method);

void Run(HMODULE* phModule) {
	global::process::hModule = *phModule;
	OpenConsole();
	LOG_INFO("Injected successfully!");

	while ((global::process::qwGameAssembly = (int64_t)GetModuleHandle("GameAssembly.dll")) == 0) {
		LOG_INFO("GameAssembly isn't initialized...\n");
		Sleep(2000);
	}

	LOG_DEBUG("GameAssembly found at: 0x%p", global::process::qwGameAssembly);

	constexpr int delayInit = 5000;
	constexpr int delaySecInit = delayInit / 1000;
	LOG_INFO("Waiting %d sec for game initialize.", delaySecInit);
	Sleep(delayInit);
	LOG_INFO("Initializing...");

	init_il2cpp();

	auto& menu = cheat::CheatMenu::GetInstance();

	#define ADD_FEATURE(feat) &cheat::feature::feat::GetInstance()
	menu.AddFeatures({
		ADD_FEATURE(InstantBreak),
		ADD_FEATURE(HitEffect),
		ADD_FEATURE(NoCD),
		ADD_FEATURE(MobVacuum),
		ADD_FEATURE(StatModifier),
		ADD_FEATURE(About)
	});
	#undef ADD_FEATURE
	
	menu.Init();

	HookManager::install(app::AdventureModuleController_Update, AdventureModuleController_Update_Hook);
}

static void AdventureModuleController_Update_Hook(app::AdventureModuleController* __this, MethodInfo* method)
{
	SAFE_BEGIN()
	for (auto func : global::AdventureModuleController_Update)
	{
		func();
	}
	SAFE_EEND()

	CALL_ORIGIN(AdventureModuleController_Update_Hook, __this, method);
}
