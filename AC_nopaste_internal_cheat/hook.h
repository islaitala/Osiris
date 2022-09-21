#pragma once
#include <Windows.h>
#include <vector>

class Hooker;

class Patch
{
private:
	BYTE* m_originalBytes;
	BYTE* m_patchBytes;
	size_t m_len;
	Hooker* m_hooker;
	BYTE* m_addr;
	bool m_prev;
	bool* m_toggleRef;
	
public:
	static std::vector<Patch*> patches;
	void Enable();
	void Disable();
	static void DisableAllPatches();
	static void WatchToggles();

	Patch(BYTE* originalBytes, BYTE* patchBytes, size_t originalBytesLen, Hooker* hooker, bool* toggleRef)
	{
		m_hooker = hooker;
		m_originalBytes = new BYTE[originalBytesLen];
		memcpy(m_originalBytes, originalBytes, originalBytesLen);
		m_patchBytes = patchBytes;
		m_len = originalBytesLen;
		m_addr = originalBytes;
		m_toggleRef = toggleRef;
		m_prev = *m_toggleRef;
		patches.push_back(this);
		m_prev = false;

		if (*toggleRef)
		{
			Enable();
		}
	}

	~Patch()
	{
		delete[] m_originalBytes;
		m_originalBytes = nullptr;
	}
};

class Hooker
{
private:
	BYTE* m_stolenBytes;
	size_t m_stolenBytesLen;
	BYTE* m_src;
	BYTE* m_dst;
	uintptr_t m_len;

public:
	bool DetourHook(BYTE* src, BYTE* dst, const uintptr_t len);
	BYTE* TrampHook(BYTE* src, BYTE* dst, const uintptr_t len);
	void Unhook();
	void WriteBytes(BYTE* to, BYTE* from, const size_t len);
	Patch* NewPatch(BYTE* to, BYTE* from, const size_t len, bool* ptr);

	~Hooker()
	{
		delete m_stolenBytes;
		m_stolenBytes = nullptr;
	}
};