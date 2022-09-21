#include "scanner.h"

char* Scanner::ScanBasic(char* pattern, char* mask, char* begin, intptr_t size)
{
    intptr_t patternLen = strlen(mask);

    for (int i = 0; i < size; i++)
    {
        bool found = true;
        for (int j = 0; j < patternLen; j++)
        {
            // break if current mask char is not wildcard AND no match for pattern
            if (mask[j] != '?' && pattern[j] != *(char*)((intptr_t)begin + i + j))
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return (begin + i);
        }
    }
    return nullptr;
}

char* Scanner::ScanInternal(char* pattern, char* mask, char* begin, intptr_t size)
{
    char* match{ nullptr };
    MEMORY_BASIC_INFORMATION mbi{};

    for (char* curr = begin; curr < begin + size; curr += mbi.RegionSize)
    {
        // query memory and skip bad areas
        if (!VirtualQuery(curr, &mbi, sizeof(mbi))
            || mbi.State != MEM_COMMIT
            || mbi.Protect == PAGE_NOACCESS)
            continue;

        match = ScanBasic(pattern, mask, curr, mbi.RegionSize);

        if (match != nullptr)
        {
            break;
        }
    }
    return match;
}

char* Scanner::TO_CHAR(wchar_t* string)
{
    size_t len = wcslen(string) + 1;
    char* c_string = new char[len];
    size_t numCharsRead;
    wcstombs_s(&numCharsRead, c_string, len, string, _TRUNCATE);
    return c_string;
}

PEB* Scanner::GetPEB()
{
#ifdef _WIN64
    PEB* peb = (PEB*)__readgsqword(0x60);

#else
    PEB* peb = (PEB*)__readfsdword(0x30);
#endif


    return peb;
}

LDR_DATA_TABLE_ENTRY* Scanner::GetLDREntry(std::string name)
{
    LDR_DATA_TABLE_ENTRY* ldr = nullptr;

    PEB* peb = GetPEB();

    LIST_ENTRY head = peb->Ldr->InMemoryOrderModuleList;


    LIST_ENTRY curr = head;

    // start looping the doubly linked list
    while (curr.Flink != head.Blink)
    {
        LDR_DATA_TABLE_ENTRY* mod = (LDR_DATA_TABLE_ENTRY*)CONTAINING_RECORD(curr.Flink, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        if (mod->FullDllName.Buffer)
        {
            char* cName = TO_CHAR(mod->BaseDllName.Buffer);

            // compare string
            if (_stricmp(cName, name.c_str()) == 0)
            {
                // strings equal, found module by name
                ldr = mod;
                break;
            }
            delete[] cName;
        }
        curr = *curr.Flink;
    }
    return ldr;
}

char* Scanner::ScanModIn(char* pattern, char* mask, std::string modName)
{
    // whole point of this is so that we do not need to scan the whole address range,
    // we can just scan the main executable's address range
    LDR_DATA_TABLE_ENTRY* ldr = GetLDREntry(modName);

    char* match = ScanInternal(pattern, mask, (char*)ldr->DllBase, ldr->SizeOfImage);

    return match;
}