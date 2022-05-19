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
* Title       : TOTAL, Total Function Library Subroutine                        
* Category    : TOTAL, NCR 2170 Hospitarity Application         
* Program Name: Ttlmdcch.c                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:        
*           UCHAR TtlMDCCheck()         - Check MDC
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-06-92:00.00.01:Kterai     :                                    
*          :        :           :                                    
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
#include	<tchar.h>
#include <ecr.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>

#if defined(POSSIBLE_DEAD_CODE)
/*
*============================================================================
**Synopsis:		UCHAR TtlMDCCheck(UCHAR uchAddress, UCHAR uchField)
*
*    Input:		UCHAR uchAddress        - MDC Address
*               UCHAR uchField          - MDC BIT Data
*
*   Output:		Nothing
*    InOut:     Nothing
*
** Return:		TTL_ON  - On of Checking MDC BIT 
*				TTl_OFF	- Off of Checking MDC BIT
*
** Description  This function checks the MDC.
*
*============================================================================
*/
UCHAR TtlMDCCheck(UCHAR uchAddress, UCHAR uchField)
{
    return(CliParaMDCCheck(uchAddress, uchField));
}
/* End of File */

#endif