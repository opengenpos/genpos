/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintCSTR Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATCSTR.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls CSTR PARAMETER FILE.
*
*           The provided function names are as follows: 
* 
*             MaintCSTRRead()          : read & display parameter from Cstr file 
*             MaintCSTREdit()          : check input data from UI & set data in self-work area 
*             MaintCSTRWrite()         : write Cstr parameter record in Cstr file & print 
*             MaintCSTRDelete()        : delete corresponding Cstr record from Cstr file  
*             MaintCSTRReport()        : print Cstr report from Cstr file  
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Apr-18-95: 00.00.01 : M.Waki    : initial                                   
* Nov-27-95: 03.01.00 : M.Ozawa   : add FSC_SEAT
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
#include <rfl.h>
#include <log.h>
#include <appllog.h>
#include <fsc.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <rfl.h>
#include <csop.h>
#include <csstbpar.h>
#include <csstbopr.h>
#include "maintram.h" 

/*
*===========================================================================
*   Work Area Declartion 
*===========================================================================
*/

MAINTCSTR MaintCstr;

USHORT    usMaintLength;        /* Control Strings Length */

/*
*===========================================================================
** Synopsis:    SHORT MaintCSTRRead( MAINTCSTR *pData )
*               
*     Input:    *pData         : pointer to structure for PARACSTR
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*               LDT_KEYOVER_ADR: wrong data
*               sReturn        : error status
*
** Description: This function reads & displays parameter from CSTR FILE in self-work area.
*===========================================================================
*/

SHORT MaintCSTRRead( MAINTCSTR *pData )  
{
    SHORT         sReturn;
	CSTRIF         CstrIf = { 0 };
                
    /* initialize */

    memset(&MaintWork, 0, sizeof(MAINTWORK));
    memset(&MaintCstr, 0, sizeof(MAINTCSTR));

    /* check status */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                        /* without data */
        return(LDT_SEQERR_ADR);                                         /* sequence error */
    } 

    /* check CSTR No. range */

    if ((pData->usCstrNumber == 0)||(pData->usCstrNumber > MAINT_CSTR_ADR_MAX)) {
        return(LDT_KEYOVER_ADR);
    }
    CstrIf.usCstrNo = pData->usCstrNumber;

    /* read data */

    sReturn = CliOpCstrIndRead(&CstrIf, usMaintLockHnd);

    /* distinguish return value */

    if (sReturn == OP_PERFORM){
        /* MaintWork Set */
        usMaintLength = CstrIf.IndexCstr.usCstrLength;  /* Length get */
        MaintWork.CSTR.uchMajorClass = CLASS_MAINTCSTR;
        MaintWork.CSTR.usCstrNumber = CstrIf.usCstrNo;
        MaintWork.CSTR.uchAddr = pData->uchAddr;    /* Address = 1 */
        MaintWork.CSTR.usCstrLength = CstrIf.IndexCstr.usCstrLength;    /* Length get */
        memcpy(&MaintWork.CSTR.ParaCstr[0], &CstrIf.ParaCstr[0], CstrIf.IndexCstr.usCstrLength);
        MaintWork.CSTR.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    } else {
        /* convert error status */
        return(OpConvertError(sReturn));
    }

    /* read & set LEADTHRU */
	RflGetLead(MaintCstr.aszMnemonics, LDT_NUM_ADR);

    MaintCstr.uchMajorClass = CLASS_MAINTCSTR;
    MaintCstr.uchMinorClass = CLASS_PARACSTR_MAINT_READ;
    MaintCstr.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;
    MaintCstr.usCstrNumber = MaintWork.CSTR.usCstrNumber;
    MaintCstr.uchACNumber = AC_CSTR_MAINT;
    MaintCstr.uchAddr = pData->uchAddr;
    MaintCstr.uchMajorCstrData = MaintWork.CSTR.ParaCstr[pData->uchAddr-1].uchMajorCstrData;
    if (MaintCstr.uchMajorCstrData >= UIE_FSC_BIAS) {   /* Programable FSC ? */
        MaintCstr.uchMajorCstrData -= UIE_FSC_BIAS;     /* -40h */
        MaintCstr.uchMajorCstrData += MAINT_FSC_BIAS;   /* +100d */
    }
    MaintCstr.uchMinorCstrData = MaintWork.CSTR.ParaCstr[pData->uchAddr-1].uchMinorCstrData;
    /* distinguish '0' whether numeric data or teminal */
    if ( MaintCstr.uchMajorCstrData == 0 ) {
        MaintCstr.uchStatus |= MAINT_WITHOUT_DATA;
    } else {
        MaintCstr.uchStatus &= ~MAINT_WITHOUT_DATA;
    }
    if (MaintCstr.uchMajorCstrData == FSC_AN) {     /* Number Key (0~9) */
        if ((MaintCstr.uchMinorCstrData >= 0x30)	/* --- Feb/17/2001 DTREE Issue#92 --- */
                && (MaintCstr.uchMinorCstrData < 0x3A)) {
            MaintCstr.uchMajorCstrData = (MaintCstr.uchMinorCstrData - (UCHAR)MAINT_NUM_BIAS); /* Minus 0x30 */
            MaintCstr.uchMinorCstrData = 0;
        }
    }
    DispWrite(&MaintCstr);
    return(SUCCESS);
}


/*
*===========================================================================
** Synopsis:    SHORT MaintCSTREdit( MAINTCSTR *pData )
*               
*     Input:    *pData              : pointer to structure for MAINTCSTR
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS             : successful
*               LDT_SEQERR_ADR      : sequence error
*               LDT_KEYOVER_ADR     : wrong data
*               MAINT_CSTR_MNEMO_ADR : set mnemonics 
*
** Description: This function reads & displays parameter from CSTR FILE in self-work area.
*===========================================================================
*/

SHORT MaintCSTREdit( MAINTCSTR *pData )
{
    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                    /* without data */

        /* distinguish minor class */
        switch(pData->uchMinorClass) {
        case CLASS_PARACSTR_MAINT_READ:  
            MaintWork.CSTR.uchAddr++;
            MaintCstr.uchAddr = MaintWork.CSTR.uchAddr;

            /* check CSTR type & address */

            if ((MaintCstr.uchAddr > (UCHAR)((usMaintLength / 2)+1))||
                (MaintCstr.uchAddr > MAINT_CSTR_ADR_MAX)) {
                MaintWork.CSTR.uchAddr = 1;
                MaintCstr.uchAddr = 1;             /* initialize address */
            }

            /* set data for display next old data */
            /* read & set LEADTHRU */
            RflGetLead (MaintCstr.aszMnemonics, LDT_NUM_ADR); /* Number Entry */

            MaintCstr.uchMajorClass = CLASS_MAINTCSTR;
            MaintCstr.uchMinorClass = CLASS_PARACSTR_MAINT_READ;
            MaintCstr.usCstrNumber = MaintWork.CSTR.usCstrNumber;
            MaintCstr.uchACNumber = AC_CSTR_MAINT;
            MaintCstr.uchMajorCstrData = MaintWork.CSTR.ParaCstr[MaintCstr.uchAddr-1].uchMajorCstrData;
            if (MaintCstr.uchMajorCstrData >= UIE_FSC_BIAS) {   /* Programable FSC ? */
                MaintCstr.uchMajorCstrData -= UIE_FSC_BIAS;     /* -40h */
                MaintCstr.uchMajorCstrData += MAINT_FSC_BIAS;   /* +100d */
            }
            MaintCstr.uchMinorCstrData = MaintWork.CSTR.ParaCstr[MaintCstr.uchAddr-1].uchMinorCstrData;
            /* distinguish '0' whether numeric data or teminal */
            if ( MaintCstr.uchMajorCstrData == 0 ) {
                MaintCstr.uchStatus |= MAINT_WITHOUT_DATA;
            } else {
                MaintCstr.uchStatus &= ~MAINT_WITHOUT_DATA;
            }
            if (MaintCstr.uchMajorCstrData == FSC_AN) {     /* Number Key (0~9) */
                if ((MaintCstr.uchMinorCstrData >= 0x30) 	/* --- Feb/17/2001 DTREE Issue#92 --- */
                        && (MaintCstr.uchMinorCstrData < 0x3A)) {
                    MaintCstr.uchMajorCstrData = (MaintCstr.uchMinorCstrData - (UCHAR)0x30);
                    MaintCstr.uchMinorCstrData = 0;
                }
            }
            DispWrite(&MaintCstr);

            return(SUCCESS);

        default: 
/*            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
            return(LDT_SEQERR_ADR);

        }
    } else {                /* with data */
        /* distinguish minor class */
        switch(pData->uchMinorClass) {

        case CLASS_PARACSTR_MAINT_READ:
            /* check CSTR  address */
            if ((pData->uchAddr < 1) ||
                (pData->uchAddr > (UCHAR)((usMaintLength / 2)+1)) ||
                (pData->uchAddr > MAINT_CSTR_ADR_MAX)) {
                return(LDT_KEYOVER_ADR);
            }
            MaintCstr.uchAddr = pData->uchAddr;
            MaintWork.CSTR.uchAddr = pData->uchAddr;

            /* set data for display next old data */
            /* read & set LEADTHRU */
            RflGetLead (MaintCstr.aszMnemonics, LDT_NUM_ADR);  /* Number Entry */

            MaintCstr.uchMajorClass = CLASS_MAINTCSTR;
            MaintCstr.uchMinorClass = CLASS_PARACSTR_MAINT_READ;
            MaintCstr.usCstrNumber = MaintWork.CSTR.usCstrNumber;
            MaintCstr.uchACNumber = AC_CSTR_MAINT;
            MaintCstr.uchMajorCstrData = MaintWork.CSTR.ParaCstr[MaintCstr.uchAddr-1].uchMajorCstrData;
            if (MaintCstr.uchMajorCstrData >= UIE_FSC_BIAS) {   /* Programable FSC ? */
                MaintCstr.uchMajorCstrData -= UIE_FSC_BIAS;     /* -40h */
                MaintCstr.uchMajorCstrData += MAINT_FSC_BIAS;   /* +100d */
            }
            MaintCstr.uchMinorCstrData = MaintWork.CSTR.ParaCstr[MaintCstr.uchAddr-1].uchMinorCstrData;
            /* distinguish '0' whether numeric data or teminal */
            if ( MaintCstr.uchMajorCstrData == 0 ) {
                MaintCstr.uchStatus |= MAINT_WITHOUT_DATA;
            } else {
                MaintCstr.uchStatus &= ~MAINT_WITHOUT_DATA;
            }
            if (MaintCstr.uchMajorCstrData == FSC_AN) {     /* Number Key (0~9) */
                if ((MaintCstr.uchMinorCstrData >= 0x30)	/* --- Feb/17/2001 DTREE Issue#92 --- */
                        && (MaintCstr.uchMinorCstrData < 0x3A)) {
                    MaintCstr.uchMajorCstrData = (MaintCstr.uchMinorCstrData - (UCHAR)MAINT_NUM_BIAS);
                    MaintCstr.uchMinorCstrData = 0;
                }
            }
            DispWrite(&MaintCstr);

            return(SUCCESS);

        case CLASS_PARACSTR_MAINT_FSC_WRITE:
            /* Input FSC Key Check */
            if(((pData->uchInputData < MAINT_FSC_BIAS)&&(pData->uchInputData > FSC_MODE))||
               ((pData->uchInputData < FSC_MODE)&&(pData->uchInputData > FSC_P5))||
               ((pData->uchInputData < FSC_PAUSE)&&(pData->uchInputData > MAINT_CSTR_NUMERIC_MAX))||
                (pData->uchInputData == FSC_KEYED_STRING - UIE_FSC_BIAS + MAINT_FSC_BIAS)||
                (pData->uchInputData == FSC_STRING - UIE_FSC_BIAS + MAINT_FSC_BIAS)||
                (pData->uchInputData == FSC_AN)){
                return(LDT_PROHBT_ADR);
            }
            MaintCstr.uchMajorCstrData = pData->uchInputData;
            if(pData->uchInputData >= MAINT_FSC_BIAS) {
                MaintCstr.uchMajorCstrData -= MAINT_FSC_BIAS;
                MaintCstr.uchMajorCstrData += UIE_FSC_BIAS;
                MaintCstr.uchMinorCstrData = 0; /* Clear */
            } else if(pData->uchInputData <= 9) {
                MaintCstr.uchMajorCstrData = FSC_AN;
                MaintCstr.uchMinorCstrData = pData->uchInputData + (UCHAR)MAINT_NUM_BIAS;
            } else {
                MaintCstr.uchMajorCstrData = pData->uchInputData;
                MaintCstr.uchMinorCstrData = 0; /* Clear */
            }
            MaintWork.CSTR.ParaCstr[MaintCstr.uchAddr-1].uchMajorCstrData = MaintCstr.uchMajorCstrData;
            MaintWork.CSTR.ParaCstr[MaintCstr.uchAddr-1].uchMinorCstrData = MaintCstr.uchMinorCstrData;

            /* read & set LEADTHRU */
            RflGetLead (MaintCstr.aszMnemonics, LDT_NUM_ADR);   /* Number Entry */

            switch(MaintCstr.uchMajorCstrData){
            case FSC_PAUSE:
            case FSC_CALL_STRING:
            case FSC_MODE:
            case FSC_KEYED_PLU:
            case FSC_KEYED_DEPT:
            case FSC_PRINT_MODIF:
            case FSC_ADJECTIVE:
            case FSC_TENDER:
            case FSC_FOREIGN:
            case FSC_TOTAL:
            case FSC_ADD_CHECK:
            case FSC_ITEM_DISC:
            case FSC_REGULAR_DISC:
            case FSC_TAX_MODIF:
            case FSC_PRT_DEMAND:
            case FSC_PRESET_AMT:
            case FSC_D_MENU_SHIFT:
            case FSC_KEYED_COUPON:      /* Exist Extend Key */
            case FSC_SEAT:
            case FSC_CHARGE_TIPS:
            case FSC_ASK:
            case FSC_ADJ_SHIFT:
            
                /* set data for display */
                MaintCstr.uchMajorClass = pData->uchMajorClass;
                MaintCstr.uchMinorClass = pData->uchMinorClass;
                MaintCstr.uchMajorCstrData = pData->uchInputData;
                MaintCstr.uchMinorCstrData = 0;
                MaintCstr.uchStatus &= ~MAINT_WITHOUT_DATA;
                DispWrite(&MaintCstr);
                return(MAINT_FSC_EXIST);

            default:                    /* Not Exist Extend */

               if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
                   MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */

                   /* make header */
                   MaintMakeHeader(CLASS_MAINTHEADER_RPT,    /* minor class */
                                   RPT_ACT_ADR,              /* "ACT #" */
                                   0,
                                   0,                        
                                   RPT_MAINT_ADR,             /* "MAINT" */
                                   0,
                                   AC_CSTR_MAINT,            /* action number(33) */
                                   0,
                                   PRT_RECEIPT | PRT_JOURNAL); /* print control */
                   uchMaintOpeCo |= MAINT_ALREADYOPERATE;
                }
                if (uchMaintOpeCo & MAINT_WRITECOMPLETE) {
                    MaintFeed();        /* feed 1 line between each control string */
                    uchMaintOpeCo &= ~MAINT_WRITECOMPLETE;
                }

                /* Modified Data Print Out */
                MaintCstr.uchMajorClass = pData->uchMajorClass;
                MaintCstr.uchMinorClass = pData->uchMinorClass;
                MaintCstr.uchMajorCstrData = pData->uchInputData;
                MaintCstr.uchMinorCstrData = 0;
                PrtPrintItem(NULL, &MaintCstr);
                /* send print data to shared module */
                PrtShrPartialSend(MaintCstr.usPrintControl);

                /* Next Old Data Display */
                MaintCstr.uchMajorClass = pData->uchMajorClass;
                MaintCstr.uchMinorClass = pData->uchMinorClass;
                if(MaintCstr.uchAddr == (UCHAR)((usMaintLength / 2)+1)){
                    usMaintLength += 2;
                }
                MaintCstr.uchAddr++;
                if (MaintCstr.uchAddr > MAINT_CSTR_ADR_MAX) {
                    MaintWork.CSTR.uchAddr = 1;
                    MaintCstr.uchAddr = 1;             /* initialize address */
                }
                MaintCstr.uchMajorCstrData = MaintWork.CSTR.ParaCstr[MaintCstr.uchAddr-1].uchMajorCstrData;
                MaintCstr.uchMinorCstrData = MaintWork.CSTR.ParaCstr[MaintCstr.uchAddr-1].uchMinorCstrData;
                if (MaintCstr.uchMajorCstrData >= UIE_FSC_BIAS) {   /* Programable FSC ? */
                    MaintCstr.uchMajorCstrData -= UIE_FSC_BIAS;     /* -40h */
                    MaintCstr.uchMajorCstrData += MAINT_FSC_BIAS;   /* +100d */
                }
                /* distinguish '0' whether numeric data or teminal */
                if ( MaintCstr.uchMajorCstrData == 0 ) {
                    MaintCstr.uchStatus |= MAINT_WITHOUT_DATA;
                } else {
                    MaintCstr.uchStatus &= ~MAINT_WITHOUT_DATA;
                }
                if (MaintCstr.uchMajorCstrData == FSC_AN) {     /* Number Key (0~9) */
                    if ((MaintCstr.uchMinorCstrData >= 0x30) 	/* --- Feb/17/2001 DTREE Issue#92 --- */
                            && (MaintCstr.uchMinorCstrData < 0x3A)) {
                        MaintCstr.uchMajorCstrData = (MaintCstr.uchMinorCstrData - (UCHAR)MAINT_NUM_BIAS);
                        MaintCstr.uchMinorCstrData = 0;
                    }
                }
                DispWrite(&MaintCstr);
                return(SUCCESS);
                
            }

        case CLASS_PARACSTR_MAINT_EXT_WRITE:
            switch(MaintWork.CSTR.ParaCstr[MaintCstr.uchAddr-1].uchMajorCstrData){
            case FSC_PAUSE:
                if((pData->uchInputData < 1) || (pData->uchInputData > MAINT_CSTR_PAUSE_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_CALL_STRING:
                if((pData->uchInputData < 1) || (pData->uchInputData > MAINT_CSTR_ADR_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_MODE:
                if((pData->uchInputData < MAINT_CSTR_MODE_MIN) || (pData->uchInputData > MAINT_CSTR_MODE_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_KEYED_PLU:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_PLU_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_KEYED_DEPT:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_DEPT_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_PRINT_MODIF:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_PM_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_ADJECTIVE:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_ADJ_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_TENDER:
				/* temporary hardcoded 11, change to FSC_TEND_MAX when implementing full
				tender database change JHHJ 3-16-04*/
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_TEND_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_FOREIGN:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_FC_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_TOTAL:
				/* temporary hardcoded 9, change to FSC_TOTAL_MAX when implementing full
				tender database change JHHJ 3-24-04*/
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_TOTAL_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_ADD_CHECK:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_ADDCHK_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_ITEM_DISC:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_IDISC_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_REGULAR_DISC:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_RDISC_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_TAX_MODIF:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_TAXMD_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_PRT_DEMAND:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_PRTDMD_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_PRESET_AMT:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_PREAMT_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_D_MENU_SHIFT:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_DMSFT_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_KEYED_COUPON:
                if((pData->uchInputData < 1) || (pData->uchInputData > FLEX_CPN_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_SEAT:  /* R3.1 */
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_SEAT_MAX)){
                    return(LDT_KEYOVER_ADR);
                }
                break;
            case FSC_CHARGE_TIPS:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_CHGTIP_MAX)){
                    return(LDT_KEYOVER_ADR);                        /* wrong data */
                }
                break;

            case FSC_ASK:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_ASK_MAX)){
                    return(LDT_KEYOVER_ADR);                        /* wrong data */
                }
                break;

            case FSC_ADJ_SHIFT:
                if((pData->uchInputData < 1) || (pData->uchInputData > FSC_ADJSFT_MAX)){
                    return(LDT_KEYOVER_ADR);                        /* wrong data */
                }
                break;
            default:
                return(LDT_SEQERR_ADR);
            }
            
            MaintCstr.uchMinorCstrData = pData->uchInputData;
            MaintWork.CSTR.ParaCstr[MaintCstr.uchAddr-1].uchMinorCstrData = MaintCstr.uchMinorCstrData;

            /* make header */
            if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
                MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */
                /* make header */
                MaintMakeHeader(CLASS_MAINTHEADER_RPT,    /* minor class */
                                RPT_ACT_ADR,              /* "ACT #" */
                                0,
                                0,                        
                                RPT_MAINT_ADR,             /* "MAINT" */
                                0,
                                AC_CSTR_MAINT,            /* action number(33) */
                                0,
                                PRT_RECEIPT | PRT_JOURNAL); /* print control */
                uchMaintOpeCo |= MAINT_ALREADYOPERATE;
            }
            if (uchMaintOpeCo & MAINT_WRITECOMPLETE) {
                MaintFeed();        /* feed 1 line between each control string */
                uchMaintOpeCo &= ~MAINT_WRITECOMPLETE;
            }

            /* read & set LEADTHRU */
            RflGetLead (MaintCstr.aszMnemonics, LDT_NUM_ADR);   /* Number Entry */

            /* Modified Data Print Out */
            MaintCstr.uchMajorClass = pData->uchMajorClass;
            MaintCstr.uchMinorClass = pData->uchMinorClass;
            MaintCstr.uchMinorCstrData = pData->uchInputData;
            PrtPrintItem(NULL, &MaintCstr);
            /* send print data to shared module */
            PrtShrPartialSend(MaintCstr.usPrintControl);

            /* Next Old Data Display */
            MaintCstr.uchMajorClass = pData->uchMajorClass;
            MaintCstr.uchMinorClass = pData->uchMinorClass;
            if(MaintCstr.uchAddr == (UCHAR)((usMaintLength / 2)+1)){
                usMaintLength += 2;
            }
            MaintCstr.uchAddr++;
            if (MaintCstr.uchAddr > MAINT_CSTR_ADR_MAX) {
                MaintWork.CSTR.uchAddr = 1;
                MaintCstr.uchAddr = 1;             /* initialize address */
            }
            MaintCstr.uchMajorCstrData = MaintWork.CSTR.ParaCstr[MaintCstr.uchAddr-1].uchMajorCstrData;
            MaintCstr.uchMinorCstrData = MaintWork.CSTR.ParaCstr[MaintCstr.uchAddr-1].uchMinorCstrData;
            if (MaintCstr.uchMajorCstrData >= UIE_FSC_BIAS) {   /* Programable FSC ? */
                MaintCstr.uchMajorCstrData -= UIE_FSC_BIAS;     /* -40h */
                MaintCstr.uchMajorCstrData += MAINT_FSC_BIAS;   /* +100d */
            }
            /* distinguish '0' whether numeric data or teminal */
            if ( MaintCstr.uchMajorCstrData == 0 ) {
                MaintCstr.uchStatus |= MAINT_WITHOUT_DATA;
            } else {
                MaintCstr.uchStatus &= ~MAINT_WITHOUT_DATA;
            }
            if (MaintCstr.uchMajorCstrData == FSC_AN) {     /* Number Key (0~9) */
                if ((MaintCstr.uchMinorCstrData >= 0x30)	/* --- Feb/17/2001 DTREE Issue#92 --- */
                        && (MaintCstr.uchMinorCstrData < 0x3A)) {
                    MaintCstr.uchMajorCstrData = (MaintCstr.uchMinorCstrData - (UCHAR)MAINT_NUM_BIAS);
                    MaintCstr.uchMinorCstrData = 0;
                }
            }
            DispWrite(&MaintCstr);
            return(SUCCESS);

        default:
/*            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
            return(LDT_KEYOVER_ADR);
        }
    }
}


/*
*===========================================================================
** Synopsis:    SHORT MaintCSTRReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS : successful
*               sReturn : error
*
** Description: This function prints CSTR report from CSTR file. 
*===========================================================================
*/

SHORT MaintCSTRReport( VOID )
{
    SHORT           sReturn = SUCCESS;
    CSTRIF           CstrIf;
    UCHAR           i;

    /* initialize */

    memset(&CstrIf, 0, sizeof(CSTRIF));

    if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */
        /* make header */
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,    /* minor class */
                        RPT_ACT_ADR,              /* "ACT #" */
                        0,
                        0,                        
                        RPT_FILE_ADR,             /* "FILE" */
                        0,
                        AC_CSTR_MAINT,            /* action number(33) */
                        0,
                        PRT_RECEIPT | PRT_JOURNAL); /* print control */
         uchMaintOpeCo |= MAINT_ALREADYOPERATE;
    }
    uchMaintOpeCo &= ~MAINT_WRITECOMPLETE;

    /* set print control */
    MaintCstr.usPrintControl = 0;

    if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT0))) {
        MaintCstr.usPrintControl |= PRT_JOURNAL;   /* print out on journal */
    } 
    if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT1))) {
        MaintCstr.usPrintControl |= PRT_RECEIPT;   /* print out receipt */
    }


    MaintCstr.uchMajorClass = CLASS_MAINTCSTR;
    MaintCstr.uchMinorClass = CLASS_PARACSTR_MAINT_REPORT;

    for (CstrIf.usCstrNo = 1 ; CstrIf.usCstrNo <= OP_CSTR_PARA_SIZE ; CstrIf.usCstrNo++){
        if((sReturn = CliOpCstrIndRead(&CstrIf, usMaintLockHnd)) != OP_PERFORM){
            break;
        }
        if(CstrIf.IndexCstr.usCstrLength == 0){
            continue;
        }
        for (i=0 ; i <= (UCHAR)((CstrIf.IndexCstr.usCstrLength/2)-1) ; i++){
            MaintCstr.uchAddr = i + (UCHAR)1;  /* address */
            MaintCstr.usCstrNumber = CstrIf.usCstrNo;
            MaintCstr.uchMajorCstrData = CstrIf.ParaCstr[i].uchMajorCstrData;
            MaintCstr.uchMinorCstrData = CstrIf.ParaCstr[i].uchMinorCstrData;
            if (MaintCstr.uchMajorCstrData >= UIE_FSC_BIAS) {   /* Programable FSC ? */
                MaintCstr.uchMajorCstrData -= UIE_FSC_BIAS;     /* -40h */
                MaintCstr.uchMajorCstrData += MAINT_FSC_BIAS;   /* +100d */
            }
            if (MaintCstr.uchMajorCstrData == FSC_AN) {     /* Number Key (0~9) */
                if ((MaintCstr.uchMinorCstrData >= 0x30) 	/* --- Feb/17/2001 DTREE Issue#92 --- */
                        && (MaintCstr.uchMinorCstrData < 0x3A)) {
                    MaintCstr.uchMajorCstrData = (MaintCstr.uchMinorCstrData - (UCHAR)MAINT_NUM_BIAS);
                    MaintCstr.uchMinorCstrData = 0;
                }
            }
            PrtPrintItem(NULL, &MaintCstr);
            /* send print data to shared module */
            PrtShrPartialSend(MaintCstr.usPrintControl);
            if ((sReturn = UieReadAbortKey()) == UIE_ENABLE) {                  /* depress abort key */
                MaintMakeAbortKey();
                break;
            }
        }
        if(sReturn == UIE_ENABLE){
            sReturn = SUCCESS;
            break;
        }
        MaintFeed();
    }
    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(OpConvertError(sReturn));
}



/*
*===========================================================================
** Synopsis:    SHORT MaintCSTRWrite( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               sReturn        : error
*
** Description: This function writes & prints CSTR PARAMETER RECORD of self-work area in CSTR PARAMETER FILE.
*===========================================================================
*/

SHORT MaintCSTRWrite( VOID )
{

    SHORT           sReturn;
    CSTRIF           CstrIf;

    if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
        if (!(uchMaintOpeCo & MAINT_WRITECOMPLETE)) {
            return(SUCCESS);    /* not write uncompleted data */
        }
    }
    else if (uchMaintOpeCo & MAINT_WRITECOMPLETE) {
        return(SUCCESS);        /* not write uncompleted data */
    }

    CstrIf.usCstrNo = MaintWork.CSTR.usCstrNumber;
    CstrIf.IndexCstr.usCstrLength = usMaintLength;
    memcpy(&CstrIf.ParaCstr[0], &MaintWork.CSTR.ParaCstr[0], CstrIf.IndexCstr.usCstrLength);
    sReturn = SerOpCstrAssign(&CstrIf, usMaintLockHnd);
    if (sReturn == OP_PERFORM) {
        uchMaintOpeCo |= MAINT_WRITECOMPLETE;   /* set flag for not print abotred */
    }
    return(OpConvertError(sReturn));
}

/*
*===========================================================================
** Synopsis:    SHORT MaintCSTRDelete( PARACSTR )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      sReturn     : error 
*               LDT_ERR_ADR : error
*
** Description: This function deletes corresponding Cstr record from Cstr file.  
*===========================================================================
*/

SHORT MaintCSTRDelete( MAINTCSTR *pData)
{
    SHORT           sReturn;
	CSTRIF          CstrIf = { 0 };
	MAINTTRANS      MaintTrans = { 0 };

    /* read string data is existed or not */
    CstrIf.usCstrNo = pData->usCstrNumber;

    if((sReturn = CliOpCstrIndRead(&CstrIf, usMaintLockHnd)) != OP_PERFORM){
        return(OpConvertError(sReturn));
    }
    if(CstrIf.IndexCstr.usCstrLength == 0){
        return (LDT_NTINFL_ADR);
    }

    sReturn = SerOpCstrDelete(&CstrIf, usMaintLockHnd);

    if (sReturn == OP_PERFORM) {

        /* make header */
        if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
            MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */

            MaintMakeHeader(CLASS_MAINTHEADER_RPT,    /* minor class */
                            RPT_ACT_ADR,              /* "ACT #" */
                            0,
                            0,                        
                            RPT_MAINT_ADR,             /* "MAINT" */
                            0,
                            AC_CSTR_MAINT,            /* action number(33) */
                            0,
                            PRT_RECEIPT | PRT_JOURNAL); /* print control */
            uchMaintOpeCo |= MAINT_ALREADYOPERATE;
        }

        if (uchMaintOpeCo & MAINT_WRITECOMPLETE) {
            MaintFeed();        /* feed 1 line between each control string */
        }

        /* print "DELETE" using structure MAINTTRANS */
        MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
        MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );
		RflGetTranMnem(MaintTrans.aszTransMnemo, TRN_DEL_ADR);

        PrtPrintItem(NULL, &MaintTrans);
        /* send print data to shared module */
        PrtShrPartialSend(MaintTrans.usPrtControl);

        /* Deleted Number Data Print Out */
        MaintCstr.uchMajorClass = CLASS_MAINTCSTR;
        MaintCstr.uchMinorClass = CLASS_PARACSTR_MAINT_DELETE;
        MaintCstr.usCstrNumber = pData->usCstrNumber;
        MaintCstr.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
        PrtPrintItem(NULL, &MaintCstr);
        /* send print data to shared module */
        PrtShrPartialSend(MaintCstr.usPrintControl);

        uchMaintOpeCo |= MAINT_WRITECOMPLETE;
    }
    return(OpConvertError(sReturn));
}    
/*
*===========================================================================
** Synopsis:    SHORT  MaintCstrLock( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*
** Return:      SUCCESS         : Successful
*               LDT_LOCK_ADR    : During Lock
*
** Description: Lock Cstr File and Save This Lock Handle to Own Save Area
*===========================================================================
*/

SHORT MaintCstrLock()
{
    SHORT    sError;
    
    if((sError = SerOpLock()) != OP_LOCK){                /* Success            */
        usMaintLockHnd = sError;                        /* Save Lock Handle    */
        return(SUCCESS);
    }
    return(LDT_LOCK_ADR);
}

/*
*===========================================================================
** Synopsis:    VOID  MaintCstrUnLock( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*
** Return:      Nothing
*
** Description: UnLock Cstr File
*===========================================================================
*/

VOID MaintCstrUnLock()
{
    SerOpUnLock();
    usMaintLockHnd = 0;
}

/**** End of File ****/
