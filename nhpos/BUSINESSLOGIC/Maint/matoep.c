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
* Title       : MaintOep Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATOEP.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function read & write & report OEP TABLE.
*
*           The provided function names are as follows: 
* 
*              MaintOepRead()   : read & display ORDER ENTRY PROMPT TABLE
*              MaintOepWrite()  : set & print ORDER ENTRY PROMPT TABLE
*              MaintOepReport() : print all ORDER ENTRY PROMPT TABLE   
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
*    -  -95: 00.00.01 : M.Waki    : initial                                   
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

#include	<tchar.h>
#include <string.h>

#include <ecr.h>
#include <uie.h>
#include <paraequ.h> 
#include <para.h>
#include <rfl.h> 
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>
#include <mld.h>
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintOepRead( PARAOEPTBL *pData )
*               
*     Input:    *pData          : pointer to structure for PARAOEPTBL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays Order Entry Prompt Table.
*===========================================================================
*/

SHORT MaintOepRead( PARAOEPTBL *pData )
{
	PARAOEPTBL      MldParaOEP = { 0 };

    switch (pData->uchStatus) {
    case MAINT_STS_TBLNUMBER:    /* case of table No. input */
        MaintWork.ParaOepTbl.uchMajorClass = pData->uchMajorClass;     /* copy major class */
        MaintWork.ParaOepTbl.uchTblNumber = pData->uchTblNumber;       /* copy table number */
        MaintWork.ParaOepTbl.uchTblAddr = pData->uchTblAddr;           /* copy table address */
        CliParaRead(&(MaintWork.ParaOepTbl));                          /* call ParaOepRead() */

		RflGetLead (MaintWork.ParaOepTbl.aszMnemonics, LDT_DATA_ADR);

        /******************/
        /* Display to MLD */
        /******************/
        MldParaOEP = MaintWork.ParaOepTbl;
        MldParaOEP.uchTblAddr = 0;
        MldDispItem(&MldParaOEP,0);

        DispWrite(&(MaintWork.ParaOepTbl));
        return(SUCCESS);

    case MAINT_STS_TBLADDINC:    /* case of table Address increment */
        MaintWork.ParaOepTbl.uchMajorClass = pData->uchMajorClass;     /* copy major class */
        MaintWork.ParaOepTbl.uchTblAddr++;      /* next table address data read and display */
        if(MaintWork.ParaOepTbl.uchTblAddr > 10) {
            MaintWork.ParaOepTbl.uchTblAddr = 1;
        }
        CliParaRead(&(MaintWork.ParaOepTbl));

		RflGetLead(MaintWork.ParaOepTbl.aszMnemonics, LDT_DATA_ADR);
        DispWrite(&(MaintWork.ParaOepTbl));
        return(SUCCESS);

    case MAINT_STS_TBLADDRESS:    /* case of table Address input */
        MaintWork.ParaOepTbl.uchMajorClass = pData->uchMajorClass;     /* copy major class */
        MaintWork.ParaOepTbl.uchTblAddr = pData->uchTblAddr;           /* copy table address */
        CliParaRead(&(MaintWork.ParaOepTbl));                          /* call ParaOepRead() */

		RflGetLead(MaintWork.ParaOepTbl.aszMnemonics, LDT_DATA_ADR);
        DispWrite(&(MaintWork.ParaOepTbl));
        return(SUCCESS);

    default:
        return(SUCCESS);
    }
}

/*
*===========================================================================
** Synopsis:    SHORT MaintOepWrite( PARAOEPTBL *pData )
*               
*     Input:    *pData             : pointer to structure for PARAOEPTBL
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*               LDT_KEYOVER_ADR    : wrong data
*
** Description: This function sets & prints OEPOUNT/BONUS RATE.
*===========================================================================
*/

SHORT MaintOepWrite( PARAOEPTBL *pData )
{

    if (pData->uchOepData[0] > MAX_TABLE_NO) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MaintWork.ParaOepTbl.uchOepData[0] = pData->uchOepData[0];
    MaintWork.ParaOepTbl.uchMajorClass = pData->uchMajorClass;
    CliParaWrite(&(MaintWork.ParaOepTbl));                         /* call ParaOepWrite() */

    /******************/
    /* Display to MLD */
    /******************/
    MldDispItem(&MaintWork.ParaOepTbl,0);

    MaintHeaderCtl(AC_OEP_MAINT, RPT_ACT_ADR);

    MaintWork.ParaOepTbl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.ParaOepTbl));
    
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintOepReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all OEP REPORT.
*===========================================================================
*/

VOID MaintOepReport( VOID )
{
    UCHAR           i;
	PARAOEPTBL      ParaOepTbl = { 0 };

    MaintHeaderCtl(AC_OEP_MAINT, RPT_ACT_ADR);
    ParaOepTbl.uchMajorClass = CLASS_PARAOEPTBL;

    ParaOepTbl.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data at every address */
    for (i = 1; i <= MAX_TABLE_NO ; i++) {
		UCHAR   j, ReportAbortFlug = 0;

		for (j = 1; j <= 10 ; j++) {

            if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
                MaintMakeAbortKey();
                ReportAbortFlug = 1;
                break;
           }
            ParaOepTbl.uchTblNumber = i;
            ParaOepTbl.uchTblAddr = j;
            CliParaRead(&ParaOepTbl);                              /* call ParaOepRead() */
            if (j == 1){
                ParaOepTbl.uchTblNumber = i;
            }else{
                ParaOepTbl.uchTblNumber = 0;
            }
            PrtPrintItem(NULL, &ParaOepTbl);
        }

        if(ReportAbortFlug){
            break;
        }
    }

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
}
