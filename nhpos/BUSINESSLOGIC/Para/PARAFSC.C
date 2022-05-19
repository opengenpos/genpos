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
* Title       : Parameter ParaFSC Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARAFSC.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaFSCRead()  : reads FSC Data of appointed address
*               ParaFSCWrite() : sets FSC Data
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:  00.00.01 : J.Ikeda  : initial                                   
* Nov-30-92:  01.00.03 : K.You    : Chg from "pararam.h" to <pararam.h>                                   
* Apr-17-95:  03.00.00 : H.Ishida : Chg Page of FSC Table
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
;    Synopsis:  VOID ParaFSCRead( PARAFSC *pData )
;       input:  pData->uchAddress
;      output:  pData->uchMajorFSCData
;               pData->uchMinorFSCData
;
;      Return:  Nothing
;
; Descruption:  This function reads FSC Data of appointed address.
;===============================================================================
**/

VOID ParaFSCRead( PARAFSC *pData )
{
    UCHAR    i;
    UCHAR    j;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */     
    j = ( UCHAR)(pData->uchPageNo - 1);    /* "-1" fits program Page to RAM address    */     

    /* pData->uchMajorFSCData = ParaFSC1[j].TblFsc[i].uchFsc;/* 0 = Major FSC RAM Address */
    /* pData->uchMinorFSCData = ParaFSC1[j].TblFsc[i].uchExt;/* 1 = Minor FSC RAM Address */

    pData->uchMajorFSCData = ParaFSC[j].TblFsc[i].uchFsc;/* 0 = Major FSC RAM Address */
    pData->uchMinorFSCData = ParaFSC[j].TblFsc[i].uchExt;/* 1 = Minor FSC RAM Address */

}

/**
;=============================================================================
;    Synopsis:  VOID ParaFSCWrite( PARAFSC *pData )
;       input:  pData->uchAddress
;               pData->uchMajorFSCData
;               pData->uchMinorFSCData
;      output:  Nothing
;
;      Return:  Nothing
;
; Descruption:  This function sets FSC Data.
;===============================================================================
**/

VOID ParaFSCWrite( PARAFSC *pData )
{
    UCHAR    i;
    UCHAR    j;

    i = ( UCHAR)(pData->uchAddress - 1);   /* "-1" fits program address to RAM address */         
    j = ( UCHAR)(pData->uchPageNo - 1);    /* "-1" fits program Page to RAM address    */

    /* ParaFSC1[j].TblFsc[i].uchFsc = pData->uchMajorFSCData;  /* 0 = Major FSC RAM Address */
    /* ParaFSC1[j].TblFsc[i].uchExt = pData->uchMinorFSCData;  /* 1 = Minor FSC RAM Address */
    ParaFSC[j].TblFsc[i].uchFsc = pData->uchMajorFSCData;  /* 0 = Major FSC RAM Address */
    ParaFSC[j].TblFsc[i].uchExt = pData->uchMinorFSCData;  /* 1 = Minor FSC RAM Address */
}

