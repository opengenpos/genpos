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
* Title       : MaintDeptNoMenu Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATDPTNO.C
* --------------------------------------------------------------------------
* Abstract: This function read & write & report Dept No .
*
*           The provided function names are as follows: 
*
*               MaintDeptNoMenuRead()   : read & display Dept No 
*               MaintDeptNoMenuWrite()  : set Dept No
*               MaintDeptNoMenuReport() : print all Dept No REPORT
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-26-99:00.00.01    :M.Ozawa    : initial                                   
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
#include <pif.h>
#include <log.h>
#include <appllog.h>
#include <rfl.h>
#include <paraequ.h> 
#include <para.h>
#include <fsc.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csstbpar.h>

#include "maintram.h" 


/*
*===========================================================================
** Synopsis:    SHORT MaintDeptNoMenuRead( PARADEPTNOMENU *pData )
*               
*     Input:    *pData          : pointer to structure for PARADEPTNOMENU
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS          : read data
*               LDT_KEYOVER_ADR  : wrong data
*
** Description: This function reads & displays Dept No.
*===========================================================================
*/

SHORT MaintDeptNoMenuRead( PARADEPTNOMENU *pData )
{
    UCHAR           uchMinorFSCData;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                    /* without data */
        uchMinorFSCData = ++MaintWork.DeptNoMenu.uchMinorFSCData;
        /* check minor FSC data */
        if (uchMinorFSCData > MAINT_MINORFSC_DEPT) {
            uchMinorFSCData = 1;                                    /* initialize address */
        }
    } else {                                                        /* with data */                                          
        uchMinorFSCData = pData->uchMinorFSCData;
    }

    /* check minor FSC data */
    if (uchMinorFSCData < 1  || MAINT_MINORFSC_DEPT < uchMinorFSCData) {
        return(LDT_KEYOVER_ADR);                                    /* wrong data */
    }

    MaintWork.DeptNoMenu.uchMajorClass = pData->uchMajorClass; 
    MaintWork.DeptNoMenu.uchMinorFSCData = uchMinorFSCData;
    MaintWork.DeptNoMenu.usDeptNumber = pData->usDeptNumber;
    MaintWork.DeptNoMenu.uchMenuPageNumber = uchMaintMenuPage;
    CliParaRead(&(MaintWork.DeptNoMenu));                            /* call ParaDeptNoMenuRead() */

	RflGetLead(MaintWork.DeptNoMenu.aszMnemonics, LDT_DATA_ADR);

    DispWrite(&(MaintWork.DeptNoMenu));

    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintDeptNoMenuWrite( PARADEPTNOMENU *pData )
*               
*     Input:    *pData             : pointer to structure for PARADEPTNOMENU
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS            : read data
*               LDT_SEQERR_ADR     : error
*
** Description: This function sets & prints Dept No.
*===========================================================================
*/

SHORT MaintDeptNoMenuWrite( PARADEPTNOMENU *pData )
{

    /* Dept number has already checked by UI */
    
    /* check input data */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {            /* without data */
        return(LDT_SEQERR_ADR);                             /* sequence error */
    }

    /* check minor class */

    switch(pData->uchMinorClass) {

    case CLASS_PARADEPTNOMENU_KEYEDDEPT:
        MaintWork.DeptNoMenu.uchMajorClass = pData->uchMajorClass; 
        MaintWork.DeptNoMenu.uchMinorFSCData = pData->uchMinorFSCData;
        MaintWork.DeptNoMenu.uchMenuPageNumber = uchMaintMenuPage;
        /* break */

    case CLASS_PARADEPTNOMENU_DEPTNO:
        MaintWork.DeptNoMenu.usDeptNumber = pData->usDeptNumber;
        break;

    default:
/*        PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
        return(LDT_KEYOVER_ADR);
    }
    CliParaWrite(&(MaintWork.DeptNoMenu));                           /* call ParaDeptNoMenuWrite() */

    /* control header item */
        
    MaintHeaderCtl(AC_DEPT_MENU, RPT_ACT_ADR);

    /* set journal bit & receipt bit */

    MaintWork.DeptNoMenu.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.DeptNoMenu));
    
    /* set minor FSC data for next display */

    /* check address */

    MaintWork.DeptNoMenu.uchMinorFSCData++;

    if (MaintWork.DeptNoMenu.uchMinorFSCData > MAINT_MINORFSC_DEPT) {
        MaintWork.DeptNoMenu.uchMinorFSCData = 1;                /* initialize address */
    }
    MaintDeptNoMenuRead(&(MaintWork.DeptNoMenu));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintDeptNoMenuReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function prints all Dept No. REPORT.
*===========================================================================
*/

VOID MaintDeptNoMenuReport( VOID )
{
    PARADEPTNOMENU    ParaDeptNoMenu = { 0 };

    /* control header item */
        
    MaintHeaderCtl(AC_DEPT_MENU, RPT_ACT_ADR);
    ParaDeptNoMenu.uchMajorClass = CLASS_PARADEPTNOMENU;

    /* set journal bit & receipt bit */

    ParaDeptNoMenu.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );

    /* set data every pages */

    for (UCHAR i = 1; i <= DEPTPG_ADR_MAX; i++) {

        ParaDeptNoMenu.uchMenuPageNumber = i;

        /* set data at every minor FSC data */

        for (UCHAR j = 1; j <= MAINT_MINORFSC_DEPT; j++) {

            /* check abort key */

            if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
                MaintMakeAbortKey();
                MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
                return;
            }
            ParaDeptNoMenu.uchMinorFSCData = j;
            CliParaRead(&ParaDeptNoMenu);                            /* call ParaDiscRead() */
            PrtPrintItem(NULL, &ParaDeptNoMenu);
        }
    }

    /* make trailer */
    
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);

}


/*
*===========================================================================
** Synopsis:    VOID MaintDeptNoMenuReplace( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing 
*
** Description: This function relocate Dept No. position in Dept No. table.
*===========================================================================
*/

VOID MaintDeptNoMenuReplace( VOID )
{
#if 0

    UCHAR           i, j, uchDeptKey;
    USHORT          usRetLen;
    PARADEPTNOMENU  ParaDeptNoMenu;
    PARAFSC         ParaFSC;
    USHORT          usDeptNo[MAX_PAGE_NO * 165/*MAX_FSC_NO*/];


    CliParaAllRead(CLASS_PARADEPTNOMENU,
                (UCHAR *)usDeptNo,
                MAX_PAGE_NO * /*MAX_FSC_NO*/165 * sizeof(USHORT),
                0,
                &usRetLen);

    ParaDeptNoMenu.uchMajorClass = CLASS_PARADEPTNOMENU;
    ParaFSC.uchMajorClass = CLASS_PARAFSC;

    /* set data every pages */

    for (i = 1; i <= MAX_PAGE_NO; i++) {

        ParaFSC.uchPageNo = i;
        ParaDeptNoMenu.uchMenuPageNumber = i;

        /* set data at every minor FSC data */

        for (j = 1; j <= 165/*MAX_FSC_NO*/; j++) {

            ParaFSC.uchAddress = j;
            ParaDeptNoMenu.uchMinorFSCData = j;
            CliParaRead(&ParaFSC);

            if ((ParaFSC.uchMajorFSCData + UIE_FSC_BIAS) == FSC_KEYED_DEPT) {

                /* relocate dept no. */

                uchDeptKey = ParaFSC.uchMinorFSCData;
                if (uchDeptKey) uchDeptKey--;
                ParaDeptNoMenu.usDeptNumber = usDeptNo[(i-1)*165/*MAX_FSC_NO*/ + uchDeptKey];
                CliParaWrite(&ParaDeptNoMenu);

                ParaFSC.uchMinorFSCData = j;
                CliParaWrite(&ParaFSC);

            } else {

                /* clear dept no. */

                ParaDeptNoMenu.usDeptNumber = 0;
                CliParaWrite(&ParaDeptNoMenu);
            }
        }
    }
#endif
}

