/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1998            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Parameter Read/Write Entry Module                         
* Category    : User Interface For Supervisor, NCR 2170 System Application         
* Program Name: PARA.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               ParaRead() :
*               ParaWrite() : 
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date   : Ver.Rev. :  Name    : Description
* May-07-92 : 00.00.01 : J.Ikeda  : initial                                   
* Nov-27-92 : 01.00.00 : J.Ikeda  : Adds CLASS_PARATARE                                   
* Jun-23-93 : 01.00.12 : J.IKEDA  : Adds CLASS_PARASHRPRT
* Jul-05-93 : 01.00.12 : J.IKEDA  : Adds CLASS_PARAPRESETAMOUNT
* Jul-09-93 : 01.00.12 : K.You    : Adds CLASS_PARAPIGRULE
* Aug-23-93 : 01.00.13 : J.IKEDA  : Add CLASS_PARASOFTCHK, Del CLASS_PARA_CHAR44
* Nov-05-93 :          : H.Yama   : Delete TAXTBL4 and PIG
* Nov-12-93 : 02.00.02 : K.You    : Add CLASS_HOTELID
* Aug-04-98 : 03.03.00 : hrkato   : V3.3 (Fast Finalize Improvement)
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
/*
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
*/

#include	<tchar.h>
#include <string.h>
#include <ecr.h>
#include <plu.h>
#include <paraequ.h>
#include <para.h>
#include <pararam.h>
#include <rfl.h>

ULONG  ParaGenerateHash (VOID)
{
	ULONG  ulLen = (CHAR *)&Para.ParaSpcCo[0] - (CHAR *)&Para;
	ULONG  ulHash = 0;

	ulHash = RflMemHash (&Para, ulLen, ulHash);

	ulLen = (CHAR *)&usSerCurStatus - (CHAR *)&ParaAutoCoupon;
	ulHash = RflMemHash (&ParaAutoCoupon, ulLen, ulHash);

	return ulHash;
}

/*
*=============================================================================
**  Synopsis: VOID ParaRead(VOID *pData) 
*               
*       Input:  *pData  
*      Output:  nothing
*
**  Return: nothing 
*               
**  Descruption:  
*===============================================================================

*/
VOID ParaRead(VOID *pData) 
{
    UCHAR       uchMajor;

    /* distinguish data class */
    uchMajor = *(( UCHAR*)pData);

    switch(uchMajor) {
    case CLASS_PARAPLUNOMENU:   
        ParaPLUNoMenuRead((PARAPLUNOMENU *)pData);          /* read PLU No.of appointed address */
        break;                                              

    case CLASS_PARAROUNDTBL:    
        ParaRoundRead((PARAROUNDTBL *)pData);               /* read rounding data */
        break;

    case CLASS_PARADISCTBL:
        ParaDiscRead(( PARADISCTBL *)pData);                /* read discount/bonus rate */
        break;

    case CLASS_PARAPROMOTION:             
        ParaPromotionRead((PARAPROMOTION *)pData);          /* read promotion sales header */
        break;

    case CLASS_PARACURRENCYTBL:           
        ParaCurrencyRead((PARACURRENCYTBL *)pData);         /* read foreign currency rate */
        break;

    case CLASS_PARAMENUPLUTBL:                              /* SHORT */
        ParaMenuPLURead((PARAMENUPLUTBL *)pData);           /* read set menu of PLU */
        break;

    case CLASS_PARATAXRATETBL:
        ParaTaxRateRead((PARATAXRATETBL *)pData);              /* read tax rate */
        break;

    case CLASS_PARAMISCPARA:
        ParaMiscParaRead((PARAMISCPARA *)pData);            /* read transaction limit */
        break;

    case CLASS_PARASTOREFORWARD:
        ParaStoreForwardParaRead((PARAMISCPARA *)pData);            /* read transaction limit */
        break;

    case CLASS_PARATERMINALINFO:
        ParaTerminalInfoParaRead((PARATERMINALINFO *)pData);  /* read terminal information */
        break;

    case CLASS_PARAMDC:
        ParaMDCRead((PARAMDC *)pData);                      /* read MDC data of appointed address */
        break;

    case CLASS_PARAFSC:
        ParaFSCRead((PARAFSC *)pData);                      /* read FSC data of appointed address */
        break;

    case CLASS_PARASECURITYNO:
        ParaSecurityNoRead((PARASECURITYNO *)pData);        /* read security No. */
        break;
    
    case CLASS_PARASUPLEVEL:                                /* SHORT */
        ParaSupLevelRead((PARASUPLEVEL *)pData);            /* read supervisor no. & level */
        break;

    case CLASS_PARAACTCODESEC:
        ParaActCodeSecRead((PARAACTCODESEC *)pData);        /* read action code security of appointed address */
        break;

    case CLASS_PARATRANSHALO:
        ParaTransHALORead((PARATRANSHALO *)pData);          /* read transaction halo */
        break;

    case CLASS_PARAHOURLYTIME:
        ParaHourlyTimeRead((PARAHOURLYTIME *)pData);        /* read hourly time data */
        break;

    case CLASS_PARASLIPFEEDCTL:
        ParaSlipFeedCtlRead((PARASLIPFEEDCTL *)pData);      /* read feed control data */
        break;

    case CLASS_PARATRANSMNEMO:
        ParaTransMnemoRead((PARATRANSMNEMO *)pData);        /* read transaction mnemonics of appointed address */
        break;

    case CLASS_PARALEADTHRU:
        ParaLeadThruRead((PARALEADTHRU *)pData);            /* read lead through of appointed address */
        break;

    case CLASS_PARAREPORTNAME:
        ParaReportNameRead((PARAREPORTNAME *)pData);        /* read report name of appointed address */
        break;

    case CLASS_PARASPECIALMNEMO:
        ParaSpeMnemoRead((PARASPEMNEMO *)pData);            /* read special mnemonics of appointed address */
        break;

    case CLASS_PARATAXTBL1:
    case CLASS_PARATAXTBL2:
    case CLASS_PARATAXTBL3:
    case CLASS_PARATAXTBL4:
        ParaTaxTableRead((PARATAXTBL *)pData);              /* read all tax table */
        break;

    case CLASS_PARAADJMNEMO:
        ParaAdjMnemoRead((PARAADJMNEMO *)pData);            /* read adjective mnemonics of appointed address */
        break;

    case CLASS_PARAPRTMODI:
        ParaPrtModiRead((PARAPRTMODI *)pData);              /* read print modifier mnemonics of appointed address */
        break;

    case CLASS_PARAMAJORDEPT:
        ParaMajorDEPTRead((PARAMAJORDEPT *)pData);          /* read major department mnemonics of appointed address */
        break;

    case CLASS_PARACHAR24:
        ParaChar24Read((PARACHAR24 *)pData);                /* read 24 character mnemonics of appointed address */
        break;

    case CLASS_PARACTLTBLMENU:
        ParaCtlTblMenuRead((PARACTLTBLMENU *)pData);        /* read control menu page no.*/
        break;

    case CLASS_PARAAUTOALTKITCH:
        ParaAutoAltKitchRead((PARAALTKITCH *)pData);        /* read kitchen printer alternative list */
        break;

    case CLASS_PARAFLEXMEM:
        ParaFlexMemRead((PARAFLEXMEM *)pData);              /* read a number of record & flag status of PTD */
        break;

    case CLASS_PARASTOREGNO:
        ParaStoRegNoRead((PARASTOREGNO *)pData);            /* read store/register no. */
        break;

    case CLASS_PARATTLKEYTYP:
        ParaTtlKeyTypRead((PARATTLKEYTYP *)pData);          /* read characteristic type of total key */
        break;

    case CLASS_PARATTLKEYCTL:
        ParaTtlKeyCtlRead((PARATTLKEYCTL *)pData);          /* read bit decide to characterize total key */
        break;

    case CLASS_PARATEND:
        ParaTendRead((PARATEND *)pData);                    /* read tender parameter,   V3.3 */
        break;

    case CLASS_PARAMANUALTKITCH:
        ParaManuAltKitchRead((PARAALTKITCH *)pData);        /* read kitchen printer alternative list */
        break;

    case CLASS_PARACASHABASSIGN:
        ParaCashABAssignRead((PARACASHABASSIGN *)pData );   /* read cashier no. of A/B key */
        break;

    case CLASS_PARASPCCO:
        ParaSpcCoRead((PARASPCCO *)pData);                  /* read special counter */
        break;

    case CLASS_PARAPCIF:
        ParaPCIFRead((PARAPCIF *)pData);
        break;

    case CLASS_PARATONECTL:
        ParaToneCtlRead((PARATONECTL *)pData);
        break;

    case CLASS_PARATARE:
        ParaTareRead(( PARATARE *)pData);
        break;

    case CLASS_PARASHRPRT:
        ParaSharedPrtRead(( PARASHAREDPRT *)pData);
        break;

    case CLASS_PARAPRESETAMOUNT:
        ParaPresetAmountRead(( PARAPRESETAMOUNT *)pData);
        break;

    case CLASS_PARAPIGRULE:                                /* A/C 130 */
        ParaPigRuleRead((PARAPIGRULE *)pData);
        break;

    case CLASS_PARASOFTCHK:                                 /* A/C 87 */
        ParaSoftChkMsgRead(( PARASOFTCHK *)pData);
        break;
    
    case CLASS_PARAHOTELID:                                 /* Prog# 54 */
        ParaHotelIdRead(( PARAHOTELID *)pData);
        break;

    case CLASS_PARAOEPTBL:                                  /* A/C 160 */
        ParaOepRead(( PARAOEPTBL *)pData);
        break;
    
    case CLASS_PARAFXDRIVE:                                 /* A/C 162  */
        ParaFxDriveRead(( PARAFXDRIVE *)pData);
        break;

    case CLASS_PARASERVICETIME:                             /* A/C 133  */
        ParaServiceTimeRead(( PARASERVICETIME *)pData);
        break;

    case CLASS_PARALABORCOST:                               /* A/C 154  */
        ParaLaborCostRead(( PARALABORCOST *)pData);
        break;

    case CLASS_PARADISPPARA:                                /* A/C 137  */
        ParaDispParaRead(( PARADISPPARA *)pData);
        break;

    case CLASS_PARAKDSIP:                               /* Prog 51, 2172  */
        ParaKdsIpRead(( PARAKDSIP *)pData);
        break;

    case CLASS_PARARESTRICTION:
        ParaRestRead(( PARARESTRICTION *)pData);            /* read PLU sales restriction, 2172 */
        break;

    case CLASS_PARABOUNDAGE:                                /* A/C 208 read boundary age, 2172 */
        ParaBoundAgeRead(( PARABOUNDAGE *)pData);
        break;

	case CLASS_PARACOLORPALETTE:
		ParaColorPaletteRead(( PARACOLORPALETTE *)pData);	/* A/C 209 read Color Palette	*/
		break;

    case CLASS_PARADEPTNOMENU:   
        ParaDeptNoMenuRead((PARADEPTNOMENU *)pData);   /* read Dept No.of appointed address, 2172 */
        break;                                              

	case CLASS_PARATENDERKEYINFO:
		ParaTendInfoRead((PARATENDERKEYINFO *)pData);
		break;

	case CLASS_PARAAUTOCPN:
		ParaAutoCouponRead((PARAAUTOCPN *)pData);
		break;

	case CLASS_PARATTLKEYORDERDEC:
		ParaTtlKeyOrderDecRead((PARATTLKEYORDDEC *)pData);
		break;

    default:
        break;
    }
}
/*
*=============================================================================
**  Synopsis: VOID ParaWrite(VOID *pData) 
*               
*       Input:  *pData  
*      Output:  nothing
*
**  Return: nothing 
*               
**  Descruption:  
*===============================================================================
*/
VOID ParaWrite(VOID *pData) 
{
    UCHAR       uchMajor;

    /* distinguish data class */
    uchMajor = *(( UCHAR*)pData);

    switch(uchMajor) {
    case CLASS_PARAPLUNOMENU:   
        ParaPLUNoMenuWrite((PARAPLUNOMENU *)pData);         /* Write PLU No.of appointed address */
        break;                                              

    case CLASS_PARAROUNDTBL:    
        ParaRoundWrite((PARAROUNDTBL *)pData);              /* Write rounding data */
        break;                                              

    case CLASS_PARADISCTBL:
        ParaDiscWrite((PARADISCTBL *)pData);                /* Write discount/bonus rate */
        break;

    case CLASS_PARAPROMOTION:             
        ParaPromotionWrite((PARAPROMOTION *)pData);         /* Write promotion sales header */
        break;

    case CLASS_PARACURRENCYTBL:           
        ParaCurrencyWrite((PARACURRENCYTBL *)pData);        /* Write foreign currency rate */
        break;

/*    case CLASS_PARAMENUPLUTBL:                              
        ParaMenuPLUWrite((PARAMENUPLUTBL *)pData);          */
        break;

    case CLASS_PARAMISCPARA:
        ParaMiscParaWrite((PARAMISCPARA *)pData);           /* read transaction limit */
        break;

    case CLASS_PARASTOREFORWARD:
        ParaStoreForwardParaWrite((PARAMISCPARA *)pData);            /* read transaction limit */
        break;

    case CLASS_PARATERMINALINFO:
        ParaTerminalInfoParaWrite((PARATERMINALINFO *)pData);  /* read terminal information */
        break;

    case CLASS_PARATAXRATETBL:
        ParaTaxRateWrite((PARATAXRATETBL *)pData);             /* Write tax rate */
        break;

    case CLASS_PARAMDC:
        ParaMDCWrite((PARAMDC *)pData);                     /* Write MDC data of appointed address */
        break;

    case CLASS_PARAFSC:
        ParaFSCWrite((PARAFSC *)pData);                     /* Write FSC data of appointed address */
        break;

    case CLASS_PARASECURITYNO:
        ParaSecurityNoWrite((PARASECURITYNO *)pData);       /* Write security No. */
        break;
    
    case CLASS_PARASUPLEVEL:                                
        ParaSupLevelWrite((PARASUPLEVEL *)pData);           /* Write supervisor no. & level */
        break;

    case CLASS_PARAACTCODESEC:
        ParaActCodeSecWrite((PARAACTCODESEC *)pData);       /* Write action code security of appointed address */
        break;

    case CLASS_PARATRANSHALO:
        ParaTransHALOWrite((PARATRANSHALO *)pData);         /* Write transaction halo */
        break;

    case CLASS_PARAHOURLYTIME:
        ParaHourlyTimeWrite((PARAHOURLYTIME *)pData);       /* Write hourly time data */
        break;

    case CLASS_PARASLIPFEEDCTL:
        ParaSlipFeedCtlWrite((PARASLIPFEEDCTL *)pData);     /* Write feed control data */
        break;

    case CLASS_PARATRANSMNEMO:
        ParaTransMnemoWrite((PARATRANSMNEMO *)pData);       /* Write transaction mnemonics of appointed address */
        break;

    case CLASS_PARALEADTHRU:
        ParaLeadThruWrite((PARALEADTHRU *)pData);           /* Write lead through of appointed address */
        break;

    case CLASS_PARAREPORTNAME:
        ParaReportNameWrite((PARAREPORTNAME *)pData);       /* Write report name of appointed address */
        break;

    case CLASS_PARASPECIALMNEMO:
        ParaSpeMnemoWrite((PARASPEMNEMO *)pData);           /* Write special mnemonics of appointed address */
        break;

    case CLASS_PARATAXTBL1:
    case CLASS_PARATAXTBL2:
    case CLASS_PARATAXTBL3:
    case CLASS_PARATAXTBL4:
        ParaTaxTableWrite((PARATAXTBL *)pData);             /* Write all tax table */
        break;

    case CLASS_PARAADJMNEMO:
        ParaAdjMnemoWrite((PARAADJMNEMO *)pData);           /* Write adjective mnemonics of appointed address */
        break;

    case CLASS_PARAPRTMODI:
        ParaPrtModiWrite((PARAPRTMODI *)pData);             /* Write print modifier mnemonics of appointed address */
        break;

    case CLASS_PARAMAJORDEPT:
        ParaMajorDEPTWrite((PARAMAJORDEPT *)pData);         /* Write major department mnemonics of appointed address */
        break;

    case CLASS_PARACHAR24:
        ParaChar24Write((PARACHAR24 *)pData);               /* Write 24 character mnemonics of appointed address */
        break;

    case CLASS_PARACTLTBLMENU:
        ParaCtlTblMenuWrite((PARACTLTBLMENU *)pData);       /* Write control menu page no.*/
        break;

    case CLASS_PARAAUTOALTKITCH:
        ParaAutoAltKitchWrite((PARAALTKITCH *)pData);       /* Write kitchen printer alternative list */
        break;

    case CLASS_PARAFLEXMEM:
        ParaFlexMemWrite((PARAFLEXMEM *)pData);             /* Write a number of record & flag status of PTD */
        break;

    case CLASS_PARASTOREGNO:
        ParaStoRegNoWrite((PARASTOREGNO *)pData);           /* Write store/register no. */
        break;

    case CLASS_PARATTLKEYTYP:
        ParaTtlKeyTypWrite((PARATTLKEYTYP *)pData);         /* Write characteristic type of total key */
        break;

    case CLASS_PARATTLKEYCTL:
        ParaTtlKeyCtlWrite((PARATTLKEYCTL *)pData);         /* Write bit decide to characterize total key */
        break;

    case CLASS_PARATEND:
        ParaTendWrite((PARATEND *)pData);                   /* Write tender parameter,  V3.3 */
        break;

    case CLASS_PARAMANUALTKITCH:
        ParaManuAltKitchWrite((PARAALTKITCH *)pData);       /* Write kitchen printer alternative list */
        break;

    case CLASS_PARACASHABASSIGN:
        ParaCashABAssignWrite((PARACASHABASSIGN *)pData );  /* Write cashier no. of A/B key */
        break;

    case CLASS_PARASPCCO:
        ParaSpcCoWrite((PARASPCCO *)pData);                 /* Write special counter */
        break;

    case CLASS_PARAPCIF:
        ParaPCIFWrite((PARAPCIF *)pData);
        break;

    case CLASS_PARATONECTL:
        ParaToneCtlWrite((PARATONECTL *)pData);
        break;

    case CLASS_PARATARE:
        ParaTareWrite(( PARATARE *)pData);
        break;

    case CLASS_PARASHRPRT:
        ParaSharedPrtWrite(( PARASHAREDPRT *)pData);
        break;

    case CLASS_PARAPRESETAMOUNT:
        ParaPresetAmountWrite(( PARAPRESETAMOUNT *)pData);
        break;
    
    case CLASS_PARAPIGRULE:                                 /* A/C 130 */
        ParaPigRuleWrite((PARAPIGRULE *)pData);
        break;

    case CLASS_PARASOFTCHK:                                 /* A/C 87 */
        ParaSoftChkMsgWrite(( PARASOFTCHK *)pData);
        break;
    
    case CLASS_PARAHOTELID:                                 /* Prog# 54 */
        ParaHotelIdWrite(( PARAHOTELID *)pData);
        break;

    case CLASS_PARAOEPTBL:                                  /* A/C 160 */
        ParaOepWrite(( PARAOEPTBL *)pData);
        break;
    
    case CLASS_PARAFXDRIVE:                                 /* A/C 162  */
        ParaFxDriveWrite(( PARAFXDRIVE *)pData);
        break;

    case CLASS_PARASERVICETIME:                             /* A/C 133  */
        ParaServiceTimeWrite(( PARASERVICETIME *)pData);
        break;

    case CLASS_PARALABORCOST:                               /* A/C 154  */
        ParaLaborCostWrite(( PARALABORCOST *)pData);
        break;

    case CLASS_PARADISPPARA:                                /* A/C 137  */
        ParaDispParaWrite(( PARADISPPARA *)pData);
        break;

    case CLASS_PARAKDSIP:                                /* Prog 51, 2172  */
        ParaKdsIpWrite(( PARAKDSIP *)pData);
        break;

    case CLASS_PARARESTRICTION:
        ParaRestWrite(( PARARESTRICTION *)pData);           /* Write PLU sales restriction, 2172 */
        break;

    case CLASS_PARABOUNDAGE:                                /* A/C 208 write boundary age, 2172 */
        ParaBoundAgeWrite(( PARABOUNDAGE *)pData);
        break;

	case CLASS_PARACOLORPALETTE:
		ParaColorPaletteWrite(( PARACOLORPALETTE *)pData);	/* A/C 209 write Color Palette */
		break;

    case CLASS_PARADEPTNOMENU:   
        ParaDeptNoMenuWrite((PARADEPTNOMENU *)pData);  /* Write Dept No.of appointed address, 2172 */
        break;  

	case CLASS_PARATENDERKEYINFO:
		ParaTendInfoWrite((PARATENDERKEYINFO *)pData);
		break;

	case CLASS_PARAAUTOCPN:
		ParaAutoCouponWrite((PARAAUTOCPN *)pData);
		break;

	case CLASS_PARATTLKEYORDERDEC:
		ParaTtlKeyOrderDecRead((PARATTLKEYORDDEC *)pData);
		break;

    default:
        break;
    }
}
