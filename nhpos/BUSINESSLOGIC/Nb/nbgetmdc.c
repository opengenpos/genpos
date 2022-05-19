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
* Title       : Notice Board Sub routine Module                         
* Category    : Notice Board, NCR 2170 US Hospitality Application         
* Program Name: NBGETMDC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: The provided function names are as follows:  
*
*               NbGetRcvTime()        : Get receive timer value
*               NbGetMDCTime()        : Read MDC 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Feb-15-93:00.00.01:H.Yamaguchi: initial                                   
* Apr-17-93:00.00.09:H.Yamaguchi: Divide Retry Co( Addr 10) & Timer (Addr 9)
* Sep-07-93:00.00.01:H.Yamaguchi: Same As HPINT.
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
#include <pif.h>
#include <log.h>
#include <paraequ.h>
#include <para.h>
#include <csstbpar.h>
#include <csstbstb.h>
#include <nb.h>
#include "nbcom.h"

/*
*===========================================================================
*   Internal Work Flag Declarations
*===========================================================================
*/

/*
*===========================================================================
*   Internal Work Area Declarations
*===========================================================================
*/
                                                                                
/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/

/*
*===========================================================================
** Synopsis:    LONG    NbGetRcvTime(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*
** Return:      Timer value ( ?? miliseconds )
*
** Description: Get Receive Timer Value  (92' 7/30 100 mili. -> mili. )
*===========================================================================
*/
LONG    NbGetRcvTime(VOID)
{
    LONG    lTime;
    SHORT   sUadd;

    lTime  = (LONG)NbGetMDCTime();      /* Read MDC timer value */

    lTime &= (LONG)NB_MASK_MDC_TIME;    /* Mask high 4 bits   */
    lTime *= (LONG)1000;                /* Muliple 1000       */

    lTime += NB_TMDEFAULT;              /* Adds default time */

    sUadd = NbCheckUAddr();        /* Check Unique address */

    switch ( sUadd ) {               
		case NB_MASTERUADD :            /* No adjustment */
			if ( 0 == NbSndBuf.NbConMes.usNbFunCode ) {  /* Is not set cmd ? */
				lTime = (NB_TMDEFAULT - NB_TMBALANCE);   /* Set  3 seconds time */
			}  else if ( (NB_FCSTARTASMASTER | NB_BMASTERUADD) == NbSndBuf.NbConMes.usNbFunCode )   {
				lTime = (NB_TMDEFAULT);   /* Set  5 seconds time */
			}
			break;

		case NB_BMASTERUADD :
			if ( 0 == NbSndBuf.NbConMes.usNbFunCode ) {  /* Is not set cmd ? */
				lTime = (NB_TMDEFAULT - NB_TMBALANCE);   /* Set  3 seconds time */
			} else {
				if ( usNbRcvFMT == 0 ) {
					/* if receive from master, then adjustment */
					lTime += NB_TMBALANCE;     /* Adds usTime to 2 seconds */
				}
			}
			break;

		default:
			lTime += (NB_TMDEFAULT + NB_TMDEFAULT);  /* Adds usTime to (10 seconds) default seconds */
			break ;
    }

    return(lTime);
}

/*
*===========================================================================
** Synopsis:    USHORT  NbGetMDCTime(VOID)
*     Input:    nothing
*     Output:   nothing
*     InOut:    nothing
*                                                           S   E
** Return:      High 4 bits:Retry Counter (0-15 times)   -->3 - 18
**              Low  4 bits:Timer Value   (0-15 seconds) -->5 - 20
*
** Description: Get MDC Timer Value and Retry Counter 
*===========================================================================
*/
USHORT  NbGetMDCTime(VOID)
{
    PARAMDC pMes;
    USHORT  usTime;

    pMes.uchMajorClass = CLASS_PARAMDC;         /* set MDC address */
    pMes.usAddress     = (USHORT)NB_MDCTIMER;   /* set MDC address */
    
    CliParaRead(&pMes);                 /* Get MDC Data   */

    usTime = (USHORT)pMes.uchMDCData;

    return(usTime);
}

/*====== End of Source File ======*/

