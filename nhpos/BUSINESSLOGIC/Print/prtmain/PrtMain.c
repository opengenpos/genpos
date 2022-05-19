/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 2000            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print Main (call Printer function(RJ/Thermal))
* Category    : Print Module, NCR 2170 US Hospitality  application
* Program Name: PrtMain.c
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract:
*   PrtTypeCheckRJ()        : check printer type
*   PrtPrintTran()          : call PrtPrintTran for Printer RJ or Thermal
*   PrtPrintDemand()        : call PrtPrintDemand for Printer RJ or Thermal
*   PrtPrintSplit()         : call PrtPrintSplit for Printer RJ or Thermal
*   PrtInitialize()         : call PrtInitialize for Printer RJ or Thermal
*   PrtPrintItem()          : call PrtPrintItem for Printer RJ or Thermal
*   PrtPrintCpEpt()         : call PrtPrintCpEpt for Printer RJ or Thermal
*   PrtDispItem()           : call PrtDispItem for Printer RJ or Thermal
*   PrtCheckTenderMedia()   : call PrtCheckTenderMedia for Printer RJ or Thermal
*   PrtGetStatus()          : call PrtGetStatus for Printer RJ or Thermal
*   PrtDispItemForm()       : call PrtDispItemForm for Printer RJ or Thermal
*   PrtCheckAbort()         : call PrtCheckAbort for Printer RJ or Thermal
*   PrtChangeFont()         : call PrtChangeFont for Printer RJ or Thermal
*   PrtShrPartialSend()     : call PrtShrPartialSend for Printer RJ or Thermal
*   PrtShrInitReverse()     : call PrtShrInitReverse for Printer RJ or Thermal
*   PrtShrEnd()             : call PrtShrEnd for Printer RJ or Thermal
*   PrtCheckShr()           : call PrtCheckShr for Printer RJ or Thermal
*   PrtShrTermLock()        : call PrtShrTermLock for Printer RJ or Thermal
*   PrtShrTermUnLock()      : call PrtShrTermUnLock for Printer RJ or Thermal
* --------------------------------------------------------------------------
* Update Histories
*  Date     : Ver.Rev. :   Name     : Description
* Jan-11-00 : 01.00.00 : H.Endo     : 
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
;========================================================================
;+                  I N C L U D E     F I L E s                         +
;========================================================================
**/
#include <tchar.h>

#include <ecr.h>
#include <regstrct.h>
#include <pif.h>
#include <log.h>
#include <appllog.h>
#include <paraequ.h>
#include <para.h>
#include <maint.h>
#include <transact.h>
#include <pmg.h>
#include <prt.h>


/**
;========================================================================
;+                    S T A T I C    R A M s
;========================================================================
**/
#if 0
static SHORT    sPrinterType = -1;
#endif
/*
*===========================================================================
** Format  : USHORT PrtTypeCheckRJ(VOID);
*
*    Input : none
*   Output : none
*    InOut : none
** Return  : USHORT    sPrinterType      if Printer is RJ return 1 
*                                        else return 0
*
** Synopsis: This function get printer information and check printer is RJ or not.
*===========================================================================
*/
USHORT PrtTypeCheckRJ(VOID)
{
    
    USHORT  usColumn = 0;
    USHORT  fbStatus;
    
    if (PmgPrtConfig(PMG_PRT_JOURNAL, &usColumn, &fbStatus) != PMG_SUCCESS) {
        
        usColumn = 0;   /* assume as thermal printer by default */
    }
    
    if (usColumn) { /* journal column exists */
        
        /* RJ printer */
        return 1;
        
    }else{
        
        /* Thermal printer */
        return 0;
    }
}

/*
*===========================================================================
** Format  : USHORT PrtPrintTran(TRANINFORMATION *pTran)
*
*   Input  : TRANINFORMATION *pTran,         -transaction information
*   Output : none
*   InOut  : none
** Return  : USHORT    sSlipLine         -current printed pages and lines
*
** Synopsis:call PrtPrintTran for Printer RJ or Thermal
*===========================================================================
*/
USHORT PrtPrintTran(TRANINFORMATION *pTran)
{
    return ThermPrtPrintTran(pTran);/* Call PrtPrintTran for Thermal(thermreg.lib) */
}

/*
*===========================================================================
** Format  : VOID PrtPrintDemand(TRANINFORMATION *pTran)
*
*   Input  : TRANINFORMATION *pTran,         -transaction information
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis:call PrtPrintDemand for Printer RJ or Thermal
*===========================================================================
*/
VOID PrtPrintDemand(TRANINFORMATION *pTran)
{
    ThermPrtPrintDemand(pTran);    /* Call PrtPrintDemand for Thermal(thermreg.lib)  */
}
VOID PrtPrintSummaryReceipt(TRANINFORMATION *pTran, ITEMTENDER *pItemTender, int iPrintItems)
{
    ThermPrtPrintSummaryReceipt(pTran, pItemTender, iPrintItems);    /* Call PrtPrintDemand for Thermal(thermreg.lib)  */
}
/*
*===========================================================================
** Format  : USHORT PrtPrintSplit(TRANINFORMATION *pTran, TRANINFSPLIT *pSplit);
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*          : TRANINFSPLIT     *pSplit    -Split GCF information address
*   Output : none
*   InOut  : none
** Return  : USHORT    sSlipLine         -current printed pages and lines
*
** Synopsis:call PrtPrintSplit for Printer RJ or Thermal
*===========================================================================
*/
USHORT PrtPrintSplit(TRANINFORMATION *pTran, TRANINFSPLIT *pSplit,
                                                USHORT usType, USHORT usNo)
{
    return ThermPrtPrintSplit(pTran,pSplit,usType,usNo);   /*Call PrtPrintSplit for Thermal(thermreg.lib)  */
}


/*
*===========================================================================
** Format  : VOID PrtInitialize(TRANINFORMATION *pTran, TRANINFSPLIT *pSplit);
*
*    Input : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis:call PrtInitialize for Printer RJ or Thermal
*===========================================================================
*/
VOID    PrtInitialize(VOID)
{
    ThermPrtInitialize();                  /* Call PrtInitialize for Thermal(thermreg.lib)  */
}

USHORT   PrtPrintLine(USHORT usPrtControl, TCHAR *pLineToPrint)
{
    return ThermPrtPrintLine (usPrtControl, pLineToPrint);  /* Call PrtPrintItem for  Thermal(thermreg.lib)  */
}

USHORT   PrtPrintLineImmediate(USHORT usPrtControl, TCHAR *pLineToPrint)
{
    return ThermPrtPrintLineImmediate (usPrtControl, pLineToPrint);  /* Call PrtPrintItem for  Thermal(thermreg.lib)  */
}


/*
*===========================================================================
** Format  : USHORT   PrtPrintItem(TRANINFORMATION *pTran, VOID *pItem);     
*
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            VOID             *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : USHORT  usPrtSlipPageLine   -current printed pages and lines       
*
** Synopsis:call PrtPrintItem for Printer RJ or Thermal
*===========================================================================
*/
USHORT   PrtPrintItem(TRANINFORMATION  *pTran, VOID *pItem)
{
    return ThermPrtPrintItem(pTran,pItem); /* Call PrtPrintItem for  Thermal(thermreg.lib)  */
}

/*
*===========================================================================
** Format  : USHORT   PrtPrintCpEpt(TRANINFORMATION *pTran, VOID *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            VOID             *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : USHORT  usPrtSlipPageLine   -current printed pages and lines       
*
** Synopsis:call PrtPrintCpEpt for Printer RJ or Thermal
*===========================================================================
*/
USHORT   PrtPrintCpEpt(TRANINFORMATION  *pTran, VOID *pItem)
{
    return ThermPrtPrintCpEpt(pTran,pItem);    /*Call PrtPrintCpEpt for Thermal(thermreg.lib)  */
}


/*
*===========================================================================
** Format  : VOID   PrtDispItem(TRANINFORMATION *pTran, VOID *pItem);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            VOID             *pItem     -Item Data address
*   Output : none
*    InOut : none
** Return  : USHORT  usPrtSlipPageLine   -current printed pages and lines       
*
** Synopsis: call PrtDispItem for Printer RJ or Thermal display on the fly.
*
*            entry point for the RS-232 display on the fly functionality when
*            MDC address 363, MDC_DSP_ON_FLY3, is zero or turned off. The alternative
*            is the kitchen display system (KDS) functionality which  flows through
*            a different area of source.
*===========================================================================
*/
VOID   PrtDispItem(TRANINFORMATION  *pTran, VOID *pItem)
{
    ThermPrtDispItem(pTran,pItem);         /* Call PrtDispItem for Thermal(thermreg.lib)  */
}
/*
*===========================================================================
** Format  : USHORT   PrtCheckTenderMedia(ITEMTENDER *pItem);      
*               
*    Input : ITEMTENDER     *pItem     -Item Data address
*   Output : none
*    InOut : none
*
** Return  : PRT_TEND_ROOM      : room charge tender
*            PRT_TEND_ACCT      : acct. charge tender
*            PRT_TEND_CREAUTH   : credit authorization tender
*            PRT_TEND_EPT       : EPT tender
*            PRT_TEND_OTHER     : other tender
*
** Synopsis:call PrtCheckTenderMedia for Printer RJ or Thermal
*===========================================================================
*/
USHORT   PrtCheckTenderMedia(ITEMTENDER *pItem)
{
    return ThermPrtCheckTenderMedia(pItem);    /*Call PrtCheckTenderMedia for Thermal(thermreg.lib)  */
}

/*
*===========================================================================
** Format  : USHORT   PrtGetStatus(VOID);      
*               
*    Input : none
*   Output : none
*    InOut : none
** Return  : PRT_THERMAL_PRINTER    
*
** Synopsis:call PrtGetStatus for Printer RJ or Thermal
*===========================================================================
*/
USHORT   PrtGetStatus(VOID)
{
    return ThermPrtGetStatus();            /* Call PrtGetStatus for Thermal(thermreg.lib)  */
}

/*
*===========================================================================
** Format  : USHORT   PrtDispItemForm(TRANINFORMATION *pTran, VOID *pItem,UCHAR *puchBuffer);      
*               
*    Input : TRANINFORMATION  *pTran     -Transaction information address
*            VOID             *pItem     -Item Data address
*            UCHAR            *puchBuffer
*   Output : none
*    InOut : none
** Return  : USHORT  usPrtSlipPageLine   -current printed pages and lines       
*
** Synopsis:call PrtDispItemForm for Printer RJ or Thermal
*===========================================================================
*/
USHORT   PrtDispItemForm(TRANINFORMATION  *pTran, VOID *pItem, TCHAR *puchBuffer)
{
    return ThermPrtDispItemForm(pTran, pItem, puchBuffer); /* Call PrtDispItemForm for Thermal(thermreg.lib)  */
}


/*
*===========================================================================
** Format  : SHORT  PrtCheckAbort(VOID);      
*
*   InPut  : none         
*   Output : none
*   InOut  : none
** Return  : UCHAR  PRT_ABORTED:   -abort key entered when error occured
*                             0:   -abort key not entered            
*
** Synopsis:call PrtCheckAbort for Printer RJ or Thermal
*===========================================================================
*/
SHORT  PrtCheckAbort(VOID)
{
    return ThermPrtCheckAbort();           /* Call PrtCheckAbort for Thermal(thermreg.lib)  */
}

/*
*===========================================================================
*   VOID     PrtChangeFont(USHORT usType);
*
*   Input  : PRT_FONT_A                         - 12X24   defined in "pmg.h" 
*            PRT_FONT_B                         -  9X 7   defined in "pmg.h" 
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis:call PrtChangeFont for Printer RJ or Thermal
*===========================================================================
*/
VOID PrtChangeFont(USHORT usType)
{
    ThermPrtChangeFont(usType);            /* Call PrtChangeFont for Thermal(thermreg.lib)  */
}

/*
*===========================================================================
** Format  : VOID  PrtShrPartialSend(VOID);      
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none       
*
** Synopsis:call PrtShrPartialSend for Printer RJ or Thermal
*===========================================================================
*/
VOID    PrtShrPartialSend(USHORT  usPrintControl)
{
	if (usPrintControl & PRT_RECEIPT) {   // Only do shared printing if report is to go to a receipt printer
		ThermPrtShrPartialSend();              /* Call PrtShrPartialSend for Thermal(thermreg.lib)  */
	}
	return;
}


/*
*===========================================================================
** Format  : VOID  PrtShrInitReverse( VOID );      
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis:call PrtShrInitReverse for Printer RJ or Thermal
*===========================================================================
*/
VOID    PrtShrInitReverse( VOID )
{
    ThermPrtShrInitReverse();              /* Call PrtShrInitReverse for Thermal(thermreg.lib)  */
}

/*
*===========================================================================
** Format  : VOID  PrtShrEnd(VOID);      
*
*   Input  : none
*   Output : none
*   InOut  : none
** Return  : none
*
** Synopsis:call PrtShrEnd for Printer RJ or Thermal
*===========================================================================
*/
VOID    PrtShrEnd(VOID)
{
    ThermPrtShrEnd();                      /* Call PrtShrEnd for Thermal(thermreg.lib)  */
}
/*
*===========================================================================
** Format  : UCHAR  PrtCheckShr(UCHAR   uchTrgPrt);      
*
*   Input  : UCHAR  uchTrgPrt    -target printer
*                     - PRT_WITHSHR     go to shared printer
*                     - PRT_WITHALTR    go to alternative printer
*            
*   Output : none
*   InOut  : none
** Return  : UCHAR  ParaBuffShr.uchTermNo:   -target terminal address
**           
*
** Synopsis:call PrtCheckShr for Printer RJ or Thermal
*===========================================================================
*/
UCHAR    PrtCheckShr(UCHAR  uchTrgPrt)
{
    return ThermPrtCheckShr(uchTrgPrt);    /* Call PrtCheckShr for Thermal(thermreg.lib)  */
}
/*
*===========================================================================
** Synopsis:    SHORT PrtShrTermLock(UCHART uchPrtStatus)
*               
*     Input:    uchPrtStatus : 
*    Output:    Nothing 
*     InOut:    Nothing           
*
** Return:      SUCCESS      : success terminal lock
*               FSC_CANCEL   : stop retry for lock
*               FSC_P5       : stop retry for lock
*
** Synopsis:call PrtShrTermLock for Printer RJ or Thermal
*===========================================================================
*/

SHORT PrtShrTermLock(UCHAR uchPrtStatus)
{
    return ThermPrtShrTermLock(uchPrtStatus);  /* Call PrtShrTermLock for Thermal(thermreg.lib)  */
}
/*
*===========================================================================
** Synopsis:    VOID PrtShrTermUnLock( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing           
*
** Return:      Nothing
*
** Synopsis:call PrtShrTermUnLock for Printer RJ or Thermal
*===========================================================================
*/
VOID PrtShrTermUnLock( VOID )
{
    ThermPrtShrTermUnLock();               /* Call PrtShrTermUnLock for Thermal(thermreg.lib)  */
}
/****** End Of Source ******/


