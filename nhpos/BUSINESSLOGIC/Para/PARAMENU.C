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
* Title       : Parameter ParaMenuPLU Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAMENU.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaMenuPLURead()  : reads PLU SET MENU
*               ParaMenuPLUWrite() : writes PLU SET MENU
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial                                   
* Nov-30-92:  01.00.03 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
*          :           :                                    
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
;=============================================================================
**/
#include	<tchar.h>
#include	<memory.h>

#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <rfl.h>

USHORT _memcmp(UCHAR *puchData1, UCHAR *puchData2, USHORT usLen);

/**
;=============================================================================
;    Synopsis:  SHORT ParaMenuPLURead( PARAMENUPLUTBL *pData )
;       input:  pData->SetPLU->usPLU
;      output:  pData->SetPLU[0] - pData->SetPLU[5]
;               pData->uchAddress  
;      Return:  SUCCESS        : successful
;               LDT_NTINFL_ADR : address is not                                   
;
; Descruption:  This function reads PLU SET MENU.
;===============================================================================
**/

SHORT ParaMenuPLURead( PARAMENUPLUTBL *pData )
{
    UCHAR    i;
    SHORT    j;
    SHORT    k;
    TCHAR   aszZero[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    if (pData->uchAddress == 0) {

        /* search address */
        for ( k = -1, j = 0; j < MAX_SETMENU_NO; j++ ) {
            
            if ( _tcsncmp(pData->SetPLU[0].aszPLUNumber, ParaMenuPLU[j][0].aszPLUNumber, PLU_MAX_DIGIT ) == 0) { 
                pData->uchAddress = (UCHAR)( j + 1);         /* "+1" fits RAM address to program address */    
                break;
            } else if ( _tcsncmp(ParaMenuPLU[j][0].aszPLUNumber, aszZero, PLU_MAX_DIGIT) == 0) {
                if ( k == -1 ) {
                    k = j;
                }
            }
            if ( j == MAX_SETMENU_NO - 1 ) {          /* address is not */
                pData->uchAddress = (UCHAR)( k + 1);         /* "+1" fits RAM address to program address */    
                return(LDT_NTINFL_ADR);
            }
        }
    }

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */

	if (i < MAX_SETMENU_NO) {
		/* set data */
		for( j = 0; j < MAX_SETPLU_DATA; j++ ) {
			pData->SetPLU[j] = ParaMenuPLU[i][j];
		}
	} else {
		NHPOS_ASSERT_TEXT((i < MAX_SETMENU_NO), "**ERROR - ParaMenuPLURead() Address out of range.");
        return(LDT_NTINFL_ADR);
	}

    return(SUCCESS);                      /* read data */

}

/**
;=============================================================================
;
;    Synopsis:  VOID ParaMenuPLUWrite( UCHAR uchAction, PARAMENUPLUTBL *pData )
;       input:  pData->uchAddress
;               uchAction
;               pData->SetPLU[0] - PARAMENUPLUTBL->SetPLU[5]
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets/resets PLU NO. of SET MENU.
;               
;===============================================================================
**/
VOID ParaMenuPLUWrite( UCHAR uchAction, PARAMENUPLUTBL *pData )
{
    UCHAR    i;
    UCHAR    j;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

	if (i < MAX_SETMENU_NO) {
		/* distinguish SET_DATA/RESET_DATA */
		switch(uchAction) {
		case SET_DATA:   
			/* set plu no. */
			for( j = 0; j < MAX_SETPLU_DATA; j++ ) {     
				ParaMenuPLU[i][j] = pData->SetPLU[j];
			}
			break;

		case RESET_DATA: 
			/* reset plu no. */
			for( j = 0; j < MAX_SETPLU_DATA; j++ ) {    
				memset(&ParaMenuPLU[i][j], 0, sizeof(MENUPLU));
			}
			break;

		default:         
			break;

		}
	} else {
		NHPOS_ASSERT_TEXT((i < MAX_SETMENU_NO), "**ERROR - ParaMenuPLUWrite() Address out of range.");
	}
}

#if POSSIBLE_DEAD_CODE
// not sure what this is for am pretty sure it should not be here.
// should use standard C library instead.
USHORT _memcmp(UCHAR *puchData1, UCHAR *puchData2, USHORT usLen)
{
    USHORT  i;

    for (i=0; i<usLen; i++) {
        if (puchData1[i] != puchData2[i]) break;
    }
    if (i == usLen) {
        return 0;   /* same data */
    } else {
        return 1;   /* different data */
    }
}
#endif
