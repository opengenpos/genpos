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
* Title       : HALO management                 
* Category    : Reentrant Functions Library, NCR 2170   Application
* Program Name: RflHALO.C                                                      
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
* Jun-11-92 :          : K.Nakajima : when halo amount = 0, return success.                                   
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
#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <rfl.h>
#include <csstbpar.h>

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/**
=========================================================================
			V A R I A B L E    D E C L A R A T I O N
=========================================================================
**/
USHORT usHaloStatus;

DCURRENCY  RflLLabs (DCURRENCY  lValue)
{
	if (lValue < 0) lValue *= -1;

	return lValue;
}

/**fh
;============================================================================
;
;   function : HALO Check
;
;	Format   : SHORT RflHALO(LONG lAmount, CHAR chHALO);       
;
;   input    : LONG lAmount             -input amount
;
;   return   : SHORT   RFL_SUCCESS      -input amount is within right
;                      RFL_FAIL         -input amount is over HALO
;
;   synopis  : This function checks an amount against a High Amount Lock Out (HALO). 
;              A HALO indicator, stored in the Para memory resident data base, has
;              a sequence of digits which encodes the HALO amount.
;               - first digit is the maximum number of digits including any decimal portion
;               - second digit is the maximum number for the first digit of the limit
;              For instance a HALO indicator of 79 means:
;                - maximum of seven (7) digits in the amount
;                - the first digit must be nine (9) or less
;              For instance a HALO indicator of 45 means:
;               - maximum of four (4) digits in the amount
;               - the first digit must be five (5) or less
;
;============================================================================
fh**/
SHORT  RflHALO_OverRide(DCURRENCY *plAmount, CHAR chHALO, USHORT usMdc, UCHAR uchField)
{
    DCURRENCY   lHALO;                              /* HALO amount */
	SHORT  sHaloCheck = RFL_SUCCESS;
	USHORT usLeadingIndic = (0x80 & chHALO);   // remember if two leading digit is set or not
    USHORT usQuot;                             /* quotient */
    USHORT i;

    if (chHALO == 0) {
        return(RFL_SUCCESS);
    }

	//SR 143 HALO Override key cwunn
	//skip halo checking if HALO Override is active
	if(usHaloStatus == 0x01){
		usHaloStatus = 0x00; //reset for next operation
		return(RFL_SUCCESS); //return sucess, indicating amount is legal (b/c of override key)
	}

    /* -- get HALO amount -- */
	chHALO &= 0x7f;                   // remove the two leading digit indicator if set
    usQuot =  (USHORT)(chHALO / 10);
	if (usLeadingIndic && usQuot > 1) usQuot--;
    for ( i = 1, lHALO = 1L; i < usQuot;  i++) {
        lHALO = 10 * lHALO;
    }

	chHALO %= 10;
	if (usLeadingIndic && usQuot > 1) {
		// two leading digits are set so value should be n5 and not single digit n
		chHALO *= 10;
		chHALO += 5;
	}
    lHALO = chHALO * lHALO;

    /* -- check input amount -- */
    if (lHALO < *plAmount) {
		if (usMdc != 0 && CliParaMDCCheckField(usMdc, uchField) != 0) {
			*plAmount = lHALO;
		} else {
			sHaloCheck = RFL_FAIL;
		}
    }

	return sHaloCheck;
}


SHORT  RflHALO(DCURRENCY lAmount, CHAR chHALO)
{
	DCURRENCY  lTempAmount = lAmount;

	return RflHALO_OverRide (&lTempAmount, chHALO, 0, 0);
}


/**fh
;============================================================================
;
;   function : set halo override status bit
;
;	Format   : VOID RflSetHALOStatus(USHORT status);       
;
;   input    : USHOR status				-input status
;
;   return   : Nothing
;
;   synopis  : This function sets the HALO Override status bit
;
;============================================================================
fh**/
VOID  RflSetHALOStatus(USHORT status)
{
	usHaloStatus = status;
}