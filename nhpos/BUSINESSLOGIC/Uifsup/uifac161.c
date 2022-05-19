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
* Title       : Coupon Maintenance Module
* Category    : User Interface For Supervisor,
*               NCR 2170 US Hospitality Application
* Program Name: UIFAC161.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
*                UifAC161Function()      : A/C No.161 Function Entry Mode
*                UifAC161EnterData1()    : A/C No.161 Enter Data1 Mode
*                UifAC161EnterData2()    : A/C No.161 Enter Data2 Mode
*                UifAC161ErrorCorrect()  : A/C No.161 Error Correct Mode
*                UifAC161EnterALPHA()    : A/C No.161 Enter Coupon Mnemonics Mode
*                UifAC161ExitFunction()  : A/C No.161 Exit Function Mode
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev.   :   Name    : Description
* Mar-14-95:03.00.00    :T.SATOH    : initial
* Apr-14-95:03.00.00    :T.SATOH    : Adds clear MLD
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
/*==========================================================================*/
/*                        I N C L U D E     F I L E s                       */
/*==========================================================================*/
#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <fsc.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <uifpgequ.h>
#include <uifsup.h>
#include <cvt.h>
#include <mldmenu.h>

#include "uifsupex.h"                        /* Unique Extern Header for UI */


/*==========================================================================*/
/*                    C O M M O N   R O M   T A B L E s                     */
/*==========================================================================*/

UCHAR   uchRptEnable = 0;                    /* Permit flag of Report Issue */
                                             /*    0 : Issue OK             */
                                             /*    1 : Prohibit issue       */

/*****************************/
/* Define Key Sequence table */
/*****************************/
static UISEQ CONST aszSeqAC161EnterData1[]   = { FSC_AC, FSC_COUPON, FSC_KEYED_COUPON, FSC_SCROLL_DOWN,FSC_SCROLL_UP, 0 };

static UISEQ CONST aszSeqAC161EnterData2[]   = { FSC_NUM_TYPE, FSC_AC, FSC_EC, FSC_SCANNER, FSC_E_VERSION,FSC_SCROLL_DOWN,FSC_SCROLL_UP, 0 };

static UISEQ CONST aszSeqAC161Error[]        = { FSC_CANCEL, 0 };

static UISEQ CONST aszSeqAC161EnterALPHA[]   = { FSC_P1, 0 };

static UISEQ CONST aszSeqAC161Adjective[]    = { FSC_AC, FSC_NUM_TYPE,FSC_SCROLL_DOWN,FSC_SCROLL_UP, 0 };

/******************************/
/* Define Next Function table */
/******************************/
static UIMENU CONST aChildAC161Init1[]        = {
                            { UifAC161EnterData1,     CID_AC161ENTERDATA1    },
                            { UifAC161ErrorFunction,  CID_AC161ERRORFUNCTION },
                            { NULL,                   0                      }
                        };

static UIMENU CONST aChildAC161Init2[]        = {
                            { UifAC161EnterData1,     CID_AC161ENTERDATA1    },
                            { UifAC161ExitFunction,   CID_AC161EXITFUNCTION  },
                            { UifAC161CancelFunction, CID_AC161CANCELFUNCTION},
                            { NULL,                   0                      }
                        };

static UIMENU CONST aChildAC161Accept1[]    = {
                            { UifAC161EnterData2,     CID_AC161ENTERDATA2    },
                            { UifAC161CancelFunction, CID_AC161CANCELFUNCTION},
                            { NULL,                   0                      }
                        };

static UIMENU CONST aChildAC161EnterData2[] = {
                            { UifACShift,             CID_ACSHIFT            },
                            { UifAC161EnterALPHA,     CID_AC161ENTERALPHA    },
                            { UifACChangeKB1,         CID_ACCHANGEKB1        },
                            { NULL,                   0                      }
                        };

static UIMENU CONST aChildAC161Adjective[] = {
                            { UifAC161Adjective,      CID_AC161ADJECTIVE     },
                            { NULL,                   0                      }
                        };

static UIMENU CONST aParentAC161Func[]      = {
                            { UifAC161Function,       CID_AC161FUNCTION      },
                            { NULL,                   0                      }
                        };

/*==========================================================================*/
/*                        P R O G R A M    S E C T I O N                    */
/*==========================================================================*/

/*
*=============================================================================
**  Synopsis: SHORT UifAC161Function(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.161 Function Entry Mode.
*==============================================================================
*/
SHORT UifAC161Function(KEYMSG *pKeyMsg)
{
    SHORT            sError = SUCCESS;

    /*****************************/
    /* Branch by kind of message */
    /*****************************/
    switch(pKeyMsg->uchMsg){
        case UIM_INIT:
            switch(pKeyMsg->SEL.usFunc){
                case CID_AC161ENTERDATA2:
                    UieNextMenu(aChildAC161Init2);
                    break;

                default:
                    UieNextMenu(aChildAC161Init1);
                    UieEchoBack(UIE_ECHO_ROW0_NUMBER,UIF_DIGIT10);
                    uchRptEnable = 0;                     /* Issue Report OK */
                    break;
            }
            break;

        case UIM_ACCEPTED:
            UieNextMenu(aChildAC161Accept1);
            uchRptEnable = 1;                             /* Prohibit issue  */
            break;

        default:
            sError = UifACDefProc(pKeyMsg);
            break;
    }

    return(sError);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC161EnterData1(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.161 Enter Data1 Mode.
*==============================================================================
*/
SHORT UifAC161EnterData1(KEYMSG *pKeyMsg)
{
    SHORT            sError = SUCCESS;
    /*UCHAR            uchCPNNumber;*/
	PARACPN            CPNNoData = { 0 };                 /* For MaintCPNRead()    */

    /*****************************/
    /* Branch by kind of message */
    /*****************************/
    switch(pKeyMsg->uchMsg){
        case UIM_INIT :
            UifACPGMLDClear();                    /* Clear MLD             */

            /*
             * set display type & print 1'st Lead-Through "NUMBER ENTRY"
             */
            MaintDisp(AC_CPN_MAINT,                     /* A/C Number */
                  CLASS_MAINTDSPCTL_10NPG,              /* Set Minor Class for 10N10D */
                      0,                                /* Page Number */
                      0,                                /* PTD Type */
                      0,                                /* Report Type */
                      0,                                /* Reset Type */
                      0L,                               /* Amount Data */
                      LDT_CPNNO_ADR);                   /* "Request Coupon # Entry" Lead Through Address */
            UieOpenSequence(aszSeqAC161EnterData1);
            break;

        case UIM_INPUT :
            /*******************************/
            /* Branch by kind of input-key */
            /*******************************/
            switch(pKeyMsg->SEL.INPUT.uchMajor){
                case FSC_AC :
                    /*************************************/
                    /* Issue Report and Finalize A/C 161 */
                    /*************************************/
                    if(pKeyMsg->SEL.INPUT.uchLen == 0){
                        /*
                         * Check permit flag and issue Report
                         */
                        if(uchRptEnable == 0){
                            MaintCPNReport();
                        }
                        MaintCPNUnLock();               /* UnLock CPN File */
                        MaintFin(CLASS_MAINTTRAILER_PRTSUP);
                        UieExit(aACEnter);
                        return(SUCCESS);
                    }
                    sError = LDT_SEQERR_ADR;
                    break;

                case FSC_COUPON :
                    /**********************/
                    /* Check input colomn */
                    /**********************/
                    if(pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT3){
                        sError = LDT_KEYOVER_ADR;        /* Over col No.   */
                        break;
                    }

                    /******************************/
                    /* Set parameters to read CPN */
                    /******************************/
                    if(pKeyMsg->SEL.INPUT.uchLen == 0){  /*Without any keys*/
                        CPNNoData.uchStatus |= MAINT_WITHOUT_DATA;
                    }
                    else{                                /* With some keys */
                        NHPOS_ASSERT(pKeyMsg->SEL.INPUT.lData <= 0xFF);//MAXBYTE
						CPNNoData.uchCPNNumber = (UCHAR)pKeyMsg->SEL.INPUT.lData;
                    }
                    break;

                case FSC_KEYED_COUPON:
                    /**********************/
                    /* Check input colomn */
                    /**********************/
                    if(pKeyMsg->SEL.INPUT.uchLen){
                        sError = LDT_KEYOVER_ADR;        /* With some keys */
                        break;
                    }

                    /******************/
                    /* Get CPN Number */
                    /******************/
                    CPNNoData.uchCPNNumber = pKeyMsg->SEL.INPUT.uchMinor;
                    CPNNoData.uchStatus = 0;
                    break;

                default:
                    return(UifACDefProc(pKeyMsg));
            }

            if(sError != SUCCESS)    break;         /* Exist some error    */

            /*********************/
            /* Read CPN Function */
            /*********************/
            CPNNoData.uchMajorClass = CLASS_PARACPN; /* Major Class        */
            CPNNoData.usACNumber = AC_CPN_MAINT;     /* A/C Number         */

            if((sError=MaintCPNRead(&CPNNoData)) == SUCCESS){
                UieAccept();                    /* Ret UifAC161Function()  */
            }
            break;

        default:
            sError = UifACDefProc(pKeyMsg);         /* Default Proc        */
            break;
    }

    return(sError);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC161EnterData2(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.161 Enter Data2 Mode.
*==============================================================================
*/
SHORT UifAC161EnterData2(KEYMSG *pKeyMsg)
{
    SHORT        sError = SUCCESS;
    SHORT        sEditflg = 0;
	MAINTCPN     CPNEditData = { 0 };

    /*****************************/
    /* Branch by kind of message */
    /*****************************/
    switch(pKeyMsg->uchMsg){
        case UIM_INIT :
            UieEchoBack(UIE_ECHO_ROW0_NUMBER,UIF_DIGIT14);
            UieOpenSequence(aszSeqAC161EnterData2);
            
            MaintCPNPLUModifier(CLASS_PARAPLU_RESET_EV); /* reset E-Version */
            break;

        case UIM_ACCEPTED :
            /*************************************/
            /* Change Key-Board  by P4's Process */
            /*************************************/
            if(pKeyMsg->SEL.usFunc == CID_ACCHANGEKB1){
                UieOpenSequence(aszSeqAC161EnterData2);
                CPNEditData.uchMajorClass = CLASS_MAINTCPN;
                CPNEditData.uchMinorClass = CLASS_PARACPN_MAINT_READ;
                CPNEditData.uchStatus = MAINT_WITHOUT_DATA;
                MaintCPNEdit(&CPNEditData);
            }

            UieAccept();                        /* Ret  UifAC161Function()  */
            break;

        case UIM_INPUT :
            /*******************************/
            /* Branch by kind of input-key */
            /*******************************/
            switch(pKeyMsg->SEL.INPUT.uchMajor){
                case FSC_NUM_TYPE :
            
                    MaintCPNPLUModifier(CLASS_PARAPLU_RESET_EV); /* Reset E-Version */
                    
                    if(pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT2){
                        sError = LDT_KEYOVER_ADR;        /* Over colomn     */
                        break;
                    }

                    /****************************************/
                    /* Set each parameters to edit CPN data */
                    /****************************************/
                    CPNEditData.uchMinorClass = CLASS_PARACPN_MAINT_READ;
                    CPNEditData.uchStatus = 0;         /* Init Status       */

                    if(pKeyMsg->SEL.INPUT.uchLen == 0){ /* without nay keys */
                        CPNEditData.uchStatus |= MAINT_WITHOUT_DATA;
                    }
                    else{                                /* with some keys  */
                        CPNEditData.uchFieldAddr = (UCHAR)pKeyMsg->SEL.INPUT.lData;
                    }

                    sEditflg = 1;
                    break;

                case FSC_AC :
                    /***************************/
                    /* Execute Write Procedure */
                    /***************************/
                    if(pKeyMsg->SEL.INPUT.uchLen == 0){/* without any keys  */
                        if((sError=MaintCPNWrite()) == SUCCESS){ 
							uchMaintMldPageNo = 0;
                            UieExit(aParentAC161Func);/*Ret UifAC161Function*/
                        }
                    }

                    /****************************************/
                    /* Set each parameters to edit CPN data */
                    /****************************************/
                    else{                                /* with some keys  */
                        if(pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT14){
                            sError = LDT_KEYOVER_ADR;    /* Over colomn     */
                            break;
                        }

                        CPNEditData.uchMinorClass = CLASS_PARACPN_MAINT_WRITE;
                        CPNEditData.uchStatus = 0;
                        CPNEditData.ulInputData = (ULONG)pKeyMsg->SEL.INPUT.lData;

                        _tcsncpy(CPNEditData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, PARA_LEADTHRU_LEN);

                        sEditflg = 1;
                    }
                    break;

                case FSC_SCANNER:
                    if((pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen > UIF_DIGIT14) ||  /* Over digit */
                       (pKeyMsg->SEL.INPUT.DEV.SCA.uchStrLen == 0)) {           /* or no entry */
                        return(LDT_KEYOVER_ADR);
                    }

                    CPNEditData.uchMinorClass = CLASS_PARACPN_MAINT_WRITE;
                    CPNEditData.uchStatus = 0;
                    CPNEditData.uchStatus |= MAINT_SCANNER_INPUT;
                    _tcsncpy(CPNEditData.aszMnemonics, pKeyMsg->SEL.INPUT.DEV.SCA.auchStr, PARA_LEADTHRU_LEN);
 
                    sEditflg = 1;
                    break;
            
                case FSC_E_VERSION:
                    if (pKeyMsg->SEL.INPUT.uchLen) {        /* With Amount ? */
                        return(LDT_KEYOVER_ADR);
                    }
                    MaintCPNPLUModifier(CLASS_PARAPLU_SET_EV); /* set E-Version */
                    break;

                case FSC_EC :
                    if(pKeyMsg->SEL.INPUT.uchLen){   /* with something keys */
                        sError = LDT_SEQERR_ADR;
                        break;
                    }

                    UieExit(aParentAC161Func);      /* Ret UifAC161Function */
                    break;


				/* ---- scroll control for plu maintenance R3.1 ---- */
				case FSC_SCROLL_UP:
					if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount Input Case */
						return(LDT_SEQERR_ADR);
					} else {                                                /* W/o Amount Input Case */
						MaintCouponDisplayFirstPage();                         /* display 1st page */
					}
					UieReject();                                            /* Shift Current Func. to Active Func. */
					return(SUCCESS);

				case FSC_SCROLL_DOWN:
					if (pKeyMsg->SEL.INPUT.uchLen) {                        /* W/ Amount Input Case */
						return(LDT_SEQERR_ADR);
					} else {                                                /* W/o Amount Input Case */
						MaintCouponDisplaySecondPage();                        /* display 2nd page */
					}
					UieReject();                                            /* Shift Current Func. to Active Func. */
					return(SUCCESS);

                default:
                    return(UifACDefProc(pKeyMsg));
            }

            /********************/
            /* Call Coupon edit */
            /********************/
            if(sError != SUCCESS)   break;

            if(sEditflg == 1){
              CPNEditData.uchMajorClass = CLASS_MAINTCPN;
              CPNEditData.usACNumber = AC_CPN_MAINT;
              sError = MaintCPNEdit(&CPNEditData);

              /********************************/
              /* Branch by result of CPN edit */
              /********************************/
              switch(sError){
                   case MAINT_CPN_MNEMO_ADR :      /* Next is Mnemonic  */
                       UieNextMenu(aChildAC161EnterData2);
                       sError = SUCCESS;
                       break;

                   case MAINT_CPN_ADJ_ADR :         /* Next is Adjective*/
                       UieNextMenu(aChildAC161Adjective);
                       sError = SUCCESS;
                       break;

                  case SUCCESS :
                       UieReject();      /* Current Func to Active Func */
                       break;
              }
              MaintCPNPLUModifier(CLASS_PARAPLU_RESET_EV); /* reset E-Version */
            }
            break;
            
        case UIM_CANCEL:
            MaintCPNPLUModifier(CLASS_PARAPLU_RESET_EV); /* set E-Version */
        

        default :
            sError = UifACDefProc(pKeyMsg);           /* Default Procedure  */
            break;
    }

    return(sError);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC161EnterALPHA(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.161 Enter Alphabet Mode.
*==============================================================================
*/
SHORT UifAC161EnterALPHA(KEYMSG *pKeyMsg)
{
    SHORT        sError = SUCCESS;
	MAINTCPN    CPNEditData = { 0 };

    /*****************************/
    /* Branch by kind of kessage */
    /*****************************/
    switch(pKeyMsg->uchMsg){
        case UIM_INIT :
            /*UieOpenSequence(aszSeqACFSC_P1);*/
            UieOpenSequence(aszSeqAC161EnterALPHA);

            /********************************************/
            /* Change Key-Board mode to Mnemonic(ALPHA) */
            /********************************************/
            UieEchoBack(UIE_ECHO_ROW1_AN,PARA_CPN_LEN); /* Define Disp Form */
            UifACChgKBType(UIF_ALPHA_KB);                /* KB Mode : ALPHA */
            break;

        case UIM_INPUT :
            /***********************/
            /* Branch by input key */
            /***********************/
            switch(pKeyMsg->SEL.INPUT.uchMajor){
                case FSC_P1 :
                    CPNEditData.uchStatus = 0;           /* Init Status     */

                    if(pKeyMsg->SEL.INPUT.uchLen == 0){ /* without any keys */
                        CPNEditData.uchStatus |= MAINT_WITHOUT_DATA;
                    }
                    else{                                /* with some keys  */
                        _tcsncpy(CPNEditData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, PARA_LEADTHRU_LEN);
                    }

                    /**********************************************/
                    /* Set each parameters to write CPN Mnemonics */
                    /**********************************************/
                    CPNEditData.uchMajorClass = CLASS_MAINTCPN;
                    CPNEditData.uchMinorClass = CLASS_PARACPN_MAINT_WRITE;
                    CPNEditData.usACNumber = AC_CPN_MAINT;

                    if((sError=MaintCPNEdit(&CPNEditData)) != SUCCESS){
                        break;
                    }

                    /************************************/
                    /* Change Key-Board mode to Numeric */
                    /************************************/
                    UifACChgKBType(UIF_NEUME_KB);
                    UieEchoBack(UIE_ECHO_ROW0_NUMBER,UIF_DIGIT10);
                    UieAccept();
                    break;

                default:
                    return(UifACDefProc(pKeyMsg));

                /*default:break;*/
            }
            break;

        default:
            sError = UifACDefProc(pKeyMsg);         /* Default Procedure   */
            break;
    }

    return(sError);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC161EnterAdjective(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.161 Enter Adjective code Mode.
*==============================================================================
*/
SHORT UifAC161Adjective(KEYMSG *pKeyMsg) 
{
    SHORT       sError = SUCCESS;
	MAINTCPN    CPNEditData = { 0 };

    /*****************************/
    /* Branch by kind of kessage */
    /*****************************/
    switch(pKeyMsg->uchMsg){
        case UIM_INIT :
            UieOpenSequence(aszSeqAC161Adjective);
            break;

        case UIM_INPUT :
            /***********************/
            /* Branch by input key */
            /***********************/
            switch(pKeyMsg->SEL.INPUT.uchMajor){
                case FSC_NUM_TYPE :
                    sError = LDT_SEQERR_ADR;
                    /*
                     *if(pKeyMsg->SEL.INPUT.uchLen > 0){
                     *    sError = LDT_KEYOVER_ADR;
                     *    break;
                     *}
                     */
                    /****************************************/
                    /* Set each parameters to edit CPN data */
                    /****************************************/
                    /*CPNEditData.uchMajorClass = CLASS_MAINTCPN;
                     *CPNEditData.uchMinorClass = CLASS_PARACPN_MAINT_READ;
                     *CPNEditData.usACNumber    = AC_CPN_MAINT;
                     *CPNEditData.uchStatus    |= MAINT_WITHOUT_DATA;
                     *if((sError=MaintCPNEdit(&CPNEditData)) != SUCCESS){
                     *    break;
                     *}
                     *UieAccept();
                     */
                    break;

                case FSC_AC :
                    if(pKeyMsg->SEL.INPUT.uchLen > UIF_DIGIT1){
                        sError = LDT_KEYOVER_ADR;
                        break;
                    }

                    CPNEditData.uchStatus = 0;          /* Init Status      */
                    if(pKeyMsg->SEL.INPUT.uchLen == 0){ /* without any keys */
                        CPNEditData.uchStatus |= MAINT_WITHOUT_DATA;
                    }
                    else{                                /* with some keys  */
                        CPNEditData.ulInputData = (ULONG)pKeyMsg->SEL.INPUT.lData;
                        _tcsncpy(CPNEditData.aszMnemonics, pKeyMsg->SEL.INPUT.aszKey, PARA_LEADTHRU_LEN);
                    }

                    /**********************************************/
                    /* Set each parameters to write CPN Adjective */
                    /**********************************************/
                    CPNEditData.uchMajorClass = CLASS_MAINTCPN;
                    CPNEditData.uchMinorClass = CLASS_PARACPN_MAINT_WRITE;
                    CPNEditData.usACNumber    = AC_CPN_MAINT;

                    if((sError=MaintCPNEdit(&CPNEditData)) != SUCCESS){
                        break;
                    }

                    UieAccept();
                    break;


                default:
                    return(UifACDefProc(pKeyMsg));
            }
            break;

        default:
            sError = UifACDefProc(pKeyMsg);         /* Default Procedure   */
            break;
    }

    return(sError);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC161ExitFunction(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.161 Enter Exit Function Mode.
*==============================================================================
*/
SHORT UifAC161ExitFunction(KEYMSG *pKeyMsg)
{
    SHORT        sError = SUCCESS;

    /*****************************/
    /* Branch by kind of message */
    /*****************************/
    switch(pKeyMsg->uchMsg){
        case UIM_INIT :
            UieOpenSequence(aszSeqACFSC_AC);
            break;

        case UIM_INPUT :
            /***********************/
            /* Branch by input key */
            /***********************/
            switch(pKeyMsg->SEL.INPUT.uchMajor){
                case FSC_AC :
                    if(pKeyMsg->SEL.INPUT.uchLen){    /* with something key */
                        sError = LDT_SEQERR_ADR;
                        break;
                    }

                    /*****************/
                    /* Finalize proc */
                    /*****************/
                    MaintCPNUnLock();                   /* UnLock CPN File */
                    MaintFin(CLASS_MAINTTRAILER_PRTSUP);/* Finalize Proc   */
                    UieExit(aACEnter);                  /* Ret UifACEnter  */
                    break;
                default:
                    return(UifACDefProc(pKeyMsg));
            }
            break;

        default :
            sError = UifACDefProc(pKeyMsg);            /* Default Procedure */
            break;
    }

    return(sError);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC161CancelFunction(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.161 Enter Cancel Function Mode.
*==============================================================================
*/
SHORT UifAC161CancelFunction(KEYMSG *pKeyMsg)
{
    SHORT        sError = SUCCESS;

    /*****************************/
    /* Branch by kind of message */
    /*****************************/
    switch(pKeyMsg->uchMsg){
        case UIM_INIT :
            UieOpenSequence(aszSeqACFSC_CANCEL);
            break;

        case UIM_INPUT :
            /***********************/
            /* Branch by input key */
            /***********************/
            switch(pKeyMsg->SEL.INPUT.uchMajor){
                case FSC_CANCEL :
                    if(pKeyMsg->SEL.INPUT.uchLen){        /* with some key */
                        sError = LDT_SEQERR_ADR;
                        break;
                    }

                    /******************************/
                    /* Execute Finalize Procedure */
                    /******************************/
                    MaintCPNUnLock();                  /* UnLock CPN File  */
                    MaintFin(CLASS_MAINTTRAILER_PRTSUP);/* Finalize Proc   */
                    UieExit(aACEnter);              /* Return UifACEnter() */
                    break;
                default:
                    return(UifACDefProc(pKeyMsg));
            }
            break;

        default :
            sError = UifACDefProc(pKeyMsg);           /* Default Procedure */
            break;
    }

    return(sError);
}

/*
*=============================================================================
**  Synopsis: SHORT UifAC161ErrorFunction(KEYMSG *pKeyMsg) 
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key Message  
*      Output:  nothing                         
**  Return:     SUCCESS         : Successful
*               LDT_KEYOVER_ADR : Wrong Data Error
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: A/C No.161 Enter Error Function Mode.
*==============================================================================
*/
SHORT UifAC161ErrorFunction(KEYMSG *pKeyMsg)
{
    SHORT        sError = SUCCESS;

    /*****************************/
    /* Branch by kind of message */
    /*****************************/
    switch(pKeyMsg->uchMsg){
        case UIM_INIT :
            UieOpenSequence(aszSeqAC161Error);
            break;

        case UIM_INPUT :
            /***********************/
            /* Branch by input key */
            /***********************/
            switch(pKeyMsg->SEL.INPUT.uchMajor){
                /*
                case FSC_AC :
                    sError = LDT_SEQERR_ADR;
                    break;
                */
                case FSC_CANCEL :
                    if(pKeyMsg->SEL.INPUT.uchLen){    /* with some keys    */
                        sError = LDT_SEQERR_ADR;
                        break;
                    }

                    /******************************/
                    /* Execute Finalize Procedure */
                    /******************************/
                    MaintCPNUnLock();                 /* UnLock CPN File   */
                    MaintFin(CLASS_MAINTTRAILER_PRTSUP);/* Finalize Proc   */
                    UieExit(aACEnter);              /* Return UifACEnter() */
                    break;
                default:
                    return(UifACDefProc(pKeyMsg));
            }
            break;

        default :
            sError = UifACDefProc(pKeyMsg);           /* Default Procedure */
            break;
    }

    return(sError);
}
/*==========================================================================*/
/*                            P R O G R A M    E N D                        */
/*==========================================================================*/
