/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Main 2x20 Display Module                         
* Category    : Display, NCR 2170 US Hospitality Application         
* Program Name: DISP20.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*       VOID DispWrite(VOID *pData): display module main
*       VOID DispWriteSpecial(REGDISPMSG *pData1, REGDISPMSG *pData2)   
*                                  : display module main
*                   
*           DispRegWriteSpecial() is called when the display in clear key 
*           entry is different from that before clear key.        
*                    
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev  :  Name    : Description
* Nov-09-95 : 03.01.00 : M.Ozawa  : initial                                   
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
/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/
#include	<tchar.h>

#include <string.h>
#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <regstrct.h>
#include <display.h>
#include <mld.h>

/*
*=============================================================================
**  Synopsis: VOID DispWrite(VOID *pData) 
*               
*       Input:  *pData  
*      Output:  nothing
*
**  Return: nothing 
*               
**  Discription: This function is the main function of display module. 
*===============================================================================
*/

VOID DispWrite(VOID *pData) 
{
    DispWrite20(pData); /* use 2x20 display format */
}

/*
*=============================================================================
**  Synopsis: VOID DispWriteSpecial(REGDISPMSG *pData1, REGDISPMSG *pData2) 
*               
*       Input:  *pData1, *pData2  
*      Output:  nothing
*
**  Return: nothing 
*               
**  Discription: This function is the main function of display module. 
*                (in special case.)
*===============================================================================
*/
VOID DispWriteSpecial(REGDISPMSG *pData1, REGDISPMSG *pData2) 
{
    DispWriteSpecial20(pData1, pData2); /* use 2x20 display format */
}

/****** End of Source ******/
