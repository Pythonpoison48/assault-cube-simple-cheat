
#include <iostream>
#include "mem.h"
#include "hook.h"
#include "glDraw.h"
#include "gltext.h"
#include "ESP.h"
#include "aimbot.h"

uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

bool bHealth = false, bAmmo = false, bRecoil = false, bTrigger = false, bNoClip = false, bAimbot = false, bDraw = false;

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);

twglSwapBuffers owglSwapBuffers;
twglSwapBuffers wglSwapBuffersGateway;


//for ESP
GL::Font glFont;
const int FONT_HEIGHT = 15;
const int FONT_WIDTH = 9;

//for noclip 

BYTE spectateOn = 5;
BYTE spectateOff = 0;
BYTE InvisibleOn = 11;
BYTE InvisibleOff = 0;

uintptr_t entAddr = 0;
uintptr_t specAddr = 0;
uintptr_t InvisibleAddr = 0;

//for triggerbot
typedef ent* (__cdecl* tGetCrossHairEnt)();

tGetCrossHairEnt GetCrossHairEnt{ nullptr };



ESP esp;
void Draw()
{
	HDC currentHDC = wglGetCurrentDC();

	if (!glFont.bBuilt || currentHDC != glFont.hdc)
	{
		glFont.Build(FONT_HEIGHT);
	}

	GL::SetupOrtho();
	esp.Draw(glFont);
	
	GL::RestoreGL();
}

BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
	if (GetAsyncKeyState(VK_F7) & 1) {
		bDraw = !bDraw;
		
	}
	if (bDraw) {
		Draw();
	}
	




	if (GetAsyncKeyState(VK_F1) & 1)
		bHealth = !bHealth;

	if (GetAsyncKeyState(VK_F2) & 1)
	{
		bAmmo = !bAmmo;
		std::cout << "infinite ammo !! " << std::endl;
	}

	//no recoil NOP
	if (GetAsyncKeyState(VK_F3) & 1)
	{
		bRecoil = !bRecoil;

		if (bRecoil)
		{
			std::cout << "no recoil" << std::endl;
			mem::Nop((BYTE*)(moduleBase + 0x63786), 10);
		}

		else
		{
			std::cout << "recoil !!!" << std::endl;
			//50 8D 4C 24 1C 51 8B CE FF D2 the original stack setup and call
			mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);
		}
	}

	if (GetAsyncKeyState(VK_F4) & 1) {
		bTrigger = !bTrigger;
		std::cout << "triggerbot !!!" << std::endl;

	}

	if (GetAsyncKeyState(VK_F5) & 1) {
		bNoClip = !bNoClip;
		specAddr = mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x338 });
		InvisibleAddr = mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x82 });

		if (bNoClip) {
			std::cout << "no clip activated" << std::endl;
			int* spectator = (int*)specAddr;
			*spectator = spectateOn;                  // essayer de remplacer sa avec reclass.net

			int* Invisible = (int*)InvisibleAddr;
			*Invisible = InvisibleOn;
		}
		else {
			std::cout << "no clip desactivated" << std::endl;
			int* spectator = (int*)specAddr;
			*spectator = spectateOff;

			int* Invisible = (int*)InvisibleAddr;
			*Invisible = InvisibleOff;
		}
	}
	if (GetAsyncKeyState(VK_F6) & 1) {
		std::cout << "aimbot !!!" << std::endl;
		bAimbot = !bAimbot;
	}

	


	//need to use uintptr_t for pointer arithmetic later
	uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10F4F4);
	ent* localPlayerPtr2 = *(ent**)(moduleBase + 0x10F4F4);
	//continuous writes / freeze
	if (localPlayerPtr)
	{
		if (bHealth)
		{
			*(int*)(*localPlayerPtr + 0xF8) = 1337;
		}

		if (bAmmo)
		{
			*(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 }) = 1337;
		}
		if (bTrigger)
		{
			GetCrossHairEnt = (tGetCrossHairEnt)(moduleBase + 0x607c0);
			ent* crossHairent{ nullptr };
			ent* crossHairEnt = GetCrossHairEnt();

			if (crossHairEnt) {
				if (localPlayerPtr2->team != crossHairEnt->team)
				{
					localPlayerPtr2->bAttacking = 1;

				}
				else {
					localPlayerPtr2->bAttacking = 0;
				}
			}

		}

		if(bAimbot){
			aimbot();
		}
	}

	
	

	return wglSwapBuffersGateway(hDc);
}

DWORD WINAPI HackThread(HMODULE hModule)
{
	//Create Console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "aaa" << std::endl;
	std::cout << "OG for a fee, stay sippin' fam " << "\n";

	// Hook
	
	Hook SwapBuffersHook("wglSwapBuffers", "opengl32.dll", (BYTE*)hkwglSwapBuffers, (BYTE*)&wglSwapBuffersGateway, 5);
	

	SwapBuffersHook.Enable();

	while (true) {
		if (GetAsyncKeyState(VK_F12) & 1) {
			SwapBuffersHook.Disable();
			break;

		}
		Sleep(1000);
	}
	

	
	
	
	
	//

	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}