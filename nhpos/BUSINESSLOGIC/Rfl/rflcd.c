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
* Title       : Make CheckDigits & CheckAmount                 
* Category    : Reentrant Functions Library, NCR 2170  Application
* Program Name: RflCD.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-08-92 : 00.00.01 : K.Nakajima :                                    
*           :          :            :                                    
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
;========================================================================
;+                  I N C L U D E     F I L E s
;========================================================================
**/
#include <tchar.h>
#include <stdlib.h>
#include <string.h>

#include <ecr.h>
#include <paraequ.h>
#include <rfl.h>
#include "rflin.h"
#include <csgcs.h>

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/**fh
;============================================================================
;
;   function : Make CheckDigits
;
;	Format   : CHAR RflMakeCD(ULONG ulNumber);       
;
;   input    : ULONG ulNumber         -input amount
;
;   return   : CHAR chCheckDigit      -Check Digit Value
;
;   synopis  : This function makes check digits. 
;
;============================================================================
fh**/

UCHAR   RflMakeCD(ULONG ulNumber)
{
    UCHAR  aszWeight[]  = "97531";
	UCHAR  aszNumber[RFL_MAXDIGIT + 1] = { 0 };            /* Input Number Area */
    USHORT usStrLen;                             /* Length of string */
    USHORT usWork;                               /* for Caliculate */
    USHORT i;

    /* -- convert long value to ASCII, 10 = radix -- */
    _ultoa(ulNumber, aszNumber, 10);

    /* -- get length of string -- */
    usStrLen = (USHORT) strlen(aszNumber);
	if (usStrLen > RFL_MAXDIGIT) usStrLen = RFL_MAXDIGIT;

    /* -- adjust position of Number -- */
    memmove(aszNumber + (RFL_MAXDIGIT - usStrLen), aszNumber, usStrLen);

    /* -- null supress -- */
    memset(aszNumber, '\0', RFL_MAXDIGIT - usStrLen);

    for (i = RFL_WEIGHTDIGIT, usWork = 0; i < RFL_MAXDIGIT; i++) {
        usWork += RFLATOH(aszNumber[i]) * RFLATOH(aszWeight[i - RFL_WEIGHTDIGIT]);
    }

    usWork = usWork % RFL_MODULUS;

    return ((UCHAR)(RFL_MODULUS - usWork));

}

/**fh
;============================================================================
;
;   function : Confirm Amount with CheckDigit
;
;	Format   : SHORT RflCheckCD(ULONG ulNumber);       
;
;   input    : ULONG ulNumber           -input amount
;
;   return   : SHORT   RFL_SUCCESS      -input amount is right
;                      RFL_FAIL         -input amount is wrong
;
;   synopis  : This function confirms amount. 
;
;============================================================================
fh**/
SHORT  RflCheckCD(ULONG ulNumber)                  
{

    if ( RflMakeCD(ulNumber / 100) == (UCHAR)(ulNumber % 100) ) {
        return (RFL_SUCCESS);
    }  else {
        return (RFL_FAIL);
    }

}

/**fh
;============================================================================
;
;   function : Confirm Guest Check Number with CheckDigit
;
;	Format   : SHORT RflPerformGcfCd (ULONG ulNumber);       
;
;   input    : ULONG ulNumber           -input amount
;
;   return   : SHORT   RFL_SUCCESS      -input amount is right
;                      RFL_FAIL         -input amount is wrong
;
;   synopis  : This function checks the Guest Check Number and returns
;              the correctly formed Guest Check Number and Check Digit pair
;              along with a status.
;              If MDC 12 Bit C is turned on then we are using check digits
;              with Guest Check Numbers which requires special processing
;              otherwise we are not.
;              Check digits are not used with Store Recall Systems.
;              Range checking is more for Table Service with Manual Guest
;              Check number entry than anything else.
;              It appears that MDC 12 Bit C does not apply to Store Recall System
;              and in some places a check on the MDC is skipped if the system
;              type is Store Recall.
;
;              See also TRANSOPEN_GCNCDV() which combines the separate values into
;              a printable Guest Check Number.
;============================================================================
fh**/
RflGcfNo  RflPerformGcfCd (ULONG  ulNumber)
{
	RflGcfNo myStatus = {0};

	myStatus.sRetStatus = RFL_SUCCESS;
	if (RflGetSystemType () == FLEX_STORE_RECALL) {
        if( ulNumber < 1 || ulNumber > GCF_MAX_DRIVE_NUMBER ) {
			myStatus.sRetStatus = LDT_KEYOVER_ADR;
		} else {
			myStatus.usGcNumber = (USHORT)ulNumber;
			myStatus.uchGcCd = 0;
		}
	} else {
		if (CliParaMDCCheckField(MDC_GCNO_ADR, MDC_PARAM_BIT_C)) {
			// Generate check digit on the Guest Check Number is set
			// Check that the least significant two digits, the check sum,
			// match the computed check sum.
			if (RflMakeCD(ulNumber / 100) == (UCHAR)(ulNumber % 100)) {
				// Break up validated Guest Check Number into Number and Check digits.
				myStatus.usGcNumber = (USHORT)(ulNumber / 100L);
				myStatus.uchGcCd = (UCHAR)(ulNumber % 100L);
			} else {
				myStatus.sRetStatus = LDT_ILLEGLNO_ADR;
			}
		} else {
			// Generate check digit is turned off so whole number if Guest Check No.
			myStatus.usGcNumber = (USHORT)ulNumber;
		}

		// Check the range on the Guest Check number.
		// This check is only valid for system type other than Store Recall.
		// Store Recall System guest check number cap is GCF_MAX_DRIVE_NUMBER.
		if (myStatus.usGcNumber < 1 || myStatus.usGcNumber > GCF_MAX_CHK_NUMBER) {
			myStatus.sRetStatus = LDT_KEYOVER_ADR;
		}
	}
	return myStatus;
}