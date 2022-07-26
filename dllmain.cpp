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

        Vec3 entHead3D = hack->GetBonePos(curEnt, 8);
        Vec2 entPos2D, entHead2D;
        entHead3D.z += 8;
        if (hack->WorldToScreen(curEnt->m_vecOrigin, entPos2D)){
            DrawLine(entPos2D.x, entPos2D.y, windowWidth / 2, windowHeight, 2, color);
            
            if (hack->WorldToScreen(entHead3D, entHead2D)) {
                DrawEspBox2D(entPos2D, entHead2D, 2, color);

                int height = ABS(entPos2D.y - entHead2D.y);
                int dx = (entPos2D.x - entHead2D.x);

                float healthPerc = curEnt->m_iHealth / 100.f;
                float armorPerc = curEnt->m_ArmorValue / 100.f;

                Vec2 botHealth, topHealth, botArmor, topArmor;
                int healthHeight = height * healthPerc;
                int armorHeight = height * armorPerc;

                botHealth.y = botArmor.y = entPos2D.y;

                botHealth.x = entPos2D.x - (height / 4) - 2;
                botArmor.x = entPos2D.x + (height / 4) + 2;

                topHealth.y = entHead2D.y + height - healthHeight;
                topArmor.y = entHead2D.y + height - armorHeight;

                topHealth.x = entPos2D.x - (height / 4) - 2 - (dx * healthPerc);
                topArmor.x = entPos2D.x + (height / 4) + 2 - (dx * armorPerc);

                DrawLine(botHealth, topHealth, 4, D3DCOLOR_ARGB(255, 46, 139, 87));
                DrawLine(botArmor, topArmor, 4, D3DCOLOR_ARGB(255, 30, 144, 255));
            }
        }
    }

    

    Vec2 l, r, t, b;
    l = r = t = b = hack->crosshair2D;
    l.x -= hack->corsshairSize;
    r.x += hack->corsshairSize;
    b.y += hack->corsshairSize;
    t.y -= hack->corsshairSize;

    DrawLine(l, r, 2, D3DCOLOR_ARGB(255, 255, 255, 255));
    DrawLine(t, b, 2, D3DCOLOR_ARGB(255, 255, 255, 255));

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

        Vec3 pAng = hack->localEnt->m_aimPunchAngle;

        hack->crosshair2D.x = windowWidth / 2 - (windowWidth / 90 * pAng.y);
        hack->crosshair2D.y = windowHeight / 2 + (windowHeight / 90 * pAng.x);
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

