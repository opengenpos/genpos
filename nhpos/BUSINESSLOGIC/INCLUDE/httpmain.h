
#if !defined(HTTPMAIN_H_INCLUDED)

#define HTTPMAIN_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ServerProc.h"

#ifdef __cplusplus
extern "C" {
#endif

extern PifSemHandle semHttpServer;

extern ServerProcInterface HttpThreadMyInterface;

#ifdef __cplusplus
};
#endif


#endif