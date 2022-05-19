/*
*---------------------------------------------------------------------------
*  Georgia Southern University, Rsearch Services and Sponsored Programs
*    (C) Copyright 2002 - 2020
*
*  NHPOS, donated by NCR Corp to Georgia Southern University, August, 2002.
*  Developemnt with NCR 7448 then ported to Windows XP and generic x86 hardware
*  along with touch screen support.
*
*---------------------------------------------------------------------------
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
* Title       : Print Modifier                 
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRIModT.c                                                      
* --------------------------------------------------------------------------
* Abstract:  The provided function names are as follows:
*
*   PrtItemModifier()   : print item modfier
*   PrtModTable_TH()    : print table no./unique trans # (receipt & journal)
*   PrtModTable_EJ()    : print table no./unique trans # (receipt & journal)
*   PrtModTable_SP()    : print table no./unique trans # (slip)
*   PrtModPerson_TH()   : print no. of person (receipt & journal)
*   PrtModPerson_EJ()   : print no. of person (slip)
*   PrtModPerson_SP()   : print no. of person (slip)
*   PrtModCustName_TH() : print no. of person (receipt & journal)
*   PrtModCustName_EJ() : print no. of person (slip)
*   PrtModCustName_SP() : print no. of person (slip)
*   PrtDispModifier()   : display item modfier
*   PrtDflModTable()    : display table no.
*   PrtDflModPerson()   : display no. of person
*   PrtDflModCustName() : display customer name
*   PrtDispModifierForm()   : display item modfier
*   PrtDflModTableForm()    : display table no.
*   PrtDflModPersonForm()   : display no. of person
*   PrtDflModCustNameForm() : display customer name
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-14-93 : 00.00.01 : R.Itoh     :                                    
* Jul-14-93 : 01.00.12 : R.Itoh     : add PrtDispModifier(), PrtDflModTable(),                                   
*           :          :            : PrtDflModPerson()                                   
* Oct-29-93 : 02.00.02 : K.You      : del. PrtDispModifier(), PrtDflModTable(),                                   
*           :          :            : PrtDflModPerson()                                   
* Jan-26-95 : 03.00.00 : M.Ozawa    : recover PrtDispModifier(), PrtDflModTable(),                                   
*           :          :            : PrtDflModPerson()                                   
* Jul-21-95 : 03.00.04 : T.Nakahata : FVT Comment (Trans# Mnemo 4 => 12Char)
*           :          :            : add customer name (16 chara)
* Dec-15-99 : 00.00.01 : M.Ozawa    : add PrtDispModifierForm(), PrtDflModTableForm(),                                   
*           :          :            : PrtDflModPersonForm()                                   
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
/**------- M S - C -------**/
#include	<tchar.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <paraequ.h>
/* #include <para.h> */
#include <regstrct.h>
#include <transact.h>
#include <pmg.h>
#include <dfl.h>
#include "prtrin.h"
#include "prtdfl.h"
#include "prrcolm_.h"
#include <rfl.h>

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s 
;========================================================================
**/
/*
*===========================================================================
** Format  : VOID  PrtItemModifier(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMODIFIER     *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints item modfier.
*===========================================================================
*/
VOID  PrtItemModifier(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem)
{
    SHORT   sWidthType;

    /* --- if current system uses unique transaction no.,
        print it in double width character. --- */
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        sWidthType = PRT_DOUBLE;
    } else {
        sWidthType = PRT_SINGLE;
    }

    /* -- set print portion to static area "fsPrtPrintPort" -- */
    PrtPortion(pItem->fsPrintStatus);

    switch (pItem->uchMinorClass) {

    case CLASS_TABLENO:
        if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
            PrtModTable_SP(pTran, pItem, sWidthType);
        }

        if ( fsPrtPrintPort & PRT_RECEIPT ) {  /* thermal print */
            PrtModTable_TH(pTran, pItem, sWidthType);
        }

        if ( fsPrtPrintPort & PRT_JOURNAL ) {  /* electric journal */
            PrtModTable_EJ(pItem, sWidthType);
        }
        break;

    case CLASS_NOPERSON:
        if ( fsPrtPrintPort & PRT_SLIP ) {     /* slip print */
            PrtModPerson_SP(pTran, pItem, sWidthType);
        }

        if ( fsPrtPrintPort & PRT_RECEIPT ) {  /* thermal print */
            PrtModPerson_TH(pTran, pItem, sWidthType);
        }

        if ( fsPrtPrintPort & PRT_JOURNAL ) {  /* electric journal */
            PrtModPerson_EJ(pItem, sWidthType);
        }
        break;

    case CLASS_ALPHATENT:
    case CLASS_ALPHANAME:
        if ( fsPrtPrintPort & PRT_SLIP ) {
            PrtModCustName_SP( pTran, pItem );
        }

        if ( fsPrtPrintPort & PRT_RECEIPT ) {
            PrtModCustName_TH( pTran, pItem );
        }

        if ( fsPrtPrintPort & PRT_JOURNAL ) {
            PrtModCustName_EJ( pItem );
        }
        break;

    case CLASS_SEATNO:
        if ( fsPrtPrintPort & ( PRT_RECEIPT | PRT_JOURNAL )) {
            PrtTHHead(pTran);                   /* print header if necessary */
            PrtTHSeatNo( pItem->uchLineNo);
        }
        break;

    default:
        break; 

    }
}

/*
*===========================================================================
** Format  : VOID  PrtModTable_TH(TRANINFORMATION *pTran,
*                                 ITEMMODIFIER *pItem,
*                                 SHORT sWidthType )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMODIFIER     *pItem     -Item Data address
*            SHORT            sWidthType - Type of Print Width
*                   PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints table no. (thermal)
*===========================================================================
*/
VOID  PrtModTable_TH(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType)
{
    PrtTHHead(pTran);                      /* print header if necessarry */

    PrtTHTblPerson(pItem->usTableNo, 0, sWidthType);/* print table mnemo. and number, "0" : not print no of person */
}

/*
*===========================================================================
** Format  : VOID  PrtModTable_EJ(ITEMMODIFIER *pItem, SHORT sWidthType );      
*
*   Input  : ITEMMODIFIER     *pItem     -Item Data address
*            SHORT  sWidthType - Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints table no. (electric journal)
*===========================================================================
*/
VOID  PrtModTable_EJ(ITEMMODIFIER *pItem, SHORT sWidthType)
{
    PrtEJTblPerson(pItem->usTableNo, 0, 0, sWidthType);   /* print table mnemo. and number, "0" : not print no of person */
}

/*
*===========================================================================
** Format  : VOID  PrtModTable_SP(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem,
*                                   SHORT sWidthType
*
*   Input  : TRANINFORMATION  *pTran     - Transaction Data address
*          : ITEMMODIFIER     *pItem     - Item Data address
*            SHORT  sWidthType  -   Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints table no. (slip)
*===========================================================================
*/
VOID PrtModTable_SP(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem, SHORT sWidthType)
{
    TCHAR   aszSPPrintBuff[PRT_SPCOLUMN + 1];   /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines */

    /* -- initialize the area -- */
    memset(aszSPPrintBuff, '\0', sizeof(aszSPPrintBuff));

    /* -- set table No. and No. of person -- */
    usSlipLine += PrtSPTblPerson(aszSPPrintBuff, pItem->usTableNo, 0, sWidthType);

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* if just after header printed, not print table No., No. of person */  
    if (usSaveLine == 0) {
        /* -- print the data in the buffer -- */ 
        if (usSlipLine != 0) {
            PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff, PRT_SPCOLUMN);
        }
    }

    /* update current line No. */
    usPrtSlipPageLine += usSlipLine + usSaveLine;    
}

/*
*===========================================================================
** Format  : VOID  PrtModPerson_TH(TRANINFORMATION *pTran, ITEMMODIFIER *pItem,
*                                   SHORT sWidthType);      
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*   Input  : ITEMMODIFIER     *pItem     -Item Data address
*            SHORT  sWidthType  -   Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints no. of person. (thermal)
*===========================================================================
*/
VOID  PrtModPerson_TH(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType)
{
    PrtTHHead(pTran);                      /* print header if necessarry */

    PrtTHTblPerson(0, pItem->usNoPerson, sWidthType);  /* print no of person,  "0" : not print table no */
}

/*
*===========================================================================
** Format  : VOID  PrtModPerson_EJ(ITEMMODIFIER *pItem, SHORT sWidthType)
*
*   Input  : ITEMMODIFIER     *pItem     -Item Data address
*            SHORT  sWidthType  -   Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints no. of person. (electric journal)
*===========================================================================
*/
VOID  PrtModPerson_EJ(ITEMMODIFIER *pItem, SHORT sWidthType )
{
    PrtEJTblPerson(0, pItem->usNoPerson, 0, sWidthType);  /* print no of person,  "0" : not print table no */
}

/*
*===========================================================================
** Format  : VOID  PrtModPerson_SP(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem,
*                                   SHORT sWidthType)
*
*   Input  : TRANINFORMATION  *pTran 
*          : ITEMMODIFIER     *pItem     -Item Data address
*            SHORT  sWidthType  -   Type of Print Width
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints no. of person. (slip)
*===========================================================================
*/
VOID PrtModPerson_SP(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem, SHORT sWidthType)
{
    TCHAR  aszSPPrintBuff[PRT_SPCOLUMN + 1];   /* print data save area */
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines to be added */


    /* -- initialize the area -- */
    memset(aszSPPrintBuff, '\0', sizeof(aszSPPrintBuff));

    /* -- set table No. and No. of person -- */
    usSlipLine += PrtSPTblPerson(aszSPPrintBuff, 0, pItem->usNoPerson, sWidthType);

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine(usSlipLine, pTran);

    /* if just after header printed, not print table No., No. of person */  
    if (usSaveLine == 0) {
        /* -- print the data in the buffer -- */ 
        if (usSlipLine != 0) {
            PrtPrint(PMG_PRT_SLIP, aszSPPrintBuff, PRT_SPCOLUMN);
        }
    }

    /* -- update current line No. -- */
    usPrtSlipPageLine += usSlipLine + usSaveLine;    
}

/*
*===========================================================================
** Format  : VOID  PrtModCustName_TH( TRANINFORMATION *pTran,
                                      ITEMMODIFIER    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*   Input  : ITEMMODIFIER     *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints customer name. (thermal)
*===========================================================================
*/
VOID PrtModCustName_TH( TRANINFORMATION *pTran, ITEMMODIFIER *pItem )
{
    PrtTHHead( pTran );
    PrtTHCustomerName( pItem->aszName );
}

/*
*===========================================================================
** Format  : VOID PrtModCustName_EJ( ITEMMODIFIER *pItem )
*
*   Input  : ITEMMODIFIER   *pItem  - address of item modifier data
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints customer name on electric jounrnal.
*===========================================================================
*/
VOID PrtModCustName_EJ( ITEMMODIFIER *pItem )
{
    PrtEJCustomerName( pItem->aszName );
}

/*
*===========================================================================
** Format  : VOID PrtModCustName_SP( TRANINFORMATION *pTran,
*                                    ITEMMODIFIER    *pItem )
*
*   Input  : TRANINFORMATION *pTran - Transaction Data address
*          : ITEMMODIFIER    *pItem - Item Data address
*
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints customer name on slip.
*===========================================================================
*/
VOID PrtModCustName_SP( TRANINFORMATION *pTran, ITEMMODIFIER *pItem )
{
    TCHAR   szSPPrintBuff[ PRT_SPCOLUMN + 1 ];
    USHORT  usSlipLine = 0;            /* number of lines to be printed */
    USHORT  usSaveLine;                /* save slip lines */

    /* -- initialize the area -- */
    memset( szSPPrintBuff, '\0', sizeof( szSPPrintBuff ));

    usSlipLine += PrtSPCustomerName( szSPPrintBuff, pItem->aszName );

    /* -- check if paper change is necessary or not -- */ 
    usSaveLine = PrtCheckLine( usSlipLine, pTran );

    /* if just after header printed, not print table No., No. of person */  
    if (( usSaveLine == 0 ) && ( usSlipLine != 0 )) {
        PrtPrint( PMG_PRT_SLIP, szSPPrintBuff, PRT_SPCOLUMN );
    }
    usPrtSlipPageLine += ( usSlipLine + usSaveLine );
}

/*
*===========================================================================
** Format  : VOID  PrtDispModifier(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMODIFIER     *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints item modfier.
*===========================================================================
*/
VOID  PrtDispModifier(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem )
{
    SHORT   sWidthType;

    /* --- if current system uses unique transaction no.,
        print it in double width character. --- */
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        sWidthType = PRT_DOUBLE;
    } else {
        sWidthType = PRT_SINGLE;
    }

    switch (pItem->uchMinorClass) {

    case CLASS_TABLENO:
        PrtDflModTable(pTran, pItem, sWidthType);
        break;

    case CLASS_NOPERSON:
        PrtDflModPerson(pTran, pItem, sWidthType);
        break;

    case CLASS_ALPHATENT:
    case CLASS_ALPHANAME:
        PrtDflModCustName( pTran, pItem );
        break;

    default:
        break; 

    }
}

/*
*===========================================================================
** Format  : VOID  PrtDflModTable(TRANINFORMATION *pTran, ITEMMODIFIER *pItem
*                                   SHORT sWidthType)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMODIFIER     *pItem     -Item Data address
*            SHORT  sWidthType  -   Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays table no. 
*===========================================================================
*/
VOID  PrtDflModTable(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType)
{

    TCHAR  aszDflBuff[5][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflTblPerson(aszDflBuff[usLineNo], pItem->usTableNo, 0, sWidthType);

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_MODIFIER); 

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData(aszDflBuff[i], &usOffset);
        if ( aszDflBuff[i][PRT_DFL_LINE] != '\0' ) {
            i++;
        }    
    }

    /* -- send display data -- */
    PrtDflSend();

}

/*
*===========================================================================
** Format  : VOID  PrtDflModPerson(TRANINFORMATION *pTran, ITEMMODIFIER *pItem,
*                                   SHORT sWidthType)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMODIFIER     *pItem     -Item Data address
*            SHORT  sWidthType  -   Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays no. of person.
*===========================================================================
*/
VOID  PrtDflModPerson(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType)
{

    TCHAR  aszDflBuff[5][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo;                       /* number of lines to be displayed */
    USHORT  usOffset = 0;                       
    USHORT  i;                       

    /* --- if this frame is 1st frame, display customer name --- */

    PrtDflCustHeader( pTran );

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));

    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);

    /* -- set item data -- */
    usLineNo += PrtDflTblPerson(aszDflBuff[usLineNo], 0, pItem->usNoPerson, sWidthType); 

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */ 
    PrtDflIType(usLineNo, DFL_MODIFIER); 

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData(aszDflBuff[i], &usOffset);
        if ( aszDflBuff[i][PRT_DFL_LINE] != '\0' ) {
            i++;
        }    
    }

    /* -- send display data -- */
    PrtDflSend();
}

/*
*===========================================================================
** Format  : VOID  PrtDflModCustName( TRANINFORMATION *pTran,
*                                     ITEMMODIFIER    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMODIFIER     *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays customer name.
*===========================================================================
*/
VOID PrtDflModCustName( TRANINFORMATION *pTran, ITEMMODIFIER *pItem )
{
    TCHAR   aszDflBuff[ 6 ][ PRT_DFL_LINE + 1 ];
    USHORT  usLineNo;               /* number of lines to be displayed */
    USHORT  usOffset = 0;
    USHORT  i;

    if ( pItem->aszName[0] == '\0' ) {
        return;
    }

    memset( aszDflBuff, 0x00, sizeof( aszDflBuff ));

    /* -- set header -- */
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );

    /* -- set item data -- */
    usLineNo += PrtDflCustName( aszDflBuff[ usLineNo ], pItem->aszName );

    /* -- set destination CRT -- */
    PrtDflIf.Dfl.DflHead.auchCRTNo[0] = 0x30;
    PrtDflIf.Dfl.DflHead.auchCRTNo[1] = 0x30;

    /* -- set display data in the buffer -- */ 
    PrtDflIType( usLineNo, DFL_CUSTNAME );

    for ( i = 0; i < usLineNo; i++ ) {
        PrtDflSetData( aszDflBuff[ i ], &usOffset );
        if ( aszDflBuff[ i ][ PRT_DFL_LINE ] != '\0' ) {
            i++;
        }        
    }

    /* -- send display data -- */
    PrtDflSend();
}

/*
*===========================================================================
** Format  : VOID  PrtDispModifier(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMODIFIER     *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function prints item modfier.
*===========================================================================
*/
USHORT  PrtDispModifierForm(TRANINFORMATION  *pTran, ITEMMODIFIER *pItem, TCHAR *puchBuffer)
{
    SHORT   sWidthType;
    USHORT usLine;

    /* --- if current system uses unique transaction no.,
        print it in double width character. --- */
    if ( pTran->TranCurQual.flPrintStatus & CURQUAL_UNIQUE_TRANS_NO ) {
        sWidthType = PRT_DOUBLE;
    } else {
        sWidthType = PRT_SINGLE;
    }

    switch (pItem->uchMinorClass) {

    case CLASS_TABLENO:
        usLine = PrtDflModTableForm(pTran, pItem, sWidthType, puchBuffer);
        break;

    case CLASS_NOPERSON:
        usLine = PrtDflModPersonForm(pTran, pItem, sWidthType, puchBuffer);
        break;

    case CLASS_ALPHATENT:
    case CLASS_ALPHANAME:
        usLine = PrtDflModCustNameForm( pTran, pItem, puchBuffer);
        break;

    case CLASS_TAXMODIFIER:
        usLine = PrtDflModTaxForm( pTran, pItem, puchBuffer);
        break;

    case CLASS_SEATNO:
        usLine = PrtDflModSeatForm( pTran, pItem, puchBuffer);
        break;

    default:
        usLine = 0;
        break; 

    }
    
    return usLine;
}

/*
*===========================================================================
** Format  : VOID  PrtDflModTable(TRANINFORMATION *pTran, ITEMMODIFIER *pItem
*                                   SHORT sWidthType)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMODIFIER     *pItem     -Item Data address
*            SHORT  sWidthType  -   Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays table no. 
*===========================================================================
*/
USHORT  PrtDflModTableForm(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType, TCHAR *puchBuffer)
{
    TCHAR  aszDflBuff[5][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflTblPerson(aszDflBuff[usLineNo], pItem->usTableNo, 0, sWidthType);

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;

}

/*
*===========================================================================
** Format  : VOID  PrtDflModPerson(TRANINFORMATION *pTran, ITEMMODIFIER *pItem,
*                                   SHORT sWidthType)
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMODIFIER     *pItem     -Item Data address
*            SHORT  sWidthType  -   Type of Print Width
*                       PRT_DOUBLE, PRT_SINGLE
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays no. of person.
*===========================================================================
*/
USHORT  PrtDflModPersonForm(TRANINFORMATION *pTran, ITEMMODIFIER *pItem, SHORT sWidthType, TCHAR *puchBuffer)
{

    TCHAR  aszDflBuff[5][PRT_DFL_LINE + 1];   /* display data save area */
    USHORT  usLineNo=0, i;                       /* number of lines to be displayed */

    /* --- if this frame is 1st frame, display customer name --- */
    PrtDflCustHeader( pTran );

    memset(aszDflBuff, '\0', sizeof(aszDflBuff));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader(aszDflBuff[0], pTran);

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer(aszDflBuff[usLineNo], pTran, pTran->TranCurQual.ulStoreregNo);
#endif
    /* -- set item data -- */
    usLineNo += PrtDflTblPerson(aszDflBuff[usLineNo], 0, pItem->usNoPerson, sWidthType); 

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;

}

/*
*===========================================================================
** Format  : VOID  PrtDflModCustName( TRANINFORMATION *pTran,
*                                     ITEMMODIFIER    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMODIFIER     *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays customer name.
*===========================================================================
*/
USHORT PrtDflModCustNameForm( TRANINFORMATION *pTran, ITEMMODIFIER *pItem, TCHAR *puchBuffer )
{
    USHORT  usLineNo=0, i;               /* number of lines to be displayed */
	TCHAR   aszDflBuff[6][PRT_DFL_LINE + 1] = { 0 };
	TCHAR  aszTranMnem[PARA_TRANSMNEMO_LEN + 1] = { 0 };

    if ( pItem->aszName[0] == '\0' ) {
        return 0;
    }

	//Checks to see if a receipt id was entered, and stores
	//the receipt id mnemonic in aszDflBuff  ***PDINU
	if ( pTran->TranModeQual.aszTent[0] > 0 ){
		RflGetTranMnem(aszTranMnem, TRN_TENT);
		usLineNo += PrtDflCustName( aszDflBuff[ usLineNo ], aszTranMnem );
	}

#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );
#endif
    /* -- set item data -- */
    usLineNo += PrtDflCustName( aszDflBuff[ usLineNo ], pItem->aszName );

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
	//Changed from _tcsncpy to memcpy becasue it is processing two lines
	//of data rather than one.  ***PDINU
    //_tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    memcpy(puchBuffer, aszDflBuff, (usLineNo*(PRT_DFL_LINE+1)*sizeof(TCHAR)));
    
    return usLineNo;
}

/*
*===========================================================================
** Format  : VOID  PrtDflModTaxName( TRANINFORMATION *pTran,
*                                     ITEMMODIFIER    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMODIFIER     *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tax modifier, 2172
*===========================================================================
*/
USHORT PrtDflModTaxForm( TRANINFORMATION *pTran, ITEMMODIFIER *pItem, TCHAR *puchBuffer )
{
    TCHAR   aszDflBuff[ 6 ][ PRT_DFL_LINE + 1 ];
    USHORT  usLineNo=0, i;               /* number of lines to be displayed */

    if ( pItem->uchLineNo == 0 ) {
        return 0;
    }

    memset( aszDflBuff, 0x00, sizeof( aszDflBuff ));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );
#endif
    /* -- set item data -- */
    usLineNo += PrtDflTaxMod2(aszDflBuff[usLineNo], pTran->TranModeQual.fsModeStatus, pItem->uchLineNo);

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    memcpy(puchBuffer, aszDflBuff, usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/*
*===========================================================================
** Format  : VOID  PrtDflModTaxName( TRANINFORMATION *pTran,
*                                     ITEMMODIFIER    *pItem )
*
*   Input  : TRANINFORMATION  *pTran     -Transaction Information address
*            ITEMMODIFIER     *pItem     -Item Data address
*   Output : none
*   InOut  : none
** Return  : none
*            
** Synopsis: This function displays tax modifier, 2172
*===========================================================================
*/
USHORT PrtDflModSeatForm( TRANINFORMATION *pTran, ITEMMODIFIER *pItem, TCHAR *puchBuffer )
{
    TCHAR   aszDflBuff[ 6 ][ PRT_DFL_LINE + 1 ];
    USHORT  usLineNo=0, i;               /* number of lines to be displayed */

    if ( pItem->uchLineNo == 0 ) {
        return 0;
    }

    memset( aszDflBuff, 0x00, sizeof( aszDflBuff ));
#if 0
    /* -- set header -- */
    usLineNo = PrtDflHeader( aszDflBuff[ 0 ], pTran );

    /* -- set trailer -- */
    usLineNo += PrtDflTrailer( aszDflBuff[ usLineNo ], pTran, pTran->TranCurQual.ulStoreregNo );
#endif
    /* -- set item data -- */
    usLineNo += PrtDflModSeatNo(aszDflBuff[usLineNo], pItem->uchLineNo);

    for (i=0; i<usLineNo; i++) {
        aszDflBuff[i][PRT_DFL_LINE] = PRT_RETURN;
    }
    _tcsncpy(puchBuffer, aszDflBuff[0], usLineNo*(PRT_DFL_LINE+1));
    
    return usLineNo;
}

/***** End Of Source *****/
