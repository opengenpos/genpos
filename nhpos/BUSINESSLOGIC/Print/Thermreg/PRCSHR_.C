/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Item  Header trailer
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrCShr_.C
* --------------------------------------------------------------------------
* Abstract:
*      PrtShrInitReverse() : initialize shared print data buffer for reverse func.
*      PrtShrInit() : initialize shared print data buffer
*      PrtShrPrint(): sets shared print data in the buffer
*      PrtShrEnd(): send final print data to shared printer module
*      PrtShrSupSend(): send super/prog print data to shared printer module
*      PrtShrRegSend(): send reg print data to shared printer module
*      PrtShrPartialSend(): send super print data partially to shared printer module
*      PrtShrWriteRetry(): execute retry process
*      PrtShrRetryPrc(): execute retry process depending on the return value
*      PrtShrAltrRetry(): executes retry process ( in alternation )
*      PrtShrAltr(): retry to send print data toward alternative printer from the 1st frame
*      PrtCheckShr(): check if shared printer system or not
*      PrtChkFrame(): checks frame status and return expected success value
*      PrtCheckAbort(): check abort key entered or not when printer trouble
*      PrtChangeFont(): changes font size of thermal printer
*      PrtShrTermLock(): makes terminal lock (used by maintenance module)
*      PrtShrLockSub(): execute terminal lock retry process
*      PrtShrTermLock(): makes terminal unlock (used by maintenance module)
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jun-21-93 : 01.00.13 :  R.Itoh    : initial
* Aug-25-93 : 01.00.13 :  K.You     : add PrtShrInitReverse().
* Sep-07-93 : 02.00.00 :  K.You     : bug fixed E-85 (mod. PrtShrInitReverse())
* Sep-08-93 : 02.00.00 :  K.You     : bug fixed E-87 (mod. PrtShrInitReverse())
* Apr-21-94 : 02.00.00 :  M.Suzuki  : add PrtShrWriteFirstRetry(),PrtShrFirstRetryPrc()
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

/**
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <pif.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <regstrct.h>
#include <transact.h>
#include <uie.h>
#include <fsc.h>
#include <csstbstb.h>
#include <csstbpar.h>
#include <csstbshr.h>
#include <pmg.h>
#include <prt.h>
#include <shr.h>
#include "prtcom.h"
#include "prtrin.h"
#include "prtshr.h"


static UCHAR   uchPrtTrgTerm = 0;             /* target terminal w/ shared printer */

/*
*===========================================================================
** Format  : VOID  PrtShrInitReverse( VOID );
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets shared print initial data for reverse func.
*===========================================================================
*/
VOID    PrtShrInitReverse( VOID )
{

    TRANINFORMATION TransWork;

    TransWork.TranCurQual.ulStoreregNo = 0L;
    TransWork.TranCurQual.usConsNo = 0;

    fbPrtShrStatus |= PRT_SHARED_SYSTEM;
    PrtShrInit(&TransWork);
}

/*
*===========================================================================
** Format  : VOID  PrtShrInit(TRANINFORMATION  *pTran);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets shared print initial data.
*===========================================================================
*/
VOID    PrtShrInit(TRANINFORMATION  *pTran)
{

    /* -- set terminal unique address -- */
    PrtShrInf.HdInf.uchTermAdr = CliReadUAddr();

    /* -- set consecutive No. -- */
    uchPrtReceiptCo ++;
    PrtShrInf.HdInf.usConsNo = pTran->TranCurQual.usConsNo + uchPrtReceiptCo;

    /* -- set frame name -- */
    PrtShrInf.HdInf.fbFrame = SHR_FIRST_FRAME;

    /* -- set sequence No. -- */
    PrtShrInf.HdInf.uchSeqNo = 1;

    /* -- set current data length -- */
    PrtShrInf.HdInf.usILI = sizeof(PrtShrInf.HdInf);

    /* -- initialize print data area -- */
    memset(PrtShrInf.auchPrtData, '\0', sizeof(PrtShrInf.auchPrtData));
    usPrtShrOffset = 0;
}

/*
*===========================================================================
** Format  : VOID  PrtShrPrint(UCHAR *pszBuff);
*
*   Input  : UCHAR  *pszBuff     -print data buffer pointer
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function sets shared print data in the buffer.
**
**			 The buffer is stored within the global variable 'PrtShrInf'.
**			 This variable is a structure with two parts, the header
**			 information and the buffer to store the data within.
**			 Within the header information is a variable 'usILI' which contains
**			 the number of bytes that are contained within the buffer
**			 area of this structure 'auchPrtData'. The total size of the
**			 global structure is SHR_FRAME_SIZE. Where SHR_FRAME_SIZE
**			 is a combination of bytes and characters. The bytes (7)
**		     represent the header size 'SHRHEADERINF'. The characters (700)
**			 represent the number of characters that can be written
**			 within the buffer.
**
**			 This function will check to make sure that there is still space
**			 within the buffer by checking to see if the number of bytes passed
**			 in + the header information + the data already within the buffer
**			 will exceed the max size. The 'usPrtShrOffset' variable within
**			 this function will state how many characters are within the buffer.
*===========================================================================
*/
VOID    PrtShrPrint(TCHAR *pszBuff, USHORT  usLen)
{
	SHORT  sRetStatus;

    /* -- return, if once abort key entered or go to alternation later -- */
    if ( ( fbPrtAbortStatus & PRT_ABORTED )
            || ( fbPrtAltrStatus & PRT_WRITE_ABORT ) ) {
        return;
    }

	if (usLen < 1)  // ignore any print lines with zero characters
		return;

    /* -- check whether print buffer full or not -- */
    if ( (PrtShrInf.HdInf.usILI + usLen + 1) > sizeof(PrtShrInf) ) {
        /* -- for super mode -- */
        if ( fbPrtLockChk & PRT_SUPPRG_MODE )
		{
            sRetStatus = PrtShrSupSend();
		}
		else
		{
            sRetStatus = PrtShrRegSend( SHR_TIMERON );
		}
        if ( PRT_SUCCESS == sRetStatus )
		{
            /* set frame name */
            PrtShrInf.HdInf.fbFrame = SHR_CONTINUE_FRAME;
            /* set sequence No. */
            PrtShrInf.HdInf.uchSeqNo ++;
            /* reset current data length */
            PrtShrInf.HdInf.usILI = sizeof(PrtShrInf.HdInf);
            /* reset print data offset */
            usPrtShrOffset = 0;
        }
		else
		{
            /* retry aborted, or alternation */
            fbPrtAbortStatus |= PRT_ABORTED;
            return;
        }
    }

	/*
		Put the print line into the frame for the shared printer output.
		A frame can contain multiple print lines.  The lines are appended
		one after the other as the line length in bytes followed by
		the characters of the print line.
	 */
    PrtShrInf.auchPrtData[usPrtShrOffset] = (UCHAR)usLen;
	usPrtShrOffset++;
    PrtShrInf.HdInf.usILI ++;
	memcpy(&PrtShrInf.auchPrtData[usPrtShrOffset], pszBuff, usLen);
	usPrtShrOffset += usLen;
    PrtShrInf.HdInf.usILI += usLen;
}

/*
*===========================================================================
** Format  : VOID  PrtShrEnd(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function send final print data to shared printer module.
*===========================================================================
*/
VOID    PrtShrEnd(VOID)
{
    if ( fbPrtShrStatus & PRT_SHARED_SYSTEM ) {

        if ( ( fbPrtAbortStatus & PRT_ABORTED )
                || ( fbPrtAltrStatus & PRT_WRITE_ABORT ) ) {

            if ( fbPrtAltrStatus & PRT_TOTAL_STUB ) {
                fbPrtAbortStatus = fbPrtAbortSav = 0;
                fbPrtAltrStatus = fbPrtAltrSav = 0;
                fbPrtShrStatus = 0;
            }

            return;
        }

        /* -- for super mode -- */
        if ( fbPrtLockChk & PRT_SUPPRG_MODE ) {

            if ( PRT_SUCCESS == PrtShrSupSend() ) {

                PrtShrInf.HdInf.uchSeqNo ++;
                                    /* set sequence No. */
                PrtShrInf.HdInf.usILI = sizeof(PrtShrInf.HdInf);
                                    /* reset current data length */
                usPrtShrOffset = 0;
                                    /* reset print data offset */
            } else {

                fbPrtAbortStatus |= PRT_ABORTED;
                                    /* retry aborted */
            }

        /* -- for reg mode -- */
        } else {

            /* -- set end frame -- */
            PrtShrInf.HdInf.fbFrame |= SHR_END_FRAME;

            if ( PRT_SUCCESS != PrtShrRegSend( SHR_PERFORM ) ) {
                fbPrtAbortStatus |= PRT_ABORTED;
            }

            if ( fbPrtAltrStatus & PRT_TOTAL_STUB ) {
                fbPrtAbortStatus = fbPrtAbortSav = 0;
                fbPrtAltrStatus = fbPrtAltrSav = 0;
                fbPrtShrStatus = 0;
            }

        }
    }
}

/*
*===========================================================================
** Format  : SHORT  PrtShrSupSend(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : SHORT      PRT_SUCCESS     - send success
*                       PRT_ABORTED     - retry aborted
*
** Synopsis: This function send super/prog print data to shared printer module.
*===========================================================================
*/
SHORT    PrtShrSupSend(VOID)
{
    SHORT   sRet;

    /* -- if printer error occured at Prog#50, not print data -- */
    if ( fbPrtShrPrint == PRT_SHR_NOT_PRINT ) {
        return ( PRT_SUCCESS );
    }

    /* -- set end frame -- */
    PrtShrInf.HdInf.fbFrame |= SHR_END_FRAME;

    /* -- write success ? -- */
    sRet = CliShrPrint( uchPrtTrgTerm, &PrtShrInf, PrtShrInf.HdInf.usILI );
    if ( sRet == SHR_TIMERON ) {
        return ( PrtShrWriteRetry( LDT_PRTTBL_ADR, SHR_TIMERON, sRet ) );
    }

    return ( PRT_SUCCESS );
}

/*
*===========================================================================
** Format  : SHORT  PrtShrRegSend(SHORT  sSuccess);
*
*   Input  : SHORT  sSuccess        -return value in case of success
*                                     SHR_TIMERON
*                                     SHR_PERFORM
*   Output : none
*   InOut  : none
** Return  : SHORT  PRT_SUCCESS     - send success
*                   PRT_ABORTED     - retry aborted
*
** Synopsis: This function send reg print data to shared printer module.
*===========================================================================
*/
SHORT    PrtShrRegSend(SHORT sSuccess)
{
    SHORT   sRet;
    UCHAR   fbPrtAltrStatusSave1, fbPrtAltrStatusSave2;
    UCHAR   uchPrtTrgTermSave1, uchPrtTrgTermSave2;

    /* -- send print data -- */
    sRet = CliShrPrint( uchPrtTrgTerm, &PrtShrInf,  PrtShrInf.HdInf.usILI );

    if ( sRet == sSuccess ) {
        if (sRet == SHR_PERFORM) {
            fbPrtAltrStatus &= ~PRT_BUFFERING_ALTR; /* reset alternation flag */
        }
        return ( PRT_SUCCESS );
    }

    /*** in case of 'buffer full' or 'busy' ***/

    /* -- during alternation ? -- */
    if ( fbPrtAltrStatus & PRT_NEED_ALTR ) {
        /* -- retry to send print data -- */
        return ( PrtShrWriteRetry_Reg( LDT_PRTTBL_ADR, PrtChkFrame(), sRet ) );
    }

    fbPrtAltrStatusSave1 = fbPrtAltrStatus;     /* save alternation flag */
    uchPrtTrgTermSave1 = uchPrtTrgTerm;         /* save target address */

    /* -- in case of not during alternation print -- */

    if ( 0 == PrtCheckShr(PRT_WITHALTR) ) {     /* alternation printer set ?  */
        /* -- retry to send print data -- */
        return ( PrtShrWriteRetry_Reg( LDT_PRTTBL_ADR, PrtChkFrame(), sRet ) );
    }

    fbPrtAltrStatusSave2 = fbPrtAltrStatus;     /* save alternation flag */
    uchPrtTrgTermSave2 = uchPrtTrgTerm;         /* save alternation target address */
    fbPrtAltrStatus = fbPrtAltrStatusSave1;     /* restore alternation flag */
    uchPrtTrgTerm = uchPrtTrgTermSave1;         /* restore target address */

    if ( PrtShrWriteFirstRetry( sSuccess, sRet ) == PRT_SUCCESS) {
        if ( sSuccess == SHR_PERFORM ) {
            fbPrtAltrStatus &= ~PRT_BUFFERING_ALTR; /* reset alternation flag */
        }
        return ( PRT_SUCCESS );
    }

    fbPrtAltrStatus = fbPrtAltrStatusSave2;     /* restore alternation flag */
    uchPrtTrgTerm = uchPrtTrgTermSave2;         /* restore alternation target address */

    /* -- display "alternation execution" -- */
    if ( FSC_CANCEL == UieErrorMsg( LDT_SHRALT_ADR, UIE_ABORT ) ) {

        /* -- alternation canceled and stop printing -- */
        fbPrtAltrStatus &= ~( PRT_NEED_ALTR | PRT_BUFFERING_ALTR );
        return ( PRT_ABORTED );
    }

    /* -- ticket item ? -- */
    if ( (SHR_PERFORM == PrtChkFrame()) && (PrtShrInf.HdInf.uchSeqNo == 1) ) {

        /* -- retry to alternation terminal -- */
        return ( PrtShrAltrRetry( LDT_PRTTBL_ADR, SHR_PERFORM, sRet ) );

    /* -- stop setting print data (later, re-set for alternative printer) -- */
    } else {

        fbPrtAltrStatus |= PRT_WRITE_ABORT;
        return ( PRT_SUCCESS );
    }

}

/*
*===========================================================================
** Format  : VOID  PrtShrPartialSend(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function send super print data partially to shared printer
*            module.
*===========================================================================
*/
VOID    PrtShrPartialSend(USHORT usPrintControl)
{
    if ( fbPrtShrStatus & PRT_SHARED_SYSTEM ) {
        /* -- shared print already canceled ? -- */
        if ( fbPrtAbortStatus & PRT_ABORTED ) {
            return;
        }

        /* -- for super mode -- */
        if ( fbPrtLockChk & PRT_SUPPRG_MODE ) {
            /* -- send success ? -- */
            if ( PRT_SUCCESS == PrtShrSupSend() ) {
                PrtShrInf.HdInf.uchSeqNo ++;                       /* set sequence No. */
                PrtShrInf.HdInf.usILI = sizeof(PrtShrInf.HdInf);   /* reset current data length */
                usPrtShrOffset = 0;                                /* reset print data offset */
            /* -- send not success and Abort/P5 key entered -- */
            } else {
                fbPrtAbortStatus |= PRT_ABORTED;    /* retry aborted */
            }
        }
    }
}

/*
*===========================================================================
** Format  : SHORT  PrtShrWriteRetry(USHORT usError, SHORT sSuccess, SHORT sRet);
*
*   Input  : USHORT     usError         - lead through address
*            SHORT      sSuccess        - return value in case of success
*            SHORT      sRet            - current return value
*   Output : none
*   InOut  : none
** Return  : SHORT      sReturn
*                           PRT_SUCCESS     - send successfully
*                           PRT_ABORTED     - retry aborted
*
** Synopsis: This function execute retry process.
*===========================================================================
*/
SHORT  PrtShrWriteRetry(USHORT  usError, SHORT  sSuccess, SHORT sRet)
{
    SHORT   sReturn, i;

    /* -- in case of 'buffer full' -- */
    if ( sRet == SHR_BUFFER_FULL ) {
        /* in case of the terminal with shared printer */
        if ( uchPrtTrgTerm == PrtShrInf.HdInf.uchTermAdr ) {
            for (;;) {
                sReturn = PrtShrRetryPrc( usError, sSuccess );
                if ( sReturn == PRT_CONT || sReturn == PRT_CONT_COND ) {
                    continue;
                } else {
                    return( sReturn );
                }
            }
        }
    }

	// Retry the print operation several times if needed.
	// If after several retries, we still haven't been able
	// to print then abort the print.
	sReturn = PRT_SUCCESS;

    for ( i = 0; i < PRT_SHR_RET_CO; i ++ ) {
        sReturn = PrtShrRetryPrc ( usError, sSuccess );
        if ( sReturn == PRT_CONT || sReturn == PRT_CONT_COND ) {
			sReturn = PRT_ABORTED;  // aborted return status in case we drop through
            continue;
        } else {
            break;
        }
    }

	return sReturn;
}

/*
*===========================================================================
** Format  : SHORT  PrtShrWriteRetry_Reg(USHORT usError, SHORT sSuccess, SHORT sRet);
*
*   Input  : USHORT     usError         - lead through address
*            SHORT      sSuccess        - return value in case of success
*            SHORT      sRet            - current return value
*   Output : none
*   InOut  : none
** Return  : SHORT      sReturn
*                       PRT_SUCCESS     - send successfully
*                       PRT_ABORTED     - retry aborted
*
** Synopsis: This function calls "PrtShrWriteRetry()" ,
*              and resets "fbPrtAltrStatus".
*===========================================================================
*/
SHORT  PrtShrWriteRetry_Reg(USHORT  usError, SHORT  sSuccess, SHORT sRet)
{
    SHORT sR;

    sR = PrtShrWriteRetry( usError, sSuccess, sRet );

    if ( (sR == PRT_SUCCESS) && (sSuccess == SHR_PERFORM) ) {
        fbPrtAltrStatus &= ~PRT_BUFFERING_ALTR; /* reset alternation flag */
    }

    if ( sR == PRT_ABORTED ) {
        fbPrtAltrStatus &= ~( PRT_NEED_ALTR | PRT_BUFFERING_ALTR );
                                                /* reset alternation flag */
    }

    return (sR);

}


/*
*===========================================================================
** Format  : SHORT  PrtShrRetryPrc(USHORT usError, SHORT sSuccess);
*
*   Input  : USHORT     usError         - lead through address
*            SHORT      sSuccess        - return value in case of success
*   Output : none
*   InOut  : none
** Return  : SHORT      PRT_SUCCESS     - send successfully
*                       PRT_ABORTED     - retry aborted
*                       PRT_CONT        - retry continue
*
** Synopsis: This function execute retry process depending on the return value
*            from CliShrPrint().
*                  -sSuccess: send successfully
*                  - SHR_BUFFER_FULL: sleep and retry
*                  - SHR_SHR_BUSY: display E05 error, clear->retry, abort/P5->abort
*===========================================================================
*/
SHORT  PrtShrRetryPrc(USHORT  usError, SHORT  sSuccess)
{
    SHORT   sReturn, sFsc;
    USHORT  usPrevious;

    /* -- write success ? -- */
    sReturn = CliShrPrint( uchPrtTrgTerm, &PrtShrInf,
                                    PrtShrInf.HdInf.usILI );

    if ( sReturn == sSuccess ) {

        return( PRT_SUCCESS );

    } else if ( sReturn == SHR_BUFFER_FULL ) {

        PifSleep( PRT_SHR_SLEEP_TIME );
        return( PRT_CONT );

    } else if ( (sReturn == SHR_BUFFER_PRINTER_ERR) || (sReturn == SHR_TIME_OUT) ) {
        /* allow power switch at error display */
        usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
        /* -- display error code and mnemonics -- */
        sFsc = UieErrorMsg( usError, UIE_ABORT );
        if ( ( sFsc == FSC_CANCEL ) || ( sFsc == FSC_P5 ) ) {
            PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
            return( PRT_ABORTED );
        }

        PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
        return( PRT_CONT_COND );
    } else {

        /* allow power switch at error display */
        usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
        /* -- display error code and mnemonics -- */
        sFsc = UieErrorMsg( usError, UIE_ABORT );
        if ( ( sFsc == FSC_CANCEL ) || ( sFsc == FSC_P5 ) ) {
            PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
            return( PRT_ABORTED );
        }

        PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
        return( PRT_CONT );
    }
}

/*
*===========================================================================
** Format  : SHORT  PrtShrAltrRetry(USHORT usError, SHORT sSuccess, SHORT sRet);
*
*  Input   : USHORT     usError         - lead through address
*            SHORT      sSuccess        - return value in case of success
*            SHORT      sRet            - current return value
*   Output : none
*   InOut  : none
** Return  : SHORT      PRT_SUCCESS     - send successfully
*                       PRT_ABORTED     - retry aborted
*
** Synopsis: This function executes retry process ( in alternation ).
*            if send not success, retry toward alternative printer.
*===========================================================================
*/
SHORT  PrtShrAltrRetry(USHORT  usError, SHORT  sSuccess, SHORT sRet)
{
	SHORT  sRetStatus = PRT_SUCCESS;

    /* -- write success ? -- */
    if ( sSuccess != CliShrPrint( uchPrtTrgTerm, &PrtShrInf,
                                    PrtShrInf.HdInf.usILI ) ) {

        sRetStatus = PrtShrWriteRetry_Reg(usError, sSuccess, sRet );
        if ( PRT_SUCCESS != sRetStatus ) {

            return( PRT_ABORTED );
        }

    }

    if ( sSuccess == SHR_PERFORM ) {
        fbPrtAltrStatus &= ~PRT_BUFFERING_ALTR; /* reset alternation flag */
    }

    return( PRT_SUCCESS );
}

/*
*===========================================================================
** Format  : VOID  PrtShrAltr(TRANINFORMATION  *pTran, SHORT hsStorage);
*
*   Input  : TRANINFORMATION  *pTran     -Transaction information address
*            SHORT            hspStorage    - storage file handle
*   Output : none
*   InOut  : none
** Return  : none
**
** Synopsis: This function retry to send print data toward alternative
*            printer from the 1st frame.
*===========================================================================
*/
VOID    PrtShrAltr(TRANINFORMATION  *pTran, SHORT hsStorage)
{

    fsPrtStatus = 0;                            /* clear status */

    fsPrtCompul = PRT_RECEIPT;                  /* receipt compulsory */

    fsPrtNoPrtMask =  PRT_RECEIPT;              /* not print except receipt */

    PrtPortion(0);                              /* set print portion to         */
                                                /* static area "fsPrtPrintPort" */
    /* -- decompress data and call IPD  -- */
    PrtCallIPD(pTran, hsStorage);

}

/*
*===========================================================================
** Format  : UCHAR  PrtCheckShr(UCHAR   uchTrgPrt);
*
*   Input  : UCHAR  uchTrgPrt    -target printer
*                     - PRT_WITHSHR     go to shared printer
*                     - PRT_WITHALTR    go to alternative printer
*
*   Output : none
*   InOut  : none
** Return  : UCHAR  ParaBuffShr.uchTermNo:   -target terminal address
**
*
** Synopsis: This function gets target terminal address and returns it.
*             if '0', normal printer system.
*===========================================================================
*/
UCHAR    PrtCheckShr(UCHAR  uchTrgPrt)
{
    USHORT  usTrgAddr;              /* target terminal address */
    USHORT  usUAddr;                /* termnal unique address */
    USHORT   i;
    PARASHAREDPRT   ParaBuffShr;    /* to get terminal address */

    /* -- set terminal unique address -- */
    usUAddr = (USHORT)CliReadUAddr();

    /* get target terminal address -- */
    for ( i = usTrgAddr = 1; i <= PRT_SHR_MAX_TERM; i++, usTrgAddr+=2 ) {

        if ( i == usUAddr ) {
            break;
        }
    }

    /* -- check destination printer -- */
    if ( uchTrgPrt == PRT_WITHALTR ) {
        usTrgAddr += 1;             /* get alternative printer# */
    }

    /* get target terminal No. */
/*    memset(&ParaBuffShr, '\0', sizeof(PARASHAREDPRT)); */
    ParaBuffShr.uchMajorClass = CLASS_PARASHRPRT;
    ParaBuffShr.uchAddress = (UCHAR)usTrgAddr;
    CliParaRead(&ParaBuffShr);

    if ( ParaBuffShr.uchTermNo != 0 ) {
        uchPrtTrgTerm = ParaBuffShr.uchTermNo;

        if ( uchTrgPrt == PRT_WITHALTR ) {
            fbPrtAltrStatus |= (PRT_NEED_ALTR | PRT_BUFFERING_ALTR);
        }                               /* go to alternative printer */
    }

    return(ParaBuffShr.uchTermNo);
}

/*
*===========================================================================
** Format  : UCHAR  PrtChkFrame(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : SHR_TIMERON: success value when SHR_END_FRAME is not on.
*            SHR_PERFORM: success value when SHR_END_FRAME is on.
*
** Synopsis: This function checks frame status and return expected success value.
*===========================================================================
*/
UCHAR    PrtChkFrame(VOID)
{
    if ( PrtShrInf.HdInf.fbFrame & SHR_END_FRAME ) {
        return ( SHR_PERFORM );
    } else {
        return ( SHR_TIMERON );
    }
}

/*
*===========================================================================
** Format  : SHORT  PrtCheckAbort(VOID);
*
*   InPut  : none
*   Output : none
*   InOut  : none
** Return  : UCHAR  PRT_ABORTED:   -abort key entered when error occured
*                             0:   -abort key not entered
*
** Synopsis: This function check abort key entered or not when printer trouble.
*
*===========================================================================
*/
SHORT  PrtCheckAbort(VOID)
{

    if ( fbPrtAbortStatus & PRT_ABORTED ) {

        return ( PRT_ABORTED );

    } else {

        return ( 0 );

    }
}



/*
*===========================================================================
** Format  : UCHAR  PrtAltrMsg(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : FSC_CLEAR:         clear key entered
*            FSC_CALCEL/FSC_P5: abort key entered
*
** Synopsis: This function displays alternation message and returns key entered.
*===========================================================================
*/
UCHAR    PrtAltrMsg(VOID)
{

    /* -- display "alternation execution" -- */
    return ( UieErrorMsg( LDT_SHRALT_ADR, UIE_ABORT ) );

}


/*
*===========================================================================
*   VOID     PrtChangeFont(USHORT usType);
*
*   Input  : PRT_FONT_A                         - 12X24   defined in "pmg.h"
*            PRT_FONT_B                         -  9X 7   defined in "pmg.h"
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis: This function changes font size of thermal printer.
*===========================================================================
*/
VOID PrtChangeFont(USHORT usType)
{
    SHRFONTMSG  FontMsg;

    FontMsg.uchHead1 = SHR_FONT_MSG_1;
    FontMsg.uchHead2 = SHR_FONT_MSG_2;
    FontMsg.usType   = usType;

    if ( fbPrtShrStatus & PRT_SHARED_SYSTEM ) {

        PrtShrPrint((USHORT *)&FontMsg, sizeof(FontMsg));

    }  else {

        PmgFont(PMG_PRT_RECEIPT, usType);

    }
}



/*
*===========================================================================
** Synopsis:    SHORT PrtShrTermLock(UCHART uchPrtStatus)
*
*     Input:    uchPrtStatus :
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS      : success terminal lock
*               FSC_CANCEL   : stop retry for lock
*               FSC_P5       : stop retry for lock
*
** Description:   This fuction makes terminal lock (used by maintenance module)
*===========================================================================
*/

SHORT PrtShrTermLock(UCHAR uchPrtStatus)
{
    UCHAR    uchKeyMsg;
    SHORT    sReturn;
    TRANINFORMATION  Tran;

    /* initialize not print flag */
    fbPrtShrPrint = 0;

    /* get terminal number with shared printer */
    if ( 0 == PrtCheckShr(PRT_WITHSHR) ) {
        return(SUCCESS);                        /* not shared printer */
    }

    fbPrtShrStatus |= PRT_SHARED_SYSTEM;        /* set shared status */

    Tran.TranCurQual.ulStoreregNo = 0L;
    Tran.TranCurQual.usConsNo = 0;
    PrtShrInit(&Tran);                          /* initialize shared printer buffer */

    if ((sReturn = CliShrTermLock(uchPrtTrgTerm)) == SHR_TIMERON) {
                                                /* terminal lock */
        fbPrtLockChk |= (PRT_SHR_LOCK | PRT_SUPPRG_MODE);
        return(SUCCESS);
    }

    /* -- in case of terminal lock not success -- */
    if ( 0 == PrtCheckShr(PRT_WITHALTR) ) {     /* if not exist alternation printer */
        return (PrtShrLockSub(uchPrtStatus));   /* retry */
    }

    /* -- alternation print  -- */
    uchKeyMsg    = PrtAltrMsg();                /* display "print alternation" and return key */

    if (uchKeyMsg == FSC_CANCEL || uchKeyMsg == FSC_P5) {
        return(uchKeyMsg);                      /* interrupt by user */
    }

    if ((sReturn = CliShrTermLock(uchPrtTrgTerm)) == SHR_TIMERON) {
                                                /* terminal lock */
        fbPrtLockChk |= (PRT_SHR_LOCK | PRT_SUPPRG_MODE);
        return(SUCCESS);
    }

    return (PrtShrLockSub(uchPrtStatus));
}


/*
*===========================================================================
** Synopsis:    SHORT  PrtShrLockSub(UCHAR uchPrtStatus)
*
*     Input:    uchPrtStatus :
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS      : success terminal lock
*               FSC_CANCEL   : stop retry for lock
*               FSC_P5       : stop retry for lock
*
** Description: This function execute terminal lock retry process.
*                   clear key: retry
*                   abort/P5 key: abort to make terminal lock.
*===========================================================================
*/
SHORT  PrtShrLockSub(UCHAR uchPrtStatus)
{
    UCHAR uchKeyMsg;
    USHORT usPrevious;
	SHORT	sReturn;

    /* allow power switch at error display */
    usPrevious = PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
	sReturn = SUCCESS;

    for (;;) {

        /* allow power switch at error display */
        PifControlPowerSwitch(PIF_POWER_SWITCH_ENABLE); /* V1.0.04 */
        uchKeyMsg = UieErrorMsg(LDT_PRTTBL_ADR, UIE_ABORT);

        if ( (uchPrtStatus & MAINT_NOT_PRINT) && (uchKeyMsg == FSC_CLEAR) ) {
            fbPrtLockChk |= PRT_SUPPRG_MODE;
            fbPrtShrPrint = PRT_SHR_NOT_PRINT;
            PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
            return(SUCCESS);
        } else if ( sReturn == SHR_TIME_OUT) { //SR481
			fbPrtLockChk |= PRT_SUPPRG_MODE;
			fbPrtShrPrint = PRT_SHR_NOT_PRINT;
            PifControlPowerSwitch(usPrevious);
			return(SUCCESS);
		} else if (sReturn == SHR_ILLEGAL) {
			fbPrtLockChk |= PRT_SUPPRG_MODE;
			fbPrtShrPrint = PRT_SHR_NOT_PRINT;
            PifControlPowerSwitch(usPrevious);
			return(SUCCESS);
		}

        if (uchKeyMsg == FSC_CANCEL || uchKeyMsg == FSC_P5) {
            PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
            return(uchKeyMsg);                 /* interrupt by user */
        }

		sReturn = CliShrTermLock(uchPrtTrgTerm);

        if ( sReturn == SHR_TIMERON ) {
            fbPrtLockChk |= (PRT_SHR_LOCK | PRT_SUPPRG_MODE);
            PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
            return(SUCCESS);
        }

    }
    PifControlPowerSwitch(usPrevious);  /* V1.0.04 */
}


/*
*===========================================================================
** Synopsis:    VOID PrtShrTermUnLock( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description:   Unlock Terminal.
*===========================================================================
*/

VOID PrtShrTermUnLock( VOID )
{

    /* check if printer is shared printer */
    if ( !(fbPrtShrStatus & PRT_SHARED_SYSTEM) ) {
        return;
    }

    /* terminal unlock */
    if (fbPrtLockChk & PRT_SHR_LOCK) {
        CliShrTermUnLock(uchPrtTrgTerm);
    }

    /* initialize shared/normal, aborted flag */
    fbPrtShrStatus = 0;
    fbPrtAbortStatus = 0;

    /* initialize not print flag */
    fbPrtShrPrint = 0;

    /* initialize lock/alternation flag */
    fbPrtLockChk = 0;
    fbPrtAltrStatus = 0;

    /* header not printed */
    fbPrtTHHead = 0;

}

/*
*===========================================================================
** Format  : UCHAR	PrtSetPrintMode(VOID);
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : UCHAR
*
** Synopsis: This function sets the print mode.to file on terminal or
*			 printer
*===========================================================================
*/ //ESMITH PRTFILE
VOID	PrtSetPrintMode(UCHAR uchMode)
{
	if ((uchMode & PRT_SENDTO_MASKFUNC) ||
		(uchMode == PRT_SENDTO_PRINTER)) {

			fbPrtSendToStatus = uchMode;
	}
}

/*
*===========================================================================
** Format  : SHORT  PrtShrWriteFirstRetry(SHORT sSuccess, SHORT sRet);
*
*   Input  : SHORT      sSuccess        - return value in case of success
*            SHORT      sRet            - current return value
*   Output : none
*   InOut  : none
** Return  : SHORT      sReturn
*                           PRT_SUCCESS     - send successfully
*                           SHR_BUSY        - error
*
** Synopsis: This function execute first retry process.
*===========================================================================
*/
SHORT  PrtShrWriteFirstRetry(SHORT  sSuccess, SHORT sRet)
{
    SHORT   sReturn, i;

    /* -- in case of 'buffer full' -- */
    if ( sRet == SHR_BUFFER_FULL ) {

        /* in case of the terminal with shared printer */
        if ( uchPrtTrgTerm == PrtShrInf.HdInf.uchTermAdr ) {

            for (;;) {

                sReturn = PrtShrFirstRetryPrc( sSuccess );
                if ( sReturn == PRT_CONT ) {
                    continue;

                } else {
                    return( sReturn );
                }
            }

        /* in case of the terminal without printer */
        } else {

            for ( i = 0; i < PRT_SHR_RET_CO; i ++ ) {

                sReturn = PrtShrFirstRetryPrc( sSuccess );
                if ( sReturn == PRT_CONT ) {
                    continue;

                } else {
                    return( sReturn );
                }
            }
        }
    }
    return( sRet );
}

/*
*===========================================================================
** Format  : SHORT  PrtShrFirstRetryPrc(SHORT sSuccess);
*
*   Input  : SHORT      sSuccess        - return value in case of success
*   Output : none
*   InOut  : none
** Return  : SHORT      PRT_SUCCESS     - send successfully
*                       PRT_CONT        - retry continue
*                       SHR_BUSY        - shared printer busy
*
** Synopsis: This function execute retry process depending on the return value
*            from CliShrPrint().
*                  -sSuccess: send successfully
*                  - SHR_BUFFER_FULL: sleep and retry
*                  - SHR_BUSY: error
*===========================================================================
*/
SHORT  PrtShrFirstRetryPrc( SHORT  sSuccess)
{
    SHORT   sReturn;

    /* -- write success ? -- */
    sReturn = CliShrPrint( uchPrtTrgTerm, &PrtShrInf,
                                    PrtShrInf.HdInf.usILI );

    if ( sReturn == sSuccess ) {

        return( PRT_SUCCESS );

    } else if ( sReturn == SHR_BUFFER_FULL ) {

        PifSleep( PRT_SHR_SLEEP_TIME );
        return( PRT_CONT );

    } else {

        return( sReturn );
    }
}
/****** End Of Source ******/

