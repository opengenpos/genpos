/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ISP Server Module, Subroutine Check Total & Create Flex Mem
* Category    : ISP Server Module, US Hospitality Model
* Program Name: ISPOPRSB.C                                            
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*           IspCheckTtlPLU();       Check PLU  total issued or not 
*           IspCheckTtlDept();      Check Dept total issued or not 
*           IspCheckTtlGCF();       Check GCF  total issued or not
*           IspCheckTtlCas();   Check cashier total issued or not
*
*           IspReadFlex();          Read a designate flex no and PTD flag
*           IspSavFlexMem();        Save flex memory data
*           IspCreateFlexMem();     Create flex memory
*           IspCreateFiles();       Create files
*           IspCheckDataFlexMem();  Check received flex para data 
*           IspReadFlexByAddr();    Read flex by address
*           IspCheckFlexFileSize(); Check flexible file size
*           IspTtlErrorConvert();   Total error convert
*           IspCheckAndCreate();    Check and create files
*           IspCheckDegTtl();       Check all totals
*           IspCheckTtlCpn();       Check Coupon total issued or not Add R3.0
*
* --------------------------------------------------------------------------
* Update Histories                                                         
* Date     :Ver.Rev.:   Name    : Description
* Jun-26-92:00.00.01:H.Yamaguchi: initial                                   
* Apr-02-93:01.00.11:H.Yamaguchi: Modify IspCheckAndCreate(); Remove total check
* Apr-07-94:00.00.04:K.You      : Add flex GC feature.(mod. IspCheckDataFlexMem)                                   
* Jun-01-95:02.05.04:M.Suzuki   : R3.0                                   
* Aug-11-99:03.05.00:M.Teraki   : Remove WAITER_MODEL
* Dec-14-99:01.00.00:hrkato     : Saratoga
*
* Jul-07-21:02.04.01:R.Chambers : replace IspReadFlexByAddr() with RflGetFlexMemByType()
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
#include	<tchar.h>
#include	<stdio.h>
#include	<math.h>
#include	<string.h>
#include    <memory.h>

#include    <ecr.h>
#include    <rfl.h>
#include    <pif.h>
#include    <log.h>
#include    <appllog.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csop.h>
#include    <cscas.h>
#include    <cswai.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csetk.h>
#include    <csstbetk.h>
#include    <csttl.h>
#include    <csstbttl.h>
#include    <csgcs.h>    
#include    <csstbgcf.h> 
#include    <csstbstb.h>
#include    <isp.h>
#include    "ispcom.h"
#include    "ispext.h"
#include    <csstbbak.h>


/*
*===========================================================================
** Synopsis:    SHORT    IspCheckAndCreate(ISPFLEXMEM  *pNew,
*                                          ISPFLEXMEM  *pOld)
*     Input:    pNew     - Start address of New Flex memory data
*               pOld     - Start address of Old Flex memory data
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:                 Data is Ok.
*               ????                         Data is over than max size.
*
** Description: This function check parameter.                  Saratoga
*===========================================================================
*/
static SHORT   IspCheckAndCreate(ISPFLEXMEM *pNew, ISPFLEXMEM *pOld)
{
    SHORT           sError = ISP_ERR_INVALID_MESSAGE;
    UCHAR           uchAdd;

	// The Flexible memory settings are retrieved and the file sizes checked.
	// See the defines for the Flexible Memory table indices starting with FLEX_DEPT_ADR.
	// See the defines for max sizes, max value for ulRecordNumber, starting with FLEX_DEPT_MAX.
    for ( uchAdd = FLEX_DEPT_ADR ; uchAdd <= MAX_FLXMEM_SIZE; uchAdd++ ) {
        
        if (uchAdd == FLEX_MLD_ADR) continue;	/* avoid to destroy display by ISP, 09/21/01 */
        
        if (  pNew->Fxmem[uchAdd-1].ulRecordNumber == pOld->Fxmem[uchAdd-1].ulRecordNumber ) {
            if ( pNew->Fxmem[uchAdd-1].uchPTDFlag == pOld->Fxmem[uchAdd-1].uchPTDFlag ) {
                continue;
            }                   
        }

/*        sError = IspCheckDegTtl(uchAdd);            */
/*                                                    */
/*        if ( ISP_SUCCESS != sError ) {              */
/*            return (sError);                        */
/*        }      04-02-93 Deleted for PEP Update      */

        sError = CliCreateFile(uchAdd, 0, 0);   /* Delete File first */
        if ( ISP_SUCCESS != sError ) {
			char  xBuff[128];

			sprintf (xBuff, "==ERROR: IspCheckAndCreate() CliCreateFile(uchAdd, 0, 0) sError %d uchAdd %d", sError, uchAdd);
			NHPOS_ASSERT_TEXT((ISP_SUCCESS == sError), xBuff);
			PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_CREATE_FILE);
			PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sError));
			PifLog(MODULE_DATA_VALUE(MODULE_ISP_LOG), uchAdd);
			PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
            return (sError);
        }
    }

    for ( uchAdd = FLEX_DEPT_ADR ; uchAdd <= MAX_FLXMEM_SIZE; uchAdd++ ) {
		PARAFLEXMEM     Flexmem = {0};

		Flexmem.uchMajorClass = CLASS_PARAFLEXMEM ;  /* FLEXIBLE MEMORY ASSIGNMENT */  
        
        if (uchAdd == FLEX_MLD_ADR) continue;	/* avoid to destroy display by ISP, 09/21/01 */

        if (  pNew->Fxmem[uchAdd-1].ulRecordNumber == pOld->Fxmem[uchAdd-1].ulRecordNumber ) {
            if ( pNew->Fxmem[uchAdd-1].uchPTDFlag == pOld->Fxmem[uchAdd-1].uchPTDFlag ) {
                continue;
            }
        }

        Flexmem.uchAddress     = uchAdd;                                /* Set address */
        Flexmem.ulRecordNumber = pNew->Fxmem[uchAdd-1].ulRecordNumber ; /* Number of Record */
        Flexmem.uchPTDFlag     = pNew->Fxmem[uchAdd-1].uchPTDFlag ;     /* PTD Provide Flag */
        CliParaWrite(&Flexmem) ;                                        /* Write Flex memory parameter */

        sError = CliCreateFile(uchAdd, pNew->Fxmem[uchAdd-1].ulRecordNumber, pNew->Fxmem[uchAdd-1].uchPTDFlag);
        if ( ISP_SUCCESS != sError ) {
			char  xBuff[128];

			sprintf (xBuff, "==ERROR: IspCheckAndCreate() CliCreateFile() sError %d uchAdd %d ulRecordNumber %d", sError, uchAdd, Flexmem.ulRecordNumber);
			NHPOS_ASSERT_TEXT((ISP_SUCCESS == sError), xBuff);

            Flexmem.ulRecordNumber = 0 ; /* Clear Number of Record */
            Flexmem.uchPTDFlag     = 0 ; /* Clear PTD Provide Flag */
            CliParaWrite(&Flexmem)     ; /* Write Flex memory parameter */
            CliCreateFile(uchAdd, 0, 0); /* Delete Files */
			PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_CREATE_FILE);
			PifLog(MODULE_ERROR_NO(MODULE_ISP_LOG), (USHORT)abs(sError));
			PifLog(MODULE_DATA_VALUE(MODULE_ISP_LOG), uchAdd);
			PifLog(MODULE_LINE_NO(MODULE_ISP_LOG), (USHORT)__LINE__);
            return (sError);
        }
    }

    PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_CREATE_FILE);      /* Write log */
    return (ISP_SUCCESS);
}


/*
*===========================================================================
** Synopsis:    SHORT    IspCheckTtlPLU(UCHAR *puchPluNo, UCHAR uchAjectNo);
*     Input:    usPluNo       - a PLU No or 0 (Means all PLU)
*               uchAjectiveNo - a Ajective No.
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:       Delete is Ok.
*               TTL_FAIL:          File not fund
*               TTL_NOT_ISSUED:    Not reset total
*
** Description:  This function executes to check for PLU Total.
*===========================================================================
*/
SHORT   IspCheckTtlPLU(TCHAR *puchPluNo, UCHAR uchAjectNo)
{
	FLEXMEM         Flexmem = RflGetFlexMemByType(FLEX_PLU_ADR);
	TTLPLU          TtlPlu = { 0 };	/* correct the missmatch of the structer, 10/03/01 */
    /* CLITTLCASWAI    TtlPlu; */
    SHORT           sError;

    TtlPlu.uchMajorClass   = CLASS_TTLPLU;
    TtlPlu.uchMinorClass   = CLASS_TTLSAVDAY;
    _tcsncpy(TtlPlu.auchPLUNumber, puchPluNo, OP_PLU_LEN);  /* Saratoga */
    TtlPlu.uchAdjectNo      = uchAjectNo;

    if ( 0 == Flexmem.ulRecordNumber ) {       /* Saratoga */
		NHPOS_NONASSERT_TEXT("==NOTE: IspCheckTtlPLU() 0 == Flex.ulRecordNumber")
        return (TTL_FAIL);
    }

    sError = SerTtlIssuedCheck(&TtlPlu, TTL_NOTRESET );
    if ( TTL_SUCCESS != (sError = IspTtlErrorConvert(sError) ) ) {
        return (sError);
    }

    TtlPlu.uchMinorClass = CLASS_TTLCURDAY;

    sError = SerTtlTotalCheck(&TtlPlu);
    if ( TTL_SUCCESS != ( sError = IspTtlErrorConvert(sError) ) ) {
        return (sError);
    }

    if ( 0 == Flexmem.uchPTDFlag ) {
        return (ISP_SUCCESS);        /* if not provide PTD, then return */
    }

    TtlPlu.uchMinorClass   = CLASS_TTLCURPTD;

    sError = SerTtlTotalCheck(&TtlPlu);
    if ( TTL_SUCCESS != ( sError = IspTtlErrorConvert(sError) ) ) {
        return (sError);
    }

    TtlPlu.uchMinorClass = CLASS_TTLSAVPTD;

    sError = SerTtlIssuedCheck(&TtlPlu, TTL_NOTRESET);
    if ( TTL_SUCCESS != (sError = IspTtlErrorConvert(sError) ) ) {
        return (sError);
    }

    return (ISP_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspCheckTtlDept(USHORT usDeptNo)
*     Input:    uchDeptNo  - a Dept No. or 0 (Means all dept.)
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:        Delete is Ok.
*               TTL_FAIL:           File not fund
*               TTL_NOT_ISSUED:     Not reset total
*
** Description: This function executes to check for Dept Total.
*===========================================================================
*/
SHORT   IspCheckTtlDept(USHORT usDeptNo)
{
	FLEXMEM         Flexmem = RflGetFlexMemByType(FLEX_DEPT_ADR);
	ISPTTLDEPT      TtlDep = { 0 };
    SHORT           sError;

    TtlDep.uchMajorClass = CLASS_TTLDEPT;
    TtlDep.uchMinorClass = CLASS_TTLCURDAY;
    TtlDep.usDeptNo      = usDeptNo;            /* Saratoga */

    if ( 0 == Flexmem.ulRecordNumber ) {           /* Saratoga */
		NHPOS_NONASSERT_TEXT("==NOTE: IspCheckTtlDept() 0 == Flex.ulRecordNumber")
        return (TTL_FAIL);
    }

    sError = SerTtlTotalCheck(&TtlDep);
    if ( TTL_SUCCESS != (sError = IspTtlErrorConvert(sError) ) ) {
        return (sError);
    }

    TtlDep.uchMinorClass = CLASS_TTLSAVDAY;

    sError = SerTtlIssuedCheck(&TtlDep, TTL_NOTRESET);
    if ( TTL_SUCCESS != (sError = IspTtlErrorConvert(sError) ) ) {
        return (sError);
    }

    if ( 0 == Flexmem.uchPTDFlag ) {
        return (ISP_SUCCESS);        /* if not provide PTD, then return */
    }

    TtlDep.uchMinorClass   = CLASS_TTLCURPTD;

    sError = SerTtlTotalCheck(&TtlDep);
    if ( TTL_SUCCESS != (sError = IspTtlErrorConvert(sError) ) ) {
        return (sError);
    }

    TtlDep.uchMinorClass = CLASS_TTLSAVDAY;

    sError = SerTtlIssuedCheck(&TtlDep, TTL_NOTRESET);
    if ( TTL_SUCCESS != (sError = IspTtlErrorConvert(sError) ) ) {
        return (sError);
    }
    
    return (ISP_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspCheckTtlGCF(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:        Delete is Ok.
*               TTL_FAIL:           File not fund
*               TTL_NOT_ISSUED:     Not reset total
*
** Description:  This function executes to check for Guest Check Total.
*===========================================================================
*/
SHORT   IspCheckTtlGCF(VOID)
{
	FLEXMEM         Flexmem = RflGetFlexMemByType(FLEX_GC_ADR);
	SHORT           sError;

    if ( 0 == Flexmem.ulRecordNumber ) {       /* Saratoga */
		NHPOS_NONASSERT_TEXT("==NOTE: IspCheckTtlGCF() 0 == Flex.ulRecordNumber")
        return (TTL_FAIL);
    }
    
    sError = SerGusCheckExist();
    if ((GCF_SUCCESS != sError) && (GCF_FILE_NOT_FOUND != sError)) {
        return (ISP_ERR_REQUEST_RESET);
    }
    return (ISP_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspCheckTtlCas(USHORT usCashierNo);
*     Input:    usCashierNo - Cas No or 0 (Mean all cashier total)
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:            Delete is Ok.
*               TTL_FAIL:               File not fund
*               TTL_NOT_ISSUED:         Not reset total
*
** Description:  This function executes to check for Cashier Total.
*===========================================================================
*/
SHORT   IspCheckTtlCas(ULONG ulCashierNo)
{
	FLEXMEM         Flexmem = RflGetFlexMemByType(FLEX_CAS_ADR);
    CLITTLCASWAI    TtlCas = { 0 };
    SHORT           sError;

    TtlCas.uchMajorClass   = CLASS_TTLCASHIER;
    TtlCas.uchMinorClass   = CLASS_TTLCURDAY;   /* Set current daily */
    TtlCas.ulCashierNo        = ulCashierNo;
    TtlCas.uchAjectNo      = 0;

    if ( 0 == Flexmem.ulRecordNumber ) {           /* Saratoga */
		NHPOS_NONASSERT_TEXT("==NOTE: IspCheckTtlCas() 0 == Flex.ulRecordNumber")
        return (TTL_FAIL);               /* if not in, then error */
    }

    sError = SerTtlTotalCheck(&TtlCas);  /* Check daily total */
    if ( TTL_SUCCESS != ( sError = IspTtlErrorConvert(sError) ) ) {  /* Check daily total */
        return (sError);
    }

    TtlCas.uchMinorClass = CLASS_TTLSAVDAY;     /* Set saved daily */

    sError = SerTtlIssuedCheck(&TtlCas, TTL_NOTRESET);   /* Check saved total */
    if ( TTL_SUCCESS != (sError = IspTtlErrorConvert(sError) ) ) { /* Check saved total */
        return(sError);
    }

    return (ISP_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspCheckTtlEtk(ULONG ulEtkNo);
*     Input:    ulEtkNo  - Etk No or  0 (Mean all ETK )
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:            Delete is Ok.
*               TTL_FAIL:               File not fund
*               TTL_NOT_ISSUED:         Not reset total
*
** Description:  This function executes to check for Cashier Total.
*===========================================================================
*/
SHORT   IspCheckTtlEtk(ULONG    ulEtkNo)
{
	FLEXMEM         Flexmem = RflGetFlexMemByType(FLEX_ETK_ADR);
	SHORT           sError;

    if ( 0 == Flexmem.ulRecordNumber ) {       /* Saratoga */
        return (TTL_FAIL);               /* if not in, then error */
    }

    sError = SerEtkAllDataCheck(CLASS_TTLCURDAY);  /* Check daily total */
    if ( ETK_SUCCESS != sError ) { /* Check saved total */
        return(sError);
    }

    sError = SerEtkIssuedCheck();    /* Check saved total */
    if ( ETK_ISSUED != sError ) { /* Check saved total */
        return(sError);
    }

    return (ISP_SUCCESS);
    ulEtkNo = ulEtkNo;
}



/*
*===========================================================================
** Synopsis:    VOID     IspSavFlexMem();
*     Input:    nothing
*     Output:   nothing   
*     InOut:    nothing
*
** Return:      nothing
*
** Description:  Save Flex Memory.
*===========================================================================
*/
VOID   IspSavFlexMem(VOID)
{
    USHORT usDataLen;

    CliParaAllRead(CLASS_PARAFLEXMEM, &auchIspTmpBuf[(ISP_MAX_TMPSIZE) - (MAX_FLXMEM_SIZE*sizeof(FLEXMEM)) -2],
                   MAX_FLXMEM_SIZE*sizeof(FLEXMEM), 0, &usDataLen);

    fsIspCleanUP |= ISP_CREATE_FLEXMEM;   /* set flag */
}

/*
*===========================================================================
** Synopsis:    SHORT    IspCreateFlexMem();
*     Input:    nothing
*     Output:   nothing   
*     InOut:    nothing
*
** Return:      nothing
*
** Description:  Create Flex Memory.
*===========================================================================
*/
SHORT   IspCreateFlexMem(VOID)
{
    ISPFLEXMEM    *pOld = (ISPFLEXMEM *)&auchIspTmpBuf[(ISP_MAX_TMPSIZE) - (MAX_FLXMEM_SIZE * sizeof(FLEXMEM)) - 2];
    SHORT         sError = ISP_SUCCESS;

    if ( fsIspCleanUP & ISP_CREATE_FLEXMEM ) {   /* Check flag */
		ISPFLEXMEM    *pNew = (ISPFLEXMEM *)IspResp.pSavBuff->auchData;

        fsIspCleanUP &= ~ISP_CREATE_FLEXMEM;   /* Reset flag */
        SerChangeInqStat();      /* Execute AC 85 */
        sError = memcmp(pNew, pOld, MAX_FLXMEM_SIZE * sizeof(FLEXMEM));
        if ( 0 == sError ) {   /* if same data, the no creation files */
            return(sError);
        }
        sError = IspCheckAndCreate(pNew, pOld);
    }
    return (sError);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspCreateFiles(VOID);
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:           Delete is Ok.
*               ??                     Fail
*
** Description: This function creates parameter file and total.
*===========================================================================
*/
SHORT   IspCreateFiles(VOID)
{
    SHORT       sError;
    UCHAR       uchAdd;

    for ( uchAdd = 1; uchAdd <= MAX_FLXMEM_SIZE; uchAdd++ ) {
        sError = CliCreateFile(uchAdd, 0 , 0 );
        if ( 0 != sError ) {
            return (sError);
        }
    }

    for ( uchAdd = 1; uchAdd <= MAX_FLXMEM_SIZE; uchAdd++ ) {
		FLEXMEM         Flexmem = RflGetFlexMemByType(uchAdd);

		sError = CliCreateFile(uchAdd, Flexmem.ulRecordNumber , Flexmem.uchPTDFlag ); /* Saratoga */
        if ( 0 != sError ) {
            return (sError);
        }
    }

    PifLog(MODULE_ISP_LOG, LOG_EVENT_ISP_CREATE_FILE);      /* Write log */
    return (ISP_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspCheckFlexFileSize(UCHAR uchAddr);
*     Input:    uchAddr    - Define option file 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:           Delete is Ok.
*               ???_FILE_NOT_FOUND     File not found.
*
** Description: This function checkes parameter file size.      Saratoga
*===========================================================================
*/
SHORT   IspCheckFlexFileSize(UCHAR uchAddr)
{
    SHORT       sError;

    if ( 0L == RflGetMaxRecordNumberByType(uchAddr)) {
        switch (uchAddr) {
        case  FLEX_PLU_ADR :   /* Number of PLU */
            sError = OP_FILE_NOT_FOUND;
            break;
    
        case  FLEX_WT_ADR :    /* Number of Waiter */  
            sError = WAI_FILE_NOT_FOUND;
            break;
    
        case  FLEX_CAS_ADR :   /* Number of Cashier */  
            sError = CAS_FILE_NOT_FOUND;
            break;

        case  FLEX_GC_ADR :    /* Number of GuestCheck */
            sError = GCF_FILE_NOT_FOUND;
            break;

        case FLEX_ETK_ADR  :  /* Number of ETK */
            sError = ETK_FILE_NOT_FOUND;   
            break;

        /******************* Add R3.0 *******************************/
        case FLEX_CPN_ADR  :  /* Number of CPN */
            sError = OP_FILE_NOT_FOUND;   
            break;
        /******************* End Add R3.0 ***************************/

        default :  /* (FLEX_DEPT_ADR) Number od Department */
            sError = OP_FILE_NOT_FOUND;
            break;
        }
    
        return(sError);
    }

    return (ISP_SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT    IspTtlErrorConvert(SHORT sError);
*     Input:    sError    - Error Code
*     Output:   nothing
*     InOut:    nothing
*
** Return:      TTL_SUCCESS:       Ok.
*               TTL_????:          TTL error
*               TTL_NOT_ISSUED:    Not reset total
*
** Description:  This function executes to convert error code.
*===========================================================================
*/
#if defined(IspTtlErrorConvert)
SHORT   IspTtlErrorConvert_Special(SHORT sError);
SHORT IspTtlErrorConvert_Debug(SHORT sError, char *aszPath, int nLineNo)
{
	SHORT  sRet = IspTtlErrorConvert_Special (sError);

	if (sRet != TTL_SUCCESS) {
		char  xBuff[128];

		sprintf (xBuff, "==WARNING: IspTtlErrorConvert() sError = %d  file %s line %d", sError, RflPathChop(aszPath), nLineNo);
		NHPOS_NONASSERT_TEXT (xBuff);
	}

	return sRet;
}

SHORT   IspTtlErrorConvert_Special(SHORT sError)
#else
SHORT   IspTtlErrorConvert(SHORT sError)
#endif
{
    switch ( sError ) {       
    case  TTL_FAIL :
    case  TTL_SUCCESS :
    case  TTL_NOTINFILE :
    case  TTL_FILE_HANDLE_ERR :
        sError = TTL_SUCCESS;  /* Convert error code */
        break;

    default :
        break;
    }

    return (sError);
}


/*
*===========================================================================
** Synopsis:    SHORT    IspCheckDegTtl(UCHAR uchAdd);
*     Input:    uchAdd    - Designate File 
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:      Data is Ok.
*               ????              Total is not zero.
*
** Description: This function check parameter.
*===========================================================================
*/
SHORT   IspCheckDegTtl(UCHAR uchAdd)
{
	TCHAR       auchPluNo[CLI_PLU_LEN] = { 0 };
    SHORT       sError;

    switch ( uchAdd ) {
    case FLEX_DEPT_ADR :   
        sError = IspCheckTtlDept(0);          /* Check Issued reset report */
        break;
    
    case FLEX_PLU_ADR  :
        sError = IspCheckTtlPLU(auchPluNo,0);   /* Check Issued reset report,   Saratoga */
        break;

    case FLEX_WT_ADR   :    
        sError = ISP_SUCCESS;
        break;

    case FLEX_CAS_ADR  :
        sError = IspCheckTtlCas(0);   /* Check Issued reset report */
        break;
 
    case FLEX_ETK_ADR  :
        sError = IspCheckTtlEtk(0);   /* Check Issued reset report */
        break;

    /************************* Add R3.0 **********************************/
    case FLEX_CPN_ADR  :
        sError = IspCheckTtlCpn(0);   /* Check Issued reset report */
        break;
    /************************* End Add R3.0 ******************************/
    
    default :
        sError = IspCheckTtlGCF();    /* Check Issued reset report */
        break;
    }
 
    if ( TTL_FAIL == sError ) {       /* if FAIL, then SUCCESS */
        sError = ISP_SUCCESS;
    } 
    if ( ISP_SUCCESS != sError){
        return (ISP_ERR_REQUEST_RESET);
    }
    return (ISP_SUCCESS);
}

/************************* Add R3.0 **********************************/
/*
*===========================================================================
** Synopsis:    SHORT    IspCheckTtlCpn(UCHAR uchCpnNo);
*     Input:    uchCpnNo  - a Coupon No. or 0 (Means all dept.)
*     Output:   nothing
*     InOut:    nothing
*
** Return:      ISP_SUCCESS:        Delete is Ok.
*               TTL_FAIL:           File not fund
*               TTL_NOT_ISSUED:     Not reset total
*
** Description: This function executes to check for Coupon Total.   Saratoga
*===========================================================================
*/
SHORT   IspCheckTtlCpn(USHORT usCpnNo)
{
	FLEXMEM         Flexmem = RflGetFlexMemByType(FLEX_CPN_ADR);
	TTLCPN          TtlCpn = { 0 };
    SHORT           sError;

    TtlCpn.uchMajorClass   = CLASS_TTLCPN;
    TtlCpn.uchMinorClass   = CLASS_TTLCURDAY;
    TtlCpn.usCpnNumber    = usCpnNo;

    if ( 0 == Flexmem.ulRecordNumber ) {
		NHPOS_NONASSERT_TEXT("==NOTE: IspCheckTtlCpn() 0 == Flex.ulRecordNumber")
        return (TTL_FAIL);               /* if not in, the errorr */
    }

    sError = SerTtlTotalCheck(&TtlCpn);
    if ( TTL_SUCCESS != (sError = IspTtlErrorConvert(sError) ) ) {
        return (sError);
    }

    TtlCpn.uchMinorClass = CLASS_TTLSAVDAY;

    sError = SerTtlIssuedCheck(&TtlCpn, TTL_NOTRESET);
    if ( TTL_SUCCESS != (sError = IspTtlErrorConvert(sError) ) ) {
        return (sError);
    }

    if ( 0 == Flexmem.uchPTDFlag ) {
        return (ISP_SUCCESS);        /* if not provide PTD, then return */
    }

    TtlCpn.uchMinorClass   = CLASS_TTLCURPTD;

    sError = SerTtlTotalCheck(&TtlCpn);
    if ( TTL_SUCCESS != (sError = IspTtlErrorConvert(sError) ) ) {
        return (sError);
    }

    TtlCpn.uchMinorClass = CLASS_TTLSAVDAY;

    sError = SerTtlIssuedCheck(&TtlCpn, TTL_NOTRESET);
    if ( TTL_SUCCESS != (sError = IspTtlErrorConvert(sError) ) ) {
        return (sError);
    }
    
    return (ISP_SUCCESS);
}
/************************* End Add R3.0 ******************************/

/*===== END OF SOURCE =====*/