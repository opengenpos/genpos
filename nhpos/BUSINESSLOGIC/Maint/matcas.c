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
* Title       : Cashier No. Assignment Module (A/C 20)                      
* Category    : Maintenance, NCR 2170 Int'l Hospitality Application Program 
* Program Name: MATCAS.C
* --------------------------------------------------------------------------
* Abstract: This function controls cashier parameter file for A/C 20.
*
*           The provided function names are as follows: 
* 
*             MaintCashierRead()    : read & display parameter from cashier file 
*             MaintCashierEdit()    : check data & set data in self-work area 
*             MaintCashierWrite()   : write cashier parameter record in cashier file & print 
*             MaintCashierDelete()  : delete cashier parameter record
*             MaintCashierReport()  : print all cashier parameter report   
*             MaintSetCasDispData() : set data for display 
*  
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Dec-24-92: 00.00.01 : K.You     : initial                                   
* Aug-18-98: 03.03.00 : H.Iwata   :                                    
*** GenPOS
* Oct-03-17 : 02.02.02 : R.Chambers   : replaced cloned code with single function CheckReturnLvalue()
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
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ecr.h"
#include "pif.h"
#include "uie.h"
#include "plu.h"
#include "csstbfcc.h"
#include "paraequ.h" 
#include "para.h"
#include "pararam.h"
#include <cscas.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csttl.h>
#include <csop.h>
#include "csgcs.h"
#include <csstbttl.h>
#include <csstbpar.h>
#include <csstbopr.h>
#include <csstbcas.h>
#include <log.h>
#include <appllog.h>
#include <rfl.h>
#include <ej.h>
#include <mld.h>
#include	<csstbej.h>

#include "maintram.h"


/*
*===========================================================================
*   Work Area Declartion 
*===========================================================================
*/

static MAINTCASHIERIF  MaintCasNo;
            
static VOID  MaintSetCasDispData( UCHAR uchFieldAddr );        /* matcas.c */


static SHORT CheckReturnLvalue (TCHAR *aszString)
{
	SHORT  retVal = SUCCESS;          // Assume success and change if there is an error.

    for (; *aszString; aszString++)
	{
		//  The following if statement contains the hard coded values for the
		//  decminal values of the ASCII characters 0-9.  If the inputed value 
		//  is not numerical, it will return a sequence error   ***PDINU
		if (*aszString >= '0' && *aszString <= '9') {
			continue;
		}
		retVal = (LDT_KEYOVER_ADR);
		break;
	}

	return retVal;
}


/*
*===========================================================================
** Synopsis:    SHORT MaintCashierRead( MAINTCASHIERIF *pData )
*               
*     Input:    *pData          : pointer to structure for MAINTCASHIERIF
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_SEQERR_ADR  : sequence error
*
** Description: This function reads from Cashier File and saves them to own-work.
*===========================================================================
*/

SHORT MaintCashierRead( MAINTCASHIERIF *pData )
{
    SHORT             sError;
	CASIF             CasIf = {0};
    MAINTCASHIERIF    MldMaintCashierIf = {0};

    /* Initialize */
    memset(&MaintWork, 0, sizeof(MAINTWORK));
    memset(&MaintCasNo, 0, sizeof(MAINTCASHIERIF));
    
    /* Check Cashier Number Range */
    /* Digits Already Checked by UI */
    if (pData->CashierIf.ulCashierNo == 0) {         /* Out of Range */
        return(LDT_KEYOVER_ADR);
    }

    MaintWork.MaintCashierIf.CashierIf.ulCashierNo = pData->CashierIf.ulCashierNo;
    MaintWork.MaintCashierIf.uchMajorClass = pData->uchMajorClass;

    CasIf.ulCashierNo = pData->CashierIf.ulCashierNo;
    sError = SerCasIndRead(&CasIf);
    if ((sError != CAS_PERFORM) && (sError != CAS_NOT_IN_FILE)) {
        return(CasConvertError(sError));
    }

    /* Save Old Cashier Parameter and Print Them */
    if (sError == CAS_PERFORM) {
        MaintWork.MaintCashierIf.CashierIf = CasIf;
        MaintCasNo.CashierIf = CasIf;
    
        /* Set Journal Bit & Receipt Bit */
        MaintWork.MaintCashierIf.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

        /* Check Operational Times */
        if ((uchMaintOpeCo & MAINT_ALREADYOPERATE)) {               /* In Case of after 1st Operation */
            /* 1 Line Feed */
            MaintFeed();
        }

        MaintHeaderCtl(AC_CASH_ASSIN, RPT_ACT_ADR);

        /* Print Cashier Parameter */
        PrtPrintItem(NULL, &(MaintWork.MaintCashierIf));

        /* send print data to shared module */
        PrtShrPartialSend(MaintWork.MaintCashierIf.usPrintControl);
    }


    /* 980901 H.Iwata */
    /******************/
    /* Display to MLD */
    /******************/
    MldMaintCashierIf = MaintWork.MaintCashierIf;
    MldMaintCashierIf.uchFieldAddr = 0;
    MldDispItem(&MldMaintCashierIf,0);

    /* Display Address 1 Data */
    MaintCasNo.uchMajorClass = CLASS_PARACASHIERNO;
    MaintCasNo.uchMinorClass = CLASS_PARACASHIERNO_STS;
    MaintCasNo.uchFieldAddr = CAS_FIELD1_ADR;
    MaintCasNo.usACNumber = AC_CASH_ASSIN;
    MaintCasNo.CashierIf.ulCashierNo = pData->CashierIf.ulCashierNo;

    MaintSetCasDispData(CAS_FIELD1_ADR);
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintCashierEdit( MAINTCASHIERIF *pData )
*               
*     Input:    *pData               : pointer to structure for HIERMAINTCASHIERIF
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS              : successful
*               LDT_SEQERR_ADR       : sequence error
*               LDT_KEYOVER_ADR      : wrong data
*               MAINT_CAS_MNEMO_ADR  : set mnemonics
*               MAINT_CAS_STSGC_ADR  : set start GC No.
*
** Description: This function checks input-data from UI and sets in self-work area.
*===========================================================================
*/

SHORT MaintCashierEdit( MAINTCASHIERIF *pData )
{
    /* Check Status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* Without Data */
        if(MaintWork.MaintCashierIf.CashierIf.usGstCheckEndNo < MaintWork.MaintCashierIf.CashierIf.usGstCheckStartNo) {
            return(LDT_SEQERR_ADR);
        }

        /* Check Minor Class */
        switch(pData->uchMinorClass) {    
        case CLASS_PARACASHIERNO_READ:
            MaintCasNo.uchFieldAddr++;

            /* Check Address */
            if (MaintCasNo.uchFieldAddr > CAS_FIELD_MAX) {
                MaintCasNo.uchFieldAddr = CAS_FIELD1_ADR;       /* Initialize Address */
            }

            /* Set Data for Display Next Old Data */
            MaintSetCasDispData(MaintCasNo.uchFieldAddr);
            break;

        case CLASS_PARACASHIERNO_WRITE:
            return(LDT_SEQERR_ADR);                             /* Sequence Error */

        default:
            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA);
        }
    } else {                                                    /* With Data */
		CASIF           CasIf = {0};
		MAINTCASHIERIF  MldMaintCashierIf = {0};
		SHORT           sStatus;

        /* Check Minor Class */
        switch(pData->uchMinorClass) {    
        case CLASS_PARACASHIERNO_READ:
            if(MaintWork.MaintCashierIf.CashierIf.usGstCheckEndNo < MaintWork.MaintCashierIf.CashierIf.usGstCheckStartNo) {
                return(LDT_SEQERR_ADR);
            }

            /* Check Address */
            if((MaintCasNo.uchFieldAddr < CAS_FIELD1_ADR) || (CAS_FIELD_MAX < MaintCasNo.uchFieldAddr)) {
                return(LDT_KEYOVER_ADR);                    /* Wrong Data */
            }
            MaintCasNo.uchFieldAddr = pData->uchFieldAddr;

            /* Set Data for Display Old Data */
            MaintSetCasDispData(MaintCasNo.uchFieldAddr);
            break;

        case CLASS_PARACASHIERNO_WRITE:
            /* Check Address */
            switch(MaintCasNo.uchFieldAddr) {
            case CAS_FIELD1_ADR:
            case CAS_FIELD3_ADR:
			case CAS_FIELD5_ADR:
                /* Check Cashier Status */
                if (MaintChkBinary(pData->aszMnemonics, 4) == LDT_KEYOVER_ADR) {
                    return(LDT_KEYOVER_ADR);      
                }

                /* Check Training Bit */
                if (MaintCasNo.uchFieldAddr == CAS_FIELD1_ADR) {
                    if ((MaintWork.MaintCashierIf.CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_0] & MAINT_TRAIN_BIT) ^ (MaintAtoStatus(pData->aszMnemonics) & MAINT_TRAIN_BIT)) {    /* Unmatch Training Bit */
                        CasIf.ulCashierNo = MaintCasNo.CashierIf.ulCashierNo;
/*
                        if (SerCasIndRead(&CasIf) == CAS_PERFORM) {                 / Cashier Already Exsist /
                            return(LDT_PROHBT_ADR);
                        }    
*/
                    }
                }

                /* Convert String Data into Bit Pattern */
                MaintWork.MaintCashierIf.CashierIf.fbCashierStatus[(MaintCasNo.uchFieldAddr - 1) / 2] &= 0xF0;
                MaintWork.MaintCashierIf.CashierIf.fbCashierStatus[(MaintCasNo.uchFieldAddr - 1) / 2] |= MaintAtoStatus(pData->aszMnemonics);
                break;                                     
    
            case CAS_FIELD2_ADR:
            case CAS_FIELD4_ADR:
                /* Check Cashier Status */
                if (MaintChkBinary(pData->aszMnemonics, 4) == LDT_KEYOVER_ADR) {
                    return(LDT_KEYOVER_ADR);      
                }

                /* Check Head Cashier Bit */
                if (MaintCasNo.uchFieldAddr == CAS_FIELD2_ADR) {
                    if (((MaintWork.MaintCashierIf.CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_0] >> 4) & MAINT_HEADCAS_BIT) 
                        ^ (MaintAtoStatus(pData->aszMnemonics) & MAINT_HEADCAS_BIT)) {  /* Unmatch Training Bit */
                        CasIf.ulCashierNo = MaintCasNo.CashierIf.ulCashierNo;              
/*
                        if (SerCasIndRead(&CasIf) == CAS_PERFORM) {                 / Cashier Already Exsist /
                            return(LDT_PROHBT_ADR);
                        }    
*/
                    }
                }

                /* Convert String Data into Bit Pattern */
                MaintWork.MaintCashierIf.CashierIf.fbCashierStatus[(MaintCasNo.uchFieldAddr - 1) / 2] &= 0x0F;
                MaintWork.MaintCashierIf.CashierIf.fbCashierStatus[(MaintCasNo.uchFieldAddr - 1) / 2] |= MaintAtoStatus(pData->aszMnemonics) << 4;
                break;                                     

    	      case CAS_FIELD6_ADR:
				/* Check Cashier Status */
				  if ((sStatus = CheckReturnLvalue (pData->aszMnemonics)) != SUCCESS) {
					  return sStatus;
				  }

                /* Check Guest Check Number Start */
                if( pData->CashierIf.usGstCheckStartNo > GCF_MAX_CHK_NUMBER) {
                    return(LDT_KEYOVER_ADR);
		          } else if (( MaintWork.MaintCashierIf.CashierIf.usGstCheckStartNo == 0 )
                                     && ( MaintWork.MaintCashierIf.CashierIf.usGstCheckEndNo == 0 )) {
                } else if ( pData->CashierIf.usGstCheckStartNo > MaintWork.MaintCashierIf.CashierIf.usGstCheckEndNo) {
                    return(LDT_KEYOVER_ADR);
                }
		        MaintWork.MaintCashierIf.CashierIf.usGstCheckStartNo = pData->CashierIf.usGstCheckStartNo;
                MaintWork.MaintCashierIf.uchMinorClass = pData->uchMinorClass;
                break;

	       case CAS_FIELD7_ADR:
			   /* Check Cashier Status */
				  if ((sStatus = CheckReturnLvalue (pData->aszMnemonics)) != SUCCESS) {
					  return sStatus;
				  }

				 /* Check Guest Check Number End */
                if(pData->CashierIf.usGstCheckStartNo > GCF_MAX_CHK_NUMBER) {
		              return(LDT_KEYOVER_ADR);
		        }
                if(pData->CashierIf.usGstCheckStartNo < MaintWork.MaintCashierIf.CashierIf.usGstCheckStartNo) {
                    return(LDT_KEYOVER_ADR);
                }

                MaintWork.MaintCashierIf.CashierIf.usGstCheckEndNo = pData->CashierIf.usGstCheckStartNo;
                MaintWork.MaintCashierIf.uchMinorClass = pData->uchMinorClass;
                break;

	       case CAS_FIELD8_ADR:
				/* Check Cashier Status */
				if ((sStatus = CheckReturnLvalue (pData->aszMnemonics)) != SUCCESS) {
					return sStatus;
				}

				/* Check Charge Tip Rate */
              if(pData->CashierIf.usGstCheckStartNo > MAINT_CHGTIP_MAX) {
                  return(LDT_KEYOVER_ADR);
              }
              MaintWork.MaintCashierIf.CashierIf.uchChgTipRate = (UCHAR)pData->CashierIf.usGstCheckStartNo;
              break;

	       case CAS_FIELD9_ADR:
				/* Check Cashier Status */
				if ((sStatus = CheckReturnLvalue (pData->aszMnemonics)) != SUCCESS) {
				  return sStatus;
				}

              /* Check Team No */
			  if(pData->CashierIf.usGstCheckStartNo > MAINT_TEAMNO_MAX) {
				  return(LDT_KEYOVER_ADR);
			  }

              MaintWork.MaintCashierIf.CashierIf.uchTeamNo = (UCHAR)pData->CashierIf.usGstCheckStartNo;
              break;

	       case CAS_FIELD10_ADR:
			   /* Check Cashier Status */
				if ((sStatus = CheckReturnLvalue (pData->aszMnemonics)) != SUCCESS) {
					return sStatus;
				}

				/* Check Allowed Terminal No */
				if(pData->CashierIf.usGstCheckStartNo > STD_NUM_OF_TERMINALS) {
					 return(LDT_KEYOVER_ADR);
				}
				MaintWork.MaintCashierIf.CashierIf.uchTerminal = (UCHAR)pData->CashierIf.usGstCheckStartNo;
				break;

    
        case CAS_FIELD11_ADR:
			memcpy(MaintWork.MaintCashierIf.CashierIf.auchCashierName, pData->CashierIf.auchCashierName, PARA_CASHIER_LEN*sizeof(TCHAR)); //SR288
			break;

        default:                                       
            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA);
      }                                              

      /* Set Data for Display Next Old Data */       
      MaintCasNo.uchFieldAddr++;

      /* Check Address */
      if (MaintCasNo.uchFieldAddr > CAS_FIELD_MAX) {
          MaintCasNo.uchFieldAddr = CAS_FIELD1_ADR;            /* Initialize Address */
      }
  
      /* 980901 H.Iwata */
      /******************/
      /* Display to MLD */
      /******************/
      MldMaintCashierIf = MaintWork.MaintCashierIf;
      MldMaintCashierIf.uchFieldAddr = 0;
      MldDispItem(&MldMaintCashierIf, 0);

      /* Display Next Old Data */
      MaintSetCasDispData(MaintCasNo.uchFieldAddr);
      break;
/*
      default:
          PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA);
*/
      }
  }

      /* Check Address */
      if (MaintCasNo.uchFieldAddr == CAS_FIELD11_ADR) {
          return(MAINT_CAS_MNEMO_ADR);                        /* Set Mnemonics */
      }
      return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintCashierWrite( VOID )                   
*               
*     Input:    Nothing                                        
*    Output:    Nothing 
*     InOut:    Nothing                                        
*
** Return:      SUCCESS   : successful                         
*               sReturn   : error 
*                                                              
** Description: This function writes & prints cashier parameter in self-work area.
*===========================================================================
*/
SHORT MaintCashierWrite( VOID )                                   
{
    SHORT       sReturn;
    SHORT       sError;
	USHORT      usTranAddress;
    CASIF       CasIf = {0};                                           
	MAINTTRANS  MaintTrans = {0};                                                     

    if(MaintWork.MaintCashierIf.CashierIf.usGstCheckEndNo < MaintWork.MaintCashierIf.CashierIf.usGstCheckStartNo) {
        return(LDT_SEQERR_ADR);
    }

    /* Dummy Read and Select Transaction Mnemonics */                                             
    CasIf.ulCashierNo = MaintWork.MaintCashierIf.CashierIf.ulCashierNo;
    sReturn = SerCasIndRead(&CasIf);
    
    if (sReturn == CAS_NOT_IN_FILE) {
        usTranAddress = TRN_ADD_ADR;            /* "ADDITION" */
    } else {
        usTranAddress = TRN_CHNG_ADR;           /* "CHANGE" */
    }

    /* Set Data for Writing Cashier Parameter */                                             
    /* Write Data */
    CasIf = MaintWork.MaintCashierIf.CashierIf;
    if ((sError = SerCasAssign(&CasIf)) != CAS_PERFORM) {
        /* Convert Error Status */
        return(CasConvertError(sError));
    }        

    if ((sReturn == CAS_NOT_IN_FILE) && (uchMaintOpeCo & MAINT_ALREADYOPERATE)) {
        /* 1 Line Feed */
        MaintFeed();
    }

    /* Make Header */
    MaintHeaderCtl(((UCHAR)(AC_CASH_ASSIN)),((UCHAR)(RPT_ACT_ADR)));                      

    /* Print "ADDITOIN" or "CHANGE" Mnemonics */
    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );
    RflGetTranMnem (MaintTrans.aszTransMnemo, usTranAddress);
    PrtPrintItem(NULL, &MaintTrans);

    /* Set Journal Bit & Receipt Bit */
    MaintWork.MaintCashierIf.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* Print Cashier Parameter */
    PrtPrintItem(NULL, &(MaintWork.MaintCashierIf));

    /* send print data to shared module */
    PrtShrPartialSend(MaintWork.MaintCashierIf.usPrintControl);

    return(SUCCESS);
}

/*
*======================================================================================
**   Synopsis:  SHORT MaintCashierDelete( MAINTCASHIERIF *pData )
*
*       Input:  *pKeyMsg        : Pointer to Structure for Key MAINTCASHIERIF  
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Cashier No. Delete Module  
*======================================================================================
*/
SHORT MaintCashierDelete( MAINTCASHIERIF *pData )
{
    SHORT           sError;
    CONST ULONG     ulCashierNo = pData->CashierIf.ulCashierNo;
	MAINTTRANS      MaintTrans = {0};
    TTLCASHIER      CastTtl = {0};

    /* Check Cashier Number Range */
    /* Digits Already Checked by UI */ 
    if (pData->CashierIf.ulCashierNo == 0) {      /* Out of Range */
        return(LDT_KEYOVER_ADR);
    }
    
    /* Lock Cashier File */
    sError = SerCasIndLock(ulCashierNo);
	if (sError < 0) return CasConvertError (sError);

    /* Check Daily,PTD Current Total File */
    CastTtl.uchMajorClass = CLASS_TTLCASHIER;
    CastTtl.uchMinorClass = CLASS_TTLCURDAY;
    CastTtl.ulCashierNumber = ulCashierNo;
    if ((sError = SerTtlTotalRead(&CastTtl)) == TTL_SUCCESS) {
        SerCasIndUnLock(ulCashierNo);
        return(LDT_TLCOACT_ADR);                                        /* Total Counter Active */      
    } else
	{
		PifLog(MODULE_CASHIER, LOG_ERROR_CASHIER_READ_CURDAY);
		PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sError));
	}

    CastTtl.uchMinorClass = CLASS_TTLCURPTD;
    if ((sError = SerTtlTotalRead(&CastTtl)) == TTL_SUCCESS) {
        SerCasIndUnLock(ulCashierNo);
        return(LDT_TLCOACT_ADR);                                        /* Total Counter Active */      
    } else
	{
		PifLog(MODULE_CASHIER, LOG_ERROR_CASHIER_READ_CURPTD);
		PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sError));
	}

    /* Check Daily,PTD Saved Total File */
    CastTtl.uchMinorClass = CLASS_TTLSAVDAY;
    CastTtl.ulCashierNumber = ulCashierNo;
    if ((sError = SerTtlTotalRead(&CastTtl)) == TTL_SUCCESS) {
        /* Check Daily Saved Reset Flag */
        if ((sError = SerTtlIssuedCheck(&CastTtl, TTL_NOTRESET)) == TTL_NOT_ISSUED) {   /* Reset Report Not Issued */
            SerCasIndUnLock(ulCashierNo);
            return(TtlConvertError(sError));                                    
        }
    } else
	{
		PifLog(MODULE_CASHIER, LOG_ERROR_CASHIER_READ_SAVDAY);
		PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sError));
	}

    CastTtl.uchMinorClass = CLASS_TTLSAVPTD;
    if ((sError = SerTtlTotalRead(&CastTtl)) == TTL_SUCCESS) {
        /* Check PTD Saved Reset Flag */
        if ((sError = SerTtlIssuedCheck(&CastTtl, TTL_NOTRESET)) == TTL_NOT_ISSUED) {   /* Reset Report Not Issued */
            SerCasIndUnLock(ulCashierNo);
            return(TtlConvertError(sError));                                    
        }
    } else
	{
		PifLog(MODULE_CASHIER, LOG_ERROR_CASHIER_READ_SAVPTD);
		PifLog(MODULE_ERROR_NO(MODULE_CASHIER), (USHORT)abs(sError));
	}

    /* Read Cashier Parameter */
	{
		CASIF    CasIf = {0};

		CasIf.ulCashierNo = ulCashierNo;                                 /* Set Cashier No. */

		sError = SerCasIndRead(&CasIf);
		if (sError != CAS_PERFORM) {
			char  xBuff[128];

			sprintf (xBuff, "ERROR: SerCasIndRead() returned %d for ulCashierNo %d", sError, ulCashierNo);
			NHPOS_ASSERT_TEXT ((sError == CAS_PERFORM), xBuff);
		}

		/* Save Cashier Parameter */
		MaintWork.MaintCashierIf.uchMajorClass = pData->uchMajorClass;            /* Save Major Class */
		MaintWork.MaintCashierIf.CashierIf = CasIf;
	}

    /* Check Operational Times */
    if ((uchMaintOpeCo & MAINT_ALREADYOPERATE)) {       /* in case of after 1st operation */
        MaintFeed();        /* 1 line feed */
    }                                                        

    /* Control Header Item */
    MaintHeaderCtl(((UCHAR)(AC_CASH_ASSIN)), ((UCHAR)(RPT_ACT_ADR)));                      

    /* Print Deleted Mnemonics */
    MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
    RflGetTranMnem (MaintTrans.aszTransMnemo, TRN_DEL_ADR);           /* Get Deleted Mnemonics "deleted" */
    MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );  

    PrtPrintItem(NULL, &MaintTrans);

    /* Print Cashier Parameter */
    MaintWork.MaintCashierIf.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.MaintCashierIf));

    /* send print data to shared module */
    PrtShrPartialSend(MaintWork.MaintCashierIf.usPrintControl);

    /* Delete Cashier Parameter and Saved Total */
    SerCasDelete(ulCashierNo);            /* Ignore Error Case */
    SerCasIndUnLock(ulCashierNo);         /* UnLock */

    return(SUCCESS);
}

/*
*======================================================================================
**   Synopsis:  SHORT MaintCashierReport( VOID )
*
*       Input:  Nothing         
*                               
*      Output:  Nothing
*
**  Return:     SUCCESS         : Successful 
*               LDT_SEQERR_ADR  : Sequence Error
*               
**  Description: Cashier No. Report Module  
*======================================================================================
*/
SHORT MaintCashierReport( VOID )
{

    UCHAR           uchFeedChk = 0;
    SHORT           sError;
    ULONG           ausCastAllIdBuff[MAX_NO_CASH];
    USHORT          usCastMax; 
    USHORT          i;

    /* Get All Cashier Id */
    if ((sError = SerCasAllIdRead(sizeof(ausCastAllIdBuff), ausCastAllIdBuff)) < 0) {   /* Error Case */
        return(CasConvertError(sError));
    }

    usCastMax = ( USHORT)sError;

    /* Control Header Item */
    MaintHeaderCtl(((UCHAR)(AC_CASH_ASSIN)), ((UCHAR)(RPT_ACT_ADR)));                      

    /* Initialize Buffer */
    memset(&MaintWork.MaintCashierIf.CashierIf.auchCashierName, '\0', PARA_CASHIER_LEN);

    /* Set Major Class */
    MaintWork.MaintCashierIf.uchMajorClass = CLASS_PARACASHIERNO;

    for (i = 0; i < usCastMax; i++) {
		CASIF           CasIf = {0};

        /* Check Abort Key */
        if (UieReadAbortKey() == UIE_ENABLE) {                          /* Abort Key Already Put */
            MaintMakeAbortKey();                                           
            break;
        }

        CasIf.ulCashierNo = ausCastAllIdBuff[i];                         /* Set Cashier No. */

        /* Read Cashier Parameter */
        if ((sError = SerCasIndRead(&CasIf)) != CAS_PERFORM) {          /* Error Case */
			MAINTERRORCODE  MaintErrorCode = {0};
			MAINTOPESTATUS  MaintOpeStatus = {0};

            /* Print Cashier No. */
            MaintOpeStatus.uchMajorClass = CLASS_MAINTOPESTATUS;
			MaintOpeStatus.uchMinorClass = 1;                       // indicate printing an Employee Id
            MaintOpeStatus.ulOperatorId = CasIf.ulCashierNo;
            MaintOpeStatus.usPrintControl = (PRT_JOURNAL | PRT_RECEIPT);   
            PrtPrintItem(NULL, &MaintOpeStatus);

            /* Print Error Code */
            MaintErrorCode.uchMajorClass = CLASS_MAINTERRORCODE;
            MaintErrorCode.sErrorCode = CasConvertError(sError);
            MaintErrorCode.usPrtControl = (PRT_JOURNAL | PRT_RECEIPT);   
            PrtPrintItem(NULL, &MaintErrorCode);
            continue;
        }

        /* Save Cashier Parameter */
        MaintWork.MaintCashierIf.CashierIf = CasIf;

        /* Print Cashier Parameter */
        MaintWork.MaintCashierIf.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

        if (uchFeedChk) {
            /* 1 Line Feed */
            MaintFeed();
        }

        PrtPrintItem(NULL, &(MaintWork.MaintCashierIf));
        uchFeedChk = 1;
    }

    /* Make Trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintSetCasDispData(UCHAR uchFieldAddr)
*               
*     Input:    uchFieldAddr : field address for display data
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function sets the user interface guide display with the
*               mnemonics appropriate for the specified field address in the
*               current Cashier Record being viewed or edited.
*===========================================================================
*/
static VOID MaintSetCasDispData( UCHAR uchFieldAddr )
{ 
    USHORT   usLeadAddress = 0;   // set to zero which gets replaced with a leadthru address where needed

    /* Initialize */
    memset(MaintCasNo.aszMnemonics, 0, sizeof(MaintCasNo.aszMnemonics));

    /* Distinguish Field Address and Set Data for Display */
    switch(uchFieldAddr) {

    case CAS_FIELD1_ADR:  /* Status 1 */
    case CAS_FIELD3_ADR:  /* Status 3 */
	case CAS_FIELD5_ADR:  /* Status 5 */
        MaintCasNo.uchMinorClass = CLASS_PARACASHIERNO_STS;
        MaintCasNo.CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_0] = ( UCHAR)(MaintWork.MaintCashierIf.CashierIf.fbCashierStatus[(uchFieldAddr - 1) / 2] & 0x0F);
        usLeadAddress = LDT_STATUS_ADR;               /* Request Status Entry */
        break;

    case CAS_FIELD2_ADR:  /* Status 2 */
    case CAS_FIELD4_ADR:  /* Status 4 */
        MaintCasNo.uchMinorClass = CLASS_PARACASHIERNO_STS;
        MaintCasNo.CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_0] = (UCHAR)(MaintWork.MaintCashierIf.CashierIf.fbCashierStatus[(uchFieldAddr - 1) / 2] & 0xF0);
        MaintCasNo.CashierIf.fbCashierStatus[CAS_CASHIERSTATUS_0] >>= 4;
        usLeadAddress = LDT_STATUS_ADR;               /* Request Status Entry */
        break;

    case CAS_FIELD6_ADR: /* Guest Check Start Number */
        MaintCasNo.uchMinorClass = CLASS_PARACASHIERNO_DATA6;
        MaintCasNo.CashierIf.usGstCheckStartNo =  MaintWork.MaintCashierIf.CashierIf.usGstCheckStartNo;
        usLeadAddress = LDT_DATA_ADR;                 /* Request Status Entry */
        break;                                                      

    case CAS_FIELD7_ADR: /* Guest Check End Number */
        MaintCasNo.uchMinorClass = CLASS_PARACASHIERNO_DATA7;
        MaintCasNo.CashierIf.usGstCheckStartNo = MaintWork.MaintCashierIf.CashierIf.usGstCheckStartNo;
        MaintCasNo.CashierIf.usGstCheckEndNo = MaintWork.MaintCashierIf.CashierIf.usGstCheckEndNo;
        usLeadAddress = LDT_DATA_ADR;                 /* Request Status Entry */
        break;                                                      

    case CAS_FIELD8_ADR: /* Charge Tip Rate */
        MaintCasNo.uchMinorClass = CLASS_PARACASHIERNO_DATA8;
        MaintCasNo.CashierIf.uchChgTipRate = MaintWork.MaintCashierIf.CashierIf.uchChgTipRate;
        usLeadAddress = LDT_DATA_ADR;
        break;

    case CAS_FIELD9_ADR: /* Team Number */
	     MaintCasNo.uchMinorClass = CLASS_PARACASHIERNO_DATA9;
	     MaintCasNo.CashierIf.uchTeamNo = MaintWork.MaintCashierIf.CashierIf.uchTeamNo;
	     usLeadAddress = LDT_DATA_ADR;
	     break;

    case CAS_FIELD10_ADR: /* Terminal */
	     MaintCasNo.uchMinorClass = CLASS_PARACASHIERNO_DATA10;
	     MaintCasNo.CashierIf.uchTerminal = MaintWork.MaintCashierIf.CashierIf.uchTerminal;
	     usLeadAddress = LDT_DATA_ADR;
	     break;

    case CAS_FIELD11_ADR: /* Mnemonics */
        MaintCasNo.uchMinorClass = CLASS_PARACASHIERNO_MNEMO;
        _tcsncpy(MaintCasNo.CashierIf.auchCashierName, MaintWork.MaintCashierIf.CashierIf.auchCashierName, PARA_CASHIER_LEN);
        break;

    default:
        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA);
    }

    /* Read & Set LEADTHRU */
    if (usLeadAddress) {                      
        RflGetLead (MaintCasNo.aszMnemonics, usLeadAddress);  // there is a leadthru mnemonic we need to display so get it.
    }
    MaintCasNo.uchMajorClass = CLASS_PARACASHIERNO;                  /* Set Major Class */
    DispWrite(&MaintCasNo);                                         
}

/*
*===========================================================================
** Synopsis:    VOID    MaintDelayedBalance(UCHAR uchStatus)
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function turn the delay balance flag on.
*===========================================================================
*/
VOID    MaintDelayedBalance(UCHAR uchStatus)
{
	CONST SYSCONFIG *pSysConfig = PifSysConfig();

	CliCasDelayedBalance(PIF_CLUSTER_MASK_TERMINAL_NO(pSysConfig->usTerminalPositionFromName), uchStatus);

	//set the flag in Para to signify that we are
	//in delay balance mode, and then save the data. JHHJ
	if(uchStatus)
	{
		uchMaintDelayBalanceFlag |= MAINT_DBON;
	} else
	{
		uchMaintDelayBalanceFlag &= ~MAINT_DBON;
	}

	/*The following PM_CNTRL was not being used, so we use this one
	to display the D-B message on the display*/
	if(uchStatus)
	{
		flDispRegDescrControl |= PM_CNTRL;
		flDispRegKeepControl |= PM_CNTRL;
		PifLog(MODULE_STB_LOG, LOG_EVENT_DB_FLAG_ON_3);
	} else
	{
		flDispRegDescrControl &= ~PM_CNTRL;
		flDispRegKeepControl &= ~PM_CNTRL;
		PifLog(MODULE_STB_LOG, LOG_EVENT_DB_FLAG_OFF_3);
		EJDelayBalanceUpdateFile();
	}

	EJWriteDelayBalance(uchMaintDelayBalanceFlag);

	//We set the class so that we can send the dummy data,
	//and trigger the discriptor to show D-B to inform the user that
	//the terminal is in delay balance mode
	RFLUpdateDisplayByMajorMinorClass(CLASS_UIFREGDISP, CLASS_UIFDISP_DESCONLY, DISP_SAVE_TYPE_0);

	PifSaveFarData();
}
