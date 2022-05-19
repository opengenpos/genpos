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
* Title       : Parameter ParaAdjMnemo Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAADJ.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaAdjMnemoRead()  : read ADJECTIVE MNEMONICS 
*               ParaAdjMnemoWrite() : set ADJECTIVE MNEMONICS 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial                                   
* Nov-30-92 : 01.00.00 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
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
#include <tchar.h>
#include <string.h>

#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/*
*=============================================================================
**  Synopsis: VOID ParaAdjMnemoRead(PARAADJMNEMO *pData) 
*               
*       Input:  *pData  : pointer to structure for PARAADJMNEMO
*                  (pData->uchAddress)
*      Output:  pData->aszMnemonics
*
**  Return: nothing 
*               
**  Descruption: This function reads ADJECTIVE MNEMONICS of appointed address. 
*===============================================================================
*/

VOID ParaAdjMnemoRead( PARAADJMNEMO *pData )
{
    USHORT    i = (pData->uchAddress - 1);         /* "-1" fits program address to RAM address */ 

	if (i < MAX_ADJM_NO ) {
		_tcsncpy(&pData->aszMnemonics[0], &Para.ParaAdjMnemo[i][0], PARA_ADJMNEMO_LEN);
		pData->aszMnemonics[PARA_ADJMNEMO_LEN] = '\0';
	} else {
		NHPOS_ASSERT(pData->uchAddress > 0);
		NHPOS_ASSERT(pData->uchAddress <= MAX_ADJM_NO);
		pData->aszMnemonics[0] = 0;
	}
}

/*
*=============================================================================
**  Synopsis: VOID ParaAdjMnemoWrite(PARAADJMNEMO *pData) 
*               
*       Input:  *pData  : pointer to structure for PARAADJMNEMO
*                  (pData->uchAddress)
*                  (pData->aszMnemonics)  
*      Output:  Nothing
*
**  Return: nothing 
*               
**  Descruption: This function set ADJECTIVE MNEMONICS in appointed address. 
*===============================================================================
*/

VOID ParaAdjMnemoWrite( PARAADJMNEMO *pData )
{
    UCHAR    i = (pData->uchAddress - 1);   /* "-1" fits program address to RAM address */       

	if (i < MAX_ADJM_NO ) {
		_tcsncpy(&Para.ParaAdjMnemo[i][0], &pData->aszMnemonics[0], PARA_ADJMNEMO_LEN);
	} else {
		NHPOS_ASSERT(pData->uchAddress > 0);
		NHPOS_ASSERT(pData->uchAddress <= MAX_ADJM_NO);
	}
}

