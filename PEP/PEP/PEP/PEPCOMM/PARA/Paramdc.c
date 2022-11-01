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
* Title       : Parameter ParaMDC Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAMDC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaMDCRead()  : read MDC Data
*               ParaMDCWrite() : sets MDC Data
*               ParaMDCCheck() : checks MDC Data
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial                                   
* Nov-30-92:  01.00.03 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
*          :           :                                    
* Jan-24-00:  01.00.00 : K.Yanagida : Saratoga
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
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/**
;=============================================================================
;    Synopsis:  VOID  ParaMDCRead( PARAMDC *pData )
;       input:  pData->usAddress
;      output:  pData->uchMDCData
;
;      Return:  Nothing
;
; Descruption:  This function read MDC Data.
;===============================================================================
**/

VOID ParaMDCRead(PARAMDC *pData )
{
    USHORT    i;

    i = pData->usAddress;              
    
    /* calculate for fitting program address to ram address */

    i = (USHORT)((i - 1) / 2); 

    pData->uchMDCData = ParaMDC[i];

}

/**
;=============================================================================
;    Synopsis:  VOID ParaMDCWrite( PARAMDC *pData )
;       input:  pData->usAddress
;               pData->uchMDCData           
;      output:  Nothing   
;
;      Return:  Nothing
;
; Descruption:  This function sets MDC Data.
;===============================================================================
**/

VOID ParaMDCWrite(PARAMDC *pData)
{

    USHORT    i;

    i = pData->usAddress;    
            
    /* calculate for fitting program address to ram address */

    i = (USHORT)((i - 1) / 2); 

    ParaMDC[i] = pData->uchMDCData;

}

/**
;=============================================================================
;    Synopsis:  UCHAR ParaMDCCheck( UCHAR uchAddress,UCHAR uchMaskData )
;       input:  uchAddress 
;               uchMaskData
;      output:  Nothing   
;
;      Return:  Masked bit pattern 
;
; Descruption:  This function checks MDC Data.
;===============================================================================
**/

UCHAR ParaMDCCheck( USHORT usAddress, UCHAR uchMaskData )
{
    USHORT   i;
    UCHAR    uchMdc = 0;

    i = usAddress; 

    /* calculate for fitting program address to ram address */

    i = ( UCHAR)((i - 1) / 2);
    
    uchMdc = ParaMDC[i];
    uchMdc &= uchMaskData;
    return(uchMdc);                     /* return - masked bit pattern */
}                   
    






                      
                      
                      
                      
                        
                        
                        
                        
