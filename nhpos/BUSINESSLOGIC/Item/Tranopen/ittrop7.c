/*=======================================================================*\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  2170         *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TRANSACTION OPEN MODULE                                 
:   Category       : NCR 2170 US Hospitality Application 
:   Program Name   : ITTROP7.C (New Key Sequence: Reduce NC/RO/AC Keys)
:  ---------------------------------------------------------------------  
:   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
:   Memory Model       : Medium Model
:   Options            : /c /AM /W4 /G1s /Os /Za /Zp                       
:  ---------------------------------------------------------------------  
:  Abstract:
:
:    
:  ---------------------------------------------------------------------  
:  Update Histories
:   Date    : Ve.Rev.  : Name        : Description
:  7-31-98  : 03.03.00 : hrkato      : V3.3 (New Key Sequence)
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include    <string.h>

#include    "ecr.h"
#include    "uie.h"
#include    "pif.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "paraequ.h"
#include    "para.h"
#include    "rfl.h"
#include    "item.h"
#include    "csetk.h"
#include    "csstbetk.h"
#include    "itmlocal.h"

/*==========================================================================
**   Synopsis:  SHORT   ItemTransOpenNewKeySeq(UIFREGTRANSOPEN *pOpen)
*
*   Input:      UIFREGTRANSOPEN *pOpen
*   Output:     none 
*   InOut:      none
*
*   Return:     
*
*   Description:    New Key Sequence
==========================================================================*/

SHORT   ItemTransOpenNewKeySeq(UIFREGTRANSOPEN *pOpen)
{
   /* --- ### 99/8/17 ADD K.Iwata R3.5 --- */
    if (RflGetSystemType () == FLEX_STORE_RECALL) {
		// for a Store Recall system we will not allow the CLASS_UINEWKEYSEQGC and we
		// will turn anything else, which should be CLASS_UINEWKEYSEQ, into a standard CLASS_UIRECALL
		// to recall an existing guest check with a Counter or Flexible Drive Thru.
        if (pOpen->uchMinorClass == CLASS_UINEWKEYSEQGC) {
			NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION: FLEX_STORE_RECALL prohibits CLASS_UINEWKEYSEQGC.");
            return(LDT_SEQERR_ADR);
        }
        /* --- set store/recall class --- */
        pOpen->uchMinorClass = CLASS_UIRECALL;
        return( ItemTransOpenAC( pOpen ) );
    } else  if (CliParaMDCCheckField (MDC_NEWKEYSEQ_ADR, MDC_PARAM_BIT_C)) {
		/* --- Split/Multi Check Transaction allowed --- */
		if (pOpen->uchMinorClass == CLASS_UINEWKEYSEQGC) {
			return(ItemTransOpenAC(pOpen));
		} else  if (pOpen->ulAmountTransOpen == 0L) {
			if (CliParaMDCCheckField (MDC_GCNO_ADR, MDC_PARAM_BIT_D)) {
				/* --- Newcheck(Auto) Transaction is allowed so do it --- */
				UIFREGTRANSOPEN     UifOpen = *pOpen;
				return(ItemTransOpenNC(&UifOpen));
			}
			NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  MDC 12 Bit D setting prohibits auto GC#.");
		} else {
			/* --- Newcheck(Manual), Reorder, Addcheck Transaction --- */
			return(ItemTransOpenAC(pOpen));
		}
	} else {
		NHPOS_NONASSERT_NOTE("==PROVISION", "==PROVISION:  MDC 257 Bit C setting prohibits GC#.");
	}
    return(LDT_BLOCKEDBYMDC_ADR);    // CliParaMDCCheckField (MDC_NEWKEYSEQ_ADR, MDC_PARAM_BIT_C)
}


/****** End of Source ******/