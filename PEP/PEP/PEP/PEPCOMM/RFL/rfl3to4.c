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
* Title       : 3 byte, 4 byte Convertion Module                         
* Category    : Print, NCR 2170 Application Program        
* Program Name: RFL3TO4.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows: 
* 
*               RflConv3bto4b()   : Convert 3 Byte Data to ULONG Data
*               RflConv4bto3b()   : Convert ULONG Data to 3 Byte
*       
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-12-92: 00.00.01 : K.You     : initial                                   
*          :          :           :                                    
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
#include <string.h>
#include <ecr.h>

/*
*===========================================================================
** Synopsis:    VOID  RflConv3bto4b( ULONG *ulPrice, UCHAR *auchPrice )
*               
*     Input:    *ulPrice    : Destination Price Save Area 
*               *auchPrice  : Pointer of Source Price Save Area  
*    Output:    Nothing          
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Convert 3 Byte Data to ULONG Type Data 
*===========================================================================
*/

VOID RflConv3bto4b( ULONG *ulPrice, UCHAR *auchPrice )
{

    *ulPrice = ( ULONG)(*auchPrice) + 256L*( ULONG)(*(auchPrice + 1)) + 65536L*( ULONG)(*(auchPrice + 2));


}

/*
*===========================================================================
** Synopsis:    VOID  RflConv4bto3b( UCHAR *auchPrice, ULONG ulPrice )
*               
*     Input:    ulPrice     : Source Price  
*               *auchPrice  : Pointer of Destination Price Save Area  
*    Output:    Nothing          
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: Convert ULONG Type Data to 3 Byte Data 
*===========================================================================
*/

VOID RflConv4bto3b( UCHAR *auchPrice, ULONG ulPrice )
{




    *(auchPrice + 2) = ( UCHAR)(ulPrice/65536L);
    *(auchPrice + 1) = ( UCHAR)((ulPrice - 65536L*( ULONG)(*(auchPrice + 2)))/256L);
    *auchPrice = ( UCHAR)(ulPrice - 65536L*( ULONG)(*(auchPrice + 2)) - 256L*( ULONG)(*(auchPrice + 1)));                                                               
                                                                                             
}

