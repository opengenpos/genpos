/*
*=============================================================================
* Title       : Display on the Fly Main                 
* Category    : Display on the Fly Manager, NCR 2170 US HOSP Appl Enh.
* Program Name: Dfl.C                                                        
* ----------------------------------------------------------------------------
* Abstract:   This file contains following functions:
*              DflInit()
*               -DflGetPort() 
*              DflPrint ()
*               -DflCheckTermAdr() 
*               -DflAtoI() 
*               -DflPrevDataCopyCheck() 
*               -DflWriteSpool() 
*-----------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name       : Description
* Jul-05-93 : 00.00.01 : NAKAJIMA,Ken : Init.                                   
* Aug-19-98 : 03.03.00 : M.Ozawa      : Change auchDflSpool to FAR
*
** NCR2171 **                                         
* Aug-26-99 : 01.00.00 : M.Teraki     :initial (for 2171)
*=============================================================================
*=============================================================================
* PVCS Entry
* ----------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*=============================================================================
*/

/**
;=============================================================================
;+                  I N C L U D E     F I L E s                              +
;=============================================================================
**/
/**------- MS - C ------**/
#include <tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include "ecr.h"
#include "pif.h"
#include "appllog.h"
#include "dfl.h"
#include "rfl.h"
#include "dfllocal.h"

/*
==============================================================================
+                  S T A T I C   R A M s                                     +
==============================================================================
*/
/* -- semaphore handle -- */
USHORT usDflSemStartDSP;        /* semaphore handle for start send thread */
USHORT usDflSemRWSpool;         /* semaphore handle for access to spool buffer */
USHORT usDflSemStubFunc;        /* semaphore handle for stub function */

/* -- spool buffer & pointer for access -- */
UCHAR  auchDflSpool[DFL_SPOOL_SIZE]; /* spool buffer, V3.3 */
USHORT usDflReadP;              /* spool buffer read pointer */
USHORT usDflWriteP;             /* spool buffer write pointer */
USHORT usDflBuffEnd;            /* spool buffer end offset */

DFLDATAHEAD DflPrevData[DFL_MAX_TERMNO];
                                /* store previous data for each terminal */

DFLDATAHEAD DflCurData;         /* current data */

UCHAR  fbDflControl;            /* control flag */
UCHAR  fbDflSpoolFull;          /* spool full flag */

CHAR   chDflPortID;             /* number of port connected disp. fly */

SHORT  hsDflPort;               /* port handle */

/**
;=============================================================================
;+              P R O G R A M    D E C L A R A T I O N s                     +
;=============================================================================
**/
/*
*=============================================================================
** Format  : VOID   DflInit(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function is called from application startup routine.
*            starts send thread, make semaphore.
*=============================================================================
*/
VOID   DflInit(VOID)
{
	CHAR  CONST aszDflThreadName[] = "DISP_FLY";         /* thread name */
	VOID (THREADENTRY *pFunc)(VOID) = DflSendProcess;    /* Add R3.0 */

    /* -- create semaphore -- */
    usDflSemStartDSP = PifCreateSem(0);         /* for start send thread */
    usDflSemRWSpool  = PifCreateSem(1);         /* for access to spool buffer */
    usDflSemStubFunc = PifCreateSem(1);         /* for access to stub function */

    /* -- set port number to "chDflPortID" -- */
    DflGetPort();

    if (chDflPortID == DFL_PORT_NONE) {         
        return ;
    }

    /* -- start send thread -- */
    PifBeginThread(pFunc, 0, 0, PRIO_DISP_FLY, aszDflThreadName);
}


/*
*=============================================================================
** Format  : VOID   DflGetPort(VOID)
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function sets port id to static area.
*=============================================================================
*/
VOID   DflGetPort(VOID)
{
    SYSCONFIG CONST FAR *pSys;
    USHORT i;

    pSys = PifSysConfig();                      /* get system configration */

    chDflPortID = DFL_PORT_NONE;                /* set "not exist bord or port ID" */

    if(!(pSys->uchCom & COM_PROVIDED)) {       /* exist RS-232 bord ? */
        return ;
    }

    for (i=1; i < PIF_LEN_PORT; i++) {          /* get port ID */
                                                /* auchComPort[0] = R/J */
        if (pSys->auchComPort[i] == DEVICE_KITCHEN_CRT) {
            chDflPortID = (CHAR)i;
            break;
        }
    }

}



/*
*=============================================================================
** Format  : SHORT    DflPrint(VOID *pData);
*               
*    Input : pData              -print data
*   Output : DFL_PERFORM        -function success (END_FRAME data)
*            DFL_BUSY           -function fail    (can not write to spool)
*            DFL_FAIL           -function fail    (wrong data received)
*    InOut : none
** Return  : none
*
** Synopsis: This function writes pData to spool buffer
*=============================================================================
*/
SHORT    DflPrint(VOID *pData)
{
    SHORT           sWork;
    DFLINF          *pInf;
    USHORT           usInfAdr;

    pInf = (DFLINF *)pData;

    /* -- check port -- */
    if (chDflPortID == DFL_PORT_NONE) {
        return (DFL_FAIL);
    }

    PifRequestSem(usDflSemStubFunc);

    /* convert term # ( ASCII to hex ) */
    DflAtoI((SHORT *)&usInfAdr, pInf->aszTermAdr, TCHARSIZEOF(pInf->aszTermAdr));
    /* convert length ( ASCII to hex ) */
    DflAtoI(&sWork, pInf->aszVLI, TCHARSIZEOF(pInf->aszVLI));

    /* -- check terminal address -- */
    DflCheckTermAdr(usInfAdr);        
    /* if term. addr = "0", handling as addr "1" */
    if (usInfAdr == 0) {
		usInfAdr = 1;
	}
	else {
		usInfAdr--;
	}

    /* -- check data length -- */
    if ( (sWork <= 0) || (DFL_SPOOL_SIZE < sWork) ) {
        /* clear previous data */
        memset(&DflPrevData[usInfAdr], 0x00, sizeof(DFLDATAHEAD));

        PifLog(MODULE_DFL, FAULT_ERROR_BAD_LENGTH);
        PifReleaseSem(usDflSemStubFunc);
        return (DFL_FAIL);
    }

    /* -- check previous data -- */
    if ( DFL_SAME == DflPrevDataCheck((DFLINTERFACE *)pData, usInfAdr) ) {
        PifReleaseSem(usDflSemStubFunc);
        return (DFL_PERFORM);
    }


    /* -- write to spool buffer -- */
    PifRequestSem(usDflSemRWSpool);             /* request sem. for access to spool buffer */
    if ( DFL_BUSY == DflWriteSpool(pData)) {    /* write to spool buffer */
        /* -- if cannot write -- */
        PifReleaseSem(usDflSemRWSpool);         /* release sem. for access buffer */
        PifReleaseSem(usDflSemStubFunc);        /* release sem. for stub function */
        return (DFL_BUSY);
    }

    /* -- if can write -- */
    PifReleaseSem(usDflSemStartDSP);            /* start send thread */

    memcpy( &(DflPrevData[usInfAdr]), &DflCurData, sizeof(DFLDATAHEAD) );
                                                /* store data for checking next data */

    PifReleaseSem(usDflSemRWSpool);             /* release sem. for access buffer */
    PifReleaseSem(usDflSemStubFunc);            /* release sem. for stub function */
    return (DFL_PERFORM);
}

/*
*=============================================================================
** Format  : VOID   DflCheckTermAdr(USHORT  usInfAdr)
*               
*    Input : usInfAdr         -terminal unique address - 1
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function writes pData to spool buffer
*=============================================================================
*/
VOID   DflCheckTermAdr(USHORT  usInfAdr)
{
    if (DFL_MAX_TERMNO < usInfAdr) {        
        PifAbort(MODULE_DFL, FAULT_ERROR_BAD_LENGTH);
    }
}

/*
*=============================================================================
** Format  : VOID   DflAtoI(SHORT *psDest, TCHAR *pszStr, USHORT usLen);
*               
*    Input : psDest             -destination buffer
*            pszStr             -source strings pointer   
*            usLen              -strings length   
*               
*   Output : none
*    InOut : none
** Return  : none
*
** Synopsis: This function converts ASCII data to short data.
*=============================================================================
*/
VOID   DflAtoI(SHORT *psDest, TCHAR *pszStr, USHORT usLen)
{
    TCHAR  aszWork[16];

    memset(aszWork, 0x00, sizeof(aszWork));

	NHPOS_ASSERT (usLen < TCHARSIZEOF(aszWork));

	if (usLen >= TCHARSIZEOF(aszWork)) {
		usLen = TCHARSIZEOF(aszWork) - 1;
	}

    _tcsncpy (aszWork, pszStr, usLen);

    *psDest = _ttoi(aszWork);
}

/*
*=============================================================================
** Format  : SHORT    DflPrevDataCheck(DFLINTERFACE *pData, USHORT usInfNo)
*               
*    Input : pData              -print data
*   Output : DFL_PERFORM        -data is not same as previous data
*            DFL_SAME           -data is same as previous data
*    InOut : none
** Return  : none
*
** Synopsis: This function writes pData to spool buffer
*=============================================================================
*/
SHORT    DflPrevDataCheck(DFLINTERFACE *pData, USHORT usInfNo)
{
    UCHAR   *puchInf;
    USHORT  usAdd;
    USHORT  i;

    /* -- convert ASCII to binary data -- */
    DflAtoI(&(DflCurData.usVLI), pData->DflHead.aszVLI, TCHARSIZEOF(pData->DflHead.aszVLI));
    DflAtoI((SHORT *)&i, pData->DflHead.aszTermAdr, TCHARSIZEOF(pData->DflHead.aszTermAdr));
    DflCurData.uchTermAdr = (UCHAR)i;
    DflAtoI(&(DflCurData.usConsNo), pData->DflHead.aszConsNo, TCHARSIZEOF(pData->DflHead.aszConsNo));
    DflCurData.uchSeqNo = pData->uchSeqNo;
    memcpy(DflCurData.auchItemType, pData->DflHead.auchItemType, sizeof(DflCurData.auchItemType));

    /* -- data set ? -- */
    puchInf = (UCHAR *)&(DflPrevData[usInfNo]);
    for (i=0; i < sizeof(DFLDATAHEAD); i++) {
         if( (usAdd = *(puchInf + i)) != 0) {
            break;
        }
    }

    if (usAdd != 0) {                           /* previous data is set */

        /* -- compare current data with previous data -- */
        if ( 0 == memcmp(&DflCurData, &(DflPrevData[usInfNo]),
                                      sizeof(DFLDATAHEAD)) )  {
            return (DFL_SAME);
        }
    }


    return (DFL_PERFORM);
}

/*
*=============================================================================
** Format  : SHORT    DflWriteSpool(DFLINTERFACE *pData)
*               
*    Input : pData              -print data
*   Output : 
*    InOut : none
** Return  : DFL_PERFORM        -success to write spool buffer
*            DFL_BUSY           -not success to write spool buffer
** Synopsis: This function writes pData to spool buffer
*
*=============================================================================
*/
SHORT  DflWriteSpool(DFLINTERFACE *pData)
{

    /* -- case of spool buffer full -- */
    if (fbDflSpoolFull & DFL_SPOOL_FULL) {

        if ( !(usDflReadP - usDflWriteP > DflCurData.usVLI) ) {
            return (DFL_BUSY);
        }

    /* -- case of not file full -- */
    } else {

        if ( usDflWriteP > DFL_SPOOL_SIZE ) {
            PifAbort(MODULE_DFL, FAULT_ERROR_BAD_LENGTH);
        }

        if ( !( (USHORT)(DFL_SPOOL_SIZE-usDflWriteP) > DflCurData.usVLI ) ) {

            if (usDflReadP > DflCurData.usVLI) {
                usDflBuffEnd = usDflWriteP;     /* set  end offset */
                fbDflSpoolFull |= DFL_SPOOL_FULL; /* set  full flag */
                usDflWriteP = 0;                /* move write point to buffer top */
            } else {
                return (DFL_BUSY);
            }
        }
    }

    //_itot((USHORT)(DflCurData.usVLI-1), pData->DflHead.aszVLI, _tcslen(pData->DflHead.aszVLI));
    DflItoA((USHORT)(DflCurData.usVLI-1), pData->DflHead.aszVLI, sizeof(pData->DflHead.aszVLI));
                                        /* " -1" for SeqNo. length */

	memcpy(&auchDflSpool[usDflWriteP], pData, sizeof(DFLINF));
                                        /* write data header to spool buffer */

	memcpy(&auchDflSpool[usDflWriteP + sizeof(DFLINF)], pData + 1, (DflCurData.usVLI - sizeof(DFLINTERFACE)));
                                        /* write data to spool buffer */

    usDflWriteP += DflCurData.usVLI-1;  /* increment write pointer */

    return (DFL_PERFORM);
}


/*
*=============================================================================
** Format  : VOID   DflItoA(USHORT usSorc, UCHAR *pszStr, USHORT usStrLen)
*               
*    Input : usSorc             -source data
*            usStrLen           -destination buffer length   
*    Output: pszStr             -destination buffer
** Return  : none
*
** Synopsis: This function converts long data to ASCII.
*=============================================================================
*/
VOID   DflItoA(USHORT usSorc,  TCHAR *pszStr, USHORT usStrLen)
{
	TCHAR auchWork[16] = { 0 };
    SHORT sLen, sStrLen = usStrLen;

    for (sLen = 0; sLen < sStrLen; sLen++) {
		pszStr[sLen] = _T('0');
	}

    _itot((SHORT)usSorc, auchWork, 10);          /* "10" = radix */

    sLen = tcharlen(auchWork);

	NHPOS_ASSERT(sStrLen >= sLen);

	if (sStrLen < sLen)
		sLen = sStrLen;

	sLen--;
    for(TCHAR *p = pszStr + sStrLen - 1; p >= pszStr && sLen >= 0; p--, sLen--) {
		*p = auchWork[sLen];
	}
}

/* ---------- End of Dfl.c ---------- */

