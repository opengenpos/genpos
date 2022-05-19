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
* Title       : MAinT FleXble DRiVe through parameter Module  ( SUPER & PROGRAM MODE )
* Category    : Maintenance, NCR 2170 US Hospitality Application Program
* Program Name: MATFXDRV.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Midium Model
* Options     : /c /AM /W4 /Gs /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: This function read & write & report Flexible Drive Through Parameter Table.
*
*           The provided function names are as follows:
*
*              MaintFlexDriveRead()   : read  & display Flexible Drive Through
*                                       Parameter Table
*              MaintFlexDriveWrite()  : write & print   Flexible Drive Through
*                                       Parameter Table
*              MaintFlexDriveReport() : print all Flexible Drive Through
*                                       Parameter Table
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  : Ver.Rev. :   Name    : Description
* Apr-18-95: 03.00.00 : T.Satoh   : initial
* Jul-19-95: 03.00.00 : M.Ozawa   : Modify for FVT Comment
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
============================================================================
                       I N C L U D E     F I L E s
============================================================================
**/

#include	<tchar.h>
#include <string.h>

#include <ecr.h>
#include <uie.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <display.h>
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <rfl.h>
#include <csstbpar.h>
#include "maintram.h"

/*
*===========================================================================
** Synopsis:    SHORT MaintFlexDriveRead( PARAFXDRIVE *pData )
*
*     Input:    *pData          : pointer to structure
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays Flexible Drive Through Parameter
*===========================================================================
*/
SHORT MaintFlexDriveRead( PARAFXDRIVE *pData )
{
    SHORT           sError = SUCCESS;

    /*
     * Set Major Class to MaintWork.ParaFxDrive
     */
    MaintWork.ParaFxDrive.uchMajorClass = CLASS_PARAFXDRIVE;

    /*************************/
    /* Branch by Minor Class */
    /*************************/
    switch(pData->uchMinorClass){
        case CLASS_PARAFXDRIVE_ALL:             /* Reading All of Datas     */
            /*
             * Reading Flexible Drive Through Parameter Table
             */
            MaintWork.ParaFxDrive.uchMinorClass = CLASS_PARAFXDRIVE_ALL;
            MaintWork.ParaFxDrive.uchTblAddr    = FXDRV_FIELD1_ADR;
            CliParaRead(&(MaintWork.ParaFxDrive));
			RflGetLead (MaintWork.ParaFxDrive.aszMnemonics, LDT_NUM_ADR);

            /*
             * Display to MLD
             */
            /*
            memcpy(&MldParaFxDrive,
                    &(MaintWork.ParaFxDrive),sizeof(PARAFXDRIVE));
            MldDispItem(&MldParaFxDrive,0);
            */

            /*
             * Display to Operater Display
             */
            DispWrite(&(MaintWork.ParaFxDrive));

            break;

        case CLASS_PARAFXDRIVE_NEXT:    /* Reading Data about Next Address  */
        case CLASS_PARAFXDRIVE_ADR :    /* Reading Data about Target Address*/
            if(pData->uchMinorClass == CLASS_PARAFXDRIVE_NEXT){
                MaintWork.ParaFxDrive.uchTblAddr ++;
            }
            else{
                MaintWork.ParaFxDrive.uchTblAddr = pData->uchTblAddr;
            }

            /*
             * Adjusting - Data Address over the Max Data Address
             */
            if(MaintWork.ParaFxDrive.uchTblAddr > FXDRV_FIELD36_ADR) {
                MaintWork.ParaFxDrive.uchTblAddr = FXDRV_FIELD1_ADR;
            }

            /*
             * Reading Flexible Drive Through Parameter Table
             */
            MaintWork.ParaFxDrive.uchMinorClass = CLASS_PARAFXDRIVE_ADR;
            CliParaRead(&(MaintWork.ParaFxDrive));
			RflGetLead(MaintWork.ParaFxDrive.aszMnemonics, LDT_NUM_ADR);

            /*
             * Display to Operater Display
             */
            DispWrite(&(MaintWork.ParaFxDrive));

            break;

        default:    break;
    }

    return(sError);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintFlexDriveWrite( PARAFXDRIVE *pData )
*
*     Input:    *pData             : pointer to structure
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function writes & prints Flexible Drive Through Parameter
*               Table
*===========================================================================
*/
SHORT MaintFlexDriveWrite( PARAFXDRIVE *pData )
{
    SHORT   sError = SUCCESS;
    UCHAR   i;

    /*
     * Set Current Data Address
     */
    i = MaintWork.ParaFxDrive.uchTblAddr;

    /****************************/
    /* Range Check of each Data */
    /****************************/
    switch(i){
      /*case FXDRV_FIELD0_ADR:  break;*/

        case FXDRV_FIELD1_ADR: case FXDRV_FIELD2_ADR:
        case FXDRV_FIELD3_ADR: case FXDRV_FIELD4_ADR:
            if(STD_NUM_OF_TERMINALS < pData->uchTblData[0]){
                sError = LDT_KEYOVER_ADR;
            }
            break;

        case FXDRV_FIELD5_ADR : case FXDRV_FIELD7_ADR : case FXDRV_FIELD9_ADR :
        case FXDRV_FIELD11_ADR: case FXDRV_FIELD13_ADR: case FXDRV_FIELD15_ADR:
        case FXDRV_FIELD17_ADR: case FXDRV_FIELD19_ADR: case FXDRV_FIELD21_ADR:
        case FXDRV_FIELD23_ADR: case FXDRV_FIELD25_ADR: case FXDRV_FIELD27_ADR:
        case FXDRV_FIELD29_ADR: case FXDRV_FIELD31_ADR: case FXDRV_FIELD33_ADR:
        case FXDRV_FIELD35_ADR:
            if(MAINT_FXDRV_SYSTEM_MAX < pData->uchTblData[0]){
                sError = LDT_KEYOVER_ADR;
            }
            break;

        case FXDRV_FIELD6_ADR : case FXDRV_FIELD8_ADR : case FXDRV_FIELD10_ADR:
        case FXDRV_FIELD12_ADR: case FXDRV_FIELD14_ADR: case FXDRV_FIELD16_ADR:
        case FXDRV_FIELD18_ADR: case FXDRV_FIELD20_ADR: case FXDRV_FIELD22_ADR:
        case FXDRV_FIELD24_ADR: case FXDRV_FIELD26_ADR: case FXDRV_FIELD28_ADR:
        case FXDRV_FIELD30_ADR: case FXDRV_FIELD32_ADR: case FXDRV_FIELD34_ADR:
        case FXDRV_FIELD36_ADR:
            if(MAINT_FXDRV_TERM_MAX < pData->uchTblData[0]){
                sError = LDT_KEYOVER_ADR;
            }
            break;

      /*default               : break;*/
    }

    /*
     * Error and return
     */
    if(sError != SUCCESS)   return(sError);

    /**************************************************/
    /* Writing Flexible Drive Through Parameter Table */
    /**************************************************/
    MaintWork.ParaFxDrive.uchMajorClass   = CLASS_PARAFXDRIVE;
    MaintWork.ParaFxDrive.uchMinorClass   = CLASS_PARAFXDRIVE_ADR;
    MaintWork.ParaFxDrive.uchTblData[i-1] = pData->uchTblData[0];
    CliParaWrite(&(MaintWork.ParaFxDrive));

    /*
     * Display to MLD
     */
    /*MldDispItem(&MaintWork.ParaFxDrive,0);*/

    /*
     * Control header item
     */
    MaintHeaderCtl(AC_FLEX_DRIVE,RPT_ACT_ADR);

    /*
     * Print journal and Receipt
     */
    MaintWork.ParaFxDrive.usPrintControl=(PRT_JOURNAL|PRT_RECEIPT);
    PrtPrintItem(NULL,&(MaintWork.ParaFxDrive));
    
    return(sError);
}

/*
*===========================================================================
** Synopsis:    VOID MaintFlexDriveReport( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function prints all Flexible Drive Through Parameter Table
*===========================================================================
*/
VOID MaintFlexDriveReport( VOID )
{
    PARAFXDRIVE ParaFxDrive;
    UCHAR       i;

    /*
     * Control header item
     */
    MaintHeaderCtl(AC_FLEX_DRIVE,RPT_ACT_ADR);

    /*
     * Set Major/Minor Class and Print Control bit(Journal and Receipt)
     */
    ParaFxDrive.uchMajorClass  = CLASS_PARAFXDRIVE;
    ParaFxDrive.uchMinorClass  = CLASS_PARAFXDRIVE_ADR;
    ParaFxDrive.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /******************/
    /* Print All Data */
    /******************/
    for(i=FXDRV_FIELD1_ADR;i<=FXDRV_FIELD36_ADR;i++){
        /*
         * Check abort key
         */
        if(UieReadAbortKey() == UIE_ENABLE){/* depress abort key */
            MaintMakeAbortKey();
            break;
        }

        /*
         * Reading Flexible Drive Trough Parameter Table
         */
        ParaFxDrive.uchTblAddr = i;
        CliParaRead(&ParaFxDrive);

        /*
         * Printing One Data
         */
        PrtPrintItem(NULL,&ParaFxDrive);
    }

    /*
     * make trailer
     */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

    return;
}
