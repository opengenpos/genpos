/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Employee No. Maintenance Module                         
* Category    : Maintenance, NCR 2170 US Hospitality Application         
* Program Name: MATETKFL.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*                                       
*               MaintETKFileRead()    : ETK File Read Module   
*               MaintETKFileWrite()   : ETK File Write Module   
*               MaintETKFilePrint()   : ETK File Print Module   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Oct-01-93: 02.00.01   : J.IKEDA   : Initial
* Jul-28-95: 03.00.03   : M.Ozawa   : Add Display on LCD
*
** NCR2171 **
* Aug-26-99: 01.00.00   : M.Teraki  : initial (for 2171)
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
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/
#include	<tchar.h>
#include <string.h>
#include <stdlib.h>

#include <ecr.h>
#include <uie.h>
/* #include <pif.h> */
/* #include <log.h> */
/* #include <appllog.h> */
#include <paraequ.h>
#include <para.h>
#include <rfl.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>
#include <csetk.h>
#include <csstbetk.h>
#include <display.h>
#include <mld.h>
/* #include <mldsup.h> */

#include "maintram.h" 

/*
==============================================================================
;                      C O M M O N   R A M   A R E A                         
;=============================================================================
*/

UCHAR  uchMaintEtkCtl;      /* control flag */ 
USHORT usMaintFieldNo;      /* keep Block No. for ETK File print */ 

/*
 *  Provide an ETK_FIELD data structure that has been initialized
 *  so as to provide a single place where any changes need to be made
 *  should the struct change.
*/
ETK_FIELD  MaintETKFieldInit(VOID)
{
	ETK_FIELD  etkField = { 0 };

	etkField.uchJobCode = 0;
	etkField.usYear = 0;
	etkField.usMonth = 0;
	etkField.usDay = 0;
	etkField.usTimeinTime = ETK_TIME_NOT_IN;        // MaintETKFieldInit() default Time-in/Time-out
	etkField.usTimeinMinute = ETK_TIME_NOT_IN;
	etkField.usTimeOutTime = ETK_TIME_NOT_IN;
	etkField.usTimeOutMinute = ETK_TIME_NOT_IN;

	return etkField;
}

/*
*======================================================================================
**   Synopsis:  SHORT MaintETKFileRead( PARAETKFL *pData )
*
*       Input:  *pKeyMsg            : Pointer to Structure for PARAETKFL  
*      Output:  Nothing
*
**  Return:     SUCCESS             : Successful 
*               LDT_SEQERR_ADR      : Sequence Error
*               LDT_KEYOVER_ADR     : Wrong Data Error
*               LDT_FILE_NOT_FOUND  : File not Found
*               LDT_NOT_IN_FILE     : Not in File
*               
**  Description: ETK File Read Module  
*======================================================================================
*/

SHORT MaintETKFileRead( PARAETKFL *pData )
{

    USHORT        usCount;
    USHORT        usTimeIOStat;
    USHORT        usBlockNo = 1;
    SHORT         sError;
    SHORT         sStatus;
	ETK_JOB       EtkJob = { 0 };
	ETK_FIELD     EtkField = { 0 };
	ETK_TIME      EtkTime = { 0 };
	MAINTETKFL    MaintEtkFl = { 0 };
	TCHAR         aszEtkMnemonics[PARA_LEADTHRU_LEN + 1] = { 0 };

    /* initialize control flag */

    uchMaintEtkCtl = 0;

    /* check Employee No. range */

    if (pData->ulEmployeeNo == 0 || pData->ulEmployeeNo > MAINT_ETK_MAX) {     /* Out of Range */
        return(LDT_KEYOVER_ADR);
    }

    /* get current Block No. */

    if ((sError = SerEtkStatRead(pData->ulEmployeeNo, &usTimeIOStat, &usBlockNo, &EtkJob)) != ETK_SUCCESS) {
        return(EtkConvertError(sError));
    } 

    if ((sError = SerEtkIndJobRead(pData->ulEmployeeNo, &EtkJob, aszEtkMnemonics)) != ETK_SUCCESS) {
        return(EtkConvertError(sError));
    }

    memset(&MaintWork.EtkFl, 0, sizeof(MAINTETKFL));

    MaintWork.EtkFl.ulEmployeeNo = pData->ulEmployeeNo; /* set Employee No. */
    MaintWork.EtkFl.usBlockNo = 1;                      /* 1st Block No. */
	RflCopyMnemonic(MaintWork.EtkFl.aszMnemonics, aszEtkMnemonics, PARA_LEADTHRU_LEN);/* employee mnemonic */

    /* display 1st block data */

    if (((sStatus = MaintETKFileMldAllDisp(usTimeIOStat)) == ETK_NOT_IN_FILE) || (sStatus == ETK_FILE_NOT_FOUND)) {
        return(EtkConvertError(sStatus));
    }
    if (((sStatus = MaintETKFileDisp()) == ETK_NOT_IN_FILE) || (sStatus == ETK_FILE_NOT_FOUND)) {
        return(EtkConvertError(sStatus));
    }

    /* check operational times */
    if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {  /* in case of 1st operation */
        MaintHeaderCtl(AC_ETK_MAINT, RPT_ACT_ADR);  /* print header */                    
    } else {
        MaintFeed();                                /* 1 line feed */
    }

    /* print employee number */
    MaintEmpNoPrint(); 

    /* set data for print */
    MaintEtkFl.uchMajorClass = CLASS_MAINTETKFL;
    MaintEtkFl.ulEmployeeNo = pData->ulEmployeeNo;
    MaintEtkFl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* print ETK File data */
    usMaintFieldNo = 0;     /* initialize static area */
    for (usCount = 1; usCount <= usBlockNo; usCount++) {

        sStatus = SerEtkCurIndRead(pData->ulEmployeeNo, &usMaintFieldNo, &EtkField, &EtkTime);

        /* check return status */
        if ((sStatus == ETK_NOT_IN_FILE) || (sStatus == ETK_FILE_NOT_FOUND)) {
            return(EtkConvertError(sError));
        }

        if (sStatus == ETK_NOT_IN_FIELD) {
            break;
        } 
        MaintEtkFl.usBlockNo = usCount;
		MaintEtkFl.EtkField = EtkField;
        PrtPrintItem(NULL, &MaintEtkFl);
    }
    if ((usTimeIOStat != (USHORT)ETK_NOT_TIME_OUT) && (usMaintFieldNo < ETK_MAX_FIELD)) { /* ### MOD (2172 Rel1.0) */
        usMaintFieldNo++;
    }

    /* send print data to shared module */
    PrtShrPartialSend(MaintEtkFl.usPrintControl);
    return(SUCCESS);
}

/*
*======================================================================================
**   Synopsis:  SHORT MaintETKFileWrite( PARAETKFL *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for PARAETKFL  
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: ETK File Write Module  
*======================================================================================
*/

SHORT MaintETKFileWrite( PARAETKFL *pData )
{

    SHORT       sReturn;
    SHORT       sError;
    SHORT       sStatus;
    USHORT      usBlockNo;
	ETK_FIELD   EtkField = { 0 };


    /* distinguish minor class */

    switch (pData->uchMinorClass) {                             
    case CLASS_PARAETKFILE_NUMTYPE:                                    

        /* check status */

        if (pData->uchStatus & MAINT_WITHOUT_DATA) {    /* W/O amount */
            usBlockNo = MaintWork.EtkFl.usBlockNo + 1;  

            if (usBlockNo > usMaintFieldNo) {    /* usMaintFieldNo = current Block No. + 1    */
                usBlockNo = 1 ;                  /* In case of Block No.15, usMaintFieldNo=15 */
            }
        } else {
            usBlockNo = pData->usBlockNo;
        }

        /* check Block No. range */

        if (usBlockNo < 1 || usMaintFieldNo < usBlockNo) {
            return(LDT_KEYOVER_ADR);
        }

        /* set Block No. */

        MaintWork.EtkFl.usBlockNo = usBlockNo;

        /* display ETK file */

        if (((sStatus = MaintETKFileDisp()) == ETK_NOT_IN_FILE) || (sStatus == ETK_FILE_NOT_FOUND)) {
            return(EtkConvertError(sStatus));
        }
        return(SUCCESS);

    case CLASS_PARAETKFILE_TIMEIN:

        /* set DATE/TIME(TIME-IN) */

        if ((sReturn = MaintEtkFileDateTime(pData)) != SUCCESS) {
            return(sReturn);
        }
        break;

    case CLASS_PARAETKFILE_TIMEOUT:

        /* set TIME(TIME-OUT) */

        if ((sReturn = MaintEtkFileDateTime(pData)) != SUCCESS) {
            return(sReturn);
        }
        break;

    case CLASS_PARAETKFILE_JOBCODE:

        MaintWork.EtkFl.EtkField.uchJobCode = pData->uchJobCode;
        break;

    default:
/*      PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
        break;
    }

    usBlockNo = MaintWork.EtkFl.usBlockNo - 1;  

    /* set NEW data */
    EtkField = MaintWork.EtkFl.EtkField;

    /* write data */

    if ((sError = EtkCurIndWrite(MaintWork.EtkFl.ulEmployeeNo, &usBlockNo, &EtkField)) != ETK_SUCCESS) {
        return(EtkConvertError(sError));
    }

    /* set flag for new ETK file print */

    uchMaintEtkCtl |= MAINT_ALREADY_PRINT;

    /* display ETK file */

    MaintETKFileDisp();
    return(SUCCESS);
}

/*
*======================================================================================
**   Synopsis:  VOID MaintETKFilePrint( VOID )
*
*       Input:  Nothing  
*      Output:  Nothing
*
**  Return:     Nothing 
*               
**  Description: ETK File Print Module  
*======================================================================================
*/

VOID MaintETKFilePrint( VOID )
{

    USHORT          usCount;
    USHORT          usBlockNo = 0;
    SHORT           sStatus;
	MAINTTRANS      MaintTrans = { 0 };
    ETK_TIME        EtkTime = { 0 };
    ETK_FIELD       EtkField = { 0 };

    /* check if old ETK File is printed */

    if (!(uchMaintEtkCtl & MAINT_ALREADY_PRINT)) {
        return;
    }

    /* print "CHANGE" */
    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );
	RflGetTranMnem(MaintTrans.aszTransMnemo, TRN_CHNG_ADR);
    PrtPrintItem(NULL, &MaintTrans);

    /* print current ETK file */
    for (usCount = 1; usCount <= usMaintFieldNo; usCount++) {
        sStatus = SerEtkCurIndRead(MaintWork.EtkFl.ulEmployeeNo, &usBlockNo, &EtkField, &EtkTime);

        /* check return status */

/*        if ((sStatus == ETK_NOT_IN_FILE) || (sStatus == ETK_FILE_NOT_FOUND)) {
            return(EtkConvertError(sStatus));
        }
*/
        if (sStatus == ETK_NOT_IN_FIELD) {
            break;
        } 
/*      MaintWork.EtkFl.uchMajorClass = CLASS_MATETKFL;     */
/*      MaintWork.EtkFl.ulEmployeeNo = MaintWork.EtkFl.ulEmployeeNo;   */
        MaintWork.EtkFl.usBlockNo = usCount;
        MaintWork.EtkFl.EtkField = EtkField;
        MaintWork.EtkFl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
        PrtPrintItem(NULL, &MaintWork.EtkFl);

        /* send print data to shared module */
        PrtShrPartialSend(MaintWork.EtkFl.usPrintControl);
    }

    /* initialize control flag */
    uchMaintEtkCtl = 0;
}

/*
*======================================================================================
**   Synopsis:  VOID MaintEmpNoPrint( VOID )
*
*       Input:  Nothing  
*      Output:  Nothing
*
**  Return:     Nothing 
*               
**  Description: Print Employee No.   
*======================================================================================
*/

VOID MaintEmpNoPrint( VOID ) 
{
    RPTEMPLOYEENO    RptEmpNo = { 0 };

    /* print Employee No */
    RptEmpNo.uchMajorClass = CLASS_RPTEMPLOYEENO;                
    RptEmpNo.uchMinorClass = CLASS_RPTEMPLOYEE_NUMBER;
    RptEmpNo.ulEmployeeNumber = MaintWork.EtkFl.ulEmployeeNo; 
	RflCopyMnemonic(RptEmpNo.aszEmpMnemo, MaintWork.EtkFl.aszMnemonics, PARA_LEADTHRU_LEN);
    RptEmpNo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &RptEmpNo);                           
    MaintFeed();    /* 1 line feed */
}

/*
*======================================================================================
**   Synopsis:  VOID MaintETKFileDisp( VOID )
*
*       Input:  Nothing  
*      Output:  Nothing
*
**  Return:     Nothing 
*               
**  Description: Display ETK File data.   
*======================================================================================
*/

SHORT MaintETKFileDisp( VOID )
{
    USHORT        usBlockNo;
    SHORT         sStatus;
	ETK_TIME      EtkTime = { 0 };
	ETK_FIELD     EtkField = { 0 };
	MAINTETKFL    MaintEtkFl = { 0 };

    usBlockNo = MaintWork.EtkFl.usBlockNo - 1;

    sStatus = SerEtkCurIndRead(MaintWork.EtkFl.ulEmployeeNo, &usBlockNo, &EtkField, &EtkTime);

    /* check return status */
    if ((sStatus == ETK_NOT_IN_FILE) || (sStatus == ETK_FILE_NOT_FOUND)) {
        return(sStatus);
    }

    MaintWork.EtkFl.uchMajorClass = CLASS_MAINTETKFL;

    if (sStatus == ETK_NOT_IN_FIELD) {
		MaintWork.EtkFl.EtkField = MaintETKFieldInit();
    } else {
        MaintWork.EtkFl.EtkField = EtkField;
    }

    MaintEtkFl = MaintWork.EtkFl;
	RflGetLead(MaintEtkFl.aszMnemonics, LDT_DATA_ADR);
    DispWrite(&MaintEtkFl); 

    MldDispItem(&MaintWork.EtkFl, 0); 

    return(SUCCESS);
}

/*
*======================================================================================
**   Synopsis:  VOID MaintETKFileDisp( VOID )
*
*       Input:  Nothing  
*      Output:  Nothing
*
**  Return:     Nothing 
*               
**  Description: Display ETK File data.   
*======================================================================================
*/

SHORT MaintETKFileMldAllDisp(USHORT usIOStatus)
{
    USHORT        usBlockNo, usReadNo;
    SHORT         sStatus;
	ETK_TIME      EtkTime = { 0 };
	ETK_FIELD     EtkField = { 0 };
	MAINTETKFL    EtkFl = { 0 };

    for (usBlockNo=0; usBlockNo<ETK_MAX_FIELD; usBlockNo++) {

        usReadNo = usBlockNo;

        sStatus = SerEtkCurIndRead(MaintWork.EtkFl.ulEmployeeNo, &usReadNo, &EtkField, &EtkTime);

        /* check return status */
        if ((sStatus == ETK_NOT_IN_FILE) || (sStatus == ETK_FILE_NOT_FOUND)) {
            return(sStatus);
        }

        EtkFl.uchMajorClass = CLASS_MAINTETKFL;

        if (sStatus == ETK_NOT_IN_FIELD) {
            /* not display if latest field is entered the time out */
            if (usIOStatus == (USHORT)ETK_NOT_TIME_OUT) break;  /* ### MOD (2172 Rel1.0) */

            EtkFl.ulEmployeeNo = MaintWork.EtkFl.ulEmployeeNo;
            EtkFl.usBlockNo = usBlockNo+1;
			EtkFl.EtkField = MaintETKFieldInit();
        } else {
            EtkFl.ulEmployeeNo = MaintWork.EtkFl.ulEmployeeNo;
            EtkFl.usBlockNo = usBlockNo+1;
            EtkFl.EtkField = EtkField;
        }

		RflCopyMnemonic(EtkFl.aszMnemonics, MaintWork.EtkFl.aszMnemonics, PARA_LEADTHRU_LEN);

        MldDispItem(&EtkFl, (USHORT)(usBlockNo?0:1));

        if (sStatus == ETK_NOT_IN_FIELD) break;
    }

    return(SUCCESS);
}

/*
*======================================================================================
**   Synopsis:  SHORT MaintETKFileDateTime( PARAETKFL *pData )
*
*       Input:  pData : Pointer to Structure for PARAETKFL  
*      Output:  Nothing
*
**  Return:     Nothing 
*               
**  Description: Set DATE & TIME.   
*======================================================================================
*/

SHORT MaintEtkFileDateTime(PARAETKFL *pData ) 
{
    UCHAR    aszBuffer[16] = { 0 };
    USHORT   usMonth;
    USHORT   usDay;
    USHORT   usHour; 
    USHORT   usMin;
    ULONG    ulDate;
    ULONG    ulTime;

    if (pData->uchMinorClass == CLASS_PARAETKFILE_TIMEIN) {

        /* check data length */
        _ultoa(pData->ulDateTimeIn, aszBuffer, 10);

        if (4 < strlen(aszBuffer)) {

            /* get Date data */
            ulDate = ( ULONG)(pData->ulDateTimeIn/10000L);

            /* check Data form */
            if (CliParaMDCCheck(MDC_DRAWER_ADR, EVEN_MDC_BIT2)) {   /* DD/MM/YY */
                usDay = ( USHORT)(ulDate/100L); 
                usMonth = ( USHORT)(ulDate%100L); 
            } else {
                usMonth = ( USHORT)(ulDate/100L); 
                usDay = ( USHORT)(ulDate%100L); 
            }

            /* check Date data range */

            if (usMonth < 1 || 12 < usMonth) {
                return(LDT_KEYOVER_ADR);
            }

            if (usDay < 1 || 31 < usDay) {
                return(LDT_KEYOVER_ADR);
            }
            MaintWork.EtkFl.EtkField.usMonth = usMonth;
            MaintWork.EtkFl.EtkField.usDay = usDay;
        }
        ulTime = ( ULONG)(pData->ulDateTimeIn%10000L);
    } else {    /* CLASS_PARAETKFILE_TIMEOUT */
        ulTime = pData->ulTimeOut;
    }

    /* get TIME data */
    usHour = ( USHORT)(ulTime/100L);
    usMin = ( USHORT)(ulTime%100L);

    /* check TIME range */
    if (23 < usHour) {
        return(LDT_KEYOVER_ADR);
    }

    if (59 < usMin) {
        return(LDT_KEYOVER_ADR);
    }

    if (pData->uchMinorClass == CLASS_PARAETKFILE_TIMEIN) {
        MaintWork.EtkFl.EtkField.usTimeinTime = usHour;
        MaintWork.EtkFl.EtkField.usTimeinMinute = usMin;
    } else {
        MaintWork.EtkFl.EtkField.usTimeOutTime = usHour;
        MaintWork.EtkFl.EtkField.usTimeOutMinute = usMin;
    }
    return(SUCCESS);
}

/**** End of File ****/
