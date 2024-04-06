#pragma once

#ifdef CS_CORRELATION_LEARN_ALGO_USER_DLL
#define CS_CORR_LEARN_ALGO_API __declspec(dllexport)
#else
#define CS_CORR_LEARN_ALGO_API __declspec(dllimport)
#endif

