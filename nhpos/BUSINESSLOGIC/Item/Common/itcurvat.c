/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-8          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Item common module                         
* Category    : Item Common Module, NCR 2170 Int'l Hsopitality Model Application         
* Program Name: ITCURVAT.C
* --------------------------------------------------------------------------
* Abstract: ItemCurVAT()
*           ItemVATCalc1()
*           ItemVATCalc2()
*           ItemVATCalc3()
*           ItemVATCalc4()
*           ItemVATCalc5()
*           ItemVATCalc6()
*           ItemVATCalc7()
*           ItemVATCalc8()
*           ItemVATCalc9()
*           ItemVATCalc10()
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. : Name        : Description
*  93-01-11: 00.00.01  : F.SHIMOKAWA : initial
*  98-08-13: 03.03.00  : hrkato      : V3.3 (HP Int'l Type VAT/Service)
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
#include	<tchar.h>
#include    <memory.h>
#include    <string.h>

#include    "ecr.h"
#include    "pif.h"
#include    "log.h"
#include    "uie.h"
#include    "pmg.h"
#include    "rfl.h"
#include    "regstrct.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "display.h"
#include    "uireg.h"
#include    "appllog.h"
#include    "itmlocal.h"

/*
*===========================================================================
**Synopsis: SHORT   ItemVATCalc1( ITEMCALCVAT *pItemCalcVAT )
*
*    Input: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATRate
*           ITEMCALCVAT *pItemCalcVAT->lServiceRate
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Ser
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Non
*
*   Output: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATable
*           ITEMCALCAVT *pItemCalcVAT->ItemVAT.lVATAmt
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lSum
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lAppAmt
*           ITEMCALCVAT *pItemCalcVAT->lServiceAmt
*           ITEMCALCVAT *pItemCalcVAT->lAffectPayVAT
*           ITEMCALCVAT *pItemCalcVAT->lAffectPaySer
*
*   Return: ITM_SUCCESS    : Function performed successfully
*           LDT_PROHBT_ADR : calculation failed
*
** Description: caluculate case 1(included VAT, included service,
*               VAT on net, service on net)
*===========================================================================
*/
static SHORT   ItemVATCalc1( ITEMCALCVAT *pItemCalcVAT )
{
    DCURRENCY    lNet_Ser = 0L, lNet_Non = 0L;

    if ( pItemCalcVAT->ItemVAT.lVATRate != 0L ) {
		ULONG  lRate = 0;

		/* 1000000 for ulRate's 4 decimal places percentage plus 100 used for RflRateCalc4() */
        if ( pItemCalcVAT->lServiceRate != 0L ) {
            lRate = 1000000L + pItemCalcVAT->ItemVAT.lVATRate + pItemCalcVAT->lServiceRate;
        } else {
            lRate = 1000000L + pItemCalcVAT->ItemVAT.lVATRate;
        }

        if ( pItemCalcVAT->lVATable_Ser != 0L ) {
            if ( RflRateCalc4( &lNet_Ser, pItemCalcVAT->lVATable_Ser,
                                lRate, RND_TAX1_ADR) != RFL_SUCCESS ) {
                return( LDT_PROHBT_ADR );
            }
        } else {
            lNet_Ser = 0L;
        }

        if ( pItemCalcVAT->lVATable_Non != 0L ) {
            if ( RflRateCalc4( &lNet_Non, pItemCalcVAT->lVATable_Non,
                                1000000L + pItemCalcVAT->ItemVAT.lVATRate,
                                RND_TAX1_ADR) != RFL_SUCCESS ) {
                return( LDT_PROHBT_ADR );
            }
        } else {
            lNet_Non = 0L;
        }

        pItemCalcVAT->ItemVAT.lVATable = lNet_Ser + lNet_Non;

        if ( RflRateCalc3( &pItemCalcVAT->ItemVAT.lVATAmt, pItemCalcVAT->ItemVAT.lVATable,
                            pItemCalcVAT->ItemVAT.lVATRate, RND_TAX1_ADR) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }

    } else {
        if ( ( pItemCalcVAT->lServiceRate != 0L )
          && ( pItemCalcVAT->lVATable_Ser != 0L ) ) {
            if ( RflRateCalc4( &lNet_Ser, pItemCalcVAT->lVATable_Ser,
                                1000000L + pItemCalcVAT->lServiceRate,
                                RND_CHGTIP1_ADR ) != RFL_SUCCESS ) {
                return(LDT_PROHBT_ADR );
            }
        } else {
            lNet_Ser = pItemCalcVAT->lVATable_Ser;
        }

        lNet_Non = pItemCalcVAT->lVATable_Non;
        pItemCalcVAT->ItemVAT.lVATable = lNet_Ser + lNet_Non;
        pItemCalcVAT->ItemVAT.lVATAmt = 0L;
    }

    pItemCalcVAT->ItemVAT.lSum = pItemCalcVAT->ItemVAT.lVATable + pItemCalcVAT->ItemVAT.lVATAmt;
    pItemCalcVAT->lServiceAmt = pItemCalcVAT->lVATable_Ser + pItemCalcVAT->lVATable_Non
                                - pItemCalcVAT->ItemVAT.lSum;
    pItemCalcVAT->ItemVAT.lAppAmt = pItemCalcVAT->ItemVAT.lSum;
    pItemCalcVAT->lAffectPayVAT = 0L;
    pItemCalcVAT->lAffectPaySer = 0L;

    return( ITM_SUCCESS );
}

/*
*===========================================================================
**Synopsis: SHORT   ItemVATCalc2( ITEMCALCVAT *pItemCalcVAT )
*
*    Input: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATRate
*           ITEMCALCVAT *pItemCalcVAT->lServiceRate
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Ser
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Non
*
*   Output: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATable
*           ITEMCALCAVT *pItemCalcVAT->ItemVAT.lVATAmt
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lSum
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lAppAmt
*           ITEMCALCVAT *pItemCalcVAT->lServiceAmt
*           ITEMCALCVAT *pItemCalcVAT->lAffectPayVAT
*           ITEMCALCVAT *pItemCalcVAT->lAffectPaySer
*
*   Return: ITM_SUCCESS    : Function performed successfully
*           LDT_PROHBT_ADR : calculation failed
*
** Description: caluculate case 2(included VAT, included service,
*               VATable service, service on net)
*
*===========================================================================
*/
static SHORT   ItemVATCalc2( ITEMCALCVAT *pItemCalcVAT )
{
    DCURRENCY    lNet_Ser = 0L, lNet_Non = 0L;

    if ( pItemCalcVAT->ItemVAT.lVATRate != 0L ) {
		ULONG  lRate = 0;

		/* 1000000 for ulRate's 4 decimal places percentage plus 100 used for RflRateCalc4() */
		lRate = 1000000L + pItemCalcVAT->ItemVAT.lVATRate;

        if ( pItemCalcVAT->lVATable_Ser != 0L ) {
            if ( RflRateCalc4( &pItemCalcVAT->ItemVAT.lVATable, pItemCalcVAT->lVATable_Ser,
                                lRate, RND_TAX1_ADR) != RFL_SUCCESS ) {
                return( LDT_PROHBT_ADR );
            }
        } else {
            pItemCalcVAT->ItemVAT.lVATable = 0L;
        }

        if ( pItemCalcVAT->lVATable_Non != 0L ) {
            if ( RflRateCalc4( &lNet_Non, pItemCalcVAT->lVATable_Non,
                                lRate, RND_TAX1_ADR) != RFL_SUCCESS ) {
                return( LDT_PROHBT_ADR );
            }
        } else {
            lNet_Non = 0L;
        }

    } else {
        pItemCalcVAT->ItemVAT.lVATable = pItemCalcVAT->lVATable_Ser;
        lNet_Non = pItemCalcVAT->lVATable_Non;
    }

    if ( ( pItemCalcVAT->lServiceRate != 0L )
      && ( pItemCalcVAT->ItemVAT.lVATable != 0L ) ) {
        if ( RflRateCalc4( &lNet_Ser, pItemCalcVAT->ItemVAT.lVATable,
                            1000000L + pItemCalcVAT->lServiceRate,
                            RND_CHGTIP1_ADR ) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
        pItemCalcVAT->lServiceAmt = pItemCalcVAT->ItemVAT.lVATable - lNet_Ser;

    } else {
        pItemCalcVAT->lServiceAmt = 0L;
    }

    pItemCalcVAT->ItemVAT.lVATable += lNet_Non;
    pItemCalcVAT->ItemVAT.lAppAmt = pItemCalcVAT->lVATable_Ser + pItemCalcVAT->lVATable_Non;
    pItemCalcVAT->ItemVAT.lVATAmt = pItemCalcVAT->ItemVAT.lAppAmt - pItemCalcVAT->ItemVAT.lVATable;
    pItemCalcVAT->ItemVAT.lSum = pItemCalcVAT->ItemVAT.lVATable + pItemCalcVAT->ItemVAT.lVATAmt;
    pItemCalcVAT->lAffectPayVAT = 0L;
    pItemCalcVAT->lAffectPaySer = 0L;

    return( ITM_SUCCESS );
}

/*
*===========================================================================
**Synopsis: SHORT   ItemVATCalc3( ITEMCALCVAT *pItemCalcVAT )
*
*    Input: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATRate
*           ITEMCALCVAT *pItemCalcVAT->lServiceRate
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Ser
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Non
*
*   Output: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATable
*           ITEMCALCAVT *pItemCalcVAT->ItemVAT.lVATAmt
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lSum
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lAppAmt
*           ITEMCALCVAT *pItemCalcVAT->lServiceAmt
*           ITEMCALCVAT *pItemCalcVAT->lAffectPayVAT
*           ITEMCALCVAT *pItemCalcVAT->lAffectPaySer
*
*   Return: ITM_SUCCESS    : Function performed successfully
*           LDT_PROHBT_ADR : calculation failed
*
** Description: caluculate case 3(included VAT, included service,
*               VAT on net, serviceable VAT)
*
*===========================================================================
*/
static SHORT   ItemVATCalc3( ITEMCALCVAT *pItemCalcVAT )
{
    DCURRENCY    lNet_VAT = 0L;

    if ( pItemCalcVAT->lServiceRate != 0L ) {
		/* 1000000 for ulRate's 4 decimal places percentage plus 100 used for RflRateCalc4() */
		if ( RflRateCalc4( &lNet_VAT, pItemCalcVAT->lVATable_Ser,
                            1000000L + pItemCalcVAT->lServiceRate,
                            RND_CHGTIP1_ADR ) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
        pItemCalcVAT->lServiceAmt = pItemCalcVAT->lVATable_Ser - lNet_VAT;

    } else {
        pItemCalcVAT->lServiceAmt = 0L;
        lNet_VAT = pItemCalcVAT->lVATable_Ser;
    }

    pItemCalcVAT->ItemVAT.lSum = pItemCalcVAT->lVATable_Non + lNet_VAT;

    if ( pItemCalcVAT->ItemVAT.lVATRate != 0L ) {
        if ( RflRateCalc4( &pItemCalcVAT->ItemVAT.lVATable, pItemCalcVAT->ItemVAT.lSum,
                            1000000L + pItemCalcVAT->ItemVAT.lVATRate,
                            RND_TAX1_ADR) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
        pItemCalcVAT->ItemVAT.lVATAmt = pItemCalcVAT->ItemVAT.lSum - pItemCalcVAT->ItemVAT.lVATable;

    } else {
        pItemCalcVAT->ItemVAT.lVATable = pItemCalcVAT->ItemVAT.lSum;
        pItemCalcVAT->ItemVAT.lVATAmt = 0L;
    }

    pItemCalcVAT->ItemVAT.lAppAmt = pItemCalcVAT->ItemVAT.lSum;
    pItemCalcVAT->lAffectPayVAT = 0L;
    pItemCalcVAT->lAffectPaySer = 0L;

    return( ITM_SUCCESS );
}


/*
*===========================================================================
**Synopsis: SHORT   ItemVATCalc4( ITEMCALCVAT *pItemCalcVAT )
*
*    Input: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATRate
*           ITEMCALCVAT *pItemCalcVAT->lServiceRate
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Ser
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Non
*
*   Output: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATable
*           ITEMCALCAVT *pItemCalcVAT->ItemVAT.lVATAmt
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lSum
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lAppAmt
*           ITEMCALCVAT *pItemCalcVAT->lServiceAmt
*           ITEMCALCVAT *pItemCalcVAT->lAffectPayVAT
*           ITEMCALCVAT *pItemCalcVAT->lAffectPaySer
*
*   Return: ITM_SUCCESS    : Function performed successfully
*           LDT_PROHBT_ADR : calculation failed
*
** Description: caluculate case 4(included VAT, excluded service,
*               VAT on net, service on net)
*
*===========================================================================
*/
static SHORT   ItemVATCalc4( ITEMCALCVAT *pItemCalcVAT )
{
    DCURRENCY    lNet_Ser = 0L, lNet_Non = 0L;

    if ( pItemCalcVAT->ItemVAT.lVATRate != 0L ) {
		ULONG  lRate = 0;

		/* 1000000 for ulRate's 4 decimal places percentage plus 100 used for RflRateCalc4() */
		lRate = 1000000L + pItemCalcVAT->ItemVAT.lVATRate;

        if ( pItemCalcVAT->lVATable_Ser != 0L ) {
            if ( RflRateCalc4( &lNet_Ser, pItemCalcVAT->lVATable_Ser,
                                lRate, RND_TAX1_ADR) != RFL_SUCCESS ) {
                return( LDT_PROHBT_ADR );
            }
        } else {
            lNet_Ser = 0L;
        }

        if ( pItemCalcVAT->lVATable_Non != 0L ) {
            if ( RflRateCalc4( &lNet_Non, pItemCalcVAT->lVATable_Non,
                                lRate, RND_TAX1_ADR) != RFL_SUCCESS ) {
                return( LDT_PROHBT_ADR );
            }
        } else {
            lNet_Non = 0L;
        }

    } else {
        lNet_Ser = pItemCalcVAT->lVATable_Ser;
        lNet_Non = pItemCalcVAT->lVATable_Non;
    }

    if ( ( pItemCalcVAT->lServiceRate != 0L ) && ( lNet_Ser != 0L ) ) {
        if ( RflRateCalc3( &pItemCalcVAT->lServiceAmt, lNet_Ser,
                            pItemCalcVAT->lServiceRate, RND_CHGTIP1_ADR ) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
    } else {
        pItemCalcVAT->lServiceAmt = 0L;
    }

    pItemCalcVAT->ItemVAT.lVATable = lNet_Ser + lNet_Non;
    pItemCalcVAT->ItemVAT.lSum = pItemCalcVAT->lVATable_Ser + pItemCalcVAT->lVATable_Non;
    pItemCalcVAT->ItemVAT.lVATAmt = pItemCalcVAT->ItemVAT.lSum - pItemCalcVAT->ItemVAT.lVATable;
    pItemCalcVAT->ItemVAT.lAppAmt = pItemCalcVAT->ItemVAT.lSum;
    pItemCalcVAT->lAffectPayVAT = 0L;
    pItemCalcVAT->lAffectPaySer = pItemCalcVAT->lServiceAmt;

    return( ITM_SUCCESS );
}

/*
*===========================================================================
**Synopsis: SHORT   ItemVATCalc5( ITEMCALCVAT *pItemCalcVAT )
*
*    Input: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATRate
*           ITEMCALCVAT *pItemCalcVAT->lServiceRate
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Ser
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Non
*
*   Output: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATable
*           ITEMCALCAVT *pItemCalcVAT->ItemVAT.lVATAmt
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lSum
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lAppAmt
*           ITEMCALCVAT *pItemCalcVAT->lServiceAmt
*           ITEMCALCVAT *pItemCalcVAT->lAffectPayVAT
*           ITEMCALCVAT *pItemCalcVAT->lAffectPaySer
*
*   Return: ITM_SUCCESS    : Function performed successfully
*           LDT_PROHBT_ADR : calculation failed
*
** Description: caluculate case 5(included VAT, excluded service,
*               VAT on net, serviceable VAT)
*
*===========================================================================
*/
static SHORT   ItemVATCalc5( ITEMCALCVAT *pItemCalcVAT )
{
    pItemCalcVAT->ItemVAT.lSum = pItemCalcVAT->lVATable_Ser + pItemCalcVAT->lVATable_Non;

    if ( pItemCalcVAT->ItemVAT.lVATRate != 0L ) {
        if ( RflRateCalc4( &pItemCalcVAT->ItemVAT.lVATable, pItemCalcVAT->ItemVAT.lSum,
                            1000000L + pItemCalcVAT->ItemVAT.lVATRate,
                            RND_TAX1_ADR) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
        pItemCalcVAT->ItemVAT.lVATAmt = pItemCalcVAT->ItemVAT.lSum - pItemCalcVAT->ItemVAT.lVATable;
    } else {
        pItemCalcVAT->ItemVAT.lVATable = pItemCalcVAT->ItemVAT.lSum;
        pItemCalcVAT->ItemVAT.lVATAmt = 0L;
    }

    if ( ( pItemCalcVAT->lServiceRate != 0L )
        && ( pItemCalcVAT->lVATable_Ser != 0L ) ){

        if ( RflRateCalc3( &pItemCalcVAT->lServiceAmt, pItemCalcVAT->lVATable_Ser,
                            pItemCalcVAT->lServiceRate, RND_CHGTIP1_ADR ) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
    } else {
        pItemCalcVAT->lServiceAmt = 0L;
    }

    pItemCalcVAT->ItemVAT.lAppAmt = pItemCalcVAT->ItemVAT.lSum;
    pItemCalcVAT->lAffectPayVAT = 0L;
    pItemCalcVAT->lAffectPaySer = pItemCalcVAT->lServiceAmt;

    return( ITM_SUCCESS );
}

/*
*===========================================================================
**Synopsis: SHORT   ItemVATCalc6( ITEMCALCVAT *pItemCalcVAT )
*
*    Input: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATRate
*           ITEMCALCVAT *pItemCalcVAT->lServiceRate
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Ser
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Non
*
*   Output: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATable
*           ITEMCALCAVT *pItemCalcVAT->ItemVAT.lVATAmt
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lSum
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lAppAmt
*           ITEMCALCVAT *pItemCalcVAT->lServiceAmt
*           ITEMCALCVAT *pItemCalcVAT->lAffectPayVAT
*           ITEMCALCVAT *pItemCalcVAT->lAffectPaySer
*
*   Return: ITM_SUCCESS    : Function performed successfully
*           LDT_PROHBT_ADR : calculation failed
*
** Description: caluculate case 6(excluded VAT, included service,
*               VAT on net, service on net)
*
*===========================================================================
*/
static SHORT   ItemVATCalc6( ITEMCALCVAT *pItemCalcVAT )
{
    DCURRENCY    lNet_Ser = 0L;

    if ( ( pItemCalcVAT->lServiceRate != 0L )
      && ( pItemCalcVAT->lVATable_Ser != 0L ) ) {
        if ( RflRateCalc4( &lNet_Ser, pItemCalcVAT->lVATable_Ser,
                            1000000L + pItemCalcVAT->lServiceRate,
                            RND_CHGTIP1_ADR ) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
        pItemCalcVAT->lServiceAmt = pItemCalcVAT->lVATable_Ser - lNet_Ser;
    } else {
        lNet_Ser = pItemCalcVAT->lVATable_Ser;
        pItemCalcVAT->lServiceAmt = 0L;
    }

    pItemCalcVAT->ItemVAT.lVATable = pItemCalcVAT->lVATable_Non + lNet_Ser;

    if ( pItemCalcVAT->ItemVAT.lVATRate != 0L ) {
        if ( RflRateCalc3( &pItemCalcVAT->ItemVAT.lVATAmt, pItemCalcVAT->ItemVAT.lVATable,
                            pItemCalcVAT->ItemVAT.lVATRate, RND_TAX1_ADR) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
    } else {
        pItemCalcVAT->ItemVAT.lVATAmt = 0L;
    }

    pItemCalcVAT->ItemVAT.lAppAmt = pItemCalcVAT->ItemVAT.lVATable;
    pItemCalcVAT->ItemVAT.lSum = pItemCalcVAT->ItemVAT.lVATable + pItemCalcVAT->ItemVAT.lVATAmt;
    pItemCalcVAT->lAffectPayVAT = pItemCalcVAT->ItemVAT.lVATAmt;
    pItemCalcVAT->lAffectPaySer = 0L;

    return( ITM_SUCCESS );
}

/*
*===========================================================================
**Synopsis: SHORT   ItemVATCalc7( ITEMCALCVAT *pItemCalcVAT )
*
*    Input: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATRate
*           ITEMCALCVAT *pItemCalcVAT->lServiceRate
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Ser
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Non
*
*   Output: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATable
*           ITEMCALCAVT *pItemCalcVAT->ItemVAT.lVATAmt
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lSum
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lAppAmt
*           ITEMCALCVAT *pItemCalcVAT->lServiceAmt
*           ITEMCALCVAT *pItemCalcVAT->lAffectPayVAT
*           ITEMCALCVAT *pItemCalcVAT->lAffectPaySer
*
*   Return: ITM_SUCCESS    : Function performed successfully
*           LDT_PROHBT_ADR : calculation failed
*
** Description: caluculate case 7(excluded VAT, included service,
*               VATable service, service on net)
*
*===========================================================================
*/
static SHORT   ItemVATCalc7( ITEMCALCVAT *pItemCalcVAT )
{
    DCURRENCY    lNet_Ser = 0L;

    if ( ( pItemCalcVAT->lServiceRate != 0L )
      && ( pItemCalcVAT->lVATable_Ser != 0L ) ) {
        if ( RflRateCalc4( &lNet_Ser, pItemCalcVAT->lVATable_Ser,
                            1000000L + pItemCalcVAT->lServiceRate,
                            RND_CHGTIP1_ADR ) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
        pItemCalcVAT->lServiceAmt = pItemCalcVAT->lVATable_Ser - lNet_Ser;
    } else {
        pItemCalcVAT->lServiceAmt = 0L;
    }

    pItemCalcVAT->ItemVAT.lVATable = pItemCalcVAT->lVATable_Ser + pItemCalcVAT->lVATable_Non;

    if ( pItemCalcVAT->ItemVAT.lVATRate != 0L ) {
        if ( RflRateCalc3( &pItemCalcVAT->ItemVAT.lVATAmt, pItemCalcVAT->ItemVAT.lVATable,
                            pItemCalcVAT->ItemVAT.lVATRate, RND_TAX1_ADR) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
    } else {
        pItemCalcVAT->ItemVAT.lVATAmt = 0L;
    }

    pItemCalcVAT->ItemVAT.lSum = pItemCalcVAT->ItemVAT.lVATable + pItemCalcVAT->ItemVAT.lVATAmt;
    pItemCalcVAT->ItemVAT.lAppAmt = pItemCalcVAT->ItemVAT.lVATable;
    pItemCalcVAT->lAffectPayVAT = pItemCalcVAT->ItemVAT.lVATAmt;
    pItemCalcVAT->lAffectPaySer = 0L;

    return( ITM_SUCCESS );
}

/*
*===========================================================================
**Synopsis: SHORT   ItemVATCalc8( ITEMCALCVAT *pItemCalcVAT )
*
*    Input: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATRate
*           ITEMCALCVAT *pItemCalcVAT->lServiceRate
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Ser
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Non
*
*   Output: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATable
*           ITEMCALCAVT *pItemCalcVAT->ItemVAT.lVATAmt
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lSum
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lAppAmt
*           ITEMCALCVAT *pItemCalcVAT->lServiceAmt
*           ITEMCALCVAT *pItemCalcVAT->lAffectPayVAT
*           ITEMCALCVAT *pItemCalcVAT->lAffectPaySer
*
*   Return: ITM_SUCCESS    : Function performed successfully
*           LDT_PROHBT_ADR : calculation failed
*
** Description: caluculate case 8(excluded VAT, excluded service,
*               VAT on net, service on net)
*
*===========================================================================
*/
static SHORT   ItemVATCalc8( ITEMCALCVAT *pItemCalcVAT )
{
    pItemCalcVAT->ItemVAT.lVATable = pItemCalcVAT->lVATable_Ser + pItemCalcVAT->lVATable_Non;

    if ( pItemCalcVAT->ItemVAT.lVATRate != 0L ) {
        if ( RflRateCalc3( &pItemCalcVAT->ItemVAT.lVATAmt, pItemCalcVAT->ItemVAT.lVATable,
                            pItemCalcVAT->ItemVAT.lVATRate, RND_TAX1_ADR) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
    } else {
        pItemCalcVAT->ItemVAT.lVATAmt = 0L;
    }

    if ( ( pItemCalcVAT->lServiceRate != 0L )
      && ( pItemCalcVAT->lVATable_Ser != 0L ) ) {
        if ( RflRateCalc3( &pItemCalcVAT->lServiceAmt, pItemCalcVAT->lVATable_Ser,
                            pItemCalcVAT->lServiceRate, RND_CHGTIP1_ADR ) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
    } else {
        pItemCalcVAT->lServiceAmt = 0L ;
    }

    pItemCalcVAT->ItemVAT.lSum = pItemCalcVAT->ItemVAT.lVATable + pItemCalcVAT->ItemVAT.lVATAmt;
    pItemCalcVAT->ItemVAT.lAppAmt = pItemCalcVAT->ItemVAT.lVATable;
    pItemCalcVAT->lAffectPayVAT = pItemCalcVAT->ItemVAT.lVATAmt;
    pItemCalcVAT->lAffectPaySer = pItemCalcVAT->lServiceAmt;

    return( ITM_SUCCESS );
}

/*
*===========================================================================
**Synopsis: SHORT   ItemVATCalc9( ITEMCALCVAT *pItemCalcVAT )
*
*    Input: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATRate
*           ITEMCALCVAT *pItemCalcVAT->lServiceRate
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Ser
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Non
*
*   Output: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATable
*           ITEMCALCAVT *pItemCalcVAT->ItemVAT.lVATAmt
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lSum
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lAppAmt
*           ITEMCALCVAT *pItemCalcVAT->lServiceAmt
*           ITEMCALCVAT *pItemCalcVAT->lAffectPayVAT
*           ITEMCALCVAT *pItemCalcVAT->lAffectPaySer
*
*   Return: ITM_SUCCESS    : Function performed successfully
*           LDT_PROHBT_ADR : calculation failed
*
** Description: caluculate case 9(excluded VAT, excluded service,
*               VATable service, service on net)
*
*===========================================================================
*/
static SHORT   ItemVATCalc9( ITEMCALCVAT *pItemCalcVAT )
{
    if ( ( pItemCalcVAT->lServiceRate != 0L )
      && ( pItemCalcVAT->lVATable_Ser != 0L ) ) {
        if ( RflRateCalc3( &pItemCalcVAT->lServiceAmt, pItemCalcVAT->lVATable_Ser,
                            pItemCalcVAT->lServiceRate, RND_CHGTIP1_ADR ) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
    } else {
        pItemCalcVAT->lServiceAmt = 0L;
    }

    pItemCalcVAT->ItemVAT.lVATable = pItemCalcVAT->lVATable_Ser + pItemCalcVAT->lVATable_Non
                                        + pItemCalcVAT->lServiceAmt;

    if ( pItemCalcVAT->ItemVAT.lVATRate != 0L ) {
        if ( RflRateCalc3( &pItemCalcVAT->ItemVAT.lVATAmt, pItemCalcVAT->ItemVAT.lVATable,
                            pItemCalcVAT->ItemVAT.lVATRate, RND_TAX1_ADR) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
    } else {
        pItemCalcVAT->ItemVAT.lVATAmt = 0L;
    }

    pItemCalcVAT->ItemVAT.lSum = pItemCalcVAT->ItemVAT.lVATable + pItemCalcVAT->ItemVAT.lVATAmt;
    pItemCalcVAT->ItemVAT.lAppAmt = pItemCalcVAT->ItemVAT.lVATable;
    pItemCalcVAT->lAffectPayVAT = pItemCalcVAT->ItemVAT.lVATAmt;
    pItemCalcVAT->lAffectPaySer = pItemCalcVAT->lServiceAmt;

    return( ITM_SUCCESS );
}

/*
*===========================================================================
**Synopsis: SHORT   ItemVATCalc10( ITEMCALCVAT *pItemCalcVAT )
*
*    Input: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATRate
*           ITEMCALCVAT *pItemCalcVAT->lServiceRate
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Ser
*           ITEMCALCVAT *pItemCalcVAT->lVATable_Non
*
*   Output: ITEMCALCVAT *pItemCalcVAT->ItemVAT.lVATable
*           ITEMCALCAVT *pItemCalcVAT->ItemVAT.lVATAmt
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lSum
*           ITEMCALCVAT *pItemCalcVAT->ItemVAT.lAppAmt
*           ITEMCALCVAT *pItemCalcVAT->lServiceAmt
*           ITEMCALCVAT *pItemCalcVAT->lAffectPayVAT
*           ITEMCALCVAT *pItemCalcVAT->lAffectPaySer
*
*   Return: ITM_SUCCESS    : Function performed successfully
*           LDT_PROHBT_ADR : calculation failed
*
** Description: caluculate case 10(excluded VAT, excluded service,
*               VAT on net, serviceable VAT)
*
*===========================================================================
*/
static SHORT   ItemVATCalc10( ITEMCALCVAT *pItemCalcVAT )
{
    DCURRENCY    lVAT_Ser = 0L;

    pItemCalcVAT->ItemVAT.lVATable = pItemCalcVAT->lVATable_Ser + pItemCalcVAT->lVATable_Non;

    if ( pItemCalcVAT->ItemVAT.lVATRate != 0L ) {
        if ( RflRateCalc3( &pItemCalcVAT->ItemVAT.lVATAmt, pItemCalcVAT->ItemVAT.lVATable,
                            pItemCalcVAT->ItemVAT.lVATRate, RND_TAX1_ADR) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }

        if ( ( pItemCalcVAT->lServiceRate != 0L )
          && ( pItemCalcVAT->lVATable_Ser != 0L ) ) {
            if ( RflRateCalc3( &lVAT_Ser, pItemCalcVAT->lVATable_Ser,
                                1000000 + pItemCalcVAT->ItemVAT.lVATRate,
                                RND_TAX1_ADR) != RFL_SUCCESS ) {
                return( LDT_PROHBT_ADR );
            }
        } else {
            lVAT_Ser = pItemCalcVAT->lVATable_Ser;
        }

    } else {
        pItemCalcVAT->ItemVAT.lVATAmt = 0L;
        lVAT_Ser = pItemCalcVAT->lVATable_Ser;
    }

    if ( ( pItemCalcVAT->lServiceRate != 0L ) && ( lVAT_Ser != 0L ) ) {
        if ( RflRateCalc3( &pItemCalcVAT->lServiceAmt, lVAT_Ser,
                            pItemCalcVAT->lServiceRate, RND_CHGTIP1_ADR ) != RFL_SUCCESS ) {
            return( LDT_PROHBT_ADR );
        }
    } else {
        pItemCalcVAT->lServiceAmt = 0L;
    }

    pItemCalcVAT->ItemVAT.lSum = pItemCalcVAT->ItemVAT.lVATable + pItemCalcVAT->ItemVAT.lVATAmt;
    pItemCalcVAT->ItemVAT.lAppAmt = pItemCalcVAT->ItemVAT.lVATable;
    pItemCalcVAT->lAffectPayVAT = pItemCalcVAT->ItemVAT.lVATAmt;
    pItemCalcVAT->lAffectPaySer = pItemCalcVAT->lServiceAmt;

    return( ITM_SUCCESS );
}


/*
*===========================================================================
**Synopsis: SHORT   ItemCurVAT(ITEMCOMMONVAT *pItemCommonVAT, SHORT sType)
*
*    Input: ITEMCOMMONVAT *pItemCommonVAT, UCHAR uchCalcType
*   Output: ITEMCOMMONVAT *pItemCommonVAT->all member
*    InOut: none
*   Return: ITM_SUCCESS    : Function performed successfully
*           LDT_PROHBT_ADR : VAT calculation failed
*
** Description: calculate VAT amount & service amount.
*===========================================================================
*/
SHORT   ItemCurVAT( CONST UCHAR *puchTotal, ITEMCOMMONVAT *pItemCommonVAT, SHORT sType)
{
    SHORT           sReturnStatus, i;
    USHORT          usFlag;
    DCURRENCY       lAffectPayVAT, lAffectPaySer;
    PARATAXRATETBL  WorkRate;
    PARAMDC         WorkMDC;
    TRANITEMIZERS   *WorkTI;
    UCHAR           uchWorkBit;

    if (sType == ITM_SPLIT) {
        TrnGetTISpl(&WorkTI, TRN_TYPE_SPLITA);
    } else {
        TrnGetTI(&WorkTI);
    }

    lAffectPayVAT = 0L;
    lAffectPaySer = 0L;
    uchWorkBit = 0x01;

    for (i = 0; i < NUM_VAT; i ++) {
        if ( *puchTotal & uchWorkBit ) {    /* total key option */
            WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;
            WorkRate.uchAddress = (UCHAR)(TXRT_NO1_ADR + i);
            CliParaRead(&WorkRate);
            pItemCommonVAT->ItemVAT[i].lVATRate = WorkRate.ulTaxRate;
        }
        uchWorkBit <<= 1;
    }
    WorkRate.uchMajorClass = CLASS_PARATAXRATETBL;
    WorkRate.uchAddress = TXRT_NO4_ADR;
    CliParaRead(&WorkRate);
    pItemCommonVAT->ItemVAT[NUM_VAT].lVATRate = WorkRate.ulTaxRate;

    memset(&WorkMDC, 0, sizeof(PARAMDC));
    WorkMDC.uchMajorClass = CLASS_PARAMDC;
    WorkMDC.usAddress = MDC_VAT1_ADR;
    CliParaRead(&WorkMDC);

    for (i = 0; i <= NUM_VAT; i ++) {
		ITEMCALCVAT     ItemCalcVAT = {0};

		if (i < NUM_VAT) {
            ItemCalcVAT.ItemVAT.lVATRate = pItemCommonVAT->ItemVAT[i].lVATRate;
            ItemCalcVAT.lVATable_Ser = WorkTI->Itemizers.TranIntlItemizers.lVATSer[i];
            ItemCalcVAT.lVATable_Non = WorkTI->Itemizers.TranIntlItemizers.lVATNon[i];
        } else {
            ItemCalcVAT.ItemVAT.lVATRate = 0L;
            ItemCalcVAT.lVATable_Ser = WorkTI->Itemizers.TranIntlItemizers.lServiceable
                 - WorkTI->Itemizers.TranIntlItemizers.lVATSer[0]
                 - WorkTI->Itemizers.TranIntlItemizers.lVATSer[1]
                 - WorkTI->Itemizers.TranIntlItemizers.lVATSer[2];
            ItemCalcVAT.lVATable_Non = 0L;
        }
        if ( ( ItemCalcVAT.lVATable_Ser != 0L )
            || ( ItemCalcVAT.lVATable_Non != 0L ) ) {
            ItemCalcVAT.lServiceRate = WorkRate.ulTaxRate;
            usFlag = 1;

            switch(WorkMDC.uchMDCData & MDC_ODD_MASK) {
            case 0x02:
            case 0x03:
                sReturnStatus = ItemVATCalc1( &ItemCalcVAT );
                break;

            case 0x00:
                sReturnStatus = ItemVATCalc2( &ItemCalcVAT );
                break;

            case 0x01:
                sReturnStatus = ItemVATCalc3( &ItemCalcVAT );
                break;

            case 0x0A:
            case 0x0B:
                sReturnStatus = ItemVATCalc4( &ItemCalcVAT );
                break;

            case 0x09:
                sReturnStatus = ItemVATCalc5( &ItemCalcVAT );
                break;

            case 0x06:
            case 0x07:
                sReturnStatus = ItemVATCalc6( &ItemCalcVAT );
                break;

            case 0x04:
                sReturnStatus = ItemVATCalc7( &ItemCalcVAT );
                break;

            case 0x0E:
            case 0x0F:
                sReturnStatus = ItemVATCalc8( &ItemCalcVAT );
                break;

            case 0x0C:
                sReturnStatus = ItemVATCalc9( &ItemCalcVAT );
                break;

            case 0x0D:
                sReturnStatus = ItemVATCalc10( &ItemCalcVAT );
                break;

            default:
                usFlag = 0;
                break;
            }

            if ( usFlag ) {
                if ( sReturnStatus != ITM_SUCCESS ) {
                    return( sReturnStatus );
                }
                pItemCommonVAT->ItemVAT[i] = ItemCalcVAT.ItemVAT;
                pItemCommonVAT->lService += ItemCalcVAT.lServiceAmt;
                lAffectPayVAT += ItemCalcVAT.lAffectPayVAT;
                lAffectPaySer += ItemCalcVAT.lAffectPaySer;
            }

        } else {
            pItemCommonVAT->ItemVAT[i].lVATRate = 0L;
        }
    }
    pItemCommonVAT->lPayment = lAffectPayVAT + lAffectPaySer;

    return(ITM_SUCCESS);
}


/* --- End of Source --- */
