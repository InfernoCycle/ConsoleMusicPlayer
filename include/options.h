#define addon
#pragma once
#ifdef addon
#include <codecvt>
#include <locale>
#include <iostream>

std::wstring ZStr::UTF8ToWChar(const char *in_char)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> l_converter;
    return l_converter.from_bytes(in_char);
}

#endif