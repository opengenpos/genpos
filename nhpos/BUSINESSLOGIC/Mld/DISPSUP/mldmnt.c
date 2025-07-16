
#if defined(POSSIBLE_DEAD_CODE)
// This source file had several TCHAR arrays defined for menus, etc.
// Since the data structures were used only in a single other source file,
// those definitions were moved from this file to the source file they were
// used, deleting them from this file.
//
// Since the data structures were only used within those specific source files,
// the data structures were marked with the static keyword to reduce visibility
// to file scope.
//
// Data structures moved:
//    CONST UCHAR FARCONST aszMldAC114[][64];
//    CONST TCHAR FARCONST aszMldAC152[][64];
//    CONST TCHAR FARCONST aszMldAC153[][64];
//    CONST TCHAR FARCONST aszMldAC160[][64];
//
//     Richard Chambers, Jul-14-2025

/*========================================================================*\
||                                                                        ||
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                Release 3.0               ||
||                                                                        ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:   Title              : NCR 2170 Header Definition                        :
:   Category           : SUPER & PROGRAM MODE,ECR 2170                     :
:   Program Name       : MLDMNT.C                                          :
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments              :
:   Program Written    : 95-03-28 : 03.00.00    : Initial                  :
:                      : 95-07-20 : 03.00.01    : Modify Coupon Menu (FVT) :
:                      : 99-08-11 : 03.05.00    : Remove WAITER_MODEL      :
:  ----------------------------------------------------------------------  :
:   Compile            : MS-C Ver. 6.00A by Microsoft Corp.                :
:   Memory Model       : Midium Model                                      :
:   Condition          :                                                   :
\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*------------------------------------------------------------------------*/
#include	<tchar.h>
#include <string.h>

#include <ecr.h>
#include <uie.h>
#include <pif.h>
#include <rfl.h>
#include <log.h>
#include <paraequ.h> 
#include <para.h>
#include <maint.h> 
#include <display.h> 
#include <regstrct.h>
#include <transact.h>
#include <prt.h>
#include <csop.h>
#include <csstbpar.h>
#include <csstbopr.h>
#include <csgcs.h>
#include <csstbgcf.h>
#include <csttl.h>
#include <csstbttl.h>
#include <appllog.h>
#include <mldmenu.h>

#include <mld.h>

#include "maintram.h" 

/*------------------------------------------------------------------------*\

       M A I N T E N A N C E   O F   S U P E R V I S O R   M O D E

\*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*\
                    S E R V E R   M a i n t e n a n c e
\*------------------------------------------------------------------------*/

#if 0
/*------------------------------------------------------------------------*\
                    C A S H I E R   M a i n t e n a n c e
\*------------------------------------------------------------------------*/
/*
CONST UCHAR FARCONST aszMldAC20[][64] = {
                            "....*....1....*....2....*....3....*....4",
                            "Name",
                            "%-10s",
                            "%-10s TRAINEE",
                            ""
                            };
*/
CONST MLDMENU FARCONST aszMldAC20[] = {
                            {-1,_T("")},
                            {LCD_CAS_ADDR1_ADR, _T(" 1 %s\t%4s ")},
                            {LCD_CAS_ADDR2_ADR, _T(" 2 %s\t%4s ")},
                            {LCD_CAS_ADDR3_ADR, _T(" 3 %s\t%4s ")},
                            {LCD_CAS_ADDR4_ADR, _T(" 4 %s\t%4s ")},
							{LCD_CAS_ADDR11_ADR, _T(" 5 %s\t%4s ")},
                            {LCD_CAS_ADDR5_ADR, _T(" 6 %s\t%4u ")},
                            {LCD_CAS_ADDR6_ADR, _T(" 7 %s\t%4u ")},
                            {LCD_CAS_ADDR7_ADR, _T(" 8 %s\t%1s ")},
                            {LCD_CAS_ADDR8_ADR, _T(" 9 %s\t%1s ")},
                            {LCD_CAS_ADDR9_ADR, _T("10 %s\t%2s ")},
                            {LCD_CAS_ADDR10_ADR, _T("11 %s\t%20s ")},
                            {0,_T("")}
                            };
/***
CONST UCHAR FARCONST aszMldAC20[][64] = {
                            "....*....1....*....2....*....3....*....4",
                            " 1  Operator Status #1\t%4s",
                            " 2  Operator Status #2\t%4s",
                            " 3  Operator Status #3\t%4s",
                            " 4  Operator Status #4\t%4s",
							" 5  Operator Status #5\t%4s",
                            " 6  Guest Check Number From\t%4u",
                            " 7  Guest Check Number To\t%4u",
                            " 8  Charge Tip Rate\t%1s",
                            " 9  Team Number \t%1s",
                            "10  Allowed Terminal No\t%2s",
                            "11  ACCII Character -%20s,
                            ""
                            };
***/
#endif
/*------------------------------------------------------------------------*\
                        P L U   M a i n t e n a n c e
\*------------------------------------------------------------------------*/

#if 0
/*------------------------------------------------------------------------*\
                        P P I   M a i n t e n a n c e
\*------------------------------------------------------------------------*/
CONST TCHAR FARCONST aszMldAC71[][64] = {
                            _T("....*....1....*....2....*....3....*....4"),
                            _T("\t%s %s"),
/*                            "\tQTY PRICE  ", */
                            _T("%2d\t%3d %6lu "),
                            _T("")
                            };
CONST TCHAR FARCONST aszMldAC71_AddSettingsTitle[][64] = {
                            _T("....*....1....*....2....*....3....*....4"),
                            _T(" %s \t%s"),
/*                            "\tQTY PRICE  ", */
                            _T(" %s \t%6lu "),
                            _T("")
                            };
CONST TCHAR FARCONST aszMldAC71_AddSettings[][64] = {
                            _T("....*....1....*....2....*....3....*....4"),
                            _T("\t%s %s"),
/*                            "\tQTY PRICE  ", */
                            _T("%2d %3s \t%6lu "),
                            _T("")
                            };
#endif

/*------------------------------------------------------------------------*\
            A S S I G N   S E T   M E N U   M a i n t e n a n c e
\*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*\
                        T A X   M a i n t e n a n c e
\*------------------------------------------------------------------------*/




/*------------------------------------------------------------------------*\
                    C O U P O N   M a i n t e n a n c e
\*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*\

                        E N D   O F   H E A D E R

\*------------------------------------------------------------------------*/

#endif
