#include "hook.h"
#define ASM_JUMP_REL 0xE9

bool Hooker::DetourHook(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 5) return false;

	DWORD curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

	uintptr_t relativeAddress = dst - src - 5;

	*src = 0xE9;

	*(uintptr_t*)(src + 1) = relativeAddress;

	VirtualProtect(src, len, curProtection, &curProtection);
	return true;
}

BYTE* Hooker::TrampHook(BYTE* src, BYTE* dst, const uintptr_t len)
{
	if (len < 5) return 0;
	m_src = src; m_dst = dst; m_len = len;

	//Create Gateway
	BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	
	m_stolenBytes = new BYTE[len];
	m_stolenBytesLen = len;

	memcpy(m_stolenBytes, src, len);

	//write the stolen bytes to the gateway
	memcpy_s(gateway, len, src, len);

	//Get the gateway to destination address
	uintptr_t gatewayRelativeAddr = src - gateway - 5;

	// add the jmp opcode to the end of the gateway
	*(gateway + len) = 0xE9;

	//Write the address of the gateway to the jmp
	*(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddr;

	//Perform the detour
	DetourHook(src, dst, len);

	return gateway;
}

void Hooker::Unhook()
{
	DWORD protection;
	VirtualProtect(m_src, m_len, PAGE_EXECUTE_READWRITE, &protection);

	// write the original shit back
	memcpy(m_src, m_stolenBytes, m_len);

	VirtualProtect(m_src, m_len, protection, &protection);
}

void Hooker::WriteBytes(BYTE* to, BYTE* from, const size_t len)
{
	DWORD prot;
	VirtualProtect(to, len, PAGE_EXECUTE_READWRITE, &prot);

	memcpy_s(to, len, from, len);

	VirtualProtect(to, len, prot, &prot);
}

void Patch::Enable()
{
	// write patchBytes
	m_hooker->WriteBytes(m_addr, m_patchBytes, m_len);
}

void Patch::Disable()
{
	// write originalBytes
	m_hooker->WriteBytes(m_addr, m_originalBytes, m_len);
}

Patch* Hooker::NewPatch(BYTE* to, BYTE* from, const size_t len, bool* ptr)
{
	Patch* patch = new Patch(to, from, len, this, ptr);
	return patch;
}

std::vector<Patch*> Patch::patches{}; // init
void Patch::DisableAllPatches()
{
	for (Patch* patch : Patch::patches)
	{
		patch->Disable();
		delete patch;
	}
}

void Patch::WatchToggles()
{
	for (Patch* patch : Patch::patches)
	{
		bool thisState = *(patch->m_toggleRef);

		// if changed
		if (thisState != patch->m_prev)
		{
			if (thisState)
				patch->Enable();
			else
				patch->Disable();

			patch->m_prev = thisState;
		}
	}
}