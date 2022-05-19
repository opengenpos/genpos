/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Initialization
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDINIT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*               MldInit(): set initial data
*            MldRefresh(): reset mld data
*            MldSetMode(): set scroll dipslay arrangement
*        MldDisplayInit(): clear scroll dipslay area
*       MldGetMldStatus(): show mld is provied or not
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Mar-10-95 : 03.00.00 :  M.Ozawa   : Initial
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbstb.h>
#include <rfl.h>
#include <uie.h>
#include <vos.h>
#include <mld.h>
#include "mldlocal.h"
#include "mldrin.h"
#include "termcfg.h"

/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
MLDCURTRNSCROL MldScroll1Buff;   /* display#1 data buffer */

#if DISPTYPE==1
MLDCURTRNSCROL MldScroll2Buff;   /* display#2 data buffer */
MLDCURTRNSCROL MldScroll3Buff;   /* display#3 data buffer */

MLDOTHERHANDLE MldOtherHandle1;  /* display #1 data buffer */
MLDOTHERHANDLE MldOtherHandle2;  /* display #2 data buffer */
MLDOTHERHANDLE MldOtherHandle3;  /* display #3 data buffer */

UCHAR   uchMldCurScroll;         /* current scroll display */
#endif

USHORT usMldPopupHandle;        /* window handle of popup display */

UCHAR   uchMldSystem;            /* current display arrangement */

#if DISPTYPE==1
UCHAR   uchMldSystem2;           /* for split guest check system */
UCHAR   uchMldScrollColumn1;     /* scroll display#1 column length */
UCHAR   uchMldScrollColumn2;     /* scroll display#2 column length */
UCHAR   uchMldScrollColumn3;     /* scroll display#3 column length */

UCHAR   uchMldScrollLine;        /* length of row of scroll display */
UCHAR   uchMldBottomCursor;      /* bottom cursor position of scroll display */
#endif

//USHORT  husMldSemaphore;         /* multiline display semaphore handle, moved to MldMain.c */

MLDTEMPSTORAGE MldTempStorage;  /* temporary storage buffer data */

/*
*===========================================================================
** Format  : SHORT MldInit(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: initialize multiline display service
*===========================================================================
*/
SHORT    MldInit(VOID)
{

    /* create semaphore for internal use */
    husMldSemaphore = PifCreateSem(0);
    PifReleaseSem(husMldSemaphore);

    memset (&MldScroll1Buff, 0x00, sizeof(MLDCURTRNSCROL));
	MldScroll1Buff.uchCurItem = 0;

#if DISPTYPE==1
    memset (&MldScroll2Buff, 0x00, sizeof(MLDCURTRNSCROL));
	MldScroll2Buff.uchCurItem = 0;

    memset (&MldScroll3Buff, 0x00, sizeof(MLDCURTRNSCROL));
	MldScroll3Buff.uchCurItem = 0;

    uchMldCurScroll = 0;
    uchMldSystem2 = 0;
#endif

    usMldPopupHandle = 0;
    uchMldSystem = 0;
    memset (&MldTempStorage, 0x00, sizeof(MLDTEMPSTORAGE));

#if DISPTYPE==1
    MldSetDescriptor(MLD_PRECHECK_SYSTEM);  /* set default descriptor */
#endif

    return (MLD_SUCCESS);
    
}

/*
*===========================================================================
** Format  : SHORT MldRefresh(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: reinitialize multiline display service
*===========================================================================
*/
SHORT   MldRefresh(VOID)
{
    if (uchMldSystem) {

        switch (uchMldSystem) {
        case MLD_PRECHECK_SYSTEM:
        case MLD_DRIVE_THROUGH_1SCROLL:
#if DISPTYPE==1
            VosDeleteWindow(MldOtherHandle1.usOrderHandle);     /* cursor window */
            VosDeleteWindow(MldScroll1Buff.usWinHandle);
            VosDeleteWindow(MldScroll2Buff.usWinHandle);
#endif
            /* --- close storage file --- */
            MldStoCloseFile();

            break;

        case MLD_DRIVE_THROUGH_3SCROLL:
        case MLD_SPLIT_CHECK_SYSTEM:
#if DISPTYPE==1
            VosDeleteWindow(MldOtherHandle1.usOrderHandle);
            VosDeleteWindow(MldScroll1Buff.usWinHandle);
            VosDeleteWindow(MldOtherHandle1.usTotalHandle);

            VosDeleteWindow(MldOtherHandle2.usOrderHandle);
            VosDeleteWindow(MldScroll2Buff.usWinHandle);
            VosDeleteWindow(MldOtherHandle2.usTotalHandle);

            VosDeleteWindow(MldOtherHandle3.usOrderHandle);
            VosDeleteWindow(MldScroll3Buff.usWinHandle);
            VosDeleteWindow(MldOtherHandle3.usTotalHandle);
#endif
            /* --- close storage file --- */
            MldStoCloseFile();

            break;

        case MLD_SUPER_MODE:
#if DISPTYPE==1
            VosDeleteWindow(MldScroll1Buff.usWinHandle);
#endif
            break;
        }
    }

    /* --- clear static area --- */
    memset (&MldScroll1Buff, 0x00, sizeof(MLDCURTRNSCROL));
	MldScroll1Buff.uchCurItem = 0;

#if DISPTYPE==1
    memset (&MldScroll2Buff, 0x00, sizeof(MLDCURTRNSCROL));
	MldScroll2Buff.uchCurItem = 0;

    memset (&MldScroll3Buff, 0x00, sizeof(MLDCURTRNSCROL));
	MldScroll3Buff.uchCurItem = 0;

    uchMldScrollColumn1 = 0;
    uchMldScrollColumn2 = 0;
    uchMldScrollColumn3 = 0;
    uchMldScrollLine = 0;
    uchMldBottomCursor = MLD_OUT_OF_SCROLL_AREA;
    //uchMldCurScroll = 0;
    uchMldSystem2 = 0;
#endif

    //usMldPopupHandle = 0;
    uchMldSystem = 0;
    memset (&MldTempStorage, 0x00, sizeof(MLDTEMPSTORAGE));

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT MldSetMode(USHORT usSystem)
*
*   Input  : USHORT     usSystem    -Display Type Selection
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: create scroll display window according to system selection
*===========================================================================
*/
SHORT    MldSetMode(USHORT usSystem)
{
    SHORT   sReturn;

#if DISPTYPE==1
    ITEMTRANSOPEN   ItemTransOpen;
    UCHAR   uchPreSystem = 0;

    if (uchMldSystem == (UCHAR)usSystem) {
        return(MLD_SUCCESS);
    }

    if (uchMldSystem2 == MLD_SPLIT_CHECK_SYSTEM) {

        uchPreSystem = MLD_SPLIT_CHECK_SYSTEM;

    }
#endif

    MldRefresh();       /* destory previous mode */

    switch(usSystem) {
    case MLD_PRECHECK_SYSTEM:
    case MLD_DRIVE_THROUGH_1SCROLL:

        sReturn = MldSetPrecheckSystem(usSystem);
#if DISPTYPE==1
        if (sReturn == MLD_SUCCESS) {
            if (uchPreSystem == MLD_SPLIT_CHECK_SYSTEM) {
                /* redisplay cashier/waiter/gcf no. */
                memset(&ItemTransOpen, 0, sizeof(ItemTransOpen));
                ItemTransOpen.uchMinorClass = CLASS_NEWCHECK;
                MldWriteGuestNo(&ItemTransOpen);
            }
        }
#endif
        return(sReturn);
        break;

    case MLD_DRIVE_THROUGH_3SCROLL:
    case MLD_SPLIT_CHECK_SYSTEM:

        return (MldSetDrive3ScrollSystem(usSystem));

        break;

    case MLD_SUPER_MODE:

        return (MldSetSuperModeSystem());

        break;

    default:
        return(MLD_ERROR);
    }

}

/*
*===========================================================================
** Format  : SHORT MldSetPrecheckSystem(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: create scroll display window for precheck/postcheck system
*===========================================================================
*/
SHORT MldSetPrecheckSystem(USHORT usSystem)
{
#if DISPTYPE==1
    USHORT usHandle1, usHandle2, usHandle3;

    /* scroll #1 display */
    usHandle3 = MldCreatePrechkScroll1A(MLD_C_BLACK);   /* cursor window */
    usHandle1 = MldCreatePrechkScroll1(MLD_C_BLACK);
    VosSetCurType(usHandle1, VOS_VISIBLE);

    /* scroll #2 display */
    usHandle2 = MldCreatePrechkScroll2(MLD_C_BLACK);

    MldScroll1Buff.usWinHandle = usHandle1;
    MldScroll1Buff.usAttrib = 0;
    MldScroll2Buff.usWinHandle = usHandle2;
    MldScroll2Buff.usAttrib = 0;
    MldOtherHandle1.usOrderHandle = usHandle3;  /* cursor window */
    MldOtherHandle1.usOrderAttrib = 0;

    uchMldCurScroll = MLD_SCROLL_1;

    uchMldScrollColumn1 = MLD_NOR_DSP1_CLM_LEN;
    uchMldScrollColumn2 = MLD_NOR_DSP2_CLM_LEN;
	MldScroll1Buff.usMldScrollColumn = MLD_NOR_DSP1_CLM_LEN;
	MldScroll2Buff.usMldScrollColumn = MLD_NOR_DSP2_CLM_LEN;

    uchMldScrollLine = MLD_NOR_DSP1_ROW_LEN;
	MldScroll1Buff.usMldScrollLine = MLD_NOR_DSP1_ROW_LEN;
	MldScroll2Buff.usMldScrollLine = MLD_NOR_DSP2_ROW_LEN;

    uchMldBottomCursor = MLD_NOR_DSP1_ROW_LEN - 1;
	MldScroll1Buff.usMldBottomCursor = MLD_NOR_DSP1_ROW_LEN - 1;
	MldScroll2Buff.usMldBottomCursor = MLD_NOR_DSP2_ROW_LEN - 1;

	MldScroll1Buff.usScroll = MLD_SCROLL_1;
	MldScroll2Buff.usScroll = MLD_SCROLL_2;

    uchMldSystem2 = 0;
#endif

    if (usSystem == MLD_PRECHECK_SYSTEM) {
        uchMldSystem = MLD_PRECHECK_SYSTEM;
    } else {
        uchMldSystem = MLD_DRIVE_THROUGH_1SCROLL;
    }
    memset (&MldTempStorage, 0x00, sizeof(MLDTEMPSTORAGE));

    /* set file handle (dummy) */
    MldScroll1Buff.sFileHandle = -1;
    MldScroll1Buff.sIndexHandle = -1;
#if DISPTYPE==1
    MldScroll2Buff.sFileHandle = -1;
    MldScroll2Buff.sIndexHandle = -1;
#endif

    /* storage file open */
    return (MldStoOpenFile());
}

/*
*===========================================================================
** Format  : SHORT MldSetDrive3ScrollSystem(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: create scroll display window for drive through 3 scroll system
*===========================================================================
*/
SHORT MldSetDrive3ScrollSystem(USHORT usSystem)
{
#if DISPTYPE==1
    USHORT usHandle1, usHandle2, usHandle3;
    USHORT usOrderHandle1, usOrderHandle2, usOrderHandle3;
    USHORT usTotalHandle1, usTotalHandle2, usTotalHandle3;

    /* scroll #1 display */
    usHandle1 = MldCreateDrive3Scroll1(MLD_C_BLACK);
    VosSetCurType(usHandle1, VOS_VISIBLE);

    /* scroll #2 display */
    usHandle2 = MldCreateDrive3Scroll2(MLD_C_BLACK);

    /* scroll #3 display */
    usHandle3 = MldCreateDrive3Scroll3(MLD_C_BLACK);

    /* order #1 display */
    usOrderHandle1 = MldCreateOrder1(MLD_C_BLACK);

    /* order #2 display */
    usOrderHandle2 = MldCreateOrder2(MLD_C_BLACK);

    /* order #3 display */
    usOrderHandle3 = MldCreateOrder3(MLD_C_BLACK);

    /* subtotal #1 display */
    usTotalHandle1 = MldCreateTotal1(MLD_C_BLACK);

    /* subtotal #2 display */
    usTotalHandle2 = MldCreateTotal2(MLD_C_BLACK);

    /* subtotal #3 display */
    usTotalHandle3 = MldCreateTotal3(MLD_C_BLACK);

    MldScroll1Buff.usWinHandle = usHandle1;
    MldScroll1Buff.usAttrib = 0;
    MldScroll2Buff.usWinHandle = usHandle2;
    MldScroll2Buff.usAttrib = 0;
    MldScroll3Buff.usWinHandle = usHandle3;
    MldScroll3Buff.usAttrib = 0;
    MldOtherHandle1.usOrderHandle = usOrderHandle1;
    MldOtherHandle1.usOrderAttrib = 0;
    MldOtherHandle2.usOrderHandle = usOrderHandle2;
    MldOtherHandle2.usOrderAttrib = 0;
    MldOtherHandle3.usOrderHandle = usOrderHandle3;
    MldOtherHandle3.usOrderAttrib = 0;
    MldOtherHandle1.usTotalHandle = usTotalHandle1;
    MldOtherHandle1.usTotalAttrib = 0;
    MldOtherHandle2.usTotalHandle = usTotalHandle2;
    MldOtherHandle2.usTotalAttrib = 0;
    MldOtherHandle3.usTotalHandle = usTotalHandle3;
    MldOtherHandle3.usTotalAttrib = 0;

    uchMldCurScroll = MLD_SCROLL_1;

    uchMldScrollColumn1 = MLD_DRV3_DSP1_CLM_LEN;
    uchMldScrollColumn2 = MLD_DRV3_DSP2_CLM_LEN;
    uchMldScrollColumn3 = MLD_DRV3_DSP3_CLM_LEN;
	MldScroll1Buff.usMldScrollColumn = MLD_DRV3_DSP1_CLM_LEN;
	MldScroll2Buff.usMldScrollColumn = MLD_DRV3_DSP2_CLM_LEN;
	MldScroll3Buff.usMldScrollColumn = MLD_DRV3_DSP3_CLM_LEN;

    uchMldScrollLine = MLD_DRV3_DSP1_ROW_LEN;
	MldScroll1Buff.usMldScrollLine = MLD_DRV3_DSP1_ROW_LEN;
	MldScroll2Buff.usMldScrollLine = MLD_DRV3_DSP2_ROW_LEN;
	MldScroll3Buff.usMldScrollLine = MLD_DRV3_DSP3_ROW_LEN;

    uchMldBottomCursor = MLD_DRV3_DSP1_ROW_LEN - 1;
	MldScroll1Buff.usMldBottomCursor = MLD_DRV3_DSP1_ROW_LEN - 1;
	MldScroll2Buff.usMldBottomCursor = MLD_DRV3_DSP2_ROW_LEN - 1;
	MldScroll3Buff.usMldBottomCursor = MLD_DRV3_DSP3_ROW_LEN - 1;

	MldScroll1Buff.usScroll = MLD_SCROLL_1;
	MldScroll2Buff.usScroll = MLD_SCROLL_2;
	MldScroll3Buff.usScroll = MLD_SCROLL_3;

    if (usSystem == MLD_SPLIT_CHECK_SYSTEM) {
        uchMldSystem2 = MLD_SPLIT_CHECK_SYSTEM;
    }
#endif

    uchMldSystem = MLD_DRIVE_THROUGH_3SCROLL;
    memset (&MldTempStorage, 0x00, sizeof(MLDTEMPSTORAGE));

    /* set file handle (dummy) */
    MldScroll1Buff.sFileHandle = -1;
    MldScroll1Buff.sIndexHandle = -1;
#if DISPTYPE==1
    MldScroll2Buff.sFileHandle = -1;
    MldScroll2Buff.sIndexHandle = -1;
    MldScroll3Buff.sFileHandle = -1;
    MldScroll3Buff.sIndexHandle = -1;
#endif

    /* storage file open */
    return (MldStoOpenFile());

#if DISPTYPE==2
    usSystem = usSystem;
#endif
}

/*
*===========================================================================
** Format  : SHORT MldSetSuperModeSystem(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: create scroll display window for supervisor mode
*===========================================================================
*/
SHORT MldSetSuperModeSystem(VOID)
{
#if DISPTYPE==1
    USHORT usHandle1;

    /* scroll #1 display */
    usHandle1 = MldCreateSuperScroll1(MLD_C_BLACK);

    MldScroll1Buff.usWinHandle = usHandle1;
    MldScroll1Buff.usAttrib = 0;

    uchMldCurScroll = 0;

    uchMldScrollColumn1 = MLD_SUPER_MODE_CLM_LEN;
	MldScroll1Buff.usMldScrollColumn = MLD_SUPER_MODE_CLM_LEN;

    uchMldScrollLine = MLD_SUPER_MODE_ROW_LEN;
	MldScroll1Buff.usMldScrollLine = MLD_SUPER_MODE_ROW_LEN;

    uchMldBottomCursor = MLD_SUPER_MODE_ROW_LEN - 1;
	MldScroll1Buff.usMldBottomCursor = MLD_SUPER_MODE_ROW_LEN - 1;

	MldScroll1Buff.usScroll = MLD_SCROLL_1;

    uchMldSystem2 = 0;
#endif

    uchMldSystem = MLD_SUPER_MODE;
    memset (&MldTempStorage, 0x00, sizeof(MLDTEMPSTORAGE));

    /* set file handle (dummy) */
    MldScroll1Buff.sFileHandle = -1;
    MldScroll1Buff.sIndexHandle = -1;

    return (MLD_SUCCESS);
}

#if DISPTYPE==2
/*
*===========================================================================
** Format  : SHORT MldDisplayInit(USHORT usDisplay)
*
*   Input  : USHORT     usDisplay   -display selection
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsys: clear display area and storage file for 2x20 display
*===========================================================================
*/
SHORT MldDisplayInit(USHORT usDisplay, USHORT usAttrib)
{
    /* clear storage data */

    if (usDisplay & MLD_SCROLL_1) {

        /* request semaphore */
        PifRequestSem(husMldSemaphore);

        /* initialize scorll #1 storage file */
        MldStorageInit(&MldScroll1Buff, uchMldSystem);

        /* release semaphore */
        PifReleaseSem(husMldSemaphore);

    }

    return (MLD_SUCCESS);

    usAttrib = usAttrib;
}
#endif

#if DISPTYPE==1
/*
*===========================================================================
** Format  : SHORT MldDisplayInit(USHORT usDisplay)
*
*   Input  : USHORT     usDisplay   -display selection
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsys: clear display area and storage file
*===========================================================================
*/
SHORT MldDisplayInit(USHORT usDisplay, USHORT usAttrib)
{
    SHORT sStatus;

    /* request semaphore */
    PifRequestSem(husMldSemaphore);

    /* modify argument to clear scroll #1, and/or scroll #2 */
    if (uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL) {
        if (usDisplay & MLD_SCROLL_1) {
            usDisplay = (MLD_SCROLL_1|MLD_SCROLL_2);
        } else {
            PifReleaseSem(husMldSemaphore);
            return (MLD_SUCCESS);
        }
    }

    sStatus = MldLocalDisplayInit(usDisplay, usAttrib);

    /* release semaphore */
    PifReleaseSem(husMldSemaphore);

    return (sStatus);
}

/*
*===========================================================================
** Format  : SHORT MldDisplayInit(USHORT usDisplay)
*
*   Input  : USHORT     usDisplay   -display selection
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsys: clear display area and storage file
*===========================================================================
*/
SHORT MldLocalDisplayInit(USHORT usDisplay, USHORT usAttrib)
{
    MLDCURTRNSCROL *pMldCurTrnScrol;
    USHORT i;
    UCHAR  uchAttr;
    TCHAR  szString[MLD_FMT_BUFF_LEN];    /* buffer for argument conversion */

    switch (uchMldSystem) {
		case MLD_SUPER_MODE:
			usDisplay &= MLD_SCROLL_1;
			break;

		case MLD_PRECHECK_SYSTEM:
		case MLD_DRIVE_THROUGH_1SCROLL:
			usDisplay &= (MLD_SCROLL_1|MLD_SCROLL_2);
			break;
    }

    /* window attribute, white or black */
    uchAttr = (UCHAR)(usAttrib ? MLD_C_WHITE : MLD_C_BLACK);

    if (usDisplay & MLD_SCROLL_1) {
		pMldCurTrnScrol = &MldScroll1Buff;
        if (usAttrib == 0) {
            VosCls (pMldCurTrnScrol->usWinHandle);
            pMldCurTrnScrol->usAttrib = 0;
        } else {
            VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);
            VosSetCurPos (pMldCurTrnScrol->usWinHandle, 0, 0);
            tcharnset (szString, 0x20, uchMldScrollColumn1);
            for (i = 0; i < pMldCurTrnScrol->usMldScrollLine; i++) {
                VosStringAttr (pMldCurTrnScrol->usWinHandle, szString, uchMldScrollColumn1, MLD_C_WHITE);
            }
            VosSetCurPos (pMldCurTrnScrol->usWinHandle, 0, 0);
            pMldCurTrnScrol->usAttrib = 1;

            if (uchMldCurScroll == MLD_SCROLL_1) {
                VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);
            }
        }

        /* initialize scroll #1 storage file */
        MldStorageInit (pMldCurTrnScrol, uchMldSystem);

        if ((uchMldSystem == MLD_PRECHECK_SYSTEM) || (uchMldSystem == MLD_DRIVE_THROUGH_1SCROLL)) {
            VosCls (MldOtherHandle1.usOrderHandle);  /* clear cursor window */
        }
    }

    if (usDisplay & MLD_SCROLL_2) {
		pMldCurTrnScrol = &MldScroll2Buff;
        if (usAttrib == 0) {
            VosCls (pMldCurTrnScrol->usWinHandle);
            pMldCurTrnScrol->usAttrib = 0;
        } else {
            VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);
            VosSetCurPos (pMldCurTrnScrol->usWinHandle, 0, 0);
            tcharnset(szString, 0x20, uchMldScrollColumn2);
            for (i = 0; i < pMldCurTrnScrol->usMldScrollLine; i++) {
                VosStringAttr (pMldCurTrnScrol->usWinHandle, szString, uchMldScrollColumn2, MLD_C_WHITE);
            }
            VosSetCurPos (pMldCurTrnScrol->usWinHandle, 0, 0);
            pMldCurTrnScrol->usAttrib = 1;

            if (uchMldCurScroll == MLD_SCROLL_2) {
                VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);
            }
        }
        memset (&(pMldCurTrnScrol->usStoVli), 0x00, sizeof(MLDCURTRNSCROL) - MLDSCROLLHEAD);

        /* initialize scroll #2 storage file */
        pMldCurTrnScrol->sFileHandle = -1;
        pMldCurTrnScrol->sIndexHandle = -1;
    }

    if (usDisplay & MLD_SCROLL_3) {
		pMldCurTrnScrol = &MldScroll3Buff;
        if (usAttrib == 0) {
            VosCls (pMldCurTrnScrol->usWinHandle);
            pMldCurTrnScrol->usAttrib = 0;
        } else {
            VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_INVISIBLE);
            VosSetCurPos (pMldCurTrnScrol->usWinHandle, 0, 0);
            tcharnset (szString, 0x20, uchMldScrollColumn3);
            for (i = 0; i < pMldCurTrnScrol->usMldScrollLine; i++) {
                VosStringAttr (pMldCurTrnScrol->usWinHandle, szString, uchMldScrollColumn3, MLD_C_WHITE);
            }
            VosSetCurPos (pMldCurTrnScrol->usWinHandle, 0, 0);
            pMldCurTrnScrol->usAttrib = 1;

            if (uchMldCurScroll == MLD_SCROLL_3) {
                VosSetCurType (pMldCurTrnScrol->usWinHandle, VOS_VISIBLE);
            }
        }
        memset (&(pMldCurTrnScrol->usStoVli), 0x00, sizeof(MLDCURTRNSCROL) - MLDSCROLLHEAD);

        /* initialize scroll #3 storage file */
        pMldCurTrnScrol->sFileHandle = -1;
        pMldCurTrnScrol->sIndexHandle = -1;
    }

    if (usDisplay & MLD_ORDER_1) {
        if (usAttrib == 0) {
            VosCls(MldOtherHandle1.usOrderHandle);
            MldOtherHandle1.usOrderAttrib = 0;
        } else {
            VosSetCurPos(MldOtherHandle1.usOrderHandle, 0, 0);
            tcharnset(szString, 0x20, uchMldScrollColumn1);
            VosStringAttr(MldOtherHandle1.usOrderHandle, szString, uchMldScrollColumn1, MLD_C_WHITE);
            MldOtherHandle1.usOrderAttrib = 1;
        }
    }

    if (usDisplay & MLD_ORDER_2) {
        if (usAttrib == 0) {
            VosCls(MldOtherHandle2.usOrderHandle);
            MldOtherHandle2.usOrderAttrib = 0;
        } else {
            VosSetCurPos(MldOtherHandle2.usOrderHandle, 0, 0);
            tcharnset(szString, 0x20, uchMldScrollColumn2);
            VosStringAttr(MldOtherHandle2.usOrderHandle, szString, uchMldScrollColumn2, MLD_C_WHITE);
            MldOtherHandle2.usOrderAttrib = 1;
        }
    }

    if (usDisplay & MLD_ORDER_3) {
        if (usAttrib == 0) {
            VosCls(MldOtherHandle3.usOrderHandle);
            MldOtherHandle3.usOrderAttrib = 0;
        } else {
            VosSetCurPos(MldOtherHandle3.usOrderHandle, 0, 0);
            tcharnset(szString, 0x20, uchMldScrollColumn3);
            VosStringAttr(MldOtherHandle3.usOrderHandle, szString, uchMldScrollColumn3, MLD_C_WHITE);
            MldOtherHandle3.usOrderAttrib = 1;
        }
    }

    if (usDisplay & MLD_TOTAL_1) {
        if (usAttrib == 0) {
            VosCls(MldOtherHandle1.usTotalHandle);
            MldOtherHandle1.usTotalAttrib = 0;
        } else {
            VosSetCurPos(MldOtherHandle1.usTotalHandle, 0, 0);
            tcharnset(szString, 0x20, uchMldScrollColumn1);
            VosStringAttr(MldOtherHandle1.usTotalHandle, szString, uchMldScrollColumn1, MLD_C_WHITE);
            MldOtherHandle1.usTotalAttrib = 1;
        }
    }

    if (usDisplay & MLD_TOTAL_2) {
        if (usAttrib == 0) {
            VosCls(MldOtherHandle2.usTotalHandle);
            MldOtherHandle2.usTotalAttrib = 0;
        } else {
            VosSetCurPos(MldOtherHandle2.usTotalHandle, 0, 0);
            tcharnset(szString, 0x20, uchMldScrollColumn2);
            VosStringAttr(MldOtherHandle2.usTotalHandle, szString, uchMldScrollColumn2, MLD_C_WHITE);
            MldOtherHandle2.usTotalAttrib = 1;
        }
    }

    if (usDisplay & MLD_TOTAL_3) {
        if (usAttrib == 0) {
            VosCls(MldOtherHandle3.usTotalHandle);
            MldOtherHandle3.usTotalAttrib = 0;
        } else {
            VosSetCurPos(MldOtherHandle3.usTotalHandle, 0, 0);
            tcharnset(szString, 0x20, uchMldScrollColumn3);
            VosStringAttr(MldOtherHandle3.usTotalHandle, szString, uchMldScrollColumn3, MLD_C_WHITE);
            MldOtherHandle3.usTotalAttrib = 1;
        }
    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT MldGetMldStatus(VOID)
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : TRUE
*
** Synopsys: show mld module is provided or not
*===========================================================================
*/
SHORT MldGetMldStatus()
{
    return(TRUE);
}

USHORT MldCreatePrechkScroll1A(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_REG103;

    VosCreateWindow(MLD_NOR_DSP1A_ROW_POS,  /* start row */
                    MLD_NOR_DSP1A_CLM_POS,  /* start colum */
                    MLD_NOR_DSP1A_ROW_LEN,  /* row length */
                    MLD_NOR_DSP1A_CLM_LEN,  /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_NONE,             /* boarder attribute */
                    &usHandle);             /* window handle */

    /* set normal status for cursor control */
    VosSetWinType(usHandle, VOS_WIN_NORMAL);

    return(usHandle);
}

USHORT MldCreatePrechkScroll1(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_REG100;

    VosCreateWindow(MLD_NOR_DSP1_ROW_POS,     /* start row */
                    MLD_NOR_DSP1_CLM_POS,   /* start colum */
                    MLD_NOR_DSP1_ROW_LEN,   /* row length */
                    MLD_NOR_DSP1_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_NONE,             /* boarder attribute */
                    &usHandle);             /* window handle */

    return(usHandle);
}

USHORT MldCreatePrechkScroll2(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_REG101;

    VosCreateWindow(MLD_NOR_DSP2_ROW_POS,     /* start row */
                    MLD_NOR_DSP2_CLM_POS,   /* start colum */
                    MLD_NOR_DSP2_ROW_LEN,   /* row length */
                    MLD_NOR_DSP2_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_LEFT|VOS_B_SINGLE, /* boarder attribute */
                    &usHandle);             /* window handle */

    /* set normal status for cursor control */
    VosSetWinType(usHandle, VOS_WIN_NORMAL);

    return(usHandle);
}

USHORT MldCreateDrive3Scroll1(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_REG200;

    VosCreateWindow(MLD_DRV3_DSP1_ROW_POS,     /* start row */
                    MLD_DRV3_DSP1_CLM_POS,   /* start colum */
                    MLD_DRV3_DSP1_ROW_LEN,   /* row length */
                    MLD_DRV3_DSP1_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_NONE,             /* boarder attribute */
                    &usHandle);             /* window handle */

    return(usHandle);
}

USHORT MldCreateDrive3Scroll2(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_REG201;

    VosCreateWindow(MLD_DRV3_DSP2_ROW_POS,     /* start row */
                    MLD_DRV3_DSP2_CLM_POS,   /* start colum */
                    MLD_DRV3_DSP2_ROW_LEN,   /* row length */
                    MLD_DRV3_DSP2_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_LEFT|VOS_B_SINGLE, /* boarder attribute */
                    &usHandle);             /* window handle */

    /* set normal status for cursor control */
    VosSetWinType(usHandle, VOS_WIN_NORMAL);

    return(usHandle);
}

USHORT MldCreateDrive3Scroll3(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_REG202;

    VosCreateWindow(MLD_DRV3_DSP3_ROW_POS,     /* start row */
                    MLD_DRV3_DSP3_CLM_POS,   /* start colum */
                    MLD_DRV3_DSP3_ROW_LEN,   /* row length */
                    MLD_DRV3_DSP3_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_LEFT|VOS_B_SINGLE, /* boarder attribute */
                    &usHandle);             /* window handle */

    /* set normal status for cursor control */
    VosSetWinType(usHandle, VOS_WIN_NORMAL);

    return(usHandle);
}

USHORT MldCreateSuperScroll1(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_SUPER300;

    VosCreateWindow(MLD_SUPER_MODE_ROW_POS,     /* start row */
                    MLD_SUPER_MODE_CLM_POS,   /* start colum */
                    MLD_SUPER_MODE_ROW_LEN,   /* row length */
                    MLD_SUPER_MODE_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_NONE,             /* boarder attribute */
                    &usHandle);            /* window handle */

    return(usHandle);
}

USHORT MldCreateOrder1(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_REG203;

    VosCreateWindow(MLD_DRV3_ORD1_ROW_POS,     /* start row */
                    MLD_DRV3_ORD1_CLM_POS,   /* start colum */
                    MLD_DRV3_ORD1_ROW_LEN,   /* row length */
                    MLD_DRV3_ORD1_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_NONE,             /* boarder attribute */
                    &usHandle);            /* window handle */

    /* set normal status for cursor control */
    VosSetWinType(usHandle, VOS_WIN_NORMAL);

    return(usHandle);
}

USHORT MldCreateOrder2(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_REG204;

    VosCreateWindow(MLD_DRV3_ORD2_ROW_POS,     /* start row */
                    MLD_DRV3_ORD2_CLM_POS,   /* start colum */
                    MLD_DRV3_ORD2_ROW_LEN,   /* row length */
                    MLD_DRV3_ORD2_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_LEFT|VOS_B_SINGLE, /* boarder attribute */
                    &usHandle);            /* window handle */

    /* set normal status for cursor control */
    VosSetWinType(usHandle, VOS_WIN_NORMAL);

    return(usHandle);
}

USHORT MldCreateOrder3(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_REG205;

    VosCreateWindow(MLD_DRV3_ORD3_ROW_POS,     /* start row */
                    MLD_DRV3_ORD3_CLM_POS,   /* start colum */
                    MLD_DRV3_ORD3_ROW_LEN,   /* row length */
                    MLD_DRV3_ORD3_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_LEFT|VOS_B_SINGLE, /* boarder attribute */
                    &usHandle);            /* window handle */

    /* set normal status for cursor control */
    VosSetWinType(usHandle, VOS_WIN_NORMAL);

    return(usHandle);
}

USHORT MldCreateTotal1(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_REG206;

    VosCreateWindow(MLD_DRV3_TTL1_ROW_POS,     /* start row */
                    MLD_DRV3_TTL1_CLM_POS,   /* start colum */
                    MLD_DRV3_TTL1_ROW_LEN,   /* row length */
                    MLD_DRV3_TTL1_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_NONE,             /* boarder attribute */
                    &usHandle);            /* window handle */

    /* set normal status for cursor control */
    VosSetWinType(usHandle, VOS_WIN_NORMAL);

    return(usHandle);
}

USHORT MldCreateTotal2(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_REG207;

    VosCreateWindow(MLD_DRV3_TTL2_ROW_POS,     /* start row */
                    MLD_DRV3_TTL2_CLM_POS,   /* start colum */
                    MLD_DRV3_TTL2_ROW_LEN,   /* row length */
                    MLD_DRV3_TTL2_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_LEFT|VOS_B_SINGLE, /* boarder attribute */
                    &usHandle);            /* window handle */

    /* set normal status for cursor control */
    VosSetWinType(usHandle, VOS_WIN_NORMAL);

    return(usHandle);
}

USHORT MldCreateTotal3(UCHAR uchAttr)
{
    USHORT usHandle = LCDWIN_ID_REG208;

    VosCreateWindow(MLD_DRV3_TTL3_ROW_POS,     /* start row */
                    MLD_DRV3_TTL3_CLM_POS,   /* start colum */
                    MLD_DRV3_TTL3_ROW_LEN,   /* row length */
                    MLD_DRV3_TTL3_CLM_LEN,   /* colum length */
                    VOS_FONT_NORMAL,        /* font attribute */
                    uchAttr,                /* charactor attribute */
                    VOS_B_LEFT|VOS_B_SINGLE, /* boarder attribute */
                    &usHandle);            /* window handle */

    /* set normal status for cursor control */
    VosSetWinType(usHandle, VOS_WIN_NORMAL);

    return(usHandle);
}
#endif

/**** End of File ****/
