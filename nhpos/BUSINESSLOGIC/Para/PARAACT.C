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
* Title       : Parameter ParaActCodeSec Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAACT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaActCodeSecRead()  : read ACTION CODE SECURITY 
*               ParaActCodeSecWrite() : set ACTION CODE SECURITY 
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
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>

/*
*=============================================================================
**  Synopsis: VOID ParaActCodeSecRead(PARAACTCODESEC *pData) 
*               
*       Input:  *pData  : pointer to structure for PARAACTCODESEC
*      Output:  nothing
*
**  Return: nothing 
*               
**  Descruption: This function reads ACTION CODE SECURITY of appointed adddress. 
*===============================================================================
*/

VOID ParaActCodeSecRead( PARAACTCODESEC *pData )
{
    USHORT   i;

    i = pData->usAddress;               
    
    /* calculate for fitting program address to ram address */

    i = (i - 1) / 2; 

    pData->uchSecurityBitPatern = ParaActCodeSec[i];

}

/*
*=============================================================================
**  Synopsis: VOID ParaActCodeSecWrite(PARAACTCODESEC *pData) 
*               
*       Input:  *pData  : pointer to structure for PARAACTCODESEC
*                  (pData->uchAddress)
*                  (pData->uchSecurityBitPattern)
*      Output:  nothing
*
**  Return: nothing 
*               
**  Descruption: This function sets ACTION CODE SECURITY in appointed adddress. 
*===============================================================================
*/

VOID ParaActCodeSecWrite( PARAACTCODESEC *pData )
{
    USHORT  i;

    i = pData->usAddress;               
    
    /* calculate for fitting program address to ram address */

    i = (i - 1) / 2; 

    ParaActCodeSec[i] = pData->uchSecurityBitPatern; 

}

