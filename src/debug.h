#pragma once
#include <windows.h>
#include <stdio.h>
#include <time.h>

inline void InitDebugConsole()
{
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONIN$", "r", stdin);
    SetConsoleTitleA("StellaSora Debug Console");
    printf("[StellaSora] Debug console initialized\n");
    OutputDebugStringA("[StellaSora] Debug console initialized\n");
}

inline void DbgMsg(const char* s)
{
    OutputDebugStringA(s);
    printf("%s\n", s);
}
