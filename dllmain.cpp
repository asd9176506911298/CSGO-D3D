// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "includes.h"
#include <sstream>
#include <string.h>

void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
extern LPDIRECT3DDEVICE9 pDevice = nullptr;
Hack* hack;

void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice)
{
    if (!pDevice)
        pDevice = o_pDevice;

    DrawText("Yuki.kaco", windowWidth / 2, windowHeight - 20, D3DCOLOR_ARGB(255, 255, 255, 255));

    int menuOffX = windowWidth / 2;
    int menuOffY = 50;
    D3DCOLOR enabled = D3DCOLOR_ARGB(255, 0, 255, 0);
    D3DCOLOR disabled = D3DCOLOR_ARGB(255, 255, 0, 0);

    if (!hack->settings.showMenu)
    {
        DrawText("Show Menu (INS)", menuOffX, menuOffY, D3DCOLOR_ARGB(255, 255, 255, 255));
    }
    else
    {
        DrawText("Show Teamates (F1)", menuOffX, menuOffY + 0 * 12, hack->settings.showTeamates ? enabled : disabled);
        DrawText("Snaplines (F2)", menuOffX, menuOffY + 1 * 12, hack->settings.snaplines ? enabled : disabled);
        DrawText("2D Box (F3)", menuOffX, menuOffY + 2 * 12, hack->settings.box2D ? enabled : disabled);
        DrawText("2D Statusbars (F4)", menuOffX, menuOffY + 3 * 12, hack->settings.status2D? enabled : disabled);
        DrawText("Status Text (F5)", menuOffX, menuOffY + 4 * 12, hack->settings.statusText ? enabled : disabled);
        DrawText("3D Box (F6)", menuOffX, menuOffY + 5 * 12, hack->settings.box3D ? enabled : disabled);
        DrawText("Velocity Esp(F7)", menuOffX, menuOffY + 6 * 12, hack->settings.velEsp ? enabled : disabled);
        DrawText("Headline Esp (F8)", menuOffX, menuOffY + 7 * 12, hack->settings.headlineEsp ? enabled : disabled);
        DrawText("Recoil crosshair (F9)", menuOffX, menuOffY + 8 * 12, hack->settings.rcsCrosshair ? enabled : disabled);
        DrawText("Hide Menu (INS)", menuOffX, menuOffY + 9 * 12, D3DCOLOR_ARGB(255, 255, 255, 255));
    }

    for (int i = 1; i < 32; i++)
    {
        Ent* curEnt = hack->entList->ents[i].ent;
        if (!hack->CheckValidEnt(curEnt))
            continue;

        D3DCOLOR espColor, snaplineColor, velocityColor, headlineColor;
        if (curEnt->m_iTeamNum == hack->localEnt->m_iTeamNum)
        {
            espColor = hack->color.team.esp;
            snaplineColor = hack->color.team.snapline;
            velocityColor = hack->color.team.velocity;
            headlineColor = hack->color.team.headline;
        }
        else
        {
            espColor = hack->color.enemy.esp;
            snaplineColor = hack->color.enemy.snapline;
            velocityColor = hack->color.enemy.velocity;
            headlineColor = hack->color.enemy.headline;
        }

        if (!hack->settings.showTeamates && (curEnt->m_iTeamNum == hack->localEnt->m_iTeamNum))
            continue;

        Vec3 entHead3D = hack->GetBonePos(curEnt, 8);
        Vec2 entPos2D, entHead2D;
        entHead3D.z += 8;
        if (hack->WorldToScreen(curEnt->m_vecOrigin, entPos2D)){
            if (hack->settings.velEsp)
            {
                Vec3 velOff = curEnt->m_vecOrigin + (curEnt->m_vecVelocity * .25);
                Vec2 velOff2D;

                if (hack->WorldToScreen(velOff, velOff2D))
                {
                    DrawLine(entPos2D, velOff2D, 2, velocityColor);
                    DrawFilledRect(velOff2D.x - 2, velOff2D.y - 2, 4, 4, espColor);
                }
            }
            if (hack->settings.snaplines) 
            {
                DrawLine(entPos2D.x, entPos2D.y, windowWidth / 2, windowHeight, 2, snaplineColor);
            }
            
            
            if (hack->WorldToScreen(entHead3D, entHead2D)) {
                if (hack->settings.box2D)
                {
                    DrawEspBox2D(entPos2D, entHead2D, 2, espColor);
                }

                if (hack->settings.box3D)
                {
                    DrawEspBox3D(entHead3D, curEnt->m_vecOrigin, curEnt->m_angEyeAnglesY, 25, 2, espColor);
                }
                
                if (hack->settings.status2D)
                {
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

                    DrawLine(botHealth, topHealth, 4, hack->color.health);
                    DrawLine(botArmor, topArmor, 4, hack->color.armor);
                }              

                if (hack->settings.headlineEsp)
                {
                    Vec3 head3D = hack->GetBonePos(curEnt, 8);
                    Vec3 entAngles;
                    entAngles.x = curEnt->m_angEyeAnglesX;
                    entAngles.y = curEnt->m_angEyeAnglesY;
                    entAngles.z = 0;
                    Vec3 endPoint = hack->TransformVec(head3D, entAngles, 60);
                    Vec2 endPoint2D, head2D;
                    hack->WorldToScreen(head3D, head2D);
                    if (hack->WorldToScreen(endPoint, endPoint2D))
                    {
                        DrawLine(head2D, endPoint2D, 2, headlineColor);
                    }
                }

                if (hack->settings.statusText)
                {
                    std::stringstream s1, s2;
                    s1 << curEnt->m_iHealth;
                    s2 << curEnt->m_ArmorValue;
                    std::string t1 = "hp: " + s1.str();
                    std::string t2 = "ap: " + s2.str();
                    char* healthMsg = (char*)t1.c_str();
                    char* armorMsg = (char*)t2.c_str();

                    DrawText(healthMsg, entPos2D.x, entPos2D.y, D3DCOLOR_ARGB(255, 255, 255, 255));
                    DrawText(armorMsg, entPos2D.x, entPos2D.y + 12, D3DCOLOR_ARGB(255, 255, 255, 255));

                    if(!curEnt->m_bHasHelmet)
                        DrawText("ez hs", entPos2D.x, entPos2D.y + 24, D3DCOLOR_ARGB(255, 255, 255, 255));
                }
            }
        }
    }

    
    if (hack->settings.rcsCrosshair)
    {
        Vec2 l, r, t, b;
        l = r = t = b = hack->crosshair2D;
        l.x -= hack->corsshairSize;
        r.x += hack->corsshairSize;
        b.y += hack->corsshairSize;
        t.y -= hack->corsshairSize;

        DrawLine(l, r, 2, hack->color.crosshair);
        DrawLine(t, b, 2, hack->color.crosshair);
    }
    

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

