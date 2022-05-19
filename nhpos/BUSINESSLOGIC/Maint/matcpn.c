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
* Title       : MaintCPN Module  ( SUPER & PROGRAM MODE )                   
* Category    : Maintenance, NCR 2170 US Hospitality Application Program    
* Program Name: MATCPN.C
* --------------------------------------------------------------------------
* Abstract: This function controls COUPON PARAMETER FILE.
*
*    The provided function names are as follows: 
* 
*        MaintCPNRead()    : read & display parameter from plu file 
*        MaintCPNEdit()    : check input data from UI & set data in self-work area 
*        MaintCPNWrite()   : write COUPON parameter record in COUPON file & print 
*        MaintCPNReport()  : print all COUPON Parameter
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Mar-15-95: 03.00.00 : T.Satoh   : initial                                   
* Jul-19-95: 03.00.01 : M.Ozawa   : Modify Status2 Bit Check (for FVT Comment)
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

#include "ecr.h"
#include "uie.h"
/* #include <pif.h> */
#include <rfl.h>
#include "paraequ.h"
#include "para.h"
#include "maint.h"
#include "display.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csop.h"
#include "csstbpar.h"
#include "csstbopr.h"
#include "csttl.h"
#include "csstbttl.h"
/* #include <log.h> */
/* #include <appllog.h> */

#include "mld.h"

#include "maintram.h" 

/*
*===========================================================================
*   Work Area Declartion 
*===========================================================================
*/

MAINTCPN    MaintCpn;

UCHAR       uchAdjStatus = 0;                    /* Adjective Edit Status */
                                                 /*    0 : Edit PLU No.   */
                                                 /*    1 : Edit Adjective */
UCHAR       uchCPNEVersion;

/*
*===========================================================================
** Synopsis:    SHORT MaintCPNRead( PARACPN *pData )
*               
*     Input:    *pData            : pointer to structure for PARACPN
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:       SUCCESS            : successful
*                LDT_SEQERR_ADR     : sequence error
*                LDT_KEYOVER_ADR    : wrong data
*                sReturn            : error status
*
** Description:    This function reads & displays parameter,
*                from CPN FILE in self-work area.
*===========================================================================
*/

SHORT MaintCPNRead( PARACPN *pData )
{
    UCHAR            i;
    SHORT            sReturn;
    SHORT            sErr = 0;
	CPNIF            CpnIf = { 0 };                      /* Read parameter I/F      */
	PARACPN          MldParaCpn = { 0 };                 /* For MLD                 */
    /*TTLCPN         TtlCpn;*/

    /*
     * initialize
     */
    memset(&MaintWork,0,sizeof(MAINTWORK));      /* Global work data        */
    memset(&MaintCpn,0,sizeof(MAINTCPN));        /* Module Global data      */

    /*
     * check status & CPN No. range
     */
    if(pData->uchStatus & MAINT_WITHOUT_DATA){   /* without data            */
        return(LDT_SEQERR_ADR);                  /* sequence error          */
    } 

    if(pData->uchCPNNumber < 1 || FLEX_CPN_MAX < pData->uchCPNNumber){
        return(LDT_KEYOVER_ADR);
    }

    /***********************************/
    /* set CpnIf parameter & read data */
    /***********************************/
    CpnIf.uchCpnNo = pData->uchCPNNumber;
    sReturn = CliOpCpnIndRead(&CpnIf,usMaintLockHnd);

    /*
     * branced by read proc's Return Code
     */
    switch(sReturn){
    case OP_PERFORM:                            /* Read is completed    */
        /*
        * set data for printing
        */
        MaintWork.CPN.uchMajorClass = CLASS_PARACPN;
        MaintWork.CPN.uchCPNNumber  = CpnIf.uchCpnNo;
        memcpy(MaintWork.CPN.uchCPNStatus,CpnIf.ParaCpn.uchCouponStatus,2);
        MaintWork.CPN.ulCPNAmount   = CpnIf.ParaCpn.ulCouponAmount;
        _tcsncpy(MaintWork.CPN.aszMnemonic, CpnIf.ParaCpn.aszCouponMnem, PARA_CPN_LEN);
        for (i = 0; i < STD_MAX_NUM_PLUDEPT_CPN; i++){
            _tcsncpy(&MaintWork.CPN.auchCPNTarget[i][0], &CpnIf.ParaCpn.auchCouponTarget[i][0], PLU_MAX_DIGIT);
            /* MaintWork.CPN.usCPNTarget[i] = CpnIf.ParaCpn.usCouponTarget[i]; */
            MaintWork.CPN.uchCPNAdj[i]   = CpnIf.ParaCpn.uchCouponAdj[i];
        }
		MaintWork.CPN.uchAutoCPNStatus = CpnIf.ParaCpn.uchAutoCPNStatus;
        MaintWork.CPN.usPrintControl =(PRT_JOURNAL|PRT_RECEIPT);

		MaintWork.CPN.usBonusIndex = CpnIf.ParaCpn.usBonusIndex;

        /*
         * If operation counter(uchMaintOpeCo) is once time
         *    => UCHAR uchMaintOpeCo is defined at matpremd.c
         */
        if(!(uchMaintOpeCo & MAINT_ALREADYOPERATE)){
            MaintIncreSpcCo(SPCO_CONSEC_ADR);/* inc consecutive cnt */

            /*
            * make header
            */
            MaintMakeHeader(CLASS_MAINTHEADER_RPT, RPT_ACT_ADR, RPT_CPN_ADR, 0 , RPT_MAINT_ADR, 0, (UCHAR)pData->usACNumber, 0, PRT_RECEIPT | PRT_JOURNAL);
            uchMaintOpeCo |= MAINT_ALREADYOPERATE;
        }
        /*
         * More than twice time
         */
        else{
            MaintFeed();                        /* 1 line feed       */
        }
        PrtPrintItem(NULL,&(MaintWork.CPN));    /* print old data    */
        PrtShrPartialSend(MaintWork.CPN.usPrintControl);    /* send print data to shared module  */
        break;

    case OP_NOT_IN_FILE:                        /* Data is not exist */
        sErr = 1;
        break;

    default:
        sErr = 1;
    }

    if (sErr != 0) {
        return(OpConvertError(sReturn));    /* convert err stat */
    }
    /*
     * distinguish A/C number & display old data every A/C number
     */
    switch(pData->usACNumber){
    case AC_CPN_MAINT:
        /* MaintCpn.uchMinorClass  = CLASS_PARACPN_DATA; */
        /* MaintCpn.uchFieldAddr   = CPN_FIELD1_ADR;     */
		RflGetLead (MaintCpn.aszMnemonics, LDT_STATUS_ADR);
		break;

    default:
        return(LDT_KEYOVER_ADR);
    }

    /******************/
    /* Display to MLD */
    /******************/
    MldParaCpn = MaintWork.CPN;
    MldParaCpn.uchFieldAddr = 0;
    MldDispItem(&MldParaCpn,0);

    /* 1995.03.09 : Display Old Status-1 */
    MaintCpn.uchMajorClass = CLASS_PARACPN;
    MaintCpn.uchMinorClass = CLASS_PARACPN_STS;
	NHPOS_ASSERT(MaintWork.CPN.uchCPNNumber <= 0xFF);//MAXBYTE
    MaintCpn.uchCPNNumber  = (UCHAR)MaintWork.CPN.uchCPNNumber;
    MaintCpn.usACNumber    = pData->usACNumber;
    MaintCpn.uchFieldAddr  = CPN_FIELD1_ADR;
    MaintCpn.ulInputData   = (ULONG)CpnIf.ParaCpn.uchCouponStatus[0] & 0x0f;
    DispWrite(&MaintCpn);

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintCPNEdit( MAINTCPN *pData )
*               
*     Input:    *pData              : pointer to structure for MAINTCPN
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS             : successful
*               LDT_SEQERR_ADR      : sequence error
*               LDT_KEYOVER_ADR     : wrong data
*               MAINT_CPN_MNEMO_ADR : set mnemonics 
*
** Description: This function is editing & displaing Coupon parameters
*===========================================================================
*/

SHORT MaintCPNEdit( MAINTCPN *pData )
{
    SHORT            i = 0;
    SHORT            j = 0;
    SHORT            sError;
    USHORT           usTerField;
    UCHAR            uchMaskData = 0x00;
	PARACPN          MldParaCpn = { 0 };                 /* For MLD                 */
    /*
     * initialize
     */
    memset(MaintCpn.aszMnemonics, 0, sizeof(MaintCpn.aszMnemonics));

    /*******************************/
    /* check status - without data */
    /*******************************/
    if(pData->uchStatus & MAINT_WITHOUT_DATA){
        /*
         * distinguish minor class
         */
        switch(pData->uchMinorClass){
            case CLASS_PARACPN_MAINT_WRITE:
                /**************************************/
                /* Just Now, Adjective code is edited */
                /**************************************/
                if(uchAdjStatus){
                    uchAdjStatus = 0;
                    if(CPN_FIELD7_ADR <= MaintCpn.uchFieldAddr && MaintCpn.uchFieldAddr <= CPN_FIELD20_ADR){
                        MaintCpn.uchFieldAddr ++;   /* Set next field addr */
                        if(MaintCpn.uchFieldAddr < CPN_FIELD1_ADR || CPN_FIELD22_ADR < MaintCpn.uchFieldAddr){
                           MaintCpn.uchFieldAddr = CPN_FIELD1_ADR;
                        }
                        MaintSetCPNDispData(MaintCpn.uchFieldAddr);
                        return(SUCCESS);
                    }
                }

                return(LDT_SEQERR_ADR);

            case CLASS_PARACPN_MAINT_READ:  
                /******************************************/
                /* Input P4 key or Type# key without data */
                /******************************************/
                MaintCpn.uchFieldAddr ++;
                uchAdjStatus = 0;                 /* Adjective Status Clear */

                /*
                 * check edit field address
                 */
                if(MaintCpn.uchFieldAddr < CPN_FIELD1_ADR || CPN_FIELD22_ADR < MaintCpn.uchFieldAddr){
                    MaintCpn.uchFieldAddr = CPN_FIELD1_ADR;
                }

                /*
                 * set data for display next old data
                 */
                MaintSetCPNDispData(MaintCpn.uchFieldAddr);

                /*
                 * check next address is mnemonics
                 */
                if(MaintCpn.uchFieldAddr == CPN_FIELD6_ADR){
                    return(MAINT_CPN_MNEMO_ADR);            /* mnemonics    */
                }

                return(SUCCESS);

            default: 
                return(LDT_KEYOVER_ADR);
        }
    }

    /****************************/
    /* check status - with data */
    /****************************/
    else{
        /*
         * distinguish minor class
         */
        switch(pData->uchMinorClass){
            case CLASS_PARACPN_MAINT_READ:
                /*************************************/
                /* Input Type# key with address data */
                /*************************************/
                /* check CPN type & address */
                if(pData->uchFieldAddr < CPN_FIELD1_ADR ||
                                       CPN_FIELD22_ADR < pData->uchFieldAddr){
                    return(LDT_KEYOVER_ADR);
                }
                MaintCpn.uchFieldAddr = pData->uchFieldAddr;
                uchAdjStatus = 0;                 /* Adjective Status Clear */
                MaintSetCPNDispData(MaintCpn.uchFieldAddr);

                /*
                 * check next address is mnemonics
                 */
                if(MaintCpn.uchFieldAddr == CPN_FIELD6_ADR){
                    return(MAINT_CPN_MNEMO_ADR);            /* mnemonics    */
                }

                return(SUCCESS);

            case CLASS_PARACPN_MAINT_WRITE:
                /*
                 * distinguish field Address
                 */
                switch(MaintCpn.uchFieldAddr){
                    case CPN_FIELD0_ADR:
                        /*
                         * read CPN parameter for excepting wrong key sequence
                         */
						{
							CPNIF     CpnIf = { 0 };

							CpnIf.uchCpnNo = MaintWork.CPN.uchCPNNumber;
							if(CliOpCpnIndRead(&CpnIf,usMaintLockHnd) != OP_NOT_IN_FILE){
								return(LDT_SEQERR_ADR);
							}
							uchAdjStatus = 0;
						}
                        break;

                    /****************************************/
                    /* Coupon Status character array        */
                    /* => COMCOUPONPARA.uchCouponStatus[2], */
                    /*    PARACPN.uchCPNStatus[2] etc...    */
                    /*       +----+----+       +----+----+  */
                    /*   [0]:|---x|xxxx|   [1]:|xxxx|xxxx|  */
                    /*       +----+----+       +----+----+  */
                    /* Field: adr2 adr1         adr4 adr3   */
                    /****************************************/
                    case CPN_FIELD3_ADR:            /* Coupon Status3       */
                        i = 1;
                    case CPN_FIELD1_ADR:            /* Coupon Status1       */
                        if (pData->uchStatus & MAINT_SCANNER_INPUT) {
                            uchCPNEVersion = 0;
                            return (LDT_SEQERR_ADR);
                        }
                        if (uchCPNEVersion) {
                            uchCPNEVersion = 0;
                            return (LDT_SEQERR_ADR);
                        }
                        uchAdjStatus = 0;
                        /*
                         * check status (4 digit)
                         */
                        sError = MaintChkBinary(pData->aszMnemonics,4);
                        if(sError != SUCCESS){
                            return(LDT_KEYOVER_ADR);
                        }

                        /*
                         * Set status bit
                         */
                        MaintWork.CPN.uchCPNStatus[i] &= 0xf0;
                        MaintWork.CPN.uchCPNStatus[i] |=
                                          MaintAtoStatus(pData->aszMnemonics);
                        break;

                    case CPN_FIELD2_ADR:            /* Coupon Status2       */
                        if (pData->uchStatus & MAINT_SCANNER_INPUT) {
                            uchCPNEVersion = 0;
                            return (LDT_SEQERR_ADR);
                        }
                        if (uchCPNEVersion) {
                            uchCPNEVersion = 0;
                            return (LDT_SEQERR_ADR);
                        }
                        uchAdjStatus = 0;
                        /*
                         * check status2 (2 digit)
                         */
                        sError = MaintChkBinary(pData->aszMnemonics,4);
                        if(sError != SUCCESS){
                            return(LDT_KEYOVER_ADR);
                        }

                        /*
                         * Set status bit
                         */
                        uchMaskData = 0x00;
                        uchMaskData |= MaintAtoStatus(pData->aszMnemonics)<<4;
                        MaintWork.CPN.uchCPNStatus[0] &= 0x0f;
                        MaintWork.CPN.uchCPNStatus[0] |= uchMaskData;
                        break;

                    case CPN_FIELD4_ADR:            /* Coupon Status4       */
                        if (pData->uchStatus & MAINT_SCANNER_INPUT) {
                            uchCPNEVersion = 0;
                            return (LDT_SEQERR_ADR);
                        }
                        if (uchCPNEVersion) {
                            uchCPNEVersion = 0;
                            return (LDT_SEQERR_ADR);
                        }
                        uchAdjStatus = 0;
                        /*
                         * check status1 (3 digit)
                         */
                        sError = MaintChkBinary(pData->aszMnemonics,4);
                        if(sError != SUCCESS){
                            return(LDT_KEYOVER_ADR);
                        }

                        /*
                         * Set status bit
                         */
                        uchMaskData = 0x00;
                        uchMaskData |= MaintAtoStatus(pData->aszMnemonics)<<4;
                        MaintWork.CPN.uchCPNStatus[1] &= 0x0f;
                        MaintWork.CPN.uchCPNStatus[1] |= uchMaskData;
                        break;

                    case CPN_FIELD5_ADR:            /* Coupon Amount        */
                        if (pData->uchStatus & MAINT_SCANNER_INPUT) {
                            uchCPNEVersion = 0;
                            return (LDT_SEQERR_ADR);
                        }
                        if (uchCPNEVersion) {
                            uchCPNEVersion = 0;
                            return (LDT_SEQERR_ADR);
                        }
                        uchAdjStatus = 0;
                        MaintWork.CPN.ulCPNAmount = pData->ulInputData;
                        /*memset(chWorkAmount,'\0',20);                     */
                        /*RflConv4bto3b(chWorkAmount,pData->ulInputData);   */
                        break;

                    case CPN_FIELD6_ADR:            /* Coupon Mnemonics     */
                        if (pData->uchStatus & MAINT_SCANNER_INPUT) {
                            uchCPNEVersion = 0;
                            return (LDT_SEQERR_ADR);
                        }
                        if (uchCPNEVersion) {
                            uchCPNEVersion = 0;
                            return (LDT_SEQERR_ADR);
                        }
                        uchAdjStatus = 0;
                        MaintWork.CPN.aszMnemonic[PARA_CPN_LEN] = 0;
                        _tcsncpy(MaintWork.CPN.aszMnemonic, pData->aszMnemonics, PARA_CPN_LEN);
                        //memcpy(MaintWork.CPN.aszMnemonic, pData->aszMnemonics, PARA_CPN_LEN);
                        break;

                    case CPN_FIELD7_ADR:            /* Target PLU Number[0] */
                    case CPN_FIELD9_ADR:            /* Target PLU Number[1] */
                    case CPN_FIELD11_ADR:           /* Target PLU Number[2] */
                    case CPN_FIELD13_ADR:           /* Target PLU Number[3] */
                    case CPN_FIELD15_ADR:           /* Target PLU Number[4] */
                    case CPN_FIELD17_ADR:           /* Target PLU Number[5] */
                    case CPN_FIELD19_ADR:           /* Target PLU Number[6] */
                        usTerField = (MaintCpn.uchFieldAddr-7)/2;
                        /**************************************************/
                        /* Set PLU Number and Next data is Adjective code */
                        /**************************************************/
                        if (uchAdjStatus == 0){
							PARAPLU          ParaPLU = { 0 };

							/* Analyze PLU Label & Convert (ParaPLU->aucPLUNumber : PLU number) */
                            _tcsncpy(ParaPLU.auchPLUNumber, pData->aszMnemonics, PLU_MAX_DIGIT+1);
                            ParaPLU.uchStatus = pData->uchStatus;
                            sError = MaintPluLabelAnalysis(&ParaPLU, uchCPNEVersion);
                            uchCPNEVersion = 0;
                            if (sError != SUCCESS) {
                                return(sError);
                            }
                            if (_tcsncmp(ParaPLU.auchPLUNumber, MLD_ZERO_PLU, PLU_MAX_DIGIT) == 0) {
                               memset(ParaPLU.auchPLUNumber, 0, sizeof(ParaPLU.auchPLUNumber));
                            }
                            _tcsncpy(&MaintWork.CPN.auchCPNTarget[usTerField][0], ParaPLU.auchPLUNumber, PLU_MAX_DIGIT);
                            /*
                            if(9999L < pData->ulInputData){
                                return(LDT_KEYOVER_ADR);
                            }
                            MaintWork.CPN.usCPNTarget[usTerField]=
                                                   (USHORT)pData->ulInputData; */
                            uchAdjStatus = 1;
                        }
                        /************************************************/
                        /* Set Adjective code and Next Code is Group Code */
                        /************************************************/
                        else {
                            if (pData->uchStatus & MAINT_SCANNER_INPUT) {
                                uchCPNEVersion = 0;
                                return (LDT_SEQERR_ADR);
                            }
                            if (uchCPNEVersion) {
                                uchCPNEVersion = 0;
                                return (LDT_SEQERR_ADR);
                            }
                            if(5L < pData->ulInputData){
                                return(LDT_KEYOVER_ADR);
                            }
                            uchMaskData = (UCHAR)pData->ulInputData;
                            uchMaskData = (UCHAR)(uchMaskData & 0x0F);
                            MaintWork.CPN.uchCPNAdj[usTerField] &= 0xF0;
                            MaintWork.CPN.uchCPNAdj[usTerField] |= uchMaskData;
                            uchAdjStatus = 0;
                        }
                        break;

                    case CPN_FIELD8_ADR:            /* Target PLU Group Number[0] */
                    case CPN_FIELD10_ADR:           /* Target PLU Group Number[1] */
                    case CPN_FIELD12_ADR:           /* Target PLU Group Number[2] */
                    case CPN_FIELD14_ADR:           /* Target PLU Group Number[3] */
                    case CPN_FIELD16_ADR:           /* Target PLU Group Number[4] */
                    case CPN_FIELD18_ADR:           /* Target PLU Group Number[5] */
                    case CPN_FIELD20_ADR:           /* Target PLU Group Number[6] */
                        if (pData->uchStatus & MAINT_SCANNER_INPUT) {
                            uchCPNEVersion = 0;
                            return (LDT_SEQERR_ADR);
                        }
                        if (uchCPNEVersion) {
                            uchCPNEVersion = 0;
                            return (LDT_SEQERR_ADR);
                        }
                        usTerField = (MaintCpn.uchFieldAddr-7)/2;
                        /************************************************/
                        /* Set Group code and Adective status clear */
                        /************************************************/
                        if(2L < pData->ulInputData){
                            return(LDT_KEYOVER_ADR);
                        }
                        uchMaskData = (UCHAR)pData->ulInputData;
                        uchMaskData = (UCHAR)((uchMaskData & 0x0F) <<4);
                        MaintWork.CPN.uchCPNAdj[usTerField] &= 0x0F;
                        MaintWork.CPN.uchCPNAdj[usTerField] |= uchMaskData;
                        uchAdjStatus = 0;
                        break;

					case CPN_FIELD21_ADR:
						MaintWork.CPN.uchAutoCPNStatus = (UCHAR)pData->ulInputData;
						break;
					case CPN_FIELD22_ADR:
						MaintWork.CPN.usBonusIndex = (USHORT)pData->ulInputData;
						break;


                    default:
                        return(LDT_KEYOVER_ADR);
                }

                /******************/
                /* Display to MLD */
                /******************/
                MldParaCpn = MaintWork.CPN;
                MldParaCpn.uchFieldAddr = MaintCpn.uchFieldAddr;
                MldDispItem(&MldParaCpn,0);

                /***********************************************************/
                /* If Next data is not Adjective code, then incrment field */
                /***********************************************************/
                if(uchAdjStatus == 0){
                    MaintCpn.uchFieldAddr ++;    /* Set next field addr */
                }

                /*
                 * check & initialize edit field address
                 */
                if(MaintCpn.uchFieldAddr < CPN_FIELD1_ADR || CPN_FIELD22_ADR < MaintCpn.uchFieldAddr){
                    MaintCpn.uchFieldAddr = CPN_FIELD1_ADR;    /* Init addr */
                }

                MaintSetCPNDispData(MaintCpn.uchFieldAddr);

                /*
                 * check next address is mnemonics
                 */
                if(MaintCpn.uchFieldAddr == CPN_FIELD6_ADR){
                    return(MAINT_CPN_MNEMO_ADR);            /* mnemonics    */
                }

                if(uchAdjStatus){
                    return(MAINT_CPN_ADJ_ADR);              /* Adjective    */
                }

                return(SUCCESS);

            default:
                return(LDT_KEYOVER_ADR);
        }
    }
}

/*
*===========================================================================
** Synopsis:    SHORT MaintCPNWrite( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               sReturn        : error
*
** Description: This function writes & prints CPN PARAMETER RECORD
*===========================================================================
*/

SHORT MaintCPNWrite( VOID )
{
    MAINTTRANS       MaintTrans = { 0 };
	CPNIF            CpnIf = { 0 };
    UCHAR            i;
    SHORT            sReturn;

    CpnIf.uchCpnNo = MaintWork.CPN.uchCPNNumber;                /* Number    */
    memcpy(CpnIf.ParaCpn.uchCouponStatus, MaintWork.CPN.uchCPNStatus,2);
    CpnIf.ParaCpn.ulCouponAmount  = MaintWork.CPN.ulCPNAmount;  /* Amount    */
    _tcsncpy(CpnIf.ParaCpn.aszCouponMnem, MaintWork.CPN.aszMnemonic, PARA_CPN_LEN); /* Mnemonic  */
                    
    for (i = 0; i < STD_MAX_NUM_PLUDEPT_CPN; i++){                   /* PLU No.   */
        _tcsncpy(&CpnIf.ParaCpn.auchCouponTarget[i][0], &MaintWork.CPN.auchCPNTarget[i][0], PLU_MAX_DIGIT);
        /* CpnIf.ParaCpn.usCouponTarget[i] = MaintWork.CPN.usCPNTarget[i]; */
        CpnIf.ParaCpn.uchCouponAdj[i]   = MaintWork.CPN.uchCPNAdj[i];
    }
	CpnIf.ParaCpn.uchAutoCPNStatus = MaintWork.CPN.uchAutoCPNStatus;

	CpnIf.ParaCpn.usBonusIndex = MaintWork.CPN.usBonusIndex;

    /*
     * Write Coupon File & Branch by result of write
     */
    sReturn = SerOpCpnAssign(&CpnIf,usMaintLockHnd);
    if(sReturn != OP_PERFORM){
        return(OpConvertError(sReturn));
    }

    if(uchMaintOpeCo != MAINT_ALREADYOPERATE){
		/*
		 * operation is once time
		 */
        MaintIncreSpcCo(SPCO_CONSEC_ADR);   /* increase consecutive counter */

        MaintMakeHeader(CLASS_MAINTHEADER_RPT,RPT_ACT_ADR,RPT_CPN_ADR,0,
                        RPT_MAINT_ADR,0,(UCHAR)MaintCpn.usACNumber,0,
                        PRT_RECEIPT|PRT_JOURNAL);
        uchMaintOpeCo |= MAINT_ALREADYOPERATE;
    }
    else{
		/*
		 * operation is more than twice time
		 */
        MaintFeed();
    }

    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    MaintTrans.usPrtControl = (PRT_JOURNAL|PRT_RECEIPT);
	RflGetTranMnem (MaintTrans.aszTransMnemo, TRN_CHNG_ADR);
    PrtPrintItem(NULL,&MaintTrans);

    MaintWork.CPN.usPrintControl = (PRT_JOURNAL|PRT_RECEIPT);

    /*
     * Set Minor Class
     */
    /*MaintWork.CPN.uchMinorClass = CLASS_PARACPN_xxxxxx;                    */

    /*
     * print Coupon parameter
     */
    PrtPrintItem(NULL,&(MaintWork.CPN));

    /*
     * send print data to shared module
     */
    PrtShrPartialSend(MaintTrans.usPrtControl);

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintSetCPNDispData(UCHAR uchFieldAddr)
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

VOID MaintSetCPNDispData(UCHAR uchFieldAddr)
{
    USHORT          usTerField;
    USHORT          i = 0;  // indicates which part of uchCPNStatus to access

    memset(MaintCpn.aszMnemonics,0,sizeof(MaintCpn.aszMnemonics));   // clear the mnemonic area and possible replace text below.

    switch(uchFieldAddr){
        case CPN_FIELD3_ADR:
            i = 1;
			// fall through and access the second part of uchCPNStatus
        case CPN_FIELD1_ADR:
            /*MaintCpn.uchMinorClass = CLASS_PARACPN_DATA;*/
            MaintCpn.uchMinorClass = CLASS_PARACPN_STS;
            MaintCpn.ulInputData = (ULONG)(MaintWork.CPN.uchCPNStatus[i] & 0x0f);
			RflGetLead(MaintCpn.aszMnemonics, LDT_STATUS_ADR);
			break;

        case CPN_FIELD4_ADR:
            i = 1;
			// fall through and access the second part of uchCPNStatus
        case CPN_FIELD2_ADR:
            /*MaintCpn.uchMinorClass = CLASS_PARACPN_DATA;*/
            MaintCpn.uchMinorClass = CLASS_PARACPN_STS;
            MaintCpn.ulInputData = (ULONG)((MaintWork.CPN.uchCPNStatus[i] & 0xf0) >> 4);
			RflGetLead(MaintCpn.aszMnemonics, LDT_STATUS_ADR);
			break;

        case CPN_FIELD5_ADR:                                /* Amount       */
            MaintCpn.uchMinorClass = CLASS_PARACPN_DATA;
            MaintCpn.ulInputData = MaintWork.CPN.ulCPNAmount;
            /*RflConv3bto4b(&MaintCpn.ulInputData,chWorkAmount);*/
			RflGetLead(MaintCpn.aszMnemonics, LDT_PP_ADR);
			break;

        case CPN_FIELD6_ADR:                                 /* Mnemonics   */
            MaintCpn.uchMinorClass = CLASS_PARACPN_DATA;
            MaintCpn.ulInputData = 0;
            _tcsncpy(MaintCpn.aszMnemonics, MaintWork.CPN.aszMnemonic, PARA_CPN_LEN);
            break;

        case CPN_FIELD7_ADR:                             /* PLU No. 1-7    */
        case CPN_FIELD9_ADR:
        case CPN_FIELD11_ADR:
        case CPN_FIELD13_ADR:
        case CPN_FIELD15_ADR:
        case CPN_FIELD17_ADR:
        case CPN_FIELD19_ADR:                            /* Last Field     */
            usTerField = (MaintCpn.uchFieldAddr-7)/2;
            /*********************/
            /* Display "PLU No." */
            /*********************/
            /*if(uchAdjStatus == 0){
             *   MaintCpn.uchMinorClass  = CLASS_PARACPN_DATA;
             *   MaintCpn.ulInputData =
             *               (ULONG)MaintWork.CPN.usCPNTarget[usTerField];
             *}
             */
            /*************************************/
            /* Display "PLU No. - Adjective No." */
            /*************************************/
            /*else{*/
                MaintCpn.uchMinorClass  = CLASS_PARACPN_ONEADJ;
                _tcsncpy(MaintCpn.auchCPNTarget, &MaintWork.CPN.auchCPNTarget[usTerField][0], PLU_MAX_DIGIT);
                /* MaintCpn.ulInputData = (ULONG)MaintWork.CPN.usCPNTarget[usTerField]; */
                MaintCpn.uchCPNAdj = MaintWork.CPN.uchCPNAdj[usTerField];
            /*}*/
			RflGetLead(MaintCpn.aszMnemonics, LDT_NUM_ADR);
			break;

        case CPN_FIELD8_ADR:                             /* PLU No. 1-7    */
        case CPN_FIELD10_ADR:
        case CPN_FIELD12_ADR:
        case CPN_FIELD14_ADR:
        case CPN_FIELD16_ADR:
        case CPN_FIELD18_ADR:
        case CPN_FIELD20_ADR:                            /* Last Field     */
            usTerField = (MaintCpn.uchFieldAddr-7)/2;
            /************************/
            /* Display "Group Code" */
            /************************/
            MaintCpn.uchMinorClass  = CLASS_PARACPN_GROUPCODE;
            _tcsncpy(MaintCpn.auchCPNTarget, &MaintWork.CPN.auchCPNTarget[usTerField][0], PLU_MAX_DIGIT);
            /* MaintCpn.ulInputData = (ULONG)MaintWork.CPN.usCPNTarget[usTerField]; */
            MaintCpn.uchCPNAdj = MaintWork.CPN.uchCPNAdj[usTerField];
			RflGetLead(MaintCpn.aszMnemonics, LDT_DATA_ADR);
			break;

		case CPN_FIELD21_ADR:
            /*MaintCpn.uchMinorClass = CLASS_PARACPN_DATA;*/
            MaintCpn.uchMinorClass = CLASS_PARACPN_AUTO_PRIORITY;
            MaintCpn.ulInputData = (ULONG)MaintWork.CPN.uchAutoCPNStatus;
			RflGetLead(MaintCpn.aszMnemonics, LDT_STATUS_ADR);
			break;

		case CPN_FIELD22_ADR:
			MaintCpn.uchMinorClass = CLASS_PARACPN_BONUS_INDEX;
			MaintCpn.ulInputData = (ULONG)MaintWork.CPN.usBonusIndex;
			RflGetLead (MaintCpn.aszMnemonics, LDT_DATA_ADR);
			break;

        default:
            return;
    }

    MaintCpn.uchMajorClass = CLASS_PARACPN;        /* set major class      */
    DispWrite(&MaintCpn);
}

/*
*===========================================================================
** Synopsis:    SHORT  MaintCPNLock( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*
** Return:      SUCCESS         : Successful
*               LDT_LOCK_ADR    : During Lock
*
** Description: Lock CPN File and Save This Lock Handle to Own Save Area
*===========================================================================
*/

SHORT MaintCPNLock()
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
** Synopsis:    VOID  MaintCPNUnLock( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*
** Return:      Nothing
*
** Description: UnLock CPN File
*===========================================================================
*/

VOID MaintCPNUnLock()
{
    SerOpUnLock();
    usMaintLockHnd = 0;
}

/*
*===========================================================================
** Synopsis:    VOID MaintCPNReport( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function prints all COUPON Parameter
*===========================================================================
*/
VOID MaintCPNReport( VOID )
{
    SHORT       sReturn;
    UCHAR       EndOfCoupon = 0;                       /* Data End Flag      */
    USHORT      i, j;

    /*
     * Check operational times and Print Header
     */
    if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {    /* 1'st operation      */
        MaintIncreSpcCo(SPCO_CONSEC_ADR);             /* Inc consecutive cnt */

        MaintMakeHeader(CLASS_MAINTHEADER_RPT,        /* Minor class         */
                        RPT_ACT_ADR,                  /* "ACTION"            */
                        RPT_CPN_ADR,                  /* "CPN"               */
                        0,
                        RPT_FILE_ADR,                 /* "FILE" */
                        0,
                        AC_CPN_MAINT,                 /* Action number(161)  */
                        0,
                        PRT_RECEIPT | PRT_JOURNAL);

        uchMaintOpeCo |= MAINT_ALREADYOPERATE;
    }
    /* set print control */

    MaintWork.CPN.usPrintControl = 0;

    if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT0))) {
        MaintWork.CPN.usPrintControl |= PRT_JOURNAL;   /* print out on journal */
    } 

    if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT1))) {
        MaintWork.CPN.usPrintControl |= PRT_RECEIPT;   /* print out receipt */
    }


    /****************/
    /* Issue Report */
    /****************/
    for(i = 1; i <= FLEX_CPN_MAX; i++){
		CPNIF       CpnIf = { 0 };       /* Read parameter I/F */

        /*
         * Check abort key
         */
        if(UieReadAbortKey() == UIE_ENABLE){            /* depress abort key */
            MaintMakeAbortKey();
            MaintFeed();                                /* 1 line feed       */
            break;
        }

        /*
         * Reading Coupon Parameter File
         */
        CpnIf.uchCpnNo = i;
        sReturn = CliOpCpnIndRead(&CpnIf,usMaintLockHnd);

        switch(sReturn){
            case OP_PERFORM:                            /* Read is completed */
                /*
                 * set data for printing
                 */
                MaintWork.CPN.uchMajorClass = CLASS_PARACPN;
                MaintWork.CPN.uchCPNNumber  = CpnIf.uchCpnNo;
                memcpy(MaintWork.CPN.uchCPNStatus, CpnIf.ParaCpn.uchCouponStatus, 2);
                MaintWork.CPN.ulCPNAmount   = CpnIf.ParaCpn.ulCouponAmount;
                _tcsncpy(MaintWork.CPN.aszMnemonic, CpnIf.ParaCpn.aszCouponMnem, PARA_CPN_LEN);

                for (j = 0; j < STD_MAX_NUM_PLUDEPT_CPN; j++){
                    _tcsncpy(&MaintWork.CPN.auchCPNTarget[j][0], &CpnIf.ParaCpn.auchCouponTarget[j][0], PLU_MAX_DIGIT);
                    /* MaintWork.CPN.usCPNTarget[j] = CpnIf.ParaCpn.usCouponTarget[j]; */
                    MaintWork.CPN.uchCPNAdj[j] = CpnIf.ParaCpn.uchCouponAdj[j];
                }

                PrtPrintItem(NULL,&(MaintWork.CPN));    /* print 1 data      */
                if(MaintWork.CPN.usPrintControl & PRT_RECEIPT){   /* print out receipt on ? */
                    MaintFeed();                            /* 1 line feed       */
                }
                break;

            /*
             * <<< Caution >>>
             *
             *   CliOpCpnIndRead() return "OP_ABNORMAL_CPN",
             *   when specified Coupon Number is Zero or
             *   over the Max Number of Coupon that setted Prog#2.
             *
             */
            case OP_ABNORMAL_CPN:
                EndOfCoupon = 1;
                MaintFeed();                            /* 1 line feed       */
                break;

            case OP_FILE_NOT_FOUND:

            default:
                MaintFeed();                            /* 1 line feed       */
                break;
        }

        if(EndOfCoupon != 0)     break;                 /* Data End          */
    }

    /*
     * make trailer
     */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

    return;
}

/*
*===========================================================================
** Synopsis:    SHORT MaintCPNPLUModifier( UCHAR uchMinorClass )
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
SHORT MaintCPNPLUModifier( UCHAR uchMinorClass )
{

    switch(uchMinorClass) {
    case CLASS_PARAPLU_EVERSION:
        uchCPNEVersion = (UCHAR)((uchCPNEVersion & MOD_STAT_EVERSION) ? 0 : MOD_STAT_EVERSION);   /* toggle control */
    case CLASS_PARAPLU_SET_EV:
        uchCPNEVersion |= MOD_STAT_EVERSION;
        break;

    case CLASS_PARAPLU_RESET_EV:
        uchCPNEVersion &= ~MOD_STAT_EVERSION;
        break;

    default:
        break;
    }
    return(SUCCESS);
}



VOID MaintCouponDisplayFirstPage()
{
    if (uchMaintMldPageNo) {
		PARACPN       MldParaCpn = MaintWork.CPN;                            /* For MLD */

        MldParaCpn.usACNumber   = AC_CPN_MAINT;
        MldDispItem(&MldParaCpn, 0); /* redisplay 1st page */
        uchMaintMldPageNo = 0;
     }
}

VOID MaintCouponDisplaySecondPage()
{
    if (! uchMaintMldPageNo) {
		PARACPN       MldParaCpn = MaintWork.CPN;                            /* For MLD */

        MldParaCpn.usACNumber   = AC_CPN_MAINT;
        MldDispItem(&MldParaCpn, 1); /* redisplay 2nd page */
        uchMaintMldPageNo = 1;
    }
}

/*==========================================================================*\
                          E N D   O F   M A T C P N . C
\*==========================================================================*/