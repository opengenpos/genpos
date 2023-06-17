/* pifqueue.c : PIF queue control function under Windows CE */

#pragma warning( disable : 4201 4214 4514)
#include "stdafx.h"
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "piflocal.h"

#define NPORTS          20      /* maximum number of ports              */
#define MAXMSGS         100     /* maximum messages on all ports        */
#define PTFREE          1       /* port is free                         */
#define PTLIMBO         2       /* port is being deleted/reset          */
#define PTALLOC         3       /* port is allocated                    */
#define PTEMPTY         -1      /* initial semaphore entries            */

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

typedef struct ptnode {         /* node on list of message ptrs         */
    void    *ptmsg;             /* message pointer                      */
    struct  ptnode  *ptnext;    /* address of next node on list         */
} PTNODE;

typedef struct pt {             /* entry in the port table              */
    char    ptstate;            /* port state (FREE/LIMBO/ALLOC)        */
    USHORT  ptssem;             /* sender semaphore                     */
    USHORT  ptrsem;             /* receiver semaphore                   */
    int     ptmaxcnt;           /* max messages to be queued            */
    int     ptct;               /* no. of messages in queue             */
    int     ptseq;              /* sequence changed at creation         */
    struct  ptnode  *pthead;    /* list of message pointers             */
    struct  ptnode  *pttail;    /* tail of message list                 */
} PT;

#if (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif /* (_WIN32_WCE || WIN32) && _MSC_VER >= 800 */

SHORT   nextqueue;
int     ptnextp = 0;        /* next port to examine when looking for a free one */
PTNODE  *ptfree = 0;        /* list of free queued nodes            */
PT      ports[NPORTS];
PTNODE  ptmax[MAXMSGS];  

CRITICAL_SECTION	g_QueCriticalSection;

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   USHOT PIFENTRY PifCreateQueue(USHORT usCount        **
**              usCount:    max number of queue                     **
**                                                                  **
**  return:     handle of queue                                     **
**                                                                  **
**  Description:Create queue                                        **     
**                                                                  **
**********************************************************************
fhfh*/
USHORT PIFENTRY PifCreateQueue(USHORT usCount)
{
    int	    i, p;
    struct  pt  *ptptr;

    EnterCriticalSection(&g_QueCriticalSection);
    
    for (i=0 ; i<NPORTS ; i++) {
        if ( (p=ptnextp--) <= 0 )
            ptnextp = NPORTS - 1;
        if ( (ptptr= &ports[p])->ptstate == PTFREE ) {
            ptptr->ptstate  = PTALLOC;
            ptptr->ptssem   = PifCreateSem(usCount);
			// specify an initial value of zero so that PifReadQueue() will block until something is available to read.
            ptptr->ptrsem   = PifCreateSemNew(usCount, 0, __FILE__, __LINE__);  // must specify initial value of zero.
            ptptr->pthead   = ptptr->pttail = (struct ptnode *)NULL;
            ptptr->ptmaxcnt = usCount;
            ptptr->ptct = 0;
            LeaveCriticalSection(&g_QueCriticalSection);
            return (USHORT)(p);
        }
    }

    LeaveCriticalSection(&g_QueCriticalSection);
    PifAbort(FAULT_AT_PIFCREATEQUEUE, FAULT_SHORT_RESOURCE);
    
    return (USHORT)PIF_ERROR_SYSTEM;
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifDeleteQueue(USHORT usQueue)        **
**              usQueu: queue handle                                **
**                                                                  **
**  Description:Delete queue                                        **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifDeleteQueue(USHORT usQueue)
{
    struct  ptnode  *p;
    struct  pt  *ptptr;

    EnterCriticalSection(&g_QueCriticalSection);

    if ( usQueue >= NPORTS ||
        (ptptr=&ports[usQueue])->ptstate != PTALLOC ) {
	    LeaveCriticalSection(&g_QueCriticalSection);
		return;
	}
    ptptr->ptstate = PTLIMBO;
    ptptr->ptseq++;
    if ( (p=ptptr->pthead) != (struct ptnode *)NULL ) {
        (ptptr->pttail)->ptnext = ptfree;
        ptfree = ptptr->pthead;
    }
    PifDeleteSem(ptptr->ptssem);
    PifDeleteSem(ptptr->ptrsem);
    ptptr->ptstate = PTFREE;
    
    LeaveCriticalSection(&g_QueCriticalSection);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID * PIFENTRY PifReadQueue(USHORT usQueue)        **
**              usQueu: queue handle                                **
**                                                                  **
**  Description:Read message from queue                             **     
**                                                                  **
**********************************************************************
fhfh*/
VOID * PIFENTRY PifReadQueue(USHORT usQueue)
{
    struct  pt  *ptptr = NULL;
    char    *msg = NULL;
    struct  ptnode  *nxtnode = NULL;

    if (usQueue >= NPORTS) {
        PifAbort(FAULT_AT_PIFREADQUEUE, FAULT_SYSTEM);
		return NULL;
    }
    if ((ptptr=&ports[usQueue])->ptstate != PTALLOC) {
        PifAbort(FAULT_AT_PIFREADQUEUE, FAULT_INVALID_HANDLE);
		return NULL;
    }

    /* wait for a message and verify that the port is still allocated */
    PifRequestSem(ptptr->ptrsem);

    /* dequeue the first message that is waiting in the port */
    
    EnterCriticalSection(&g_QueCriticalSection);

    nxtnode = ptptr->pthead;
	if (nxtnode) {
		msg = nxtnode->ptmsg;
		if (ptptr->pthead == ptptr->pttail)	    /* delete the last item */
			ptptr->pthead = ptptr->pttail = (struct ptnode *)NULL;
		else
			ptptr->pthead = nxtnode->ptnext;
		nxtnode->ptnext = ptfree;           /* return to free list */
		ptfree = nxtnode;
	}
	else {
#if  defined(_DEBUG) || defined(DEBUG)
        __debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
#endif
	}

    LeaveCriticalSection(&g_QueCriticalSection);
    PifReleaseSem(ptptr->ptssem);
    return (msg);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PIFENTRY PifWriteQueue(USHORT usQueue)         **
**              usQueu: queue handle                                **
**                                                                  **
**  Description:Write message to queue                              **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PIFENTRY PifWriteQueue(USHORT usQueue, VOID *pMessage)
{
    struct  pt  *ptptr;
    struct  ptnode  *freenode;

    if (usQueue >= NPORTS) {
        PifAbort(FAULT_AT_PIFWRITEQUEUE, FAULT_SYSTEM);
		return;
    }

    if ((ptptr=&ports[usQueue])->ptstate != PTALLOC) {
        PifAbort(FAULT_AT_PIFWRITEQUEUE, FAULT_INVALID_HANDLE);
		return;
    }

    /* wait for space and verify that the port is still allocated */
    PifRequestSem(ptptr->ptssem);
    EnterCriticalSection(&g_QueCriticalSection);

    if ( ptfree == (struct ptnode *)NULL ) {
		LeaveCriticalSection(&g_QueCriticalSection);
		PifReleaseSem(ptptr->ptrsem);
        PifAbort(FAULT_AT_PIFWRITEQUEUE, FAULT_SHORT_RESOURCE);
		return;
    }
    freenode = ptfree;
    ptfree = freenode->ptnext;
    freenode->ptnext = (struct ptnode *)NULL;
    freenode->ptmsg = pMessage;
    if ( ptptr->pttail == (struct ptnode *)NULL )	/* empty queue */
        ptptr->pttail = ptptr->pthead = freenode;
    else {
        (ptptr->pttail)->ptnext = freenode;
        ptptr->pttail = freenode;
    }

    LeaveCriticalSection(&g_QueCriticalSection);
    PifReleaseSem(ptptr->ptrsem);
}

/*fhfh
**********************************************************************
**                                                                  **
**  Synopsis:   VOID PifQueueInitialize(VOID)			           **
**                                                                  **
**  Description:initialize queue buffer                              **     
**                                                                  **
**********************************************************************
fhfh*/
VOID PifQueueInitialize(VOID)
{
    int     i, maxmsgs;
    struct  ptnode  *next, *prev;

    maxmsgs = MAXMSGS;
    ptfree = ptmax;
    for (i=0 ; i<NPORTS ; i++)
        ports[i].ptstate = PTFREE;
    ptnextp = NPORTS - 1;

    /*  link up free list of message pointer nodes  */

    for ( prev=next=ptfree ; --maxmsgs > 0 ; prev=next )
        prev->ptnext = ++next;
    prev->ptnext = (struct ptnode *)NULL;

}

/* end of pifqueue.c */
