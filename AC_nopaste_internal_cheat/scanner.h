#pragma once
#include <Windows.h>
#include "ntdll.h"
#include <iostream>

class Scanner
{
public:
    char* ScanBasic(char* pattern, char* mask, char* begin, intptr_t size);

    char* ScanInternal(char* pattern, char* mask, char* begin, intptr_t size);
    char* TO_CHAR(wchar_t* string);
    PEB* GetPEB();
    LDR_DATA_TABLE_ENTRY* GetLDREntry(std::string name);
    char* ScanModIn(char* pattern, char* mask, std::string modName);
};