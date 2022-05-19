
#if !defined(HTTPMAIN_H_INCLUDED)

#define HTTPMAIN_H_INCLUDED

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