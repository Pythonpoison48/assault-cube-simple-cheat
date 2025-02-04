#pragma once
#include "stdafx.h"
#include <windows.h>
#include <vector>
//https://guidedhacking.com/threads/opengl-hooking-drawing-text-rendering.14460/

namespace mem
{
	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
	void Nop(BYTE* dst, unsigned int size);
	void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
}