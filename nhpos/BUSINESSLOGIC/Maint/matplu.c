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
* Title       : MaintPLU Module  ( SUPER & PROGRAM MODE )
* Category    : Maintenance, NCR 2170 US Hospitality Application Program
* Program Name: MATPLU.C
* --------------------------------------------------------------------------
* Abstract: This function controls PLU PARAMETER FILE.
*
*           The provided function names are as follows:
*
*             MaintPLURead()          : read & display parameter from plu file
*             MaintPLUEdit()          : check input data from UI & set data in self-work area
*             MaintPLUWrite()         : write plu parameter record in plu file & print
*             MaintPLUDelete()        : delete corresponding plu record from plu file
*             MaintPLUReport()        : print plu report from plu file
*             MaintSetPLUDispData()   : display data
*             MaintSetPriceDispData() : display the preset price
*             MaintSetMnemoDispData() : display mnemonic
*             MaintChkPLUType()       : check PLU type
*             MaintCheckBinary()      : check digit                          (move matcom.c later)
*             MaintAtoStatus()        : convert string into bit pattern      (move matcom.c later)
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Jun-15-92: 00.00.01 : J.Ikeda   : initial
* Jul-07-93: 01.00.12 : J.IKEDA   : Maintenance - initialize PARALEADTHRU
*          :          :           : in MaintPLUEdit().
* Aug-25-93: 01.00.13 : J.IKEDA   : Add PrtShrPartialSend() in MaintPLURead()
*          :          :           : & MaintPLUWrite()
* Aug-31-93: 01.00.13 : J.IKEDA   : Add PrtShrPartialSend() in MaintPLUDelete()
* Feb-07-95: 03.00.00 : h.ishida  : Add Linknumber , Class Number
* Nov-22-95: 03.01.00 : M.Ozawa   : Add PPI code, status code
* Dec-13-99: 01.00.00 : hrkato    : Saratoga
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
#include <rfl.h>
#include <log.h>
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
#include <BlFWif.h>

#include <mld.h>

#include "maintram.h"

/*
*===========================================================================
*   Work Area Declartion
*===========================================================================
*/

MAINTPLU MaintPlu;
UCHAR    uchPLUEVersion = 0;

PARAPLU *MaintCopyOpPluParaEntry2ParaPlu(PARAPLU *pPlu, WCHAR * auchPluNo, OPPLU_PARAENTRY *pParaPlu)
{
	memset(pPlu, 0, sizeof(*pPlu));     // initialize receiving buffer to binary zeros for base line

	pPlu->uchMajorClass = CLASS_PARAPLU;
	_tcsncpy(pPlu->auchPLUNumber, auchPluNo, PLU_MAX_DIGIT);
	pPlu->usDEPTNumber = pParaPlu->ContPlu.usDept; /* get out DEPT Number */
	pPlu->uchReportCode = (UCHAR)(pParaPlu->ContPlu.uchRept & PLU_REPORT_MASK); /* get out Report Code */
	memcpy(pPlu->auchPLUStatus, pParaPlu->ContPlu.auchContOther, sizeof(pPlu->auchPLUStatus));
	memcpy(pPlu->auchContOther2, pParaPlu->ContPlu.auchContOther2, sizeof(pPlu->auchContOther2));
	_tcsncpy(pPlu->aszMnemonics, pParaPlu->auchPluName, PARA_PLU_LEN);
	pPlu->usLinkNumber = pParaPlu->usLinkNo; /* 2172 */
	pPlu->uchSalesRestriction = pParaPlu->uchRestrict; /* 2172 */
	pPlu->uchPPICode = (USHORT)pParaPlu->uchLinkPPI; //ESMITH
	pPlu->uchPriceMultiple = pParaPlu->uchPriceMulWeight; /* 2172 */
	pPlu->usFamilyCode = pParaPlu->usFamilyCode; /* 2172 */
	pPlu->uchTableNumber = (UCHAR)pParaPlu->uchTableNumber;                          /* get out Link Number */
	pPlu->uchGroupNumber = (UCHAR)pParaPlu->uchGroupNumber;                        /* get out Class Number */
	pPlu->uchPrintPriority = (UCHAR)pParaPlu->uchPrintPriority;                    /* get out Class Number */
	pPlu->uchPLUType = pParaPlu->ContPlu.uchItemType;       /* get out PLU Type */
	pPlu->uchPLUType >>= 6;
	pPlu->usBonusIndex = pParaPlu->ContPlu.usBonusIndex;
	memcpy(pPlu->auchAlphaNumRouting, pParaPlu->auchAlphaNumRouting, sizeof(pParaPlu->auchAlphaNumRouting));
	_tcsncpy(pPlu->auchAltPluName, pParaPlu->auchAltPluName, PARA_PLU_LEN);
	pPlu->uchColorPaletteCode = pParaPlu->uchColorPaletteCode;
	memcpy(pPlu->auchStatus2, pParaPlu->auchStatus2, sizeof(pParaPlu->auchStatus2));
	pPlu->usTareInformation = pParaPlu->usTareInformation;
	memcpy(pPlu->uchExtendedGroupPriority, pParaPlu->uchExtendedGroupPriority, sizeof(pPlu->uchExtendedGroupPriority));
	memcpy (pPlu->uchPluOptionFlags, pParaPlu->uchPluOptionFlags, sizeof(pPlu->uchPluOptionFlags));

	return pPlu;
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPLURead( PARAPLU *pData )
*
*     Input:    *pData         : pointer to structure for PARAPLU
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*               LDT_KEYOVER_ADR: wrong data
*               sReturn        : error status
*
** Description: This function reads & displays parameter from PLU FILE in self-work area.
*===========================================================================
*/
SHORT MaintPLURead( PARAPLU *pData )
{
    UCHAR         i;
    UCHAR         uchAdj;
    SHORT         sReturn;
    PLUIF         PluIf = { 0 };
    TTLPLU        TtlPlu = { 0 };
	PARAPLU       MldParaPlu = { 0 };   /* For MLD */

    /* initialize */
    memset(&MaintWork, 0, sizeof(MAINTWORK));
    memset(&MaintPlu, 0, sizeof(MAINTPLU));

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                        /* without data */
        uchPLUEVersion = 0;
        return(LDT_SEQERR_ADR);                                         /* sequence error */
    }

    /* check PLU No. range */

    /* Analyze PLU Label & Convert (ParaPLU->aucPLUNumber : PLU number) */
    sReturn = MaintPluLabelAnalysis(pData, uchPLUEVersion);
    uchPLUEVersion = 0;
    if (sReturn != SUCCESS) {
        return(sReturn);
    }
    if (_tcsncmp(pData->auchPLUNumber, MLD_ZERO_PLU, PLU_MAX_DIGIT) == 0) {
        return(LDT_KEYOVER_ADR);
    }
    _tcsncpy(PluIf.auchPluNo, pData->auchPLUNumber, PLU_MAX_DIGIT);
    PluIf.uchPluAdj = 1;

    /*
	 * attempt to read PLU data and based on the return status
	 * do something appropriate.
	*/
    sReturn = CliOpPluIndRead(&PluIf, usMaintLockHnd);
    switch(sReturn) {

    case OP_PERFORM:

        /* set data for printing */
#if 1
		MaintCopyOpPluParaEntry2ParaPlu(&MaintWork.PLU, PluIf.auchPluNo, &PluIf.ParaPlu);
		MaintWork.PLU.usPrintControl = (PRT_JOURNAL | PRT_RECEIPT);
#else
        MaintWork.PLU.uchMajorClass = CLASS_PARAPLU;
        _tcsncpy(MaintWork.PLU.auchPLUNumber, PluIf.auchPluNo, PLU_MAX_DIGIT);
        MaintWork.PLU.usDEPTNumber = PluIf.ParaPlu.ContPlu.usDept; /* get out DEPT Number */
        MaintWork.PLU.uchReportCode = ( UCHAR)(PluIf.ParaPlu.ContPlu.uchRept & PLU_REPORT_MASK); /* get out Report Code */
        memcpy(MaintWork.PLU.auchPLUStatus, PluIf.ParaPlu.ContPlu.auchContOther, sizeof(MaintWork.PLU.auchPLUStatus));
        memcpy(MaintWork.PLU.auchContOther2, PluIf.ParaPlu.ContPlu.auchContOther2, sizeof(MaintWork.PLU.auchContOther2));
        _tcsncpy(MaintWork.PLU.aszMnemonics, PluIf.ParaPlu.auchPluName, PARA_PLU_LEN);
        MaintWork.PLU.aszMnemonics[PARA_PLU_LEN] = 0;  // ensure mnemonic is terminated with binary zero
        MaintWork.PLU.usLinkNumber = PluIf.ParaPlu.usLinkNo; /* 2172 */
        MaintWork.PLU.uchSalesRestriction = PluIf.ParaPlu.uchRestrict; /* 2172 */
        MaintWork.PLU.uchPPICode = ( USHORT)PluIf.ParaPlu.uchLinkPPI; //ESMITH
        MaintWork.PLU.uchPriceMultiple = PluIf.ParaPlu.uchPriceMulWeight; /* 2172 */
        MaintWork.PLU.usFamilyCode = PluIf.ParaPlu.usFamilyCode; /* 2172 */
        MaintWork.PLU.uchTableNumber = ( UCHAR)PluIf.ParaPlu.uchTableNumber;                          /* get out Link Number */
        MaintWork.PLU.uchGroupNumber = ( UCHAR)PluIf.ParaPlu.uchGroupNumber;                        /* get out Class Number */
        MaintWork.PLU.uchPrintPriority = ( UCHAR)PluIf.ParaPlu.uchPrintPriority;                    /* get out Class Number */
        MaintWork.PLU.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
        MaintWork.PLU.uchPLUType =PluIf.ParaPlu.ContPlu.uchItemType;       /* get out PLU Type */
        MaintWork.PLU.uchPLUType >>= 6;
		MaintWork.PLU.usBonusIndex             = PluIf.ParaPlu.ContPlu.usBonusIndex;
		memcpy(MaintWork.PLU.auchAlphaNumRouting, PluIf.ParaPlu.auchAlphaNumRouting, sizeof(PluIf.ParaPlu.auchAlphaNumRouting));
        _tcsncpy(MaintWork.PLU.auchAltPluName, PluIf.ParaPlu.auchAltPluName, PARA_PLU_LEN);
        MaintWork.PLU.auchAltPluName[PARA_PLU_LEN] = 0;  // ensure mnemonic is terminated with binary zero
		MaintWork.PLU.uchColorPaletteCode      = PluIf.ParaPlu.uchColorPaletteCode;
		memcpy(MaintWork.PLU.auchStatus2, PluIf.ParaPlu.auchStatus2, sizeof(PluIf.ParaPlu.auchStatus2));
		MaintWork.PLU.usTareInformation        = PluIf.ParaPlu.usTareInformation;
		memcpy (MaintWork.PLU.uchExtendedGroupPriority, PluIf.ParaPlu.uchExtendedGroupPriority, sizeof(MaintWork.PLU.uchExtendedGroupPriority));
#endif

        /* check if adjective exist or not */

        if (MaintWork.PLU.uchPLUType == PLU_NONADJ) {               /* Non Adjective */
            MaintWork.PLU.uchMinorClass = CLASS_PARAPLU_NONADJ;
            memcpy(MaintWork.PLU.auchPresetPrice[0], PluIf.ParaPlu.auchPrice, OP_PLU_PRICE_SIZE);
        } else if (MaintWork.PLU.uchPLUType == PLU_ONEADJ) {        /* One Adjective */
            MaintWork.PLU.uchMinorClass = CLASS_PARAPLU_ONEADJ;

            for (i = 1; i <= MAX_ADJECTIVE_NO; i++) {
                PluIf.uchPluAdj = i;

                /* read data */
                if ((sReturn = CliOpPluIndRead(&PluIf, usMaintLockHnd)) == OP_PERFORM) {
                    memcpy(MaintWork.PLU.auchPresetPrice[i - 1], PluIf.ParaPlu.auchPrice, OP_PLU_PRICE_SIZE);
                } else {
                    /* convert error status */
                    return(OpConvertError(sReturn));
                }
            }
        } else {                                                    /* Open PLU */
            MaintWork.PLU.uchMinorClass = CLASS_PARAPLU_OPENPLU;

            if (pData->usACNumber == AC_PLU_PRICE_CHG) {
                return(LDT_PROHBT_ADR);                            /* prohibit operation */
            }
        }

        /* check A/C number */
        if (pData->usACNumber != AC_PLU_ADDDEL) {

            /* check operational times */
            if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
                MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */

                /* make header */
                MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* minor class */
                                RPT_ACT_ADR,                            /* "ACT #" */
                                RPT_PLU_ADR,                            /* "PLU" */
                                0,
                                RPT_MAINT_ADR,                          /* "MAINTE" */
                                0,
                                ( UCHAR)(pData->usACNumber),            /* action number */
                                0,
                                PRT_RECEIPT | PRT_JOURNAL); /* print control */
                uchMaintOpeCo |= MAINT_ALREADYOPERATE;
            } else {
                /* 1 line feed */
                MaintFeed();
            }

            /******************/
            /* Display to MLD */
            /******************/
            MldParaPlu = MaintWork.PLU;
            MldParaPlu.usACNumber   = pData->usACNumber;
            MldParaPlu.uchFieldAddr = 0;
            MldDispItem(&MldParaPlu,0);
            uchMaintMldPageNo = 0;  /* display 1st page */

            /* print old data */
            PrtPrintItem(NULL, &(MaintWork.PLU));

            /* send print data to shared module */
            PrtShrPartialSend(MaintWork.PLU.usPrintControl);
        }
        break;

    case OP_NOT_IN_FILE:
        /* check A/C number */
        if (pData->usACNumber != AC_PLU_ADDDEL) {
            return(LDT_NTINFL_ADR);
        }

        /* set PLU number */
        _tcsncpy(MaintWork.PLU.auchPLUNumber, PluIf.auchPluNo, PLU_MAX_DIGIT);

        /******************/
        /* Display to MLD */
        /******************/
        MldParaPlu.uchMajorClass = CLASS_PARAPLU;
        MldParaPlu.uchMinorClass = CLASS_PARAPLU_ADD_READ;
        MldParaPlu.usACNumber    = pData->usACNumber;
        _tcsncpy(MldParaPlu.auchPLUNumber, PluIf.auchPluNo, PLU_MAX_DIGIT);
        MldParaPlu.uchFieldAddr  = 0;
        MldDispItem(&MldParaPlu,0);
        uchMaintMldPageNo = 0;  /* display 1st page */

        /* set data & display */

        MaintPlu.uchMajorClass = CLASS_PARAPLU;
        MaintWork.PLU.uchMajorClass = CLASS_PARAPLU;
        MaintPlu.uchMinorClass = CLASS_PARAPLU_INT;
        MaintPlu.uchFieldAddr = PLU_FIELD0_ADR;
        MaintPlu.usACNumber = pData->usACNumber;
        _tcsncpy(MaintPlu.auchPLUNumber, PluIf.auchPluNo, PLU_MAX_DIGIT);
		RflGetLead(MaintPlu.aszMnemonics, LDT_PLUTYPE_ADR);
        MaintPlu.uchPageNumber = uchMaintMenuPage;
        DispWrite(&MaintPlu);
        return(LDT_NTINFL_ADR);

    default:
        /* convert error status */
        return(OpConvertError(sReturn));
    }

    /* distinguish A/C number & display old data every A/C number */

    switch(pData->usACNumber) {

    case AC_PLU_MAINT:
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.uchFieldAddr = PLU_FIELD1_ADR;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.usDEPTNumber);
        /* MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.uchDEPTNumber); */
		RflGetLead(MaintPlu.aszMnemonics, LDT_DEPTNO_ADR);
		break;

    case AC_PLU_ADDDEL:

        /* check total */
        TtlPlu.uchMajorClass = CLASS_TTLPLU;
        _tcsncpy(TtlPlu.auchPLUNumber, MaintWork.PLU.auchPLUNumber, PLU_MAX_DIGIT);

        /* check PLU Type */
        uchAdj = 1;
       if (MaintWork.PLU.uchPLUType == PLU_ONEADJ) {               /* One Adjective */
            uchAdj = MAX_ADJECTIVE_NO;
       }

        /* check current daily total */

        TtlPlu.uchMinorClass = CLASS_TTLCURDAY;
        for (i = 1; i <= uchAdj; i++) {
            TtlPlu.uchAdjectNo = i;
            if ((sReturn = SerTtlTotalRead(&TtlPlu)) == TTL_SUCCESS) {
                return(LDT_TLCOACT_ADR);                            /* Total & Counter are active */
            }else
			{
				PifLog(MODULE_PLU, LOG_ERROR_PLU_READ_CURDAY);
			}
        }

        /* check current PTD total */

        TtlPlu.uchMinorClass = CLASS_TTLCURPTD;
        for (i = 1; i <= uchAdj; i++) {
            TtlPlu.uchAdjectNo = i;
            if ((sReturn = SerTtlTotalRead(&TtlPlu)) == TTL_SUCCESS) {
                return(LDT_TLCOACT_ADR);                            /* Total & Counter are active */
            }else
			{
				PifLog(MODULE_PLU, LOG_ERROR_PLU_READ_CURPTD);
			}
        }

        /* check saved daily total */

        TtlPlu.uchMinorClass = CLASS_TTLSAVDAY;
        for (i = 1; i <= uchAdj; i++) {
            TtlPlu.uchAdjectNo = i;
            if ((sReturn = SerTtlTotalRead(&TtlPlu)) == TTL_SUCCESS) {      /* Saratoga */
                if ((sReturn = SerTtlIssuedCheck(&TtlPlu, TTL_NOTRESET)) == TTL_ISSUED) {
                    break;
                } else {
                    return(TtlConvertError(sReturn));
                }
            }else
			{
				PifLog(MODULE_PLU, LOG_ERROR_PLU_READ_SAVDAY);
			}
        }
        /* check saved PTD total */

        TtlPlu.uchMinorClass = CLASS_TTLSAVPTD;
        for (i = 1; i <= uchAdj; i++) {
            TtlPlu.uchAdjectNo = i;
            if ((sReturn = SerTtlTotalRead(&TtlPlu)) == TTL_SUCCESS) {      /* Saratoga */
                if ((sReturn = SerTtlIssuedCheck(&TtlPlu, TTL_NOTRESET)) == TTL_ISSUED) {
                    break;
                } else {
                    return(TtlConvertError(sReturn));
                }
            }else
			{
				PifLog(MODULE_PLU, LOG_ERROR_PLU_READ_SAVPTD);
			}
        }

        /* check operational times */
        if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
            MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */

            /* make header */
            MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* minor class */
                            RPT_ACT_ADR,                            /* "ACT #" */
                            RPT_PLU_ADR,                            /* "PLU" */
                            0,
                            RPT_MAINT_ADR,                          /* "MAINTE" */
                            0,
                            ( UCHAR)(pData->usACNumber),            /* action number */
                            0,
                            PRT_RECEIPT | PRT_JOURNAL); /* print control */
            uchMaintOpeCo |= MAINT_ALREADYOPERATE;
        } else {
            /* 1 line feed */
            MaintFeed();
        }

        /* print old data */
        PrtPrintItem(NULL, &(MaintWork.PLU));

        /* send print data to shared module */
        PrtShrPartialSend(MaintWork.PLU.usPrintControl);

        /* set data for display */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DEL;
        MaintPlu.uchFieldAddr = PLU_FIELD0_ADR;
		RflGetLead(MaintPlu.aszMnemonics, LDT_DELFUNC_ADR);
		break;

    case AC_PLU_PRICE_CHG:
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.uchFieldAddr = PLU_FIELD1_ADR;
        MaintPlu.ulInputData = RflFunc3bto4b(MaintWork.PLU.auchPresetPrice[0]);
		RflGetLead(MaintPlu.aszMnemonics, LDT_PP_ADR);
		break;

    case AC_PLU_MNEMO_CHG:
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.uchFieldAddr = PLU_FIELD1_ADR;
        _tcsncpy(MaintPlu.aszMnemonics, MaintWork.PLU.aszMnemonics, PARA_PLU_LEN);
        break;

    default:
/*        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
        return(LDT_KEYOVER_ADR);
    }

    /* read & set LEADTHRU */
    MaintPlu.uchMajorClass = CLASS_PARAPLU;
    _tcsncpy(MaintPlu.auchPLUNumber, PluIf.auchPluNo, PLU_MAX_DIGIT);
    MaintPlu.usACNumber = pData->usACNumber;
    MaintPlu.uchPageNumber = uchMaintMenuPage;
    DispWrite(&MaintPlu);
    return(SUCCESS);
}


/*
*===========================================================================
** Synopsis:    SHORT MaintPLUEdit( MAINTPLU *pData )
*
*     Input:    *pData              : pointer to structure for MAINTPLU
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS             : successful
*               LDT_SEQERR_ADR      : sequence error
*               LDT_KEYOVER_ADR     : wrong data
*               MAINT_PLU_MNEMO_ADR : set mnemonics
*
** Description: This function reads & displays parameter from PLU FILE in self-work area.
*===========================================================================
*/

SHORT MaintPLUEdit( MAINTPLU *pData )
{
    UCHAR         uchMaxFieldAddr;
    SHORT         sReturn;
	PLUIF         PluIf = { 0 };
	PARAPLU       MldParaPlu = { 0 };                                    /* For MLD */
	DEPTIF        DeptIf = { 0 };

    /* initialize */
    memset(MaintPlu.aszMnemonics, 0, sizeof(MaintPlu.aszMnemonics));

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                    /* without data */
        /* distinguish minor class */
        switch(pData->uchMinorClass) {
        case CLASS_PARAPLU_MAINT_WRITE:                         /* W/o Data in Mnemonics Set */
        case CLASS_PARAPLU_ADD_WRITE:
        case CLASS_PARAPLU_MNEMO_WRITE:
            return(LDT_SEQERR_ADR);

        case CLASS_PARAPLU_PRICE_READ:
            /* check PLU type */
            if (MaintWork.PLU.uchPLUType != PLU_NONADJ) {       /* One Adjective */
                MaintPlu.uchFieldAddr++;

                /* check address */
                if (MaintPlu.uchFieldAddr > PLU_FIELD5_ADR) {
                    MaintPlu.uchFieldAddr = PLU_FIELD1_ADR;    /* initialize address */
                }
            }

            /* set data for display next old data */
            MaintSetPriceDispData(MaintPlu.uchFieldAddr);
            return(SUCCESS);

        case CLASS_PARAPLU_MAINT_READ:
        case CLASS_PARAPLU_ADD_READ:
            MaintPlu.uchFieldAddr++;

            /* check PLU type & address */
            if (MaintPlu.uchFieldAddr > MaintChkPLUType(MaintWork.PLU.uchPLUType)) {
                MaintPlu.uchFieldAddr = PLU_FIELD1_ADR;             /* initialize address */
            }

            /* set data for display next old data */
            MaintSetPLUDispData(MaintPlu.uchFieldAddr);

            if (MaintPlu.uchFieldAddr > PLU_FIELD21_ADR) {
                MaintPLUDisplaySecondPage();
            } else {
                MaintPLUDisplayFirstPage();
            }

            /* check address */
            if (MaintPlu.uchFieldAddr == PLU_FIELD13_ADR) {
                return(MAINT_PLU_MNEMO_ADR);                        /* set mnemonics */
            }
            return(SUCCESS);

        default:
/*            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
            return(LDT_KEYOVER_ADR);

        }
    } else {                                                        /* with data */
        /* distinguish minor class */
        switch(pData->uchMinorClass) {

        case CLASS_PARAPLU_PRICE_READ:
            /* check PLU type */
            switch(MaintWork.PLU.uchPLUType) {
            case PLU_NONADJ:
                uchMaxFieldAddr = PLU_FIELD1_ADR;
                break;

            case PLU_ONEADJ:
                uchMaxFieldAddr = PLU_FIELD5_ADR;
                break;

            default:
/*                PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
                return(LDT_KEYOVER_ADR);
            }

            /* check address */
            if (pData->uchFieldAddr < PLU_FIELD1_ADR || uchMaxFieldAddr < pData->uchFieldAddr) {
                return(LDT_KEYOVER_ADR);
            }
            MaintPlu.uchFieldAddr = pData->uchFieldAddr;
            MaintSetPriceDispData(MaintPlu.uchFieldAddr);
            return(SUCCESS);

        case CLASS_PARAPLU_MNEMO_READ:
            MaintPlu.uchFieldAddr = PLU_FIELD1_ADR;
            MaintSetMnemoDispData();
            return(SUCCESS);

        case CLASS_PARAPLU_MAINT_READ:
        case CLASS_PARAPLU_ADD_READ:
            /* check PLU type & address */
            if (pData->uchFieldAddr < PLU_FIELD1_ADR || MaintChkPLUType(MaintWork.PLU.uchPLUType) < pData->uchFieldAddr) {
                return(LDT_KEYOVER_ADR);
            }
            MaintPlu.uchFieldAddr = pData->uchFieldAddr;
            MaintSetPLUDispData(MaintPlu.uchFieldAddr);

            if (MaintPlu.uchFieldAddr > PLU_FIELD21_ADR) {
                MaintPLUDisplaySecondPage();
            } else {
                MaintPLUDisplayFirstPage();
            }

            /* check address */
            if (MaintPlu.uchFieldAddr == PLU_FIELD13_ADR) {
                return(MAINT_PLU_MNEMO_ADR);                        /* set mnemonics */
            }
            return(SUCCESS);

        case CLASS_PARAPLU_PRICE_WRITE:
            /* price range has already checked by UI */
            /* check PLU type */
            switch(MaintWork.PLU.uchPLUType) {

            case PLU_NONADJ:
                /* set data */
                RflConv4bto3b(MaintWork.PLU.auchPresetPrice[0], pData->ulInputData);
                MaintPlu.uchFieldAddr = PLU_FIELD1_ADR;
                break;

            case PLU_ONEADJ:
                /* set data */
                RflConv4bto3b(MaintWork.PLU.auchPresetPrice[MaintPlu.uchFieldAddr - 1], pData->ulInputData);
                MaintPlu.uchFieldAddr++;

                /* check address */
                if (MaintPlu.uchFieldAddr > PLU_FIELD5_ADR) {
                    MaintPlu.uchFieldAddr = PLU_FIELD1_ADR;         /* initialize address */
                }
                break;

            default:
/*                PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA);  */
                return(LDT_KEYOVER_ADR);

            }

            /******************/
            /* Display to MLD */
            /******************/
            MldParaPlu = MaintWork.PLU;
            MldParaPlu.usACNumber    = AC_PLU_PRICE_CHG;
            MldParaPlu.uchFieldAddr  = MaintPlu.uchFieldAddr;
            MldParaPlu.uchMinorClass = CLASS_PARAPLU_NONADJ;
            if(MldParaPlu.uchPLUType == PLU_ONEADJ){
                MldParaPlu.uchMinorClass = CLASS_PARAPLU_ONEADJ;
                if(MldParaPlu.uchFieldAddr ==  PLU_FIELD1_ADR){
                    MldParaPlu.uchFieldAddr =  PLU_FIELD5_ADR;
                }
                else{
                    MldParaPlu.uchFieldAddr --;
                }
            }
            MldDispItem(&MldParaPlu,0);
            uchMaintMldPageNo = 0;  /* display 1st page */

            /* set data for display next old data */
            MaintSetPriceDispData(MaintPlu.uchFieldAddr);
            return(SUCCESS);

        case CLASS_PARAPLU_MNEMO_WRITE:
            MaintWork.PLU.aszMnemonics[PARA_PLU_LEN] = 0;
            _tcsncpy(MaintWork.PLU.aszMnemonics, pData->aszMnemonics, PARA_PLU_LEN);

            /******************/
            /* Display to MLD */
            /******************/
            MldParaPlu = MaintWork.PLU;
            MldParaPlu.usACNumber   = AC_PLU_MNEMO_CHG;
            MldParaPlu.uchFieldAddr = PLU_FIELD1_ADR;
            MldDispItem(&MldParaPlu,0);
            uchMaintMldPageNo = 0;  /* display 1st page */

            /* set data for display next old data */
            MaintPlu.uchFieldAddr = PLU_FIELD1_ADR;
            MaintSetMnemoDispData();
            return(SUCCESS);                                    /* set mnemonics */

        case CLASS_PARAPLU_ADD_WRITE:
        case CLASS_PARAPLU_MAINT_WRITE:
            /* distinguish field Address */
            switch(MaintPlu.uchFieldAddr) {
            case PLU_FIELD0_ADR:
                /* read PLU parameter for excepting wrong key sequence */
                _tcsncpy(PluIf.auchPluNo, MaintWork.PLU.auchPLUNumber, PLU_MAX_DIGIT);
                PluIf.uchPluAdj = 1;
                if (CliOpPluIndRead(&PluIf, usMaintLockHnd) != OP_NOT_IN_FILE)  {
                    return(LDT_SEQERR_ADR);
                }

                /* check plu type range */
                if (pData->ulInputData  < 1L || ( ULONG)PLU_TYPE_MAX < pData->ulInputData) { /* PLU_TYPE_MAX=3 */
                    return(LDT_KEYOVER_ADR);                        /* wrong data */
                }

                MaintWork.PLU.uchPLUType = ( UCHAR)(pData->ulInputData);

                /******************/
                /* Display to MLD */
                /******************/
                MldParaPlu = MaintWork.PLU;
                MldParaPlu.usACNumber   = AC_PLU_ADDDEL;
                MldParaPlu.uchFieldAddr = PLU_FIELD0_ADR;
                MldDispItem(&MldParaPlu,0);
                uchMaintMldPageNo = 0;  /* display 1st page */

                break;

            case PLU_FIELD1_ADR:
                /* check department number range */
                if (pData->ulInputData < 1L || pData->ulInputData > MAX_DEPT_NO) { /* 9999 */
                /* if (pData->ulInputData < 1L || ( ULONG)ParaFlexMem.usRecordNumber < pData->ulInputData) { / PLU_DEPTNO_MAX=50 */
                    return(LDT_KEYOVER_ADR);                        /* wrong data */
                }

                /* saratoga */
                DeptIf.usDeptNo = ( USHORT)(pData->ulInputData);                 /* Set DEPT Number */
                if ((sReturn = CliOpDeptIndRead(&DeptIf, usMaintLockHnd)) != OP_PERFORM) {
                    return(OpConvertError(sReturn));
                }

                MaintWork.PLU.usDEPTNumber = ( USHORT)(pData->ulInputData);

                /******************/
                /* Display to MLD */
                /******************/
                MaintPLUDisplayFirstPage();

                MldParaPlu = MaintWork.PLU;
                MldParaPlu.usACNumber   = AC_PLU_ADDDEL;
                MldParaPlu.uchFieldAddr = PLU_FIELD1_ADR;
                MldDispItem(&MldParaPlu,0);
                uchMaintMldPageNo = 0;  /* display 1st page */

                /* display */
				RflGetLead(MaintPlu.aszMnemonics, LDT_DEPTCTL_ADR);   /* Request Copy Department Control Code */
                MaintPlu.uchMinorClass = CLASS_PARAPLU_COPY;
                MaintPlu.ulInputData = pData->ulInputData;          /* DEPT Number */
                MaintPlu.uchPageNumber = uchMaintMenuPage;
                DispWrite(&MaintPlu);
                return(MAINT_PLU_CPYDEPT_ADR);                      /* copy DEPT status */

            case PLU_FIELD2_ADR:
                /* check report code range */
                if (pData->ulInputData < 1L || ( ULONG)PLU_REPORT_MAX < pData->ulInputData) { /* PLU_REPORT_MAX=10 */
                    return(LDT_KEYOVER_ADR);                         /* wrong data */
                }

                MaintWork.PLU.uchReportCode = ( UCHAR)(pData->ulInputData);
                break;

            case PLU_FIELD9_ADR:  /* status 7 */
                /* check bonus total index range */
                if (STD_NO_BONUS_TTL < pData->ulInputData) {
                    return(LDT_KEYOVER_ADR);                         /* wrong data */
                }
				// Originally we were treating bonus total as binary data, now it's decimal.
				// Not sure why bonus total info was put in MaintWork.PLU.auchPLUStatus, but
				// we'll leave it til I get a chance to look into the issue a little more.
				// For now we'll just put the bonus total info where it belongs (MaintWork.PLU.usBonusIndex)
				// are 3 - 12 below saved correctly, or is their placement into status a prob too?
				//
				// On investigating this what it appears is that originally there were eight bonus totals
				// that could be affected by a PLU. Within this area of auchPLUStatus[3] there were
				// eight bits that could be used to indicate which of the eight bonus totals were
				// affected by the PLU. This bit mask with each of the eight bonus totals being indicated
				// by one of the eight bits in the MDC field allowed a PLU to affect all or none or some
				// of the eight possible bonus totals. This is how it worked with the NCR 2170 and the
				// NCR 7448 and NHPOS. See defines BONUS_MASK_DATA as well as PLU_BONUS_IDX1, etc.
				// However with the donations of the software to Georgia Southern University along with
				// changes requested by the NCR Product Manager Dan Parliman to have more than eight bonus totals
				// we have gone from a bit map allowing any combination of bonus totals to be assigned to a PLU
				// to allowing only a single bonus total to be assigned to a PLU but there are 100 possible bonus totals now.
				// See other places where PLU_FIELD9_ADR is used or referenced including in comments.
				//    Richard Chambers, Mar-03-2020
                //MaintWork.PLU.auchPLUStatus[(MaintPlu.uchFieldAddr - 3) / 2] &= 0xF0;
                //MaintWork.PLU.auchPLUStatus[(MaintPlu.uchFieldAddr - 3) / 2] = pData->ulInputData;    // no longer used - RJC Mar-03-2020.
				MaintWork.PLU.usBonusIndex = pData->ulInputData;
                break;

            case PLU_FIELD3_ADR:  /* status 1 */
            case PLU_FIELD5_ADR:  /* status 3 */
            case PLU_FIELD7_ADR:  /* status 5 */
            case PLU_FIELD11_ADR: /* status 9 */
                /* check PLU status */
                if (MaintChkBinary(pData->aszMnemonics, 4) == LDT_KEYOVER_ADR) {
                    return(LDT_KEYOVER_ADR);
                }

                MaintWork.PLU.auchPLUStatus[(MaintPlu.uchFieldAddr - 3) / 2] &= 0xF0;
                MaintWork.PLU.auchPLUStatus[(MaintPlu.uchFieldAddr - 3) / 2] |= MaintAtoStatus(pData->aszMnemonics);
                break;

            case PLU_FIELD4_ADR:  /* status 2 */
            case PLU_FIELD6_ADR:  /* status 4 */
            case PLU_FIELD8_ADR:  /* status 6 */
            case PLU_FIELD10_ADR: /* status 8 */
            case PLU_FIELD12_ADR: /* status 10 */
                if (MaintChkBinary(pData->aszMnemonics, 4) == LDT_KEYOVER_ADR) {
                    return(LDT_KEYOVER_ADR);
                }

                MaintWork.PLU.auchPLUStatus[(MaintPlu.uchFieldAddr - 4) / 2] &= 0x0F;
                MaintWork.PLU.auchPLUStatus[(MaintPlu.uchFieldAddr - 4) / 2] |= MaintAtoStatus(pData->aszMnemonics) << 4;
                break;

            case PLU_FIELD13_ADR:
                MaintWork.PLU.aszMnemonics[PARA_PLU_LEN] = 0;
                _tcsncpy(MaintWork.PLU.aszMnemonics, pData->aszMnemonics, PARA_PLU_LEN);
                break;

            case PLU_FIELD14_ADR:
                /* check Table Number range */
                if ((ULONG)PLU_OEPTBL_MAX < pData->ulInputData) {
                    return(LDT_KEYOVER_ADR);                         /* wrong data */
                }

                MaintWork.PLU.uchTableNumber = ( UCHAR)(pData->ulInputData);
                break;

            case PLU_FIELD15_ADR:
                /* check Group Number range */
                if ((ULONG)PLU_OEPGRP_MAX < pData->ulInputData) {
                    return(LDT_KEYOVER_ADR);                         /* wrong data */
                }

                MaintWork.PLU.uchGroupNumber = ( UCHAR)(pData->ulInputData);
                break;

            case PLU_FIELD16_ADR:
                /* check Print Priority range */
                if ( (ULONG)PLU_PRIORITY_MAX < pData->ulInputData) {
                    return(LDT_KEYOVER_ADR);                         /* wrong data */
                }

                MaintWork.PLU.uchPrintPriority = ( UCHAR)(pData->ulInputData);
                break;

            case PLU_FIELD17_ADR:
                /* check PPI code range */
                if ( (ULONG)PLU_PPICODE_MAX < pData->ulInputData) {
                    return(LDT_KEYOVER_ADR);                         /* wrong data */
                }

                MaintWork.PLU.uchPPICode = ( USHORT)(pData->ulInputData); //ESMITH
                break;

            case PLU_FIELD18_ADR: /* status 11 */
                /* check PLU status */
                if (MaintChkBinary(pData->aszMnemonics, 4) == LDT_KEYOVER_ADR) {
                    return(LDT_KEYOVER_ADR);
                }

                MaintWork.PLU.auchPLUStatus[(MaintPlu.uchFieldAddr - 8) / 2] &= 0xF0;
                MaintWork.PLU.auchPLUStatus[(MaintPlu.uchFieldAddr - 8) / 2] |= MaintAtoStatus(pData->aszMnemonics);
                break;

            case PLU_FIELD19_ADR: /* status 12 */
                if (MaintChkBinary(pData->aszMnemonics, 4) == LDT_KEYOVER_ADR) {
                    return(LDT_KEYOVER_ADR);
                }

                MaintWork.PLU.auchPLUStatus[(MaintPlu.uchFieldAddr - 8) / 2] &= 0x0F;
                MaintWork.PLU.auchPLUStatus[(MaintPlu.uchFieldAddr - 8) / 2] |= MaintAtoStatus(pData->aszMnemonics) << 4;
                break;

            case PLU_FIELD20_ADR: /* 2172 */
                /* check link no range */
                if ( (ULONG)PLU_LINKNO_MAX < pData->ulInputData) {
                    return(LDT_KEYOVER_ADR);                         /* wrong data */
                }

                MaintWork.PLU.usLinkNumber = ( USHORT)(pData->ulInputData);
                break;

            case PLU_FIELD21_ADR: /* 2172 */
                /* check sales restriction range */
                if ( (ULONG)PLU_RESTRICT_MAX < pData->ulInputData) {
                    return(LDT_KEYOVER_ADR);                         /* wrong data */
                }

                MaintWork.PLU.uchSalesRestriction = ( UCHAR)(pData->ulInputData);
                break;

            case PLU_FIELD22_ADR: /* 2172 */
                /* check sales restriction range */
                if ( (ULONG)PLU_FAMILY_MAX < pData->ulInputData) {
                    return(LDT_KEYOVER_ADR);                         /* wrong data */
                }

                MaintWork.PLU.usFamilyCode = (USHORT)(pData->ulInputData);
                break;

            case PLU_FIELD23_ADR: /* 2172 */
                /* check price multiple range */
                if ( (ULONG)PLU_PM_MAX < pData->ulInputData) {
                    return(LDT_KEYOVER_ADR);                         /* wrong data */
                }

                MaintWork.PLU.uchPriceMultiple = ( UCHAR)(pData->ulInputData);
                break;

            case PLU_FIELD24_ADR: /* preset price 1 */
            case PLU_FIELD25_ADR: /* preset price 2 */
            case PLU_FIELD26_ADR: /* preset price 3 */
            case PLU_FIELD27_ADR: /* preset price 4 */
            case PLU_FIELD28_ADR: /* preset price 5 */
            /* case PLU_FIELD19_ADR: / preset price 1 */
            /* case PLU_FIELD20_ADR: / preset price 2 */
            /* case PLU_FIELD21_ADR: / preset price 3 */
            /* case PLU_FIELD22_ADR: / preset price 4 */
            /* case PLU_FIELD23_ADR: / preset price 5 */
                RflConv4bto3b(MaintWork.PLU.auchPresetPrice[MaintPlu.uchFieldAddr - PLU_FIELD24_ADR], pData->ulInputData);                /* Saratoga */
                break;

            default:
/*                PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
                return(LDT_KEYOVER_ADR);
            }

            /******************/
            /* Display to MLD */
            /******************/
            if(MaintPlu.uchFieldAddr != PLU_FIELD0_ADR){
                MldParaPlu = MaintWork.PLU;
                MldParaPlu.usACNumber   = AC_PLU_MAINT;
                if(pData->uchMinorClass == CLASS_PARAPLU_ADD_WRITE){
                    MldParaPlu.usACNumber = AC_PLU_ADDDEL;
                }
                MldParaPlu.uchMinorClass = (MaintWork.PLU.uchPLUType == PLU_ONEADJ ? CLASS_PARAPLU_ONEADJ : CLASS_PARAPLU_NONADJ);
                if (MaintPlu.uchFieldAddr > PLU_FIELD21_ADR) {
                    MldParaPlu.uchFieldAddr = 0;
                    MldDispItem(&MldParaPlu,1); /* display 2nd page */
                    uchMaintMldPageNo = 1;
                } else {
                    if (uchMaintMldPageNo) {    /* if 2nd page displayed */
                        MldParaPlu.uchFieldAddr = 0;
                        MldDispItem(&MldParaPlu,0); /* redisplay 1st page */
                    }
                    MldParaPlu.uchFieldAddr = MaintPlu.uchFieldAddr;
                    MldDispItem(&MldParaPlu,0);
                    uchMaintMldPageNo = 0;
                }
            }

            MaintPlu.uchFieldAddr++;

            /* check PLU type & address */
            if (MaintPlu.uchFieldAddr > MaintChkPLUType(MaintWork.PLU.uchPLUType)) {
                MaintPlu.uchFieldAddr = PLU_FIELD1_ADR;             /* initialize address */
            }

            MaintSetPLUDispData(MaintPlu.uchFieldAddr);

            /* check address */
            if (MaintPlu.uchFieldAddr == PLU_FIELD13_ADR) {
                return(MAINT_PLU_MNEMO_ADR);                        /* set mnemonics */
            }
            return(SUCCESS);

        default:
/*            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
            return(LDT_KEYOVER_ADR);
        }
    }
}


/*
*===========================================================================
** Synopsis:    VOID MaintSetPLUDispData(UCHAR uchFieldAddr)
*
*     Input:    uchFieldAddr
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function sets data for display.
*===========================================================================
*/

VOID MaintSetPLUDispData(UCHAR uchFieldAddr)
{
    /* initialize */
    memset(MaintPlu.aszMnemonics, 0, sizeof(MaintPlu.aszMnemonics));

    /* distinguish address */
    switch(uchFieldAddr) {
    case PLU_FIELD1_ADR:  /* department number */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.usDEPTNumber);
        /* MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.uchDEPTNumber); */
		RflGetLead(MaintPlu.aszMnemonics, LDT_DEPTNO_ADR);
		break;

    case PLU_FIELD2_ADR:  /* report code */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.uchReportCode);
		RflGetLead(MaintPlu.aszMnemonics, LDT_REPOCD_ADR);
		break;

    case PLU_FIELD9_ADR:  /* status 7 */
		// We have gone from a bit map allowing any combination of eight bonus totals to be assigned to a PLU
		// to allowing only a single bonus total to be assigned to a PLU but there are 100 possible bonus totals now.
		// See other places where PLU_FIELD9_ADR is used or referenced including in comments.
		//    Richard Chambers, Mar-03-2020
		MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        //MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.auchPLUStatus[(uchFieldAddr - 3)/2])/* & 0x0F*/;   // not binary anymore
		MaintPlu.ulInputData = MaintWork.PLU.usBonusIndex;
		RflGetLead(MaintPlu.aszMnemonics, LDT_STATUS_ADR);
		break;

    case PLU_FIELD3_ADR:  /* status 1 */
    case PLU_FIELD5_ADR:  /* status 3 */
    case PLU_FIELD7_ADR:  /* status 5 */
    case PLU_FIELD11_ADR: /* status 9 */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_STS;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.auchPLUStatus[(uchFieldAddr - 3)/2]) & 0x0F;
		RflGetLead(MaintPlu.aszMnemonics, LDT_STATUS_ADR);
		break;

    case PLU_FIELD4_ADR:  /* status 2 */
    case PLU_FIELD6_ADR:  /* status 4 */
    case PLU_FIELD8_ADR:  /* status 6 */
    case PLU_FIELD10_ADR: /* status 8 */
    case PLU_FIELD12_ADR: /* status 10 */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_STS;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.auchPLUStatus[(uchFieldAddr - 4) / 2]) & 0xF0;
        MaintPlu.ulInputData >>= 4;
		RflGetLead(MaintPlu.aszMnemonics, LDT_STATUS_ADR);
		break;

    case PLU_FIELD13_ADR: /* mnemonics */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.ulInputData = 0;
        _tcsncpy(MaintPlu.aszMnemonics, MaintWork.PLU.aszMnemonics, PARA_PLU_LEN);
        break;

    case PLU_FIELD14_ADR:  /* Table Number */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.uchTableNumber);
		RflGetLead(MaintPlu.aszMnemonics, LDT_NUM_ADR);
		break;

    case PLU_FIELD15_ADR:  /* Group Number */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.uchGroupNumber);
		RflGetLead(MaintPlu.aszMnemonics, LDT_NUM_ADR);
		break;

    case PLU_FIELD16_ADR:  /* Priority Number */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.uchPrintPriority);
		RflGetLead(MaintPlu.aszMnemonics, LDT_NUM_ADR);
		break;

    case PLU_FIELD17_ADR:  /* PPI Code */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.uchPPICode);
		RflGetLead(MaintPlu.aszMnemonics, LDT_NUM_ADR);
		break;

    case PLU_FIELD18_ADR: /* status 11 */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_STS;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.auchPLUStatus[(uchFieldAddr - 8)/2]) & 0x0F;
		RflGetLead(MaintPlu.aszMnemonics, LDT_STATUS_ADR);
		break;

    case PLU_FIELD19_ADR: /* status 12 */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_STS;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.auchPLUStatus[(uchFieldAddr - 8)/2]) & 0xF0;
        MaintPlu.ulInputData >>= 4;
		RflGetLead(MaintPlu.aszMnemonics, LDT_STATUS_ADR);
		break;

    case PLU_FIELD20_ADR:  /* Link Code */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.usLinkNumber);
		RflGetLead(MaintPlu.aszMnemonics, LDT_NUM_ADR);
		break;

    case PLU_FIELD21_ADR:  /* Sales Rectriction Code */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.uchSalesRestriction);
		RflGetLead(MaintPlu.aszMnemonics, LDT_NUM_ADR);
		break;

    case PLU_FIELD22_ADR:  /* Family Code */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.usFamilyCode);
		RflGetLead(MaintPlu.aszMnemonics, LDT_NUM_ADR);
		break;

    case PLU_FIELD23_ADR:  /* Price Multiple */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.ulInputData = ( ULONG)(MaintWork.PLU.uchPriceMultiple);
		RflGetLead(MaintPlu.aszMnemonics, LDT_NUM_ADR);
		break;

    case PLU_FIELD24_ADR: /* preset price 1 */
    case PLU_FIELD25_ADR: /* preset price 2 */
    case PLU_FIELD26_ADR: /* preset price 3 */
    case PLU_FIELD27_ADR: /* preset price 4 */
    case PLU_FIELD28_ADR: /* preset price 5 */
    /* case PLU_FIELD19_ADR: / preset price 1 */
    /* case PLU_FIELD20_ADR: / preset price 2 */
    /* case PLU_FIELD21_ADR: / preset price 3 */
    /* case PLU_FIELD22_ADR: / preset price 4 */
    /* case PLU_FIELD23_ADR: / preset price 5 */
        MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
        MaintPlu.ulInputData = RflFunc3bto4b(MaintWork.PLU.auchPresetPrice[uchFieldAddr - PLU_FIELD24_ADR]); /* Saratoga */
		RflGetLead(MaintPlu.aszMnemonics, LDT_PP_ADR);
		break;

    default:
/*        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
        return;
    }

    MaintPlu.uchMajorClass = CLASS_PARAPLU;                         /* set major class */
    MaintPlu.uchPageNumber = uchMaintMenuPage;
    DispWrite(&MaintPlu);
}

/*
*===========================================================================
** Synopsis:    VOID MaintSetPriceDispData(UCHAR uchFieldAddr)
*
*     Input:    uchFieldAddr
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function displays the preset price.
*===========================================================================
*/

VOID MaintSetPriceDispData(UCHAR uchFieldAddr)
{
    MaintPlu.uchMajorClass = CLASS_PARAPLU;
    MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
    MaintPlu.ulInputData = RflFunc3bto4b(MaintWork.PLU.auchPresetPrice[uchFieldAddr - 1]);
	RflGetLead (MaintPlu.aszMnemonics, LDT_PP_ADR);
    MaintPlu.uchPageNumber = uchMaintMenuPage;
    DispWrite(&MaintPlu);
}

/*
*===========================================================================
** Synopsis:    VOID MaintSetMnemoDispData( VOID )
*
*     Input:    uchFieldAddr
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function displays mnemonic data.
*===========================================================================
*/

VOID MaintSetMnemoDispData( VOID )
{
    MaintPlu.uchMajorClass = CLASS_PARAPLU;                         /* set major class */
    MaintPlu.uchMinorClass = CLASS_PARAPLU_DATA;
    MaintPlu.ulInputData = 0L;
	memset(MaintPlu.aszMnemonics, 0, sizeof(MaintPlu.aszMnemonics));
    _tcsncpy(MaintPlu.aszMnemonics, MaintWork.PLU.aszMnemonics, PARA_PLU_LEN);
    MaintPlu.uchPageNumber = uchMaintMenuPage;
    DispWrite(&MaintPlu);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPLUWrite( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               sReturn        : error
*
** Description: This function writes & prints PLU PARAMETER RECORD of self-work area in PLU PARAMETER FILE.
*===========================================================================
*/

SHORT MaintPLUWrite( VOID )
{
    UCHAR           i;
    UCHAR           uchAdjMax;
    SHORT           sReturn;
	USHORT          usAddress;
	PLUIF           PluIf = { 0 };
	MAINTTRANS      MaintTrans = { 0 };

    /* check PLU type */

    uchAdjMax = 1;
    if (MaintWork.PLU.uchPLUType == PLU_ONEADJ) {               /* One Adjective */
        uchAdjMax = MAX_ADJECTIVE_NO;
    }

    _tcsncpy(PluIf.auchPluNo, MaintWork.PLU.auchPLUNumber, PLU_MAX_DIGIT);
    PluIf.ParaPlu.ContPlu.usDept = MaintWork.PLU.usDEPTNumber;  /* Saratoga */
    PluIf.ParaPlu.ContPlu.uchRept = MaintWork.PLU.uchReportCode;
    memcpy(PluIf.ParaPlu.ContPlu.auchContOther, MaintWork.PLU.auchPLUStatus, MAX_PLUSTS_SIZE);
    memcpy(PluIf.ParaPlu.ContPlu.auchContOther2, MaintWork.PLU.auchContOther2, sizeof(PluIf.ParaPlu.ContPlu.auchContOther2));
    _tcsncpy(PluIf.ParaPlu.auchPluName, MaintWork.PLU.aszMnemonics, PARA_PLU_LEN);
    PluIf.ParaPlu.usLinkNo = MaintWork.PLU.usLinkNumber; /* 2172 */
    PluIf.ParaPlu.uchRestrict = MaintWork.PLU.uchSalesRestriction; /* 2172 */
    PluIf.ParaPlu.uchLinkPPI = MaintWork.PLU.uchPPICode;                            /* set PPI code */
    PluIf.ParaPlu.uchPriceMulWeight = MaintWork.PLU.uchPriceMultiple; /* 2172 */
    PluIf.ParaPlu.usFamilyCode = MaintWork.PLU.usFamilyCode; /* 2172 */
    PluIf.ParaPlu.uchTableNumber = MaintWork.PLU.uchTableNumber;                     /* set Table Number */
    PluIf.ParaPlu.uchGroupNumber = MaintWork.PLU.uchGroupNumber;                    /* set Group Number */
    PluIf.ParaPlu.uchPrintPriority = MaintWork.PLU.uchPrintPriority;                /* set Print Priority */
    PluIf.ParaPlu.ContPlu.uchItemType = MaintWork.PLU.uchPLUType;    /* set PLUType */
    PluIf.ParaPlu.ContPlu.uchItemType <<= 6;                    /* set PLUType, Saratoga */
	PluIf.ParaPlu.ContPlu.usBonusIndex = MaintWork.PLU.usBonusIndex;
	memcpy(PluIf.ParaPlu.auchAlphaNumRouting, MaintWork.PLU.auchAlphaNumRouting, sizeof(PluIf.ParaPlu.auchAlphaNumRouting));
    _tcsncpy(PluIf.ParaPlu.auchAltPluName, MaintWork.PLU.auchAltPluName, PARA_PLU_LEN);
	PluIf.ParaPlu.uchColorPaletteCode = MaintWork.PLU.uchColorPaletteCode;
	memcpy(PluIf.ParaPlu.auchStatus2, MaintWork.PLU.auchStatus2, sizeof(PluIf.ParaPlu.auchStatus2));
	PluIf.ParaPlu.usTareInformation = MaintWork.PLU.usTareInformation;
	memcpy (PluIf.ParaPlu.uchExtendedGroupPriority, MaintWork.PLU.uchExtendedGroupPriority, sizeof(PluIf.ParaPlu.uchExtendedGroupPriority));
	memcpy(PluIf.ParaPlu.uchPluOptionFlags, MaintWork.PLU.uchPluOptionFlags, sizeof(PluIf.ParaPlu.uchPluOptionFlags));

    for (i = 1; i <= uchAdjMax; i++) {
        PluIf.uchPluAdj = i;
        //_tcsncpy(PluIf.ParaPlu.auchPrice, MaintWork.PLU.auchPresetPrice[i - 1], OP_PLU_PRICE_SIZE);
        memcpy(PluIf.ParaPlu.auchPrice, MaintWork.PLU.auchPresetPrice[i - 1], OP_PLU_PRICE_SIZE);

        /* check if data write or not */
		sReturn = SerOpPluAssign(&PluIf, usMaintLockHnd);
		if (sReturn == OP_NEW_PLU) {
			usAddress = TRN_ADD_ADR;            /* "ADDITION" */
        } else if (sReturn == OP_CHANGE_PLU) {
			usAddress = TRN_CHNG_ADR;           /* "CHANGE" */
        } else if (sReturn == OP_PLU_FULL) {                    /* file full */
            for (i -= 1; i > 0; i--) {
                /* recovery */
                PluIf.uchPluAdj = i;
                if ((sReturn = SerOpPluDelete(&PluIf, usMaintLockHnd)) != OP_PERFORM) {
                    PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_CODE_03);
                    PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_NOTDELPLU);
                }
            }
            return(LDT_FLFUL_ADR);
        } else {
            return(OpConvertError(sReturn));
        }
    }

    /* check operational times */
    if (uchMaintOpeCo != MAINT_ALREADYOPERATE) {
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */
        /* make header */
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* minor class */
                        RPT_ACT_ADR,                            /* "ACT #" */
                        RPT_PLU_ADR,                            /* "PLU" */
                        0,
                        RPT_MAINT_ADR,                          /* "MAINTE" */
                        0,
                        ( UCHAR)(MaintPlu.usACNumber),          /* action number */
                        0,
                        PRT_RECEIPT | PRT_JOURNAL); /* print control */
        uchMaintOpeCo |= MAINT_ALREADYOPERATE;
    } else {
        /* check A/C number */
        if (MaintPlu.usACNumber == AC_PLU_ADDDEL) {
            MaintFeed();
        }
    }

    /* print "ADDITOIN" or "CHANGE" using structure MAINTTRANS */

    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );
	RflGetTranMnem (MaintTrans.aszTransMnemo, usAddress);
    PrtPrintItem(NULL, &MaintTrans);

    /* set journal bit & receipt bit */
    MaintWork.PLU.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* Check PLU Type and Set Minor Class */
    if (MaintWork.PLU.uchPLUType == PLU_NONADJ) {
        MaintWork.PLU.uchMinorClass = CLASS_PARAPLU_NONADJ;
    } else if (MaintWork.PLU.uchPLUType == PLU_ONEADJ) {
        MaintWork.PLU.uchMinorClass = CLASS_PARAPLU_ONEADJ;
    } else {
        MaintWork.PLU.uchMinorClass = CLASS_PARAPLU_OPENPLU;
    }

    /* print plu parameter */
    PrtPrintItem(NULL, &(MaintWork.PLU));

    /* send print data to shared module */
    PrtShrPartialSend(MaintWork.PLU.usPrintControl);

	BlFwIfReloadOEPGroup ();

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPLUDelete( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      sReturn     : error
*               LDT_ERR_ADR : error
*
** Description: This function deletes corresponding plu record from plu file.
*===========================================================================
*/

SHORT MaintPLUDelete( VOID )
{
    UCHAR           i;
    UCHAR           uchAdjMax;
    SHORT           sReturn;
    SHORT           sSavedReturn = 0;
	PLUIF           PluIf = { 0 };
	MAINTTRANS      MaintTrans = { 0 };

    /* check PLU type */
    uchAdjMax = 1;
    if (MaintWork.PLU.uchPLUType == PLU_ONEADJ) {               /* One Adjective */
        uchAdjMax = MAX_ADJECTIVE_NO;
    }

    /* set data */
    for (i = 1; i <= uchAdjMax; i++) {
		NHPOS_ASSERT_ARRAYSIZE(PluIf.auchPluNo, PLU_MAX_DIGIT);
        _tcsncpy(PluIf.auchPluNo, MaintWork.PLU.auchPLUNumber, PLU_MAX_DIGIT);
        /* PluIf.usPluNo = MaintWork.PLU.usPLUNumber; */
        PluIf.uchPluAdj = i;
        if ((sReturn = SerOpPluDelete(&PluIf, usMaintLockHnd)) != OP_PERFORM) {
            sSavedReturn = sReturn;
            if (i == 1) {
                break;
            }
            PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_CODE_04);
            PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_NOTDELPLU);
        }
    }

    if (sSavedReturn) {                                        /* error occurred */
        return(OpConvertError(sSavedReturn));
    }
    /* print "DELETE" using structure MAINTTRANS */

    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );
	RflGetTranMnem (MaintTrans.aszTransMnemo, TRN_DEL_ADR);
    PrtPrintItem(NULL, &MaintTrans);

    /* send print data to shared module */
    PrtShrPartialSend(MaintTrans.usPrtControl);

	BlFwIfReloadOEPGroup ();

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPLUReport( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS : successful
*               sReturn : error
*
** Description: This function prints PLU report from PLU file.
*===========================================================================
*/

SHORT MaintPLUReport( VOID )
{
    UCHAR           i;
    UCHAR           uchFeedChk = 0;
    SHORT           sSavedReturn = 0;
    SHORT           sReturn;
	PLUIF           PluIf = { 0 };
	MAINTERRORCODE  MaintErrorCode = { 0 };

    /* initialize */
    memset(MaintWork.PLU.aszMnemonics, 0, sizeof(MaintWork.PLU.aszMnemonics));
    memset(MaintWork.PLU.auchPLUNumber, 0, sizeof(MaintWork.PLU.auchPLUNumber));

    /* check operational times */

    if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */

        /* make header */
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,                  /* minor class */
                        RPT_ACT_ADR,                            /* "ACT #" */
                        RPT_PLU_ADR,                            /* "PLU" */
                        0,
                        RPT_FILE_ADR,                           /* "FILE" */
                        0,                                      /* report type */
                        AC_PLUPARA_READ_RPT,                    /* action number(72) */
                        0,
                        PRT_RECEIPT | PRT_JOURNAL); /* print control */
        uchMaintOpeCo |= MAINT_ALREADYOPERATE;
    }

    /* set print control */

    MaintWork.PLU.usPrintControl = 0;

    if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT0))) {
        MaintWork.PLU.usPrintControl |= PRT_JOURNAL;   /* print out on journal */
    }

    if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT1))) {
        MaintWork.PLU.usPrintControl |= PRT_RECEIPT;   /* print out receipt */
    }

    while(SerOpPluAllRead(&PluIf, usMaintLockHnd) == OP_PERFORM) {

        /* check abort key */

        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            SerOpPluAbort(PluIf.husHandle, usMaintLockHnd);
            MaintMakeAbortKey();
            break;
        }

        /* read PLU parameter */
#if 1
		MaintCopyOpPluParaEntry2ParaPlu(&MaintWork.PLU, PluIf.auchPluNo, &PluIf.ParaPlu);
		MaintWork.PLU.usPrintControl = (PRT_JOURNAL | PRT_RECEIPT);
#else
		MaintWork.PLU.uchMajorClass = CLASS_PARAPLU;
        _tcsncpy(MaintWork.PLU.auchPLUNumber, PluIf.auchPluNo, PLU_MAX_DIGIT);
        /* MaintWork.PLU.usPLUNumber = PluIf.usPluNo; */
        MaintWork.PLU.uchPLUType = PluIf.ParaPlu.ContPlu.uchItemType;         /* get out PLU Type */
        /* MaintWork.PLU.uchPLUType = ( UCHAR)(PluIf.ParaPlu.ContPlu.uchDept & PLU_TYPE_MASK_REG);     / get out PLU Type */
        MaintWork.PLU.uchPLUType >>= 6;
        MaintWork.PLU.usDEPTNumber = PluIf.ParaPlu.ContPlu.usDept;    /* get out dept number */
        /* MaintWork.PLU.uchDEPTNumber = ( UCHAR)(PluIf.ParaPlu.ContPlu.uchDept & PLU_DEPTNO_MASK);    / get out dept number */
        MaintWork.PLU.uchReportCode = ( UCHAR)(PluIf.ParaPlu.ContPlu.uchRept & PLU_REPORT_MASK);    /* get out report code */
        MaintWork.PLU.uchTableNumber = ( UCHAR)PluIf.ParaPlu.uchTableNumber;                          /* get out Table Number */
        MaintWork.PLU.uchGroupNumber = ( UCHAR)PluIf.ParaPlu.uchGroupNumber;                        /* get out Group Number */
        MaintWork.PLU.uchPrintPriority = ( UCHAR)PluIf.ParaPlu.uchPrintPriority;                    /* get out Print Priority */
        MaintWork.PLU.uchPPICode = ( USHORT)PluIf.ParaPlu.uchLinkPPI;                                /* get out PPI code  ESMITH*/
		//MaintWork.PLU.uchPPICode = ( UCHAR)PluIf.ParaPlu.uchLinkPPI;                                /* get out PPI code */
        MaintWork.PLU.uchPriceMultiple = PluIf.ParaPlu.uchPriceMulWeight; /* 2172 */
        MaintWork.PLU.uchSalesRestriction = PluIf.ParaPlu.uchRestrict; /* 2172 */
        MaintWork.PLU.usLinkNumber = PluIf.ParaPlu.usLinkNo; /* 2172 */
        MaintWork.PLU.usFamilyCode = PluIf.ParaPlu.usFamilyCode; /* 2172 */
        memcpy(MaintWork.PLU.auchPLUStatus, PluIf.ParaPlu.ContPlu.auchContOther, MAX_PLUSTS_SIZE);
	    memcpy(MaintWork.PLU.auchContOther2, PluIf.ParaPlu.ContPlu.auchContOther2, sizeof(MaintWork.PLU.auchContOther2));
        _tcsncpy(MaintWork.PLU.aszMnemonics, PluIf.ParaPlu.auchPluName, PARA_PLU_LEN);
        _tcsncpy(MaintWork.PLU.auchAltPluName, PluIf.ParaPlu.auchAltPluName, PARA_PLU_LEN);
		MaintWork.PLU.uchColorPaletteCode      = PluIf.ParaPlu.uchColorPaletteCode;
		MaintWork.PLU.usBonusIndex             = PluIf.ParaPlu.ContPlu.usBonusIndex;
		MaintWork.PLU.usTareInformation        = PluIf.ParaPlu.usTareInformation;
		memcpy (MaintWork.PLU.uchExtendedGroupPriority, PluIf.ParaPlu.uchExtendedGroupPriority, sizeof(MaintWork.PLU.uchExtendedGroupPriority));
		memcpy(MaintWork.PLU.auchStatus2, PluIf.ParaPlu.auchStatus2, sizeof(PluIf.ParaPlu.auchStatus2));
#endif

        /* check if adjective exist or not */

        if (MaintWork.PLU.uchPLUType == PLU_NONADJ) {           /* Non Adjective */
            MaintWork.PLU.uchMinorClass = CLASS_PARAPLU_NONADJ;
            memcpy(MaintWork.PLU.auchPresetPrice[0], PluIf.ParaPlu.auchPrice, MAX_PLUPRICE_SIZE);
        } else if (MaintWork.PLU.uchPLUType == PLU_ONEADJ) {    /* One Adjective */
            MaintWork.PLU.uchMinorClass = CLASS_PARAPLU_ONEADJ;
            memcpy(MaintWork.PLU.auchPresetPrice[0], PluIf.ParaPlu.auchPrice, MAX_PLUPRICE_SIZE);
            for (i = 1; i < MAX_ADJECTIVE_NO; i++) {
                if ((sReturn = SerOpPluAllRead(&PluIf, usMaintLockHnd)) == OP_PERFORM) {
                    memcpy(MaintWork.PLU.auchPresetPrice[i], PluIf.ParaPlu.auchPrice, MAX_PLUPRICE_SIZE);
                } else {
					PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_CODE_05);
                    PifLog(MODULE_MAINT_LOG_ID, LOG_ERROR_MAT_NOTREADPLU);
                    sSavedReturn = OpConvertError(sReturn);
                    memset(MaintWork.PLU.auchPresetPrice[i], 0, MAX_PLUPRICE_SIZE);
                }
            }
        } else {                                                /* Open PLU */
            MaintWork.PLU.uchMinorClass = CLASS_PARAPLU_OPENPLU;
        }

        /* check print control */
        if (MaintWork.PLU.usPrintControl & PRT_RECEIPT) {
            /* check feed */
            if (uchFeedChk) {
                /* 1 line feed */
                MaintFeed();
            }
        }
        PrtPrintItem(NULL, &(MaintWork.PLU));
        uchFeedChk = 1;

        /* print error status */
        if (sSavedReturn) {                                     /* error occurred */
            MaintErrorCode.uchMajorClass = CLASS_MAINTERRORCODE;
            MaintErrorCode.sErrorCode = sSavedReturn;

            /* set print control */
            MaintErrorCode.usPrtControl = 0;

            if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT0))) {
                MaintErrorCode.usPrtControl |= PRT_JOURNAL;   /* print out on journal */
            }

            if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT1))) {
                MaintErrorCode.usPrtControl |= PRT_RECEIPT;   /* print out repeipt */
            }
            PrtPrintItem(NULL, &MaintErrorCode);
            sSavedReturn = 0;
        }
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    UCHAR MaintChkPLUType(UCHAR uchPLUType)
*
*     Input:    uchPLUType
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Max Field Address
*
** Description: This function checks PLU Type.
*===========================================================================
*/

UCHAR MaintChkPLUType(UCHAR uchPLUType)
{
    /* check PLU type */
    switch(uchPLUType) {            /* Saratoga */
    case PLU_OPEN:
        return(PLU_FIELD23_ADR);

    case PLU_NONADJ:
        return(PLU_FIELD24_ADR);

    case PLU_ONEADJ:
        return(PLU_FIELD28_ADR);

    default:
/*        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA);  */
        return(LDT_KEYOVER_ADR);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT MaintChkBinary(UCHAR auchChkString[], UCHAR uchChkDigit)
*
*     Input:    auchChkString
*               uchChkDigit
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      LDT_KEYOVER_ADR : wrong data
*
** Description: This function checks binary.
*===========================================================================
*/

SHORT MaintChkBinary(TCHAR auchChkString[], UCHAR uchChkDigit)
{
    UCHAR i;

    /* check string every digits */

    for (i = 0; i < uchChkDigit; i++) {
        if (auchChkString[i] != _T('0') && auchChkString[i] != _T('1') && auchChkString[i] != _T('\0')) {
            return(LDT_KEYOVER_ADR);
        }
    }
    if (auchChkString[uchChkDigit] != _T('\0')) {           /* Over Digit Case */
        return(LDT_KEYOVER_ADR);
    }
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    UCHAR MaintAtoStatus(UCHAR auchString[])
*
*     Input:    auchString
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      uchWork : bit pattern
*
** Description: This function converts string data into bit pattern.
*===========================================================================
*/
UCHAR MaintAtoStatus(TCHAR auchString[])
{
    UCHAR uchWork = 0;
    UCHAR i;

    for (i = 0; i < 4; i++) {
        if (auchString[i] == _T('\0')) {
            break;
        }
        uchWork <<= 1;
        if (auchString[i] == _T('1')) {
            uchWork++;
        }
    }
    return(uchWork);
}

VOID MaintPLUDisplayFirstPage()
{
    /* if (MaintWork.PLU.uchPLUType == PLU_ONEADJ) {        / One Adjective */
        if (uchMaintMldPageNo) {
			PARAPLU       MldParaPlu = MaintWork.PLU;                            /* For MLD */

			MldParaPlu.usACNumber   = AC_PLU_MAINT;
            MldParaPlu.uchMinorClass = (MaintWork.PLU.uchPLUType == PLU_ONEADJ ? CLASS_PARAPLU_ONEADJ : CLASS_PARAPLU_NONADJ);
            /* MldParaPlu.uchMinorClass = CLASS_PARAPLU_ONEADJ; */
            MldParaPlu.uchFieldAddr = 0;
            MldDispItem(&MldParaPlu,0); /* redisplay 1st page */
            uchMaintMldPageNo = 0;
        }
    /* } */
}

VOID MaintPLUDisplaySecondPage()
{

    /* if (MaintWork.PLU.uchPLUType == PLU_ONEADJ) {        / One Adjective */

    if (! uchMaintMldPageNo) {
		PARAPLU       MldParaPlu = MaintWork.PLU;

        MldParaPlu.usACNumber   = AC_PLU_MAINT;
        switch (MaintWork.PLU.uchPLUType) {         /* Saratoga */
        case PLU_OPEN:
            MldParaPlu.uchMinorClass = CLASS_PARAPLU_OPENPLU;
            break;

        case PLU_NONADJ:
            MldParaPlu.uchMinorClass = CLASS_PARAPLU_NONADJ;
            break;

        case PLU_ONEADJ:
            MldParaPlu.uchMinorClass = CLASS_PARAPLU_ONEADJ;
            break;

        default:
            break;
        }
        /* MldParaPlu.uchMinorClass = CLASS_PARAPLU_ONEADJ; */
        MldParaPlu.uchFieldAddr = 0;
        MldDispItem(&MldParaPlu,1); /* redisplay 2nd page */
        uchMaintMldPageNo = 1;
    }
    /* } */
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPLUModifier( MAINTPLU *pData )
*
*     Input:    *pData              : pointer to structure for MAINTPLU
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*
** Description: This function controls modifier status.
*===========================================================================
*/
SHORT MaintPLUModifier( MAINTPLU *pData )
{
    switch(pData->uchMinorClass) {
    case CLASS_PARAPLU_EVERSION:
        MaintWork.PLU.uchEVersion = (UCHAR)((MaintWork.PLU.uchEVersion & MOD_STAT_EVERSION) ? 0 : MOD_STAT_EVERSION);   /* toggle control */
        MaintPlu.uchEVersion = (UCHAR)((MaintPlu.uchEVersion & MOD_STAT_EVERSION) ? 0 : MOD_STAT_EVERSION);   /* toggle control */

    case CLASS_PARAPLU_SET_EV:
        MaintWork.PLU.uchEVersion=(UCHAR)1;     /* set E-Version */
        MaintPlu.uchEVersion=(UCHAR)1;
        uchPLUEVersion = (UCHAR)1;
        break;

    case CLASS_PARAPLU_RESET_EV:
        MaintWork.PLU.uchEVersion=(UCHAR)0;     /* reset E-Version */
        MaintPlu.uchEVersion=(UCHAR)0;
        uchPLUEVersion = (UCHAR)0;
        break;

    default:
        break;
    }
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPluLabelAnalysis(PARAPLU *ParaPLU, UCHAR uchEVersion)
*
*     Input:    *ParaPLU
*               uchEversion
*    Output:    ParaPLU->aucPLUNumber : converted PLU number (BCD format)
*     InOut:    Nothing
*
** Return:      SUCCESS : successful
*               sReturn : error
*
** Description: This function checks PLU label.
*===========================================================================
*/
SHORT   MaintPluLabelAnalysis(PARAPLU *ParaPLU, UCHAR uchEVersion)
{
    SHORT           sReturn;
	LABELANALYSIS   LabelAnalysis = { 0 };

    _tcsncpy(LabelAnalysis.aszScanPlu, ParaPLU->auchPLUNumber, NUM_PLU_SCAN_LEN);

/**********  Deleted R2.0
    if(0 != uchEVersion)
        LabelAnalysis.fchModifier|=LA_EMOD;     * Set E-Version status *
**********/
    /* R2.0 Start */
    if((uchEVersion & MOD_STAT_EVERSION) && (uchEVersion & MOD_STAT_VELOCITY)) {
        return(LDT_KEYOVER_ADR);    /* illegal data input error */
    }
    if(uchEVersion & MOD_STAT_EVERSION) {
        LabelAnalysis.fchModifier |= LA_EMOD;     /* Set E-Version status */
    }
    if(uchEVersion & MOD_STAT_VELOCITY) {
        LabelAnalysis.fchModifier |= LA_UPC_VEL;  /* set Velocity Status */
    }
    /* R2.0 End   */

    if(ParaPLU->uchStatus & MAINT_SCANNER_INPUT)
        LabelAnalysis.fchModifier |= LA_SCANNER;  /* Set Scanner Input status */

    sReturn=RflLabelAnalysis(&LabelAnalysis);
    if((sReturn == LABEL_ERR) ||
       ((LabelAnalysis.uchType == LABEL_RANDOM) &&
        (LabelAnalysis.uchLookup == LA_NOT_LOOKUP))) {  /* PLU label analyze */
        return(LDT_KEYOVER_ADR);    /* illegal data input error */
    }
    if((LabelAnalysis.uchType == LABEL_RANDOM) &&       /* random label && exe_lookup */
       (LabelAnalysis.uchLookup == LA_EXE_LOOKUP)) {
        _tcsncpy(ParaPLU->auchPLUNumber, LabelAnalysis.aszMaskLabel, PLU_MAX_DIGIT);
    } else
    if(LabelAnalysis.uchType == LABEL_COUPON) {         /* UPC Coupon */
        return(LDT_KEYOVER_ADR);    /* illegal data input error */
    }
    else {
        _tcsncpy(ParaPLU->auchPLUNumber, LabelAnalysis.aszLookPlu, PLU_MAX_DIGIT);
    }
    return SUCCESS;
}

/***** End of File *****/