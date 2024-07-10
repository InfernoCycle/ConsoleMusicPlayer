#pragma once
#pragma once

#ifdef FILELIBRARY_EXPORTS
#define FILELIBRARY_API __declspec(dllexport)
#else
#define FILELIBRARY_API __declspec(dllimport)
#endif

extern "C" FILELIBRARY_API bool check_create();