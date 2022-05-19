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
* Title       : MaintDEPT Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATDEPT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls DEPARTMENT PARAMETER FILE.
*
*           The provided function names are as follows: 
* 
*             MaintDEPTRead()        : read & display parameter from department file 
*             MaintDEPTEdit()        : check data & set data in self-work area 
*             MaintDEPTWrite()       : write department parameter record in department file & print 
*             MaintDEPTReport()      : print all department parameter report   
*             MaintSetDEPTDispData() : set data for display 
*  
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-22-92: 00.00.01 : J.Ikeda   : initial                                   
* Aug-25-93: 01.00.13 : J.Ikeda   : Add PrtShrPartialSend() in MaintDEPTRead()                                   
*          :          :           : & MaintDEPTWrite()
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
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/
#include	<tchar.h>
#include <string.h>

#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <log.h>
#include <rfl.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csop.h>
#include <csstbpar.h>
#include <csstbopr.h>
#include <csttl.h>
#include <csstbttl.h>
#include <appllog.h>

#include <mld.h>

#include "maintram.h" 

/*
*===========================================================================
*   Work Area Declartion 
*===========================================================================
*/

MAINTDEPT MaintDept;

/*
*===========================================================================
** Synopsis:    SHORT MaintDEPTRead( PARADEPT *pData )
*               
*     Input:    *pData          : pointer to structure for PARDEPT
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_SEQERR_ADR  : sequence error
*
** Description: This function reads & displays parameter from DEPARTMENT FILE in self-work area.
*===========================================================================
*/

SHORT MaintDEPTRead( PARADEPT *pData )
{
    SHORT         sReturn;
	DEPTIF        DeptIf = { 0 };
	PARADEPT      MldParaDept = { 0 };                                  /* FOR MLD */

    /* initialize */
    memset(&MaintWork, 0, sizeof(MAINTWORK));
    memset(&MaintDept, 0, sizeof(MAINTDEPT));
    
    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    } 
    DeptIf.usDeptNo = pData->usDEPTNumber;

    /* read data */
    if ((sReturn = CliOpDeptIndRead(&DeptIf, usMaintLockHnd)) != OP_PERFORM) {
#if 0
        /* get the number of department */

        ParaFlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
        ParaFlexMem.uchAddress = FLEX_DEPT_ADR;
        CliParaRead(&ParaFlexMem);

        /* check the number of department */

        if (( USHORT)(pData->uchDEPTNumber) < ParaFlexMem.usRecordNumber && pData->uchDEPTNumber != 0) {
            PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_NOTREADDEPT);
        }

        /* convert error status */
#endif
        if (sReturn != OP_NOT_IN_FILE) {
            return(OpConvertError(sReturn));
        }
    }

    /* check operational times */
    if (sReturn == OP_PERFORM) {
        if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
            MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */

            /* make header */
            MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* minor class */
                        RPT_ACT_ADR,                            /* "ACT #" */
                        RPT_DEPT_ADR,                           /* "DEPART" */
                        0,                                                              
                        RPT_MAINT_ADR,                          /* "MAINTE" */
                        0,                                                              
                        AC_DEPT_MAINT,                          /* action number(114) */
                        0,                                                              
                        PRT_RECEIPT | PRT_JOURNAL); /* print control */
                                                                        
            uchMaintOpeCo |= MAINT_ALREADYOPERATE;
        } else {
            /* 1 line feed */
            MaintFeed();
        }
    }

    /* set data for printing */
    MaintWork.DEPT.uchMajorClass = CLASS_PARADEPT;
    MaintWork.DEPT.usDEPTNumber = DeptIf.usDeptNo;
    MaintWork.DEPT.uchMajorDEPT = DeptIf.ParaDept.uchMdept;
    memcpy(MaintWork.DEPT.auchDEPTStatus, DeptIf.ParaDept.auchControlCode, MAX_DEPTSTS_SIZE);
    MaintWork.DEPT.uchHALO = DeptIf.ParaDept.auchHalo[0];
    MaintWork.DEPT.uchPrintPriority = DeptIf.ParaDept.uchPrintPriority;
	RflCopyMnemonic(MaintWork.DEPT.aszMnemonic, DeptIf.ParaDept.auchMnemonic, PARA_DEPT_LEN);
    MaintWork.DEPT.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /******************/
    /* Display to MLD */
    /******************/
    MldParaDept = MaintWork.DEPT;
    MldParaDept.uchFieldAddr = 0;
    MldDispItem(&MldParaDept,0);

    if (sReturn == OP_PERFORM) {
        PrtPrintItem(NULL, &(MaintWork.DEPT));

        /* send print data to shared module */
        PrtShrPartialSend(MaintWork.DEPT.usPrintControl);
    }

    /* display address 1 data */
    MaintDept.uchMajorClass = CLASS_PARADEPT;
    MaintDept.uchMinorClass = CLASS_PARADEPT_DATA;
    MaintDept.uchFieldAddr = DPT_FIELD1_ADR;
    MaintDept.usDEPTNumber = pData->usDEPTNumber;
    MaintDept.usACNumber = pData->usACNumber;
    MaintDept.ulInputData = ( ULONG)(MaintWork.DEPT.uchMajorDEPT);
	RflGetLead(MaintDept.aszMnemonics, LDT_MDEPT_ADR);
    DispWrite(&MaintDept);

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintDEPTEdit( MAINTDEPT *pData )
*               
*     Input:    *pData               : pointer to structure for MAINTDEPT
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS              : successful
*               LDT_SEQERR_ADR       : sequence error
*               LDT_KEYOVER_ADR      : wrong data
*               MAINT_DEPT_MNEMO_ADR : set mnemonics
*
** Description: This function checks input-data from UI and sets in self-work area.
*===========================================================================
*/

SHORT MaintDEPTEdit( MAINTDEPT *pData )
{
	PARADEPT    MldParaDept = { 0 };                                    /* FOR MLD */

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */

        /* check minor class */
        switch(pData->uchMinorClass) {    
        case CLASS_MAINTDEPT_READ:
            MaintDept.uchFieldAddr++;

            /* check address */
            if (MaintDept.uchFieldAddr > DPT_FIELD14_ADR) {
                MaintDept.uchFieldAddr = DPT_FIELD1_ADR;        /* initialize address */
            }

            /* set data for display next old data */
            MaintSetDEPTDispData(MaintDept.uchFieldAddr);
            break;

        case CLASS_MAINTDEPT_WRITE:
            return(LDT_SEQERR_ADR);                             /* sequence error */

        default:
/*            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
            return(LDT_SEQERR_ADR);                             /* sequence error */
        }
    } else {                                                    /* with data */

        /* check minor class */
        switch(pData->uchMinorClass) {    
        case CLASS_MAINTDEPT_READ:

            /* check address */
            if (pData->uchFieldAddr < DPT_FIELD1_ADR || DPT_FIELD14_ADR < pData->uchFieldAddr) {
                return(LDT_KEYOVER_ADR);                        /* wrong data */
            }
            MaintDept.uchFieldAddr = pData->uchFieldAddr;

            /* set data for display old data */
            MaintSetDEPTDispData(MaintDept.uchFieldAddr);
            break;

        case CLASS_MAINTDEPT_WRITE:

            /* check address */
            switch(MaintDept.uchFieldAddr) {

            case DPT_FIELD1_ADR:
    
                /* check major dept range */
                if (pData->ulInputData < 1L || ( ULONG)MAINT_MDEPT_MAX < pData->ulInputData) {
                    return(LDT_KEYOVER_ADR);                    /* wrong data */
                }
                MaintWork.DEPT.uchMajorDEPT = ( UCHAR)(pData->ulInputData);
                break;

            case DPT_FIELD8_ADR:

                /* check bonus total index range */
				/* Add a Hardcoded 8 for the time being, so that we are just adding to the 
				8 original Bonus totals, and not the newly updated 100 totals JHHJ  STD_NO_BONUS_TTL */
                if (( ULONG) 8 /*MAINT_BONUSTL_MAX*/ < pData->ulInputData) {
                    return(LDT_KEYOVER_ADR);                    /* wrong data */
                }

                MaintWork.DEPT.auchDEPTStatus[(MaintDept.uchFieldAddr - 2) / 2] &= 0xF0;
                MaintWork.DEPT.auchDEPTStatus[(MaintDept.uchFieldAddr - 2) / 2] |= pData->ulInputData;
                break;                                     

            case DPT_FIELD2_ADR:
            case DPT_FIELD4_ADR:
            case DPT_FIELD6_ADR:
    
                /* check DEPT status */
                if (MaintChkBinary(pData->aszMnemonics, 4) == LDT_KEYOVER_ADR) {
                    return(LDT_KEYOVER_ADR);      
                }

                /* convert string data into bit pattern */
                MaintWork.DEPT.auchDEPTStatus[(MaintDept.uchFieldAddr - 2) / 2] &= 0xF0;
                MaintWork.DEPT.auchDEPTStatus[(MaintDept.uchFieldAddr - 2) / 2] |= MaintAtoStatus(pData->aszMnemonics);
                break;                                     
    
            case DPT_FIELD3_ADR:
            case DPT_FIELD5_ADR:
            case DPT_FIELD7_ADR:
            case DPT_FIELD9_ADR:
    
                /* check DEPT status */
                if (MaintChkBinary(pData->aszMnemonics, 4) == LDT_KEYOVER_ADR) {
                    return(LDT_KEYOVER_ADR);      
                }

                /* convert string data into bit pattern */
                MaintWork.DEPT.auchDEPTStatus[(MaintDept.uchFieldAddr - 3) / 2] &= 0x0F;        
                MaintWork.DEPT.auchDEPTStatus[(MaintDept.uchFieldAddr - 3) / 2] |= MaintAtoStatus(pData->aszMnemonics) << 4;
                break;                                     
    
            case DPT_FIELD10_ADR:

                /* check HALO */
                if (pData->ulInputData > ( ULONG)MAINT_HALO_MAX) {
                    return(LDT_KEYOVER_ADR);                        /* wrong data */
                }                                              
                MaintWork.DEPT.uchHALO = ( UCHAR)(pData->ulInputData);
                break;

            case DPT_FIELD11_ADR:
				RflCopyMnemonic(MaintWork.DEPT.aszMnemonic, pData->aszMnemonics, PARA_DEPT_LEN);
                break;

            case DPT_FIELD12_ADR:

                /* check Print Priority */
                if (pData->ulInputData > (ULONG)PLU_PRIORITY_MAX) {
                    return(LDT_KEYOVER_ADR);                        /* wrong data */
                }                                              
                MaintWork.DEPT.uchPrintPriority = ( UCHAR)(pData->ulInputData);
                break;

            case DPT_FIELD13_ADR:
    
                /* check DEPT status */
                if (MaintChkBinary(pData->aszMnemonics, 4) == LDT_KEYOVER_ADR) {
                    return(LDT_KEYOVER_ADR);      
                }

                /* convert string data into bit pattern */
                MaintWork.DEPT.auchDEPTStatus[(MaintDept.uchFieldAddr - 5) / 2] &= 0xF0;
                MaintWork.DEPT.auchDEPTStatus[(MaintDept.uchFieldAddr - 5) / 2] |= MaintAtoStatus(pData->aszMnemonics);
                break;                                     
    
            case DPT_FIELD14_ADR: /* 2172 */
    
                /* check DEPT status */
                if (MaintChkBinary(pData->aszMnemonics, 4) == LDT_KEYOVER_ADR) {
                    return(LDT_KEYOVER_ADR);      
                }

                /* convert string data into bit pattern */
                MaintWork.DEPT.auchDEPTStatus[(MaintDept.uchFieldAddr - 5) / 2] &= 0x0F;        
                MaintWork.DEPT.auchDEPTStatus[(MaintDept.uchFieldAddr - 5) / 2] |= MaintAtoStatus(pData->aszMnemonics) << 4;
                break;                                     
    
            default:                                       
/*                PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
                return(LDT_SEQERR_ADR);                             /* sequence error */
            }                                              

            /******************/
            /* Display to MLD */
            /******************/
			MldParaDept = MaintWork.DEPT;
            MldParaDept.uchFieldAddr = MaintDept.uchFieldAddr;
            MldDispItem(&MldParaDept,0);

            /* set data for display next old data */       
            MaintDept.uchFieldAddr++;

            /* check address */
            if (MaintDept.uchFieldAddr > DPT_FIELD14_ADR) {
                MaintDept.uchFieldAddr = DPT_FIELD1_ADR;            /* initialize address */
            }

            /* display next old data */
            MaintSetDEPTDispData(MaintDept.uchFieldAddr);
            break;

        default:
/*            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
            return(LDT_SEQERR_ADR);                             /* sequence error */
        }                                            
    }    

    /* check address */
    if (MaintDept.uchFieldAddr == DPT_FIELD11_ADR) {
        return(MAINT_DEPT_MNEMO_ADR);                       /* set mnemonics */
    }
    return(SUCCESS);
}                                                              

/*                                                             
*===========================================================================
** Synopsis:    SHORT MaintDEPTWrite( VOID )                   
*               
*     Input:    Nothing                                        
*    Output:    Nothing 
*     InOut:    Nothing                                        
*
** Return:      SUCCESS   : successful                         
*               sReturn   : error 
*                                                              
** Description: This function writes & prints DEPARTMENT PARAMETER RECORD in self-work area.
*===========================================================================
*/
                                                               
SHORT MaintDEPTWrite( VOID )                                   
{
    SHORT          sReturn, sReturnSave;
	DEPTIF         DeptIf = { 0 };
	MAINTTRANS     MaintTrans = { 0 };
                                                               
    DeptIf.usDeptNo = MaintWork.DEPT.usDEPTNumber;            
    sReturnSave = CliOpDeptIndRead(&DeptIf, usMaintLockHnd);    /* read previous dept */

    /* set data for writing */                                             
    DeptIf.usDeptNo = MaintWork.DEPT.usDEPTNumber;            
    DeptIf.ParaDept.uchMdept = MaintWork.DEPT.uchMajorDEPT;
    memcpy(DeptIf.ParaDept.auchControlCode, MaintWork.DEPT.auchDEPTStatus, MAX_DEPTSTS_SIZE);
    DeptIf.ParaDept.auchHalo[0] = MaintWork.DEPT.uchHALO;       
    DeptIf.ParaDept.uchPrintPriority = MaintWork.DEPT.uchPrintPriority;
    _tcsncpy(DeptIf.ParaDept.auchMnemonic, MaintWork.DEPT.aszMnemonic, PARA_DEPT_LEN);
                                                               
    /* write data */
    if ((sReturn  = SerOpDeptAssign(&DeptIf, usMaintLockHnd)) != OP_PERFORM) {
        
        PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_NOTASSIGNDEPT);
        
        /* convert error status */
        return(OpConvertError(sReturn));
    }        

    if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */

        /* make header */
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* minor class */
                    RPT_ACT_ADR,                            /* "ACT #" */
                    RPT_DEPT_ADR,                           /* "DEPART" */
                    0,                                                              
                    RPT_MAINT_ADR,                          /* "MAINTE" */
                    0,                                                              
                    AC_DEPT_MAINT,                          /* action number(114) */
                    0,                                                              
                    PRT_RECEIPT | PRT_JOURNAL); /* print control */
                                                                        
        uchMaintOpeCo |= MAINT_ALREADYOPERATE;
    }
        
    /* print "CHANGE" using structure MAINTTRANS */
    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT);
    if (sReturnSave == OP_PERFORM) {
		RflGetTranMnem(MaintTrans.aszTransMnemo, TRN_CHNG_ADR);
	} else {
		RflGetTranMnem(MaintTrans.aszTransMnemo, TRN_ADD_ADR);
    }
    PrtPrintItem(NULL,&MaintTrans);

    /* set journal bit & receipt bit */
    MaintWork.DEPT.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* print department parameter */
    PrtPrintItem(NULL, &(MaintWork.DEPT));

    /* send print data to shared module */
    PrtShrPartialSend(MaintWork.DEPT.usPrintControl);

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintDEPTReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               sReturn         : error
*
**Description: This function prints all DEPARTMENT PARAMETER REPORT from DEPARTMENT FILE.
*========================================================================
*/

SHORT MaintDEPTReport( VOID )
{
    UCHAR           uchFeedChk = 0;
    SHORT           sReturn;
	PARAFLEXMEM     ParaFlexMem = { 0 };
	DEPTIF          DeptIf = { 0 };
	MAINTERRORCODE  MaintErrorCode = { 0 };

    /* initialize */
    memset(MaintWork.DEPT.aszMnemonic, 0, sizeof(MaintWork.DEPT.aszMnemonic));

    /* check operational times */
    if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */

        /* make header */
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                   /* minor class */
                        RPT_ACT_ADR,                             /* "ACT #" */
                        RPT_DEPT_ADR,                            /* "DEPART" */
                        0,
                        RPT_FILE_ADR,                            /* "FILE" */
                        0,                                       /* report type */
                        AC_DEPT_MAINT,                           /* action number(114) */
                        0,                                                              
                        PRT_RECEIPT | PRT_JOURNAL);  /* print control */
        uchMaintOpeCo |= MAINT_ALREADYOPERATE;
    }

    /* get the number of department */
    ParaFlexMem.uchMajorClass = CLASS_PARAFLEXMEM;
    ParaFlexMem.uchAddress = FLEX_DEPT_ADR;
    CliParaRead(&ParaFlexMem);                                  /* call ParaFlexMemRead() */

    /* set print control */
    MaintWork.DEPT.usPrintControl = 0;

    if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT0))) {
        MaintWork.DEPT.usPrintControl |= PRT_JOURNAL;   /* print out on journal */
    } 

    if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT1))) {
        MaintWork.DEPT.usPrintControl |= PRT_RECEIPT;
    }

    /* set data at every address */
    do {
        sReturn = SerOpDeptAllRead(&DeptIf, usMaintLockHnd);
        if (sReturn != OP_PERFORM) {
            break;
        }

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }

        /* set data */
        MaintWork.DEPT.uchMajorClass = CLASS_PARADEPT;
        MaintWork.DEPT.usDEPTNumber = DeptIf.usDeptNo;
        MaintWork.DEPT.uchMajorDEPT = DeptIf.ParaDept.uchMdept;
        memcpy(MaintWork.DEPT.auchDEPTStatus, DeptIf.ParaDept.auchControlCode, MAX_DEPTSTS_SIZE);
        MaintWork.DEPT.uchHALO = DeptIf.ParaDept.auchHalo[0];
        MaintWork.DEPT.uchPrintPriority = DeptIf.ParaDept.uchPrintPriority;
		RflCopyMnemonic(MaintWork.DEPT.aszMnemonic, DeptIf.ParaDept.auchMnemonic, PARA_DEPT_LEN);
            
        /* check print control */
        if (MaintWork.DEPT.usPrintControl & PRT_RECEIPT) {

            /* check feed */
            if (uchFeedChk) {
                /* 1 line feed */
                MaintFeed();
            }
        }
        PrtPrintItem(NULL, &(MaintWork.DEPT));
        uchFeedChk = 1;

    } while (sReturn == OP_PERFORM);
    
                                                  
    /* print error status */
    if ((sReturn != OP_PERFORM) && (sReturn != OP_EOF)) {            /* error occurred */
        MaintErrorCode.uchMajorClass = CLASS_MAINTERRORCODE;
        MaintErrorCode.sErrorCode = sReturn;

        /* set print control */
        MaintErrorCode.usPrtControl = 0;

        if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT0))) {
            MaintErrorCode.usPrtControl |= PRT_JOURNAL;   /* print out on journal */
        } 

        if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT1))) {
            MaintErrorCode.usPrtControl |= PRT_RECEIPT;   /* print out repeipt */
        }
        PrtPrintItem(NULL, &MaintErrorCode);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintSetDEPTDispData(UCHAR uchFieldAddr)
*               
*     Input:    uchFieldAddr : field address for display data
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function sets data for display.
*===========================================================================
*/

VOID MaintSetDEPTDispData(UCHAR uchFieldAddr)
{
    memset(MaintDept.aszMnemonics, 0, sizeof(MaintDept.aszMnemonics));

    /* distinguish field address and set data for display */
    switch(uchFieldAddr) {
    case DPT_FIELD1_ADR:  /* major department number */
        MaintDept.uchMinorClass = CLASS_PARADEPT_DATA;
        MaintDept.ulInputData = ( ULONG)MaintWork.DEPT.uchMajorDEPT;
		RflGetLead(MaintDept.aszMnemonics, LDT_MDEPT_ADR);     /* Request Major Deprtment No. Entry */
		break;

    case DPT_FIELD8_ADR:  /* status 7 */
        MaintDept.uchMinorClass = CLASS_PARADEPT_DATA;
        MaintDept.ulInputData = ( ULONG)(MaintWork.DEPT.auchDEPTStatus[(uchFieldAddr - 2) / 2]) & 0x0F;
		RflGetLead(MaintDept.aszMnemonics, LDT_STATUS_ADR);      /* Request Status Entry */
		break;

    case DPT_FIELD2_ADR:  /* status 1 */
    case DPT_FIELD4_ADR:  /* status 3 */
    case DPT_FIELD6_ADR:  /* status 5 */
        MaintDept.uchMinorClass = CLASS_PARADEPT_STS;
        MaintDept.ulInputData = ( ULONG)(MaintWork.DEPT.auchDEPTStatus[(uchFieldAddr - 2) / 2]) & 0x0F;
		RflGetLead(MaintDept.aszMnemonics, LDT_STATUS_ADR);      /* Request Status Entry */
		break;

    case DPT_FIELD3_ADR:  /* status 2 */
    case DPT_FIELD5_ADR:  /* status 4 */
    case DPT_FIELD7_ADR:  /* status 6 */
    case DPT_FIELD9_ADR:  /* status 8 */
        MaintDept.uchMinorClass = CLASS_PARADEPT_STS;
        MaintDept.ulInputData = ( ULONG)(MaintWork.DEPT.auchDEPTStatus[(uchFieldAddr - 3) / 2]) & 0xF0;
        MaintDept.ulInputData >>= 4;
		RflGetLead(MaintDept.aszMnemonics, LDT_STATUS_ADR);      /* Request Status Entry */
		break;

    case DPT_FIELD10_ADR: /* HALO */
        MaintDept.uchMinorClass = CLASS_PARADEPT_DATA;
        MaintDept.ulInputData = ( ULONG)(MaintWork.DEPT.uchHALO);
		RflGetLead(MaintDept.aszMnemonics, LDT_HALO_ADR);       /* Request HALO Data */
		break;

    case DPT_FIELD11_ADR: /* mnemonics */
        MaintDept.uchMinorClass = CLASS_PARADEPT_DATA;
        MaintDept.ulInputData = 0;
		RflCopyMnemonic(MaintDept.aszMnemonics, MaintWork.DEPT.aszMnemonic, PARA_DEPT_LEN);
        break;

    case DPT_FIELD12_ADR: /* Print Priority */
        MaintDept.uchMinorClass = CLASS_PARADEPT_DATA;
        MaintDept.ulInputData = ( ULONG)(MaintWork.DEPT.uchPrintPriority);
		RflGetLead(MaintDept.aszMnemonics, LDT_NUM_ADR);       /* Request Enter Number */
		break;

    case DPT_FIELD13_ADR:  /* status 9 */
        MaintDept.uchMinorClass = CLASS_PARADEPT_STS;
        MaintDept.ulInputData = ( ULONG)(MaintWork.DEPT.auchDEPTStatus[(uchFieldAddr - 5) / 2]) & 0x0F;
		RflGetLead(MaintDept.aszMnemonics, LDT_STATUS_ADR);      /* Request Status Entry */
		break;

    case DPT_FIELD14_ADR:  /* status 10 */
        MaintDept.uchMinorClass = CLASS_PARADEPT_STS;
        MaintDept.ulInputData = ( ULONG)(MaintWork.DEPT.auchDEPTStatus[(uchFieldAddr - 5) / 2]) & 0xF0;
        MaintDept.ulInputData >>= 4;
		RflGetLead(MaintDept.aszMnemonics, LDT_STATUS_ADR);      /* Request Status Entry */
		break;

    default:
/*        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
        return;   
    }

    MaintDept.uchMajorClass = CLASS_PARADEPT;                   /* set major class */
    DispWrite(&MaintDept);
}

/*
*======================================================================================
**   Synopsis:  SHORT MaintDEPTDelete( PARADEPT *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key PARADEPT
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Cashier No. Delete Module  
*======================================================================================
*/

SHORT MaintDEPTDelete( PARADEPT *pData )
{
    SHORT           sError;
	MAINTTRANS      MaintTrans = { 0 };
	DEPTIF          DeptIf = { 0 };
	TTLDEPT         TtlDept = { 0 };
    
    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    } 
    DeptIf.usDeptNo = pData->usDEPTNumber;

    /* read data */
    if ((sError = CliOpDeptIndRead(&DeptIf, usMaintLockHnd)) != OP_PERFORM) {
        return(OpConvertError(sError));
    }

    /* Check Daily,PTD Current Total File */
    TtlDept.uchMajorClass = CLASS_TTLDEPT;
    TtlDept.uchMinorClass = CLASS_TTLCURDAY;
    TtlDept.usDEPTNumber = pData->usDEPTNumber;
    if ((sError = SerTtlTotalRead(&TtlDept)) == TTL_SUCCESS) {
        return(LDT_TLCOACT_ADR);                                        /* Total Counter Active */      
    }else
	{
		PifLog(MODULE_DEPT, LOG_ERROR_DEPT_READ_CURDAY);
	}


    TtlDept.uchMinorClass = CLASS_TTLCURPTD;
    if ((sError = SerTtlTotalRead(&TtlDept)) == TTL_SUCCESS) {
        return(LDT_TLCOACT_ADR);                                        /* Total Counter Active */      
    }else
	{
		PifLog(MODULE_DEPT, LOG_ERROR_DEPT_READ_CURPTD);
	}

    /* Check Daily,PTD Saved Total File */
    TtlDept.uchMinorClass = CLASS_TTLSAVDAY;
    TtlDept.usDEPTNumber = pData->usDEPTNumber;
    if ((sError = SerTtlTotalRead(&TtlDept)) == TTL_SUCCESS) {

        /* Check Daily Saved Reset Flag */
        if ((sError = SerTtlIssuedCheck(&TtlDept, TTL_NOTRESET)) == TTL_NOT_ISSUED) {   /* Reset Report Not Issued */
            return(TtlConvertError(sError));                                    
        }
    }else
	{
		PifLog(MODULE_DEPT, LOG_ERROR_DEPT_READ_SAVDAY);
	}

    TtlDept.uchMinorClass = CLASS_TTLSAVPTD;
    if ((sError = SerTtlTotalRead(&TtlDept)) == TTL_SUCCESS) {

        /* Check PTD Saved Reset Flag */
        if ((sError = SerTtlIssuedCheck(&TtlDept, TTL_NOTRESET)) == TTL_NOT_ISSUED) {   /* Reset Report Not Issued */
            return(TtlConvertError(sError));                                    
        }
    }else
	{
		PifLog(MODULE_DEPT, LOG_ERROR_DEPT_READ_SAVPTD);
	}

    /* Delete Dept Parameter and Saved Total */
    if ((sError = SerOpDeptDelete(&DeptIf, usMaintLockHnd)) != OP_PERFORM) {
        return(OpConvertError(sError));
    }

    /* Check Operational Times */
    if ((uchMaintOpeCo & MAINT_ALREADYOPERATE)) {       /* in case of after 1st operation */
        MaintFeed();        /* 1 line feed */
    }                                                        

    /* Control Header Item */
    MaintHeaderCtl(((UCHAR)(AC_DEPT_MAINT)), ((UCHAR)(RPT_ACT_ADR)));                      
    
    /* Print Deleted Mnemonics */
    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
	RflGetTranMnem(MaintTrans.aszTransMnemo, TRN_DEL_ADR);
    MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );  

    PrtPrintItem(NULL, &MaintTrans);

    /* set data for printing */
    MaintWork.DEPT.uchMajorClass = CLASS_PARADEPT;
    MaintWork.DEPT.usDEPTNumber = DeptIf.usDeptNo;
    MaintWork.DEPT.uchMajorDEPT = DeptIf.ParaDept.uchMdept;
    memcpy(MaintWork.DEPT.auchDEPTStatus, DeptIf.ParaDept.auchControlCode, MAX_DEPTSTS_SIZE);
    MaintWork.DEPT.uchHALO = DeptIf.ParaDept.auchHalo[0];
    MaintWork.DEPT.uchPrintPriority = DeptIf.ParaDept.uchPrintPriority;
	RflCopyMnemonic(MaintWork.DEPT.aszMnemonic, DeptIf.ParaDept.auchMnemonic, PARA_DEPT_LEN);
    MaintWork.DEPT.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.DEPT));

    /* send print data to shared module */
    PrtShrPartialSend(MaintWork.DEPT.usPrintControl);

    return(SUCCESS);
}


