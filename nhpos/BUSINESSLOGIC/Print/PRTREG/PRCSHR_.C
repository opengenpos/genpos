/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrCShr_.C                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
*      This file contains nop function.
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Aug-25-93 : 01.00.13 :  K.You     : initial.   
* Jan-07-00 : 01.00.00 :  H.Endo    : change PrtShrInitReverse's parameter
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
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/

#include	<tchar.h>
#include <ecr.h>
#include <paraequ.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
/*VOID    PrtShrInitReverse(TRANINFORMATION  *pTran)*/
VOID    PrtShrInitReverse(VOID)	/* endo update(parameter change) 2000/1/7 */
{
    /*pTran = pTran;*/  /* endo update(delete) 2000/1/7 */
    return;
}

VOID    PrtShrEnd(VOID)
{
    return;
}

SHORT   PrtShrTermLock(UCHAR uchPrtStatus)
{
    uchPrtStatus = uchPrtStatus;
    return (SUCCESS);
}

VOID    PrtShrTermUnLock( VOID )
{
}

    
VOID PrtChangeFont(USHORT usType)
{
    usType = usType;
}

VOID PrtShrPartialSend(USHORT  usPrintControl)
{
}

SHORT  PrtCheckAbort(VOID)
{
    return ( 0 );
}

UCHAR    PrtCheckShr(UCHAR  uchTrgPrt)
{

    uchTrgPrt = uchTrgPrt;
    return( 0 );

}


/****** End Of Source ******/
