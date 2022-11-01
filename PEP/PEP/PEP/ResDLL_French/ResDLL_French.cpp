// ResDLL_Spanish.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#ifdef __cplusplus
extern "C" {
	void APIENTRY dllDummyEntry (void);
};
#endif

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

void APIENTRY dllDummyEntry (void)
{
	// This is a dummy entry point to force create of a lib file
	// The lib file is needed to create dependencies.
}
