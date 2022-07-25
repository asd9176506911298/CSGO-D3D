// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "includes.h"

void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
extern LPDIRECT3DDEVICE9 pDevice = nullptr;
Hack* hack;

void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice)
{
    if (!pDevice)
        pDevice = o_pDevice;

    for (int i = 1; i < 32; i++)
    {
        Ent* curEnt = hack->entList->ents[i].ent;
        if (!hack->CheckValidEnt(curEnt))
            continue;

        D3DCOLOR color;
        if (curEnt->m_iTeamNum == hack->localEnt->m_iTeamNum)
            color = D3DCOLOR_ARGB(255, 0, 255, 0);
        else
            color = D3DCOLOR_ARGB(255, 255, 0, 0);

        Vec2 entPos2D;

        if (hack->WorldToScreen(curEnt->m_vecOrigin, entPos2D))
            DrawLine(entPos2D.x, entPos2D.y, windowWidth / 2, windowHeight, 2, color);
    }

    DrawFilledRect(windowWidth / 2 -2 , windowHeight / 2 - 2, 4, 4, D3DCOLOR_ARGB(255, 255, 255, 255));

    oEndScene(pDevice);
}

DWORD WINAPI HackThread(HMODULE hModule)
{

    if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
        memcpy(EndSceneBytes, (char*)d3d9Device[42], 7);

        oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);
    }

    hack = new Hack();
    hack->Init();

    while (!GetAsyncKeyState(VK_END))
    {
        hack->Update();
    }
    
    Patch((BYTE*)d3d9Device[42], EndSceneBytes, 7);

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0);
        if (hThread) CloseHandle(hThread);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

