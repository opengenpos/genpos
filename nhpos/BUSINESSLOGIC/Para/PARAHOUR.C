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
* Title       : Parameter ParaHourlyTime Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAHOUR.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaHourlyTimeRead()  : reads HOURLY TIME Data
*               ParaHourlyTimeWrite() : sets HOURLY TIME Data
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


/**
;=============================================================================
;    Synopsis:  VOID ParaHourlyTimeRead( PARAHOURLYTIME *pData )
;       input:  pData->uchAddress
;      output:  pData->uchBlockNo
;                     or
;               pData->uchHour
;               pData->uchMin
;
;      Return:  Nothing
;
; Descruption:  This function reads HOURLY TIME Data.
;===============================================================================
**/

VOID ParaHourlyTimeRead( PARAHOURLYTIME *pData )
{
    UCHAR    i;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

    /* distinguish block number from the others */

    if (i == 0) {
        pData->uchBlockNo = ParaHourlyTime[i][0];
    } else {
        pData->uchHour = ParaHourlyTime[i][0];  /* 0 = Major FSC RAM Address */
         
        pData->uchMin = ParaHourlyTime[i][1];   /* 1 = Minor FSC RAM Address */
    }
}

/**
;=============================================================================
;    Synopsis:  VOID ParaHourlyTimeWrite( PARAHOURLYTIME *pData )
;       input:  pData->uchAddress
;               pData->uchBlockNo
;                       or  
;               pData->uchHour
;               pData->uchMin
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets HOURLY TIME Data.
;===============================================================================
**/

VOID ParaHourlyTimeWrite( PARAHOURLYTIME *pData )
{
    UCHAR    i;
                                           
    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     

    /* distinguish block number from the others */

    if (i == 0) {
        ParaHourlyTime[i][0] = pData->uchBlockNo;   
    } else {
        ParaHourlyTime[i][0] = pData->uchHour;  /* 0 = Major FSC RAM Address */

        ParaHourlyTime[i][1] = pData->uchMin;   /* 1 = Minor FSC RAM Address */
    }
}


