/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-8          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Set Flexible Drive Through/Multi Store Recall Parameter Module
* Category    : User Interface For Supervisor, NCR 2170 US Hospitality Application
* Program Name: UIFAC162.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*               UifAC162Function()        : A/C No.162 Function Entry Mode
*               UifAC162EnterAddr()       : A/C No.162 Enter Address Mode
*               UifAC162EnterData1()      : A/C No.162 Enter Data1 Mode
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev.   :   Name    : Description
* Apr-17-95: 03.00.00   : T.Satoh   : initial
* Aug-27-98: 03.03.00   : hrkato    : V3.3
* Aug-16-99: 03.05.00   : K.Iwata   : V3.5 merge GUEST_CHECK_MODEL,STORE_RECALL_MODEL
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
============================================================================
                      I N C L U D E     F I L E s
============================================================================
*/
#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifsup.h>
#include <uifpgequ.h>
#include <mldmenu.h>

#include "uifsupex.h"



/*
============================================================================
                      C O M M O N   R O M   T A B L E s
============================================================================
*/
/*
 * Define Key Sequence
 */
UISEQ FARCONST aszSeqAC162EnterAddr[] = {FSC_AC, FSC_NUM_TYPE, 0};
UISEQ FARCONST aszSeqAC162EnterData[] = {FSC_AC, FSC_NUM_TYPE, 0};

/*
 * Define Next Function
 */
UIMENU FARCONST
aChildAC162Init[]   = {
                       {UifAC162EnterAddr, CID_AC162ENTERADDR},
                       {NULL             , 0                 }
                      };

UIMENU FARCONST
aChildAC162Accept[] = {
                       {UifAC162EnterData, CID_AC162ENTERDATA},
                       {NULL             , 0                 }
                      };

/*
*===========================================================================
**  Synopsis: SHORT UifAC162Function(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*
**  Description: A/C No.162 Function Entry Mode
*===========================================================================
*/
SHORT UifAC162Function(KEYMSG *pKeyMsg)
{
    /*
     * A/C 162's Main Control brancher
     */
    switch(pKeyMsg->uchMsg){
        case UIM_INIT:
            UieNextMenu(aChildAC162Init);           /* Open Next Function   */
            break;

        case UIM_ACCEPTED:
            UieNextMenu(aChildAC162Accept);         /* Open Next Function   */
            break;

        default:
            return(UifACDefProc(pKeyMsg));          /* exec Default Proc    */
    }

    return(SUCCESS);
}

/*
*===========================================================================
**  Synopsis: SHORT UifAC162EnterAddr(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message
*      Output:  nothing
*
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               
**  Description: A/C No.162 Enter Address Mode
*===========================================================================
*/
SHORT UifAC162EnterAddr(KEYMSG *pKeyMsg)
{
    SHORT       sError = SUCCESS;
    PARAFXDRIVE ParaFxDrive;

    switch(pKeyMsg->uchMsg){
        case UIM_INIT:
            /*
             * Display initialize and open key sequence
             */
/*
            MaintDisp(AC_FLEX_DRIVE,
                      CLASS_MAINTDSPCTL_10NPG,0,0,0,0,0L,LDT_NUM_ADR);
*/
            UieOpenSequence(aszSeqAC162EnterAddr);

            /********************/
            /* Reading All data */
            /********************/
            ParaFxDrive.uchMajorClass = CLASS_PARAFXDRIVE;
            ParaFxDrive.uchMinorClass = CLASS_PARAFXDRIVE_ALL;
            ParaFxDrive.uchTblAddr    = FXDRV_FIELD0_ADR;
            ParaFxDrive.uchStatus     = 0;

            MaintFlexDriveRead(&ParaFxDrive);

            break;

        case UIM_INPUT:
            switch(pKeyMsg->SEL.INPUT.uchMajor){
                case FSC_NUM_TYPE:
                    /*
                     * Over Digit Check
                     */
                    if(pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2){
                        sError = LDT_KEYOVER_ADR;
                        UieReject();
                        break;
                    }

                    if(pKeyMsg->SEL.INPUT.lData > FXDRV_FIELD36_ADR){
                        sError = LDT_KEYOVER_ADR;
                        UieReject();
                        break;
                    }

                    /********************************/
                    /* Execute Read Table Procedure */
                    /********************************/
                    ParaFxDrive.uchMajorClass = CLASS_PARAFXDRIVE;
                    ParaFxDrive.uchTblAddr    = 0;
                    ParaFxDrive.uchStatus     = 0;

                    if(pKeyMsg->SEL.INPUT.uchLen == 0){
                        ParaFxDrive.uchMinorClass = CLASS_PARAFXDRIVE_NEXT;
                        ParaFxDrive.uchStatus |= MAINT_WITHOUT_DATA;
                    }
                    else{
                       /*
                        * Zero Check
                        */ 
                       if(pKeyMsg->SEL.INPUT.lData == FXDRV_FIELD0_ADR){
                            sError = LDT_KEYOVER_ADR;
                            UieReject();
                            break;
                        }

                        ParaFxDrive.uchMinorClass = CLASS_PARAFXDRIVE_ADR;
                        ParaFxDrive.uchTblAddr =
                                              (UCHAR)pKeyMsg->SEL.INPUT.lData;
                    }

                    if((sError=MaintFlexDriveRead(&ParaFxDrive)) == SUCCESS){
                        UieAccept();
                    }

                    break;
            
                case FSC_AC:
                    /***********************************/
                    /* Issue Report & Finalize A/C 162 */
                    /***********************************/
                    if(pKeyMsg->SEL.INPUT.uchLen == 0){
                        MaintFlexDriveReport();
                        MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                        UieExit(aACEnter);
                        break;
                    }

                    /****************************************************/
                    /* Writing Current Data to Flex Drive Through Table */
                    /****************************************************/
                    else{
                        /*
                         * Check Over Digit
                         */
                        if(pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2){
                            sError = LDT_KEYOVER_ADR;
                            break;
                        }

                        /************************/
                        /* Writing Current Data */
                        /************************/
                        ParaFxDrive.uchMajorClass = CLASS_PARAFXDRIVE;
                        ParaFxDrive.uchMinorClass = CLASS_PARAFXDRIVE_ADR;
                        ParaFxDrive.uchStatus     = 0;
                        ParaFxDrive.uchTblData[0] =
                                               (UCHAR)pKeyMsg->SEL.INPUT.lData;

                        sError = MaintFlexDriveWrite(&ParaFxDrive);

                        /*********************/
                        /* Reading Next Data */
                        /*********************/
                        if(sError == SUCCESS){
                            ParaFxDrive.uchMajorClass = CLASS_PARAFXDRIVE;
                            ParaFxDrive.uchMinorClass = CLASS_PARAFXDRIVE_NEXT;
                            ParaFxDrive.uchStatus     = 0;

                            sError = MaintFlexDriveRead(&ParaFxDrive);

                            /*
                             * If Write and Read proc' success,
                             *      then accept this sequene and
                             *         return to UifAC162Function().
                             */
                            if(sError == SUCCESS){
                                UieAccept();
                                break;
                            }
                        }

                        /*
                         * Anythig Error - retry this sequence
                         */
                        UieReject();
                    }
                    break;

                default:
                    return(UifACDefProc(pKeyMsg));
            }
            break;

        default:
            return(UifACDefProc(pKeyMsg));  /* Execute Default Procedure    */
    }

    return(sError);
}

/*
*===========================================================================
**  Synopsis: SHORT UifAC162EnterData(KEYMSG *pKeyMsg)
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.162 Enter Data Mode 
*===========================================================================
*/
SHORT UifAC162EnterData(KEYMSG *pKeyMsg)
{
    SHORT       sError = SUCCESS;
    PARAFXDRIVE ParaFxDrive;

    switch(pKeyMsg->uchMsg){
        case UIM_INIT:
            UieOpenSequence(aszSeqAC162EnterData);
            break;

        case UIM_INPUT:
            switch(pKeyMsg->SEL.INPUT.uchMajor){
                case FSC_NUM_TYPE:
                    /*
                     * Over Digit Check
                     */
                    if(pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2){
                        sError = LDT_KEYOVER_ADR;
                        UieReject();
                        break;
                    }

                    if(pKeyMsg->SEL.INPUT.lData > FXDRV_FIELD36_ADR){
                        sError = LDT_KEYOVER_ADR;
                        UieReject();
                        break;
                    }

                    /********************************/
                    /* Execute Read Table Procedure */
                    /********************************/
                    ParaFxDrive.uchMajorClass = CLASS_PARAFXDRIVE;
                    ParaFxDrive.uchTblAddr    = 0;
                    ParaFxDrive.uchStatus     = 0;

                    if(pKeyMsg->SEL.INPUT.uchLen == 0){
                        ParaFxDrive.uchMinorClass = CLASS_PARAFXDRIVE_NEXT;
                        ParaFxDrive.uchStatus |= MAINT_WITHOUT_DATA;
                    }
                    else{
                       /*
                        * Zero Check
                        */ 
                       if(pKeyMsg->SEL.INPUT.lData == FXDRV_FIELD0_ADR){
                            sError = LDT_KEYOVER_ADR;
                            UieReject();
                            break;
                        }

                        ParaFxDrive.uchMinorClass = CLASS_PARAFXDRIVE_ADR;
                        ParaFxDrive.uchTblAddr =
                                              (UCHAR)pKeyMsg->SEL.INPUT.lData;
                    }

                    if((sError=MaintFlexDriveRead(&ParaFxDrive)) == SUCCESS){
                        UieReject();
                    }

                    break;

                case FSC_AC:
                    /********************/
                    /* Finalize A/C 162 */
                    /********************/
                    if(pKeyMsg->SEL.INPUT.uchLen == 0){
                        MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                        UieExit(aACEnter);
                        break;
                    }

                    /****************************************************/
                    /* Writing Current Data to Flex Drive Through Table */
                    /****************************************************/
                    else{
                        /*
                         * Check Over Digit
                         */
                        if(pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2){
                            sError = LDT_KEYOVER_ADR;
                            break;
                        }

                        /************************/
                        /* Writing Current Data */
                        /************************/
                        ParaFxDrive.uchMajorClass = CLASS_PARAFXDRIVE;
                        ParaFxDrive.uchMinorClass = CLASS_PARAFXDRIVE_ADR;
                        ParaFxDrive.uchStatus     = 0;
                        ParaFxDrive.uchTblData[0] =
                                               (UCHAR)pKeyMsg->SEL.INPUT.lData;

                        sError = MaintFlexDriveWrite(&ParaFxDrive);

                        /*********************/
                        /* Reading Next Data */
                        /*********************/
                        if(sError == SUCCESS){
                            ParaFxDrive.uchMajorClass = CLASS_PARAFXDRIVE;
                            ParaFxDrive.uchMinorClass = CLASS_PARAFXDRIVE_NEXT;
                            ParaFxDrive.uchStatus     = 0;

                            sError = MaintFlexDriveRead(&ParaFxDrive);

                            if(sError == SUCCESS){
                                UieReject();
                            }
                        }
                    }
                    break;

                default:
                    return(UifACDefProc(pKeyMsg));      /* Exec Default Proc        */
            }
            break;

        default:
            return(UifACDefProc(pKeyMsg));      /* Exec Default Proc        */
    }

    return(sError);
}


/* --- End of Source --- */
