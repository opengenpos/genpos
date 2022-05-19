/*
************************************************************************************
**                                                                                **
**        *=*=*=*=*=*=*=*=*                                                       **
**        *  NCR 2170     *             NCR Corporation, E&M OISO                 **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992                    **
**    <|\/~               ~\/|>                                                   **
**   _/^\_                 _/^\_                                                  **
**                                                                                **
************************************************************************************
*===================================================================================
* Title       : Display Module                         
* Category    : Display, NCR 2170 US Hospitality Application         
* Program Name: DSPDSCO2.C
* ----------------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               DispRegOpDscrCntr() : control descriptor ON/OFF
*               DispRegSetOpStat() : set current descriptor status
* ----------------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-09-92: 00.00.01 :  R.Itoh   : initial                                   
*          :          :                                    
*===================================================================================
*===================================================================================
* PVCS Entry
* ----------------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===================================================================================
*/
#include	<tchar.h>

#include    <ecr.h>
#include    <uie.h>
#include    <paraequ.h>
#include    <para.h>
#include    <plu.h>
#include	<pararam.h>
#include    <maint.h>
#include    <display.h>
#include    "disp20.h"
#include	"csstbpar.h"
#include	"csstbstb.h"

/*
*===========================================================================
*
*             A Brief Explanation of the Descriptors
*
*  Most calls into the display logic using DispWrite() or similar functions will also
*  refresh the descriptor window and the state indicators displayed there. These state
*  indicators are derived from the NCR 2170 2X20 LCD display which had two lines of text
*  and a set of LEDs that displayed various state information through being on or off or
*  blinking. That functionality was replicated with first NHPOS on the NCR 7448 and
*  now GenPOS on standard terminal displays with a line of text with acronyms for the
*  various states. The displayed text has several possible views:
*    - not shown       -> LED is off
*    - shown as gray   -> LED is blinking
*    - show as red     -> LED is on
*
*===========================================================================
*/


/*
*===========================================================================
** Synopsis:    ULONG DispRegSetOpStat(VOID);      
*     Input:    nothing    
*     Output:   nothing
*     InOut:    nothing
*
** Return:      USHORT usDispRegOpCurrStat        
*
** Description: This function sets current descriptor status to the buffer.  
*===========================================================================
*/
static ULONG DispRegSetOpStat(VOID)       
{   
    ULONG  ulDispRegOpCurrStat = 0;

    if (flDispRegDescrControl & RECEIPT_20_CONTROL) {                 
        ulDispRegOpCurrStat |= RECEIPT_20_STATUS;       
    }		   		  			/* Status Flag for 'RECEIPT' On */

    if (flDispRegDescrControl & PRT_OFFLINE_20_CONTROL) {                 
        ulDispRegOpCurrStat |= PRT_OFFLINE_20_STATUS;       
    }		   		 	    	/* Status Flag for 'TAKE TO KITCHEN' On */

    if (flDispRegDescrControl & VOID_DELETE_20_CONTROL) {                 
        ulDispRegOpCurrStat |= DELETE_20_STATUS;       
    }		   				    /* Status Flag for 'VOID/DELETE' On */

    if (flDispRegDescrControl & T_VOID_20_CONTROL) {                 
        ulDispRegOpCurrStat |= T_VOID_20_STATUS;       
                                /* Status Flag for 'T-VOID' On */
    }	    	   				  	  	

    if (flDispRegDescrControl & MINUS_20_CONTROL) {                 
        ulDispRegOpCurrStat |= MINUS_20_STATUS;       
    }		  			        /* Status Flag for '%/(-)' On */

    if (flDispRegDescrControl & CRED_ALPHA_20_CONTROL) {                 
        ulDispRegOpCurrStat |= CRED_ALPHA_20_STATUS;       
    }		  				    /* Status Flag for 'CRED/ALPHA' On */

    if (flDispRegDescrControl & TXBLE_TRANS_20_CONTROL) {                 
        ulDispRegOpCurrStat |= TXBLE_TRANS_20_STATUS;       
    }	   				  	    /* Status Flag for 'TXBLE/TRANS' On */

    if (flDispRegDescrControl & TOTAL_B_DUE_20_CONTROL) {                 
        ulDispRegOpCurrStat |= TOTAL_B_DUE_20_STATUS;       
    }		 	           	    /* Status Flag for 'TOTAL/B.DUE' On */

    if (flDispRegDescrControl & CHANGE_20_CONTROL) {                 
        ulDispRegOpCurrStat |= CHANGE_20_STATUS;       
    }		 				   	/* Status Flag for 'CHANGE' On */

    if (flDispRegDescrControl & NEW_STORE_20_CONTROL) {                 
        ulDispRegOpCurrStat |= NEW_STORE_20_STATUS;       
    } 						  	/* Status Flag for 'NEW/STORE' On */

    if (flDispRegKeepControl & P_BALANCE_20_CONTROL) {                 
        ulDispRegOpCurrStat |= P_BALANCE_20_STATUS;       
    } 							/* Status Flag for 'P.BALANCE' On */

    if (flDispRegDescrControl & PAYMENT_RECALL_20_CONTROL) {                 
        ulDispRegOpCurrStat |= PAYMENT_RECALL_20_STATUS;       
    } 							/* Status Flag for 'PAYMENT/RECALL' On */

    if (flDispRegDescrControl & TRAIN_20_CONTROL) {                 
        ulDispRegOpCurrStat |= TRAIN_20_STATUS;       
    }						    /* Status Flag for 'TRAIN' On */

    if (flDispRegDescrControl & COMM_20_CONTROL) {                 
        ulDispRegOpCurrStat |= COMM_20_STATUS;       
    }						    /* Status Flag for 'TRAIN' On */
	if( flDispRegDescrControl & KDS_20_CONTROL) {
		ulDispRegOpCurrStat |= KDS_20_STATUS;
	}
	if( flDispRegDescrControl & DB_20_CONTROL) {	//Delayed Balance JHHJ
		ulDispRegOpCurrStat |= DB_20_STATUS;
	}

    return(ulDispRegOpCurrStat);
}																	



/*
*===========================================================================
** Synopsis:    VOID DispRegOpDscrCntr( REGDISPMSG *pData, UCHAR uchDscSave );     
*
*     Input:    *pData      - Pointer of the Display Data Buffer
*     Input:    uchDscSave   - check dscriptor must be saved or not
*     Output:   nothing
*     InOut:    nothing
*
** Return:      nothing
*
** Description:	This function compares current descriptor status with old    
*               status and turns the descriptors on/off according to the   
*               status change. These indicators are the ones displayed on
*               the Operator Display in the Descriptor Window of the layout
*               when in Register Mode.
*
*               See also DispRegCuDscrCntr() for the Customer Display.
*
*               NOTE:  See also function DispSupOpDscrCntr()
*
*===========================================================================
*/
VOID DispRegOpDscrCntr( REGDISPMSG *pData, UCHAR uchDscSave )   
{
    ULONG   ulCurrSave;                  /* current descriptor status */
    ULONG   ulWork;
    UCHAR   fbCheckData;

    if (pData->fbSaveControl & DISP_SAVE_ECHO_ONLY) {
        return;
    }
    if (uchDscSave == DSC_SAV) {
        ulCurrSave = ulDispRegSaveDsc;           /* use saved descriptor to redisplay */ 
    } else {
        ulCurrSave = DispRegSetOpStat();         /* usCurrSave = current descriptor status */
    }
    ulWork = ((ulCurrSave ^ ulDispRegOpBackStat) | (ulCurrSave ^ ulDispRegOpOldStat));    /* usWork = status change data */
    
    if (ulWork & RECEIPT_20_STATUS) {
        if (ulCurrSave & RECEIPT_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_RECEIPT, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_RECEIPT, UIE_DESC_OFF);
        }    
    }
    if (ulWork & DELETE_20_STATUS) {
        if (ulCurrSave & DELETE_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_VOID_DELETE, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_VOID_DELETE, UIE_DESC_OFF);
        }
    }
    if (ulWork & T_VOID_20_STATUS) {
        if (ulCurrSave & T_VOID_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_T_VOID, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_T_VOID, UIE_DESC_OFF);
        }
    }
    if (ulWork & MINUS_20_STATUS) {
        if (ulCurrSave & MINUS_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_MINUS, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_MINUS, UIE_DESC_OFF);
        }
    }
    if (ulWork & CRED_ALPHA_20_STATUS) {
        if (ulCurrSave & CRED_ALPHA_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_CRED_ALPHA, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_CRED_ALPHA, UIE_DESC_OFF);
        }
    }
    if (ulWork & TXBLE_TRANS_20_STATUS) {
        if (ulCurrSave & TXBLE_TRANS_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_TXBLE_TRANS, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_TXBLE_TRANS, UIE_DESC_OFF);
        }
    }
    if (ulWork & TOTAL_B_DUE_20_STATUS) {
        if (ulCurrSave & TOTAL_B_DUE_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_TOTAL_B_DUE, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_TOTAL_B_DUE, UIE_DESC_OFF);
        }
    }
    if (ulWork & CHANGE_20_STATUS) {
        if (ulCurrSave & CHANGE_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_CHANGE, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_CHANGE, UIE_DESC_OFF);
        }
    }
    if (ulWork & NEW_STORE_20_STATUS) {
        if (ulCurrSave & NEW_STORE_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_NEW_STORE, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_NEW_STORE, UIE_DESC_OFF);
        }
    }
    if (ulWork & P_BALANCE_20_STATUS) {
        if (ulCurrSave & P_BALANCE_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_P_BALANCE, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_P_BALANCE, UIE_DESC_OFF);
        }
    }
    if (ulWork & PAYMENT_RECALL_20_STATUS) {
        if (ulCurrSave & PAYMENT_RECALL_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_PAYMENT_RECALL, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_PAYMENT_RECALL, UIE_DESC_OFF);
        }
    }
    if (ulWork & TRAIN_20_STATUS) {
        if (ulCurrSave & TRAIN_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_TRAIN, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_TRAIN, UIE_DESC_OFF);
        }
    }
    if (ulWork & COMM_20_STATUS) {
        if (ulCurrSave & COMM_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_COMM, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_COMM, UIE_DESC_OFF);
        }
    }
	//SR 236, Added a descriptor to be  shown whenever the KDS system goes down
	//this will stay on until the KDS system has been turned back on JHHJ
	if (ulWork & KDS_20_STATUS) {
        if (ulCurrSave & KDS_20_STATUS) {
            UieDescriptor(UIE_OPER, _2x20_KDS, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2x20_KDS, UIE_DESC_OFF);
        }
    }
	//Delayed Balance Added a descriptor to be  shown whenever the terminal goes
	//into delay balance mode. JHHJ
	if (uchMaintDelayBalanceFlag) {
		UieDescriptor(UIE_OPER, _2x20_DELAY_BALANCE, UIE_DESC_ON);
	}else
	{
		UieDescriptor(UIE_OPER, _2x20_DELAY_BALANCE, UIE_DESC_OFF);
	}

	// Store and Forward indicator
	//Gets flag from Para.uchStoreForwardFlag
	{
		UCHAR  myStoreAndForwardStatus = ParaStoreForwardFlagAndStatus();

		if ((myStoreAndForwardStatus & PIF_STORE_AND_FORWARD_ENABLED) == PIF_STORE_AND_FORWARD_DISABLED) {
			UieDescriptor(UIE_OPER, _2X20_SF, UIE_DESC_OFF);
		} else if ((myStoreAndForwardStatus & (PIF_STORE_AND_FORWARD_ONE_ON | PIF_STORE_AND_FORWARD_TWO_ON)) == 0) {
			UieDescriptor(UIE_OPER, _2X20_SF, UIE_DESC_BLINK);
		} else if ((myStoreAndForwardStatus & (PIF_STORE_AND_FORWARD_ONE_ON | PIF_STORE_AND_FORWARD_TWO_ON)) != 0) {
			UieDescriptor(UIE_OPER, _2X20_SF, UIE_DESC_ON);
		}

		if ((myStoreAndForwardStatus & (PIF_STORE_AND_FORWARD_ONE_FORWARDING | PIF_STORE_AND_FORWARD_TWO_FORWARDING)) == 0) {
			UieDescriptor(UIE_OPER, _2X20_SF_TRANSFERRING, UIE_DESC_OFF);
		} else {
			UieDescriptor(UIE_OPER, _2X20_SF_TRANSFERRING, UIE_DESC_ON);
		}
	}

	if (CstIamDisconnectedUnjoinedSatellite() == STUB_SUCCESS) {
		UieDescriptor(UIE_OPER, _2X20_DISCON_SAT, UIE_DESC_ON);
	}
	else {
		UieDescriptor(UIE_OPER, _2X20_DISCON_SAT, UIE_DESC_OFF);
	}

	if (ulWork & PRT_OFFLINE_20_STATUS) {
        if (ulCurrSave & PRT_OFFLINE_20_STATUS) {
            UieDescriptor(UIE_OPER, _2X20_PRT_OFFLINE, UIE_DESC_ON);
        } else {
            UieDescriptor(UIE_OPER, _2X20_PRT_OFFLINE, UIE_DESC_OFF);
        }
    }
    ulDispRegOpOldStat = ulCurrSave;   /* save current descriptor status to compare with next descriptor status */   
                                        
    fbCheckData = (UCHAR)(pData->fbSaveControl & DISP_SAVE_TYPE_MASK);    
    if (fbCheckData == DISP_SAVE_TYPE_1) { 
        ulDispRegSaveDsc = ulCurrSave;
    }
}    

