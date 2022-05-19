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
* Title       : Rate Calculation & rounding                 
* Category    : Reentrant Functions Library, NCR 2170
* Program Name: RflRound.C                                                      
* --------------------------------------------------------------------------
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
*       2015  -> GenPOS Rel 2.3.0 (EMV for Electronic Payment)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
* --------------------------------------------------------------------------
* Abstruct:
*   RflRateCalc1()
*   RflRateCalc2()
*   RflRound()
*   RflRateCalc5()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-08-92 : 00.00.01 : K.Nakajima :                                    
*
*** NCR2171
* Sep-17-99 : 01.00.00 : M.Teraki   : Modified RflRateCalc5() depends on
*                                   : 'D13DIGITS' type change
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
;+                  I N C L U D E     F I L E s
;========================================================================
**/
#include	<tchar.h>
#include<memory.h>

#include<ecr.h>
#include<paraequ.h>
#include<para.h>
#include<csstbpar.h>
#include<rfl.h>
#include"rflin.h"

VOID _RflGetRoundingTable(PARAROUNDTBL *pRndTbl, UCHAR uchType);

/**
;========================================================================
;+        P R O G R A M    D E C L A R A T I O N s
;========================================================================
**/
/*
*===========================================================================
** Format  : SHORT   RflRateCalc1(LONG *plModAmount, LONG lAmount,
*                                                    ULONG ulRate, UCHAR uchType);              
*
*   Input  : LONG   lAmount       -Source Data
*            ULONG  ulRate        -Rate(5 digits under decimal point)
*            UCHAR  uchType       -Rounding Type
*   Output : LONG   *plModAmount  -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS          -function success
*            RFL_FAIL             -function fail
*
** Synopsis: This function multiply amount by rate and is used in discount calculations and
*            in Foreign Currency conversion calculations.
*
*            A tax rate from AC 86 has 3 digits of precision. So a 2% discount rate
*            is entered as 2 and the ulRate value will be 2000. See RFL_DISC_RATE_MUL
*            which is used in some places to adjust discount rate from pItemDisc->uchRate
*            or pParaDiscTbl->uchDiscRate
*
*            A Currency Conversion rate from AC 89 has 5 digits of precision with
*            values from 0.00001 - 9999.99999 in the AC 89 table. So a conversion
*            rate of 1.32591 in the AC 89 dialog will cause the argument ulRate
*            to have a value of 132591.
*===========================================================================
*/
SHORT   RflRateCalc1(DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType)
{
    D13DIGITS     d13Quot = lAmount;

    d13Quot *= ulRate;
    d13Quot /= 10000L;       /* under 1 digits of decimal point is available */

    if (RFL_SUCCESS == RflRoundDcurrency (plModAmount, d13Quot, uchType)) {
        return (RFL_SUCCESS);
    } else {
        return (RFL_FAIL);
    }
}

/*
*===========================================================================
** Format  : SHORT   RflRateCalc2(LONG *plModAmount, LONG lAmount,
*                                                     ULONG ulRate, UCHAR uchType);              
*
*   Input  : LONG   lAmount          -Source Data
*            ULONG  ulRate           -Rate(5 digits under decimal point)
*            UCHAR  uchType          -Rounding Type
*   Output : LONG   *plModAmount     -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS             -function success
*            RFL_FAIL                -function fail
*
** Synopsis: This function divides lAmount by Rate and is used in foreign currency
*            conversions.
*
*            This function is the same as RflRateCalc4() except that it does 5 digit
*            conversion while RflRateCalc4() does 6 digit conversion.
*===========================================================================
*/
SHORT   RflRateCalc2(DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType) 
{
    D13DIGITS    d13Quot = lAmount;

    if (0L == ulRate) {                         /* check 0 divide */
        *plModAmount = lAmount;                 
        return (RFL_FAIL);
    }

    d13Quot *= 1000000L;     /* 100000 for ulRate's decimal point, 10 for result divide calc */
    d13Quot /= ulRate; 

    if (RFL_SUCCESS == RflRoundDcurrency(plModAmount, d13Quot, uchType)) {
        return (RFL_SUCCESS);
    } else {
        return (RFL_FAIL);
    }
}

/*
*===========================================================================
** Format  : SHORT   RflRateCalc3(LONG *plModAmount, LONG lAmount,
*                                                    ULONG ulRate, UCHAR uchType);
*
*   Input  : LONG   lAmount       -Source Data
*            ULONG  ulRate        -Rate (6 digits under decimal point)
*            UCHAR  uchType       -Rounding Type
*   Output : LONG   *plModAmount  -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS          -function success
*            RFL_FAIL             -function fail
*
** Synopsis: This function multiply amount by rate and is used in tax calculations.
*
*            A tax rate from AC 127 has 4 digits of precision. So a 4% tax rate
*            is entered at 4.0000 and the ulRate value will be 40000.
*===========================================================================
*/
SHORT   RflRateCalc3(DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType)
{
    D13DIGITS    d13Quot = lAmount;

    d13Quot *= ulRate;
    d13Quot /= 100000L;       /* under 1 digits of decimal point is valiable */

    if (RFL_SUCCESS == RflRoundDcurrency(plModAmount, d13Quot, uchType)) {
        return (RFL_SUCCESS);
    } else {
        return (RFL_FAIL);
    }
}

/*
*===========================================================================
** Format  : SHORT   RflRateCalc4(LONG *plModAmount, LONG lAmount,
*                                                     ULONG ulRate, UCHAR uchType);              
*
*   Input  : LONG   lAmount          -Source Data
*            ULONG  ulRate           -Rate(6 digits under decimal point)
*            UCHAR  uchType          -Rounding Type
*   Output : LONG   *plModAmount     -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS             -function success
*            RFL_FAIL                -function fail
*
** Synopsis: This function divides lAmount by Rate and is used in VAT calculations.
*            This function is the same as RflRateCalc2() except that it does 6 digit
*            conversion while RflRateCalc2() does 5 digit conversion.
*
*            Something a bit weird is that the ulRate is a rate from the VAT tables
*            but to which 1000000L is added before this function is called. Why?
*            Because this function is designed to calculate rate conversion with
*            six digits of precision and the VAT tables are percentages to four
*            decimal places (99.9999) that are stored as integers with an implied
*            decimal point. We must convert the percentage such as 4% to 104% for
*            the conversion.
*
*            This function is also used by function ItemTendFCRateCalc2() to calculate
*            foreign currency to local currency when the foreign currency rate is
*            to six digits of precision. The values in the Currency Conversion table
*            are not percentages but are actual rates that are stored as integers
*            with an implied decimal point.
*===========================================================================
*/
SHORT   RflRateCalc4(DCURRENCY *plModAmount, DCURRENCY lAmount, ULONG ulRate, UCHAR uchType) 
{
    D13DIGITS    d13Quot = lAmount;

    if (0L == ulRate) {                         /* check 0 divide */
        *plModAmount = lAmount;                 
        return (RFL_FAIL);
    }

    d13Quot *= 10000000L; /* 1000000 for ulRate's decimal point, 10 for result divide calc */
    d13Quot /= ulRate; 

    if (RFL_SUCCESS == RflRoundDcurrency(plModAmount, d13Quot, uchType)) {
        return (RFL_SUCCESS);
    } else {
        return (RFL_FAIL);
    }
}

/*
*===========================================================================
** Format  : SHORT  RflRoundDcurrency (DCURRENCY *plModAmount, D13DIGITS d13Amount, UCHAR uchType)
*
*   Input  : D13DIGITS    d13Amount     - amount to be rounded
*            UCHAR        uchType       - rounding type
*   Output : DCURRENCY    *plModAmount  - rounded amount
*   InOut  : none
*
** Return  : RFL_SUCCESS                -function success
*            RFL_FAIL                   -function fail
*
** Synopsis: This function rounds the provided D13DIGITS amount and then returns
*            the rounded value as a DCURRENCY through a pointer.
*===========================================================================
*/SHORT  RflRoundDcurrency (DCURRENCY *plModAmount, D13DIGITS d13Amount, UCHAR uchType)
{
    PARAROUNDTBL RndTbl;
    LONG         lModulus, lDelimit;

    _RflGetRoundingTable(&RndTbl, uchType);

    if (RndTbl.uchRoundModules == 0        /* check 0 divide */
        || RndTbl.chRoundPosition == 0     /* round positon effective? */
        || RndTbl.chRoundPosition < -1     
        || 3 < RndTbl.chRoundPosition) {     

        d13Amount /= 10L;                      /* delete under decimal point */
        *plModAmount = (DCURRENCY)d13Amount;
        return (RFL_FAIL);
    }

    lModulus = RndTbl.uchRoundModules;     /* adjust modules, delimiter */
    lDelimit = RndTbl.uchRoundDelimit;

    if (RndTbl.chRoundPosition > 0) {
		SHORT        i;

        /* '1' = digits of under decimalpoints */
        for (i= 1; i < RndTbl.chRoundPosition + 1; i++) { 
            lModulus = 10 * lModulus;
            lDelimit = 10 * lDelimit;
        }
    }

    if ( d13Amount < 0) { 
        lDelimit = -1L * lDelimit;                  /* if minus value */
    }

    d13Amount += lDelimit;      /* Add delimiter to amount         */
    d13Amount /= lModulus;      /* divide by modulus */
    d13Amount *= lModulus;      /* multiply by modulus */
    d13Amount /= 10L;           /* 10L for 1digits of under decimal point */
    *plModAmount = (DCURRENCY)d13Amount;

    return(RFL_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT   RflRound(LONG *plModAmount, LONG lAmount, UCHAR uchType);              
*
*   Input  : LONG lAmount            -Source Data
*            UCHAR uchType           -Rounding Type
*
*   Output : LONG         *plModAmount  -modifierd amount
*
*   InOut  : none
*
** Return  : RFL_SUCCESS                -function success
*            RFL_FAIL                   -function fail
*
** Synopsis: This function roundes long data
*===========================================================================
*/
SHORT   RflRound(DCURRENCY *plModAmount, DCURRENCY lAmount, UCHAR uchType)
{
    PARAROUNDTBL  RndTbl;
    SHORT         sModules;
    SHORT         sDelimit;
    SHORT         i;

    RndTbl.uchMajorClass = CLASS_PARAROUNDTBL;
    RndTbl.uchAddress = uchType;
    CliParaRead(&RndTbl);                  /* get modules, delimiter, rounding position */

    if (RndTbl.uchRoundModules == 0        /* check 0 divide */
        || RndTbl.chRoundPosition < 1     
        || 3 < RndTbl.chRoundPosition) {

        *plModAmount = lAmount;
        return (RFL_FAIL);
    }

    sModules = RndTbl.uchRoundModules;     /* adjust modules, delimiter */
    sDelimit = RndTbl.uchRoundDelimit;
    for (i= 1; i < RndTbl.chRoundPosition; i++) {
        sModules = 10 * sModules;
        sDelimit = 10 * sDelimit;
    }

    if (lAmount < 0) {                     /* minus management */
        sDelimit = -1 * sDelimit;
    }

    *plModAmount = lAmount + sDelimit;
    *plModAmount = *plModAmount / sModules;
    *plModAmount = *plModAmount * sModules;
    
    return(RFL_SUCCESS);
}

/*
*===========================================================================
** Format  : SHORT   RflRateCalc5(D13DIGITS *pd13ModAmount, D13DIGITS *pd13Amount,
*                                                     ULONG ulRate, UCHAR uchType);
*
*   Input  : D13DIGITS   d13Amount        -Source Data
*            ULONG       ulRate           -Rate(6 digits under decimal point)
*            UCHAR       uchType          -Rounding Type
*   Output : D13DIGITS   *pd13ModAmount   -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS             -function success
*            RFL_FAIL                -function fail
*
** Synopsis: This function divides lAmount by Rate. V3.3
*===========================================================================
*/
SHORT   RflRateCalc5(D13DIGITS *pd13ModAmount, D13DIGITS *pd13Amount, ULONG ulRate, UCHAR uchType)
{
#if defined (USE_2170_ORIGINAL)
    LONG         lRemain;
    D24DIGITS    d24Work;

    if (0L == ulRate) {                         /* check 0 divide */
        memcpy(pd13ModAmount, pd13Amount, sizeof(D13DIGITS));
        return (RFL_FAIL);
    }

    RflMulD13byD9(&d24Work, pd13Amount, 10000000L);
                            /* 1000000 for ulRate's decimal point, 10 for result divide calc */
                             
    RflDivD24byD9(&d24Work, &lRemain, ulRate);

    if (RFL_SUCCESS == RflRoundD24(pd13ModAmount, &d24Work, uchType)) {
        return (RFL_SUCCESS);
    }

    return (RFL_FAIL);
#else
    D13DIGITS    d13Work;
    ULONG        ulMultiply = 10000000L;	/* 8 digits */
                            /* 1000000 for ulRate's decimal point, 10 for result divide calc */
    SHORT        i;

    if (0L == ulRate) {                         /* check 0 divide */
        *pd13ModAmount = *pd13Amount;
        return (RFL_FAIL);
    }

	/* avoid the overflow at the calculation */
	for (i=0; i<8; i++) {
		if (ulRate%10) break;
		ulRate = ulRate/10;
		ulMultiply = ulMultiply/10;
	}

	/* just use int64 calculation, because D24Digits function does not work correctly */
	d13Work = *pd13Amount;
	d13Work *= ulMultiply;
	d13Work /= ulRate;
    if (RFL_SUCCESS == RflRoundHugeHuge(pd13ModAmount, &d13Work, uchType)) {
        return (RFL_SUCCESS);
    }
    return (RFL_FAIL);
#endif
}

#if defined(USE_2170_ORIGINAL)
/*
*===========================================================================
** Format  : SHORT  RflRoundD24(D13DIGITS *pd13ModAmount, D24DIGITS *pd24Amount, UCHAR uchType)
*
*   Input  : D24DIGITS    *pd24Amount     -amount
*            UCHAR        uchType         -rounding type
*   Output : D13DIGITS    *pd13ModAmount  -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS                -function success
*            RFL_FAIL                   -function fail
*
** Synopsis: This function rounding D24DIGITS data . V3.3
*===========================================================================
*/
SHORT  RflRoundD24(D13DIGITS *pd13ModAmount, D24DIGITS *pd24Amount, UCHAR uchType)
{

    PARAROUNDTBL RndTbl;
    LONG         lModulus, lDelimit;
    SHORT        i;
    LONG         lRemain;
    D24DIGITS    d24Work;

    _RflGetRoundingTable(&RndTbl, uchType);

    if (RndTbl.uchRoundModules == 0        /* check 0 divide */
        || RndTbl.chRoundPosition == 0     /* round positon effective? */
        || RndTbl.chRoundPosition < -1
        || 3 < RndTbl.chRoundPosition) {

        RflDivD24byD9(pd24Amount, &lRemain, 10L); /* delete under dicimal point */
        RflCastD24toD13(pd13ModAmount, pd24Amount);
        return (RFL_FAIL);
    }

    lModulus = RndTbl.uchRoundModules;     /* adjust modules, delimiter */
    lDelimit = RndTbl.uchRoundDelimit;

    if (RndTbl.chRoundPosition > 0) {

        /* '1' = digits of under decimalpoints */
        for (i= 1; i < RndTbl.chRoundPosition + 1; i++) {
            lModulus = 10 * lModulus;
            lDelimit = 10 * lDelimit;
        }
    }

    if ( ((CHAR *)pd24Amount)[sizeof(EHUGEINT) - 1] & 0x80 ) {

        lDelimit = -1L * lDelimit;                  /* if minus value */

    }


    RflCastLONGtoD24(&d24Work, lDelimit);             /* Add delimiter     */
    RflAddD24toD24(pd24Amount, &d24Work);             /* to amount         */

    RflDivD24byD9(pd24Amount, &lRemain, lModulus);    /* divide by modulus */

    RflCastD24toD13(pd13ModAmount, pd24Amount);

    RflMulD13byD9(pd24Amount, pd13ModAmount, lModulus);/* multiply modules  */

    RflDivD24byD9(pd24Amount, &lRemain, 10L);         /* 10L for 1digits of under decimal point */

    RflCastD24toD13(pd13ModAmount, pd24Amount);       /* convert 96bit data to 48 bit data */

    return(RFL_SUCCESS);
}
#endif

/*
*===========================================================================
** Format  : SHORT   RflRateCalcHuge1(HUGEINT *phuModAmount, HUGEINT *phuAmount,
*                                                    ULONG ulRate, UCHAR uchType);
*
*   Input  : HUGEINT phuAmount       -Source Data
*            ULONG  ulRate        -Rate(5 digits under decimal point)
*            UCHAR  uchType       -Rounding Type
*   Output : HUGEINT *phuModAmount  -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS          -function success
*            RFL_FAIL             -function fail
*
** Synopsis: This function multiply amount by rate, V2.2
*===========================================================================
*/
SHORT   RflRateCalcHuge1(D13DIGITS *phuModAmount, D13DIGITS *phuAmount, ULONG ulRate, UCHAR uchType)
{
    D13DIGITS    ehuQuot;   // was EHUGEINT for 96 bits
    D13DIGITS    huQuot;

    ehuQuot = *phuAmount * ulRate;
    ehuQuot /= 10000L;              /* under 1 digits of decimal point is valiable */
    huQuot = ehuQuot;

    return RflRoundHugeHuge(phuModAmount, &huQuot, uchType);
}

/*
*===========================================================================
** Format  : SHORT   RflRateCalcHuge2(HUGEINT *phuModAmount, HUGEINT *phuAmount,
*                                                     ULONG ulRate, UCHAR uchType);
*
*   Input  : HUGEINT *phuAmount          -Source Data
*            ULONG  ulRate           -Rate(5 digits under decimal point)
*            UCHAR  uchType          -Rounding Type
*   Output : HUGEINT *phuModAmount     -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS             -function success
*            RFL_FAIL                -function fail
*
** Synopsis: This function divides lAmount by Rate. V2.2
*===========================================================================
*/
SHORT   RflRateCalcHuge2(D13DIGITS *phuModAmount, D13DIGITS *phuAmount, ULONG ulRate, UCHAR uchType)
{
    D13DIGITS    ehuQuot;   // was EHUGEINT for 96 bits
    D13DIGITS    huQuot;

    if (0L == ulRate) {                         /* check 0 divide */
        *phuModAmount = *phuAmount;
        return (RFL_FAIL);
    }

    ehuQuot = *phuAmount * 1000000L; /* 100000 for ulRate's decimal point, 10 for result divide calc */
    ehuQuot /= ulRate;
    huQuot = ehuQuot;

    return RflRoundHugeHuge(phuModAmount, &huQuot, uchType);
}

/*
*===========================================================================
** Format  : SHORT   RflRateCalcHuge3(HUGEINT *phuModAmount, HUGEINT *phuAmount,
*                                                    ULONG ulRate, UCHAR uchType);
*
*   Input  : LONG   lAmount       -Source Data
*            ULONG  ulRate        -Rate (6 digits under decimal point)
*            UCHAR  uchType       -Rounding Type
*   Output : LONG   *plModAmount  -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS          -function success
*            RFL_FAIL             -function fail
*
** Synopsis: This function multiply amount by rate V2.2
*===========================================================================
*/
SHORT   RflRateCalcHuge3(D13DIGITS *phuModAmount, D13DIGITS *phuAmount, ULONG ulRate, UCHAR uchType)
{
    D13DIGITS    ehuQuot;   // was EHUGEINT for 96 bits
    D13DIGITS    huQuot;

    ehuQuot = *phuAmount * ulRate;
    ehuQuot /= 100000L;             /* under 1 digits of decimal point is valiable */
    huQuot = ehuQuot;

    return RflRoundHugeHuge(phuModAmount, &huQuot, uchType);
}

/*
*===========================================================================
** Format  : SHORT   RflRateCalcHuge4(HUGEINT *phuModAmount, HUGEINT *phuAmount,
*                                                     ULONG ulRate, UCHAR uchType);
*
*   Input  : HUGEINT *phuAmount          -Source Data
*            ULONG  ulRate           -Rate(6 digits under decimal point)
*            UCHAR  uchType          -Rounding Type
*   Output : HUGEINT *phuModAmount     -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS             -function success
*            RFL_FAIL                -function fail
*
** Synopsis: This function divides lAmount by Rate. V2.2
*===========================================================================
*/
SHORT   RflRateCalcHuge4(D13DIGITS *phuModAmount, D13DIGITS *phuAmount, ULONG ulRate, UCHAR uchType)
{
    D13DIGITS    ehuQuot;   // was EHUGEINT for 96 bits
    D13DIGITS    huQuot;

    if (0L == ulRate) {                         /* check 0 divide */
        *phuModAmount = *phuAmount;
        return (RFL_FAIL);
    }

    ehuQuot = *phuAmount * 10000000L; /* 1000000 for ulRate's decimal point, 10 for result divide calc */
    ehuQuot /= ulRate;
    huQuot = ehuQuot;

    return RflRoundHugeHuge(phuModAmount, &huQuot, uchType);
}

/*
*===========================================================================
** Format  : SHORT  RflRoundHugeHuge(HUGEINT *phuModAmount, HUGEINT *pAmount, UCHAR uchType)
*
*   Input  : HUGEINT      *pAmount      -amount
*            UCHAR        uchType       -rounding type
*   Output : HUGEINT      *phuModAmount  -modifierd amount
*   InOut  : none
*
** Return  : RFL_SUCCESS                -function success
*            RFL_FAIL                   -function fail
*
** Synopsis: This function rounding HUGEINT data .V2.2
*===========================================================================
*/
SHORT  RflRoundHugeHuge(D13DIGITS *phuModAmount, D13DIGITS *phuAmount, UCHAR uchType)
{

    PARAROUNDTBL RndTbl;
    DCURRENCY    lModulus, lDelimit;
    SHORT        i;
    D13DIGITS    huWork;
    D13DIGITS    ehuAmount;   // was EHUGEINT 96 bit variable

    _RflGetRoundingTable(&RndTbl, uchType);

    if (RndTbl.uchRoundModules == 0        /* check 0 divide */
        || RndTbl.chRoundPosition == 0     /* round positon effective? */
        || RndTbl.chRoundPosition < -1
        || 3 < RndTbl.chRoundPosition)
	{
        *phuAmount /= 10L; /* delete under dicimal point */
        *phuModAmount = *phuAmount;
        return (RFL_FAIL);
    }

    lModulus = RndTbl.uchRoundModules;     /* adjust modules, delimiter */
    lDelimit = RndTbl.uchRoundDelimit;

    if (RndTbl.chRoundPosition > 0) {
        /* '1' = digits of under decimalpoints */
        for (i = 1; i < RndTbl.chRoundPosition + 1; i++) {
            lModulus = 10 * lModulus;
            lDelimit = 10 * lDelimit;
        }
    }

    if ( *phuAmount < 0 ) {
        lDelimit = -1L * lDelimit;                  /* if minus value */
    }

    huWork = lDelimit;               /* Add delimiter     */
    *phuAmount += huWork;            /* to amount         */
    *phuAmount /= lModulus;          /* divide by modulus */
    *phuModAmount = *phuAmount;
    ehuAmount = *phuModAmount * lModulus;     /* multiply modules  */
    ehuAmount /= 10L;                         /* 10L for 1digits of under decimal point */
    *phuModAmount = ehuAmount;                /* convert 64bit data to 32 bit data */
    return(RFL_SUCCESS);
}

SHORT  RflRoundD13D13(D13DIGITS *pd13ModAmount, D13DIGITS d13Amount, UCHAR uchType)
{
    PARAROUNDTBL  RndTbl;
    LONG          lModulus, lDelimit;

    _RflGetRoundingTable(&RndTbl, uchType);
    if (RndTbl.uchRoundModules == 0        /* check 0 divide */
        || RndTbl.chRoundPosition == 0     /* round positon effective? */
        || RndTbl.chRoundPosition < -1
        || 3 < RndTbl.chRoundPosition) {

        *pd13ModAmount = d13Amount / 10L; /* delete under decimal point */
        return (RFL_FAIL);
    }

    lModulus = RndTbl.uchRoundModules;     /* adjust modules, delimiter */
    lDelimit = RndTbl.uchRoundDelimit;
    if (RndTbl.chRoundPosition > 0) {
		SHORT         i;

        /* '1' = digits of under decimalpoints */
        for (i = 1; i < RndTbl.chRoundPosition + 1; i++) {
            lModulus = 10 * lModulus;
            lDelimit = 10 * lDelimit;
        }
    }

    if ( d13Amount < 0 ) {
        lDelimit *= -1L;         /* if minus value */
    }

	// perform the rounding to the nearest lModulus of the specified value.
    d13Amount += lDelimit;          /* Add delimiter to amount   */
    d13Amount /= lModulus;          /* divide then multiply by modulus */
    d13Amount *= lModulus;
    d13Amount /= 10L;               /* 10L for 1digits of under decimal point */
    *pd13ModAmount = d13Amount;     /* return the result */
    return(RFL_SUCCESS);
}



/*
*===========================================================================
** Format  : VOID _RflGetRoundingTable(PARAROUNDTBL *pRndTbl, UCHAR uchType)
*
*   Input  : UCHAR        uchType       -rounding type
*   Output : PARAROUNDTABLE *pRndTbl    -rounding table
*   InOut  : none
*
** Return  : none
*
** Synopsis: This function gets rounding table, V2.2
*===========================================================================
*/
VOID _RflGetRoundingTable(PARAROUNDTBL *pRndTbl, UCHAR uchType)
{
    if ( uchType / 100 ) {

        pRndTbl->chRoundPosition = -1;

        switch (uchType) {

        case RND_RND_UP:
            pRndTbl->uchRoundModules = 10;
            pRndTbl->uchRoundDelimit =  0;
            break;

        case RND_RND_DOWN:
            pRndTbl->uchRoundModules = 10;
            pRndTbl->uchRoundDelimit =  9;
            break;

        case RND_RND_NORMAL:
            pRndTbl->uchRoundModules = 10;
            pRndTbl->uchRoundDelimit =  5;
            break;

        }

    } else {

        pRndTbl->uchMajorClass = CLASS_PARAROUNDTBL;
        pRndTbl->uchAddress    = uchType;
        CliParaRead(pRndTbl);              /* get modules, delimiter, rounding position */
    }


    /* -- case of parameter not set -- */
    if (pRndTbl->uchRoundModules == 0 && pRndTbl->chRoundPosition == 0
                                    && pRndTbl->uchRoundDelimit == 0 ) {

        pRndTbl->uchRoundModules = 10;       /* set normal rounding */
        pRndTbl->uchRoundDelimit =  5;
        pRndTbl->chRoundPosition = -1;
    }
}

