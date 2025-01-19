#pragma once

#ifdef DCM_EXPORTS
#define DCM_API __declspec(dllexport)
#else
#define DCM_API __declspec(dllimport)
#endif