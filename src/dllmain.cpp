// dllmain.cpp : Defines the entry point for the DLL application.
#include <windows.h>
#include <stdio.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        printf("[StellaSora] DLL attached\n");
        OutputDebugStringA("[StellaSora] DLL attached\n");
        // Optional: write a quick file
        FILE *f = fopen("C:\\Temp\\stellalog.txt", "a");
        if (f) {
            fprintf(f, "[%lld] attached\n", (long long)time(NULL));
            fclose(f);
        }
        break;
    case DLL_PROCESS_DETACH:
        printf("[StellaSora] DLL detached\n");
        break;
    }
    return TRUE;
}
