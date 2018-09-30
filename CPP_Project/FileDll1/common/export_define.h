#pragma once

//#define _EXPORT

#ifdef _EXPORT
#define EXPORTDLL __declspec(dllexport)
#else
#define EXPORTDLL __declspec(dllimport)
#endif