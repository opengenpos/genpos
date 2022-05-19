/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Memory Control Functions
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosMem.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00 by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:     
*
* --------------------------------------------------------------------------
* Update Histories
*
* Data       Ver.      Name         Description
* Feb-23-95  G0        O.Nakada     Initial
*
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/


/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <tchar.h>
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "vos.h"
#include "vosl.h"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
VOSMEM      FAR VosMemFree;                 /* free memory list                              */
VOSMEM      FAR aVosMem[VOS_NUM_MEMBLKS+3]; /* aVosMem[0]: free memory list header           */
                                            /* aVosMem[1]: terminator                        */
                                            /* aVosMem[2]~[2+VOS_NUM_MEMBLKS-1]: free memory */
                                            /* aVosMem[2+MBLOCKS]: terminator                */
USHORT      FAR usVosRover;                 /* index roving around free memory list          */


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID VosVirtualInit(VOID);
*
** Return:      nothing
*===========================================================================
*/
VOID VosVirtualInit(VOID)
{
    aVosMem[0].usTag  = VOS_TAG_FREE;       /* Header of free memory list    */
    aVosMem[0].usLen  = 0;                  /* Header must not be reserved   */
    aVosMem[0].usNext = 2;                  /* Point to the first free block */
    aVosMem[0].usPrev = 2;                  /* Point to the first free block */
    aVosMem[1].usTag  = VOS_TAG_USED;       /* Keep the 1'st block unused    */
    aVosMem[2].usTag  = VOS_TAG_FREE;       /* Mark the top of free block    */
    aVosMem[2].usLen  = VOS_NUM_MEMBLKS;    /* Length of the free block      */
    aVosMem[2].usNext = 0;                  /* Point to the header           */
    aVosMem[2].usPrev = 0;                  /* Point to the header           */
    aVosMem[2 + VOS_NUM_MEMBLKS - 1].usTag  /* Mark the end of free block    */
        = VOS_TAG_FREE;
    aVosMem[2 + VOS_NUM_MEMBLKS - 1].usLen  /* Length of the free block      */
        = VOS_NUM_MEMBLKS;
    aVosMem[2 + VOS_NUM_MEMBLKS].usTag      /* Keep the last block unused    */
        = VOS_TAG_USED;
    usVosRover = 2;                         /* Points the first free block   */
}


/*
*===========================================================================
** Synopsis:    VOID FAR * VosVirtualAlloc(USHORT usBytes);
*     Input:    usBytes - length of virtual memory
*
** Return:      address of virtual memory
*===========================================================================
*/
VOID FAR * VosVirtualAlloc(USHORT usBytes)
{
    USHORT  usNeeded;                   /* # of blocks needed      */
    USHORT  usFit;                      /* Fit block               */
    USHORT  usGot;                      /* Block to get            */
    USHORT  usLeft;                     /* # of blocks left        */

    if (usBytes <= 0) {                     /* invalid memory size */
        PifAbort(MODULE_VOS_VIRTUALALLOC, FAULT_INVALID_ARGUMENTS);
    }

    /* --- 2 blocks are needed for tags --- */

    usNeeded = (usBytes + sizeof(VOSMEM) - 1) / sizeof(VOSMEM) + 2;

    /* --- search for a fit block --- */

    for (usFit = usVosRover; usNeeded > aVosMem[usFit].usLen; ) {
        usFit = aVosMem[usFit].usNext;      /* see the next block         */

        if (usFit == usVosRover) {          /* now, checked over the list */
            PifAbort(MODULE_VOS_VIRTUALALLOC, FAULT_SHORT_RESOURCE);
        }
    }

    /* --- rover points the block followed by the fit block --- */

    usVosRover = aVosMem[usFit].usNext;     /* rover points the block     */
                                            /* followed by the fit block  */

    usLeft = aVosMem[usFit].usLen - usNeeded;   /* blocks will be left    */

    if (usLeft <= 2) {                      /* 2 is too small to leave    */
        usGot = usFit;                      /* get the whole block        */
        aVosMem[aVosMem[usFit].usPrev].usNext
            = usVosRover;
        aVosMem[usVosRover].usPrev          /* now, the fit block removed */
            = aVosMem[usFit].usPrev;        /* from the free list         */
    } else {
        usGot = usFit + usLeft;             /* break the fit block in two */
        aVosMem[usFit].usLen = usLeft;      /* save the new length at the */
                                            /* top of the left block      */
        aVosMem[usGot - 1].usTag            /* mark 'free' at the end of  */
            = VOS_TAG_FREE;                 /* the left block             */
        aVosMem[usGot - 1].usLen = usLeft;  /* save the new length at the */
                                            /* end of the left block      */
        aVosMem[usGot].usLen = usNeeded;    /* save the length at the top */
                                            /* of the block to get        */
    }

    aVosMem[usGot].usTag = VOS_TAG_USED;    /* mark 'used' at the top of  */
                                            /* the block to get           */
    aVosMem[usGot + usNeeded - 1].usTag     /* mark 'used' at the end of  */
        = VOS_TAG_USED;                     /* the block to get           */

    return (&aVosMem[usGot + 1]);           /* user can use the block     */
                                            /* following to the tag block */
}


/*
*===========================================================================
** Synopsis:    VOID VosVirtualFree(VOID FAR *pvBlock);
*     Input:    pvBlock - address of virtual memory
*
** Return:      nothing
*===========================================================================
*/
VOID VosVirtualFree(VOID FAR *pvBlock)
{
    USHORT  usFreed;                    /* nlock to free              */
    USHORT  usSide;                     /* neighbor block             */
    USHORT  usNext;                     /* foreward free memory link  */
    USHORT  usPrev;                     /* backward free memory link  */
    VOSMEM  FAR *pMem = aVosMem;        /* points the address of heap */

    if (pvBlock < (VOID FAR *)&aVosMem[1] ||        /* pointer is out of heap area */
        (VOID FAR *)&aVosMem[VOS_NUM_MEMBLKS - 2] < pvBlock) {	/* ### Mod (2171 for Win32 V1.0) */
        PifAbort(MODULE_VOS_VIRTUALFREE, FAULT_INVALID_ARGUMENTS);
    }

    usFreed = (FP_OFF(pvBlock) - FP_OFF(pMem)) / sizeof(VOSMEM) - 1;

    if (aVosMem[usFreed].usTag != VOS_TAG_USED) { /* this is a wrong block */
        PifAbort(MODULE_VOS_VIRTUALFREE, FAULT_INVALID_ARGUMENTS);
    }

    if (aVosMem[usFreed - 1].usTag == VOS_TAG_FREE) {
                                            /* The neighbor block just      */
                                            /* before 'usFreed' is free     */
        usSide = usFreed - aVosMem[usFreed - 1].usLen;
                                            /* usSize points the neighbor   */
        aVosMem[usSide].usLen += aVosMem[usFreed].usLen;
                                            /* Bind the two blocks          */
        usNext = aVosMem[usSide].usNext;
        usPrev = aVosMem[usSide].usPrev;
        aVosMem[usNext].usPrev = usPrev;
        aVosMem[usPrev].usNext = usNext;
                                            /* Now, the bound block removed */
                                            /* from the free list           */
        usFreed = usSide;
    }

    usSide = usFreed + aVosMem[usFreed].usLen;

    if (aVosMem[usSide].usTag == VOS_TAG_FREE) {
                                            /* The neighbor block just      */
                                            /* after 'usFreed' is free      */
        aVosMem[usFreed].usLen += aVosMem[usSide].usLen;
                                            /* Bind the two blocks          */
        usNext = aVosMem[usSide].usNext;
        usPrev = aVosMem[usSide].usPrev;
        aVosMem[usNext].usPrev = usPrev;
        aVosMem[usPrev].usNext = usNext;
                                            /* Now, the bound block removed */
                                            /* from the free list           */
        usSide += aVosMem[usSide].usLen;    /* This is a new neighbor       */
    }

    aVosMem[usSide - 1].usLen               /* save the length of the freed */
        = aVosMem[usFreed].usLen;           /* block at the end             */

    aVosMem[usFreed].usNext                 /* the freed block is followed */
        = aVosMem[0].usNext;                /* by the current 1'st block   */
    aVosMem[usFreed].usPrev = 0;            /* the freed block follows     */
                                            /* the free list header        */
    aVosMem[aVosMem[0].usNext].usPrev       /* The current 1'st block      */
        = usFreed;                          /* follows the freed block     */
    aVosMem[0].usNext = usFreed;            /* the free list header is     */
                                            /* followed by the freed block */

    aVosMem[usFreed].usTag    = VOS_TAG_FREE;   /* mark 'free' at the top  */
    aVosMem[usSide - 1].usTag = VOS_TAG_FREE;   /* mark 'free' at the end  */
}


/* ===================================== */
/* ========== End of VosMem.C ========== */
/* ===================================== */
