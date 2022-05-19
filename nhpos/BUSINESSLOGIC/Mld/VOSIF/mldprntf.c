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
* Title       : Multiline Display Printf Functions
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDPRNTF.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*         MldPrintfAttr(): display strings with attribute selection
*             MldPrintf(): display strings 
*         MldStringAttr(): display strings with attribute selection
*             MldString(): display strings 
*  MldStringAttrSpecial(): dispaly stirngs with attribute selection
*          MldScrollUp() : scroll up display
*        MldScrollDown() : scroll down display
*        MldScrollClear(): clear scroll display
*            MldSPrintf(): create display format and returns no. of lines
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
#include "termcfg.h"
/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/

/*
*===========================================================================
** Format  : SHORT MldPrintfAttr(UCHAR usScroll, SHORT sRow, UCHAR uchAttr,
*                       CONST UCHAR FAR *pszFormat, ...)
*
*   Input  : UCHAR  usScroll    -display selection
*            SHORT  sRow        -display position
*            UCHAR  uchAttr     -display attribute
*            CONST UCHAR FAR *pszFormat -display format
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: create display format and display on the area which is selected
*===========================================================================
*/
SHORT    MldPrintfAttr(USHORT usScroll, SHORT sRow, UCHAR uchAttr,
                        CONST TCHAR FAR *pszFormat, ...)
{
    USHORT usRow;
    USHORT usRowFlag = 1;
    TCHAR  szString[MLD_FMT_BUFF_LEN];    /* buffer for argument conversion */
    TCHAR  szString2[MLD_FMT_BUFF_LEN2];  /* buffer for tab and cr control  */
    USHORT  usLength;

    /* --- reset cursor position --- */
    switch (sRow) {
    case MLD_SCROLL_APPEND:
        usRowFlag = 0;
        break;
    case MLD_SCROLL_TOP:
        usRow = 0;
        break;
    case MLD_SCROLL_END:
        if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
            usRow = MLD_DRV3_DSP1_ROW_LEN-1;
        } else {
            usRow = MLD_NOR_DSP1_ROW_LEN-1;
        }
        break;
    default:
        usRow = (USHORT)sRow;
        break;
    }

    /* --- argument conversion --- */
    usLength = _RflFormStr(pszFormat,
                  (SHORT *)( (UCHAR *)&pszFormat + sizeof(pszFormat) ),
                   szString,
                   MLD_FMT_BUFF_LEN);//(USHORT)sizeof(szString));

    /* --- display on scroll #1 --- */
    if (usScroll & MLD_SCROLL_1) {

        /* tab and carrage return conversion */
        usLength = RflStrAdjust(szString2, MLD_FMT_BUFF_LEN2/*sizeof(szString2)*/, szString,
                        uchMldScrollColumn1, RFL_FEED_ON);

        if (usRowFlag) {
            VosSetCurPos(MldScroll1Buff.usWinHandle, usRow, 0);
        }
        VosStringAttr(MldScroll1Buff.usWinHandle, szString2, usLength, uchAttr);

        MldScroll1Buff.uchStatus |= MLD_ALREADY_WRITED;
    }

    /* --- display on scroll #2 --- */
    if (usScroll & MLD_SCROLL_2) {
        /* tab and carrage return conversion */
        usLength = RflStrAdjust(szString2, MLD_FMT_BUFF_LEN2/*sizeof(szString2)*/, szString,
                        uchMldScrollColumn2, RFL_FEED_ON);

        if (usRowFlag) {
            VosSetCurPos(MldScroll2Buff.usWinHandle, usRow,0);
        }
        VosStringAttr(MldScroll2Buff.usWinHandle, szString2, usLength, uchAttr);

        MldScroll2Buff.uchStatus |= MLD_ALREADY_WRITED;
    }

    /* --- display on scroll #3 --- */
    if (usScroll & MLD_SCROLL_3) {
        /* tab and carrage return conversion */
        usLength = RflStrAdjust(szString2, MLD_FMT_BUFF_LEN2/*sizeof(szString2)*/, szString,
                        uchMldScrollColumn3, RFL_FEED_ON);

        if (usRowFlag) {
            VosSetCurPos(MldScroll3Buff.usWinHandle, usRow,0);
        }
        VosStringAttr(MldScroll3Buff.usWinHandle, szString, usLength, uchAttr);

        MldScroll3Buff.uchStatus |= MLD_ALREADY_WRITED;
    }

    return(MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT MldPrintf(UCHAR usScroll, SHORT sRow,
*                       CONST UCHAR FAR *pszFormat, ...)
*
*   Input  : UCHAR  usScroll    -display selection
*            SHORT  sRow        -display position
*            CONST UCHAR FAR *pszFormat -display format
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: create display format and display on the area which is selected
*===========================================================================
*/
SHORT    MldPrintf(USHORT usScroll, SHORT sRow, CONST TCHAR FAR *pszFormat, ...)
{
    USHORT usRow= 0;
    USHORT usRowFlag = 1;
    TCHAR  szString[MLD_FMT_BUFF_LEN];    /* buffer for argument conversion */
    TCHAR  szString2[MLD_FMT_BUFF_LEN2];  /* buffer for tab and cr control  */
    USHORT usLength = 0;
    UCHAR   uchAttr = 0;
//	char aszErrorBuffer[128];	

	memset(szString, 0x00, sizeof(szString));
	memset(szString2, 0x00, sizeof(szString2));


    /* --- reset cursor position --- */
    switch (sRow) {
    case MLD_SCROLL_APPEND:
        usRowFlag = 0;
        break;
    case MLD_SCROLL_TOP:
        usRow = 0;
        break;
    case MLD_SCROLL_END:
        if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
            usRow = MLD_DRV3_DSP1_ROW_LEN-1;
        } else {
            usRow = MLD_NOR_DSP1_ROW_LEN-1;
        }
        break;
    default:
        usRow = (USHORT)sRow;
        break;
    }

    /* --- argument conversion --- */
    usLength = _RflFormStr(pszFormat,
                  (SHORT *)( (UCHAR *)&pszFormat + sizeof(pszFormat) ),
                   szString,
                   MLD_FMT_BUFF_LEN/*sizeof(szString)*/);

    /* --- display on scroll #1 --- */
    if (usScroll & MLD_SCROLL_1) {

        /* tab and carrage return conversion */
        usLength = RflStrAdjust(szString2, MLD_FMT_BUFF_LEN2/*sizeof(szString2)*/, szString,
                        uchMldScrollColumn1, RFL_FEED_ON);

        if (usRowFlag) {
            VosSetCurPos(MldScroll1Buff.usWinHandle, usRow,0);
        }
        uchAttr = (UCHAR)(MldScroll1Buff.usAttrib ? MLD_C_WHITE : MLD_C_BLACK);
        VosStringAttr(MldScroll1Buff.usWinHandle, szString2, usLength, uchAttr);

        MldScroll1Buff.uchStatus |= MLD_ALREADY_WRITED;
    }

    /* --- display on scroll #2 --- */
    if (usScroll & MLD_SCROLL_2) {
        /* tab and carrage return conversion */
        usLength = RflStrAdjust(szString2, MLD_FMT_BUFF_LEN2/*sizeof(szString2)*/, szString,
                        uchMldScrollColumn2, RFL_FEED_ON);

        if (usRowFlag) {
            VosSetCurPos(MldScroll2Buff.usWinHandle, usRow,0);
        }
        uchAttr = (UCHAR)(MldScroll2Buff.usAttrib ? MLD_C_WHITE : MLD_C_BLACK);
		VosStringAttr(MldScroll2Buff.usWinHandle, szString2, usLength, uchAttr);


        MldScroll2Buff.uchStatus |= MLD_ALREADY_WRITED;
    }

    /* --- display on scroll #3 --- */
    if (usScroll & MLD_SCROLL_3) {

        /* tab and carrage return conversion */
        usLength = RflStrAdjust(szString2, MLD_FMT_BUFF_LEN2/*sizeof(szString2)*/, szString,
                        uchMldScrollColumn3, RFL_FEED_ON);

        if (usRowFlag) {
            VosSetCurPos(MldScroll3Buff.usWinHandle, usRow,0);
        }
        uchAttr = (UCHAR)(MldScroll3Buff.usAttrib ? MLD_C_WHITE : MLD_C_BLACK);
        VosStringAttr(MldScroll3Buff.usWinHandle, szString2, usLength, uchAttr);

        MldScroll3Buff.uchStatus |= MLD_ALREADY_WRITED;
    }

    return(MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT MldStringAttr(UCHAR usScroll, SHORT sRow, UCHAR uchAttr,
*                       UCHAR *pszString, USHORT usLength);
*
*   Input  : UCHAR  usScroll    -display selection
*            SHORT  sRow        -display position
*            UCHAR  uchAttr     -display attribute
*            UCHAR  *pszString  -character string
*            USHORT usLength    -length of *pszString
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: display characters on the area which is selected
*===========================================================================
*/
SHORT    MldStringAttr(USHORT usScroll, SHORT sRow, UCHAR uchAttr,
                        TCHAR *pszString, USHORT usLength)
{
    USHORT usRow;
    USHORT usRowFlag = 1;

    /* --- reset cursor position --- */
    switch (sRow) {
    case MLD_SCROLL_APPEND:
        usRowFlag = 0;
        break;
    case MLD_SCROLL_TOP:
        usRow = 0;
        break;
    case MLD_SCROLL_END:
        if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
            usRow = MLD_DRV3_DSP1_ROW_LEN-1;
        } else {
            usRow = MLD_NOR_DSP1_ROW_LEN-1;
        }
        break;
    default:
        usRow = (USHORT)sRow;
        break;
    }

    /* --- display on scroll #1 --- */
    if (usScroll & MLD_SCROLL_1) {
        if (usRowFlag) {
            VosSetCurPos(MldScroll1Buff.usWinHandle, usRow,0);
        }
        VosStringAttr(MldScroll1Buff.usWinHandle, pszString, usLength, uchAttr);

        MldScroll1Buff.uchStatus |= MLD_ALREADY_WRITED;
    }

    /* --- display on scroll #2 --- */
    if (usScroll & MLD_SCROLL_2) {
        if (usRowFlag) {
            VosSetCurPos(MldScroll2Buff.usWinHandle, usRow,0);
        }
        VosStringAttr(MldScroll2Buff.usWinHandle, pszString, usLength, uchAttr);

        MldScroll2Buff.uchStatus |= MLD_ALREADY_WRITED;
    }

    /* --- display on scroll #3 --- */
    if (usScroll & MLD_SCROLL_3) {
        if (usRowFlag) {
            VosSetCurPos(MldScroll3Buff.usWinHandle, usRow,0);
        }
        VosStringAttr(MldScroll3Buff.usWinHandle, pszString, usLength, uchAttr);

        MldScroll3Buff.uchStatus |= MLD_ALREADY_WRITED;
    }

    return(MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT MldString(UCHAR usScroll, SHORT sRow, UCHAR uchAttr,
*                       UCHAR *pszString, USHORT usLength);
*
*   Input  : UCHAR  usScroll    -display selection
*            SHORT  sRow        -display position
*            UCHAR  *pszString  -character string
*            USHORT usLength    -length of *pszString
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: display characters on the area which is selected
*===========================================================================
*/
SHORT    MldString(USHORT usScroll, SHORT sRow, TCHAR *pszString, USHORT usLength)
{
    USHORT usRow;
    USHORT usRowFlag = 1;
    UCHAR   uchAttr;

    /* --- reset cursor position --- */
    switch (sRow) {
    case MLD_SCROLL_APPEND:
        usRowFlag = 0;
        break;
    case MLD_SCROLL_TOP:
        usRow = 0;
        break;
    case MLD_SCROLL_END:
        if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
            usRow = MLD_DRV3_DSP1_ROW_LEN-1;
        } else {
            usRow = MLD_NOR_DSP1_ROW_LEN-1;
        }
        break;
    default:
        usRow = (USHORT)sRow;
        break;
    }

    /* --- display on scroll #1 --- */
    if (usScroll & MLD_SCROLL_1) {
        if (usRowFlag) {
            VosSetCurPos(MldScroll1Buff.usWinHandle, usRow,0);
        }
        uchAttr = (UCHAR)(MldScroll1Buff.usAttrib ? MLD_C_WHITE : MLD_C_BLACK);
        VosStringAttr(MldScroll1Buff.usWinHandle, pszString, usLength, uchAttr);

        MldScroll1Buff.uchStatus |= MLD_ALREADY_WRITED;
    }

    /* --- display on scroll #2 --- */
    if (usScroll & MLD_SCROLL_2) {
        if (usRowFlag) {
            VosSetCurPos(MldScroll2Buff.usWinHandle, usRow,0);
        }
        uchAttr = (UCHAR)(MldScroll2Buff.usAttrib ? MLD_C_WHITE : MLD_C_BLACK);
        VosStringAttr(MldScroll2Buff.usWinHandle, pszString, usLength, uchAttr);

        MldScroll2Buff.uchStatus |= MLD_ALREADY_WRITED;
    }

    /* --- display on scroll #3 --- */
    if (usScroll & MLD_SCROLL_3) {
        if (usRowFlag) {
            VosSetCurPos(MldScroll3Buff.usWinHandle, usRow,0);
        }
        uchAttr = (UCHAR)(MldScroll3Buff.usAttrib ? MLD_C_WHITE : MLD_C_BLACK);
        VosStringAttr(MldScroll3Buff.usWinHandle, pszString, usLength, uchAttr);

        MldScroll3Buff.uchStatus |= MLD_ALREADY_WRITED;
    }

    return(MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT MldStringAttrSpecial(UCHAR usScroll, SHORT sRow, UCHAR uchAttr,
*                       UCHAR *pszString, USHORT usLength, LONG lAttr);
*
*   Input  : UCHAR  usScroll    -display selection
*            SHORT  sRow        -display position
*            UCHAR  *pszString  -character string
*            USHORT usLength    -length of *pszString
*            LONG   lAttr       -reverse video attribute
*               exp. 0x0000000000001111 displays the former 4 char. reverse video
*               and other displays normal video.
*
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: display characters on the area which is directed the reverse video.
*===========================================================================
*/
SHORT    MldStringAttrSpecial(USHORT usScroll, SHORT sRow, TCHAR *pszString,
                                USHORT usLength, LONG lAttrib)
{
    USHORT  usRow, usI, usIs;
    USHORT  usRowFlag = 1;
	MLDCURTRNSCROL *mldCurTrn[] = {&MldScroll1Buff, &MldScroll2Buff, &MldScroll3Buff};

    /* --- reset cursor position --- */
    switch (sRow) {
    case MLD_SCROLL_APPEND:
        usRowFlag = 0;
        break;
    case MLD_SCROLL_TOP:
        usRow = 0;
        break;
    case MLD_SCROLL_END:
        if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
            usRow = MLD_DRV3_DSP1_ROW_LEN-1;
        } else {
            usRow = MLD_NOR_DSP1_ROW_LEN-1;
        }
        break;

	case MLD_SCROLL_RETRIEVE:
		VosStringEdit(MldScroll1Buff.usWinHandle, pszString, usLength, 1);
		return MLD_SUCCESS;

	case MLD_SCROLL_REPLACE:
		VosStringEdit(MldScroll1Buff.usWinHandle, pszString, usLength, 2);
		return MLD_SUCCESS;
	case MLD_SCROLL_BEGIN:
		VosStringEdit(MldScroll1Buff.usWinHandle, pszString, usLength, 3);
	case MLD_SCROLL_BEGIN_ADD:
		VosStringEdit(MldScroll1Buff.usWinHandle, pszString, usLength, 4);
		return MLD_SUCCESS;

    default:
        usRow = (USHORT)sRow;
        break;
    }

#if 1
	for (usI = 0, usIs = MLD_SCROLL_1; usIs <= MLD_SCROLL_3; usI++, usIs <<= 1) {
		/* --- display on scroll #1 --- */
		if (usScroll & usIs) {
			USHORT     i, j;
			D13DIGITS  d13ProdWork;   /* prepare bit shift for optimization */
			LONG       lCheckAttrib;
			UCHAR      uchAttr = 0;

			if (usRowFlag) {
				VosSetCurPos(mldCurTrn[usI]->usWinHandle, usRow,0);
			}

			if (mldCurTrn[usI]->usAttrib) {  /* if background is reversed */
				lAttrib = ~lAttrib;
			}
			/* check reverse video control flag */
			for(i = 0, lCheckAttrib = 0x00000001L; i < usLength; pszString+=j) {
			   //uchAttr = (UCHAR)(lAttrib & lCheckAttrib ? MLD_C_WHITE : MLD_C_BLACK); //colorpalette-changes
				for (i++, j = 1; i < usLength; i++, j++) {
					d13ProdWork = lCheckAttrib * 2LL;          /* calcurate LONG x LONG data */
					lCheckAttrib = (LONG)d13ProdWork;
					if (uchAttr != (UCHAR)(lAttrib & lCheckAttrib ? MLD_C_WHITE : MLD_C_BLACK)) break;
				}
				uchAttr = (UCHAR)lAttrib; // colorpalette-changes
				VosStringAttr(mldCurTrn[usI]->usWinHandle, pszString, j, uchAttr);
			}
			mldCurTrn[usI]->uchStatus |= MLD_ALREADY_WRITED;

			if (mldCurTrn[usI]->usAttrib) {  /* if background is reversed */
				lAttrib = ~lAttrib;
			}
		}
	}
#else
    /* --- display on scroll #1 --- */
    if (usScroll & MLD_SCROLL_1) {
        if (usRowFlag) {
            VosSetCurPos(MldScroll1Buff.usWinHandle, usRow,0);
        }

        if (MldScroll1Buff.usAttrib) {  /* if background is reversed */
            lAttrib = ~lAttrib;
        }
        /* check reverse video control flag */
        for(i=0,lCheckAttrib=0x00000001L; i<usLength; pszString+=j) {
           //uchAttr = (UCHAR)(lAttrib & lCheckAttrib ? MLD_C_WHITE : MLD_C_BLACK); //colorpalette-changes
            for (i++,j=1; i<usLength; i++,j++) {
                RflMul32by32(&huProdWork, lCheckAttrib, 2L);
                                            /* calcurate LONG x LOND data */
                lCheckAttrib = *((LONG *) (&huProdWork));
                if (uchAttr != (UCHAR)(lAttrib & lCheckAttrib ? MLD_C_WHITE : MLD_C_BLACK)) break;
            }
			uchAttr = (UCHAR)lAttrib; // colorpalette-changes
            VosStringAttr(MldScroll1Buff.usWinHandle, pszString, j, uchAttr);
        }
        MldScroll1Buff.uchStatus |= MLD_ALREADY_WRITED;

        if (MldScroll1Buff.usAttrib) {  /* if background is reversed */
            lAttrib = ~lAttrib;
        }
    }

    /* --- display on scroll #2 --- */
    if (usScroll & MLD_SCROLL_2) {
        if (usRowFlag) {
            VosSetCurPos(MldScroll2Buff.usWinHandle, usRow,0);
        }

        if (MldScroll2Buff.usAttrib) {  /* if background is reversed */
            lAttrib = ~lAttrib;
        }
        /* check reverse video control flag */
        for(i=0,lCheckAttrib=0x00000001L; i<usLength; pszString+=j) {
            //uchAttr = (UCHAR)(lAttrib & lCheckAttrib ? MLD_C_WHITE : MLD_C_BLACK); //colorpalette-changes-needed
            for (i++,j=1; i<usLength; i++,j++) {
                RflMul32by32(&huProdWork, lCheckAttrib, 2L);
                                            /* calcurate LONG x LOND data */
                lCheckAttrib = *((LONG *) (&huProdWork));
                if (uchAttr != (UCHAR)(lAttrib & lCheckAttrib ? MLD_C_WHITE : MLD_C_BLACK)) break;
            }
			uchAttr = (UCHAR)lAttrib; // colorpalette-changes
            VosStringAttr(MldScroll2Buff.usWinHandle, pszString, j, uchAttr);
        }
        MldScroll2Buff.uchStatus |= MLD_ALREADY_WRITED;

        if (MldScroll2Buff.usAttrib) {  /* if background is reversed */
            lAttrib = ~lAttrib;
        }
    }

    /* --- display on scroll #3 --- */
    if (usScroll & MLD_SCROLL_3) {
        if (usRowFlag) {
            VosSetCurPos(MldScroll3Buff.usWinHandle, usRow,0);
        }

        if (MldScroll3Buff.usAttrib) {  /* if background is reversed */
            lAttrib = ~lAttrib;
        }
        /* check reverse video control flag */
        for(i=0,lCheckAttrib=0x00000001L; i<usLength; pszString+=j) {
            //uchAttr = (UCHAR)(lAttrib & lCheckAttrib ? MLD_C_WHITE : MLD_C_BLACK); //colorpalette-changes-needed
            for (i++,j=1; i<usLength; i++,j++) {
                RflMul32by32(&huProdWork, lCheckAttrib, 2L);
                                            /* calcurate LONG x LOND data */
                lCheckAttrib = *((LONG *) (&huProdWork));
                if (uchAttr != (UCHAR)(lAttrib & lCheckAttrib ? MLD_C_WHITE : MLD_C_BLACK)) break;
            }
			uchAttr = (UCHAR)lAttrib; // colorpalette-changes
            VosStringAttr(MldScroll3Buff.usWinHandle, pszString, j, uchAttr);
        }
        MldScroll3Buff.uchStatus |= MLD_ALREADY_WRITED;

        if (MldScroll3Buff.usAttrib) {  /* if background is reversed */
            lAttrib = ~lAttrib;
        }
    }
#endif

    return(MLD_SUCCESS);
}


/*
*===========================================================================
** Format  : SHORT MldStringAttrSpecialColumn(UCHAR usScroll, SHORT sRow, UCHAR uchAttr,
*                       UCHAR *pszString, USHORT usLength, LONG *plAttr);
*
*   Input  : UCHAR  usScroll    -display selection
*            SHORT  sRow        -display position
*            UCHAR  *pszString  -character string
*            USHORT usLength    -length of *pszString
*            LONG   lAttr       -reverse video attribute
*               exp. 0x0000000000001111 displays the former 4 char. reverse video
*               and other displays normal video.
*
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: display characters on the area which is directed the reverse video.
*===========================================================================
*/
SHORT    MldStringAttrSpecialColumn(USHORT usScroll, SHORT sRow, TCHAR *pszString,
                                USHORT usLength, LONG *plAttrib)
{
    USHORT usRow;
    USHORT usRowFlag = 1;

    /* --- reset cursor position --- */
    switch (sRow) {
    case MLD_SCROLL_APPEND:
        usRowFlag = 0;
        break;
    case MLD_SCROLL_TOP:
        usRow = 0;
        break;
    case MLD_SCROLL_END:
        if (uchMldSystem == MLD_DRIVE_THROUGH_3SCROLL) {
            usRow = MLD_DRV3_DSP1_ROW_LEN-1;
        } else {
            usRow = MLD_NOR_DSP1_ROW_LEN-1;
        }
        break;

	case MLD_SCROLL_RETRIEVE:
		VosStringEdit(MldScroll1Buff.usWinHandle, pszString, usLength, 1);
		return MLD_SUCCESS;

	case MLD_SCROLL_REPLACE:
		VosStringEdit(MldScroll1Buff.usWinHandle, pszString, usLength, 2);
		return MLD_SUCCESS;

    default:
        usRow = (USHORT)sRow;
        break;
    }

    /* --- display on scroll #1 --- */
    if (usScroll & MLD_SCROLL_1) {
        if (usRowFlag) {
            VosSetCurPos(MldScroll1Buff.usWinHandle, usRow,0);
        }

        VosStringAttrColumn(MldScroll1Buff.usWinHandle, pszString, usLength, plAttrib);

        MldScroll1Buff.uchStatus |= MLD_ALREADY_WRITED;

        //if (MldScroll1Buff.usAttrib) {  /* if background is reversed */
		//	plAttrib = ~plAttrib;
        //}
    }

    /* --- display on scroll #2 --- */
    if (usScroll & MLD_SCROLL_2) {
        if (usRowFlag) {
            VosSetCurPos(MldScroll2Buff.usWinHandle, usRow,0);
        }

		VosStringAttrColumn(MldScroll2Buff.usWinHandle, pszString, usLength, plAttrib);
        MldScroll2Buff.uchStatus |= MLD_ALREADY_WRITED;

        //if (MldScroll2Buff.usAttrib) {  /* if background is reversed */
        //	plAttrib = ~plAttrib;
        //}
    }

    /* --- display on scroll #3 --- */
    if (usScroll & MLD_SCROLL_3) {
        if (usRowFlag) {
            VosSetCurPos(MldScroll3Buff.usWinHandle, usRow,0);
        }

		VosStringAttrColumn(MldScroll3Buff.usWinHandle, pszString, usLength/*j*/, plAttrib);
		MldScroll3Buff.uchStatus |= MLD_ALREADY_WRITED;

        //if (MldScroll3Buff.usAttrib) {  /* if background is reversed */
		//	plAttrib = ~plAttrib;
        //}
    }

    return(MLD_SUCCESS);
}




/*
*===========================================================================
** Format  : SHORT MldScrollUp(UCHAR usScroll, USHORT usNumRow);
*
*   Input  : UCHAR  usScroll    -display selection
*            USHORT usNumRow    -number of scroll line
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: scroll up display area whitch is selected
*===========================================================================
*/
SHORT MldScrollUp(USHORT usScroll, USHORT usNumRow)
{
    /* --- display on scroll #1 --- */
    if (usScroll & MLD_SCROLL_1) {
        VosScrollUp(MldScroll1Buff.usWinHandle, usNumRow);
    }

    /* --- display on scroll #2 --- */
    if (usScroll & MLD_SCROLL_2) {
        VosScrollUp(MldScroll2Buff.usWinHandle, usNumRow);
    }

    /* --- display on scroll #3 --- */
    if (usScroll & MLD_SCROLL_3) {
        VosScrollUp(MldScroll3Buff.usWinHandle, usNumRow);
    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT MldScrollDown(UCHAR usScroll, USHORT usNumRow);
*
*   Input  : UCHAR  usScroll    -display selection
*            USHORT usNumRow    -number of scroll line
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: scroll up display area whitch is selected
*===========================================================================
*/
SHORT MldScrollDown(USHORT usScroll, USHORT usNumRow)
{
    /* --- display on scroll #1 --- */
    if (usScroll & MLD_SCROLL_1) {
        VosScrollDown(MldScroll1Buff.usWinHandle, usNumRow);
    }

    /* --- display on scroll #2 --- */
    if (usScroll & MLD_SCROLL_2) {
        VosScrollDown(MldScroll2Buff.usWinHandle, usNumRow);
    }

    /* --- display on scroll #3 --- */
    if (usScroll & MLD_SCROLL_3) {
        VosScrollDown(MldScroll3Buff.usWinHandle, usNumRow);
    }

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT MldScrollClear(UCHAR usScroll);
*
*   Input  : UCHAR  usScroll    -display selection
*   Output : none
*   InOut  : none
** Return  : MLD_SUCCESS
*
** Synopsis: clear display area whitch is selected
*===========================================================================
*/
SHORT MldScrollClear (USHORT usScroll)
{
    MLDCURTRNSCROL *pMldCurTrnScrol[] = {&MldScroll1Buff, &MldScroll2Buff, &MldScroll3Buff};
    TCHAR   szString[MLD_FMT_BUFF_LEN];    /* buffer for argument conversion */
    USHORT  i, iScroll;
	USHORT  usScrollType[] = {MLD_SCROLL_1, MLD_SCROLL_2, MLD_SCROLL_3};

	for (iScroll = 0; iScroll < (sizeof(usScrollType)/sizeof(usScrollType[0])); iScroll++) {
		if (usScroll & usScrollType[iScroll]) {
			if (pMldCurTrnScrol[iScroll]->uchStatus & MLD_ALREADY_WRITED) {
				if (pMldCurTrnScrol[iScroll]->usAttrib == 0) {
					VosCls(pMldCurTrnScrol[iScroll]->usWinHandle);
				} else {
					VosSetCurType(pMldCurTrnScrol[iScroll]->usWinHandle, VOS_INVISIBLE);
					VosSetCurPos(pMldCurTrnScrol[iScroll]->usWinHandle, 0, 0);
					tcharnset(szString, 0x20, uchMldScrollColumn1);
					for (i = 0; i < pMldCurTrnScrol[iScroll]->usMldScrollLine; i++) {
						VosStringAttr (pMldCurTrnScrol[iScroll]->usWinHandle, 
								szString, pMldCurTrnScrol[iScroll]->usMldScrollColumn, MLD_C_WHITE);
					}
					VosSetCurPos (pMldCurTrnScrol[iScroll]->usWinHandle, 0, 0);
					if (uchMldCurScroll == usScrollType[iScroll]) {
						VosSetCurType (pMldCurTrnScrol[iScroll]->usWinHandle, VOS_VISIBLE);
					}
				}
			}
			pMldCurTrnScrol[iScroll]->uchStatus &= ~MLD_ALREADY_WRITED;
		}
	}

    return (MLD_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT MldSPrintf(UCHAR *pszDest, UCHAR uchDestLen,
*                       CONST UCHAR FAR *pszFormat, ...)
*
*   Input  : UCHAR  *pszDest    -destination buffer pointer
*            UCHAR  uchDestLen  -length of format area
*            CONST UCHAR FAR *pszFormat -display format
*   Output : none
*   InOut  : none
** Return  : number of lines which contains formatted data
*
** Synopsis: create display format and return no of lines
*===========================================================================
*/
USHORT  MldSPrintf(TCHAR *pszDest, UCHAR uchDestLen, const TCHAR FAR *pszFormat, ...)
{
    USHORT usLines=0, i, j;
    TCHAR  szString[MLD_FMT_BUFF_LEN];    /* buffer for argument conversion */
    TCHAR  szString2[MLD_FMT_BUFF_LEN2];  /* buffer for tab and cr control  */
    USHORT usLength;
    TCHAR   *pWork, *pWork2;

    /* --- argument conversion --- */
    _RflFormStr(pszFormat,
                  (SHORT *)( (UCHAR *)&pszFormat + sizeof(pszFormat) ),
                   szString,
                   MLD_FMT_BUFF_LEN);//(USHORT)sizeof(szString));

    /* ---- tab conversion ---- */
    usLength = RflStrAdjust(szString2, MLD_FMT_BUFF_LEN2/*sizeof(szString2)*/, szString,
                        uchDestLen, RFL_FEED_ON);

    /* ---- carrage return conversion and set number of lines ---- */
    pWork = &szString2[0];
    pWork2 = &szString2[0];
    for (i=0; i<usLength; ) {
        for (j=0; j<(USHORT)uchDestLen; i++,j++) {
            if (*pWork == '\n') {
                break;
            } else if (*pWork == '\0') {
                break;
            } else {
                pWork++;
            }
        }

        if ((j == 1) && ((*pWork == '\0') || (*pWork == '\n')) ) break;

        //memset (pszDest, 0x00, (USHORT)(uchDestLen+1) * sizeof(TCHAR));
        tcharnset(pszDest, 0x20, uchDestLen);
		pszDest[uchDestLen] = 0x00;
		_tcsncpy(pszDest, pWork2, j);  /* copy to destination buffer */
        usLines++;

        if (*pWork == '\0') {
            break;
        } else {
            pWork++; i++; j++;   /* skip CR */
            pWork2 += j;
            pszDest += (USHORT)uchDestLen+1;    /* 1 char space between 2 lines */
        }
    }
    return(usLines);
}

/*** End of File ***/
