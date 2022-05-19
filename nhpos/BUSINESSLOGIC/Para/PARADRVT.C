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
* Title       : Parameter ParaDrvT Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARADRVT.C
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaFxDriveRead()  : read  Flexible Drive Through Parameter
*               ParaFxDriveWrite() : write Flexible Drive Through Parameter
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :  Version  : Name    : Description
* Apr-18-95:  03.00.00 : T.Satoh : initial
*          :           :         :
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
============================================================================
                      I N C L U D E     F I L E s                         
============================================================================
**/
#include <tchar.h>
#include <string.h>
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <rfl.h>

/**
;===========================================================================
;    Synopsis:  VOID ParaFxDriveRead( PARAFXDRIVE *pData )
;       input:  pData->uchMinorClass
;               pData->uchTblAddr
;      output:  pData->uchTblData[]
;
;      Return:  Nothing
;
; Descruption:  This function read Flexible Drive Through Parameter
;===========================================================================
**/
VOID ParaFxDriveRead( PARAFXDRIVE *pData )
{
    UCHAR   i;

    switch((USHORT)pData->uchMinorClass){
        case CLASS_PARAFXDRIVE_ALL :
            memcpy (&(pData->uchTblData[0]), &(ParaFlexDrive[0]),MAX_FLEXDRIVE_SIZE); /* ### Mod (2171 for Win32) V1.0 */
            break;

        case CLASS_PARAFXDRIVE_ADR :
            i = (UCHAR)(pData->uchTblAddr - 1); /* "-1" fits to RAM address */
            pData->uchTblData[i] = ParaFlexDrive[i];
            break;

/*      default: break;                                                     */
    }

    return;
}

/**
;============================================================================
;    Synopsis:  VOID ParaFxDriveWrite( PARAFXDRIVE *pData )
;       input:  pData->uchMinorClass
;               pData->uchTblAddr
;               pData->uchTblData[]
;      output:  ParaFlexDrive[]
;
;      Return:  Nothing
;
; Descruption:  This function Write Flexible Drive Through Parameter
;============================================================================
**/
VOID ParaFxDriveWrite( PARAFXDRIVE *pData )
{
    UCHAR   i;

    switch((USHORT)pData->uchMinorClass){
        case CLASS_PARAFXDRIVE_ALL :
            memcpy (&(ParaFlexDrive[0]), &(pData->uchTblData[0]),MAX_FLEXDRIVE_SIZE);
            break;

        case CLASS_PARAFXDRIVE_ADR :
            i = (UCHAR)(pData->uchTblAddr - 1); /* "-1" fits to RAM address */
            ParaFlexDrive[i] = pData->uchTblData[i];
            break;

/*      default: break;                                                     */
    }

    return;
}


