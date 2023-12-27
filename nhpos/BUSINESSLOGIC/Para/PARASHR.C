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
* Title       : Parameter ParaSharedPrt Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARASHR.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaSharedPrtRead()  : reads TERMINAL NO.with SHARED PRINTER
*               ParaSharedPrtWrite() : sets TERMINAL NO. with SHARED PRINTER 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-23-93:  J.Ikeda  : initial                                   
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

#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID ParaSharedPrtRead( PARASHAREDPRT *pData )
;       input:  pData->uchAddress
;      output:  pData->uchTermNo
;
;      Return:  Nothing
;
; Descruption:  This function reads TERMINAL NO. with SHARED/ALTERNATIVE PRINTER.
;               The Para_SharedPrt data is an array of byte values that contain
;               the P50 Shared/Alternative Printer Assignment provisioning data.
;
;               The Para_SharedPrt array has the following structure:
;                 - a list of 16 shared and alternate pairs, one for each terminal
;                   . first item of pair is the shared printer terminal number
;                   . second item of pair is the alternate printer terminal number
;                   . list begins at address SHR_TERM1_DEF_ADR
;                 - a list of 8 remote device addresses
;                   . list begins at address SHR_KP1_DEF_ADR
;                 - a list of 8 remote printer COM port addresses
;                   . list begins at address SHR_COM_KP1_DEF_ADR
;===============================================================================
**/

VOID ParaSharedPrtRead( PARASHAREDPRT *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);      /* "-1" fits program address to RAM address */

    pData->uchTermNo = Para_SharedPrt[i];

}

PARA_KP_SHRALT ParaSharedPrtReadShrAlt(USHORT iTermNo) {
    PARA_KP_SHRALT shrAlt = { 0 };
    UCHAR  i = (iTermNo - 1) * 2 + SHR_TERM1_DEF_ADR - 1;

    shrAlt.ucShared = Para_SharedPrt[i];
    shrAlt.ucAlt = Para_SharedPrt[i + 1];

    return shrAlt;
}

UCHAR ParaSharedPrtReadKp(USHORT iKpNo) {
    UCHAR  ucShared = 0;
    UCHAR  i = (iKpNo - 1) + SHR_KP1_DEF_ADR - 1;

    if (i < SHR_KP1_DEF_ADR + 8) ucShared = Para_SharedPrt[i];

    return ucShared;
}

UCHAR ParaSharedPrtReadCom(USHORT iComNo) {
    UCHAR  ucShared = 0;
    UCHAR  i = (iComNo - 1) + SHR_COM_KP1_DEF_ADR - 1;

    if (i < SHR_COM_KP1_DEF_ADR + 8) ucShared = Para_SharedPrt[i];

    return ucShared;
}

/**
;=============================================================================
;    Synopsis:  VOID ParaSharedPrtWrite( PARASHAREDPRT *pData )
;       input:  pData->uchAddress
;               pData->uchTermNo           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets TERMINAL NO. with SHARED/ALTERNATIVE PRINTER.
;===============================================================================
**/

VOID ParaSharedPrtWrite( PARASHAREDPRT *pData )
{
    UCHAR    i = ( UCHAR)(pData->uchAddress - 1);        /* "-1" fits program address to RAM address */
                                     
	if( i < MAX_SHAREDPRT_SIZE ) {   // out of bounds guard, ParaSharedPrtWrite( PARASHAREDPRT *pData )
		Para_SharedPrt[i] = pData->uchTermNo;
	}
}
