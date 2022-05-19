/*========================================================================*\
||                                                                        ||
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995-2000       ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                Release 3.0               ||
||                                                                        ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:   Title              : NCR 2170 Header Definition                        :
:   Category           : SUPER & PROGRAM MODE,ECR 2170                     :
:   Program Name       :                                                   :
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments              :
:   Program Written    : 92-05-05 : 00.00.00     :                         :
\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*------------------------------------------------------------------------*/

/*
		Directions, instructions, and bits of good advice.

	When making changes to the Supervisor and Program Mode menus, you will
	need to do a couple of things.
		- allocate an unused MLD mnemonic for the new menu item (P65)
		- assign a unique number to the action code (AC) or program code (P)
		- create the #define for the MLD in paraequ.h for instance LCD_PG95_ADR
		- add the menu item to the tables below
		- add any new sub-menu items needed to the sub-menu trees or create
		  new sub-menu trees

	How the code works.
	In the function UifACMenu() in file uifacmod.c the code checks to see
	the role of the terminal - Master, Backup Master, or Satellite - and then
	displays the appropriate menu array - ACEntryMenu, ACEntryMenuBMAST, or
	ACEntryMenuSatt respectively.
	
	Each menu array contains one or more entries for a list of menu items, the
	list being a single display page.  The rule of thumb is each of the display
	pages should contain no more than 20 menu items.

	Some of the Supervisor and Program Mode menu items have sub-menus.  Most of
	the report types of menu items, for instance, have a sub-menu allowing the
	user to pick the type of report to generate such as a detailed report or a
	summary report.

	The sub-menus are accessed from the specific menu entry function which in
	turn displays the sub-menu.  For an example take a look at the source code
	supporting the AC26 menu item on the Supervisor Menu.  This code can be
	found in file uifac26.c with the menu function starting with UifAC26Function ().

	As you look at the code you will see how different sub-menus are displayed based
	on where the user is in the menu tree.

	Notice that the function UieNextMenu() is used to go to the next menu state and
	that the function UifACPGMLDDispCom () is used to display the appropriate sub-menu.

 */

#include	<tchar.h>
#include "ecr.h"
#include "uie.h"
#include "paraequ.h"
#include "mldmenu.h"

/*------------------------------------------------------------------------*\

                    S U P E R V I S O R   M O D E

\*------------------------------------------------------------------------*/

static  MLDMENU   CONST    DummyLine[]   = { -1, _T("* 123") };  // Function UifACPGMLDDispCom() starts with array item 1 not 0
static  MLDMENU   CONST    Terminater[]  = { 0, _T("\0") };      // indicated end of menu page for function UifACPGMLDDispCom().

/*------------------------------------------------------------------------*\
                    A c t i o n   C o d e   E n t r y
\*------------------------------------------------------------------------*/

//                                            P65 MLD Addr.   AC Number Desc.
static  MLDMENU   CONST    ACNo1[]     = { LCD_AC1_ADR,  _T("    1") }; // AC1 Guest Check Assignment
static  MLDMENU   CONST    ACNo2[]     = { LCD_AC2_ADR,  _T("*   2") }; // AC2 Guest Check Report, AC_GCFL_READ_RPT
static  MLDMENU   CONST    ACNo3[]     = { LCD_AC3_ADR,  _T("    3") }; // AC3 Guest Check Settlement
static  MLDMENU   CONST    ACNo4[]     = { LCD_AC4_ADR,  _T("    4") }; // AC4 Set PLU No on Menu
static  MLDMENU   CONST    ACNo5[]     = { LCD_AC5_ADR,  _T("    5") }; // AC5 Set Menu Control Table
static  MLDMENU   CONST    ACNo6[]     = { LCD_AC6_ADR,  _T("    6") }; // AC6 Manual Alter Remote Printer
static  MLDMENU   CONST    ACNo7[]     = { LCD_AC7_ADR,  _T("    7") }; // AC7 Cashier A/B Key Assignment
//static  UCHAR     CONST    ACNo9[]     = { LCD_AC9_ADR,  _T("    9") };  // AC9 Assign Guest Check No Range
static  MLDMENU   CONST    ACNo10[]    = { LCD_AC10_ADR, _T("   10") }; // AC10 
static  MLDMENU   CONST    ACNo11[]    = { LCD_AC11_ADR, _T("   11") }; // AC11 
static  MLDMENU   CONST    ACNo15[]    = { LCD_AC15_ADR, _T("   15") }; // AC15 Change Supervisor Secret No
static  MLDMENU   CONST    ACNo17[]    = { LCD_AC17_ADR, _T("   17") }; // Ac17 Set Date and Time
static  MLDMENU   CONST    ACNo18[]    = { LCD_AC18_ADR, _T("   18") }; // AC18 Store Reset Report
static  MLDMENU   CONST    ACNo19[]    = { LCD_AC19_ADR, _T("   19") }; // AC19 Cashier Settlement
static  MLDMENU   CONST    ACNo20[]    = { LCD_AC20_ADR, _T("   20") }; // AC20 Cashier Assignment
static  MLDMENU   CONST    ACNo21[]    = { LCD_AC21_ADR, _T("*  21") }; // AC21 Cashier Report, AC_CASH_READ_RPT
static  MLDMENU   CONST    ACNo23[]    = { LCD_AC23_ADR, _T("*  23") }; // AC23 Financial Report, AC_REGFIN_READ_RPT
static  MLDMENU   CONST    ACNo24[]    = { LCD_AC24_ADR, _T("*  24") }; // AC24 Hourly Activity Report, AC_HOURLY_READ_RPT
static  MLDMENU   CONST    ACNo26[]    = { LCD_AC26_ADR, _T("*  26") }; // AC26 Department Report, AC_DEPTSALE_READ_RPT
static  MLDMENU   CONST    ACNo29[]    = { LCD_AC29_ADR, _T("*  29") }; // AC29 PLU Report, AC_PLUSALE_READ_RPT
static  MLDMENU   CONST    ACNo30[]    = { LCD_AC30_ADR, _T("*  30") }; // AC30 Coupon Report, AC_CPN_READ_RPT
//GroupReport - CSMALL
static	MLDMENU	  CONST	  ACNo31[]	  = { LCD_AC31_ADR, _T("*  31") }; // AC31 PLU Group Summary Report, AC_PLUGROUP_READ_RPT

static  MLDMENU   CONST    ACNo33[]    = { LCD_AC33_ADR, _T("   33") };  // AC33 Set Control String Table
static  MLDMENU   CONST    ACNo39[]    = { LCD_AC39_ADR, _T("   39") };  // AC39 PLU Reset Report
static  MLDMENU   CONST    ACNo40[]    = { LCD_AC40_ADR, _T("   40") };  // AC40 Coupon Reset Report
static  MLDMENU   CONST    ACNo42[]    = { LCD_AC42_ADR, _T("   42") };  // AC42 File Syncronization
static  MLDMENU   CONST    ACNo63[]    = { LCD_AC63_ADR, _T("   63") };  // AC63 PLU Maintenance
static  MLDMENU   CONST    ACNo64[]    = { LCD_AC64_ADR, _T("   64") };  // AC64 PLU Price Change
//static  MLDMENU   CONST    ACNo66[]    = { LCD_AC66_ADR,"   66" };       // AC66 Broadcast PLU Parameter
static  MLDMENU   CONST    ACNo68[]    = { LCD_AC68_ADR, _T("   68") };  // AC68 Add/Delete PLU
static  MLDMENU   CONST    ACNo70[]    = { LCD_AC70_ADR, _T("*  70") };  // AC70 Electronic Journal Report, AC_EJ_READ_RPT
static  MLDMENU   CONST    ACNo71[]    = { LCD_AC71_ADR, _T("   71") };  // AC71 Set PPI Table of PLU
static  MLDMENU   CONST    ACNo72[]    = { LCD_AC72_ADR, _T("   72") };  // AC72 PLU Parameter Report
static  MLDMENU   CONST    ACNo75[]    = { LCD_AC75_ADR, _T("   75") };  // AC75 Broadcast Terminal Parameter
static  MLDMENU   CONST    ACNo78[]    = { LCD_AC78_ADR, _T("   78") };  // AC78 Start/Stop Charge Posting
static  MLDMENU   CONST    ACNo82[]    = { LCD_AC82_ADR, _T("   82") };  // AC82 Change PLU Mnemonic
static  MLDMENU   CONST    ACNo84[]    = { LCD_AC84_ADR, _T("   84") };  // AC84 Set Rounding Table
static  MLDMENU   CONST    ACNo85[]    = { LCD_AC85_ADR, _T("   85") };  // AC85 Disconnect Master or B-Master
static  MLDMENU   CONST    ACNo86[]    = { LCD_AC86_ADR, _T("   86") };  // AC86 Set Percent Rate
static  MLDMENU   CONST    ACNo87[]    = { LCD_AC87_ADR, _T("   87") };  // AC87 Set Soft Check Trailer Message
static  MLDMENU   CONST    ACNo88[]    = { LCD_AC88_ADR, _T("   88") };  // AC88 Set Sales Promotion Message
static  MLDMENU   CONST    ACNo89[]    = { LCD_AC89_ADR, _T("   89") };  // AC89 Set Currency Conversion Rate
static  MLDMENU   CONST    ACNo97[]    = { LCD_AC97_ADR, _T("   97") };  // AC97 CPM/EPT Tally Report

static  MLDMENU   CONST    ACNo99[]   = { LCD_AC99_ADR,  _T("   99") };  // AC99 End of Day Reset Report
static  MLDMENU   CONST    ACNo100[]  = { LCD_AC100_ADR, _T("  100") };  // AC100 Electro Journal Reset Report
static  MLDMENU   CONST    ACNo102[]  = { LCD_AC102_ADR, _T("* 102") };  // AC102 Store and Forward, AC_STORE_FORWARD
static  MLDMENU   CONST    ACNo103[]  = { LCD_AC103_ADR, _T("  103") };  // AC103 Guest Check File Close
static  MLDMENU   CONST    ACNo111[]  = { LCD_AC111_ADR, _T("  111") };  // AC111 Set Tare Table
static  MLDMENU   CONST    ACNo114[]  = { LCD_AC114_ADR, _T("  114") };  // AC114 Department Maintenance
//static  MLDMENU   CONST    ACNo115[]  = { LCD_AC115_ADR,"  115" };
static  MLDMENU   CONST    ACNo116[]  = { LCD_AC116_ADR, _T("  116") };  // AC116 Set Promotional PLU
static  MLDMENU   CONST    ACNo119[]  = { LCD_AC119_ADR, _T("  119") };  // AC119 Cashier File Close 
static  MLDMENU   CONST    ACNo122[]  = { LCD_AC122_ADR, _T("* 122") };  // AC122 Major Department Report, AC_MAJDEPT_RPT
static  MLDMENU   CONST    ACNo123[]  = { LCD_AC123_ADR, _T("* 123") };  // AC123 Media Flash Report, AC_MEDIA_RPT
static  MLDMENU   CONST    ACNo124[]  = { LCD_AC124_ADR, _T("  124") };  // AC124 Set Tax Table1
static  MLDMENU   CONST    ACNo125[]  = { LCD_AC125_ADR, _T("  125") };  // AC125 Set Tax Table2
static  MLDMENU   CONST    ACNo126[]  = { LCD_AC126_ADR, _T("  126") };  // AC126 Set Tax Table3
static  MLDMENU   CONST    ACNo127[]  = { LCD_AC127_ADR, _T("  127") };  // AC127 Set Tax Rate
static  MLDMENU   CONST    ACNo128[]  = { LCD_AC128_ADR, _T("  128") };  // AC128 Set Transaction Limit Foodstamps or coupons
static  MLDMENU   CONST    ACNo129[]  = { LCD_AC129_ADR, _T("  129") };  // AC129 Set Tax Table4
static  MLDMENU   CONST    ACNo130[]  = { LCD_AC130_ADR, _T("  130") };  // AC130 Set Item Count Table
static  MLDMENU   CONST    ACNo131[]  = { LCD_AC131_ADR, _T("* 131") };  // AC131 Service Time Report, AC_SERVICE_READ_RPT
static  MLDMENU   CONST    ACNo132[]  = { LCD_AC132_ADR, _T("  132") };  // AC132 Service Time Reset Report
static  MLDMENU   CONST    ACNo133[]  = { LCD_AC133_ADR, _T("  133") };  // AC133 Set Service Time Parameter

static  MLDMENU   CONST    ACNo135[]  = { LCD_AC135_ADR, _T("* 135") }; // AC135 Programable Report, AC_PROG_READ_RPT
static  MLDMENU   CONST    ACNo136[]  = { LCD_AC136_ADR, _T("  136") }; // AC136 Programable Reset Report
static  MLDMENU   CONST    ACNo137[]  = { LCD_AC137_ADR, _T("  137") }; // AC137 Set Beverage Dispenser Parameter
//static  MLDMENU   CONST    ACNo146[]  = { LCD_AC146_ADR, _T("  146") }; // 146 Server File Close
static  MLDMENU   CONST    ACNo150[]  = { LCD_AC150_ADR, _T("* 150") }; // AC150 ETK File Report, AC_ETK_READ_RPT
static  MLDMENU   CONST    ACNo151[]  = { LCD_AC151_ADR, _T("  151") }; // AC151 ETK File Reset Report, AC_ETK_RESET_RPT
static  MLDMENU   CONST    ACNo152[]  = { LCD_AC152_ADR, _T("  152") }; // AC152 ETK Assignment
static  MLDMENU   CONST    ACNo153[]  = { LCD_AC153_ADR, _T("  153") }; // AC153 ETK File Maintenance
static  MLDMENU   CONST    ACNo154[]  = { LCD_AC154_ADR, _T("  154") }; // AC154 Set ETK Wage Rate for Job Code
static  MLDMENU   CONST    ACNo160[]  = { LCD_AC160_ADR, _T("  160") }; // AC160 Set Order Entry Prompt Table
static  MLDMENU   CONST    ACNo161[]  = { LCD_AC161_ADR, _T("  161") }; // AC161 Coupon Maintenance
static  MLDMENU   CONST    ACNo162[]  = { LCD_AC162_ADR, _T("  162") }; // AC162 Set Drive Through Parameter
static  MLDMENU   CONST    ACNo169[]  = { LCD_AC169_ADR, _T("  169") }; // AC169 Adjust Tone Volume
static  MLDMENU   CONST    ACNo170[]  = { LCD_AC170_ADR, _T("  170") };
static  MLDMENU   CONST    ACNo175[]  = { LCD_AC175_ADR, _T("  175") }; // AC175 Broadcast Super Mode Para
static  MLDMENU   CONST    ACNo176[]  = { LCD_AC176_ADR, _T("* 176") }; // AC176 Ope/Guest Chk Status Report, AC_CSOPN_RPT
static	MLDMENU   CONST	  ACNo194[]  = { LCD_AC194_ADR, _T("  194") }; // AC194 Dynamic Window Reload
static  MLDMENU   CONST    ACNo208[]  = { LCD_AC208_ADR, _T("  208") };
static  MLDMENU   CONST    ACNo223[]  = { LCD_AC223_ADR, _T("* 223") }; // AC223 Individual Financial Report, AC_IND_READ_RPT
static  MLDMENU   CONST    ACNo233[]  = { LCD_AC233_ADR, _T("  233") }; // AC233 Individual Financial Reset Report
static  MLDMENU   CONST    ACNo444[]  = { LCD_AC444_ADR, _T("  444") }; // AC444 Change Display Layout, CID_AC444FUNCTION
static  MLDMENU   CONST    ACNo777[]  = { LCD_AC777_ADR, _T("  777") }; // AC777 Change Display Layout, CID_AC777FUNCTION
static  MLDMENU   CONST    ACNo888[]  = { LCD_AC888_ADR, _T("* 888") }; // AC888 Print/Display Version Information, CID_AC888FUNCTION, AC_CONFIG_INFO
//static  MLDMENU   CONST    ACNo999[]     = { LCD_AC999_ADR, _T("  999") }; // AC999 Application shutdown

// -------------
// Create the menu trees, each of the ACEntryxxxx arrays being for a single page
// of menu items to be displayed.
//
// WARNING:  See also ACEntryMenuTouch[] below which uses some of these same
//           MLDMENU arrays to create the Supervisor menu for Touchscreen.
//           AC5, which modifies the FSC lookup table, is removed from the
//           Touchscreen version of the Master Terminal menus.

MLDMENU CONST  *  ACEntryMenu[] = {
	ACNo1,   ACNo2,   ACNo3,   ACNo4,
	ACNo5,   ACNo6,   ACNo7,   ACNo10,
	ACNo11,  ACNo15,  ACNo17,  ACNo18,
	ACNo19,  ACNo20,  ACNo21,  ACNo23,
	ACNo24,  ACNo26,  ACNo29,  ACNo30,
	Terminater,
	ACNo31,  ACNo33,  ACNo39,  ACNo40,
	ACNo42,  ACNo63,  ACNo64,  ACNo68,
	ACNo70,  ACNo71,  ACNo72,  ACNo75,
	ACNo78,  ACNo82,  ACNo84,  ACNo85,
	ACNo86,  ACNo87,  ACNo88,  ACNo89,
	Terminater,
	ACNo97,  ACNo99,  ACNo100, ACNo102,
	ACNo103, ACNo111, ACNo114, ACNo116,
	ACNo119, ACNo122, ACNo123, ACNo124,
	ACNo125, ACNo126, ACNo127, ACNo128,
	ACNo129, ACNo130, ACNo131, ACNo132,
	Terminater,
	ACNo133, ACNo135, ACNo136, ACNo137,
	ACNo150, ACNo151, ACNo152, ACNo153,
	ACNo154, ACNo160, ACNo161, ACNo162,
	ACNo169, ACNo170, ACNo175, ACNo176,
	ACNo194, ACNo208, ACNo223, ACNo233,
	Terminater,
	ACNo777, ACNo888,
	Terminater,
	Terminater
};

MLDMENU CONST  *  ACEntryDisconnectedMenu[] = {
	ACNo1,   ACNo2,   ACNo3,   ACNo4,
	ACNo5,   ACNo6,   ACNo7,   ACNo10,
	ACNo11,  ACNo15,  ACNo17,  ACNo18,
	ACNo19,  ACNo20,  ACNo21,  ACNo23,
	ACNo24,  ACNo26,  ACNo29,  ACNo30,
	Terminater,
	ACNo31,  ACNo33,  ACNo39,  ACNo40,
	ACNo42,  ACNo63,  ACNo64,  ACNo68,
	ACNo70,  ACNo71,  ACNo72,  ACNo75,
	ACNo78,  ACNo82,  ACNo84,  ACNo85,
	ACNo86,  ACNo87,  ACNo88,  ACNo89,
	Terminater,
	ACNo97,  ACNo99,  ACNo100, ACNo102,
	ACNo103, ACNo111, ACNo114, ACNo116,
	ACNo119, ACNo122, ACNo123, ACNo124,
	ACNo125, ACNo126, ACNo127, ACNo128,
	ACNo129, ACNo130, ACNo131, ACNo132,
	Terminater,
	ACNo133, ACNo135, ACNo136, ACNo137,
	ACNo150, ACNo151, ACNo152, ACNo153,
	ACNo154, ACNo160, ACNo161, ACNo162,
	ACNo169, ACNo170, ACNo175, ACNo176,
	ACNo194, ACNo208, ACNo223, ACNo233,
	Terminater,
	ACNo444, ACNo777, ACNo888,
	Terminater,
	Terminater
};

// Create the menu tree for a Satellite Terminal to display
MLDMENU CONST  *  ACEntryMenuSatt[] = {
	ACNo2,   ACNo7,   ACNo17,  ACNo21,  ACNo23,
	ACNo24,  ACNo26,  ACNo29,  ACNo31,  ACNo70,
	ACNo75,  ACNo97,  ACNo100, ACNo102, ACNo123,
	ACNo150, ACNo162, ACNo169, ACNo175, ACNo194, 
	ACNo223,
	Terminater,
	ACNo777, ACNo888,
	Terminater,
	Terminater
};

// Create the menu tree for a Satellite Terminal to display
MLDMENU CONST  *  ACEntryMenuDisconnectedSatt[] = {
	ACNo2,   ACNo7,   ACNo17,  ACNo21,  ACNo23,
	ACNo24,  ACNo26,  ACNo29,  ACNo31,  ACNo70,
	ACNo75,  ACNo97,  ACNo100, ACNo102, ACNo123,
	ACNo150, ACNo162, ACNo169, ACNo175, ACNo194, 
	ACNo223,
	Terminater,
	ACNo444, ACNo777, ACNo888,
	Terminater,
	Terminater
};

//----------------
// Create the menu tree for a Backup Master Terminal to display
// when it is not a Master Terminal
MLDMENU CONST  *  ACEntryMenuBMast[] = {
	ACNo2,   ACNo4,   ACNo5,   ACNo7,   ACNo17,
	ACNo21,  ACNo23,  ACNo24,  ACNo26,  ACNo29,
	ACNo31,  ACNo70,  ACNo75,  ACNo85,  ACNo97,
	ACNo100, ACNo102, ACNo123, ACNo150, ACNo162, 
	ACNo169,
	Terminater,
	ACNo175, ACNo194, ACNo223, ACNo777,  ACNo888,
	Terminater,
	Terminater
};

MLDMENU CONST  *  ACEntryMenuTouch[] = {
	ACNo1,   ACNo2,   ACNo3,   
	         ACNo6,   ACNo7,   ACNo10,
	ACNo11,  ACNo15,  ACNo17,  ACNo18,
	ACNo19,  ACNo20,  ACNo21,  ACNo23,
	ACNo24,  ACNo26,  ACNo29,  ACNo30,
	Terminater,
	ACNo31,  ACNo33,  ACNo39,  ACNo40,
	ACNo42,  ACNo63,  ACNo64,  ACNo68,
	ACNo70,  ACNo71,  ACNo72,  ACNo75,
	ACNo78,  ACNo82,  ACNo84,  ACNo85,
	ACNo86,  ACNo87,  ACNo88,  ACNo89,
	Terminater,
	ACNo97,  ACNo99,  ACNo100, ACNo102,
	ACNo103, ACNo111, ACNo114, ACNo116,
	ACNo119, ACNo122, ACNo123, ACNo124,
	ACNo125, ACNo126, ACNo127, ACNo128,
	ACNo129, ACNo130, ACNo131, ACNo132,
	Terminater,
	ACNo133, ACNo135, ACNo136, ACNo137,
	ACNo150, ACNo151, ACNo152, ACNo153,
	ACNo154, ACNo160, ACNo161, ACNo162,
	ACNo169, ACNo170, ACNo175, ACNo176,
	ACNo194, ACNo208, ACNo223, ACNo233,
	Terminater,
	ACNo777, ACNo888,
	Terminater,
	Terminater
};

MLDMENU CONST  *  ACEntryMenuDisconnectedTouch[] = {
	ACNo1,   ACNo2,   ACNo3,   
	         ACNo6,   ACNo7,   ACNo10,
	ACNo11,  ACNo15,  ACNo17,  ACNo18,
	ACNo19,  ACNo20,  ACNo21,  ACNo23,
	ACNo24,  ACNo26,  ACNo29,  ACNo30,
	Terminater,
	ACNo31,  ACNo33,  ACNo39,  ACNo40,
	ACNo42,  ACNo63,  ACNo64,  ACNo68,
	ACNo70,  ACNo71,  ACNo72,  ACNo75,
	ACNo78,  ACNo82,  ACNo84,  ACNo85,
	ACNo86,  ACNo87,  ACNo88,  ACNo89,
	Terminater,
	ACNo97,  ACNo99,  ACNo100, ACNo102,
	ACNo103, ACNo111, ACNo114, ACNo116,
	ACNo119, ACNo122, ACNo123, ACNo124,
	ACNo125, ACNo126, ACNo127, ACNo128,
	ACNo129, ACNo130, ACNo131, ACNo132,
	Terminater,
	ACNo133, ACNo135, ACNo136, ACNo137,
	ACNo150, ACNo151, ACNo152, ACNo153,
	ACNo154, ACNo160, ACNo161, ACNo162,
	ACNo169, ACNo170, ACNo175, ACNo176,
	ACNo194, ACNo208, ACNo223, ACNo233,
	Terminater,
	ACNo444, ACNo777, ACNo888,
	Terminater,
	Terminater
};

MLDMENU CONST  *  ACEntryMenuBMastTouch[] = {
	ACNo2,   ACNo7,   ACNo17,  ACNo21,  ACNo23,
	ACNo24,  ACNo26,  ACNo29,  ACNo70,  ACNo75,
	ACNo85,  ACNo97,  ACNo100, ACNo102, ACNo123, 
	ACNo150, ACNo162, ACNo169, ACNo175, ACNo194, 
	ACNo223,
	Terminater,
	ACNo777, ACNo888,
	Terminater,
	Terminater
};

// Create the menu tree for a Satellite Terminal to display
MLDMENU CONST  *  ACEntryMenuSattTouch[] = {
	ACNo2,   ACNo7,   ACNo17,  ACNo21,  ACNo23,
	ACNo24,  ACNo26,  ACNo29,  ACNo70,  ACNo75,
	ACNo97,  ACNo100, ACNo102, ACNo123, ACNo150, 
	ACNo162, ACNo169, ACNo175, ACNo194, ACNo223, 
	ACNo777,
	Terminater,
	ACNo888,
	Terminater,
	Terminater
};

// Create the menu tree for a Disconnected Satellite Terminal to display
MLDMENU CONST  *  ACEntryMenuDisconnectedSattTouch[] = {
	ACNo2,   ACNo7,   ACNo17,  ACNo21,  ACNo23,
	ACNo24,  ACNo26,  ACNo29,  ACNo70,  ACNo75,
	ACNo97,  ACNo100, ACNo102, ACNo123, ACNo150, 
	ACNo162, ACNo169, ACNo175, ACNo194, ACNo223, 
	ACNo444,
	Terminater,
	ACNo777, ACNo888,
	Terminater,
	Terminater
};
// ========================================================================
/*------------------------------------------------------------------------*\
                         C o m m o n   S u b M e n u s

	The following sub-menu trees are shared between multiple Action Codes.
	For instance, several of the report Action Codes use the DaylyPTDSelect
	sub-menu tree to display options to the user.

	These menus are used with function UifACPGMLDDispCom() to present to the
	operator the actions that can be done.
\*------------------------------------------------------------------------*/


/*****                ( B l a n k   o r   R e p o r t )               *****/


/*------------------------------------------------------------------------*\
                    D a i l y / P T D   S e l e c t i o n
\*------------------------------------------------------------------------*/

static  MLDMENU   CONST    DPSelDAY[]     = { LCD_RPT_DAILY_ADR, _T("    1") };      // 1 Daily Report, RPT_TYPE_DALY
static  MLDMENU   CONST    DPSelPTD[]     = { LCD_RPT_PDT_ADR,   _T("    2") };      // 2 PTD Report, RPT_TYPE_PTD
static  MLDMENU   CONST    DPSelDAYSAVE[] = { LCD_RPT_DAILYSAVE_ADR, _T("    3") };  // 3 Previous or Saved Daily Report, RPT_TYPE_DALYSAVE
static  MLDMENU   CONST    DPSelPTDSAVE[] = { LCD_RPT_PDTSAVE_ADR,   _T("    4") };  // 4 Previous or Saved PTD Report, RPT_TYPE_PTDSAVE

MLDMENU CONST    *   DaylyPTDSelect[] = {
	DPSelDAY,  DPSelPTD,
	Terminater,
	Terminater
};

MLDMENU CONST    *   DaylyPTDSaveSelect[] = {
	DPSelDAY,  DPSelPTD,
	DPSelDAYSAVE,  DPSelPTDSAVE,
	Terminater,
	Terminater
};

/*------------------------------------------------------------------------*\
                    R e s e t   T y p e   S e l e c t i o n
\*------------------------------------------------------------------------*/

static  MLDMENU   CONST    ResetType1[]     = { LCD_RPT_SAVED_ADR,      _T("    1") }; // 1 Report Saved Total
static  MLDMENU   CONST    ResetType2[]     = { LCD_RPT_READ_RESET_ADR, _T("    2") }; // 2 Report and Reset Active Total
static  MLDMENU   CONST    ResetType3[]     = { LCD_RPT_ONLY_RESET_ADR, _T("    3") }; // 3 Only Reset Active Total

MLDMENU CONST    *   ResetTypeSelect[] = {
	ResetType1, ResetType2, ResetType3,
	Terminater,
	Terminater
};

/*------------------------------------------------------------------------*\
            T y p e   S e l e c t i o n   f o r   R e p o r t
\*------------------------------------------------------------------------*/

static  MLDMENU   CONST    ReportType1_1[]     = { LCD_RPT_ALL_READ_ADR, _T("    1") }; // 1 All Report, RPT_TYPE_ALL
static  MLDMENU   CONST    ReportType1_2[]     = { LCD_RPT_IND_READ_ADR, _T("    2") }; // 2 Individual Report, RPT_TYPE_IND
static  MLDMENU   CONST    ReportType1_3[]     = { LCD_RPT_SERV_ADR,     _T("    3") }; // 3 Report by Server, RPT_TYPE_WAIIND

MLDMENU CONST    *   TypeSelectReport1[] = {
	ReportType1_1, ReportType1_2, ReportType1_3,
	Terminater,
	Terminater
};

static  MLDMENU   CONST    ReportType2_1[]     = { LCD_RPT_ALL_RESET, _T("    1") }; // 1 All Settlement
static  MLDMENU   CONST    ReportType2_2[]     = { LCD_RPT_IND_RESET, _T("    2") }; // 2 Individual Settlement


MLDMENU CONST    *   TypeSelectReport2[] = {
	ReportType2_1, ReportType2_2,
	Terminater,
	Terminater
};

static  MLDMENU   CONST    ReportType3_1[]     = { LCD_RPT_ALL_READ_ADR, _T("    1") }; // 1 All Report
static  MLDMENU   CONST    ReportType3_2[]     = { LCD_RPT_RANGE_ADR,    _T("    2") }; // 2 Report by Range

MLDMENU CONST    *   TypeSelectReport3[] = {
	ReportType3_1, ReportType3_2,
	Terminater,
	Terminater
};

static  MLDMENU   CONST    ReportType4_1[]     = { LCD_RPT_ALL_READ_ADR, _T("    1") }; // 1 All Report
static  MLDMENU   CONST    ReportType4_2[]     = { LCD_RPT_MAJOR_ADR,    _T("    2") }; // 2 Report by Major Department
static  MLDMENU   CONST    ReportType4_3[]     = { LCD_RPT_IND_READ_ADR, _T("    3") }; // 3 Individual Report

MLDMENU CONST    *   TypeSelectReport4[] = {
	ReportType4_1, ReportType4_2, ReportType4_3,
	Terminater,
	Terminater
};

static  MLDMENU   CONST    ReportType5_1[]     = { LCD_RPT_ALL_READ_ADR, _T("    1") }; // 1 All Report
static  MLDMENU   CONST    ReportType5_2[]     = { LCD_RPT_DEPT_ADR,     _T("    2") }; // 2 Report by Department
static  MLDMENU   CONST    ReportType5_3[]     = { LCD_RPT_CODE_ADR,     _T("    3") }; // 3 Report by Report Code
static  MLDMENU   CONST    ReportType5_4[]     = { LCD_RPT_IND_READ_ADR, _T("    4") }; // 4 Individual Report


MLDMENU CONST    *   TypeSelectReport5[] = {
	ReportType5_1, ReportType5_2,
	ReportType5_3, ReportType5_4,
	Terminater,
	Terminater
};

static  MLDMENU   CONST    ReportType6_1[]     = { LCD_RPT_ALL_READ_ADR, _T("    1") }; // 1 All Report
static  MLDMENU   CONST    ReportType6_2[]     = { LCD_RPT_IND_READ_ADR, _T("    2") }; // 2 Individual Report

MLDMENU CONST    *   TypeSelectReport6[] = {
	ReportType6_1, ReportType6_2,
	Terminater,
	Terminater
};

static  MLDMENU   CONST    ReportType7_1[]     = { LCD_CPM_START_ADR, _T("    1") }; // 1 Start Charge Posting
static  MLDMENU   CONST    ReportType7_2[]     = { LCD_CPM_STOP_ADR,  _T("    2") }; // Stop Charge Posting

MLDMENU CONST    *   TypeSelectReport7[] = {
	ReportType7_1, ReportType7_2,
	Terminater,
	Terminater
};

static  MLDMENU   CONST    ReportType8_1[]     = { LCD_CPM_READ_ADR,  _T("    1") }; // 1 CPM Tally Read
static  MLDMENU   CONST    ReportType8_2[]     = { LCD_CPM_RESET_ADR, _T("    2") }; // 2 CPM Tally Reset
static  MLDMENU   CONST    ReportType8_3[]     = { LCD_EPT_READ_ADR,  _T("    3") }; // 3 EPT Tally Read
static  MLDMENU   CONST    ReportType8_4[]     = { LCD_EPT_RESET_ADR, _T("    4") }; // 4 EPT Tally Reset


MLDMENU CONST    *   TypeSelectReport8[] = {
	ReportType8_1, ReportType8_2,
	ReportType8_3, ReportType8_4,
	Terminater,
	Terminater
};

/**
static  MLDMENU   CONST    ReportType9_1[]     = { LCD_RPT_ALL_RESET,_T("    1") };
static  MLDMENU   CONST    ReportType9_2[]     = { LCD_RPT_IND_RESET,_T("    2") };

/
static  UCHAR   CONST    ReportType9_1[]     = { "All Report                            1" };
static  UCHAR   CONST    ReportType9_2[]     = { "Individual Report                     2" };
/
 UCHAR CONST    *   TypeSelectReport9[] = {
                                   (UCHAR *)&DummyLine,
                                   (UCHAR *)&ReportType9_1,(UCHAR *)&ReportType9_2,
                                   (UCHAR *)&Terminater };
***/
/* V3.3 */
static  MLDMENU   CONST    ReportType10_1[]     = { LCD_RPT_ALL_RESET, _T("    1") }; // 1 All Reset
static  MLDMENU   CONST    ReportType10_2[]     = { LCD_RPT_IND_RESET, _T("    2") }; // 2 Individual Reset
static  MLDMENU   CONST    ReportType10_3[]     = { LCD_RPT_SERV_ADR,  _T("    3") }; // 3 Report by Server
static  MLDMENU	  CONST	  ReportType10_4[]	   = { LCD_OPTIMIZE_ADR,  _T("    4") };

MLDMENU CONST    *   TypeSelectReport10[] = {
	ReportType10_1, ReportType10_2, ReportType10_3,ReportType10_4,
	Terminater,
	Terminater
};

// following used with AC102 Store and Forward, main function UifAC102Function().
static  MLDMENU   CONST    StoreForwardSubMenu_1[]     = { LCD_AC102_ALT_CREDIT,  _T("    1") }; // 1 Store and Forward for Credit
static  MLDMENU   CONST    StoreForwardSubMenu_2[]     = { LCD_AC102_ALT_EPD,     _T("    2") }; // 2 Store and Forward for Employee Payroll Deduction (VCS)
MLDMENU CONST    *   TypeStoreForwardSubMenu[] = {
	StoreForwardSubMenu_1, StoreForwardSubMenu_2,
	Terminater,
	Terminater
};

static  MLDMENU   CONST    StoreForwardToggle_1[]     = { LCD_AC102_SF_TOGGLE,  _T("    1") }; // 1 Toggle Store and Forward
static  MLDMENU   CONST    StoreForwardReport_2[]     = { LCD_AC102_SF_REPORT,  _T("    2") }; // 2 Store and Forward Report
static  MLDMENU   CONST    StoreForwardForward_3[]    = { LCD_AC102_SF_FORWARD, _T("    3") }; // 3 Forward Stored Transactions
static  MLDMENU   CONST    StoreForwardForward_4[]    = { LCD_AC102_SF_DELETE,  _T("    4") }; // 3 Forward Stored Transactions


MLDMENU CONST    *   TypeStoreForward[] = {
	StoreForwardToggle_1, StoreForwardReport_2, StoreForwardForward_3, StoreForwardForward_4,
	Terminater,
	Terminater
};

static  MLDMENU   CONST    StoreFwdReport_1[]     = { LCD_AC102_SF_REPORT,      _T("    1") }; // 2 Store and Forward Report
static  MLDMENU   CONST    StoreFwdReport_2[]     = { LCD_AC102_SF_DETAILEDRPT, _T("    2") }; // 2 Store and Forward Report
static  MLDMENU   CONST    StoreFwdReport_3[]     = { LCD_AC102_SF_FAILEDRPT,   _T("    3") }; // 2 Store and Forward Report

MLDMENU CONST    *   TypeStoreReport[] = {
	StoreFwdReport_1, StoreFwdReport_2, StoreFwdReport_3, 
	Terminater,
	Terminater
};



static  MLDMENU   CONST    AC444_1[]  = { LCD_AC444_STATUS, _T("    1") };  // 1 Status
static  MLDMENU   CONST    AC444_2[]  = { LCD_AC444_UNJOIN, _T("    2") };  // 2 Detach
static  MLDMENU   CONST    AC444_3[]  = { LCD_AC444_JOIN,   _T("    3") };  // 3 Clear

MLDMENU CONST    *   TypeAC444[] = {
	AC444_1, AC444_2, AC444_3,
	Terminater,
	Terminater
};

/*------------------------------------------------------------------------*\

                        P R O G R A M   M O D E

	The following menu trees are displayed to the user when the terminal is
	in Program Mode.
\*------------------------------------------------------------------------*/

static  MLDMENU   CONST     PGNo1[]    = { LCD_PG1_ADR,  _T("    1") };   // P01 Machine Definition Code
static  MLDMENU   CONST     PGNo2[]    = { LCD_PG2_ADR,  _T("    2") };   // P02 Flexible Memory Assignment
static  MLDMENU   CONST     PGNo3[]    = { LCD_PG3_ADR,  _T("    3") };   // P03 Function Selection Code
static  MLDMENU   CONST     PGNo6[]    = { LCD_PG6_ADR,  _T("    6") };   // P06 Program Mode Security Code
static  MLDMENU   CONST     PGNo7[]    = { LCD_PG7_ADR,  _T("    7") };   // P07 Store/Terminal No
static  MLDMENU   CONST     PGNo8[]    = { LCD_PG8_ADR,  _T("    8") };   // P08 Supervisor No/Level
static  MLDMENU   CONST     PGNo9[]    = { LCD_PG9_ADR,  _T("    9") };   // P09 Action Code Security
static  MLDMENU   CONST    PGNo10[]    = { LCD_PG10_ADR, _T("   10") };   // P10 High Amount Lock Out
static  MLDMENU   CONST    PGNo15[]    = { LCD_PG15_ADR, _T("   15") };   // P15 Preset Cash Amount
static  MLDMENU   CONST    PGNo17[]    = { LCD_PG17_ADR, _T("   17") };   // P17 Hourly Activity Block
static  MLDMENU   CONST    PGNo18[]    = { LCD_PG18_ADR, _T("   18") };   // P18 Print Lines and EJ Control
static  MLDMENU   CONST    PGNo20[]    = { LCD_PG20_ADR, _T("   20") };   // P20 Transaction Mnemonics
static  MLDMENU   CONST    PGNo21[]    = { LCD_PG21_ADR, _T("   21") };   // P21 Leadthrough Mnemonics
static  MLDMENU   CONST    PGNo22[]    = { LCD_PG22_ADR, _T("   22") };   // P22 Report Mnemonics
static  MLDMENU   CONST    PGNo23[]    = { LCD_PG23_ADR, _T("   23") };   // P23 Special Mnemonics
static  MLDMENU   CONST    PGNo39[]    = { LCD_PG39_ADR, _T("   39") };   // P39 Character Strings
static  MLDMENU   CONST    PGNo46[]    = { LCD_PG46_ADR, _T("   46") };   // P46 Adjective Mnemonics
static  MLDMENU   CONST    PGNo47[]    = { LCD_PG47_ADR, _T("   47") };   // P47 Print Modifier Mnemonics
static  MLDMENU   CONST    PGNo48[]    = { LCD_PG48_ADR, _T("   48") };   // P48 Major Dept Mnemonics
static  MLDMENU   CONST    PGNo49[]    = { LCD_PG49_ADR, _T("   49") };   // P49 Auto Alter Remote Printer
static  MLDMENU   CONST    PGNo50[]    = { LCD_PG50_ADR, _T("   50") };   // P50 Shared/Alternative Printer

static  MLDMENU   CONST    PGNo51[]    = { LCD_PG51_ADR, _T("   51") };
static  MLDMENU   CONST    PGNo54[]    = { LCD_PG54_ADR, _T("   54") };  // 54 Set CPM/EPT Parameter
static  MLDMENU   CONST    PGNo57[]    = { LCD_PG57_ADR, _T("   57") };  // 57 Header Message
static  MLDMENU   CONST    PGNo60[]    = { LCD_PG60_ADR, _T("   60") };  // 60 Total Key Type
static  MLDMENU   CONST    PGNo61[]    = { LCD_PG61_ADR, _T("   61") };  // 61 Total Key Function
static  MLDMENU   CONST    PGNo62[]    = { LCD_PG62_ADR, _T("   62") };
static  MLDMENU   CONST    PGNo65[]    = { LCD_PG65_ADR, _T("   65") };
static  MLDMENU   CONST    PGNo95[]    = { LCD_PG95_ADR, _T("   95") };  // 95 Enter Software Register Information
static  MLDMENU   CONST    PGNo99[]    = { LCD_PG99_ADR, _T("   99") };  // 99 Set Cluster IP Addresses


MLDMENU CONST    *   PGEntryMenu[] = {
	PGNo1,  PGNo2,  PGNo3,  PGNo6, PGNo7,
	PGNo8,  PGNo9,  PGNo10, PGNo15,
	PGNo17, PGNo18, PGNo20, PGNo21,
	PGNo22, PGNo23, PGNo39, PGNo46,
	PGNo47, PGNo48, PGNo49, PGNo50,
	Terminater,
	PGNo51,PGNo54,PGNo57,PGNo60,PGNo61,
	PGNo62,PGNo65,PGNo95,PGNo99,
	Terminater,
	Terminater
};


MLDMENU CONST    *   PGEntryMenuSatt[] = {
	PGNo1,PGNo3,PGNo7,PGNo50,
	PGNo54,PGNo61,PGNo99,
	Terminater,
	Terminater
};

 
	/* SR 474 JHHJ, added this so that the user cannot edit FSC
	when we are on a touch screen terminal.  The user does not need
	to change the FSC's for a touch screen because they are set to specific location
	so that FrameworkWndButton uses them properly.
	This code basically removes the MLD for Program 3*/
 
MLDMENU CONST    *   PGEntryMenuTouch[] = {
	PGNo1,  PGNo2,  PGNo6,  PGNo7,
	PGNo8,  PGNo9,  PGNo10, PGNo15,
	PGNo17, PGNo18, PGNo20, PGNo21,
	PGNo22, PGNo23, PGNo39, PGNo46,
	PGNo47, PGNo48, PGNo49, PGNo50,
	Terminater,
	PGNo51, PGNo54, PGNo57, PGNo60,
	PGNo61, PGNo62, PGNo65, PGNo95,
	PGNo99,
	Terminater,
	Terminater
};
 
MLDMENU CONST    *   PGEntryMenuSattTouch[] = {
	PGNo1,PGNo7,PGNo50,
	PGNo54,PGNo61,PGNo99,
	Terminater,
	Terminater
};


/*------------------------------------------------------------------------*\

                        E N D   O F   H E A D E R

\*------------------------------------------------------------------------*/
