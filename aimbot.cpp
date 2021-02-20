#include "ESP.h"
#include "assault_cube.h"
#include "aimbot.h"
#include <iostream>


#define OFFSET_ISVISIBLE 0x48ABD0
#define M_PI 3.141592653589793F
int getNumberOfPlayers() {
    return *((int*)((UINT)GetModuleHandleW(0) + 0xE4E10));
}

void DoAimBot(Player* pLocalPlayer, Vec3 vAimPos)
{
    float flYaw = 0;
    float flPitch = 0;

    GetAimAngles(pLocalPlayer->m_HeadPos, vAimPos, &flYaw, &flPitch);

    pLocalPlayer->m_Pitch = flPitch;
    pLocalPlayer->m_Yaw = flYaw;
}

void GetAimAngles(Vec3 vLocalPosition, Vec3 vTargetPosition, float* pflYaw, float* pflPitch)
{
    Vec3 vDelta = (vTargetPosition - vLocalPosition);
    vDelta.Normalize();

    *pflYaw = -atan2(vDelta.x, vDelta.y) / M_PI * 180 + 180;
    *pflPitch = atan2(vDelta.z, sqrt(vDelta.x * vDelta.x + vDelta.y * vDelta.y)) / M_PI * 180;
}

bool IsVisible(Vec3 vFrom, Vec3 vTo)
{
    __asm
    {
        push vTo.z;
        push vTo.y;
        push vTo.x;

        push vFrom.z;
        push vFrom.y;
        push vFrom.x;

        xor cl, cl;   //Tracer
        xor eax, eax;
        mov ebx, OFFSET_ISVISIBLE;
        call ebx;
        add esp, 0x18;
    }
}
bool IsValidPtr(void* pPointer)
{
    if (pPointer && HIWORD(pPointer))
    {
        if (!IsBadReadPtr(pPointer, sizeof(DWORD_PTR)))
            return true;
    }
    return false;
}

Player* GetAimbotTarget(Game* pGame, Player* pLocalPlayer)
{
    
    Player* pAimTarget = nullptr;
    float flClosestDist = INFINITY;

    for (int i = 0; i < pGame->m_PlayerCount; i++)
    {
        Player* pPlayer = pGame->GetPlayer(i);
        if (!IsValidPtr(pPlayer))
            continue;

        if (pPlayer == pLocalPlayer)
            continue;

        /* (pPlayer->m_State != CS_ALIVE)
            continue;*/

        if (pPlayer->m_Health <= 0 || pPlayer->m_Health > 100)
            continue;

        if (pPlayer->m_HeadPos.x == 0 && pPlayer->m_HeadPos.y == 0 && pPlayer->m_HeadPos.z == 0)
            continue;

        if (&ESP::IsTeamGame )
        {
            if (pLocalPlayer->m_Team == pPlayer->m_Team)
                continue;
        }

        if (!IsVisible(pLocalPlayer->m_HeadPos, pPlayer->m_HeadPos))
            continue;

        float flDist = pPlayer->m_HeadPos.Distance(pLocalPlayer->m_HeadPos);

        if (flDist < flClosestDist)
        {
            flClosestDist = flDist;
            pAimTarget = pPlayer;
        }
    }

    return pAimTarget;
}


void aimbot() {
    Game *game = Game::GetInstance();
    
    Player *pLocalPlayer = game->m_LocalPlayer;
    Player* pAimTarget = GetAimbotTarget(game, pLocalPlayer);

    if (IsValidPtr(pAimTarget)) {
        Vec3 vAimPos = Vec3(pAimTarget->m_HeadPos.x, pAimTarget->m_HeadPos.y, pAimTarget->m_HeadPos.z);
        DoAimBot(pLocalPlayer, vAimPos);
    }
}