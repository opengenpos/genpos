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
* Title       : Employee No. Assignment Module                         
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: MATETK.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*                                       
*               MaintEmployeeNoRead()     : Employee No. Read Module   
*               MaintEmployeeNoWrite()    : Employee No. Write Module   
*               MaintEmployeeNoDelete()   : Employee No. Delete Module   
*               MaintEmployeeNoReport()   : Employee No. Report Module
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev.   :   Name    : Description
* Jun-22-92:00.00.01    :M.Yamamoto : initial                                   
* Sep-24-93: 02.00.01   : J.IKEDA   : Adds PrtShrPartialSend();                                    
* Sep-27-93: 02.00.01   : J.IKEDA   : Maint print for change
* Nov-27-95: 03.01.00   : M.Ozawa   : Add mnemonic maintenance
** GenPOS
* Jul-20-17: 02.02.02   : R.Chambers : cleanup, localize variables, Rfl functions for mnemonics.
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
#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <rfl.h>
#include <uie.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>
#include <csetk.h>
#include <csstbetk.h>
#include <display.h>
#include <mld.h>

#include "maintram.h" 


/*
*======================================================================================
**   Synopsis:  SHORT MaintEmployeeRecordRead (PARAEMPLOYEENO *pPara, ULONG ulEmployeeNo)
*
*       Input:  *pPara        : Pointer to Structure for PARAEMPLOYEENO  
*               ulEmployeeNo  : employee number of record to be read.
*
*      Output:  employee job code data if pPara is not NULL
*
**  Return:     SUCCESS         : Successful 
*               ETK_NOT_IN_FILE : employee record not found
*               
**  Description: Read the Employee Time Keeping file to obtain the job code data and
*                return the job code data if the pointer to a data buffer is not NULL.
*
*                This function is used to obtain the job code data for a particular employee
*                and to also obtain the status of the employee job code record such as
*                ETK_NOT_IN_FILE. If the employee job code data is not in the file and
*                the pPara pointer is not NULL then we zero out the job code data to return
*                something reasonable.
*======================================================================================
*/
static SHORT  MaintEmployeeRecordRead (PARAEMPLOYEENO *pPara, ULONG ulEmployeeNo)
{
	SHORT     sError;
	ETK_JOB   EtkJob = {0};
	TCHAR     aszEtkMnemonics[PARA_LEADTHRU_LEN + 1] = {0};

    if ((sError = SerEtkIndJobRead(ulEmployeeNo, &EtkJob, aszEtkMnemonics)) == ETK_NOT_IN_FILE) {
		if (pPara) {
			pPara->ulEmployeeNo = ulEmployeeNo;
			pPara->uchJobCode1 = 0;
			pPara->uchJobCode2 = 0;
			pPara->uchJobCode3 = 0;
			pPara->aszMnemonics[0] = '\0';
		}
    } else {    /* in case of CHANGE */
		if (pPara) {
			pPara->ulEmployeeNo = ulEmployeeNo;
			pPara->uchJobCode1 = EtkJob.uchJobCode1;
			pPara->uchJobCode2 = EtkJob.uchJobCode2;
			pPara->uchJobCode3 = EtkJob.uchJobCode3;
			_tcsncpy(pPara->aszMnemonics, aszEtkMnemonics, PARA_LEADTHRU_LEN);
		}
	}

	return sError;
}

/*
*======================================================================================
**   Synopsis:  SHORT MaintEmployeeNoRead( PARAEMPLOYEENO *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for PARAEMPLOYEENO  
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Employee No. Read Module  
*======================================================================================
*/

SHORT MaintEmployeeNoRead( PARAEMPLOYEENO *pData )
{

    SHORT           sError;
	PARAEMPLOYEENO  ParaEmployeeNo = {0};

    /* Check Employee Number Range */
    if (pData->ulEmployeeNo == 0 || pData->ulEmployeeNo > MAINT_ETK_MAX) {     /* Out of Range */
        return(LDT_KEYOVER_ADR);
    }

    /* Save and Read Employee Parameter */
    memset(&MaintWork.ParaEmployeeNo, 0x00, sizeof(PARAEMPLOYEENO));

    MaintWork.ParaEmployeeNo.uchMajorClass = pData->uchMajorClass;                /* Save Major Class */
    MaintWork.ParaEmployeeNo.ulEmployeeNo = pData->ulEmployeeNo;                  /* Save Employee No. */

    MaintWork.ParaEmployeeNo.uchFieldAddr = 1;

	sError = MaintEmployeeRecordRead (&MaintWork.ParaEmployeeNo, pData->ulEmployeeNo);
    if (sError == ETK_NOT_IN_FILE) {
        /******************/
        /* Display to MLD */
        /******************/
        MldDispItem(&MaintWork.ParaEmployeeNo, 1);
    } else {    /* in case of CHANGE */
        /* check operational times */
        if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
            /* Control Header Item */
            MaintHeaderCtl(AC_ETK_ASSIN, RPT_ACT_ADR);                      
        }

        MaintWork.ParaEmployeeNo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

        /******************/
        /* Display to MLD */
        /******************/
        MldDispItem(&MaintWork.ParaEmployeeNo, 1);

        /* print old data */
        PrtPrintItem(NULL, &(MaintWork.ParaEmployeeNo));

        /* send print data to shared module */
        PrtShrPartialSend(MaintWork.ParaEmployeeNo.usPrintControl);

        if ((sError != ETK_SUCCESS) && (sError != ETK_NOT_IN_FILE)) {
            return(EtkConvertError(sError));
        }
    }
    
    MaintWork.ParaEmployeeNo.uchMinorClass = pData->uchMinorClass;

    /* Set Leadthru Data */
    ParaEmployeeNo = MaintWork.ParaEmployeeNo;
    RflGetLead (ParaEmployeeNo.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&ParaEmployeeNo);                         /* Display Employee Parameter */

    return(SUCCESS);
}

/*
*======================================================================================
**   Synopsis:  SHORT MaintEmployeeNoEdit( PARAEMPLOYEENO *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for PARAEMPLOYEENO
*                               : pData.uchJobCode1 = input Job code data
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Employee No. Edit Module  
*======================================================================================
*/

SHORT MaintEmployeeNoEdit( PARAEMPLOYEENO *pData  )
{
    PARAEMPLOYEENO  ParaEmployeeNo;

    /* Check W/ Amount */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* W/O Amount Case */
        return(LDT_SEQERR_ADR);
    }

    switch (MaintWork.ParaEmployeeNo.uchFieldAddr) {
    case ETK_FIELD1_ADR:
        MaintWork.ParaEmployeeNo.uchJobCode1 = pData->uchJobCode1;
        break;

    case ETK_FIELD2_ADR:
        MaintWork.ParaEmployeeNo.uchJobCode2 = pData->uchJobCode1;
        break;

    case ETK_FIELD3_ADR:
        MaintWork.ParaEmployeeNo.uchJobCode3 = pData->uchJobCode1;
        break;

    case ETK_FIELD4_ADR:
        _tcsncpy(MaintWork.ParaEmployeeNo.aszMnemonics, pData->aszMnemonics, PARA_LEADTHRU_LEN);
        break;

    default:
        return(LDT_SEQERR_ADR);                             /* sequence error */

    }

    /******************/
    /* Display to MLD */
    /******************/
    MldDispItem(&MaintWork.ParaEmployeeNo, 0);

    if (MaintWork.ParaEmployeeNo.uchFieldAddr == ETK_FIELD4_ADR) {
        MaintWork.ParaEmployeeNo.uchFieldAddr = 1;
    } else {
        MaintWork.ParaEmployeeNo.uchFieldAddr++;
    }

    /* display leadthruogh */
    ParaEmployeeNo = MaintWork.ParaEmployeeNo;
    if (MaintWork.ParaEmployeeNo.uchFieldAddr != ETK_FIELD4_ADR) {
        RflGetLead (ParaEmployeeNo.aszMnemonics, LDT_DATA_ADR);
    }
    DispWrite(&ParaEmployeeNo);

    if (MaintWork.ParaEmployeeNo.uchFieldAddr == ETK_FIELD4_ADR) {
        return(MAINT_EMPLOYEE_MNEMO_ADR);
    } else {
        return(SUCCESS);
    }
}

/*
*======================================================================================
**   Synopsis:  SHORT MaintEmployeeNoWrite( VOID )
*
*       Output:  Nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Employee No. Write Module  
*======================================================================================
*/

SHORT MaintEmployeeNoWrite( VOID )
{
    SHORT           sErrorSav, sError;
    ETK_JOB         EtkJob = {0};
	TCHAR           aszEtkMnemonics[PARA_LEADTHRU_LEN + 1] = {0};

    /* check operational times */
    if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
        /* Control Header Item */
        MaintHeaderCtl(AC_ETK_ASSIN, RPT_ACT_ADR);                      
    }

	// check to see if we can access the ETK file successfully or not.
	sErrorSav =  MaintEmployeeRecordRead (NULL, MaintWork.ParaEmployeeNo.ulEmployeeNo);
	if (sErrorSav != ETK_SUCCESS) {
        if (sErrorSav != ETK_NOT_IN_FILE) { /* ### MOD (2171 Rel1.0) */
            return(EtkConvertError(sErrorSav));
        }
    }

    /* Assign Employee Parameter */
    EtkJob.uchJobCode1 =  MaintWork.ParaEmployeeNo.uchJobCode1;
    EtkJob.uchJobCode2 =  MaintWork.ParaEmployeeNo.uchJobCode2;
    EtkJob.uchJobCode3 =  MaintWork.ParaEmployeeNo.uchJobCode3;
    _tcsncpy(aszEtkMnemonics, MaintWork.ParaEmployeeNo.aszMnemonics, PARA_LEADTHRU_LEN);

    if ((sError = SerEtkAssign(MaintWork.ParaEmployeeNo.ulEmployeeNo, &EtkJob, aszEtkMnemonics)) != ETK_SUCCESS) {
        return(EtkConvertError(sError));
    }

    if (sErrorSav == ETK_SUCCESS) {
        MaintEmployeeMnemo( TRN_CHNG_ADR );     /* get mneminics "CHANGE" */
    } else if (sErrorSav == ETK_NOT_IN_FILE) {  /* ### MOD (2172 Rel1.0) */
        MaintEmployeeMnemo( TRN_ADD_ADR );      /* get mnemonics "ADDITION" */
    }

    /* Print Employee Parameter */
    MaintWork.ParaEmployeeNo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.ParaEmployeeNo));

    /* send print data to shared module */
    PrtShrPartialSend(MaintWork.ParaEmployeeNo.usPrintControl);

    return(SUCCESS);
}

/*
*======================================================================================
**   Synopsis:  SHORT MaintEmployeeNoDelete( PARAEMPLOYEENO *pData )
*
*       Input:  *pData          : Pointer to Structure for Key PARAEMPLOYEENO
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: Employee No. Delete Module
*======================================================================================
*/

SHORT MaintEmployeeNoDelete( PARAEMPLOYEENO *pData )
{
    SHORT           sError;

    /* check operational times */
    if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */

        /* Control Header Item */
        MaintHeaderCtl(AC_ETK_ASSIN, RPT_ACT_ADR);                      
    }

    /* Check Employee Number Range */
    if (pData->ulEmployeeNo == 0 || pData->ulEmployeeNo > MAINT_ETK_MAX) {   /* Out of Range */
        return(LDT_KEYOVER_ADR);
    }

	sError = MaintEmployeeRecordRead (&MaintWork.ParaEmployeeNo, pData->ulEmployeeNo);
    if (sError != ETK_SUCCESS) {
        return(EtkConvertError(sError));
    }

    if ((sError = SerEtkDelete(pData->ulEmployeeNo)) != ETK_SUCCESS) {
        return(EtkConvertError(sError));
    }
    
    /* Save Employee Parameter */
    MaintWork.ParaEmployeeNo.uchMajorClass = pData->uchMajorClass;            /* Save Major Class */
    MaintWork.ParaEmployeeNo.ulEmployeeNo = pData->ulEmployeeNo;
    MaintWork.ParaEmployeeNo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* Get Deleted Mnemonics and Print */
    MaintEmployeeMnemo(TRN_DEL_ADR);
    
    /* Print Employee Parameter */
    PrtPrintItem(NULL, &(MaintWork.ParaEmployeeNo));

    /* send print data to shared module */
    PrtShrPartialSend(MaintWork.ParaEmployeeNo.usPrintControl);

    return(SUCCESS);
}

/*
*======================================================================================
**   Synopsis:  SHORT MaintEmployeeNoReport( UCHAR uchMinorClass )
*
*       Input:  uchMinorClass   : CLASS_PARAEMPLOYEENO_STSGC9 
*                               : CLASS_PARAEMPLOYEENO_STSGC50
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Employee No. Report Module  
*======================================================================================
*/
SHORT MaintEmployeeNoReport( UCHAR uchMinorClass )
{
    SHORT       sError;
    ULONG       aulEtkAllIdBuff[FLEX_ETK_MAX];
    USHORT      i, usEtkMax;

    /* Get All Employee Id */
    if ((sError = SerEtkCurAllIdRead(sizeof(aulEtkAllIdBuff), aulEtkAllIdBuff)) < 0) {   /* Error Case */
        return(EtkConvertError(sError));
    }

    usEtkMax = ( USHORT)sError;

    /* Control Header Item */
    MaintHeaderCtl(AC_ETK_ASSIN, RPT_ACT_ADR);                      

    /* Set Major Class and Minor Class for the report output */
    MaintWork.ParaEmployeeNo.uchMajorClass = CLASS_PARAEMPLOYEENO;
    MaintWork.ParaEmployeeNo.uchMinorClass =  uchMinorClass;

    for (i = 0; i < usEtkMax; i++) {
        /* Check Abort Key */
        if (UieReadAbortKey() == UIE_ENABLE) {                           /* Abort Key Already Put */
            MaintMakeAbortKey();                                           
            break;
        }
        /* Read Employee Parameter */
        if (MaintEmployeeRecordRead (&MaintWork.ParaEmployeeNo, aulEtkAllIdBuff[i]) != ETK_SUCCESS) {
            continue;
        }

        /* Print Employee Parameter */
        MaintWork.ParaEmployeeNo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
        PrtPrintItem(NULL, &(MaintWork.ParaEmployeeNo));
    }

    /* Make Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(SUCCESS);
}

/*
*======================================================================================
**   Synopsis:  VOID MaintEmployeeNoDisp( PARAEMPLOYEENO *pData )
*
*       Input:  uchMinorClass   :   
*      Output:  Nothing
*
**  Return:     Nothing 
*               
**  Description: Job Code Display.
*======================================================================================
*/

VOID MaintEmployeeNoDisp( PARAEMPLOYEENO *pData )
{
    PARAEMPLOYEENO    ParaEtk;

    ParaEtk = MaintWork.ParaEmployeeNo;
    ParaEtk.uchMinorClass = pData->uchMinorClass;

    if (ParaEtk.uchFieldAddr != ETK_FIELD4_ADR) {
        RflGetLead (ParaEtk.aszMnemonics, LDT_DATA_ADR); /* Set Lead Through Mnemonics */
    }

    DispWrite(&ParaEtk);
}
/*
*======================================================================================
**   Synopsis:  VOID MaintEmployeeNoDispJob( PARAEMPLOYEENO *pData )
*
*       Input:  uchMinorClass   :   
*      Output:  Nothing
*
**  Return:     Nothing 
*               
**  Description: Job Code Display.
*======================================================================================
*/

VOID MaintEmployeeNoDispJob( PARAEMPLOYEENO *pData )
{
    MaintWork.ParaEmployeeNo.uchFieldAddr = pData->uchFieldAddr;
    MaintEmployeeNoDisp(pData);
}
/*
*======================================================================================
**   Synopsis:  SHORT MaintEmployeeNoIncJob( PARAEMPLOYEENO *pData )
*
*       Input:  uchMinorClass   :   
*      Output:  Nothing
*
**  Return:     Nothing 
*               
**  Description: Job Code Display.
*======================================================================================
*/

SHORT MaintEmployeeNoIncJob( PARAEMPLOYEENO *pData )
{
    if (MaintWork.ParaEmployeeNo.uchFieldAddr == ETK_FIELD4_ADR) {
        MaintWork.ParaEmployeeNo.uchFieldAddr = 1;
    } else {
        MaintWork.ParaEmployeeNo.uchFieldAddr++;
    }
    MaintEmployeeNoDisp(pData);

    if (MaintWork.ParaEmployeeNo.uchFieldAddr == ETK_FIELD4_ADR) {
        return(MAINT_EMPLOYEE_MNEMO_ADR);
    } else {
        return(SUCCESS);
    }
}
/*
*======================================================================================
**   Synopsis:  VOID MaintEmployeeMnemo( UCHAR uchAddress )
*
*       Input:  uchAddress   :  TRN_ADD_ADR   "ADDITION"
*                               TRN_DEL_ADR   "DELETION"
*                               TRN_CHNG_ADR  "CHANGE"
*      Output:  Nothing
*
**  Return:     Nothing 
*               
**  Description:    Print Addtion, Deletion, Change.
*======================================================================================
*/
VOID MaintEmployeeMnemo( UCHAR uchAddress )
{
	MAINTTRANS      MaintTrans = {0};

    /* Print Deleted Mnemonics */
    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    RflGetTranMnem (MaintTrans.aszTransMnemo, uchAddress);
    MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );

    PrtPrintItem(NULL, &MaintTrans);
}

/*
*======================================================================================
**   Synopsis:  VOID MaintCalcIndTtl( UCHAR uchType ,USHORT usTtlTime, USHORT usTtlMinute
*                                     USHORT *pusCalcTime, USHORT *pusCalcMinute)
*
*       Input:  uchType     -   0 - CLEAR
*                               1 - ADD
*               usTtlTime   -   Add Hours.
*               usTtlMinute -   Add Minutes
*                               
*      Output:  *pusCalcTime      -   Caluculated Time
*               *pusCalcMinute    -   Caluculated Minute
*
**  Return:     Nothing 
*
**  Description:    uchType = NEW : Clear Total
*                   uchType = ADD : Add Total
*======================================================================================
*/
VOID MaintCalcIndTtl( UCHAR uchType ,USHORT usTtlTime, USHORT usTtlMinute,
                      USHORT *pusCalcTime, USHORT *pusCalcMinute)
{
    if (uchType == 0) {
        MaintWork.ParaEmployeeNo.usRptAllTtlTime = 0;
        MaintWork.ParaEmployeeNo.usRptAllTtlMinute = 0;
    } else {
		USHORT  usWorkTime;

        MaintWork.ParaEmployeeNo.usRptAllTtlTime += usTtlTime;
        MaintWork.ParaEmployeeNo.usRptAllTtlMinute += usTtlMinute;
        if ((usWorkTime = ( MaintWork.ParaEmployeeNo.usRptAllTtlMinute / 60)) != 0) {
            MaintWork.ParaEmployeeNo.usRptAllTtlTime += usWorkTime;
            MaintWork.ParaEmployeeNo.usRptAllTtlMinute = (MaintWork.ParaEmployeeNo.usRptAllTtlMinute % 60);
        }
        *pusCalcTime = MaintWork.ParaEmployeeNo.usRptAllTtlTime;
        *pusCalcMinute = MaintWork.ParaEmployeeNo.usRptAllTtlMinute;
    }
}
