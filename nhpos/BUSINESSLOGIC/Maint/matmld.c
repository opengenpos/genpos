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
* Title       : MaintMldMnemo Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATMLD.C
* --------------------------------------------------------------------------
* Abstract: This function read & write TRANSACTION MNEMONICS, P 65.
*
*           The provided function names are as follows: 
* 
*              MaintMldMnemoRead()   : read & display LCD MNEMONICS
*              MaintMldMnemoWrite()  : read & display LCD MNEMONICS
*              MaintMldMnemoReport() : report & print LCD MNEMONICS report  
*               
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Aug-14-98: 03.03.00 : M.Ozawa   : initial                                   
** GenPOS
* Jul-20-17: 02.02.02 : R.Chambers : replace use of LCD_ADR_MAX with MAX_MLDMNEMO_SIZE
*          :          :           :                                    
*===========================================================================
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
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
#include <tchar.h>
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
#include <csstbpar.h>
#include <csop.h>
#include <csstbopr.h>
#include "maintram.h" 

/*
*===========================================================================
** Synopsis:    SHORT MaintMldMnemoRead( PARAMLDMNEMO *pData )
*               
*     Input:    *pData          : ponter to structure for PARAMLDMNEMO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS         : successful
*               LDT_KEYOVER_ADR : wrong data 
*
** Description: This function reads & displays MLDACTION MNEMONICS, P 65.
*               This is a Program Mode command, P 65.
*===========================================================================
*/

SHORT MaintMldMnemoRead( PARAMLDMNEMO *pData )
{
    USHORT  usAddress;
    SHORT   sReturn;
	MLDIF   MldIf = {0};

    /* initialize */
    memset(MaintWork.MldMnemo.aszMnemonics, '\0', sizeof(MaintWork.MldMnemo.aszMnemonics));

    /* check status */
    if(pData->uchStatus & MAINT_WITHOUT_DATA) {                 /* without data */
        usAddress = ++MaintWork.MldMnemo.usAddress;

        /* check address */
        if (usAddress > MAX_MLDMNEMO_SIZE) {                      
            usAddress = 1;                                     /* initialize address */
        }
    } else {                                                    /* with data */
        usAddress = pData->usAddress;
    }        

    /* check address */
    if (usAddress < 1 || MAX_MLDMNEMO_SIZE < usAddress) {
        return(LDT_KEYOVER_ADR);                                /* wrong data */
    }
    MldIf.usAddress = usAddress;
    sReturn = CliOpMldIndRead(&MldIf, usMaintLockHnd);
    if (sReturn == OP_PERFORM){
        _tcsncpy(MaintWork.MldMnemo.aszMnemonics, MldIf.aszMnemonics, PARA_MLDMNEMO_LEN);
    }
    MaintWork.MldMnemo.uchMajorClass = pData->uchMajorClass;  /* copy major class */
    MaintWork.MldMnemo.usAddress = usAddress;
    DispWrite(&(MaintWork.MldMnemo));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    SHORT MaintMldMnemoWrite( PARAMLDMNEMO *pData )
*               
*     Input:    *pData         : pointer to structure for PARAMLDMNEMO
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*
** Description: This function reads & displays MLDACTION MNEMONICS, P 65.
*               This is a Program Mode command, P 65.
*===========================================================================
*/

SHORT MaintMldMnemoWrite( PARAMLDMNEMO *pData )
{
	MLDIF   MldIf = {0};
    SHORT   sReturn;

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* without data */
        return(LDT_SEQERR_ADR);                                 /* sequence error */
    }

    _tcsncpy(MaintWork.MldMnemo.aszMnemonics, pData->aszMnemonics, PARA_MLDMNEMO_LEN);
	MaintWork.MldMnemo.aszMnemonics[PARA_MLDMNEMO_LEN] = 0;

    MldIf.usAddress = MaintWork.MldMnemo.usAddress;
    _tcsncpy(MldIf.aszMnemonics, pData->aszMnemonics, PARA_MLDMNEMO_LEN);
    sReturn = SerOpMldAssign(&MldIf, usMaintLockHnd);
    if (sReturn != OP_PERFORM) {
        return(OpConvertError(sReturn));
    }
    /* control header item */
	MaintHeaderCtl(PG_MLD_MNEMO, RPT_PRG_ADR);

    /* set journal bit & receipt bit */
    MaintWork.MldMnemo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
    PrtPrintItem(NULL, &(MaintWork.MldMnemo));

    /* set address for Display next address */
    MaintWork.MldMnemo.usAddress++;
    if (MaintWork.MldMnemo.usAddress > MAX_MLDMNEMO_SIZE) {
        MaintWork.MldMnemo.usAddress = 1;                    /* initialize address */
    }
    MaintMldMnemoRead(&(MaintWork.MldMnemo));
    return(SUCCESS);
}

/*
*===========================================================================
** Synopsis:    VOID MaintMldMnemoReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      Nothing
*
** Description: This function reports & prints MLDACTION MNEMONICS report, P 65.
*               This is a Program Mode command, P 65.
*===========================================================================
*/

VOID MaintMldMnemoReport( VOID )
{
    USHORT        usLoop;

    /* control header */
    MaintHeaderCtl(PG_MLD_MNEMO, RPT_PRG_ADR);

    /* set data at every address */
    for (usLoop = 1; usLoop <= MAX_MLDMNEMO_SIZE; usLoop++) {
		MLDIF         MldIf = {0};
		PARAMLDMNEMO  ParaMldMnemo = {0};
		SHORT         sReturn;

        /* check abort key */
        if (UieReadAbortKey() == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        MldIf.usAddress = usLoop;
        sReturn = CliOpMldIndRead(&MldIf, 0);

		ParaMldMnemo.uchMajorClass = CLASS_PARAMLDMNEMO;
        ParaMldMnemo.usAddress = usLoop;
		ParaMldMnemo.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
        if (sReturn == OP_PERFORM){
            _tcsncpy(ParaMldMnemo.aszMnemonics, MldIf.aszMnemonics, PARA_MLDMNEMO_LEN);
        }
        PrtPrintItem(NULL, &ParaMldMnemo);
    }

    /* make trailer */
    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTPRG);
}
