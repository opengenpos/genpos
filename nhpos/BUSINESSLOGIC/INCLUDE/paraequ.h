/*========================================================================*\
||                                                                        ||
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                Release 1.0               ||
||                                                                        ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:   Title              : NCR 2170 Header Definition                        :
:   Category           : SUPER & PROGRAM MODE,ECR 2170                     :
:   Program Name       : paraequ.h                                         :
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments              :
:                      : 95-02-14   03.00.00         R3.0                  :
:                      : 95-02-22   03.00.00 : H.Ishida : Add Flex Coupon
:                      :                                : Add Flex Conrtol String Size
:                      : 95-02-27   03.00.00 : H.Ishida : Add MAX_CPNTARGET_SIZE,
:                      :                                :     CLASS_PARACPN, CLASS_MAINTCPN
:                      : 95-03-01   03.00.00.: H.Ishida : Add CLASS_PARACPN_MAINT_READ,
:                      :                                :     CLASS_PARACPN_MAINT_WRITE,
:                      :                                :     CLASS_PARACPN_DATA,
:                      :                                :     CLASS_PARACPN_STS,MAX_CPNSTS_SIZE
:                      : 95-03-02   03.00.00.: T.Satoh  : Add A/C CODE DEFINITION A/C 161
:                      : 95-03-14   03.00.00.: T.Satoh  : Add CLASS_PARACPN_ONEADJ
:                      : 95-07-19   03.00.00.: M.Ozawa  : Add FVT Commment for R3.0
:                      : 96-03-01   03.01.00.: M.Ozawa  : Initial for R3.1
:                      : 02-12-06            : R.Chamber: additions for LePeeps SR07
:                      : 12-19-02:           :R.Chambers : SR 15 for Software Security
:					   : 01-22-03:			 : E.Smith	: SR 14
: --------------------------------------------------------------------------
:    Georgia Southern University Research Services Foundation
:    donated by NCR to the research foundation in 2002 and maintained here
:    since.
:       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
:       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
:       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
:       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
:       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
:       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
:       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
:       2016  -> GenPOS Rel 2.3.0 (Windows 7, completely Out of Scope, Visual Studio 2013.)
:
:    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
:    moved from Visual Studio 2005 to Visual Studio 2013 with Rel 2.3.0
\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*\

            D E F I N E    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

#if !defined(PARAEQU_H_INCLUDED)
#define PARAEQU_H_INCLUDED

/*------------------------------------------------------------------------*\
*   PARAMETER - MAJOR DATA CLASS DEFINE
       These defines are global in scope meaning that each must be
	   unique.  These defines must also agree with the defines used
	   in the PCIF 3rd Party Toolkit version of paraequ.h and in the PEP
	   version of paraequ.h.

       These constants are used in conjuction with the Minor Data Class
	   identifiers to access specific portions of the memory resident
	   database in the UNINIRAM data struct.

	They are normally used with the PARA structs such as PARADISPPARA
	which are defined in the file para.h in the BusinessLogic subsystem.
	The idea is to fill in the PARA struct with the correct Major and
	Minor class identifiers and then call one of the data base accessing
	functions.  For examples see the maint project in the BusinessLogic
	subsystem.

	** NOTE: When adding New Major Classes
	When adding new major classes for a new data item in UNINIRAM Para, you
	may also need to add the logic to handle the transfer of the data item
	from a provisioning utility such as MWS or PEP as well as to add the logic
	to handle broadcasting the new data item from the Master terminal to the
	various satellite terminals.

	The place to look for adding handling a provisioning utility is in the ISP
	subsystem.  Specially take a look at functions IspRecvCheckPara () and
	IspAfterProcess () in subystems Isp, part of BusinessLogic, file ispopr.c.

	To handle the broadcasting of new data items to satellite terminals take
	a look at the array auchClassNoAll which contains a list of all of the
	different Major classes to broadcast and is used by the functions OpReqAllPara ()
	and OpResAll () in subsystem Opepara, part of BusinessLogic, file nwop.c.

	You will also want to add the code to copy to/from UNINIRAM Para for your
	new major class by modifying the functions ParaAllRead () and ParaAllWrite ()
	in subsystem Para, part of BusinessLogic, file paraall.c.
\*------------------------------------------------------------------------*/

#define CLASS_PARAGUESTNO       101     /* GUEST CHECK NO. ASSIGNMENT */
#define CLASS_PARAPLUNOMENU     102     /* SET PLU NO.ON MENU PAGE (S) */
#define CLASS_PARAROUNDTBL      103     /* SET ROUNDING TABLE (S) */
#define CLASS_PARADISCTBL       104     /* DISCOUNT/BONUS % RATE (S) */
#define CLASS_PARAPROMOTION     105     /* SET SALES PROMOTION MESSAGE (S) */
#define CLASS_PARACURRENCYTBL   106     /* SET CURRENCY CONVERSION RATE (S) */
#define CLASS_PARAMENUPLUTBL    107     /* SET SET MENU OF PLU (S) */
#define CLASS_PARATAXRATETBL    108     /* SET TAX/SERVICE/RECIPROCAL RATES (S) */
#define CLASS_PARADEPT          109     /* MAINTENANCE DEPARTMENT */
#define CLASS_PARAPLU           110     /* ADD/DELETE/MAINTENANCE PLU */
#define CLASS_PARACASHIERNO     111     /* CASHIER NO. ASSIGNMENT */
#define CLASS_PARAWAITERNO      112     /* WAITER NO. ASSIGNMENT */
#define CLASS_PARAMDC           113     /* SET MDC */
#define CLASS_PARAFSC           114     /* SET FSC */
#define CLASS_PARASECURITYNO    115     /* SECURITY NUMBER FOR PROGRAM MODE */
#define CLASS_PARASUPLEVEL      116     /* SUPERVISOR NUMBER/LEVEL ASSINMENT */
#define CLASS_PARAACTCODESEC    117     /* ACTION CODE SECURITY */
#define CLASS_PARATRANSHALO     118     /* TRANSACTION HALO value. Use RflGetHaloData() */
#define CLASS_PARAHOURLYTIME    119     /* HOURLY ACTIVITY TIME */
#define CLASS_PARASLIPFEEDCTL   120     /* SLIP PRINTER FEED CONTROL - P18 Printer Feed and EJ Control parameters */
#define CLASS_PARATRANSMNEMO    121     /* TRANSACTION MNEMONICS - P20 Transaction Mnemonics. Use RflGetTranMnem() */
#define CLASS_PARALEADTHRU      122     /* LEAD THROUGH TABLE - P21 Lead-thru Mnemonics. Use RflGetLead() */
#define CLASS_PARAREPORTNAME    123     /* REPORT NAME - P22 Report Mnemonics. Use RflGetReportMnem() */
#define CLASS_PARASPECIALMNEMO  124     /* SPECIAL MNEMONICS - P23 Special Mnemonics. Use RflGetSpecMnem() */
#define CLASS_PARATAXTBL1       125     /* TAX TABLE PROGRAMMING (S) */
#define CLASS_PARATAXTBL2       126     /* TAX TABLE PROGRAMMING (S) */
#define CLASS_PARATAXTBL3       127     /* TAX TABLE PROGRAMMING (S) */
#define CLASS_PARAADJMNEMO      128     /* ADJECTIVE MNEMONICS - P46 Adjective Mnemonics. Use RflGetAdj() */
#define CLASS_PARAPRTMODI       129     /* PRINT MODIFIER MNEMONICS - P47 Print Modifier Mnemonics */
#define CLASS_PARAMAJORDEPT     130     /* REPORT CODE MNEMONICS - P48 Major Department Mnemonics, use RflGetMajorDeptMnem() */
#define CLASS_PARACHAR24        131     /* 24 CHARACTERS MNEMONICS - P57 Header/Special Mnemonics. Use RflGet24Mnem()  */
#define CLASS_PARACTLTBLMENU    132     /* SET CONTROL TABLE OF MENU PAGE (S) */
#define CLASS_PARAAUTOALTKITCH  133     /* AUTO ALTERNATIVE KITCHEN PTR. */
#define CLASS_PARAMANUALTKITCH  134     /* MANUAL ALTERNATIVE KITCHEN PTR. (S) */
#define CLASS_PARAFLEXMEM       135     /* FLEXIBLE MEMORY ASSIGNMENT */
#define CLASS_PARASTOREGNO      136     /* STORE/REGISTER NO. */
#define CLASS_PARATTLKEYTYP     137     /* TOTAL KEY TYPE ASSIGNMENT */
#define CLASS_PARATTLKEYCTL     138     /* TOTAL TYPE KEY CONTROL */
#define CLASS_PARACASHABASSIGN  139     /* CASHIER A/B KEYS ASSIGNMENT (S) */
#define CLASS_PARASPCCO         140     /* SPECIAL COUNTER */
#define CLASS_PARAPCIF          141     /* PC INTERFACE */
#define CLASS_PARATONECTL       142     /* ADJUST TONE VALUE */
#define CLASS_PARATARE          143     /* TARE CODE */
#define CLASS_PARASOFTCHK       144     /* Trailer Print for Soft Check AC87 */
#define CLASS_PARASHRPRT        145     /* ASSIGNMENT TERMINAL NO. INSTALLING SHARED PRINTER  Prog 50 */
#define CLASS_PARAEMPLOYEENO    146     /* ASSIGNMENT EMPLOYEE NO */
#define CLASS_PARAPRESETAMOUNT  147     /* PRESET  AMOUNT  Prog 15 */
#define CLASS_PARAHOTELID       148     /* HOTEL ID AND SLD */
#define CLASS_PARAETKFILE       149     /* ETK File MAINTENANCE */
#define CLASS_PARACPMEPTTALLY   150     /* READ AND RESET TALLY */
#define CLASS_PARAOEPTBL        151     /* OEP TABLE ACCESS */
#define CLASS_PARACPN           152     /* MAINTENANCE Combination Coupon */
#define CLASS_PARAFXDRIVE       153     /* SET FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL PARAMETER */
#define CLASS_PARAPPI           154     /* SET PPI TABLE (S),   R3.1 */
#define CLASS_PARASERVICETIME   155     /* SET SERVICE TIME PARAMETER,  R3.1 */
#define CLASS_PARATAXTBL4       156     /* TAX TABLE PROGRAMMING (S) R3.1 */
#define CLASS_PARAPIGRULE       157     /* PIG RULE TABLE R3.1 */
#define CLASS_PARALABORCOST     158     /* LABOR COST VALUE R3.1 */
#define CLASS_PARADISPPARA      159     /* BEVERAGE DISPENSER PARAMETER R3.1 */


	/* --- Number "159" is last one, start next number of CLASS_MAINTxxx --- */

	/*------------------------------------------------------------------------*\
	*   MAINTENANCE - MAJOR DATA CLASS DEFINE  160 ~
	\*------------------------------------------------------------------------*/

#define CLASS_MAINTHEADER       160     /* for print header */
#define CLASS_MAINTTRAILER      161     /* for print trailer */
#define CLASS_MAINTTRANS        162     /* for transaction mnemonics */
#define CLASS_MAINTERRORCODE    163     /* for error */
#define CLASS_MAINTOPESTATUS    164     /* Operator/Guest Check Status Report */
#define CLASS_MAINTMODEIN       165     /* program/super mode in */
#define CLASS_MAINTTOD          166     /* read/write/edit TOD */
#define CLASS_MAINTENT          167     /* check supervisor security level */
#define CLASS_MAINTSETPAGE      168     /* set menu page no. in maintenance-work */
#define CLASS_MAINTFEED         169     /* receipt/journal feed */
#define CLASS_MAINTDSPCTL       170     /* descriptor/page/leadthrough/program/action control */
#define CLASS_MAINTDSPFLINE     171     /* display any data & any mnemonics to first display line */
#define CLASS_MAINTTAXTBL1      172     /* edit Tax Table */
#define CLASS_MAINTTAXTBL2      173     /* edit Tax Table */
#define CLASS_MAINTTAXTBL3      174     /* edit Tax Table */
#define CLASS_MAINTMENUPLUTBL   175     /* edit Set PLU */
#define CLASS_MAINTDEPT         176     /* edit DEPT */
#define CLASS_MAINTPLU          177     /* edit PLU */
#define CLASS_MAINTREDISP       178     /* Control Descriptor and Redisplay */
#define CLASS_MAINTBCAS         179     /* Broadcast/Request to Download parameter */
#define CLASS_MAINTETK          180     /* Employee maintenance */
#define CLASS_MAINTTAXTBL4      181     /* edit Tax Table */
#define CLASS_MAINTETKFL        182     /* ETK File Maintenance */
#define CLASS_MAINTCPM          183     /* START.STOP CPM A/C78 */
#define CLASS_MAINTCPN          184     /* edit Combination Coupon */
#define CLASS_MAINTCSTR         185     /* edit control strings */

#define CLASS_PARATEND          186     /* Tender Key Parameter,    V3.3 */
#define CLASS_PARAMLDMNEMO      187     /* MLD Mnemonics Parameter, V3.3 */
#define CLASS_PARAKDSIP         188     /* KDS IP, Port #, 2172 */
#define CLASS_PARARESTRICTION   189     /* SET SALES RESTRICTION TABLE (S), 2172 */
#define CLASS_MAINTPRINTLINE    190     /* Print the line specified, PrtThrmSupOutputLine() */

#define CLASS_PARABOUNDAGE      230     /* SET BOUNDARY AGE (S), 2172 */
#define CLASS_PARADEPTNOMENU    231     /* Set Dept NO.ON MENU PAGE, 2172 (S) */
#define CLASS_MAINTLOAN         232     /* Loan,    Saratoga */
#define CLASS_MAINTPICKUP       233     /* Pick up */
#define CLASS_MAINTSPHEADER     234     /* for slip print header */
#define CLASS_MAINTSPTRAILER    235     /* for slip print trailer */
#define CLASS_PARAMISCPARA      236     /* Misc Limits Table, AC128 Transaction Limits for Food Stamp, coupon, EPT  */
#define CLASS_MAINTHOSTSIP      237     /* Hosts IP, Port #, 2172 */
#define CLASS_PARATENDERKEYINFO 238		// Tender Key Info
#define CLASS_PARASTOREFORWARD  239     /* Store and Forward Info, PARASTOREFORWARD */
#define CLASS_PARATERMINALINFO  240     /* terminal information */
#define CLASS_TENDERKEY_INFO    241     // new interface for Tender key information

#define CLASS_FILEVERSION_INFO  242
#define CLASS_PARAREASONCODE    243     /* Mnemonics file info, PARAREASONCODEINFO */

#define CLASS_PARAUNLOCKNO      251     /* UNLOCK NUMBER FOR PROGRAM MODE */

#define CLASS_PARAAUTOCPN		252		/* AUTO COMBINATIONAL COUPONS	  */
#define CLASS_PARACOLORPALETTE	253		/* COLOR PALETTE	*/
#define CLASS_PARATTLKEYORDERDEC 254	/* Order Dec Total Key JHHJ*/

	/*------------------------------------------------------------------------*\
	*   TOTAL - MAJOR DATA CLASS DEFINE  190 ~ 199, 220 ~
	\*------------------------------------------------------------------------*/

#define     CLASS_TTLREGFIN     190     /* DAILY/PTD REGISTER FINANCIAL FILE (TOTAL,REPORT) */
#define     CLASS_TTLHOURLY     191     /* DAILY/PTD HOURLY ACTIVITY FILE (TOTAL,REPORT) */
#define     CLASS_TTLDEPT       192     /* DAILY/PTD DEPARTMENT FILE (TOTAL,REPORT) */
#define     CLASS_TTLPLU        193     /* DAILY/PTD PLU FILE (TOTAL,REPORT) */
#define     CLASS_TTLCASHIER    194     /* CASHIER FILE (TOTAL,REPORT) */
#define     CLASS_TTLWAITER     195     /* DAILY/PTD WAITER FILE (TOTAL,REPORT) */
#define     CLASS_TTLGUEST      196     /* GUEST CHECK FILE HEADER (TOTAL,REPORT) */
#define     CLASS_TTLEODRESET   197     /* EOD RESET */
#define     CLASS_TTLPTDRESET   198     /* PTD RESET */
#define     CLASS_TTLCPN        199     /* DAILY/PTD COUPON FILE (TOTAL,REPORT), R3.0 */
	/*  Additions to Total Major data class (Release 3.1) defined below   */

	/*------------------------------------------------------------------------*\
	*   REPORT - MAJOR DATA CLASS DEFINE  200 ~
	\*------------------------------------------------------------------------*/

#define     CLASS_RPTREGFIN     200     /* DAILY/PTD REGISTER FINANCIAL FILE (REPORT,PRINT) */
#define     CLASS_RPTHOURLY     201     /* DAILY/PTD HOURLY ACTIVITY FILE (REPORT,PRINT) */
#define     CLASS_RPTDEPT       202     /* DAILY/PTD DEPARTMENT FILE (REPORT,PRINT) */
#define     CLASS_RPTPLU        203     /* DAILY/PTD PLU FILE (REPORT,PRINT) */
#define     CLASS_RPTCASHIER    204     /* CASHIER FILE (REPORT,PRINT) */
#define     CLASS_RPTWAITER     205     /* DAILY/PTD WAITER FILE (REPORT,PRINT) */
#define     CLASS_RPTGUEST      206     /* GUEST CHECK FILE HEADER (REPORT,PRINT) */
#define     CLASS_RPTEJ         207     /* EJ FILE (REPORT,PRINT) */
#define     CLASS_RPTOPENGCF    208     /* EMERGENT GC FILE CLOSE */
#define     CLASS_RPTOPENWAI    209     /* EMERGENT WAITER FILE CLOSE */
#define     CLASS_RPTOPENCAS    210     /* EMERGENT CASHIER FILE CLOSE */
#define     CLASS_RPTEMPLOYEE   211     /* EMPLOYEE FILE (REPORT,PRINT) */
#define     CLASS_RPTCPMTALLY   212     /* CPM TALLY (REPORT,PRINT) */
#define     CLASS_RPTCPN        213     /* DAILY/PTD COUPON FILE (REPORT,PRINT) */
#define     CLASS_RPTEMPLOYEENO 214     /* EMPLOYEE FILE (REPORT,PRINT) */
#define     CLASS_RPTSERVICE    215     /* DAILY/PTD SERVICE TIME FILE (REPORT,PRINT) */
#define     CLASS_RPTPROGRAM    216     /* PROGRAMABLE REPORT (REPORT,PRINT) */
#define     CLASS_RPTJCSUMMARY  217     /* LEPEEPS JOB COST SUMMARY FOR EMPLOYEE SR07 */
#define     CLASS_RPTJCSUMMARYTITLES  218     /* LEPEEPS JOB COST SUMMARY TITLES SR07 */
#define     CLASS_RPTJCSUMMARYTOTALS  219     /* LEPEEPS JOB COST SUMMARY TOTALS SR07 */
#define		CLASS_RPTCONFIGURATION	  222	/*Configuration information*/
#define		CLASS_RPTPLUGROUPSUMMARY  223	/*PLU GROUP SUMMARY (REPORT,PRINT) */
#define		CLASS_RPTSTOREFORWARD	224		/* Store and Forward Report */

	/* ----- New Functions added to Total Major data class (Release 3.1) ----- */
#define CLASS_TTLSERVICE        220     /* DAILY/PTD SERVICE TIME (TOTAL,REPORT) */
#define CLASS_TTLINDFIN         221     /* INDIVIDUAL FINANCIAL (TOTAL,REPORT) */


/*------------------------------------------------------------------------*\
*   PARAMETER - MINOR DATA CLASS DEFINE
\*------------------------------------------------------------------------*/

/* for FSC Setting */

#define     CLASS_PARAFSC_MAJOR     1    /* Defined Major FSC Code */
#define     CLASS_PARAFSC_MINOR     2    /* Defined Minor FSC Code */
#define     CLASS_PARAFSC_INIT      3    /* initial read mode */
#define     CLASS_PARAFSC_PAGE      4    /* page parameter set read mode */
#define     CLASS_PARAFSC_ADDRESS   5    /* address parameter set read mode */
#define     CLASS_PARAFSC_MINOR2    6    /* Defined Minor FSC Code 2172 */


/* for Supervisor Level Setting */

#define     CLASS_PARASUPLEVEL_ADR      1    /* Read Data by Address */
#define     CLASS_PARASUPLEVEL_SUPNO    2    /* Search Data by Super No. */
#define     CLASS_PARASUPLEVEL_SUP      3    /* Clear Secret Case */
#define     CLASS_PARASUPLEVEL_SEC      4    /* Write Secret Case */


/* for DEPT/PLU Setting */

#define     CLASS_PARAPLU_NONADJ        1    /* Non Adjective */
#define     CLASS_PARAPLU_ONEADJ        2    /* One Adjective */
#define     CLASS_PARAPLU_OPENPLU       3
#define     CLASS_PARAPLU_MNEMO_READ    4
#define     CLASS_PARAPLU_PRICE_READ    5
#define     CLASS_PARAPLU_MAINT_READ    6
#define     CLASS_PARAPLU_ADD_READ      7
#define     CLASS_PARAPLU_MNEMO_WRITE   8
#define     CLASS_PARAPLU_PRICE_WRITE   9
#define     CLASS_PARAPLU_MAINT_WRITE  10
#define     CLASS_PARAPLU_ADD_WRITE    11
#define     CLASS_PARAPLU_DATA         12
#define     CLASS_PARAPLU_STS          13
#define     CLASS_PARAPLU_INT          14
#define     CLASS_PARAPLU_COPY         15
#define     CLASS_PARAPLU_DEL          16
#define     CLASS_PARAPLU_EVERSION     17 /* 2172 */
#define     CLASS_PARAPLU_SET_EV       18 /* 2172 */
#define     CLASS_PARAPLU_RESET_EV     19 /* 2172 */

/* for Combination Coupon Setting */

#define     CLASS_PARACPN_ONEADJ        2
#define     CLASS_PARACPN_MAINT_READ    6
#define     CLASS_PARACPN_MAINT_WRITE  10
#define     CLASS_PARACPN_DATA         12
#define     CLASS_PARACPN_STS          13
#define     CLASS_PARACPN_GROUPCODE    17
#define     CLASS_PARACPN_AUTO_PRIORITY 21
#define		CLASS_PARACPN_BONUS_INDEX	22

/* for control strings maintenance */

#define     CLASS_PARACSTR_MAINT_READ           1
#define     CLASS_PARACSTR_MAINT_REPORT         2
#define     CLASS_PARACSTR_MAINT_FSC_WRITE      3
#define     CLASS_PARACSTR_MAINT_EXT_WRITE      4
#define     CLASS_PARACSTR_MAINT_DELETE         5

/* for Flex Memory */

#define     CLASS_PARAFLEXMEM_RECORD    14   /* Indicate Number of Record */
#define     CLASS_PARAFLEXMEM_PTD       15   /* Indicate PTD Flag Data */

/* for Waiter No Setting */

#define     CLASS_PARAWAITERNO_MNEMO        16   /* Indicate Waiter Name Data */
#define     CLASS_PARAWAITERNO_STSGC9       17   /* Indicate Start GC No. Data for A/C 9 */
#define     CLASS_PARAWAITERNO_STSGC50      18   /* Indicate Start GC No. Data for A/C 50 */
#define     CLASS_PARAWAITERNO_ENDGC        19   /* Indicate End GC No. Data */
#define     CLASS_PARAWAITERNO_STSGC9_INT   20   /* Indicate Start GC No. Data for A/C 9 */
#define     CLASS_PARAWAITERNO_STSGC50_INT  21   /* Indicate Start GC No. Data for A/C 50 */

/* for Rounding Table  */

#define     CLASS_PARAROUNDTBL_DELI     22   /* Delimit & Modules */
#define     CLASS_PARAROUNDTBL_POSI     23   /* Round Position */

/* for Tax Rate Table  */

#define     CLASS_PARATAXRATETBL_RATE   24   /* Tax Rate */
#define     CLASS_PARATAXRATETBL_LIMIT  25   /* Tax Exempt */

/* for Set PLU No. On Menu Page  */
/* 2172
#define CLASS_PARAPLUNOMENU_KEYEDPLU    26
#define CLASS_PARAPLUNOMENU_PLUNO       27
*/
/* for Tax Table */

#define CLASS_PARATAXTBL_1              28         /* Tax Table #1 */
#define CLASS_PARATAXTBL_2              29         /* Tax Table #2 */
#define CLASS_PARATAXTBL_3              30         /* Tax Table #3 */
#define CLASS_PARATAXTBL_RATE           31         /* Input Tax Rate */
#define CLASS_PARATAXTBL_TAXABLE        32         /* Input Taxable Amount */
#define CLASS_PARATAXTBL_TAX            33         /* Input Tax Amount */
#define CLASS_PARATAXTBL_EC_TAXABLE     34         /* Correct Taxable Amount */
#define CLASS_PARATAXTBL_EC_TAX         35         /* Correct Tax Amount */

/* for Employee No Setting */

#define  CLASS_PARAEMPLOYEENOCUR        36         /* Current Employee No */
#define  CLASS_PARAEMPLOYEENOSAV        37         /* Saved Employee No */
#define  CLASS_PARAEMPLOYEENO_2ND       38         /* employee NO */
#define  CLASS_PARAEMPLOYEENO_INT       39         /* employee NO display */

/* for Currency Rate Table  */

#define  CLASS_PARACURRENCYTBL_DECPOS0   1   /* Decimal Point Nothing */
#define  CLASS_PARACURRENCYTBL_DECPOS2   2   /* Decimal Point 2 */
#define  CLASS_PARACURRENCYTBL_DECPOS3   3   /* Decimal Point 3 */

/* for Hourly Activity Time Block Table  */

#define  CLASS_PARAHOURLYTIME_BLOCK      1   /* Hourly Time Block No. */
#define  CLASS_PARAHOURLYTIME_TIME       2   /* Hourly Activity Time */

/* for Total Key Control */

#define  CLASS_PARATTLKEYCTL_ALL         1   /* Address and Field Input */
#define  CLASS_PARATTLKEYCTL_ADR         2   /* Address Input */
#define  CLASS_PARATTLKEYCTL_FLD         3   /* Field Input */

/* for Set PLU */

#define  CLASS_PARAMENUPLU_READ          1
#define  CLASS_PARAMENUPLU_PLUWRITE      2
#define  CLASS_PARAMENUPLU_ADJWRITE      3
#define  CLASS_PARAMENUPLU_PAGE          4

/* for Tare */

#define  CLASS_PARATARE_001              1   /* minimum unit is 0.01 */
#define  CLASS_PARATARE_0001             2   /* minimum unit is 0.001 or 0.005 */

/* for Pig Rule Table Setting */

#define  CLASS_PARAPIGRULE_COLIMIT       1   /* Counter limit for Non GST Tax */
#define  CLASS_PARAPIGRULE_PRICE         2   /* Pig Rule Unit Price */

/* for ETK File Maintenance */

#define  CLASS_PARAETKFILE_NUMTYPE       1
#define  CLASS_PARAETKFILE_TIMEIN        2
#define  CLASS_PARAETKFILE_TIMEOUT       3
#define  CLASS_PARAETKFILE_JOBCODE       4

/* for SET FLEXIBLE DRIVE THROUGH/MULTI STORE REACLL PARAMERTER */

#define     CLASS_PARAFXDRIVE_ALL        1    /* To Operate All  Data */
#define     CLASS_PARAFXDRIVE_ADR        2    /* To Operate One  Data */
#define     CLASS_PARAFXDRIVE_NEXT       3    /* To Operate Next Data */


/* for ppi maintenance */

#define     CLASS_PARAPPI_MAINT_READ           1
#define     CLASS_PARAPPI_MAINT_REPORT         2
#define     CLASS_PARAPPI_MAINT_QTY_WRITE      3
#define     CLASS_PARAPPI_MAINT_PRICE_WRITE    4
#define     CLASS_PARAPPI_MAINT_DELETE         5

/* for beverage dispenser - see UifAC137Function() */
#define     CLASS_PARADISPPARA_PLUNO           1
#define     CLASS_PARADISPPARA_ADJECTIVE       2

/* A/C 4 for Set PLU No. On Menu Page - see UifAC4Function(), NCR 2172, unused with Touchscreen, non-keyboard */
#define CLASS_PARAPLUNOMENU_KEYEDPLU    1
#define CLASS_PARAPLUNOMENU_PLUNO       2
#define CLASS_PARAPLUNOMENU_ADJECTIVE   3
#define CLASS_PARAPLUNOMENU_ADDRESS     4
#define CLASS_PARAPLUNOMENU_EVERSION    5
#define CLASS_PARAPLUNOMENU_VELOCITY    6

/* for Set Dept No. On Menu Page - see UifAC115Function(), NCR 2172  */
#define CLASS_PARADEPTNOMENU_KEYEDDEPT  1
#define CLASS_PARADEPTNOMENU_DEPTNO     2

/* for PLU Sales Restriction Table 2172 */

#define CLASS_PARAREST_ADDRESS          1       /* #/Type Key Entry */
#define CLASS_PARAREST_FIELD            2       /* For Key Entry */
#define CLASS_PARAREST_DATA             3       /* A/C Key Entry */
#define CLASS_PARAREST_READ             4       /* Read Table from Para. */
#define CLASS_PARAREST_WRITE            5       /* Write Table to Para. */

/* for SR14 ESMITH PARAKDS */
#define CLASS_PARAKDS_IPADDRESS         1       /* # IP Address Class */
#define CLASS_PARAKDS_KDSPORT           2       /* # KDS Port Class */
#define CLASS_PARAKDS_INTPORT           3       /* # NHPOS Interface Port Class */

/*------------------------------------------------------------------------*\
*   RAM SIZE DEFINE
\*------------------------------------------------------------------------*/

/*The number below is 9 * 165.  9 for the number of menu pages that the user
can assign PLUs to, and 165 is the max number of keys that can be assigned per page
(Micromotion holds 165). JHHJ*/
#define MAX_PLUKEY_SIZE      1485    /* Keyed PLU Key which is FSC_MAX_PAGE * FSC_MAX_ADR or 9 * 165 */
//#define MAX_RNDTBL_SIZE        15    /* Rounding Table, 2172 */
#define MAX_RNDTBL_SIZE        17    /* Rounding Table, 2172 */
#define MAX_RND_DATA            3    /* Set Data for Rounding */
//#define MAX_DISCRATE_SIZE      20    /* Discount/Bonus % Rates, V3.3 */
#define MAX_DISCRATE_SIZE      120    /* Discount/Bonus % Rates, V3.3 */
#define MAX_TABLE_NO           99    /* OEP TABLE, V3.3 */
#define MAX_TABLE_ADDRESS      10    /* OEP TABLE */
#define MAX_ONE_OEPTABLE_SIZE  10    /* OEP TABLE */
#define MAX_SOFTCHK_NO         20     /* 42 Characters */

#if defined(R20_DATABASE_CHANGES)
// See explanation below about why this proposed change was not made.

#define MAX_PROMO_NO           10    /* Line for Promotion Message */
#else
#define MAX_PROMO_NO            3    /* Line for Promotion Message */
#endif

#define MAX_FC_SIZE             8    /* Currency Conversion Rate, 2172 */
#define MAX_SETMENU_NO         20    /* Set Menu */
#define MAX_SETPLU_DATA         6    /* Set PLU  */
#define MAX_TAXRATE_SIZE        5    /* Tax Rate */


#define MAX_MDC_SIZE          600    /* MDC Data for 1byte Data, 2172 */
#define MAX_FSC_TBL            11    /* Function Key Table including Touchscreen for Rel 2.0 and not same as FSC_MAX_ADR *///RPH 12-2-3
#define MAX_FSC_NO            300    /* Function Key 165->300 including Touchscreen for Rel 2.0 and not same as FSC_MAX_ADR - JHHJ Database Changes 3-17-04*/
#define MAX_FSC_DATA            2    /* Major FSC Data/Minor FSC Data */
#define MAX_SUPLEVEL_SIZE      22    /* Supervisor Level */
#define MAX_ACSEC_SIZE        127    /* Action Code Security for Supervisor. See also ACSC_ADR_MAX which is max number in use */
//#define MAX_HALOKEY_NO         36    /* Key for setting HALO 36->55*/
#define MAX_HALOKEY_NO         50    /* Key for setting HALO */
#define MAX_HOUR_NO            49    /* Data for setting Hourly Activity Time, V3.3 */
#define MAX_HM_DATA             2    /* Hour/Minute */
#define MAX_SLIP_SIZE           6    /* Slip Printer Feed Control */
#define MAX_TRANSM_NO			400		/*Transaction Menomnics update for Database Change JHHJ 3-25-04*/

#define MAX_LEAD_NO           220    /* Lead Through, 2172 increased to 220 messages max */
#define MAX_REPO_NO            32    /* Report Name */
#define MAX_SPEM_NO            75    /* Special Mnemonics, 2172 */
#define MAX_TTBLE_SIZE        208    /* Ram Fix Size for setting Tax Rate */
#define MAX_TTBLDATA_SIZE     200    /* Tax Data Table Size for Tax Table */
#define MAX_TTBLHEAD_SIZE       4    /* Header Size for Tax Table */
#define MAX_TTBLRATE_SIZE       5    /* WDC & RATE Size for Tax Table */
#define MAX_ADJM_NO            20    /* Adjective Mneminics */
#define MAX_PRTMODI_NO          8    /* Print Modifier */

#define MAX_MDEPT_NO           30    /* Major Department Mnemonics, 2172 */
#define MAX_CH24_NO            40    /* 24 Characters, CLASS_PARACHAR24 */
#define MAX_PAGE_SIZE          23    /* Control Menu Page, 2172 */ //RPH 12-2-3 TOUCHSCREEN
#define MAX_DEST_SIZE           8    /* Destination Kitchen Printer */
#define MAX_FLXMEM_SIZE        14    /* Flexible Memory Assignment, with additional files such as MLD File */
#define MAX_STOREG_SIZE         2    /* Store/Register No. */

#define MAX_KEYTYPE_SIZE        17    /* Total Keys are in need of type assign, STD_TOTAL_MAX - 3 */
#define MAX_TTLKEY_NO           STD_TOTAL_MAX    /* The number of Total Key, 3 + MAX_KEYTYPE_SIZE */
#define MAX_TTLKEY_DATA         6    /* The number of bit */
#define MAX_AB_SIZE             2    /* Cashier A/Cashier B */
#define MAX_CO_SIZE             4    /* Special counter */
#define MAX_DEPTSTS_SIZE        5    /* DEPT Status Field Size R3.1 */
#define MAX_CPNTARGET_SIZE      7    /* Combination Coupon Target Menu Size */
#define MAX_CPNSTS_SIZE         2    /* Combination Coupon Status Field Size */
#define MAX_PLUSTS_SIZE        10    /* PLU Status Field Size, OP_PLU_CONT_OTHER */
#define MAX_ADJECTIVE_NO        5    /* The number of Adjective Price */
#define MAX_DEPT_NO           9999    /* Max Size of PLU Number */
#define MAX_PLU_NO           9999    /* Max Size of PLU Number */
#define MAX_PLUPRICE_SIZE       3    /* Max Size of 1 PLU Price Save Area */
#define MAX_PCIF_SIZE          10    /* Max Size of Password for PC I/F */
#define MAX_TONE_SIZE           1    /* Max Size of Tone control */
//#define MAX_TARE_SIZE          10    /* Max Size of Tare table   */
#define MAX_TARE_SIZE          30    /* Max Size of Tare table   */
#define MAX_SHAREDPRT_SIZE     48    /* Max Size of Shared Printer P50, 2172 - ParaSharedPrtRead() shared and alternate printers for 16 terminals, shared printer for 8 kitchen printers, COM port for 8 kitchen printers */
#define MAX_EMPLOYEENO       9999    /* Max Employee No */
#define MAX_JOB_CODE           99    /* Max Job Code */
#define MAX_JOB_CODE_ADDR      3L    /* Max Job Code Address */
#define MAX_DISPPARA_OFFSET 31712    /* Max Offset Code of Beverage Dispenser */
#define MAX_DISPPARA_TENDER    STD_TENDER_MAX    /* Max Tender Key Data of Beverage Dispenser */
#define MAX_DISPPARA_TOTAL     19    /* Max Total Key Data of Beverage Dispenser */
#define MAX_TEND_NO            STD_TENDER_MAX    /* Key for Tender Para, V3.3 */

#define MAX_PRESETAMOUNT_SIZE   4    /* Max Size of Preset Amount Cash Tender array memory resident area. May be divided into 8 USHORTs. See FSC_PREAMT_MAX. See MDC 82 Bit A, MDC_DRAWER2_ADR */
#define MAX_PIGRULE_SIZE       10    /* Max Size of Pig Rule Table */
#define MAX_HOTELID_SIZE       10    /* HOTEL ID OR SLD */
#define MAX_HOTELID_NO         18    /* HOTEL ID AND SLD */

#define MAX_FLEXDRIVE_SIZE     36    /* Max Size of SET FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL PARAMETER TABLE */
#define MAX_PARACSTR_SIZE     400    /* Max One Control String Size */

#define MAX_PARAPPI_SIZE       40    /* Max One PPI Price Level */ /* 20->40 for PPI Enhancement - CSMALL */
#define MAX_SERVICETIME_SIZE    2    /* Max Size of Service Time Parameter */
#define MAX_PROGRPT_FORMAT      8    /* Max no. of prog. report format */
#define MAX_PROGRPT_EODPTD     11    /* Max no. of record at EOD/PTD */
#define MAX_PROGRPT_PTD         8    /* Max no. of record at PTD */
#define MAX_LABORCOST_SIZE    100    /* AC 154, Max no. of labor cost value, LABOR_COST100_ADR used as default job code in RptCalWageTotal() */
#define MAX_DISPPARA_SIZE     259    /* Max no. of beverage dispenser parameter */

#define MAX_MLDMNEMO_SIZE     240    /* Max no. of multi-line display parameter, saratoga */
#define MAX_RSNMNEMO_SIZE     400    /* Max no. of Reason Code mnemonics */

#define MAX_KDSIP_SIZE          8   /* KDS IP Address, 2172 */
#define PARA_KDSIP_LEN          4   /* KDS IP Address, 2172 */
#define MAX_KDSIP_INPUT_LEN    12   /* KDS IP Address for input, 2172 */

#define MAX_REST_SIZE           6    /* PLU Sales Restriction Table, 2172 */
#define MAX_AGE_SIZE            3    /* Boundary age, 2172 */
#define MAX_DEPTKEY_SIZE      450    /* Keyed Dept Key, 2172, this value should be FSC_DEPT_MAX times number of keyboard menu pages, currently 9 */
#define MAX_MISC_SIZE           9    /* Misc. Parameter */
#define MAX_AUTOCOUPON_SIZE		8	 /* Auto Coupon Max Size */

#define MAX_TERMINALINFO_SIZE  16    /* Max number of terminals in cluster for terminal info, PIF_NET_MAX_IP, STD_NUM_OF_TERMINALS */

#define MAX_COLORPALETTE_SIZE	256	/* Color Palette ('0' thru '255')*/

#define MAX_REASON_CODE_ENTRIES   2  /* number of entries per reason code item entry */
#define MAX_REASON_CODE_ITEMS    12  /* number of reason code items: itemsales, itemdisc, etc. */

#define MAX_SUGGESTED_TIP         4  /* maximum number of suggested tip entries, ParaSuggestedTip[]  */

/*------------------------------------------------------------------------*\
*   MAX DATA DEFINE
\*------------------------------------------------------------------------*/

#define MAX_SPCO_CONSEC         9999        /* Max Consecutive Counter */
#define MAX_SPCO_PRG            9999        /* Max Program Counter */
#define MAX_SPCO_EODRST         9999        /* Max EOD Reset Counter */
#define MAX_SPCO_PTDRST         9999        /* Max PTD Reset Counter */

#define MAX_NO_CASH             300         /* Max Number of Cashier, V3.3 */
#define MAX_NO_WAIT             100         /* Max Number of Waiter */
#define MAX_PAGE_NO             11          /* Max Number of Menu Shift Page *///RPH 12-2-3
#define MAX_SET_MENU            20          /* Max Number of Set Menu */
#define MAX_TONE_VOL            3           /* Max Tone Volume */
#define MAX_TERMINAL_NO         16          /* Max Number of Terminal, STD_NUM_OF_TERMINALS */

#define PLU_MAX_DIGIT           STD_PLU_NUMBER_LEN      /* Maximum number of digits for PLU# */
#define PLU_MAX_PRICE           9999999L    /* limitation for building */
#define PLU_MAX_PM              99          /* limitation for building */
#define	MAX_PRINT_BUFFER		2048		/* Maximum number of characters available in print buffer */
#define PLU_STANDARD_RPT_CODE   10          // Standard report code used with PLU building up ItemSalesPLUSetupBuilding()
#define PLU_STANDARD_GROUP_NO   89          // Standard group number used with PLU building up ItemSalesPLUSetupBuilding() for Compass

#define PLU_MAX_TOUCH_PRESET    10          // max number of keyed PLU slots in menu page 10 for PLU buttons

/*------------------------------------------------------------------------*\
*   UNIVERSAL DEFINITION
\*------------------------------------------------------------------------*/

/* MDC Address Definition */
/*
	The following macros allow access to a specific bit within an MDC
	location.

	Notice that the orderings of the bits depends on whether the nibble is
	an even numbered nibble or an odd numbered nibble.

	If the MDC location is an odd number, then use the macros
	beginning with ODD_MDC.  An example would be MDC_ETK_ADR
	which is 95 which is odd.
		Bit		Macro
		A		ODD_MDC_BIT3
		B		ODD_MDC_BIT2
		C		ODD_MDC_BIT1
		D		ODD_MDC_BIT0

  	If the MDC location is an even number, then use the macros
	beginning with EVEN_MDC.
		Bit		Macro
		A		EVEN_MDC_BIT3
		B		EVEN_MDC_BIT2
		C		EVEN_MDC_BIT1
		D		EVEN_MDC_BIT0

	These are used with the CliParaMDCCheck function as in
		if (CliParaMDCCheck(MDC_ETK_ADR, ODD_MDC_BIT3))
*/
UCHAR   CliParaMDCCheck(USHORT address, UCHAR field);
UCHAR   CliParaMDCCheckField(USHORT address, UCHAR field);

#define ODD_MDC_BIT0            0x01    /* MDC Odd Address Bit 0, same as MDC_PARAM_BIT_D */
#define ODD_MDC_BIT1            0x02    /* MDC Odd Address Bit 1, same as MDC_PARAM_BIT_C */
#define ODD_MDC_BIT2            0x04    /* MDC Odd Address Bit 2, same as MDC_PARAM_BIT_B */
#define ODD_MDC_BIT3            0x08    /* MDC Odd Address Bit 3, same as MDC_PARAM_BIT_A */
#define EVEN_MDC_BIT0           0x10    /* MDC even Address Bit 0, same as MDC_PARAM_BIT_D */
#define EVEN_MDC_BIT1           0x20    /* MDC even Address Bit 1, same as MDC_PARAM_BIT_C */
#define EVEN_MDC_BIT2           0x40    /* MDC even Address Bit 2, same as MDC_PARAM_BIT_B */
#define EVEN_MDC_BIT3           0x80    /* MDC even Address Bit 3, same as MDC_PARAM_BIT_A */

#define MDC_ODD_MASK            0x0F    /* Mask Data,   V3.3 */
#define MDC_EVEN_MASK           0xF0    /* Mask Data,   V3.3 */

// The following macros are used with the function
// CliParaMDCCheckField() to make the job of accessing
// the MDC bits easier.
#define FLAG_MDC_BITD            0x01    /* MDC Address independent Bit D or Bit 0 returned by CliParaMDCCheckField() */
#define FLAG_MDC_BITC            0x02    /* MDC Address independent Bit C or Bit 1 returned by CliParaMDCCheckField() */
#define FLAG_MDC_BITB            0x04    /* MDC Address independent Bit B or Bit 2 returned by CliParaMDCCheckField() */
#define FLAG_MDC_BITA            0x08    /* MDC Address independent Bit A or Bit 3 returned by CliParaMDCCheckField() */

#define MDC_PARAM_BIT_A         'A'  // same as ODD_MDC_BIT3 or EVEN_MDC_BIT3
#define MDC_PARAM_BIT_B         'B'  // same as ODD_MDC_BIT2 or EVEN_MDC_BIT2
#define MDC_PARAM_BIT_C         'C'  // same as ODD_MDC_BIT1 or EVEN_MDC_BIT1
#define MDC_PARAM_BIT_D         'D'  // same as ODD_MDC_BIT0 or EVEN_MDC_BIT0
#define MDC_PARAM_2_BIT_A       'M'  //same area as BIT_A and BIT_B
#define MDC_PARAM_2_BIT_C       'N'  //same area as BIT_C and BIT_D
#define MDC_PARAM_BYTE			'Z'	//Z is not used by anything except for MDC 476 with gets the whole byte of information
#define MDC_PARAM_NIBBLE_A		'Y'	//Y is not used by anything, alternate or unused nibble from MDC addresses, shift/mask used
#define MDC_PARAM_NIBBLE_B		'X'	//standard or used nibble from MDC addresses, same area as MDC_PARAM_2_BIT_A and MDC_PARAM_2_BIT_C

/* Return Status */

#define SUCCESS                 0x00    /* Success Return Status */

/* Gift Card Comm Status */
#define	GIFT_COMM_SUCCESS		1		/* set to 1 for compelted gift tran */
#define	GIFT_COMM_NO_ATTEMPT	0		/* set to 0 for for no attempts made */
#define	GIFT_COMM_FAILURE		2		/* set to 2 if communication has failed */

/*------------------------------------------------------------------------*\
*   MSR IDENTIFICATION CODE DEFINE  R3.1
\*------------------------------------------------------------------------*/

#define MSR_ID_CASHIER      '1'         /* MSR ID is cashier        */
#define MSR_ID_SERVER       '2'         /* MSR ID is server         */
#define MSR_ID_BARTENDER    '3'         /* MSR ID is bartender      */
#define MSR_ID_EMPLOYEE     '4'         /* MSR ID is emplyee        */
#define MSR_ID_SUPERVISOR   '5'         /* MSR ID is supervisor     */
#define MSR_ID_FQS          '6'         /* MSR ID is frequent shopper */
#define MSR_ID_CASINT       '7'         /* MSR ID is cashier interrupt, R3.3 */

#define MSR_ID_GENERIC      '9'         // MSR ID is a generic which may be one of several.  Used for Amtrak.

// Following defines are used with Amtrak functionality to indicate ranges for various Job Codes
#define MSR_ID_JOBCODE_UNASSIGNED    0             // Access no functionality other than standard employee
#define MSR_ID_JOBCODE_REG_MIN      50             // Access REG mode functionality as a Cashier
#define MSR_ID_JOBCODE_REG_MAX      69             // Access REG mode functionality as a Cashier
#define MSR_ID_JOBCODE_SUP_MIN      70             // Access SUP mode functionality as a Supervisor
#define MSR_ID_JOBCODE_SUP_REG      80             // Access SUP mode functionality and REG mode functionality as a Supervisor and Cashier
#define MSR_ID_JOBCODE_SUP_MAX      99             // Access SUP mode functionality as a Supervisor

/*------------------------------------------------------------------------*\
*   ISP LOG IDENTIFICATION CODE DEFINE  V3.3
*     used with ITEMOTHER *pItem->uchAction
*     used with CLIREQISPLOG *pReq->uchAction
*     used with global variable UCHAR uchResetLog
\*------------------------------------------------------------------------*/

#define ISP_RESET_READ              1
#define ISP_RESET_RESET             2
#define ISP_RESET_ISSUE             3
#define UIF_UCHRESETLOG_NOACTION           0xFF      // used with uchResetLog to indicate, print the mnemonic into the Electronic Journal
#define UIF_UCHRESETLOG_LOGONLY            0xFE      // used with uchResetLog to indicate reset with a log entry only
#define UIF_UCHRESETLOG_TRAILERONLY        0xFD      // used with uchResetLog to indicate reset with a log entry only
#define UIF_UCHRESETLOG_LOGWITHTRAILER     0xFC      // used with uchResetLog to indicate reset

#define ISP_DAYPTD_DAY              1
#define ISP_DAYPTD_PTD              2

#define ISP_CURSAVE_CUR             1
#define ISP_CURSAVE_SAVE            2

/*------------------------------------------------------------------------*\
*   PARAMETER - MODIFIER STATUS OF KEYED DEPT DATA, 2172
\*------------------------------------------------------------------------*/

#define MOD_STAT_EVERSION   0x01    /* E-Version Modifier */
#define MOD_STAT_VELOCITY   0x02    /* UPC-Velocity Modifier (R2.0)

/*------------------------------------------------------------------------*\
*   PARAMETER - TOD DAY OF WEEK 2172
\*------------------------------------------------------------------------*/

#define TOD_WEEK_SUN            0     /* Sunday */
#define TOD_WEEK_MON            1     /* Monday */
#define TOD_WEEK_TUS            2     /* Tuesday */
#define TOD_WEEK_WED            3     /* Wednesday */
#define TOD_WEEK_THR            4     /* Thursday */
#define TOD_WEEK_FRI            5     /* Friday */
#define TOD_WEEK_SAT            6     /* Saturday */

/*------------------------------------------------------------------------*\
*   ACTION CODE DEFINE
\*------------------------------------------------------------------------*/

#define AC_GCNO_ASSIN           1           /* GUEST CHECK NO. ASSIGNMENT   */
#define AC_GCFL_READ_RPT        2           /* GUEST CHECK FILE READ REPORT  */
#define AC_GCFL_RESET_RPT       3           /* GUEST CHECK FILE RESET REPORT  */
#define AC_PLU_MENU             4           /* SET PLU NO.ON MENU PAGE  */
#define AC_SET_CTLPG            5           /* SET CONTROL TABLE OF MENU PAGE  */
#define AC_MANU_KITCH           6           /* MANUAL ALTERNATIVE KITCHEN PRINTER ASSIGNMENT */
#define AC_CASH_ABKEY           7           /* CASHIER A/B KEYS ASSIGNMENT  */
#define AC_RANGE_GCNO           9           /* ASSIGNMENT OF RANGE OF GC NUMBER FOR WAITER */
#define AC_LOAN                 10          /* LOAN, 2172 */
#define AC_PICKUP               11          /* PICKUP, 2172 */
#define AC_SUPSEC_NO            15          /* SUPERVISOR SECRET NUMBER CHANGE  */
#define AC_TOD                  17          /* SET DATE AND TIME  */
#define AC_PTD_RPT              18          /* PTD RESET REPORT  */
#define AC_CASH_RESET_RPT       19          /* CASHIER RESET REPORT  */
#define AC_CASH_ASSIN           20          /* CASHIER NO. ASSIGNMENT  */
#define AC_CASH_READ_RPT        21          /* CASHIER READ REPORT  */
#define AC_REGFIN_READ_RPT      23          /* REGISTER FINANCIAL REPORT  */
#define AC_HOURLY_READ_RPT      24          /* HOURLY ACTIVITY REPORT  */
#define AC_DEPTSALE_READ_RPT    26          /* DEPARTMENT SALES READ REPORT  */
#define AC_PLUSALE_READ_RPT     29          /* PLU SALES READ REPORT  */
#define AC_CPN_READ_RPT         30          /* COMBINATION COUPON READ REPORT */
//GroupReport - CSMALL
#define AC_PLUGROUP_READ_RPT	31			/* PLU GROUP SUMMARY READ REPORT */
#define AC_CSTR_MAINT           33          /* CONTROL STRINGS MAINTENANCE */
#define AC_PLU_RESET_RPT        39          /* PLU ALL RESET REPORT  */
#define AC_CPN_RESET_RPT        40          /* COMBINATION COUPON RESET REPORT */
#define AC_WAI_READ_RPT         41          /* WAITER FILE READ REPORT  */
#define AC_COPY_TTL             42          /* COPY TOTAL/COUNTER FILE  */
#define AC_WAI_RESET_RPT        46          /* WAITER FILE RESET REPORT  */
#define AC_WAI_ASSIN            50          /* WAITER NO. ASSIGNMENT  */
#define AC_PLU_MAINT            63          /* PLU  MAINTENANCE  */
#define AC_PLU_PRICE_CHG        64          /* PLU PRICE CHANGE  */
#define AC_PLU_DOWN             66          /* BROADCAST/REQUEST TO DOWNLOAD PLU PARAMETER  */
#define AC_PLU_ADDDEL           68          /* PLU ADD/DELETION */
#define AC_EJ_READ_RPT          70          /* E/J READ REPORT  */
#define AC_PPI_MAINT            71          /* PPI MAINTENANCE  */
#define AC_PLUPARA_READ_RPT     72          /* PLU PARAMETER FILE READ REPORT  */
#define AC_ALL_DOWN             75          /* BROADCAST/REQUEST TO DOWNLOAD ALL PARAMETER  */
#define AC_START_STOP_CPM       78          /* START/STOP CPM */
#define AC_PLU_MNEMO_CHG        82          /* PLU MNEMONIC CHANGE  */
#define AC_ROUND                84          /* SET ROUNDING TABLE  */
#define AC_DISCON_MAST          85          /* DISCONNECT MASTER OR BACKUP MASTER */
#define AC_DISC_BONUS           86          /* SET DISCOUNT/BONUS % RATES  */
#define AC_SOFTCHK_MSG          87          /* SET 42 Char. SET TRAILER PRINT for SOFT CHECK */
#define AC_SALES_PROMOTION      88          /* SET SALES PROMOTION MESSAGE  */
#define AC_CURRENCY             89          /* SET CURRENCY CONVERSION RATE  */
#define AC_TALLY_CPM_EPT        97          /* READ AND RESET EPT AND CPM TALLY */
#define AC_EOD_RPT              99          /* END OF DAY RESET REPORT  */
#define AC_EJ_RESET_RPT         100         /* E/J RESET REPORT  */
#define AC_STORE_FORWARD		102			/* STORE AND FORWARD */
#define AC_EMG_GCFL_CLS         103         /* EMERGENT GUEST CHECK FILE CLOSE  */
#define AC_TARE_TABLE           111         /* SET TARE TABLE */
#define AC_DEPT_ADDDEL          113         /* DEPARTMENT  ADD/DELETE, 2172  */
#define AC_DEPT_MAINT           114         /* DEPARTMENT  MAINTENANCE/REPORT  */
#define AC_DEPT_MENU            115         /* SET DEPT NO.ON MENU PAGE, 2172  */
#define AC_SET_PLU              116         /* ASSIGN SET MENU  */
#define AC_EMG_CASHFL_CLS       119         /* EMERGENT CASHIER FILE CLOSE  */
#define AC_MAJDEPT_RPT          122         /* MAJOR DEPARTMENT SALES READ REPORT  */
#define AC_MEDIA_RPT            123         /* MEDIA FLASH REPORT  */
#define AC_TAXTBL1              124         /* TAX TABLE #1 PROGRAMMING */
#define AC_TAXTBL2              125         /* TAX TABLE #2 PROGRAMMING */
#define AC_TAXTBL3              126         /* TAX TABLE #3 PROGRAMMING */
#define AC_TAX_RATE             127         /* SET TAX/SERVICE/RECIPROCAL RATES  */
#define AC_MISC_PARA            128         /* SET MISC. LIMITATION DATA */
#define AC_TAXTBL4              129         /* TAX TABLE #4 PROGRAMMING */
#define AC_PIGRULE              130         /* PIG RULE TABLE */
#define AC_SERVICE_READ_RPT     131         /* SERVICE TIME READ REPORT */
#define AC_SERVICE_RESET_RPT    132         /* SERVICE TIME RESET REPORT */
#define AC_SERVICE_MAINT        133         /* SERVICE TIME MAINTENANCE */
#define AC_PROG_READ_RPT        135         /* PROGRAMABLE READ REPORT */
#define AC_PROG_RESET_RPT       136         /* PROGRAMABLE RESET REPORT */
#define AC_DISPENSER_PARA       137         /* BEVERAGE DISPENSER PARAMETER */
#define AC_EMG_WAIFL_CLS        146         /* EMERGENT WAITER FILE CLOSE  */
#define AC_ETK_READ_RPT         150         /* EMPLOYEE TIME KEEPING FILE READ REPORT */
#define AC_ETK_RESET_RPT        151         /* EMPLOYEE TIME KEEPING FILE RESET REPORT */
#define AC_ETK_ASSIN            152         /* EMPLOYEE NUMBER ASSIGNMENT  */
#define AC_ETK_MAINT            153         /* ETK FILE MAINTENANCE */
#define AC_ETK_LABOR_COST       154         /* SET ETK LABOR COST VALUE */
#define AC_OEP_MAINT            160         /* SET ORDER ENTRY PROMPT TABLE */
#define AC_CPN_MAINT            161         /* CONBINATION COUPON MAINTENANCE */
#define AC_FLEX_DRIVE           162         /* SET FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL PARAMETER */
#define AC_TONE_CTL             169         /* ADJUST TONE VOLUME */
#define AC_PLU_SLREST           170          /* PLU SALES RESTRICTION TABLE, 2172 */
#define AC_SUP_DOWN             175         /* BROADCAST/REQUEST TO DOWNLOAD SUPER PARAMETER */
#define AC_CSOPN_RPT            176         /* OPEN/GUEST CHECK STATUS READ REPORT */
//adding new AC for dynamic PLU window refresh
#define AC_RELOADOEP			194			/* Dynamic Window Reload - CSMALL*/
#define AC_BOUNDARY_AGE         208         /* SET BOUNDARY AGE, 2172 */
#define	AC_COLORPALETTE			209			/* SET COLOR PALETTE */
#define AC_IND_READ_RPT         223         /* INDIVIDUAL READ REPORT */
#define AC_IND_RESET_RPT        233         /* INDIVIDUAL RESET REPORT */
#define AC_SET_DELAYED_BAL		275			// Set Delayed Balance EOD JHHJ
#define AC_SET_DELAYED_BAL_OFF	276			// Turn Delayed Balance Off JHHJ
#define AC_JOIN_CLUSTER			444			// Join a cluster as Satellite, CID_AC444FUNCTION  RJC
#define AC_SET_LAYOUT			777			// Change Layout File  RJC
#define AC_CONFIG_INFO			888			// Configuration Information JHHJ 10-22-04
#define AC_FORCE_MAST           985         // Force Master and Backup Master to be up to date
#define AC_SHUTDOWN				999			// Shut down Application


// Defines for security level for offset into ParaActCodeSec[]
// Most Supvervisor action codes use the Action Code number as the address
// however if the Action Code number is greater than 250 it will be out of
// range for an address so special addresses are provided below.
//
// The max value of 250 is indicated because the offset has to be within
// the memory resident array ParaActCodeSec[] which contains the security
// settings.  Each byte of ParaActCodeSec[] contains two security codes as
// they are stored in a single nibble.
#define PARA_SEC_AC_SET_DELAYED_BAL     139    // AC275
#define PARA_SEC_AC_JOIN_CLUSTER        222    // AC444


/*------------------------------------------------------------------------*\
*   PROGRAM NO DEFINE
\*------------------------------------------------------------------------*/

#define PG_MDC                  1           /* MACHINE DEFINITION CODE */
#define PG_FLX_MEM              2           /* FLEXIBLE MEMORY ASSIGNMENT */
#define PG_FSC                  3           /* FSC FOR REGULAR KEYBOARD */
#define PG_SEC_PROG             6           /* SECURITY NUMBER FOR PROGRAM MODE */
#define PG_STOREG_NO            7           /* STORE/REGISTER No. */
#define PG_SUPLEVEL             8           /* SUPERVISOR NUMBER/LEVEL ASSIGNMENT */
#define PG_ACT_SEC              9           /* ACTION CODE SECURITY */
#define PG_TRAN_HALO            10          /* TRANSACTION HALO */
#define PG_PRESET_AMT           15          /* PRESET AMOUNT CASH TENDER */
#define PG_HOURLY_TIME          17          /* HOURLY ACTIVITY TIME */
#define PG_SLIP_CTL             18          /* SLIP PRINTER FEED CONTROL */
#define PG_TRAN_MNEMO           20          /* TRANSACTION MNEMONICS */
#define PG_LEAD_MNEMO           21          /* LEAD THROUGH MNEMONICS */
#define PG_RPT_NAME             22          /* REPORT NAME */
#define PG_SPEC_MNEMO           23          /* SPECIAL MNEMONICS */
#define PG_PCIF                 39          /* PC INTERFACE */
#define PG_ADJ_MNEMO            46          /* ADJECTIVE MNEMONICS */
#define PG_PRT_MNEMO            47          /* PRINT MODIFIER MNEMONICS */
#define PG_MAJDEPT_MNEMO        48          /* MAJOR DEPARTMENT MNEMONICS */
#define PG_AUTO_KITCH           49          /* AUTO ALTERNATIVE KITCHEN PRINTER ASSIGNMENT */
#define PG_SHR_PRT              50          /* ASSIGNMENT TERMINAL NO. INSTALLING SHARED PRINTER */
#define PG_KDS_IP               51          /* ASSIGNMENT IP ADDRESS FOR KDS, 2172 */
#define PG_HOTELID              54          /* SET HOTEL ID */
#define PG_CHA24_MNEMO          57          /* 24 CHRACTERS MNEMONICS */
#define PG_TTLKEY_TYP           60          /* TOTAL KEY TYPE ASSIGNMENT */
#define PG_TTLKEY_CTL           61          /* TOTAL KEY CONTROL */
#define PG_TEND_PARA            62          /* TENDER KEY PARAMETER,    V3.3 */
#define PG_MLD_MNEMO            65          /* MLD MNEMONICS,    V3.3 */
#define PG_AUTO_CPN				67			/* Auto Coupon assignment JHHJ */
#define PG_UNLOCK_NO            95          /* ASSIGNMENT UNLOCK CODE FOR SECURITY, Rel 1.4 */
#define PG_HOSTS_IP             99          /* ASSIGNMENT HOSTS IP ADDRESS, 2172 */

/*------------------------------------------------------------------------*\
*   PROGRAM MODE DEFINE
\*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*\
*   Define - Machine Definition Code Address (PRG #1)
\*------------------------------------------------------------------------*/

#define MDC_CLI1_ADR           1     /* Client/Server #1 */
#define MDC_CLI2_ADR           2     /* Client/Server #2 */
#define MDC_CLI3_ADR           3     /* Client/Server #3 */
#define MDC_CLI4_ADR           4     /* Client/Server #4 */
#define MDC_CLI5_ADR           5     /* Client/Server #5 */
#define MDC_CLI6_ADR           6     /* Client/Server #6 */
#define MDC_CLI7_ADR           7     /* Client/Server #7 */
#define MDC_CLI8_ADR           8     /* Client/Server #8 */
#define MDC_CLI9_ADR           9     /* Client/Server #9 */
#define MDC_CLI10_ADR         10     /* Client/Server #10 */
#define MDC_TAXINTL_ADR       11     /* Tax (Int&l Canada), V3.3 */
#define MDC_GCNO_ADR          12     /* Guest Check No. */
#define MDC_KPTR_ADR          13     /* Kitchen Printer */
#define MDC_DRAWER_ADR        14     /* Drawer & Date And Time */
#define MDC_TAX_ADR           15     /* Tax & Canadian Tax     R3.1 */
#define MDC_CANTAX_ADR        16     /* Canadian Tax           R3.1 */
#define MDC_CASIN_ADR         17     /* Cashier Sign-In */
#define MDC_WTIN_ADR          18     /* Waiter Sign-In */
#define MDC_SIGNOUT_ADR       19     /* Sign-Out & Negative or Zero Sales */
#define MDC_SLIP_ADR          20     /* Slip Print & Sales Promotion Header */
#define MDC_INTERVENE_ADR     21     /* Supervisor Intervention */
#define MDC_USCUSTOMS_ADR     21	 // US Customs Functionality cwunn 4/25/03
#define MDC_CANCEL_ADR        22     /* Cancel & Abort */
#define MDC_CHRGTIP1_ADR      23     /* Charge Tips #1 */
#define MDC_CHRGTIP2_ADR      24     /* Charge Tips #2 */
#define MDC_CHRGTIP3_ADR      25     /* Charge Tips #3         R3.1 */
#define MDC_DEPT1_ADR         27     /* Department & PLU #1 */
#define MDC_DEPT2_ADR         28     /* Department & PLU #2 is also MDC_PRINT_ITEM_MNEMO */
#define MDC_SCALE_ADR         29     /* Scale */
#define MDC_VALIDATION_ADR    30     /* 1 line validation, often used after (fsPrintStatus & PRT_VALIDATION) check */
#define MDC_MODID11_ADR       31     /* Modified Item Discount Key (ID #1) */
#define MDC_MODID12_ADR       32     /* Modified Item Discount Key (ID #1) */
#define MDC_MODID13_ADR       33     /* Modified Item Discount Key (ID #1) */
#define MDC_MODID14_ADR       34     /* Modified Item Discount Key (ID #1) */
#define MDC_MODID21_ADR       35     /* Modified Item Discount Key (ID #2) */
#define MDC_MODID22_ADR       36     /* Modified Item Discount Key (ID #2) */
#define MDC_MODID23_ADR       37     /* Modified Item Discount Key (ID #2) */
#define MDC_MODID24_ADR       38     /* Modified Item Discount Key (ID #2) */
#define MDC_RDISC11_ADR       39     /* Regular Discount #1 Key */
#define MDC_RDISC12_ADR       40     /* Regular Discount #1 Key */
#define MDC_RDISC13_ADR       41     /* Regular Discount #1 Key */
#define MDC_RDISC14_ADR       42     /* Regular Discount #1 Key */
#define MDC_RDISC21_ADR       43     /* Regular Discount #2 Key */
#define MDC_RDISC22_ADR       44     /* Regular Discount #2 Key */
#define MDC_RDISC23_ADR       45     /* Regular Discount #2 Key */
#define MDC_RDISC24_ADR       46     /* Regular Discount #2 Key */
#define MDC_EC_ADR            47     /* Error Correct */
#define MDC_PB_ADR            48     /* Manual PB Operation By PB Key */
#define MDC_ADDCHK1_ADR       49     /* Manual Add Check Operation By Add Check Key #1 */
#define MDC_ADDCHK2_ADR       50     /* Manual Add Check Operation By Add Check Key #2 */
#define MDC_ADDCHK3_ADR       51     /* Manual Add Check Operation By Add Check Key #3 */
#define MDC_TAX2_ADR          52     /* Tax 2   ** add '94 4/25             */
#define MDC_MENU_ADR          53     /* Menu Shift Key & No Sale */
#define MDC_PSN_ADR           54     /* Number Of Person */
#define MDC_PO_ADR            55     /* Paid Out & Received On Account */
#define MDC_PSELVD_ADR        56     /* Preselect Void */
#define MDC_RECEIPT_ADR       57     /* Receipt/No Receipt Key & Table No. Key */
#define MDC_TAXEXM_ADR        58     /* Tax Exempt Keys */
#define MDC_TIPPO1_ADR        59     /* Tips Paid Out Key #1 */
#define MDC_TIPPO2_ADR        60     /* Tips Paid Out Key #2 & Declared Tips Key */
#define MDC_VOID_ADR          61     /* Void Key */
#define MDC_CHKSUM_ADR        62     /* Check sum */
#define MDC_PLU_TICKET        62     /* PLU ticket printing  */
#define MDC_TEND11_ADR        63     /* Tender #1, MDC page 1 */
#define MDC_TEND12_ADR        64     /* Tender #1, MDC page 2 */
#define MDC_TEND21_ADR        65     /* Tender #2, MDC page 1 */
#define MDC_TEND22_ADR        66     /* Tender #2, MDC page 2 */
#define MDC_TEND31_ADR        67     /* Tender #3, MDC page 1 */
#define MDC_TEND32_ADR        68     /* Tender #3, MDC page 2 */
#define MDC_TEND41_ADR        69     /* Tender #4, MDC page 1 */
#define MDC_TEND42_ADR        70     /* Tender #4, MDC page 2 */
#define MDC_TEND51_ADR        71     /* Tender #5, MDC page 1 */
#define MDC_TEND52_ADR        72     /* Tender #5, MDC page 2 */
#define MDC_TEND61_ADR        73     /* Tender #6, MDC page 1 */
#define MDC_TEND62_ADR        74     /* Tender #6, MDC page 2 */
#define MDC_TEND71_ADR        75     /* Tender #7, MDC page 1 */
#define MDC_TEND72_ADR        76     /* Tender #7, MDC page 2 */
#define MDC_TEND81_ADR        77     /* Tender #8, MDC page 1 */
#define MDC_TEND82_ADR        78     /* Tender #8, MDC page 2 */
// Tender 9 - 12 MDC page 1 and page 2 were added later and are below at MDC_TEND91_ADR
#define MDC_FC1_ADR           79     /* Foreign Currency #1 */
#define MDC_FC2_ADR           80     /* Foreign Currency #2 */
#define MDC_GCTRN_ADR         81     /* Guest Check Transfer */
#define MDC_DRAWER2_ADR       82     /* drawer open timing */
#define MDC_POST_ADR          83     /* post receipt */
#define MDC_PRETEND_ADR       84     /* allow/not allow QTY extension entry for preset tender */
#define MDC_SUPER_ADR         85     /* Supervisor Sign-In & Resettable/Non-Resettable Control see TtlTresetFin() */
#define MDC_MSRSIGNIN_ADR	  85	 /* MSR Required Sign In*/
#define MDC_PCTL_ADR          86     /* Read Report Print Control & Reset Report Print Control */
#define MDC_PARA_ADR          87     /* Parameter Read Report */
#define MDC_COUPON_ADR        88     /* Coupon Report */
#define MDC_DEPTREPO_ADR      89     /* Department Report & PLU Report */
#define MDC_HOUR_ADR          90     /* Hourly Activity & Cashier Reset & Waiter File Report */
#define MDC_EOD1_ADR          91     /* EOD Report #1 */
#define MDC_EOD2_ADR          92     /* EOD Report #2 */
#define MDC_PTD1_ADR          93     /* PTD Report #1 */
#define MDC_PTD2_ADR          94     /* PTD Report #2 */
#define MDC_ETK_ADR           95     /* ETK */
#define MDC_EODPTD_ADR        96     /* EOD/PTD */
#define MDC_WAITER_ADR        97     /* Waiter Report Option */
#define MDC_RESERVE4_ADR      98     /* Reserve 4 */
#define MDC_CPPARA1_ADR       99     /* Charge Posting Parameter #1 */
#define MDC_CPPARA2_ADR      100     /* Charge Posting Parameter #2 also EPT receipt printing */

#define MDC_RPTFIN1_ADR      101     /* Financial Report */
#define MDC_RPTFIN2_ADR      102     /* Financial Report */
#define MDC_RPTFIN3_ADR      103     /* Financial Report */
#define MDC_RPTFIN4_ADR      104     /* Financial Report */
#define MDC_RPTFIN5_ADR      105     /* Financial Report */
#define MDC_RPTFIN6_ADR      106     /* Financial Report */
#define MDC_RPTFIN7_ADR      107     /* Financial Report */
#define MDC_RPTFIN8_ADR      108     /* Financial Report */
#define MDC_RPTFIN9_ADR      109     /* Financial Report */
#define MDC_RPTFIN10_ADR     110     /* Financial Report */
#define MDC_RPTFIN11_ADR     111     /* Financial Report */
#define MDC_RPTFIN12_ADR     112     /* Financial Report */
#define MDC_RPTFIN13_ADR     113     /* Financial Report */
#define MDC_RPTFIN14_ADR     114     /* Financial Report */
#define MDC_RPTFIN15_ADR     115     /* Financial Report */
#define MDC_RPTFIN16_ADR     116     /* Financial Report */
#define MDC_RPTFIN17_ADR     117     /* Financial Report */
#define MDC_RPTFIN18_ADR     118     /* Financial Report */
#define MDC_RPTFIN19_ADR     119     /* Financial Report */
#define MDC_RPTFIN20_ADR     120     /* Financial Report */
#define MDC_RPTFIN21_ADR     121     /* Financial Report */
#define MDC_RPTFIN22_ADR     122     /* Financial Report */
#define MDC_RPTFIN23_ADR     123     /* Financial Report */
#define MDC_RPTFIN24_ADR     124     /* Financial Report */
#define MDC_RPTFIN25_ADR     125     /* Financial Report */
#define MDC_RPTFIN26_ADR     126     /* Financial Report */
#define MDC_RPTFIN27_ADR     127     /* Financial Report */
#define MDC_RPTFIN28_ADR     128     /* Financial Report */
#define MDC_RPTFIN29_ADR     129     /* Financial Report       R3.0 */
#define MDC_RPTFIN30_ADR     130     /* Financial Report       R3.1 */

#define MDC_RPTCAS1_ADR      131     /* Cashier Report   */
#define MDC_RPTCAS2_ADR      132     /* Cashier Report   */
#define MDC_RPTCAS3_ADR      133     /* Cashier Report   */
#define MDC_RPTCAS4_ADR      134     /* Cashier Report   */
#define MDC_RPTCAS5_ADR      135     /* Cashier Report   */
#define MDC_RPTCAS6_ADR      136     /* Cashier Report   */
#define MDC_RPTCAS7_ADR      137     /* Cashier Report   */
#define MDC_RPTCAS8_ADR      138     /* Cashier Report   */
#define MDC_RPTCAS9_ADR      139     /* Cashier Report   */
#define MDC_RPTCAS10_ADR     140     /* Cashier Report   */
#define MDC_RPTCAS11_ADR     141     /* Cashier Report   */
#define MDC_RPTCAS12_ADR     142     /* Cashier Report   */
#define MDC_RPTCAS13_ADR     143     /* Cashier Report   */
#define MDC_RPTCAS14_ADR     144     /* Cashier Report   */
#define MDC_RPTCAS15_ADR     145     /* Cashier Report   */
#define MDC_RPTCAS16_ADR     146     /* Cashier Report   */
#define MDC_RPTCAS17_ADR     147     /* Cashier Report   */
#define MDC_RPTCAS18_ADR     148     /* Cashier Report   */
#define MDC_RPTCAS19_ADR     149     /* Cashier Report   */
#define MDC_RPTCAS20_ADR     150     /* Cashier Report   */
#define MDC_RPTCAS21_ADR     151     /* Cashier Report   */
#define MDC_RPTCAS22_ADR     152     /* Cashier Report R3.1 */
#define MDC_RPTCAS23_ADR     153     /* Cashier Report R3.1 */
#define MDC_RPTCAS24_ADR     154     /* Cashier Report V3.3 */
#define MDC_RPTCAS25_ADR     155     /* Cashier Report V3.3 */
#define MDC_RPTCAS26_ADR     156     /* Cashier Report V3.3 */

#define MDC_RPTWAI1_ADR      157     /* Waiter Report    */
#define MDC_RPTWAI2_ADR      158     /* Waiter Report    */
#define MDC_RPTWAI3_ADR      159     /* Waiter Report    */
#define MDC_RPTWAI4_ADR      160     /* Waiter Report    */
#define MDC_RPTWAI5_ADR      161     /* Waiter Report    */
#define MDC_RPTWAI6_ADR      162     /* Waiter Report    */
#define MDC_RPTWAI7_ADR      163     /* Waiter Report    */
#define MDC_RPTWAI8_ADR      164     /* Waiter Report    */
#define MDC_RPTWAI9_ADR      165     /* Waiter Report    */
#define MDC_RPTWAI10_ADR     166     /* Waiter Report    */
#define MDC_RPTWAI11_ADR     167     /* Waiter Report    */
#define MDC_RPTWAI12_ADR     168     /* Waiter Report    */
#define MDC_RPTWAI13_ADR     169     /* Waiter Report R3.1 */
#define MDC_RPTWAI14_ADR     170     /* Waiter Report R3.1 */
#define MDC_RPTWAI15_ADR     171     /* Waiter Report R3.1 */
#define MDC_RPTWAI16_ADR     172     /* Waiter Report R3.1 */

#define MDC_RPTGCF1_ADR      173     /* GCF Report */
#define MDC_RPTGCF2_ADR      174     /* GCF Report */

#define MDC_DFL_INT_TIME      175     /* interval time for display on the fly */
#define MDC_DFL_SND_TIME      176     /* send timer value for display on the fly */
#define MDC_DSP_ON_FLY        177     /* display on the fly */
#define MDC_DSP_ON_FLY2       178     /* display on the fly */
#define MDC_SHR_MON_TIME_ADR  179     /* Shared printer monitor timer */
#define MDC_CPTEND1_ADR       180     /* Tender #1 for charge posting, Tender #1, MDC page 3 */
#define MDC_CPTEND2_ADR       181     /* Tender #2 for charge posting, Tender #2, MDC page 3 */
#define MDC_CPTEND3_ADR       182     /* Tender #3 for charge posting, Tender #3, MDC page 3 */
#define MDC_CPTEND4_ADR       183     /* Tender #4 for charge posting, Tender #4, MDC page 3 */
#define MDC_CPTEND5_ADR       184     /* Tender #5 for charge posting, Tender #5, MDC page 3 */
#define MDC_CPTEND6_ADR       185     /* Tender #6 for charge posting, Tender #6, MDC page 3 */
#define MDC_CPTEND7_ADR       186     /* Tender #7 for charge posting, Tender #7, MDC page 3 */
#define MDC_CPTEND8_ADR       187     /* Tender #8 for charge posting, Tender #8, MDC page 3 */
#define MDC_CPTEND9_ADR       188     /* Tender #9 for charge posting, Tender #9, MDC page 3 */
#define MDC_CPTEND10_ADR      189     /* Tender #10 for charge posting, Tender #10, MDC page 3 */
#define MDC_CPTEND11_ADR      190     /* Tender #11 for charge posting, Tender #11, MDC page 3 */
#define MDC_TEND91_ADR        191     /* Tender #9, MDC page 1 */
#define MDC_TEND92_ADR        192     /* Tender #9, MDC page 2 */
#define MDC_TEND101_ADR       193     /* Tender #10, MDC page 1 */
#define MDC_TEND102_ADR       194     /* Tender #10, MDC page 2 */
#define MDC_TEND111_ADR       195     /* Tender #11, MDC page 1 */
#define MDC_TEND112_ADR       196     /* Tender #11, MDC page 2 */
#define MDC_CPM_INTVL_TIME    197     /* interval time for PifWriteCom() on CPM */
#define MDC_CPM_WAIT_TIME     198     /* wait time for PifReadCom() on CPM  */
#define MDC_EPT_SND_TIME      199     /* send timer for PifWriteCom() on EPT */
#define MDC_EPT_RCV_TIME      200     /* receive timer for PifReadCom() on EPT */
#define MDC_TEND13_ADR        201     /* Tender #1, MDC page 4            */
#define MDC_TEND14_ADR        202     /* Tender #1, MDC page 5           */
#define MDC_TEND23_ADR        203     /* Tender #2, MDC page 4            */
#define MDC_TEND24_ADR        204     /* Tender #2, MDC page 5            */
#define MDC_TEND33_ADR        205     /* Tender #3, MDC page 4            */
#define MDC_TEND34_ADR        206     /* Tender #3, MDC page 5            */
#define MDC_TEND43_ADR        207     /* Tender #4, MDC page 4            */
#define MDC_TEND44_ADR        208     /* Tender #4, MDC page 5            */
#define MDC_TEND53_ADR        209     /* Tender #5, MDC page 4            */
#define MDC_TEND54_ADR        210     /* Tender #5, MDC page 5            */
#define MDC_TEND63_ADR        211     /* Tender #6, MDC page 4            */
#define MDC_TEND64_ADR        212     /* Tender #6, MDC page 5            */
#define MDC_TEND73_ADR        213     /* Tender #7, MDC page 4            */
#define MDC_TEND74_ADR        214     /* Tender #7, MDC page 5            */
#define MDC_TEND83_ADR        215     /* Tender #8, MDC page 4            */
#define MDC_TEND84_ADR        216     /* Tender #8, MDC page 5            */
#define MDC_TEND93_ADR        217     /* Tender #9, MDC page 4            */
#define MDC_TEND94_ADR        218     /* Tender #9, MDC page 5            */
#define MDC_TEND103_ADR       219     /* Tender #10, MDC page 4           */
#define MDC_TEND104_ADR       220     /* Tender #10, MDC page 5           */
#define MDC_TEND113_ADR       221     /* Tender #11, MDC page 4           */
#define MDC_TEND114_ADR       222     /* Tender #11, MDC page 5           */
#define MDC_DISPLAY_ADR       231     /* LCD Display                 R3.0   */
#define MDC_PRINTPRI_ADR      232     /* Print Priority              R3.0   */
#define MDC_COMBCOUPON_ADR    233     /* Combination Coupon          R3.0 is also MDC_DBLSIDEPRINT_ADR  */
#define MDC_TRANNUM_ADR       234     /* Unique Trans#               R3.0   */
#define MDC_DEPT3_ADR         235     /* Department & PLU #3   not print void item on receipt print  */
#define MDC_SPLIT_GCF_ADR     236     /* Split Guest Check           R3.1   */
#define MDC_RDISC31_ADR       237     /* Regular Discount #3 Key     R3.1   */
#define MDC_RDISC32_ADR       238     /* Regular Discount #3 Key     R3.1   */
#define MDC_RDISC33_ADR       239     /* Regular Discount #3 Key     R3.1   */
#define MDC_RDISC34_ADR       240     /* Regular Discount #3 Key     R3.1   */
#define MDC_RDISC41_ADR       241     /* Regular Discount #4 Key     R3.1   */
#define MDC_RDISC42_ADR       242     /* Regular Discount #4 Key     R3.1   */
#define MDC_RDISC43_ADR       243     /* Regular Discount #4 Key     R3.1   */
#define MDC_RDISC44_ADR       244     /* Regular Discount #4 Key     R3.1   */
#define MDC_RDISC51_ADR       245     /* Regular Discount #5 Key     R3.1   */
#define MDC_RDISC52_ADR       246     /* Regular Discount #5 Key     R3.1   */
#define MDC_RDISC53_ADR       247     /* Regular Discount #5 Key     R3.1   */
#define MDC_RDISC54_ADR       248     /* Regular Discount #5 Key     R3.1   */
#define MDC_RDISC61_ADR       249     /* Regular Discount #6 Key     R3.1   */
#define MDC_RDISC62_ADR       250     /* Regular Discount #6 Key     R3.1   */
#define MDC_RDISC63_ADR       251     /* Regular Discount #6 Key     R3.1   */
#define MDC_RDISC64_ADR       252     /* Regular Discount #6 Key     R3.1   */
#define MDC_EOD3_ADR          253     /* EOD Report #3               R3.1   */
#define MDC_EOD4_ADR          254     /* EOD Report #4               R3.1   */
#define MDC_PTD3_ADR          255     /* PTD Report #3               R3.1   */
#define MDC_PTD4_ADR          256     /* PTD Report #4               R3.1   */
#define MDC_NEWKEYSEQ_ADR     257     /* New key sequence,           V3.3   */
#define MDC_KP2_ADR           258     /* K/P option,                 V3.3   */
#define MDC_QTY2LINE_ADR	  258	  /* Print Qty Extension on 1 or 2 lines*/
#define MDC_VAT1_ADR          259     /* VAT#1,                      V3.3   */
#define MDC_VAT2_ADR          260     /* VAT#2,                      V3.3   */

#define MDC_RPTWAI17_ADR      269     /* Waiter Report R3.1 */

#define MDC_SLIPVAL_ADR       270

#define MDC_HOUR2_ADR         271     /* Hourly Activity Report,     V3.3 */
#define MDC_HOUR3_ADR         272     /* Hourly Activity Report,     V3.3 */
#define MDC_HOUR4_ADR         273     /* Hourly Activity Report,     V3.3 */

#define MDC_RPTFIN31_ADR      274     /* Financial Report            V3.3 */

#define MDC_EJ_ADR            275     /* E/J,                        V3.3 */
#define MDC_STR_FWD_ADR		 276		//Store and forward

#define MDC_EURO_ADR         277     /* Euro Enhancement         V3.4 */
#define MDC_EUROFC_ADR       278     /* Euro Enhancement FC1/2   V3.4 */
#define MDC_EUROFC2_ADR      279     /* Euro Enhancement FC3/4   2172 */
#define MDC_EUROFC3_ADR      280     /* Euro Enhancement FC5/6   2172 */
#define MDC_EUROFC4_ADR      281     /* Euro Enhancement FC7/8   2172 */

#define MDC_PLU2_ADR         295     /* PLU #2, 2172 */
#define MDC_PLU3_ADR         296     /* PLU #3, 2172 */
#define MDC_PLU4_ADR         297     /* PLU #4, 2172 */
#define MDC_PLU5_ADR         298     /* PLU #5, 2172 */

#define MDC_PLULB02L_ADR     299     /* PLU (Label 02), 2172 */
#define MDC_PLULB02H_ADR     300     /* PLU (Label 02), 2172 */
#define MDC_PLULB20L_ADR     301     /* PLU (Label 20), 2172 */
#define MDC_PLULB20H_ADR     302     /* PLU (Label 20), 2172 */
#define MDC_PLULB21L_ADR     303     /* PLU (Label 21), 2172  */
#define MDC_PLULB21H_ADR     304     /* PLU (Label 21), 2172  */
#define MDC_PLULB22L_ADR     305     /* PLU (Label 22), 2172  */
#define MDC_PLULB22H_ADR     306     /* PLU (Label 22), 2172  */
#define MDC_PLULB23L_ADR     307     /* PLU (Label 23), 2172  */
#define MDC_PLULB23H_ADR     308     /* PLU (Label 23), 2172  */
#define MDC_PLULB24L_ADR     309     /* PLU (Label 24), 2172  */
#define MDC_PLULB24H_ADR     310     /* PLU (Label 24), 2172  */
#define MDC_PLULB25L_ADR     311     /* PLU (Label 25), 2172  */
#define MDC_PLULB25H_ADR     312     /* PLU (Label 25), 2172  */
#define MDC_PLULB26L_ADR     313     /* PLU (Label 26), 2172  */
#define MDC_PLULB26H_ADR     314     /* PLU (Label 26), 2172  */
#define MDC_PLULB27L_ADR     315     /* PLU (Label 27), 2172  */
#define MDC_PLULB27H_ADR     316     /* PLU (Label 27), 2172  */
#define MDC_PLULB28L_ADR     317     /* PLU (Label 28), 2172  */
#define MDC_PLULB28H_ADR     318     /* PLU (Label 28), 2172  */
#define MDC_PLULB29L_ADR     319     /* PLU (Label 29), 2172  */
#define MDC_PLULB29H_ADR     320     /* PLU (Label 29), 2172  */
#define MDC_PLULB04L_ADR     321     /* PLU (Label 04), 2172  */
#define MDC_PLULB04H_ADR     322     /* PLU (Label 04), 2172  */

#define MDC_LOANPICK1_ADR    331     /* Loan/Pick-Up #1, 2172*/
#define MDC_LOANPICK2_ADR    332     /* Loan/Pick-Up #2, 2172 */

#define MDC_MONEY1_ADR       333     /* Money #1         2172 */

#define MDC_FC3_ADR          337     /* Foreign Currency #3, 2172 */
#define MDC_FC4_ADR          338     /* Foreign Currency #4, 2172 */
#define MDC_FC5_ADR          339     /* Foreign Currency #5, 2172 */
#define MDC_FC6_ADR          340     /* Foreign Currency #6, 2172 */
#define MDC_FC7_ADR          341     /* Foreign Currency #7, 2172 */
#define MDC_FC8_ADR          342     /* Foreign Currency #8, 2172 */

#define MDC_RPTFIN32_ADR     343     /* Financial Report       2172 */
#define MDC_RPTFIN33_ADR     344     /* Financial Report       2172 */

#define MDC_RPTCAS27_ADR     345     /* Cashier Report 2172 */
#define MDC_RPTCAS28_ADR     346     /* Cashier Report 2172 */
#define MDC_RPTCAS29_ADR     347     /* Cashier Report 2172 */
#define MDC_RPTCAS30_ADR     348     /* Cashier Report 2172 */
#define MDC_RPTCAS31_ADR     349     /* Cashier Report 2172 */
#define MDC_RPTCAS32_ADR     350     /* Cashier Report 2172 */
#define MDC_RPTCAS33_ADR     351     /* Cashier Report 2172 */
#define MDC_RPTCAS34_ADR     352     /* Cashier Report 2172 */

#define MDC_FS1_ADR          353     /* Food stamp #1 */
#define MDC_FS2_ADR          354     /* Food stamp #2 */

#define MDC_WIC1_ADR         355     /* WIC */
#define MDC_WIC2_ADR         356     /* WIC */
#define MDC_RPTFIN34_ADR     357     /* Financial Report */
#define MDC_RPTFIN35_ADR     358     /* Financial Report */

#define MDC_ENHEPT_ADR       361     /* Enhanced EPT 2172 */
#define	MDC_GIFT_MASK		 362	 /* Gift Card Printing & Masking */

#define MDC_DSP_ON_FLY3      363     /* display on the fly, 2172 */
#define MDC_CLEAR_ON_TENDER  363     // Clear display on tender (bit C)
#define MDC_ADJ_ADR          364     /* adjective plu enhancement, 2172 */

#define MDC_SYSTEM1_ADR      365     /* system mdc, 2172 - Number of terminals */
#define MDC_SYSTEM2_ADR      366     /* system mdc, 2172 */
#define MDC_SYSTEM3_ADR      367     /* system mdc, 2172 - Decimal point */
#define MDC_SYSTEM4_ADR      368     /* system mdc, 2172 - Backup System */

#define MDC_ROUND1_ADR       369     /* Rounding #1    2172 */
#define MDC_ROUND2_ADR       370     /* Rounding #2    2172 */

#define MDC_UPC1_ADR         371     /* UPC Coupon #1    2172 */
#define MDC_UPC2_ADR         372     /* UPC Coupon #2    2172 */
#define MDC_UPC3_ADR         373     /* UPC Coupon #3    2172 */
#define MDC_UPC4_ADR         374     /* UPC Coupon #4    2172 */
#define MDC_UPC5_ADR         375     /* UPC Coupon #5    2172 */
#define MDC_UPC6_ADR         376     /* UPC Coupon #6    2172 */

#define MDC_EPT_MASK1_ADR	 377	//SR 131 Tender Digit Masking for CP/EPT (Account print On/Off Masking On/Off)
#define MDC_EPT_MASK2_ADR	 378	//SR 131 Tender Digit Masking for CP/EPT (Digit masking length settings)
#define MDC_CHK_NO_PURCH	 379	//SR 18  Check No Purchase Functionality
#define MDC_DOUBLE1_ADR		 380	//SR 47  Double/Triple Coupon Functionality
#define MDC_DOUBLE2_ADR		 381	//SR 47  Double/Triple Coupon Functionality
#define MDC_TRIPLE1_ADR		 382	//SR 47	 Double/Triple Coupon Functionality
#define MDC_TRIPLE2_ADR		 383	//SR 47  Double/Triple Coupon Functionality
#define MDC_HALOOVR_ADR		 384	//SR 143 Halo Override Key cwunn

#define MDC_TEND1201_ADR	 385   //Tender #12, MDC page 1
#define MDC_TEND1202_ADR	 386   //Tender #12, MDC page 2
#define MDC_TEND1301_ADR	 387   //Tender #13, MDC page 1
#define MDC_TEND1302_ADR	 388   //Tender #13, MDC page 2
#define MDC_TEND1401_ADR	 389   //Tender #14, MDC page 1
#define MDC_TEND1402_ADR	 390   //Tender #14, MDC page 2
#define MDC_TEND1501_ADR	 391   //Tender #15, MDC page 1
#define MDC_TEND1502_ADR	 392   //Tender #15, MDC page 2
#define MDC_TEND1601_ADR	 393   //Tender #16, MDC page 1
#define MDC_TEND1602_ADR	 394   //Tender #16, MDC page 2
#define MDC_TEND1701_ADR	 395   //Tender #17, MDC page 1
#define MDC_TEND1702_ADR	 396   //Tender #17, MDC page 2
#define MDC_TEND1801_ADR	 397   //Tender #18, MDC page 1
#define MDC_TEND1802_ADR	 398   //Tender #18, MDC page 2
#define MDC_TEND1901_ADR	 399   //Tender #19, MDC page 1
#define MDC_TEND1902_ADR	 400   //Tender #19, MDC page 2
#define MDC_TEND2001_ADR	 401   //Tender #20, MDC page 1
#define MDC_TEND2002_ADR	 402   //Tender #20, MDC page 2

#define MDC_CPTEND12_ADR	 403   //Tender #12 CP Address Tender #12, MDC page 3
#define MDC_CPTEND13_ADR	 404   //Tender #13 CP Address Tender #13, MDC page 3
#define MDC_CPTEND14_ADR	 405   //Tender #14 CP Address Tender #14, MDC page 3
#define MDC_CPTEND15_ADR	 406   //Tender #15 CP Address Tender #15, MDC page 3
#define MDC_CPTEND16_ADR	 407   //Tender #16 CP Address Tender #16, MDC page 3
#define MDC_CPTEND17_ADR	 408   //Tender #17 CP Address Tender #17, MDC page 3
#define MDC_CPTEND18_ADR	 409   //Tender #18 CP Address Tender #18, MDC page 3
#define MDC_CPTEND19_ADR	 410   //Tender #19 CP Address Tender #19, MDC page 3
#define MDC_CPTEND20_ADR	 411   //Tender #20 CP Address Tender #20, MDC page 3

#define MDC_TEND1203_ADR	 412   //Tender #12, MDC page 4
#define MDC_TEND1204_ADR	 413   //Tender #12, MDC page 5
#define MDC_TEND1303_ADR	 414   //Tender #13, MDC page 4
#define MDC_TEND1304_ADR	 415   //Tender #13, MDC page 5
#define MDC_TEND1403_ADR	 416   //Tender #14, MDC page 4
#define MDC_TEND1404_ADR	 417   //Tender #14, MDC page 5
#define MDC_TEND1503_ADR	 418   //Tender #15, MDC page 4
#define MDC_TEND1504_ADR	 419   //Tender #15, MDC page 5
#define MDC_TEND1603_ADR	 420   //Tender #16, MDC page 4
#define MDC_TEND1604_ADR	 421   //Tender #16, MDC page 5
#define MDC_TEND1703_ADR	 422   //Tender #17, MDC page 4
#define MDC_TEND1704_ADR	 423   //Tender #17, MDC page 5
#define MDC_TEND1803_ADR	 424   //Tender #18, MDC page 4
#define MDC_TEND1804_ADR	 425   //Tender #18, MDC page 5
#define MDC_TEND1903_ADR	 426   //Tender #19, MDC page 4
#define MDC_TEND1904_ADR	 427   //Tender #19, MDC page 5
#define MDC_TEND2003_ADR	 428   //Tender #20, MDC page 4
#define MDC_TEND2004_ADR	 429   //Tender #20, MDC page 5
#define MDC_RPTFIN36_ADR	 430   //Financial Report (added for new tenders)
#define MDC_RPTFIN37_ADR     431   //Financial Report (added for new tenders)
#define MDC_RPTCAS35_ADR	 432   //Operator Report (added for new tenders)
#define MDC_RPTCAS36_ADR	 433   //Operator Report (added for new tenders)
#define MDC_RPTFIN38_ADR     434   //Financial Report (added for new totals)
#define MDC_RPTFIN39_ADR     435   //Financial Report (added for new totals)
#define MDC_RPTCAS37_ADR     436   //Operator Report (added for new totals)
#define MDC_RPTCAS38_ADR     437   //Operator Report (added for new totals)
#define MDC_RPTCAS39_ADR     438   //Operator Report (added for new totals)
#define MDC_RPTCAS40_ADR     439   //Operator Report (added for new totals)
#define MDC_RPTFIN40_ADR     440   //Financial Report (added for new Tenders)
#define MDC_RPTFIN41_ADR	 441   //Financial Report (added for new Tenders)
#define MDC_PREAUTH_MASK_ADR 442   //Digit Masking for EPT/Pre-Auth (Digit masking length settings)
#define	MDC_EPT_MASK3_ADR	 443   //Tender Digit Masking for CP/EPT (Digit masking length settings) 11-20
#define MDC_PREAUTH_PADR	 444   //Pre-Authorization Entry Control
#define MDC_AUTOCOUP_ADR	 445   //Auto Combinational Coupon
#define MDC_RPTCAS41_ADR     446     /* Operator Report */
#define MDC_RPTCAS42_ADR     447     /* Operator Report */
#define MDC_RPTCAS43_ADR     448     /* Operator Report */
#define MDC_RPTCAS44_ADR     449     /* Operator Report */
#define MDC_RPTCAS45_ADR	 450	 /* Operator Report */
#define MDC_MODID31_ADR      451     /* Modified Item Discount Key (ID #3) */
#define MDC_MODID32_ADR      452     /* Modified Item Discount Key (ID #3) */
#define MDC_MODID33_ADR      453     /* Modified Item Discount Key (ID #3) */
#define MDC_MODID41_ADR      454     /* Modified Item Discount Key (ID #4) */
#define MDC_MODID42_ADR      455     /* Modified Item Discount Key (ID #4) */
#define MDC_MODID43_ADR      456     /* Modified Item Discount Key (ID #4) */
#define MDC_MODID51_ADR      457     /* Modified Item Discount Key (ID #5) */
#define MDC_MODID52_ADR      458     /* Modified Item Discount Key (ID #5) */
#define MDC_MODID53_ADR      459     /* Modified Item Discount Key (ID #5) */
#define MDC_MODID61_ADR      460     /* Modified Item Discount Key (ID #6) */
#define MDC_MODID62_ADR      461     /* Modified Item Discount Key (ID #6) */
#define MDC_MODID63_ADR      462     /* Modified Item Discount Key (ID #6) */

#define MDC_RPTCAS46_ADR	 463	//Operator Reports (for bonus totals)
#define MDC_RPTCAS47_ADR	 464	//Operator Reports (for bonus totals)
#define MDC_RPTCAS48_ADR	 465	//Operator Reports (for bonus totals)
#define MDC_RPTCAS49_ADR	 466	//Operator Reports (for item discount #3, #4, #5, #6)
#define MDC_RPTFIN42_ADR	 467	//Financial Reports (for bonus totals)
#define MDC_RPTFIN43_ADR	 468	//Financial Reports (for bonus totals)
#define MDC_RPTFIN44_ADR	 469	//Financial Reports (for bonus totals)
#define MDC_RPTFIN45_ADR	 470	//Financial Reports (for item discount #3, #4, #5, #6)

#define MDC_HOUR5_ADR		 471	//Hourly Reports (for bonus totals)
#define MDC_HOUR6_ADR		 472	//Hourly Reports (for bonus totals)
#define MDC_HOUR7_ADR		 473	//Hourly Reports (for bonus totals)
#define MDC_HOUR8_ADR		 474	//Hourly Reports (for bonus totals)
#define MDC_RCT_PRT_ADR		 475    // Receipt print for customer and merchant copies
#define MDC_AUTO_SIGNOUT_ADR 477	//Auto sign out
//		MDC_AUTO_SIGNOUTADR2 478	//478 is being used with 477 becuase the MDC uses a whole byte, next MDC is 479
#define MDC_EMPLOYEE_ID_ADR  479    //MDC settings used with the new employee id settings for Amtrak
#define MDC_DRAWEROPEN_TIME  481	//number of seconds before issuing drawer open sound and error dialog
//		MDC_DRAWEROPEN_TIME  482	//482 is being used with 481 becuase the MDC uses a whole byte, next MDC is 483

#define MDC_TEND15_ADR        483     /* Tender #1, MDC page 6            */
#define MDC_TEND25_ADR        484     /* Tender #2, MDC page 6            */
#define MDC_TEND35_ADR        485     /* Tender #3, MDC page 6            */
#define MDC_TEND45_ADR        486     /* Tender #4, MDC page 6            */
#define MDC_TEND55_ADR        487     /* Tender #5, MDC page 6            */
#define MDC_TEND65_ADR        488     /* Tender #6, MDC page 6            */
#define MDC_TEND75_ADR        489     /* Tender #7, MDC page 6            */
#define MDC_TEND85_ADR        490     /* Tender #8, MDC page 6            */
#define MDC_TEND95_ADR        491     /* Tender #9, MDC page 6            */
#define MDC_TEND105_ADR       492     /* Tender #10, MDC page 6           */
#define MDC_TEND115_ADR       493     /* Tender #11, MDC page 6           */
#define MDC_TEND1205_ADR	  494     //Tender #12, MDC page 6
#define MDC_TEND1305_ADR	  495     //Tender #13, MDC page 6
#define MDC_TEND1405_ADR	  496     //Tender #14, MDC page 6
#define MDC_TEND1505_ADR	  497     //Tender #15, MDC page 6
#define MDC_TEND1605_ADR	  498     //Tender #16, MDC page 6
#define MDC_TEND1705_ADR	  499     //Tender #17, MDC page 6
#define MDC_TEND1805_ADR	  500     //Tender #18, MDC page 6
#define MDC_TEND1905_ADR	  501     //Tender #19, MDC page 6
#define MDC_TEND2005_ADR	  502     //Tender #20, MDC page 6

#define MDC_MODID15_ADR       503     /* Modified Item Discount Key (ID #1), page 5 */
#define MDC_MODID25_ADR       504     /* Modified Item Discount Key (ID #2), page 5 */
#define MDC_MODID35_ADR       505     /* Modified Item Discount Key (ID #3), page 5 */
#define MDC_MODID45_ADR       506     /* Modified Item Discount Key (ID #4), page 5 */
#define MDC_MODID55_ADR       507     /* Modified Item Discount Key (ID #5), page 5 */
#define MDC_MODID65_ADR       508     /* Modified Item Discount Key (ID #6), page 5 */

#define MDC_RDISC15_ADR       509     /* Regular Discount #1 Key, page 5   */
#define MDC_RDISC25_ADR       510     /* Regular Discount #2 Key, page 5   */
#define MDC_RDISC35_ADR       511     /* Regular Discount #3 Key, page 5   */
#define MDC_RDISC45_ADR       512     /* Regular Discount #4 Key, page 5   */
#define MDC_RDISC55_ADR       513     /* Regular Discount #5 Key, page 5   */
#define MDC_RDISC65_ADR       514     /* Regular Discount #6 Key, page 5   */
#define MDC_PAUSE_1_2_ADR     515     /* Numeric Pause Key #1 and #2 settings, LDT_PAUSEMSG1_AD and LDT_PAUSEMSG2_AD */
#define MDC_PAUSE_3_4_ADR     516     /* Numeric Pause Key #3 and #4 settings, LDT_PAUSEMSG3_AD and LDT_PAUSEMSG4_AD */
#define MDC_RETURNS_RTN1_ADR  517     /* Returns key (option 1 of void), Amtrak/VCS */
#define MDC_RETURNS_RTN2_ADR  518     /* Returns key (option 2 of void), Amtrak/VCS */
#define MDC_RETURNS_RTN3_ADR  519     /* Returns key (option 3 of void), Amtrak/VCS */
#define MDC_RPTFIN46_ADR      520     /* Financial Reports for returns, Amtrak/VCS */
#define MDC_RPTCAS50_ADR      521     /* Operator Reports for returns, Amtrak/VCS */
#define MDC_RECEIPT_RTN_ADR   522     /* Governs receipt changes needed for Returns, Amtrak/VCS */
#define MDC_SF_ALERT_TIME     523	//number of minutes to wait before alerting that Store and Forward is turned on
//		MDC_SF_ALERT_TIME     524	//524 is being used with 523 becuase the MDC uses a whole byte, next MDC is 525
#define MDC_SF_AUTOOFF_TIME   525	//number of minutes to wait before automatically turning Store and Forward off
//		MDC_SF_AUTOOFF_TIME   526	//526 is being used with 525 becuase the MDC uses a whole byte, next MDC is 527
#define MDC_SF_TRANRULE_TIME  527	//number of seconds to wait before implementing special Store and Forward transaction rules
//		MDC_SF_TRANRULE_TIME  528	//528 is being used with 527 becuase the MDC uses a whole byte, next MDC is 529
#define MDC_SIGCAP_USE_RULES  529   // governs use of Signature Capture for various operations such as returns
#define MDC_RTN_RECALL_RULES  530   // governs use of Returns and legal operator actions with regard to returns
#define MDC_SF_CNTRL_ALERT    531   // contains controls string id for when MDC_SF_ALERT_TIME timer expires
//		MDC_SF_CNTRL_ALERT    532	//532 is being used with 531 because the MDC uses a whole byte, next MDC is 533
#define MDC_SF_CNTRL_AUTOOFF  533   // contains controls string id for when MDC_SF_AUTOOFF_TIME timer expires
//		MDC_SF_CNTRL_AUTOOFF  534	//534 is being used with 533 because the MDC uses a whole byte, next MDC is 535
#define MDC_SF_EXEC_RULES_01  535   // Rules for additional Store and Forward functionality governing execution and processing
#define MDC_RECEIPT_POST_LMT  536   // nibble with the count for the max number of post receipt prints allowed
#define MDC_RECEIPT_GIFT_LMT  537   // nibble with the count for the max number of gift receipt prints allowed
#define MDC_RECEIPT_POD_LMT   538   // nibble with the count for the max number of Print on Demand reprint receipt prints allowed

#define MDC_POWERSTATE_INDIC  600   // used in Powerdown. See UifPowerMonitor() and ItemOtherPowerDownLog(). Replaces use of MDC_ADR_MAX.

/* Warning !!! You have to change this Define When Above Max. address is changed */
#define MDC_ADR_MAX         MAX_MDC_SIZE    /* Max Address */

// MDC defines that use the same area but a different bit for different things
#define MDC_PRINT_ITEM_MNEMO    MDC_DEPT2_ADR     // indicates whether to print item mnemonic or not
#define MDC_DBLSIDEPRINT_ADR	MDC_COMBCOUPON_ADR     /* Turn double side printing on and off        R3.0   */

/*------------------------------------------------------------------------*\
*   Define - Flexible Memory Assignment Address (PRG #2)

	see ParaFlexMem in UNINIRAM UNINT Para.
\*------------------------------------------------------------------------*/
// following are flex memory addresses for data in memory resident Para UNINIRAM struct
#define FLEX_DEPT_ADR           1    /* Number of Department */
#define FLEX_PLU_ADR            2    /* Number of PLU */
#define FLEX_WT_ADR             3    /* Number of Waiter. NOTE: Waiter functionality doe not exist in NHPOS 1.4 or later (GenPOS 2.2)  */
#define FLEX_CAS_ADR            4    /* Number of Cashier */
#define FLEX_EJ_ADR             5    /* Size of EJ File */
#define FLEX_ETK_ADR            6    /* Number of ETK File */
#define FLEX_ITEMSTORAGE_ADR    7    /* Max Number of Item per guest check for Item Storage */
#define FLEX_CONSSTORAGE_ADR    8    /* Max Number of Item per guest check for Cons. Storage */
#define FLEX_GC_ADR             9    /* System Type and Max Number of GuestCheck */
#define FLEX_CPN_ADR           10    /* Number of Coupon */
#define FLEX_CSTRING_ADR       11    /* Control String Size */
#define FLEX_PROGRPT_ADR       12    /* Programable Report Size */
#define FLEX_PPI_ADR           13    /* PPI Size */
// following are for flex memory addresses for data in files stored in file system
#define FLEX_MLD_ADR           14    // Multi-Line Display mnemonics file
#define FLEX_RSN_ADR           15    // Reason codes mnemonics file

/* Warning !!! You have to change this Define When Above Max. address is changed */
/* except MLD Mnemonics file, because hiden data at Prog.#2, V3.3 */
#define FLEX_ADR_MAX     FLEX_PPI_ADR

// These system types are also used, left shifted, in WorkCur.flPrintStatus. see CURQUAL_GC_SYS_MASK.
// System Type seems to be a way of supporting the various types of NCR 2172 system behaviors.
// NHPOS, predecessor to GenPOS, combined several different NCR 2172 system types into one all encompassing application.
//   Precheck has to do with those sites where a paper check with the transaction details was kept by the waiter.
//     Unbuffered is immediately printed as items are entered, Buffered items are printed after all items are entered.
//   Post Check has Hospitality and Table Service
//   Store Recall has Quick Service and Drive Thru functionality.
//
// WARNING: These values should not be changed due backward compatibility with PEP and CWS utilities.
//          There is also a dependency on Transaction Current Qualifier TranCurQual->flPrintStatus
//          values CURQUAL_PRE_BUFFER, CURQUAL_PRE_UNBUFFER, CURQUAL_POSTCHECK, and CURQUAL_STORE_RECALL
#define FLEX_PRECHK_BUFFER      0    /* pre-check buffering print, transaction details are NOT stored, CURQUAL_PRE_BUFFER */
#define FLEX_PRECHK_UNBUFFER    1    /* pre-check unbuffering print, transaction details are NOT stored, CURQUAL_PRE_UNBUFFER */
#define FLEX_POST_CHK           2    /* post check buffering print system, CURQUAL_POSTCHECK */
#define FLEX_STORE_RECALL       3    /* store/ recall buffering print system, CURQUAL_STORE_RECALL */

/*---- Define the number of Record ----*/

#define FLEX_DEPT_MAX         250       /* Max Dept Number Size, 2172 */
#define FLEX_PLU_MAX       500000L      /* Max PLU Number Size, 2172 */
#define FLEX_WT_MAX           100
#define FLEX_CAS_MAX          300       /* Max Cashier/Operator Size, V3.3 */
#define FLEX_ETK_MAX          250
#define FLEX_GC_MAX          5000	    //800 JHHJ Guestcheck Change 800->5000 3-15-04
#define FLEX_EJ_MAX          12000      // Max Electronic Journal Size
#define FLEX_ITEMSTORAGE_MAX  200       // Max Items per Guest Check Size
#define FLEX_CPN_MAX          300		// 100 to 300 Max Coupon size,  JHHJ 3-1504
#define FLEX_CSTRING_MAX     8000	    //ESMITH CSTR Max Control String size. See also FSC_STRING_MAX for max count of strings.
#define FLEX_PROGRPT_MAX       64       // Max Programmable Report Size
#define FLEX_PPI_MAX          300       //ESMITH

#define FLEX_MLD_MAX          220       /* saratoga */

#define FLEX_EJ_MIN            10       /* Minimum Number of Block for E.J File */
#define FLEX_ITEMSTORAGE_MIN   30
#define FLEX_TMP_FILE_ID		254

#define FILE_LEN_ID		10 /* 10-1-2004 JHHJ for file version information*/

/*------------------------------------------------------------------------*\
*   Define - FSC PAGE (PRG #3)
*   Define - FSC for Regular Keyboard Address (PRG #3)
*            These defined constants are for the old NCR 7448 terminal
*            which had two types of keyboards, a Micromotion keyboard with
*            flexible membrane switches and a Conventional keyboard that
*            looked like a standard 102 PC keyboard though with fewer keys.
*            The original Function Selection Code tables were designed to
*            provide a translation table between key press scan codes and the
*            actual FSCs used internally to represent key presses as functional
*            codes.
*
*            See example conversion tables such as:
*                CVTTBL FARCONST CvtProg5932_78[] - NCR 5932 Wedge 78 key keyboard
*                CVTTBL FARCONST CvtProgTouch[]   - Touchscreen as provisioned with Layout Manager
*
*            See also dependency on UNINIRAM struct element PLUNO ParaPLUNoMenu[MAX_PLUKEY_SIZE];
\*------------------------------------------------------------------------*/

#define FSC_MIN_PAGE          1     /* Menu Page 1 */
#define FSC_MAX_PAGE          9     /* Menu Page 9 was max number of menu pages before Touchscreen. DO NOT CHANGE. Not same as MAX_FSC_TBL */

#define FSC_MIN_ADR           1     /* Menu Page Address 1 */
#define FSC_MAX_ADR         165     /* Menu Page Address 165 was max FSC table address before Touchscreen DO NOT CHANGE. Not same as MAX_FSC_NO */

#define FSC_MICRO_ADR       165     /* NCR 7448 Micromotion Keyboard Max menu page Address */
#define FSC_CONV_ADR         68     /* NCR 7448 Conventional Keyboard Max menu page Address */


/*------------------------------------------------------------------------*\
*   Define - Major FSC Max Data (PRG #3)
\*------------------------------------------------------------------------*/

#define FSC_MAJOR_MAX        86     /* Max Number of Major FSC Data, NCR 2172 and NCR 7448 */

/*------------------------------------------------------------------------*\
*   Define - Minor or Extended FSC Max Data (PRG #3) as well as layout file
*            button assignment data specified with Layout Manager utility.
*
*   These are data input limits used to check the extended FSC that is used
*   with the FSC of a key press to determine the functionality of the key.
*
*   Many extended FSC values are used as an index into a member of the Para UNINIRAM
*   memory resident database and the extended FSC ranges need to agree with the sizes
*   of those variables.
*
*   For instance the number of tender keys must correspond to (this list may not be complete):
*    - number of entries for P62, UCHAR   ParaTend[MAX_TEND_NO]
*    - number of entries for P98, TENDERKEYINFO  TenderKeyInformation[MAX_TEND_NO]
*    - number of MDC addresses devoted to tender key parameters
\*------------------------------------------------------------------------*/

#define FSC_MINOR_MAX        11      /* Minor FSC Data Max */

#define FSC_PLU_MAX         165      /* Keyed PLU for NCR 7448 Micromotion,  R3.1, max number physical keys on Micromotion keyboard. */
#define FSC_PLU_CONVMAX      68      /* Keyed PLU Key for NCR 7448 Conventional Keyboard, max number physical keys on Conventional keyboard. */
#define FSC_DEPT_MAX         50      /* Keyed Department Key, NCR 2172, see also MAX_DEPTKEY_SIZE */
#define FSC_DEPT_CONVMAX     50      /* Keyed PLU Key for Conventional Keyboard, NCR 2172 */

#define FSC_PM_MAX            8      /* Print Modifier Key. see also MAX_PRTMODI_NO */
#define FSC_ADJ_MAX          20      /* Adjective Key. see also MAX_ADJM_NO */
#define FSC_TEND_MAX         STD_TENDER_MAX      /* Tender Key */
#define FSC_FC_MAX           STD_NO_FOREIGN_TTL  /* Foreign Currency Key, used with FSC_FOREIGN NCR 2172 */
#define FSC_TOTAL_MAX        STD_TOTAL_MAX       /* Total Key */
#define FSC_ADDCHK_MAX        3      /* Add Check Key, STD_MAX_SERVICE_TOTAL */
#define FSC_IDISC_MAX         6      /* Item Discount Key, used with FSC_ITEM_DISC */
#define FSC_RDISC_MAX         6      /* Regular Discount Key, used with FSC_REGULAR_DISC    R3.1 */
#define FSC_TAXMD_MAX        11      /* Tax Modifier Key, used with FSC_TAX_MODIF */
#define FSC_PREAMT_MAX        8      /* Preset Cash Amount Key, see Para size of PRESET_AMT_ADR_MAX and MAX_PRESETAMOUNT_SIZE. See MDC 82 Bit A, MDC_DRAWER2_ADR */
#define FSC_DMSFT_MAX         9      /* Direct Menu Shift Key,    R3.1 */
#define FSC_STRING_MAX      400      /* String Key, used with FSC_KEYED_STRING */
#define FSC_SEAT_MAX          2      /* Seat No. Key, used with FSC_SEAT     R3.1 */
#define FSC_CHGTIP_MAX        3      /* Charge Tips Key,          V3.3 */
#define FSC_ASK_MAX          20      /* Ask Key, 2172 */
#define FSC_ADJSFT_MAX        5      /* Adjective Shift Key, 2172 */
#define FSC_ORDERDEC_MAX	  5		 /* Order Declaration Max JHHJ */

#define FSC_MODIFIER_DISCOUNT   2                   /* modifiered discount special item discount, used with FSC_ITEM_DISC */
#define FSC_PRTDMD_MAX          5                   /* Max extended FSC for Print on Demand Key, FSC_PRT_DEMAND */
#define FSC_PRINTDEMAND         1                   /* print on demand, used with FSC_PRT_DEMAND   R3.0 */
#define FSC_POSTRECEIPT         2                   /* Post Receipt Key, used with FSC_PRT_DEMAND R3.0 */
#define FSC_PARKING             3                   /* Parking Key, used with FSC_PRT_DEMAND      R3.0 */
#define FSC_GIFT_RECEIPT		4					/* Gift Receipt, used with FSC_PRT_DEMAND JHHJ   */
#define FSC_PODREPRINT			5					/* Print on Demand reprint, used with FSC_PRT_DEMAND  */

/*------------------------------------------------------------------------*\
*   Minor FSC for Power Down,                               Saratoga
\*------------------------------------------------------------------------*/
#define     FSC_MINOR_POWER_DOWN    0x01    /* AC Power Down */
#define     FSC_MINOR_SUSPEND       0x02    /* Suspend/Resume */

/*------------------------------------------------------------------------*\
*   Define - Security Number for Program Mode Address (PRG #6)
\*------------------------------------------------------------------------*/

#define SEC_NO_ADR              1    /* Secret Number */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define SEC_ADR_MAX             1    /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Store/Register No. Address (PRG #7)
\*------------------------------------------------------------------------*/

#define SREG_NO_ADR             1    /* Store/Register Number */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define SREG_NO_ADR_MAX         1    /* Max Address */


/*------------------------------------------------------------------------*\
*   Define - Supervisor Number/Level Assignment Address (PRG #8)

	see ParaSupLevel in UNINIRAM UNINT Para
\*------------------------------------------------------------------------*/

#define SPNO_LEV0_ADR           1    /* The Highest Supervisor # for Level 0 */
#define SPNO_LEV1_ADR           2    /* The Highest Supervisor # for Level 1 */
#define SPNO_SUP1_ADR           3    /* Supervisor # 1 */
#define SPNO_SUP2_ADR           4    /* Supervisor # 2 */
#define SPNO_SUP3_ADR           5    /* Supervisor # 3 */
#define SPNO_SUP4_ADR           6    /* Supervisor # 4 */
#define SPNO_SUP5_ADR           7    /* Supervisor # 5 */
#define SPNO_SUP6_ADR           8    /* Supervisor # 6 */

#define SPNO_ADR_MAX       MAX_SUPLEVEL_SIZE



/*------------------------------------------------------------------------*\
*   Define - Action Code Security Address (PRG #9)
*   each security mask is a single 4 bit nibble value and these values are stored
*   in the memory resident data base as odd address, even address pairs. So this
*   means that address 1 and address 2 are both stored in Para.ParaActCodeSec[0].
\*------------------------------------------------------------------------*/

#define ACSC_GCNO_ADR           1    /* Guest Check No. Assignment */
#define ACSC_GCFREAD_ADR        2    /* Guest Check File Read Report */
#define ACSC_GCFRST_ADR         3    /* Guest Check File Reset Report */
#define ACSC_PLUNO_ADR          4    /* Set PLU No. on Menu Page */
#define ACSC_CTLTBL_ADR         5    /* Set Control Table of Menu Page */
#define ACSC_ALTKTN_ADR         6    /* Manual Alternative Kitchen Printer Assignment */
#define ACSC_CASKEY_ADR         7    /* Cashier A/B Keys Assignment */
#define ACSC_RANGE_ADR          9    /* Assignment of Range of Guest Check No. for Waiter */
#define ACSC_LOAN_ADR          10    /* Loan, 2172 */
#define ACSC_PICKUP_ADR        11    /* Pick-Up, 2172 */
#define ACSC_SECRET_ADR        15    /* Supervisor Secret Number Change */
#define ACSC_TOD_ADR           17    /* Set Date & Time */
#define ACSC_PTDRST_ADR        18    /* PTD Reset Report */
#define ACSC_CASRST_ADR        19    /* Cashier Reset Report */
#define ACSC_ASNCAS_ADR        20    /* Cashier No. Assignment */
#define ACSC_CASREAD_ADR       21    /* Cashier Read Report */
#define ACSC_FINANC_ADR        23    /* Terminal Financial Report */
#define ACSC_HOUR_ADR          24    /* Hourly Activity Time */
#define ACSC_DEPTSAL_ADR       26    /* Department Sales Report */
#define ACSC_PLUSAL_ADR        29    /* PLU Sales Read Report */
#define ACSC_PLURESET_ADR      39    /* PLU AllReset Report */
#define ACSC_WFREAD_ADR        41    /* Waiter File Read Report */
#define ACSC_CPYTTL_ADR        42    /* Copy Total Files */
#define ACSC_WFREST_ADR        46    /* Waiter File Reset Report */
#define ACSC_ASNWT_ADR         50    /* Waiter No. Assignment */
#define ACSC_PLUMAINT_ADR      63    /* PLU Maintenance */
#define ACSC_PLUPCHG_ADR       64    /* PLU Price Change */
#define ACSC_DWNPLU_ADR        66    /* Broadcast/Request to Down Load PLU Parameter File */
#define ACSC_PLUADD_ADR        68    /* PLU Addition/Deletion */
#define ACSC_XEJ_ADR           70    /* Read EJ Report */
#define ACSC_PLUPARA_ADR       72    /* PLU Parameter File Repot */
#define ACSC_DWNALL_ADR        75    /* Broadcast/Request to Down Load All Parameter File */
#define ACSC_PLUMCHG_ADR       82    /* PLU Mnemonic Change */
#define ACSC_ROUND_ADR         84    /* Set Rounding Table */
#define ACSC_RATE_ADR          86    /* Set Discount/Bonus(Commision) % Rates */
#define ACSC_CHAR44_ADR        87    /* Set Trailer Print Message for Soft Check */
#define ACSC_PROMO_ADR         88    /* Set Sales Promotion Message */
#define ACSC_CCONV_ADR         89    /* Set Currency Conversion Rate */
#define ACSC_EOD_ADR           99    /* End of Day Reset Report */
#define ACSC_ZEJ_ADR          100    /* Reset EJ Report */
#define ACSC_EMERGENTGC_ADR   103    /* Emergent Guest Check File Close */
#define ACSC_TARE_TABLE       111    /* Set Tare Table */
#define ACSC_DEPT_ADDDEL_ADR  113    /* Department Add/Delete, 2172 */
#define ACSC_DEPT_ADR         114    /* Department Maintenance/Report */
#define ACSC_DEPTNO_ADR       115    /* Set DEPT No. on Menu Page, 2172 */
#define ACSC_SETMENU_ADR      116    /* Assign Set Menu */
#define ACSC_EMERGENTCAS_ADR  119    /* Emergent Cashier File Close */
#define ACSC_MDEPT_ADR        122    /* Major Department Report */
#define ACSC_MFLSH_ADR        123    /* Madia Flash Report */
#define ACSC_TXTBL1_ADR       124    /* Tax Table #1 Programming */
#define ACSC_TXTBL2_ADR       125    /* Tax Table #2 Programming */
#define ACSC_TXTBL3_ADR       126    /* Tax Table #3 Programming */
#define ACSC_TXRATE_ADR       127    /* Set Tax/Service/Reciprocal Rates */
#define ACSC_FSPARA_ADR       128    /* Set Food Stamp Parameter */
#define ACSC_TXTBL4_ADR       129    /* Tax Table #4 Programming */
#define ACSC_PIGRULE_ADR      130    /* Pig Rule Table */
#define ACSC_EMERGENTWAI_ADR  146    /* Emergent Waiter File Close */
#define ACSC_ETKRPT_ADR       150    /* Employee Read Report */
#define ACSC_ETKRST_ADR       151    /* Employee Reset Report */
#define ACSC_ETKMNT_ADR       152    /* Employee Mantenace */
#define ACSC_ETKFLM_ADR       153    /* ETK File Maintenance */
#define ACSC_SRTBL_ADR        170    /* Set Sales Restriction Table, 2172 */
#define ACSC_DWNSUP_ADR       175    /* Broadcast/Request to Down Load Super Parameter File */
#define ACSC_CSOPN_RPT        176    /* Open/Guest Check Status Read Report */
#define ACSC_BOUND_ADR        208    /* Set Boundary Age, 2172 */

#define ACSC_TBL_END          0xFFFF   /* Table End */

/* Warning !!! You have to change this Define When Above Max. address, MAX_ACSEC_SIZE, is changed */

#define ACSC_ADR_MAX          233    /* Max Address, values are stored as two nibbles with odd/even address in single UCHAR */

/*------------------------------------------------------------------------*\
*   Define - Transaction HALO Address (PRG # 10) - CLASS_PARATRANSHALO
*
*   Use function RflGetHaloData() to retrieve the HALO value.
*   See function RflHALO() for how the HALO amount is calculated.
*   See MAX_HALOKEY_NO for the maximum index possible.
*   See ParaTransHALO[] which is part of UNINIRAM memory resident database.
\*------------------------------------------------------------------------*/

#define HALO_TEND1_ADR          1    /* Tender #1.  Use RflGetHaloData() */
#define HALO_TEND2_ADR          2    /* Tender #2 */
#define HALO_TEND3_ADR          3    /* Tender #3 */
#define HALO_TEND4_ADR          4    /* Tender #4 */
#define HALO_TEND5_ADR          5    /* Tender #5 */
#define HALO_TEND6_ADR          6    /* Tender #6 */
#define HALO_TEND7_ADR          7    /* Tender #7 */
#define HALO_TEND8_ADR          8    /* Tender #8 */
#define HALO_TIPPAID_ADR        9    /* Tips Paid Out */
#define HALO_PAIDOUT_ADR       10    /* Paid Out */
#define HALO_MOTERID_ADR       11    /* Moterized Item Discount Key */
#define HALO_MODIFID_ADR       12    /* Modified Item Discount Key */
#define HALO_RDISC1_ADR        13    /* Regular Discount #1.  Use RflGetHaloData() */
#define HALO_RDISC2_ADR        14    /* Regular Discount #2 */
#define HALO_VOID_ADR          15    /* Void */
#define HALO_DCTIP_ADR         16    /* Declared Tips */
#define HALO_CHRGTIP_ADR       17    /* Charge Tips */
#define HALO_RECVACNT_ADR      18    /* Received Account */
#define HALO_FOREIGN1_ADR      19    /* Foreign Currency 1.  Use RflGetHaloData() */
#define HALO_FOREIGN2_ADR      20    /* Foreign Currency 2 */
#define HALO_TEND9_ADR         21    /* Tender #9 */
#define HALO_TEND10_ADR        22    /* Tender #10 */
#define HALO_TEND11_ADR        23    /* Tender #11 */
#define HALO_RDISC3_ADR        24    /* Regular Discount #3 */
#define HALO_RDISC4_ADR        25    /* Regular Discount #4 */
#define HALO_RDISC5_ADR        26    /* Regular Discount #5 */
#define HALO_RDISC6_ADR        27    /* Regular Discount #6 */
#define HALO_CHRGTIP2_ADR      28    /* Charge Tips #2, V3.3 */
#define HALO_CHRGTIP3_ADR      29    /* Charge Tips #3, V3.3 */
#define HALO_FOREIGN3_ADR      30    /* Foreign Currency 3, 2172 */
#define HALO_FOREIGN4_ADR      31    /* Foreign Currency 4, 2172 */
#define HALO_FOREIGN5_ADR      32    /* Foreign Currency 5, 2172 */
#define HALO_FOREIGN6_ADR      33    /* Foreign Currency 6, 2172 */
#define HALO_FOREIGN7_ADR      34    /* Foreign Currency 7, 2172 */
#define HALO_FOREIGN8_ADR      35    /* Foreign Currency 8, 2172 */
#define HALO_PLU_DIGIT_ADR     36    /* Max. digit for PLU search at Prog#3, Saratoga */
#define HALO_TEND12_ADR        37    /* Tender #12 */
#define HALO_TEND13_ADR        38    /* Tender #13 */
#define HALO_TEND14_ADR        39    /* Tender #14 */
#define HALO_TEND15_ADR        40    /* Tender #15 */
#define HALO_TEND16_ADR        41    /* Tender #16 */
#define HALO_TEND17_ADR        42    /* Tender #17 */
#define HALO_TEND18_ADR        43    /* Tender #18 */
#define HALO_TEND19_ADR        44    /* Tender #19 */
#define HALO_TEND20_ADR        45    /* Tender #20 */
#define HALO_DISC3_ADR         46    /* Item Discount #3 */
#define HALO_DISC4_ADR         47    /* Item Discount #4 */
#define HALO_DISC5_ADR         48    /* Item Discount #5 */
#define HALO_DISC6_ADR         49    /* Item Discount #6 */
#define HALO_OPT_TENDER_ADR    50    /* Optional tender HALO require Supervisor Internvention governed by MDCs 483-502 bit C */


/*------------------------------------------------------------------------*\
*   Define - Preset Amount Cash Tender. Address (PRG #15)
\*------------------------------------------------------------------------*/

#define PRESET_AMT1_ADR         1    /* Preset Amount 1 */
#define PRESET_AMT2_ADR         2    /* Preset Amount 2 */
#define PRESET_AMT3_ADR         3    /* Preset Amount 3 */
#define PRESET_AMT4_ADR         4    /* Preset Amount 4 */

#define PRESET_AMT_ADR_MAX      4    /* Max Address */

/*------------------------------------------------------------------------*\
*	Define - Reason code addresses for accessing the reason code mnemonics
*            There are two values, the starting mnemonic address and the range
*            for the mnemonic.  See Para.ParaReasonCode
\*------------------------------------------------------------------------*/

#define REASON_CODE_RETURN_1     1    /* address of reason code data for Return #1 */
#define REASON_CODE_RETURN_2     2    /* address of reason code data for Return #2 */
#define REASON_CODE_RETURN_3     3    /* address of reason code data for Return #3 */
#define REASON_CODE_CURSOR_VOID  4    /* address of reason code data for Cursor Void of an item (VOID_MODIFIER) */
#define REASON_CODE_TRAN_VOID    5    /* address of reason code data for Transaction Void of a transaction */
#define REASON_CODE_ITEMSALE     6    /* address of reason code data for ITEMSALE */
#define REASON_CODE_ITEMDISC     7    /* address of reason code data for ITEMDISC */
#define REASON_CODE_TRAN_DISC    8    /* address of reason code data for transaction discount of a transaction */

#define REASON_CODE_ADR_MAX     MAX_REASON_CODE_ITEMS

/*------------------------------------------------------------------------*\
*   Define - Hourly Activity Time Address (PRG # 17)
\*------------------------------------------------------------------------*/

#define HOUR_ACTBLK_ADR         1    /* Number of Activity Blocks */
#define HOUR_TSEG1_ADR          2    /* Time Segment #1 */
#define HOUR_TSEG2_ADR          3    /* Time Segment #2 */
#define HOUR_TSEG3_ADR          4    /* Time Segment #3 */
#define HOUR_TSEG4_ADR          5    /* Time Segment #4 */
#define HOUR_TSEG5_ADR          6    /* Time Segment #5 */
#define HOUR_TSEG6_ADR          7    /* Time Segment #6 */
#define HOUR_TSEG7_ADR          8    /* Time Segment #7 */
#define HOUR_TSEG8_ADR          9    /* Time Segment #8 */
#define HOUR_TSEG9_ADR         10    /* Time Segment #9 */
#define HOUR_TSEG10_ADR        11    /* Time Segment #10 */
#define HOUR_TSEG11_ADR        12    /* Time Segment #11 */
#define HOUR_TSEG12_ADR        13    /* Time Segment #12 */
#define HOUR_TSEG13_ADR        14    /* Time Segment #13 */
#define HOUR_TSEG14_ADR        15    /* Time Segment #14 */
#define HOUR_TSEG15_ADR        16    /* Time Segment #15 */
#define HOUR_TSEG16_ADR        17    /* Time Segment #16 */
#define HOUR_TSEG17_ADR        18    /* Time Segment #17 */
#define HOUR_TSEG18_ADR        19    /* Time Segment #18 */
#define HOUR_TSEG19_ADR        20    /* Time Segment #19 */
#define HOUR_TSEG20_ADR        21    /* Time Segment #20 */
#define HOUR_TSEG21_ADR        22    /* Time Segment #21 */
#define HOUR_TSEG22_ADR        23    /* Time Segment #22 */
#define HOUR_TSEG23_ADR        24    /* Time Segment #23 */
#define HOUR_TSEG24_ADR        25    /* Time Segment #24 */
#define HOUR_TSEG25_ADR        26    /* Time Segment #25 */
#define HOUR_TSEG26_ADR        27    /* Time Segment #26 */
#define HOUR_TSEG27_ADR        28    /* Time Segment #27 */
#define HOUR_TSEG28_ADR        29    /* Time Segment #28 */
#define HOUR_TSEG29_ADR        30    /* Time Segment #29 */
#define HOUR_TSEG30_ADR        31    /* Time Segment #30 */
#define HOUR_TSEG31_ADR        32    /* Time Segment #31 */
#define HOUR_TSEG32_ADR        33    /* Time Segment #32 */
#define HOUR_TSEG33_ADR        34    /* Time Segment #33 */
#define HOUR_TSEG34_ADR        35    /* Time Segment #34 */
#define HOUR_TSEG35_ADR        36    /* Time Segment #35 */
#define HOUR_TSEG36_ADR        37    /* Time Segment #36 */
#define HOUR_TSEG37_ADR        38    /* Time Segment #37 */
#define HOUR_TSEG38_ADR        39    /* Time Segment #38 */
#define HOUR_TSEG39_ADR        40    /* Time Segment #39 */
#define HOUR_TSEG40_ADR        41    /* Time Segment #40 */
#define HOUR_TSEG41_ADR        42    /* Time Segment #41 */
#define HOUR_TSEG42_ADR        43    /* Time Segment #42 */
#define HOUR_TSEG43_ADR        44    /* Time Segment #43 */
#define HOUR_TSEG44_ADR        45    /* Time Segment #44 */
#define HOUR_TSEG45_ADR        46    /* Time Segment #45 */
#define HOUR_TSEG46_ADR        47    /* Time Segment #46 */
#define HOUR_TSEG47_ADR        48    /* Time Segment #47 */
#define HOUR_TSEG48_ADR        49    /* Time Segment #48 */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define HOUR_ADR_MAX           49    /* Max Address, V3.3 */

/*------------------------------------------------------------------------*\
*   Define - Slip Printer Feed Control Address (PRG #18)
\*------------------------------------------------------------------------*/

#define SLIP_MAXLINE_ADR        1    /* Max Line # of Guest Check for Slip Printer */
#define SLIP_NO1ST_ADR          2    /* # of Line Up to 1st Print Line Guest Check for Slip Printer */
#define SLIP_MAXVAL_ADR         3    /* Max Co. of Validation Key (R2.0) SR 47*/
#define SLIP_EJLEN_ADR          4    /* Length of E.J */
#define SLIP_REVJLEN_ADR        5    /* Length of Reverse E.J */
#define SLIP_NEARFULL_ADR       6    /* Persent of Near Full for E.J */


/* Warning !!! You have to change this Define When Above Max. address is changed */
//NOTE: SLIP_ADR_MAX was defined to be the same as MAX_SLIP_SIZE, so we make SLIP_ADR_MAX
//the same as MAX_SLIP_SIZE so that it is less confusing.

#define SLIP_ADR_MAX         MAX_SLIP_SIZE    /* Max Address SR 47*/

#define SLIP_EJLEN_DEF         50    /* Default Length of E.J */
#define SLIP_REVJLEN_DEF       20    /* Default Length of Reserve E.J */


/*------------------------------------------------------------------------*\
*   Define - Transaction Mnemonics Address (PRG #20), P20 Mnemonics
*
*	see ParaTransMnemo in UNINIRAM UNINT Para and CLASS_PARATRANSMNEMO with CliParaAllRead()
\*------------------------------------------------------------------------*/

#define TRN_PFROM_ADR           1        /* Period From */
#define TRN_PTO_ADR             2        /* Period To */
#define TRN_CURGGT_ADR          3        /* Current Gross Group Total */
#define TRN_TXBL1_ADR           4        /* Taxable #1 Total */
#define TRN_TX1_ADR             5        /* Tax #1 Total */
#define TRN_TXEXM1_ADR          6        /* Tax Exempt #1 Total */
#define TRN_TXBL2_ADR           7        /* Taxable #2 Total */
#define TRN_TX2_ADR             8        /* Tax #2 Total */
#define TRN_TXEXM2_ADR          9        /* Tax Exempt #2 Total */
#define TRN_TXBL3_ADR          10        /* Taxable #3 Total */
#define TRN_TX3_ADR            11        /* Tax #3 Total */
#define TRN_TXEXM3_ADR         12        /* Tax Exempt #3 Total */
#define TRN_TXBL4_ADR          13        /* Taxable #4 Total */
#define TRN_TX4_ADR            14        /* Tax #4 Total */
#define TRN_TXEXM4_ADR         15        /* Tax Exempt #4 Total */
#define TRN_PST_ADR            16        /* PST All Tax */
#define TRN_CONSTX_ADR         17        /* Consolidation Tax */
#define TRN_NONTX_ADR          18        /* Non Taxable Total */
#define TRN_TRNGGT_ADR         19        /* Training Gross Group Total */
#define TRN_DECTIP_ADR         20        /* Declared Tips */
#define TRN_DAIGGT_ADR         21        /* Daily Gross Group Total */
#define TRN_PLUSVD_ADR         22        /* Plus Void */
#define TRN_PSELVD_ADR         23        /* Preselect Void */
#define TRN_CONSCPN_ADR        24        /* Consolidated Coupon */
#define TRN_ITMDISC_ADR        25        /* Item Discount */
#define TRN_MODID_ADR          26        /* Modified Item Discount */
#define TRN_RDISC1_ADR         27        /* Regular Discount #1 */
#define TRN_RDISC2_ADR         28        /* Regular Discount #2 */
#define TRN_PO_ADR             29        /* Paid Out */
#define TRN_RA_ADR             30        /* Received On Account */
#define TRN_TIPPO_ADR          31        /* Tips Paid Out */
#define TRN_TEND1_ADR          32        /* Tender #1 */
#define TRN_TEND2_ADR          33        /* Tender #2 */
#define TRN_TEND3_ADR          34        /* Tender #3 */
#define TRN_TEND4_ADR          35        /* Tender #4 */
#define TRN_TEND5_ADR          36        /* Tender #5 */
#define TRN_TEND6_ADR          37        /* Tender #6 */
#define TRN_TEND7_ADR          38        /* Tender #7 */
#define TRN_TEND8_ADR          39        /* Tender #8 */
#define TRN_FT1_ADR            40        /* Foreign Total #1 */
#define TRN_FT2_ADR            41        /* Foreign Total #2 */
#define TRN_TTL1_ADR           42        /* Total #1, hard coded as Sub Total */
#define TRN_TTL2_ADR           43        /* Total #2, hard coded as Check Total */
#define TRN_TTL3_ADR           44        /* Total #3, programmable total */
#define TRN_TTL4_ADR           45        /* Total #4, programmable total */
#define TRN_TTL5_ADR           46        /* Total #5, programmable total */
#define TRN_TTL6_ADR           47        /* Total #6, programmable total */
#define TRN_TTL7_ADR           48        /* Total #7, programmable total */
#define TRN_TTL8_ADR           49        /* Total #8, programmable total */
#define TRN_SERV1_ADR          50        /* Serveice Total #1 */
#define TRN_SERV2_ADR          51        /* Serveice Total #2 */
#define TRN_SERV3_ADR          52        /* Serveice Total #3 */
#define TRN_ADCK1_ADR          53        /* Add Check Total #1 */
#define TRN_ADCK2_ADR          54        /* Add Check Total #2 */
#define TRN_ADCK3_ADR          55        /* Add Check Total #3 */
#define TRN_CHRGTIP_ADR        56        /* Charge Tips */
#define TRN_TRACAN_ADR         57        /* Transaction Cancel */
#define TRN_CANCEL_ADR         58        /* Cancel Total */
#define TRN_MVD_ADR            59        /* Misc Void */
#define TRN_AUD_ADR            60        /* Audaction */
#define TRN_NOSALE_ADR         61        /* No Sale Counter */
#define TRN_ITMPROCO_ADR       62        /* Item Productivity Counter */
#define TRN_PSN_ADR            63        /* Number of Person Counter */
#define TRN_CHKOPN_ADR         64        /* Number of Checks Opened */
#define TRN_CHKCLS_ADR         65        /* Number of Checks Closed */
#define TRN_CUST_ADR           66        /* Customer Counter */
#define TRN_HASHDEPT_ADR       67        /* Hash Department */
#define TRN_BNS1_ADR           68        /* Bonus #1 */
#define TRN_BNS2_ADR           69        /* Bonus #2 */
#define TRN_BNS3_ADR           70        /* Bonus #3 */
#define TRN_BNS4_ADR           71        /* Bonus #4 */
#define TRN_BNS5_ADR           72        /* Bonus #5 */
#define TRN_BNS6_ADR           73        /* Bonus #6 */
#define TRN_BNS7_ADR           74        /* Bonus #7 */
#define TRN_BNS8_ADR           75        /* Bonus #8 */
#define TRN_TTLR_ADR           76        /* Total for Report */
#define TRN_STTLR_ADR          77        /* Sub Total for Report */
#define TRN_OTSTN_ADR          78        /* Outstanding for Report */
#define TRN_ADD_ADR            79        /* Additon */
#define TRN_DEL_ADR            80        /* Deletion */
#define TRN_CHNG_ADR           81        /* Change */
#define TRN_NETSP_ADR          82        /* Net Sales/Person */
#define TRN_ABORT_ADR          83        /* Abort */
#define TRN_AMTTL_ADR          84        /* Amount Total */
#define TRN_DEMAND_ADR         85        /* Print on Demand */
#define TRN_GCNO_ADR           86        /* Guest Check No. */
#define TRN_PB_ADR             87        /* Previous Balance */
#define TRN_CKPD_ADR           88        /* Check Paid */
#define TRN_EC_ADR             89        /* Error Correct */
#define TRN_NUM_ADR            90        /* Number(#) */
#define TRN_ORDNO_ADR          91        /* Order No. Entry */
#define TRN_TXSUM_ADR          92        /* Tax Sum */
#define TRN_CKSUM_ADR          93        /* Check Sum */
#define TRN_GCFFL_ADR          94        /* Guest Check File is Full */
#define TRN_TOPEN_ADR          95        /* Time Opened */
#define TRN_ELPT_ADR           96        /* Elapsed Time */
#define TRN_CURB_ADR           97        /* Current Balance */
#define TRN_TRNSB_ADR          98        /* Transferred Balance */
#define TRN_OLDWT_ADR          99        /* Old Waiter No. */
#define TRN_CKTO_ADR          100        /* Checks Transferred To */
#define TRN_CKFRM_ADR         101        /* Checks Transferred From Others */
#define TRN_WTTL_ADR          102        /* Waiter Total */
#define TRN_CALC_ADR          103        /* Caluculate Tips */
#define TRN_QTY_ADR           104        /* Quantity */
#define TRN_DBLRID_ADR        105        /* Double Receipt ID */
#define TRN_REGCHG_ADR        106        /* Change Amount for Reg. */
#define TRN_MTTLR_ADR         107        /* Minus Total for PLU/DEPT Report */
#define TRN_GTTLR_ADR         108        /* Grand Total for PLU/DEPT Report */
#define TRN_HTTLR_ADR         109        /* Hash Total for PLU/DEPT Report */
#define TRN_MSCALE_ADR        110        /* Manual(Scale Operation) */
#define TRN_TRAYCO_ADR        111        /* Tray Counter */
#define TRN_ETKIN_ADR         112        /* Etk Time in */
#define TRN_ETKOUT_ADR        113        /* Etk Time Out */
#define TRN_POST_ADR          114        /* Post receipt mnemonic if PRT_POST_RECEIPT set */
#define TRN_TEND9_ADR         115        /* Tender #9 */
#define TRN_TEND10_ADR        116        /* Tender #10 */
#define TRN_TEND11_ADR        117        /* Tender #11 */
#define TRN_OFFTEND_ADR       118        /* Off Line Tender #1 */
#define TRN_OFFTNDCHG_ADR     119        /* Off Line Tender for Charge Posting */
#define TRN_OFFTNDAUT_ADR     120        /* Off Line Tender for Auth. */
#define TRN_PREAUTHTTL_ADR    121        /* Pre-Authorization Total  */
#define TRN_COMBCPN_ADR       122        /* Combination Coupon       */
#define TRN_PARKING_ADR       123        /* Parking Receipt          */
#define TRN_PAIDRECALL_ADR    124        /* Paid Order Recall        */
#define TRN_RDISC3_ADR        125        /* Regular Discount #3 */
#define TRN_RDISC4_ADR        126        /* Regular Discount #4 */
#define TRN_RDISC5_ADR        127        /* Regular Discount #5 */
#define TRN_RDISC6_ADR        128        /* Regular Discount #6 */
#define TRN_NETTTL_ADR        129        /* Net Sales Total */
#define TRN_SPLIT_ADR         130        /* Split Key */
#define TRN_LABORTTL_ADR      131        /* Labor Cost Total */
#define TRN_LABORPERCENT_ADR  132        /* Labor Cost % */
#define TRN_CASINT_ADR        133        /* Cashier Interrupt, V3.2 */
#define TRN_CHRGTIP2_ADR      134        /* Charge Tips #2, V3.3 */
#define TRN_CHRGTIP3_ADR      135        /* Charge Tips #3, V3.3 */
#define TRN_WAITING_ADR       136        /* Waiting Message, V3.3 */
#define TRN_CONTINUE_ADR      137        /* Continue Message, for OEP dialog, MldUpdatePopUp() V3.3 */
#define TRN_MNEMONIC_ADR      138        /* Mnemonic, V3.3 */
#define TRN_TAX_RATE_ADR      139        /* Tax Rate, V3.3 */
#define TRN_TAX_ADR           140        /* Tax, V3.3 */
#define TRN_JOB_ADR           141        /* Job, V3.3 */
#define TRN_BLOCK_ADR         142        /* Block, V3.3 */
#define TRN_DATE_ADR          143        /* Date, V3.3 */
#define TRN_FT3_ADR           144        /* Foreign Total #3, 2172 */
#define TRN_FT4_ADR           145        /* Foreign Total #4, 2172 */
#define TRN_FT5_ADR           146        /* Foreign Total #5, 2172 */
#define TRN_FT6_ADR           147        /* Foreign Total #6, 2172 */
#define TRN_FT7_ADR           148        /* Foreign Total #7, 2172 */
#define TRN_FT8_ADR           149        /* Foreign Total #8, 2172 */
#define TRN_LOAN_ADR          150        /* Loan, 2172 */
#define TRN_PICKUP_ADR        151        /* Pick-Up, 2172 */
#define TRN_ONHAND_ADR        152        /* Money On Hand Total, 2172 */
#define TRN_OVRSHRT_ADR       153        /* Over/Short, 2172 */
#define TRN_CONSNO_ADR        154        /* Consumer Number, 2172 */
#define TRN_PLUBLD_ADR        155        /* PLU Building, 2172 */
#define TRN_SKUNO_ADR         156        /* SKU Number, 2172 */
#define TRN_POWER_DOWN_ADR    157        /* Power Down Log, Saratoga */
#define TRN_FSTL_ADR          158        /* Food Stamp Total */
#define TRN_TTL9_ADR          159        /* Total #9, hard coded as Food Stamp Total */
#define TRN_FSCHNG_ADR        160        /* Food Stamp Change */
#define TRN_FSCRD_ADR         161        /* Food Stamp Credit */
#define TRN_FSTXEXM1_ADR      162        /* FS Tax Exempt #1 Total */
#define TRN_FSTXEXM2_ADR      163        /* FS Tax Exempt #2 Total */
#define TRN_FSTXEXM3_ADR      164        /* FS Tax Exempt #3 Total */
#define TRN_VCPN_ADR          165        /* UPC Coupon */
#define TRN_FOR_ADR	          166        /* @/For key (SR 143 cwunn) */
#define TRN_HALO_ADR		  167		 /* HALO Override key (SR 143 cwunn) */
#define TRN_PCKUP_REQ		  168		 /* Cash Drawer Over Limit (SR 155 JHHJ)*/
#define TRN_TENDER12_ADR      169		 /* Tender 12 */
#define TRN_TENDER13_ADR      170		 /* Tender 13 */
#define TRN_TENDER14_ADR      171		 /* Tender 14 */
#define TRN_TENDER15_ADR      172		 /* Tender 15 */
#define TRN_TENDER16_ADR      173		 /* Tender 16 */
#define TRN_TENDER17_ADR      174		 /* Tender 17 */
#define TRN_TENDER18_ADR      175		 /* Tender 18 */
#define TRN_TENDER19_ADR      176		 /* Tender 19 */
#define TRN_TENDER20_ADR      177		 /* Tender 20 */
#define TRN_TOTAL10_ADR		  178		 /* Total 10, programmable total */
#define TRN_TOTAL11_ADR		  179		 /* Total 11, programmable total */
#define TRN_TOTAL12_ADR		  180		 /* Total 12, programmable total */
#define TRN_TOTAL13_ADR		  181		 /* Total 13, programmable total */
#define TRN_TOTAL14_ADR		  182		 /* Total 14, programmable total */
#define TRN_TOTAL15_ADR		  183		 /* Total 15, programmable total */
#define TRN_TOTAL16_ADR		  184		 /* Total 16, programmable total */
#define TRN_TOTAL17_ADR		  185		 /* Total 17, programmable total */
#define TRN_TOTAL18_ADR		  186		 /* Total 18, programmable total */
#define TRN_TOTAL19_ADR		  187		 /* Total 19, programmable total */
#define TRN_TOTAL20_ADR		  188		 /* Total 20, programmable total */
#define	TRN_PRICE_ADR		  189		 /* PRICE	 */ //SR281
#define TRN_NONE_DONE_ADR	  190		// Mnemonic for NONE/DONE for OEP dialog, MldUpdatePopUp()
#define TRN_BACK_OEP_ADR	  191		// Mnemonic for BACK for OEP dialog, MldUpdatePopUp()
#define TRN_ITEMCOUNT_ADR	  192		//  Item Count Summary SR217 JHHJ
#define TRN_MERC_CPY_ADR	  193		// Merchant Copy mnemonic for receipt EPT if CURQUAL_MERCH_DUP set
#define TRN_CUST_CPY_ADR	  194		// Customer Copy mnemonic for receipt EPT if CURQUAL_CUST_DUP set
#define TRN_TIP_LINE_ADR	  195		// Tip Line for receipt EPT
#define TRN_TTL_LINE_ADR	  196		// Total Line for receipt EPT
#define TRN_GIFT_RECEIPT_ADR  197		// Gift Receipt mnemonic, if PRT_GIFT_RECEIPT set
#define TRN_OPTIMIZE_FILE_ADR 198		// Optimize File JHH*/
#define TRN_ITMDISC_ADR_3	  199		// Item Discount 3
#define TRN_ITMDISC_ADR_4	  200		// Item Discount 4
#define TRN_ITMDISC_ADR_5	  201		// Item Discount 5
#define TRN_ITMDISC_ADR_6	  202		// Item Discount 6
#define TRN_BNS_9_ADR		  203		// Bonus Total # 9
#define TRN_BNS_10_ADR		  204		// Bonus Total # 10
#define TRN_BNS_11_ADR		  205		// Bonus Total # 11
#define TRN_BNS_12_ADR		  206		// Bonus Total # 12
#define TRN_BNS_13_ADR		  207		// Bonus Total # 13
#define TRN_BNS_14_ADR		  208		// Bonus Total # 14
#define TRN_BNS_15_ADR		  209		// Bonus Total # 15
#define TRN_BNS_16_ADR		  210		// Bonus Total # 16
#define TRN_BNS_17_ADR		  211		// Bonus Total # 17
#define TRN_BNS_18_ADR		  212		// Bonus Total # 18
#define TRN_BNS_19_ADR		  213		// Bonus Total # 19
#define TRN_BNS_20_ADR		  214		// Bonus Total # 20
#define TRN_BNS_21_ADR		  215		// Bonus Total # 21
#define TRN_BNS_22_ADR		  216		// Bonus Total # 22
#define TRN_BNS_23_ADR		  217		// Bonus Total # 23
#define TRN_BNS_24_ADR		  218		// Bonus Total # 24
#define TRN_BNS_25_ADR		  219		// Bonus Total # 25
#define TRN_BNS_26_ADR		  220		// Bonus Total # 26
#define TRN_BNS_27_ADR		  221		// Bonus Total # 27
#define TRN_BNS_28_ADR		  222		// Bonus Total # 28
#define TRN_BNS_29_ADR		  223		// Bonus Total # 29
#define TRN_BNS_30_ADR		  224		// Bonus Total # 30
#define TRN_BNS_31_ADR		  225		// Bonus Total # 31
#define TRN_BNS_32_ADR		  226		// Bonus Total # 32
#define TRN_BNS_33_ADR		  227		// Bonus Total # 33
#define TRN_BNS_34_ADR		  228		// Bonus Total # 34
#define TRN_BNS_35_ADR		  229		// Bonus Total # 35
#define TRN_BNS_36_ADR		  230		// Bonus Total # 36
#define TRN_BNS_37_ADR		  231		// Bonus Total # 37
#define TRN_BNS_38_ADR		  232		// Bonus Total # 38
#define TRN_BNS_39_ADR		  233		// Bonus Total # 39
#define TRN_BNS_40_ADR		  234		// Bonus Total # 40
#define TRN_BNS_41_ADR		  235		// Bonus Total # 41
#define TRN_BNS_42_ADR		  236		// Bonus Total # 42
#define TRN_BNS_43_ADR		  237		// Bonus Total # 43
#define TRN_BNS_44_ADR		  238		// Bonus Total # 44
#define TRN_BNS_45_ADR		  239		// Bonus Total # 45
#define TRN_BNS_46_ADR		  240		// Bonus Total # 46
#define TRN_BNS_47_ADR		  241		// Bonus Total # 47
#define TRN_BNS_48_ADR		  242		// Bonus Total # 48
#define TRN_BNS_49_ADR		  243		// Bonus Total # 49
#define TRN_BNS_50_ADR		  244		// Bonus Total # 50
#define TRN_BNS_51_ADR		  245		// Bonus Total # 51
#define TRN_BNS_52_ADR		  246		// Bonus Total # 52
#define TRN_BNS_53_ADR		  247		// Bonus Total # 53
#define TRN_BNS_54_ADR		  248		// Bonus Total # 54
#define TRN_BNS_55_ADR		  249		// Bonus Total # 55
#define TRN_BNS_56_ADR		  250		// Bonus Total # 56
#define TRN_BNS_57_ADR		  251		// Bonus Total # 57
#define TRN_BNS_58_ADR		  252		// Bonus Total # 58
#define TRN_BNS_59_ADR		  253		// Bonus Total # 59
#define TRN_BNS_60_ADR		  254		// Bonus Total # 60
#define TRN_BNS_61_ADR		  255		// Bonus Total # 61
#define TRN_BNS_62_ADR		  256		// Bonus Total # 62
#define TRN_BNS_63_ADR		  257		// Bonus Total # 63
#define TRN_BNS_64_ADR		  258		// Bonus Total # 64
#define TRN_BNS_65_ADR		  259		// Bonus Total # 65
#define TRN_BNS_66_ADR		  260		// Bonus Total # 66
#define TRN_BNS_67_ADR		  261		// Bonus Total # 67
#define TRN_BNS_68_ADR		  262		// Bonus Total # 68
#define TRN_BNS_69_ADR		  263		// Bonus Total # 69
#define TRN_BNS_70_ADR		  264		// Bonus Total # 70
#define TRN_BNS_71_ADR		  265		// Bonus Total # 71
#define TRN_BNS_72_ADR		  266		// Bonus Total # 72
#define TRN_BNS_73_ADR		  267		// Bonus Total # 73
#define TRN_BNS_74_ADR		  268		// Bonus Total # 74
#define TRN_BNS_75_ADR		  269		// Bonus Total # 75
#define TRN_BNS_76_ADR		  270		// Bonus Total # 76
#define TRN_BNS_77_ADR		  271		// Bonus Total # 77
#define TRN_BNS_78_ADR		  272		// Bonus Total # 78
#define TRN_BNS_79_ADR		  273		// Bonus Total # 79
#define TRN_BNS_80_ADR		  274		// Bonus Total # 80
#define TRN_BNS_81_ADR		  275		// Bonus Total # 81
#define TRN_BNS_82_ADR		  276		// Bonus Total # 82
#define TRN_BNS_83_ADR		  277		// Bonus Total # 83
#define TRN_BNS_84_ADR		  278		// Bonus Total # 84
#define TRN_BNS_85_ADR		  279		// Bonus Total # 85
#define TRN_BNS_86_ADR		  280		// Bonus Total # 86
#define TRN_BNS_87_ADR		  281		// Bonus Total # 87
#define TRN_BNS_88_ADR		  282		// Bonus Total # 88
#define TRN_BNS_89_ADR		  283		// Bonus Total # 89
#define TRN_BNS_90_ADR		  284		// Bonus Total # 90
#define TRN_BNS_91_ADR		  285		// Bonus Total # 91
#define TRN_BNS_92_ADR		  286		// Bonus Total # 92
#define TRN_BNS_93_ADR		  287		// Bonus Total # 93
#define TRN_BNS_94_ADR		  288		// Bonus Total # 94
#define TRN_BNS_95_ADR		  289		// Bonus Total # 95
#define TRN_BNS_96_ADR		  290		// Bonus Total # 96
#define TRN_BNS_97_ADR		  291		// Bonus Total # 97
#define TRN_BNS_98_ADR		  292		// Bonus Total # 98
#define TRN_BNS_99_ADR		  293		// Bonus Total # 99
#define TRN_BNS_100_ADR		  294		// Bonus Total # 100
#define	TRN_PPI_ADD_SETTING	  295		// PPI Addititional Settings
#define	TRN_PPI_ADD_VALUE	  296		// PPI Additional Setting Value
#define TRN_PPI_BEFORE_QTY	  297
#define	TRN_PPI_AFTER_QTY	  298
#define TRN_PPI_MINSALE		  299
#define TRN_TENT			  300		// Receipt ID ***PDINU
#define TRN_ORDER_DEC_OFFSET  300       // add order declaration fsc to this value to get mnemonic
#define TRN_ORDER_DEC1		  301
#define TRN_ORDER_DEC2		  302
#define TRN_ORDER_DEC3		  303
#define TRN_ORDER_DEC4		  304
#define TRN_ORDER_DEC5		  305
#define TRN_TOTAL21_ADR		  311		 /* Total 21 */
#define TRN_TOTAL22_ADR		  312		 /* Total 22 */
#define TRN_TOTAL23_ADR		  313		 /* Total 23 */
#define TRN_TOTAL24_ADR		  314		 /* Total 24 */
#define TRN_TOTAL25_ADR		  315		 /* Total 25 */
#define TRN_TOTAL26_ADR		  316		 /* Total 26 */
#define TRN_TOTAL27_ADR		  317		 /* Total 27 */
#define TRN_TOTAL28_ADR		  318		 /* Total 28 */
#define TRN_TOTAL29_ADR		  319		 /* Total 39 */
#define TRN_TOTAL30_ADR		  320		 /* Total 30 */
#define TRN_TENDER21_ADR      321		 /* Tender 21 */
#define TRN_TENDER22_ADR      322		 /* Tender 22 */
#define TRN_TENDER23_ADR      323		 /* Tender 23 */
#define TRN_TENDER24_ADR      324		 /* Tender 24 */
#define TRN_TENDER25_ADR      325		 /* Tender 25 */
#define TRN_TENDER26_ADR      326		 /* Tender 26 */
#define TRN_TENDER27_ADR      327		 /* Tender 27 */
#define TRN_TENDER28_ADR      328		 /* Tender 28 */
#define TRN_TENDER29_ADR      329		 /* Tender 29 */
#define TRN_TENDER30_ADR      330		 /* Tender 30 */
#define TRN_SIGNIN_ADR        331        /* Operator Sign in */
#define TRN_SIGNOUT_ADR       332        /* Operator Sign Out */
#define TRN_TRETURN1_ADR      333        /* Transaction return #1 mnemonic, report MDC_RPTFIN46_ADR and MDC_RPTCAS50_ADR */
#define TRN_TRETURN2_ADR      334        /* Transaction return #2 mnemonic, report MDC_RPTFIN46_ADR and MDC_RPTCAS50_ADR */
#define TRN_TRETURN3_ADR      335        /* Transaction return #3 mnemonic, report MDC_RPTFIN46_ADR and MDC_RPTCAS50_ADR */
#define TRN_PRETURN1_ADR      336        /* Transaction return mnemonic for creating a Transaction Return by entering items */
#define TRN_PRETURN2_ADR      337        /* Transaction return mnemonic for creating a Transaction Return by entering items */
#define TRN_PRETURN3_ADR      338        /* Transaction return mnemonic for creating a Transaction Return by entering items */
#define TRN_FT_EQUIVALENT     339        /* Tender amount equivalent mnemonic for specifing local currency equivalent to foreign currency */
#define TRN_GCNO_RETURNS      340        // lead thru mnemonic for guest check for Ask #6, Returns
#define TRN_DSI_CARDTYPE      341        // CARD TYPE, ItemEptMsgTextCopyReplace() DSI Client mnemonic using #P20.nnn replacement
#define TRN_DSI_CARDNUM       342        // CARD #, ItemEptMsgTextCopyReplace() DSI Client mnemonic using #P20.nnn replacement
#define TRN_DSI_CARDEXP       343        // EXP. DATE, ItemEptMsgTextCopyReplace() DSI Client mnemonic using #P20.nnn replacement
#define TRN_DSI_AUTHCODE      344        // AUTH CODE, ItemEptMsgTextCopyReplace() DSI Client mnemonic using #P20.nnn replacement
#define TRN_DSI_REFNUM        345        // REF NUM, ItemEptMsgTextCopyReplace() DSI Client mnemonic using #P20.nnn replacement
#define TRN_DSI_AUTHORIZE     346        // AUTHORIZE, ItemEptMsgTextCopyReplace() DSI Client mnemonic using #P20.nnn replacement
#define TRN_DSI_BALANCE       347        // BALANCE, ItemEptMsgTextCopyReplace() DSI Client mnemonic using #P20.nnn replacement
#define TRN_DSI_METHOD        348        // METHOD, ItemEptMsgTextCopyReplace() DSI Client mnemonic using #P20.nnn replacement
#define TRN_DSI_TERMINAL      349        // TERMINAL ID, ItemEptMsgTextCopyReplace() DSI Client mnemonic using #P20.nnn replacement
#define TRN_DSI_TRANSTYPE     350        // TRANSACTION TYPE, ItemEptMsgTextCopyReplace() DSI Client mnemonic using #P20.nnn replacement
#define TRN_DSI_TRANSSALE     351        // SALE, ItemEptMsgTextCopyReplace() DSI Client mnemonic using #P20.nnn replacement
#define TRN_EJ_PODREPRINT     352        // Print on Demand #5 Electronic Journal mnemonic if PRT_DEMAND set 
#define TRN_TNDR_PRT_DUP_COPY 353        // Print on a duplicate receipt the mnemonic for Duplicate Copy

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define TRN_ADR_MAX           MAX_TRANSM_NO        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Lead Through Mnemonics Address (PRG # 21), P21 Mnemonics
*
*	see ParaLeadThru in UNINIRAM UNINT Para and CLASS_PARALEADTHRU with CliParaAllRead()
\*------------------------------------------------------------------------*/

#define LDT_NTINFL_ADR          1       /* Not In File */
#define LDT_FLFUL_ADR           2       /* File Full */
#define LDT_KTNERR_ADR          3       /* Kitchen Printer Error */
#define LDT_JNLLOW_ADR          4       /* Journal Low Condition */
#define LDT_PRTTBL_ADR          5       /* Printer Trouble */
#define LDT_LNKDWN_ADR          6       /* Link Down or Server state is SER_STINQUIRY */
#define LDT_SYSBSY_ADR          7       /* System Busy */
#define LDT_KEYOVER_ADR         8       /* Key Entered Over Limitation */
#define LDT_SEQERR_ADR          9       /* Key Entry Sequence Error */
#define LDT_PROHBT_ADR         10       /* Prohibit Operation */
#define LDT_TAKETL_ADR         11       /* Take Total for Transaction Over */
#define LDT_GCFNTEMPT_ADR      12       /* Guest Check File is not Empty */
#define LDT_TLCOACT_ADR        13       /* Total & Counter are Active */
#define LDT_OPNWTIN_ADR        14       /* Opened Cashier/Waiter File exist in Reset Report */
#define LDT_GCFWFERR_ADR       15       /* Guest Check File or Waiter File can not be accessed */
#define LDT_NOUNMT_ADR         16       /* Cashier/Waiter # Unmatch */
#define LDT_SAMEGCNO_ADR       17       /* Same Check # Already Exists in GCF */
#define LDT_ILLEGLNO_ADR       18       /* Illegal Guest Check # */
#define LDT_DRAWCLSE_ADR       19       /* Request Drawer Closing */
#define LDT_GCFSUNMT_ADR       20       /* Guest Check File Status Unmatch */
#define LDT_ERR_ADR            21       /* Error(Global) */
#define LDT_LOCK_ADR           22       /* During Lock */
#define LDT_NOT_ISSUED_ADR     23       /* Not Issued Report */
#define LDT_DEPTNO_ADR         24       /* Request Department # Entry */
#define LDT_DEPTCTL_ADR        25       /* Request Copy Department Control Code */
#define LDT_FUNCTYPE_ADR       26       /* Request Function Type Entry */
#define LDT_REPOTYPE_ADR       27       /* Request Report Type Entry */
#define LDT_NUM_ADR            28       /* Number Entry */
#define LDT_PLUTYPE_ADR        29       /* Request PLU Type Entry */
#define LDT_DELFUNC_ADR        30       /* Request Delete Function */
#define LDT_REPOCD_ADR         31       /* Request Report Code # Entry */
#define LDT_STATUS_ADR         32       /* Request Status Entry */
#define LDT_PP_ADR             33       /* Request Preset Price Entry */
#define LDT_MDEPT_ADR          34       /* Request Major Department No. Entry */
#define LDT_HALO_ADR           35       /* Request HALO Data */
#define LDT_RESET_ADR          36       /* Lead Through for Reset Report */
#define LDT_SIGNIN_ADR         37       /* Request Cashier/Waiter/Supervisor Sign-in */
#define LDT_SUPINTR_ADR        38       /* Request Supervisor Intervention */
#define LDT_VSLPINS_ADR        39       /* Request Validation Slip Insertion */
#define LDT_VSLPREMV_ADR       40       /* Request Validation Slip Removing */
#define LDT_ID_ADR             41       /* Request ID # Entry */
#define LDT_AC_ADR             42       /* Request Action Code Entry */
#define LDT_NOACFIN_ADR        43       /* Request # Entry or A/C Key Finalize */
#define LDT_PMODCNDI_ADR       44       /* Request Print Modifier/Condiment Entry */
#define LDT_WTNO_ADR           45       /* Enter Waiter # */
#define LDT_PROGRES_ADR        46       /* Progress Transaction */
#define LDT_NOOFPRSN_ADR       47       /* Enter No. of Person */
#define LDT_TBLNO_ADR          48       /* Enter Table No. */
#define LDT_SLPINS_ADR         49       /* Request Slip Insertion */
#define LDT_PAPERCHG_ADR       50       /* Request Slip Paper Change */
#define LDT_LINE_ADR           51       /* Request Line # Entry */
#define LDT_DATA_ADR           52       /* Request Data Entry */
#define LDT_CLRABRT_ADR        53       /* Request Clear/Abort Entry */
#define LDT_SLCDORP_ADR        54       /* Select Daily or PTD Report */
#define LDT_COPYOK_ADR         55       /* Copy OK? */
#define LDT_COMUNI_ADR         56       /* During Communication */
#define LDT_BD_ADR             57       /* Balance Due */
#define LDT_SCALEERR_ADR       58       /* Scale Error */
#define LDT_TARE_ADR           59       /* Scale Tare */
#define LDT_NEARFULL_ADR       60       /* E/J File Full */
#define LDT_FULL_ADR           61       /* E/J Near Full */
#define LDT_SHRALT_ADR         62       /* Printer Alternated */
#define LDT_DFLERR_ADR         63       /* display on the fly error */
#define LDT_ETKERR_ADR         64       /* ETK error */
#define LDT_ENTROOM_ADR        65       /* Enter room number */
#define LDT_GUESTID_ADR        66       /* Enter guest ID */
#define LDT_GLINE_ADR          67       /* Enter guest line number */
#define LDT_OVERINT_ADR        68       /* Request override intervation */
#define LDT_CP_NOALLOW_ADR     69       /* Charge not allow */
#define LDT_GC_OUT_ADR         70       /* Guest checked out */
#define LDT_WRONG_RM_ADR       71       /* Wrong room number */
#define LDT_WRONG_GID_ADR      72       /* Wrong guest ID */
#define LDT_VOID_NOALLOW_ADR   73       /* Void not allowed */
#define LDT_WRONG_ACNO_ADR     74       /* Wrong account number */
#define LDT_CP_ADVISE_ADR      75       /* Advise front desk */
#define LDT_WRONG_ACTYPE_ADR   76       /* Wrong account type */
#define LDT_WRONG_SLD_ADR      77       /* Wrong SLD */
#define LDT_WRONG_VOUCH_ADR    78       /* Wrong voucher number */
#define LDT_CPM_BUSY_ADR       79       /* CPM busy */
#define LDT_CPM_STOP_FUNC_ADR  80       /* CPM Stop function */
#define LDT_CPM_OFFLINE_ADR    81       /* CPM offline */
#define LDT_EPT_REJECT_ADR     82       /* EPT rejected */
#define LDT_EPT_OFFLINE_ADR    83       /* EPT offline */
#define LDT_CANCEL_ADR         84       /* Canceled by user */
#define LDT_ENT_EXP_ADR        85       /* Lead Expiration Date */
#define LDT_TBLNUM_ADR         86       /* Table Number Request */
#define LDT_CPNNO_ADR          87       /* Coupoun Number Request */
// See function UifRegPauseString() for the handling of Pause keys
#define LDT_PAUSEMSG1_ADR      88       /* Control String Table -- Pause Message #1 for Numeric, influenced by MDC Address 515 MDC_PAUSE_1_2_ADR */
#define LDT_PAUSEMSG2_ADR      89       /* Control String Table -- Pause Message #2 for Numeric, influenced by MDC Address 515 MDC_PAUSE_1_2_ADR */
#define LDT_PAUSEMSG3_ADR      90       /* Control String Table -- Pause Message #3 for Numeric, influenced by MDC Address 516 MDC_PAUSE_3_4_ADR */
#define LDT_PAUSEMSG4_ADR      91       /* Control String Table -- Pause Message #4 for Numeric, influenced by MDC Address 516 MDC_PAUSE_3_4_ADR */
#define LDT_PAUSEMSG5_ADR      92       /* Control String Table -- Pause Message #5 for Numeric */
#define LDT_PAUSEMSG6_ADR      93       /* Control String Table -- Pause Message #6 for Alpha */
#define LDT_PAUSEMSG7_ADR      94       /* Control String Table -- Pause Message #7 for Alpha */
#define LDT_PAUSEMSG8_ADR      95       /* Control String Table -- Pause Message #8 for Alpha */
#define LDT_PAUSEMSG9_ADR      96       /* Control String Table -- Pause Message #9 for Alpha */
#define LDT_PAUSEMSG10_ADR     97       /* Control String Table -- Pause Message #10 for Alpha */
#define LDT_CUST_NAME_ADR      98       /* Customer Name Entry */
#define LDT_UNIQUE_NO_ADR      99       /* Unique Transaction No. for Print */
#define LDT_OEP_ADR           100       /* OEP Lead Through */
#define LDT_OEP91_ADR         101       /* OEP Lead Through (91) */
#define LDT_OEP92_ADR         102       /* OEP Lead Through (92) */
#define LDT_OEP93_ADR         103       /* OEP Lead Through (93) */
#define LDT_OEP94_ADR         104       /* OEP Lead Through (94) */
#define LDT_OEP95_ADR         105       /* OEP Lead Through (95) */
#define LDT_OEP96_ADR         106       /* OEP Lead Through (96) */
#define LDT_OEP97_ADR         107       /* OEP Lead Through (97) */
#define LDT_OEP98_ADR         108       /* OEP Lead Through (98) */
#define LDT_UPPER_ADR         109       /* Scroll Up End */
#define LDT_LOWER_ADR         110       /* Scroll Down End */
#define LDT_WELCOME_ADR       111       /* Welcome Message for 2x20 Customer */
#define LDT_EJREAD_ADR        112       /* E/J Read Error */
#define LDT_EJRESET_ADR       113       /* E/J Reset Error */
#define LDT_NEXTENT_ADR       114       /* Next Entry Message */
#define LDT_SECRET_ADR        115       /* Enter Secret Code, V3.3 */
#define LDT_REGMSG_ADR        116       /* Reg. Mode Message, V3.3 */
#define LDT_SUPMSG_ADR        117       /* Sup. Mode Message, V3.3 */
#define LDT_CUSCLOSE_ADR      118       /* Closed Message for 2X20 Customer Diaplay, V3.3 */
#define LDT_RMVWTLOCK_ADR     119       /* Remove Waiter Lock, V3.3 */
#define LDT_INSWTLOCK_ADR     120       /* insert waiter lock, V3.3 */
#define LDT_PLUBLDOP_ADR      121       /* Start PLU Building Opration, 2172 */
#define LDT_BDAY_ADR          122       /* Request Birthday Date, 2172 */
#define LDT_CLEAR_ADR         123       /* Request Clear KEy,   Saratoga */
#define LDT_WICTRN_ADR        124       /* WIC transaction key */
#define LDT_NONWIC_ADR        125       /* Non WIC item */
#define LDT_POWERDOWN_ADR     126       /* Power Down Process */
#define LDT_LOWSTORAGE_ADR    127       /* Storage Low */
#define LDT_NOSTORAGE_ADR     128       /* Not Enough Storage */
#define LDT_NOMEMORY_ADR      129       /* Not Enough Memory */
//Additional alpha and numeric pause messages, FSC_PAUSE, (5 each) added for US Customs SCER cwunn 3/19/2003
// See function UifRegPauseString() for the handling of Pause keys
#define LDT_PAUSEMSG11_ADR    130       /* Control String Table -- Pause Message #11 for Numeric */
#define LDT_PAUSEMSG12_ADR    131       /* Control String Table -- Pause Message #12 for Numeric */
#define LDT_PAUSEMSG13_ADR    132       /* Control String Table -- Pause Message #13 for Numeric */
#define LDT_PAUSEMSG14_ADR    133       /* Control String Table -- Pause Message #14 for Numeric */
#define LDT_PAUSEMSG15_ADR    134       /* Control String Table -- Pause Message #15 for Numeric, asterisk substitution */
#define LDT_PAUSEMSG16_ADR    135       /* Control String Table -- Pause Message #16 for Alpha */
#define LDT_PAUSEMSG17_ADR    136       /* Control String Table -- Pause Message #17 for Alpha */
#define LDT_PAUSEMSG18_ADR    137       /* Control String Table -- Pause Message #18 for Alpha */
#define LDT_PAUSEMSG19_ADR    138       /* Control String Table -- Pause Message #19 for Alpha */
#define LDT_PAUSEMSG20_ADR    139       /* Control String Table -- Pause Message #20 for Alpha, asterisk substitution */

#define LDT_BLOCKEDBYMDC_ADR    140     /* action blocked by MDC bit setting in parameters */
#define LDT_TARENONSCALABLE_ADR 141     /* tare key for a non-scalable item find MOD_TARE_KEY_DEPRESSED */
#define LDT_CPM_UNKNOWNRESPONSE 142     /* Charge Post responded with an unknown response code */
#define LDT_INVALID_SUP_NUM		143		/* Invalid Supervisor Number JHHJ*/
#define LDT_SALESRESTRICTED     144     /* Item sales restricted as in age or not on Sunday */
#define LDT_EQUIPPROVERROR_ADR  145     /* Equipment provisioning needs changing */

#define LDT_KPS_BUSY				146		// Kitchen printer is busy as indicated by KpsCheckPrint ()
#define LDT_REQST_TIMED_OUT			147		// Requested operation timed out before completion
#define LDT_I_AM_NOT_MASTER			148		// This terminal is not acting as a Master terminal
#define LDT_REFERENCENUM_ADR		149		// Reference # entry
#define LDT_AUTHCODE_ADR			150		// Authorization Code Entry
#define LDT_UNREGISTERED_ADR		151		// Unregistered Copy
#define LDT_KDS_BACKUP_DOWN			152		// KDS ERROR JHHJ SR 236
#define LDT_NOTSIGNEDIN_ADR			153		// Sequence error when not signed into register SR289 ESMITH
#define	LDT_CHKSEQUENCENUM_ADR		154		// Check Sequence number
#define LDT_CHKROUTINGNUM_ADR		155		// Check Routing Number
#define LDT_AMT_ENTRY_REQUIRED_ADR	156		// Amount Entry Required
#define	LDT_PRT_COVEROPEN_ADR		157		// Printer Cover Open
#define	LDT_PRT_PAPERCHNG_ADR		158		// Change Paper
#define LDT_TIP_REQUEST_ADR			159		// No Tips Entered
#define LDT_PLUGROUPNO_ADR			160		// PLU Group Number Request
#define LDT_TENT					161		// Receipt ID information
#define LDT_ORDER_DEC_ADR			162		// Require Order Declaration JHHJ"
#define LDT_NOT_ALLOW_QR			163		// Quantity not allowed for PLU error
#define LDT_REQUIRE_QR				164		// Require Quantity required
#define LDT_TARE_ENTER				165		// Tare required before weighing JHHJ
#define LDT_GIFTCARD_BALANCE		166		// GiftCard Balance  SS
#define LDT_CARD_NUM				167		// Card number		 SS
#define LDT_VOID_ISSUE				168		// Gift Card Void Issue		SS
#define LDT_RELOAD					169		// Gift Card Reload
#define LDT_VOID_RELOAD				170		// Gift Card Void Reload
#define LDT_ISSUE					171		// Gift Card Issue
#define LDT_STORE_FORWARD_STATUS	172		// Store and Forward
#define LDT_AC_444					173		// Disconn. Satellite
#define LDT_END_FORWARD				174		// end message for Forwarding Transactions
#define LDT_STORED_TRANS_ERROR		175		// when deleting Store and Forward, a transactions has not been processed
#define LDT_SF_TRANS_LIMIT			176		// error when the size of the transactions is over the limit Store and Forward
#define LDT_STORED_LIMIT			177		// when a transactions is being stored that is over the stored limit Store and Forward
#define LDT_FILE_EMPTY				178		// File is empty and has nothing to read
#define LDT_FORWARDING_INFO			179		// Forwarding info for Store and forward
#define LDT_AUTO_SIGN_OUT			180		// Auto Sign Out
#define LDT_ALTERNATE_TENDER_RQD    181     // An alternate tender is required
#define LDT_INVALID_ACCOUNT_USED    182     // Validation of account number failed so alternate tender required

// NOTE: Any changes to this list should be reflected in build\nhpos\businesslogic\para\uniniram.c
// in section called Prog.21 LEAD THROUGH TABLE
// See code in comment at head of table for an easy way to format your table entry.

// The following leadthrough defines are special,
// used for conveying events back up to the error handling
// code so that the event can be handled either as a 
// special window display event or an error dialoge event.
#define LDT_ORDER_DEC_EVENT			(MAX_LEAD_NO+1)		// Forced Order Decrlaration Event
#define LDT_POST_ODRDEC_EVENT		(MAX_LEAD_NO+2)		//
#define LDT_OPERATOR_ACTION_EVENT	(MAX_LEAD_NO+3)		// used to indicate FSC_OPERATOR_ACTION received in UI thread
#define LDT_SUC_KEY_TRANSPARENT     (MAX_LEAD_NO+4)     // used to indicate action should be ignore as if did not happen.  same as UIE_SUC_KEY_TRANSPARENT.
#define LDT_EPT_ERROR_MULTIUSE      (MAX_LEAD_NO+5)     // used to indicate EPT error with extended error code.  see UieSetEptErrorText().

#define LDT_EXTENDED_ERROR_INFO     0x4000              // flag that extended error information is available
#define LDT_EXTENDED_ERROR_MASK     0x01ff              // mask to remove flags from the base LDT code

/*------------------------------------------------------------------------*\
*   Define - Report Name Address (PRG #22), P22 Mnemonics
*
*	ParaReportName in UNINIRAM UNINT Para and CLASS_PARAREPORTNAME with CliParaAllRead()
\*------------------------------------------------------------------------*/

#define RPT_ACT_ADR             1        /* Action Number */
#define RPT_FINANC_ADR          2        /* Register Financial Report */
#define RPT_DEPT_ADR            3        /* Department Report/Maintenance */
#define RPT_PLU_ADR             4        /* PLU Report */
#define RPT_HOUR_ADR            5        /* Hourly Activity Report */
#define RPT_CAS_ADR             6        /* Cashier Report */
#define RPT_WAT_ADR             7        /* Waiter Report */
#define RPT_EOD_ADR             8        /* EOD */
#define RPT_PTD_ADR             9        /* PTD */
#define RPT_GCF_ADR            10        /* Guest Check File Report */
#define RPT_PRG_ADR            11        /* Program Number */
#define RPT_FLASH_ADR          12        /* Madia Flash Report */
#define RPT_MDEPT_ADR          13        /* Major Department Report */
#define RPT_SETPLU_ADR         14        /* Set PLU No. on Menu Page */
#define RPT_READ_ADR           15        /* Read Report */
#define RPT_RESET_ADR          16        /* Reset Report */
#define RPT_MAINT_ADR          17        /* Maintenance Report */
#define RPT_FILE_ADR           18        /* File Report */
#define RPT_EJ_ADR             19        /* EJ File */
#define RPT_ETK_ADR            20        /* Employee Maintenance REPORT */
#define RPT_CPM_ADR            21        /* CPM Tally Report */
#define RPT_EPT_ADR            22        /* EPT Tally Report */
#define RPT_CPN_ADR            23        /* Coupon Report    */
#define RPT_SERVICE_ADR        24        /* Service Time Report    */

#define RPT_ADR_MAX        MAX_REPO_NO      /* Max Address */



/*------------------------------------------------------------------------*\
*   Define - Special Mnemonics Address (PRG #23), P23 Mnemonics
*
*	see ParaSpeMnemo in UNINIRAM UNINT Para and CLASS_PARASPECIALMNEMO with CliParaAllRead()
\*------------------------------------------------------------------------*/

#define SPC_GC_ADR              1        /* Guest Check No. */
#define SPC_CAS_ADR             2        /* Cashier ID */
#define SPC_WT_ADR              3        /* Waiter ID */
#define SPC_TBL_ADR             4        /* Table No. */
#define SPC_DAIRST_ADR          5        /* Daily Reset Report */
#define SPC_PTDRST_ADR          6        /* PTD Reset Counter */
#define SPC_PRGCO_ADR           7        /* Program Counter */
#define SPC_TRNGID_ADR          8        /* Training ID for Cashier and Waiter */
#define SPC_TXMOD1_ADR          9        /* Tax Modifier #1 */
#define SPC_TXMOD2_ADR         10        /* Tax Modifier #2 */
#define SPC_TXMOD3_ADR         11        /* Tax Modifier #3 */
#define SPC_ROOMNO_ADR         12        /* Room Number for Charge Posting */
#define SPC_GUESTID_ADR        13        /* Guest ID for Charge Posting */
#define SPC_FOLINO_ADR         14        /* Folio Number for Charge Posting */
#define SPC_POSTTRAN_ADR       15        /* Post Transaction Number for Charge Posting */
#define SPC_TXMOD8_ADR         16        /* Tax Modifier #8 */
#define SPC_TXMOD9_ADR         17        /* Tax Modifier #9 */
#define SPC_TXMOD10_ADR        18        /* Tax Modifier #10 */
#define SPC_TXMOD11_ADR        19        /* Tax Modifier #11 */
#define SPC_LINE_ADR           20        /* Line No. */
#define SPC_CNSYMNTV_ADR       21        /* Currency Symbol for Native */
#define SPC_CNSYMFC1_ADR       22        /* Currency Symbol for Foreign Curency #1 */
#define SPC_CNSYMFC2_ADR       23        /* Currency Symbol for Foreign Curency #2 */
#define SPC_KPTR_ADR           24        /* Kitchen Printer */
#define SPC_LB_ADR             25        /* KgLB */
#define SPC_VOID_ADR           26        /* Void */
#define SPC_OFFTEND_ADR        27        /* Off Line Tender */
#define SPC_SEAT_ADR           28        /* Seat No. */
#define SPC_TXMOD4_ADR         29        /* Tax Modifier #4,   R3.1 */
#define SPC_TXMOD5_ADR         30        /* Tax Modifier #5,   R3.1 */
#define SPC_TXMOD6_ADR         31        /* Tax Modifier #6,  R3.1 */
#define SPC_TXMOD7_ADR         32        /* Tax Modifier #7,  R3.1 */
#define SPC_CNSYMFC3_ADR       33        /* Currency Symbol for Foreign Curency #3, 2172 */
#define SPC_CNSYMFC4_ADR       34        /* Currency Symbol for Foreign Curency #4, 2172 */
#define SPC_CNSYMFC5_ADR       35        /* Currency Symbol for Foreign Curency #5, 2172 */
#define SPC_CNSYMFC6_ADR       36        /* Currency Symbol for Foreign Curency #6, 2172 */
#define SPC_CNSYMFC7_ADR       37        /* Currency Symbol for Foreign Curency #7, 2172 */
#define SPC_CNSYMFC8_ADR       38        /* Currency Symbol for Foreign Curency #8, 2172 */
#define SPC_FSMOD_ADR          39        /* Food Stamp Modifier */

#define SPC_EC_ADR             40        /* Mnemonic for Qty SR 420 JHHJ*/
#define SPC_TX1_ADR            41        /* Taxable #1, 21RFC05437*/
#define SPC_TX2_ADR            42        /* Taxable #2 */
#define SPC_TX3_ADR            43        /* Taxable #3 */
#define SPC_TX12_ADR           44        /* Taxable #1&2 */
#define SPC_TX13_ADR           45        /* Taxable #1&3 */
#define SPC_TX23_ADR           46        /* Taxable #2&3 */
#define SPC_TX123_ADR          47        /* Taxable #1,2&3 */
#define SPC_FS_ADR             48        /* Food Stampable */
#define SPC_TX4_ADR            49        /* Taxable #4 */
#define SPC_TX14_ADR           50        /* Taxable #1&4 */
#define SPC_RETURNS_1          51        /* Returns #1 */
#define SPC_RETURNS_2          52        /* Returns #2 */
#define SPC_RETURNS_3          53        /* Returns #3 */


#define SPC_ADR_MAX        MAX_SPEM_NO       /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - PC Interface Address (PRG # 39)
\*------------------------------------------------------------------------*/

#define PCIF_PASSWORD_ADR       1      /* Password */
#define PCIF_COMMAND_ADR        2      /* Command for Modem */

#define PCIF_ADR_MAX            2       /* Max Address */


/*------------------------------------------------------------------------*\
*   Define - Adjective Mnemonics Address (PRG # 46), P46 Mnemonics
*
*	ParaAdjMnemo in UNINIRAM UNINT Para and CLASS_PARAADJMNEMO with CliParaAllRead()
\*------------------------------------------------------------------------*/

#define ADJ_GP1VAR1_ADR         1        /* Adjective Key Group #1 /Variation #1 */
#define ADJ_GP1VAR2_ADR         2        /* Adjective Key Group #1 /Variation #2 */
#define ADJ_GP1VAR3_ADR         3        /* Adjective Key Group #1 /Variation #3 */
#define ADJ_GP1VAR4_ADR         4        /* Adjective Key Group #1 /Variation #4 */
#define ADJ_GP1VAR5_ADR         5        /* Adjective Key Group #1 /Variation #5 */
#define ADJ_GP2VAR1_ADR         6        /* Adjective Key Group #2 /Variation #1 */
#define ADJ_GP2VAR2_ADR         7        /* Adjective Key Group #2 /Variation #2 */
#define ADJ_GP2VAR3_ADR         8        /* Adjective Key Group #2 /Variation #3 */
#define ADJ_GP2VAR4_ADR         9        /* Adjective Key Group #2 /Variation #4 */
#define ADJ_GP2VAR5_ADR        10        /* Adjective Key Group #2 /Variation #5 */
#define ADJ_GP3VAR1_ADR        11        /* Adjective Key Group #3 /Variation #1 */
#define ADJ_GP3VAR2_ADR        12        /* Adjective Key Group #3 /Variation #2 */
#define ADJ_GP3VAR3_ADR        13        /* Adjective Key Group #3 /Variation #3 */
#define ADJ_GP3VAR4_ADR        14        /* Adjective Key Group #3 /Variation #4 */
#define ADJ_GP3VAR5_ADR        15        /* Adjective Key Group #3 /Variation #5 */
#define ADJ_GP4VAR1_ADR        16        /* Adjective Key Group #4 /Variation #1 */
#define ADJ_GP4VAR2_ADR        17        /* Adjective Key Group #4 /Variation #2 */
#define ADJ_GP4VAR3_ADR        18        /* Adjective Key Group #4 /Variation #3 */
#define ADJ_GP4VAR4_ADR        19        /* Adjective Key Group #4 /Variation #4 */
#define ADJ_GP4VAR5_ADR        20        /* Adjective Key Group #4 /Variation #5 */

#define ADJ_ADR_MAX        MAX_ADJM_NO       /* Max Address */



#define ADJ_GP_MAX              4        /* Max Adjective Group # */
#define ADJ_GRP_MAX             4        /* Max Adjective Group # */
#define ADJ_VAL_MAX             5        /* Max Adjective Variation # */

/*------------------------------------------------------------------------*\
*   Define - Print Modifier Mnemonics Address (PRG #47)

	ParaPrtModi in UNINIRAM UNINT Para.
\*------------------------------------------------------------------------*/

#define PM_NO1_ADR              1        /* Print Modifier #1 */
#define PM_NO2_ADR              2        /* Print Modifier #2 */
#define PM_NO3_ADR              3        /* Print Modifier #3 */
#define PM_NO4_ADR              4        /* Print Modifier #4 */
#define PM_NO5_ADR              5        /* Print Modifier #5 */
#define PM_NO6_ADR              6        /* Print Modifier #6 */
#define PM_NO7_ADR              7        /* Print Modifier #7 */
#define PM_NO8_ADR              8        /* Print Modifier #8 */

#define PM_ADR_MAX        MAX_PRTMODI_NO       /* Max Address */



/*------------------------------------------------------------------------*\
*   Define - Major Department Mnemonics Address (PRG #48)

	see ParaMajorDEPT in UNINIRAM UNINT Para.
\*------------------------------------------------------------------------*/

#define MDPT_NO1_ADR            1        /* Major Department #1 */
#define MDPT_NO2_ADR            2        /* Major Department #2 */
#define MDPT_NO3_ADR            3        /* Major Department #3 */
#define MDPT_NO4_ADR            4        /* Major Department #4 */
#define MDPT_NO5_ADR            5        /* Major Department #5 */
#define MDPT_NO6_ADR            6        /* Major Department #6 */
#define MDPT_NO7_ADR            7        /* Major Department #7 */
#define MDPT_NO8_ADR            8        /* Major Department #8 */
#define MDPT_NO9_ADR            9        /* Major Department #9 */
#define MDPT_NO10_ADR          10        /* Major Department #10 2172 */
#define MDPT_NO11_ADR          11        /* Major Department #11 2172 */
#define MDPT_NO12_ADR          12        /* Major Department #12 2172 */
#define MDPT_NO13_ADR          13        /* Major Department #13 2172 */
#define MDPT_NO14_ADR          14        /* Major Department #14 2172 */
#define MDPT_NO15_ADR          15        /* Major Department #15 2172 */
#define MDPT_NO16_ADR          16        /* Major Department #16 2172 */
#define MDPT_NO17_ADR          17        /* Major Department #17 2172 */
#define MDPT_NO18_ADR          18        /* Major Department #18 2172 */
#define MDPT_NO19_ADR          19        /* Major Department #19 2172 */
#define MDPT_NO20_ADR          20        /* Major Department #20 2172 */
#define MDPT_NO21_ADR          21        /* Major Department #21 2172 */
#define MDPT_NO22_ADR          22        /* Major Department #22 2172 */
#define MDPT_NO23_ADR          23        /* Major Department #23 2172 */
#define MDPT_NO24_ADR          24        /* Major Department #24 2172 */
#define MDPT_NO25_ADR          25        /* Major Department #25 2172 */
#define MDPT_NO26_ADR          26        /* Major Department #26 2172 */
#define MDPT_NO27_ADR          27        /* Major Department #27 2172 */
#define MDPT_NO28_ADR          28        /* Major Department #28 2172 */
#define MDPT_NO29_ADR          29        /* Major Department #29 2172 */
#define MDPT_NO30_ADR          30        /* Major Department #30 2172 */

#define MDPT_ADR_MAX       MAX_MDEPT_NO        /* Max Address */



/*------------------------------------------------------------------------*\
*   Define - Auto Alternative Kitchen Printer Assignment Address (PRG #49)

	see ParaAutoAltKitch in UNINIRAM UNINT Para.
\*------------------------------------------------------------------------*/

#define AAKTN_NO1_ADR           1       /* Kitchen Device #1 */
#define AAKTN_NO2_ADR           2       /* Kitchen Device #2 */
#define AAKTN_NO3_ADR           3       /* Kitchen Device #3 */
#define AAKTN_NO4_ADR           4       /* Kitchen Device #4 */
#define AAKTN_NO5_ADR           5       /* Kitchen Device #5 */
#define AAKTN_NO6_ADR           6       /* Kitchen Device #6 */
#define AAKTN_NO7_ADR           7       /* Kitchen Device #7 */
#define AAKTN_NO8_ADR           8       /* Kitchen Device #8 */

#define AAKTN_ADR_MAX       MAX_DEST_SIZE      /* Max. Number of This Address size of ParaAutoAltKitch[] */



/*------------------------------------------------------------------------*\
*   Define - Assignment Terminal No.Installing Shared Printer  (PRG #50)

	see ParaSharedPrt in UNINIRAM UNINT Para.
\*------------------------------------------------------------------------*/

#define SHR_TERM1_DEF_ADR       1       /* Assign Shared Printer for Terminal No.1 */
#define SHR_TERM1_ALT_ADR       2       /* Assign Alternative Printer for Terminal No.1 */
#define SHR_TERM2_DEF_ADR       3       /* Assign Shared Printer for Terminal No.2 */
#define SHR_TERM2_ALT_ADR       4       /* Assign Alternative Printer for Terminal No.2 */
#define SHR_TERM3_DEF_ADR       5       /* Assign Shared Printer for Terminal No.3 */
#define SHR_TERM3_ALT_ADR       6       /* Assign Alternative Printer for Terminal No.3 */
#define SHR_TERM4_DEF_ADR       7       /* Assign Shared Printer for Terminal No.4 */
#define SHR_TERM4_ALT_ADR       8       /* Assign Alternative Printer for Terminal No.4 */
#define SHR_TERM5_DEF_ADR       9       /* Assign Shared Printer for Terminal No.5 */
#define SHR_TERM5_ALT_ADR      10       /* Assign Alternative Printer for Terminal No.5 */
#define SHR_TERM6_DEF_ADR      11       /* Assign Shared Printer for Terminal No.6 */
#define SHR_TERM6_ALT_ADR      12       /* Assign Alternative Printer for Terminal No.6 */
#define SHR_TERM7_DEF_ADR      13       /* Assign Shared Printer for Terminal No.7 */
#define SHR_TERM7_ALT_ADR      14       /* Assign Alternative Printer for Terminal No.7 */
#define SHR_TERM8_DEF_ADR      15       /* Assign Shared Printer for Terminal No.8 */
#define SHR_TERM8_ALT_ADR      16       /* Assign Alternative Printer for Terminal No.8 */
#define SHR_TERM9_DEF_ADR      17       /* Assign Shared Printer for Terminal No.9 */
#define SHR_TERM9_ALT_ADR      18       /* Assign Alternative Printer for Terminal No.9 */
#define SHR_TERM10_DEF_ADR     19       /* Assign Shared Printer for Terminal No.10 */
#define SHR_TERM10_ALT_ADR     20       /* Assign Alternative Printer for Terminal No.10 */
#define SHR_TERM11_DEF_ADR     21       /* Assign Shared Printer for Terminal No.11 */
#define SHR_TERM11_ALT_ADR     22       /* Assign Alternative Printer for Terminal No.11 */
#define SHR_TERM12_DEF_ADR     23       /* Assign Shared Printer for Terminal No.12 */
#define SHR_TERM12_ALT_ADR     24       /* Assign Alternative Printer for Terminal No.12 */
#define SHR_TERM13_DEF_ADR     25       /* Assign Shared Printer for Terminal No.13 */
#define SHR_TERM13_ALT_ADR     26       /* Assign Alternative Printer for Terminal No.13 */
#define SHR_TERM14_DEF_ADR     27       /* Assign Shared Printer for Terminal No.14 */
#define SHR_TERM14_ALT_ADR     28       /* Assign Alternative Printer for Terminal No.14 */
#define SHR_TERM15_DEF_ADR     29       /* Assign Shared Printer for Terminal No.15 */
#define SHR_TERM15_ALT_ADR     30       /* Assign Alternative Printer for Terminal No.15 */
#define SHR_TERM16_DEF_ADR     31       /* Assign Shared Printer for Terminal No.16 */
#define SHR_TERM16_ALT_ADR     32       /* Assign Alternative Printer for Terminal No.16 */
#define SHR_KP1_DEF_ADR        33       /* Assign Shared Printer for Kithchen Printer No.1 */
#define SHR_KP2_DEF_ADR        34       /* Assign Shared Printer for Kithchen Printer No.2 */
#define SHR_KP3_DEF_ADR        35       /* Assign Shared Printer for Kithchen Printer No.3 */
#define SHR_KP4_DEF_ADR        36       /* Assign Shared Printer for Kithchen Printer No.4 */
#define SHR_KP5_DEF_ADR        37       /* Assign Shared Printer for Kithchen Printer No.5 */
#define SHR_KP6_DEF_ADR        38       /* Assign Shared Printer for Kithchen Printer No.6 */
#define SHR_KP7_DEF_ADR        39       /* Assign Shared Printer for Kithchen Printer No.7 */
#define SHR_KP8_DEF_ADR        40       /* Assign Shared Printer for Kithchen Printer No.8 */
#define SHR_COM_KP1_DEF_ADR    41       /* Assign Com Port Printer for Kithchen Printer No.1, 2172 */
#define SHR_COM_KP2_DEF_ADR    42       /* Assign Com Port Printer for Kithchen Printer No.2, 2172 */
#define SHR_COM_KP3_DEF_ADR    43       /* Assign Com Port Printer for Kithchen Printer No.3, 2172 */
#define SHR_COM_KP4_DEF_ADR    44       /* Assign Com Port Printer for Kithchen Printer No.4, 2172 */
#define SHR_COM_KP5_DEF_ADR    45       /* Assign Com Port Printer for Kithchen Printer No.5, 2172 */
#define SHR_COM_KP6_DEF_ADR    46       /* Assign Com Port Printer for Kithchen Printer No.6, 2172 */
#define SHR_COM_KP7_DEF_ADR    47       /* Assign Com Port Printer for Kithchen Printer No.7, 2172 */
#define SHR_COM_KP8_DEF_ADR    48       /* Assign Com Port Printer for Kithchen Printer No.8, 2172 */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define SHR_ADR_MAX       MAX_SHAREDPRT_SIZE     /* Max. Number of This Address */

/*------------------------------------------------------------------------*\
*   Define - Hotel Id Address (PRG #54)

	see CPMDATA ParaHotelId in UNINIRAM UNINT Para.
\*------------------------------------------------------------------------*/

#define HOTEL_ID_ADR           1         /* Hotel Id Address */
#define HOTEL_SLD_ADR          2         /* SLD No. Address */
#define HOTEL_PDT_ADR          3         /* Product Id Address */
#define HOTEL_TEND1_ADR        4         /* Tender 1 Action No */
#define HOTEL_TEND2_ADR        5         /* Tender 2 Action No */
#define HOTEL_TEND3_ADR        6         /* Tender 3 Action No */
#define HOTEL_TEND4_ADR        7         /* Tender 4 Action No */
#define HOTEL_TEND5_ADR        8         /* Tender 5 Action No */
#define HOTEL_TEND6_ADR        9         /* Tender 6 Action No */
#define HOTEL_TEND7_ADR       10         /* Tender 7 Action No */
#define HOTEL_TEND8_ADR       11         /* Tender 8 Action No */
#define HOTEL_TEND9_ADR       12         /* Tender 9 Action No */
#define HOTEL_TEND10_ADR      13         /* Tender 10 Action No */
#define HOTEL_TEND11_ADR      14         /* Tender 11 Action No */
#define HOTEL_TEND12_ADR      15         /* Tender 12 Action No */
#define HOTEL_TEND13_ADR      16         /* Tender 13 Action No */
#define HOTEL_TEND14_ADR      17         /* Tender 14 Action No */
#define HOTEL_TEND15_ADR      18         /* Tender 15 Action No */
#define HOTEL_TEND16_ADR      19         /* Tender 16 Action No */
#define HOTEL_TEND17_ADR      20         /* Tender 17 Action No */
#define HOTEL_TEND18_ADR      21         /* Tender 18 Action No */
#define HOTEL_TEND19_ADR      22         /* Tender 19 Action No */
#define HOTEL_TEND20_ADR      23         /* Tender 20 Action No */
#define HOTEL_APVL_CODE_ADR	  24         /* Approval Code       */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define HOTELID_ADR_MAX       24          /* HOTEL ID MAX ADDRESS */

/*------------------------------------------------------------------------*\
*   Define - 24 Character Mnemonics Address (PRG #57), P57 Mnemonics
*
*	see ParaChar24 in UNINIRAM UNINT Para and use of CLASS_PARACHAR24 with CliParaAllRead()
\*------------------------------------------------------------------------*/

#define CH24_1STVAL_ADR         1        /* 1st Validation Header */
#define CH24_2NDVAL_ADR         2        /* 2nd Validation Header */
#define CH24_3RDVAL_ADR         3        /* 3rd Validation Header */
#define CH24_NXTMSG_ADR         4        /* Continue to Next Message Slip */
#define CH24_TRNGHED_ADR        5        /* Training Mode Header Print */
#define CH24_PVOID_ADR          6        /* Preselect Void Transaction Header */
#define CH24_KTCHN_ADR          7        /* Miss Transaction Kitchen Information Print Marker */
#define CH24_1STRH_ADR          8        /* Receipt Header 1st Line */
#define CH24_2NDRH_ADR          9        /* Receipt Header 2nd Line */
#define CH24_3RDRH_ADR         10        /* Receipt Header 3rd Line */
#define CH24_4THRH_ADR         11        /* Receipt Header 4th Line */
#define CH24_POSTR_ADR         12        /* post receipt */
#define CH24_MONEY_ADR         13        /* Money Decleration Header 2172 */
#define CH24_TERM_DESCR_ADR    14        /* Unique Terminal descriptive name, Amtrak */
#define CH24_1STCE_ADR         15        /* Check Endorsement 1st Line  DTREE#2 */
#define CH24_2NDCE_ADR         16        /* Check Endorsement 2nd Line  DTREE#2 */
#define CH24_STRFWRD		   17		 //Store and forward mnemonics
#define CH24_PRETURN_ADR       18        /* Preselect Return Transaction Header */
#define CH24_ORGTRAN_ADR       19        /* Header for originating unique identifier for a Return Transaction Header */
#define CH24_JCSUMTITLE_ADR    20        /* Job cost summary title for LePeeps SR07 */
#define CH24_PEXCHANGE_ADR     21        /* Preselect Exchange Transaction Header */
#define CH24_PREFUND_ADR       22        /* Preselect Refund Transaction Header */
#define CH24_PLU_TICKET_01     23        /* Preselect Refund Transaction Header */
#define CH24_PLU_TICKET_02     24        /* Preselect Refund Transaction Header */

#define CH24_ADR_MAX       MAX_CH24_NO    /* Max Address */



/*------------------------------------------------------------------------*\
*   Define - Total Key Type Assignment Address (PRG #60)
*
*	see ParaTtlKeyTyp in UNINIRAM UNINT Para.
*	see also function ItemTotalTypeIndex() which uses these for address
*	calculation for CliParaAllRead() of CLASS_PARATTLKEYTYP 
\*------------------------------------------------------------------------*/

#define TLTY_NO3_ADR            1        /* Total #3 */
#define TLTY_NO4_ADR            2        /* Total #4 */
#define TLTY_NO5_ADR            3        /* Total #5 */
#define TLTY_NO6_ADR            4        /* Total #6 */
#define TLTY_NO7_ADR            5        /* Total #7 */
#define TLTY_NO8_ADR            6        /* Total #8 */
#define TLTY_NO10_ADR           7        /* Total #10, Total #9 does not have an entry */
#define TLTY_NO11_ADR           8        /* Total #11 */

#define TLTY_ADR_MAX     MAX_KEYTYPE_SIZE       /* Max Address */



/*------------------------------------------------------------------------*\
*   Define - Total Key Control Address (PRG #61)
*	see ParaTtlKeyCtl in UNINIRAM UNINT Para.
*   used CliParaAllRead() of CLASS_PARATTLKEYCTL.
\*------------------------------------------------------------------------*/

#define TLCT_NO1_ADR            1        /* Total #1 */
#define TLCT_NO2_ADR            2        /* Total #2 */
#define TLCT_NO3_ADR            3        /* Total #3 */
#define TLCT_NO4_ADR            4        /* Total #4 */
#define TLCT_NO5_ADR            5        /* Total #5 */
#define TLCT_NO6_ADR            6        /* Total #6 */
#define TLCT_NO7_ADR            7        /* Total #7 */
#define TLCT_NO8_ADR            8        /* Total #8 */
#define TLCT_NO9_ADR            9        /* Total #9 */
#define TLCT_N10_ADR           10        /* Total #10 */
#define TLCT_N11_ADR           11        /* Total #11 */
#define TLCT_N12_ADR           12        /* Total #12 */
#define TLCT_N13_ADR           13        /* Total #13 */
#define TLCT_N14_ADR           14        /* Total #14 */
#define TLCT_N15_ADR           15        /* Total #15 */
#define TLCT_N16_ADR           16        /* Total #16 */
#define TLCT_N17_ADR           17        /* Total #17 */
#define TLCT_N18_ADR           18        /* Total #18 */
#define TLCT_N19_ADR           19        /* Total #19 */
#define TLCT_N20_ADR           20        /* Total #20 */

/* Warning !!! You have to change this Define When Above Max. address is changed */
#define TLCT_ADR_MAX          MAX_TTLKEY_NO        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Total Key Control Field (PRG #61)
\*------------------------------------------------------------------------*/
#define TLCT_NO1_FLD            1        /* Field #1 for CLASS_PARATTLKEYCTL */
#define TLCT_FLD_MAX            12       /* Max Field for CLASS_PARATTLKEYCTL */

/*------------------------------------------------------------------------*\
*   Define -Auto Combination Coupon Control Field (PRG #61)
\*------------------------------------------------------------------------*/
#define AUTOCPN_AMT_ADR_MAX      2    /* Max Address for CLASS_PARAAUTOCPN */


/*------------------------------------------------------------------------*\
*   Define - Tender Key Parameter Address (PRG #62)
\*------------------------------------------------------------------------*/

#define TEND_NO1_ADR            1        /* Tender #1 */
#define TEND_NO2_ADR            2        /* Tender #2 */
#define TEND_NO3_ADR            3        /* Tender #3 */
#define TEND_NO4_ADR            4        /* Tender #4 */
#define TEND_NO5_ADR            5        /* Tender #5 */
#define TEND_NO6_ADR            6        /* Tender #6 */
#define TEND_NO7_ADR            7        /* Tender #7 */
#define TEND_NO8_ADR            8        /* Tender #8 */
#define TEND_NO9_ADR            9        /* Tender #9 */
#define TEND_NO10_ADR           10       /* Tender #10 */
#define TEND_NO11_ADR           11       /* Tender #11 */
#define TEND_NO12_ADR           12       /* Tender #12 */
#define TEND_NO13_ADR           13       /* Tender #13 */
#define TEND_NO14_ADR           14       /* Tender #14 */
#define TEND_NO15_ADR           15       /* Tender #15 */
#define TEND_NO16_ADR           16       /* Tender #16 */
#define TEND_NO17_ADR           17       /* Tender #17 */
#define TEND_NO18_ADR           18       /* Tender #18 */
#define TEND_NO19_ADR           19       /* Tender #19 */
#define TEND_NO20_ADR           20       /* Tender #20 */


/* Warning !!! You have to change this Define When Above Max. address is changed */
#define TEND_ADR_MAX          MAX_TEND_NO       /* Max Address */


// Transaction types and Transactions codes for DataCap and other payment terminals
#define TENDERKEY_TRANTYPE_CREDIT        1    // Tender key generates a Credit transaction type
#define TENDERKEY_TRANTYPE_DEBIT         2    // Tender key generates a Debit transaction type
#define TENDERKEY_TRANTYPE_EBT           3    // Tender key generates a EBT transaction type
#define TENDERKEY_TRANTYPE_CHARGE        4    // Tender key generates a Cash transaction type
#define TENDERKEY_TRANTYPE_CHECK         5    // Tender key generates a Check transaction type for check transactions
#define TENDERKEY_TRANTYPE_PREPAID	     6	  // Tender key generates a PrePaid transaction type (Gift Cards)
#define TENDERKEY_TRANTYPE_FREEDOMPAY	 7	  // Tender key generates a SCF_DEVICENAME_EPTEXDLL_FREEDOMPAY
#define TENDERKEY_TRANTYPE_EPD			 8	  // Tender key generates a EPD or Employee Payroll Deduction request
#define TENDERKEY_TRANTYPE_EMEAL		 9	  // Tender key generates a E-Meal or Electronic Meal (VCS)
#define TENDERKEY_TRANTYPE_BATCH		10	  // Tender key generates a NetePay server batch request
#define TENDERKEY_TRANTYPE_CREDIT_EMV	11	  // Tender key generates a EMV type of Credit transaction type.
#define TENDERKEY_TRANTYPE_DEBIT_EMV	12	  // Tender key generates a EMV type of Debit transaction type.
#define TENDERKEY_TRANTYPE_EMV_PARAM	13	  // Special transaction type to implement EMVParamDownload to EMV payment Terminal, see EEPT_FC2_ACTION_EPT

#define TENDERKEY_TRANCODE_SALE					1    // Tender key generates a Sale transaction code
#define TENDERKEY_TRANCODE_RETURN				2    // Tender key generates a Return transaction code
#define TENDERKEY_TRANCODE_VOID_SALE			3    // Tender key generates a Void Sale transaction code
#define TENDERKEY_TRANCODE_VOID_RETURN			4    // Tender key generates a Void Return transaction code
#define TENDERKEY_TRANCODE_ADJUST				5    // Tender key generates a Adjust transaction code
#define TENDERKEY_TRANCODE_VOUCHER				6    // Tender key generates a Voucher transaction code
#define	TENDERKEY_TRANCODE_CASHSALE				7	  // Tender key generates a Cash Sale transaction code, for EBT
#define	TENDERKEY_TRANCODE_CASHRETURN			8	  // Tender key generates a Cash Return transaction code, for EBT
#define	TENDERKEY_TRANCODE_FOODSTAMPSALE		9	  // Tender key generates a Food Stamp Sale transaction code, for EBT
#define	TENDERKEY_TRANCODE_FOODSTAMPRETURN		10	  // Tender key generates a Food Stamp Return transaction code, for EBT
#define	TENDERKEY_TRANCODE_DRIVERLICENSE_MANUAL	11	  // Tender key generates a Check Drivers License manual entry transaction code
#define	TENDERKEY_TRANCODE_DRIVERLICENSE_STRIPE	12	  // Tender key generates a Check Drivers License stripe entry transaction code
#define	TENDERKEY_TRANCODE_MICR_READER			13	  // Tender key generates a Check MICR Reader transaction code
#define	TENDERKEY_TRANCODE_MICR_MANUAL			14	  // Tender key generates a Check MICR Manual transaction code
#define TENDERKEY_TRANCODE_PREAUTH				15	  // Tender key generates a Credit Pre-Authorization code
#define	TENDERKEY_TRANCODE_PREAUTHCAPTURE		16	  // Tender key generates a Credit Pre-Authorization Capture code
#define	TENDERKEY_TRANCODE_ISSUE				17	  // Tender key generates a PrePaid issue transaction code
#define TENDERKEY_TRANCODE_RELOAD				18	  // Tender key generates a PrePaid reload transaction code (add value to gift card)
#define TENDERKEY_TRANCODE_VOID_RELOAD			19	  // Tender key generates a PrePaid void reload transaction code (remove value from gift card)
#define TENDERKEY_TRANCODE_BALANCE				20	  // Tender key generates a Balance transaction code
#define TENDERKEY_TRANCODE_NONSFSALE			21	  // Tender key generates a NoNSFSale transaction code
#define TENDERKEY_TRANCODE_VOID_ISSUE			22	  // Tender key generates a Void Issue transaction code	
#define TENDERKEY_TRANCODE_EPD_PAYMENT			23	  // Tender key generates a payment transaction code for Employee Payroll Deduction
#define TENDERKEY_TRANCODE_EPD_VOIDPAYMENT		24	  // Tender key generates a void payment transaction code for Employee Payroll Deduction
#define TENDERKEY_TRANCODE_EPD_LOOKUPNAMESS4	25	  // Tender key generates a lookup name transaction code for Employee Payroll Deduction
#define TENDERKEY_TRANCODE_EPD_LOOKUPEPDACCTNO	26	  // Tender key generates a lookup name transaction code for Employee Payroll Deduction
#define TENDERKEY_TRANCODE_BATCH_ITEMDETAIL		27	  // Tender key generates an Item Detail request for NetePay server admin request 
#define TENDERKEY_TRANCODE_BATCH_SUMMARY		28	  // Tender key generates an Item Detail request for NetePay server admin request 

#define TENDERKEY_TRANCODE_TOKENIZED_OPTION				0x40		// option flag indicating to force token usage with RecordNo.
#define TENDERKEY_TRANCODE_PREAUTH_TOKENIZED			(0x40+15)	// Tender key generates tokenized Credit Pre-Authorization code used with AdjustByRecordNo not Preauth Capture
#define TENDERKEY_TRANCODE_PREAUTHCAPTURE_TOKENIZED		(0x40+16)	// Tender key generates tokenized Credit Pre-Authorization Capture code used with AdjustByRecordNo not Preauth Capture

#define TENDERKEY_CHECK_TENDMDC01         0x00000001
#define TENDERKEY_CHECK_TENDMDC02         0x00000002
#define TENDERKEY_CHECK_TENDMDC03         0x00000004
#define TENDERKEY_CHECK_TENDMDC04         0x00000008
#define TENDERKEY_CHECK_TENDMDC05         0x00000010
#define TENDERKEY_CHECK_TENDMDC06         0x00000020
#define TENDERKEY_CHECK_TENDMDC07         0x00000040
#define TENDERKEY_CHECK_TENDMDC08         0x00000080
#define TENDERKEY_CHECK_TENDMDC09         0x00000100
#define TENDERKEY_CHECK_TENDMDC010        0x00000200

// These defines are for accessing multi-bit values stored in ulFlags0 of a TENDERKEYINFO struct
// Notice requires a pointer so if using a non-pointer use the addressof operator (&).
#define TENDERKEY_CHECK_RADIO_GET_AMNT_ENTRY(x)  ((x)->ulFlags0 & 0x00000003)
#define TENDERKEY_CHECK_RADIO_SET_AMNT_ENTRY(x,v)  ((x)->ulFlags0 = ((x)->ulFlags0 & 0xFFFFFFFC) | ((v) & 0x00000003))


/*------------------------------------------------------------------------*\
*   SUPERVISOR MODE DEFINE
\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
*   Define - Set PLU No. On Menu Page Address (A/C 4)
*            The AC 4 Menu Page address functionality only applies to keyboard
*            type systems and not to Touchscreen.
*            It was originally implemented for NCR 2172 to allow for a keyboard
*            to be used with several different virtual keyboards by allowing
*            the operator to do a menu page change switching from one keyboard
*            layout to another.
\*------------------------------------------------------------------------*/
#if defined(POSSIBLE_DEAD_CODE)
// none of these defined constants appears to be used anywhere in GenPOS source.
//   Richard Chambers, Apr-24-2015 working with GenPOS Rel 2.2.1
#define PLUPG_PAGE1_ADR         1        /* Menu Page 1 Rel 1.4 and earlier for NCR 2172 and NCR 7448 */
#define PLUPG_PAGE2_ADR         2        /* Menu Page 2 Rel 1.4 and earlier for NCR 2172 and NCR 7448 */
#define PLUPG_PAGE3_ADR         3        /* Menu Page 3 Rel 1.4 and earlier for NCR 2172 and NCR 7448 */
#define PLUPG_PAGE4_ADR         4        /* Menu Page 4 Rel 1.4 and earlier for NCR 2172 and NCR 7448 */
#define PLUPG_PAGE5_ADR         5        /* Menu Page 5 Rel 1.4 and earlier for NCR 2172 and NCR 7448 */
#define PLUPG_PAGE6_ADR         6        /* Menu Page 6 Rel 1.4 and earlier for NCR 2172 and NCR 7448 */
#define PLUPG_PAGE7_ADR         7        /* Menu Page 7 Rel 1.4 and earlier for NCR 2172 and NCR 7448 */
#define PLUPG_PAGE8_ADR         8        /* Menu Page 8 Rel 1.4 and earlier for NCR 2172 and NCR 7448 */
#define PLUPG_PAGE9_ADR         9        /* Menu Page 9 Rel 1.4 and earlier for NCR 2172 and NCR 7448 */
#define PLUPG_PAGE10_ADR		10		 //Menu Page 10 RPH 12-2-3 TOUCHSCREEN only and not keyboard
#define PLUPG_PAGE11_ADR		11		 //Menu Page 11 RPH 12-2-3 TOUCHSCREEN only and not keyboard

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define PLUPG_ADR_MAX           9        /* Max Address */
#endif

/*------------------------------------------------------------------------*\
*   Define - Set PLU No. On Menu Page Address (A/C 5)
\*------------------------------------------------------------------------*/

#define CTLPG_STAND_ADR         1        /* Standard Page No. */
#define CTLPG_PAGE1_ADR         2        /* Menu Page 1 */
#define CTLPG_PAGE2_ADR         3        /* Menu Page 2 */
#define CTLPG_PAGE3_ADR         4        /* Menu Page 3 */
#define CTLPG_PAGE4_ADR         5        /* Menu Page 4 */
#define CTLPG_PAGE5_ADR         6        /* Menu Page 5 */
#define CTLPG_PAGE6_ADR         7        /* Menu Page 6 */
#define CTLPG_PAGE7_ADR         8        /* Menu Page 7 */
#define CTLPG_PAGE8_ADR         9        /* Menu Page 8 */
#define CTLPG_PAGE9_ADR        10        /* Menu Page 9 */
#define CTLPG_PAGE10_ADR	   11		 // Menu Page 10 RPH 12-2-3 TOUCHSCREEN
#define CTLPG_PAGE11_ADR	   12		 // Menu Page 11 RPH 12-2-3 TOUCHSCREEN
#define CTLPG_ADJ_PAGE1_ADR    13        /* Adjective Menu Page 1, 2172 */
#define CTLPG_ADJ_PAGE2_ADR    14        /* Adjective Menu Page 2, 2172 */
#define CTLPG_ADJ_PAGE3_ADR    15        /* Adjective Menu Page 3, 2172 */
#define CTLPG_ADJ_PAGE4_ADR    16        /* Adjective Menu Page 4, 2172 */
#define CTLPG_ADJ_PAGE5_ADR    17        /* Adjective Menu Page 5, 2172 */
#define CTLPG_ADJ_PAGE6_ADR    18        /* Adjective Menu Page 6, 2172 */
#define CTLPG_ADJ_PAGE7_ADR    19        /* Adjective Menu Page 7, 2172 */
#define CTLPG_ADJ_PAGE8_ADR    20        /* Adjective Menu Page 8, 2172 */
#define CTLPG_ADJ_PAGE9_ADR    21        /* Adjective Menu Page 9, 2172 */
#define CTLPG_ADJ_PAGE10_ADR   22        /* Adjective Menu Page 10, RPH 12-2-3 TOUCHSCREEN*/
#define CTLPG_ADJ_PAGE11_ADR   23        /* Adjective Menu Page 11, RPH 12-2-3 TOUCHSCREEN/

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define CTLPG_ADR_MAX          23        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Manual Alternative Kitchen Printer Assignment Address (A/C 6)
\*------------------------------------------------------------------------*/

#define MAKTN_SOURCE1_ADR       1        /* Source Kitchen Printer #1 */
#define MAKTN_SOURCE2_ADR       2        /* Source Kitchen Printer #2 */
#define MAKTN_SOURCE3_ADR       3        /* Source Kitchen Printer #3 */
#define MAKTN_SOURCE4_ADR       4        /* Source Kitchen Printer #4 */
#define MAKTN_SOURCE5_ADR       5        /* Source Kitchen Printer #5 */
#define MAKTN_SOURCE6_ADR       6        /* Source Kitchen Printer #6 */
#define MAKTN_SOURCE7_ADR       7        /* Source Kitchen Printer #7 */
#define MAKTN_SOURCE8_ADR       8        /* Source Kitchen Printer #8 */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define MAKTN_ADR_MAX           MAX_DEST_SIZE        /* Max Address for ParaManuAltKitch[] */

/*------------------------------------------------------------------------*\
*   Define - Cashier A/B Keys Assignment Address (A/C 7)
\*------------------------------------------------------------------------*/

#define ASN_AKEY_ADR            1        /* Cashier A Key, COMMON_DRAWER_A */
#define ASN_BKEY_ADR            2        /* Cashier B Key, COMMON_DRAWER_B */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define ASN_ADR_MAX             2        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Set Date And Time Address (A/C 17)
\*------------------------------------------------------------------------*/

#define TOD_DATE_ADR            1        /* Set Date */
#define TOD_TIME_ADR            2        /* Set Time */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define TOD_ADR_MAX             2        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Set Promotional Pricing Item Address (A/C 71)
\*------------------------------------------------------------------------*/

#define PPI_PRICE1_ADR          1        /* Price Level 1 */
#define PPI_PRICE2_ADR          2        /* Price Level 2 */
#define PPI_PRICE3_ADR          3        /* Price Level 3 */
#define PPI_PRICE4_ADR          4        /* Price Level 4 */
#define PPI_PRICE5_ADR          5        /* Price Level 5 */
#define PPI_PRICE6_ADR          6        /* Price Level 6 */
#define PPI_PRICE7_ADR          7        /* Price Level 7 */
#define PPI_PRICE8_ADR          8        /* Price Level 8 */
#define PPI_PRICE9_ADR          9        /* Price Level 9 */
#define PPI_PRICE10_ADR        10        /* Price Level 10 */
#define PPI_PRICE11_ADR        11        /* Price Level 11 */
#define PPI_PRICE12_ADR        12        /* Price Level 12 */
#define PPI_PRICE13_ADR        13        /* Price Level 13 */
#define PPI_PRICE14_ADR        14        /* Price Level 14 */
#define PPI_PRICE15_ADR        15        /* Price Level 15 */
#define PPI_PRICE16_ADR        16        /* Price Level 16 */
#define PPI_PRICE17_ADR        17        /* Price Level 17 */
#define PPI_PRICE18_ADR        18        /* Price Level 18 */
#define PPI_PRICE19_ADR        19        /* Price Level 19 */
#define PPI_PRICE20_ADR        20        /* Price Level 20 */
#define PPI_PRICE21_ADR        21        /* Price Level 21 */ // New PPI addresses - CSMALL
#define PPI_PRICE22_ADR        22        /* Price Level 22 */
#define PPI_PRICE23_ADR        23        /* Price Level 23 */
#define PPI_PRICE24_ADR        24        /* Price Level 24 */
#define PPI_PRICE25_ADR        25        /* Price Level 25 */
#define PPI_PRICE26_ADR        26        /* Price Level 26 */
#define PPI_PRICE27_ADR        27        /* Price Level 27 */
#define PPI_PRICE28_ADR        28        /* Price Level 28 */
#define PPI_PRICE29_ADR        29        /* Price Level 29 */
#define PPI_PRICE30_ADR        30        /* Price Level 30 */
#define PPI_PRICE31_ADR        31        /* Price Level 31 */
#define PPI_PRICE32_ADR        32        /* Price Level 32 */
#define PPI_PRICE33_ADR        33        /* Price Level 33 */
#define PPI_PRICE34_ADR        34        /* Price Level 34 */
#define PPI_PRICE35_ADR        35        /* Price Level 35 */
#define PPI_PRICE36_ADR        36        /* Price Level 36 */
#define PPI_PRICE37_ADR        37        /* Price Level 37 */
#define PPI_PRICE38_ADR        38        /* Price Level 38 */
#define PPI_PRICE39_ADR        39        /* Price Level 39 */
#define PPI_PRICE40_ADR        40        /* Price Level 40 */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define PPI_ADR_MAX            MAX_PARAPPI_SIZE        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Set Rounding Table Address (A/C 84)
\*------------------------------------------------------------------------*/

#define RND_ITEMDISC1_ADR       1        /* Item Discount #1 */
#define RND_ITEMDISC2_ADR       1        /* Item Discount #2 (same as RND_ITEMDISC1_ADR) */
#define RND_REGDISC1_ADR        2        /* Regular Discount #1 */
#define RND_REGDISC2_ADR        2        /* Regular Discount #2 */
#define RND_CHGTIP1_ADR         3        /* Charge Tips/Service #1 */
#define RND_CHGTIP2_ADR         3        /* Charge Tips/Service #2 */
#define RND_TAX1_ADR            4        /* Tax #1 */
#define RND_TAX2_ADR            4        /* Tax #2 */
#define RND_FOREIGN1_1_ADR      5        /* Foreign Currency Rate #1_1 */
#define RND_FOREIGN1_2_ADR      5        /* Foreign Currency Rate #1_2 */
#define RND_FOREIGN2_1_ADR      6        /* Foreign Currency Rate #2_1 */
#define RND_FOREIGN2_2_ADR      6        /* Foreign Currency Rate #2_2 */
#define RND_FOREIGN3_1_ADR      7        /* Foreign Currency Rate #3_1, 2172 */
#define RND_FOREIGN3_2_ADR      7        /* Foreign Currency Rate #3_2, 2172 */
#define RND_FOREIGN4_1_ADR      8        /* Foreign Currency Rate #4_1, 2172 */
#define RND_FOREIGN4_2_ADR      8        /* Foreign Currency Rate #4_2, 2172 */
#define RND_FOREIGN5_1_ADR      9        /* Foreign Currency Rate #5_1, 2172 */
#define RND_FOREIGN5_2_ADR      9        /* Foreign Currency Rate #5_2, 2172 */
#define RND_FOREIGN6_1_ADR     10        /* Foreign Currency Rate #6_1, 2172 */
#define RND_FOREIGN6_2_ADR     10        /* Foreign Currency Rate #6_2, 2172 */
#define RND_FOREIGN7_1_ADR     11        /* Foreign Currency Rate #7_1, 2172 */
#define RND_FOREIGN7_2_ADR     11        /* Foreign Currency Rate #7_2, 2172 */
#define RND_FOREIGN8_1_ADR     12        /* Foreign Currency Rate #8_1, 2172 */
#define RND_FOREIGN8_2_ADR     12        /* Foreign Currency Rate #8_2, 2172 */
#define RND_TOTAL1_ADR         13        /* Local Currency/Total #1     2172 */
#define RND_TOTAL2_ADR         13        /* Local Currency/Total #2     2172 */

// Following defines are special purpose defines to indicate a rounding operation that is other
// than using the rounding data in the rounding tables. See _RflGetRoundingTable().
#define RND_RND_UP            100        /* _RflGetRoundingTable() special round address RFL_RND_UP - round up */
#define RND_RND_DOWN          101        /* _RflGetRoundingTable() special round address RFL_RND_DOWN - round down */
#define RND_RND_NORMAL        102        /* _RflGetRoundingTable() special round address RFL_RND_NORMAL - round */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define RND_ADR_MAX            MAX_RNDTBL_SIZE        /* Max Address  */

/*------------------------------------------------------------------------*\
*   Define - Set Discount / Bonus % Rates Address (A/C 86)
\*------------------------------------------------------------------------*/

#define RATE_MTRID_ADR           1        /* Item Discount (Motorized) */
#define RATE_MODID_ADR           2        /* Item Discount (Modified) */
#define RATE_RDISC1_ADR          3        /* Regular Discount #1 */
#define RATE_RDISC2_ADR          4        /* Regular Discount #2 */
#define RATE_BONUS1_ADR          5        /* Bonus #1 */
#define RATE_BONUS2_ADR          6        /* Bonus #2 */
#define RATE_BONUS3_ADR          7        /* Bonus #3 */
#define RATE_BONUS4_ADR          8        /* Bonus #4 */
#define RATE_BONUS5_ADR          9        /* Bonus #5 */
#define RATE_BONUS6_ADR         10        /* Bonus #6 */
#define RATE_BONUS7_ADR         11        /* Bonus #7 */
#define RATE_BONUS8_ADR         12        /* Bonus #8 */
#define RATE_CRG_TIP_ADR        13        /* Charge Tips */
#define RATE_CALT_ADR           14        /* Calculated Tips */
#define RATE_RDISC3_ADR         15        /* Regular Discount #3 */
#define RATE_RDISC4_ADR         16        /* Regular Discount #4 */
#define RATE_RDISC5_ADR         17        /* Regular Discount #5 */
#define RATE_RDISC6_ADR         18        /* Regular Discount #6 */
#define RATE_CRG_TIP2_ADR       19        /* Charge Tips #2, V3.3 */
#define RATE_CRG_TIP3_ADR       20        /* Charge Tips #3, V3.3 */
#define RATE_ITMDISC3_ADR		21        // Item Discount # 3, CLASS_UIITEMDISC1, uchDiscountNo == 3
#define RATE_ITMDISC4_ADR		22
#define RATE_ITMDISC5_ADR		23
#define RATE_ITMDISC6_ADR		24
#define RATE_BONUS9_ADR			25
#define RATE_BONUS10_ADR		26
#define RATE_BONUS11_ADR		27
#define RATE_BONUS12_ADR		28
#define RATE_BONUS13_ADR		29
#define RATE_BONUS14_ADR		30
#define RATE_BONUS15_ADR		31
#define RATE_BONUS16_ADR		32
#define RATE_BONUS17_ADR		33
#define RATE_BONUS18_ADR		34
#define RATE_BONUS19_ADR		35
#define RATE_BONUS20_ADR		36
#define RATE_BONUS21_ADR		37
#define RATE_BONUS22_ADR		38
#define RATE_BONUS23_ADR		39
#define RATE_BONUS24_ADR		40
#define RATE_BONUS25_ADR		41
#define RATE_BONUS26_ADR		42
#define RATE_BONUS27_ADR		43
#define RATE_BONUS28_ADR		44
#define RATE_BONUS29_ADR		45
#define RATE_BONUS30_ADR		46
#define RATE_BONUS31_ADR		47
#define RATE_BONUS32_ADR		48	
#define RATE_BONUS33_ADR		49
#define RATE_BONUS34_ADR		50
#define RATE_BONUS35_ADR		51
#define RATE_BONUS36_ADR		52
#define RATE_BONUS37_ADR		53
#define RATE_BONUS38_ADR		54
#define RATE_BONUS39_ADR		55
#define RATE_BONUS40_ADR		56
#define RATE_BONUS41_ADR		57
#define RATE_BONUS42_ADR		58
#define RATE_BONUS43_ADR		59
#define RATE_BONUS44_ADR		60
#define RATE_BONUS45_ADR		61
#define RATE_BONUS46_ADR		62
#define RATE_BONUS47_ADR		63
#define RATE_BONUS48_ADR		64
#define RATE_BONUS49_ADR		65
#define RATE_BONUS50_ADR		66
#define RATE_BONUS51_ADR		67
#define RATE_BONUS52_ADR		68
#define RATE_BONUS53_ADR		69
#define RATE_BONUS54_ADR		70
#define RATE_BONUS55_ADR		71
#define RATE_BONUS56_ADR		72
#define RATE_BONUS57_ADR		73
#define RATE_BONUS58_ADR		74
#define RATE_BONUS59_ADR		75
#define RATE_BONUS60_ADR		76
#define RATE_BONUS61_ADR		77
#define RATE_BONUS62_ADR		78
#define RATE_BONUS63_ADR		79
#define RATE_BONUS64_ADR		80
#define RATE_BONUS65_ADR		81
#define RATE_BONUS66_ADR		82
#define RATE_BONUS67_ADR		83
#define RATE_BONUS68_ADR		84
#define RATE_BONUS69_ADR		85
#define RATE_BONUS70_ADR		86
#define RATE_BONUS71_ADR		87
#define RATE_BONUS72_ADR		88
#define RATE_BONUS73_ADR		89
#define RATE_BONUS74_ADR		90
#define RATE_BONUS75_ADR		91
#define RATE_BONUS76_ADR		92	
#define RATE_BONUS77_ADR		93
#define RATE_BONUS78_ADR		94
#define RATE_BONUS79_ADR		95
#define RATE_BONUS80_ADR		96
#define RATE_BONUS81_ADR		97
#define RATE_BONUS82_ADR		98
#define RATE_BONUS83_ADR		99
#define RATE_BONUS84_ADR		100
#define RATE_BONUS85_ADR		101
#define RATE_BONUS86_ADR		102
#define RATE_BONUS87_ADR		103
#define RATE_BONUS88_ADR		104
#define RATE_BONUS89_ADR		105
#define RATE_BONUS90_ADR		106
#define RATE_BONUS91_ADR		107
#define RATE_BONUS92_ADR		108
#define RATE_BONUS93_ADR		109
#define RATE_BONUS94_ADR		110
#define RATE_BONUS95_ADR		111
#define RATE_BONUS96_ADR		112
#define RATE_BONUS97_ADR		113
#define RATE_BONUS98_ADR		114
#define RATE_BONUS99_ADR		115
#define RATE_BONUS100_ADR		116
#define RATE_FUTURE1_USE_ADR		117
#define RATE_FUTURE2_USE_ADR		118
#define RATE_FUTURE3_USE_ADR		119
#define RATE_FUTURE4_USE_ADR		120
/* Warning !!! You have to change this Define When Above Max. address is changed */

#define RATE_ADR_MAX            120        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Set Trailer Print for Soft Check Address (A/C 87)
\*------------------------------------------------------------------------*/

#define SOFTCHK_MSG1_ADR     1         /* trailer print for soft check message 1 */
#define SOFTCHK_MSG2_ADR     2         /* trailer print for soft check message 2 */
#define SOFTCHK_MSG3_ADR     3         /* trailer print for soft check message 3 */
#define SOFTCHK_MSG4_ADR     4         /* trailer print for soft check message 4 */
#define SOFTCHK_MSG5_ADR     5         /* trailer print for soft check message 5 */
#define SOFTCHK_EPT1_ADR     6         /* Line No. 1 for EPT TGrailer line  */
#define SOFTCHK_EPT2_ADR     7         /* Line No. 2 for EPT TGrailer line  */
#define SOFTCHK_EPT3_ADR     8         /* Line No. 3 for EPT TGrailer line  */
#define SOFTCHK_EPT4_ADR     9         /* Line No. 4 for EPT TGrailer line  */
#define SOFTCHK_EPT5_ADR     10        /* Line No. 5 for EPT TGrailer line  */
#define SOFTCHK_RTN1_ADR     11        /* Line No. 1 for Returns TGrailer line  */
#define SOFTCHK_RTN2_ADR     12        /* Line No. 2 for Returns TGrailer line  */
#define SOFTCHK_RTN3_ADR     13        /* Line No. 3 for Returns TGrailer line  */
#define SOFTCHK_RTN4_ADR     14        /* Line No. 4 for Returns TGrailer line  */
#define SOFTCHK_RTN5_ADR     15        /* Line No. 5 for Returns TGrailer line  */

#define SOFTCHK_ADR_MAX      MAX_SOFTCHK_NO        /* max address */

/*------------------------------------------------------------------------*\
*   Define - Set Slip Sales Promotion Message Address (A/C 87)
\*------------------------------------------------------------------------*/

/* #define  CHAR44_MSG1_ADR      1     */    /* sales promotion message 1 */
/* #define  CHAR44_MSG2_ADR      2     */    /* sales promotion message 2 */
/* #define  CHAR44_MSG3_ADR      3     */    /* sales promotion message 3 */
/* #define  CHAR44_MSG4_ADR      4     */    /* sales promotion message 4 */

/* #define  CHAR44_ADR_MAX       4     */    /* max address */

/*------------------------------------------------------------------------*\
*   Define - Set Sales Promotion Message Address (A/C 88)
\*------------------------------------------------------------------------*/

#define PRM_LINE1_ADR            1        /* Line No. 1 */
#define PRM_LINE2_ADR            2        /* Line No. 2 */
#define PRM_LINE3_ADR            3        /* Line No. 3 */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define PRM_ADR_MAX              3        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Set Currency Conversion Rate Address (A/C 89)

	see ParaCurrency in UNINIRAM UNINT Para.
\*------------------------------------------------------------------------*/

#define CNV_NO1_ADR              1        /* Rate 1 */
#define CNV_NO2_ADR              2        /* Rate 2 */
#define CNV_NO3_ADR              3        /* Rate 3, 2172 */
#define CNV_NO4_ADR              4        /* Rate 4, 2172 */
#define CNV_NO5_ADR              5        /* Rate 5, 2172 */
#define CNV_NO6_ADR              6        /* Rate 6, 2172 */
#define CNV_NO7_ADR              7        /* Rate 7, 2172 */
#define CNV_NO8_ADR              8        /* Rate 8, 2172 */

#define CNV_ADR_MAX        MAX_FC_SIZE



/*------------------------------------------------------------------------*\
*   Define - Set Tare Table Address (A/C 111, AC111)
*
*	see ParaTare in UNINIRAM UNINT Para as well as CLASS_UITARE usage.
*
*	tare is the weight of an empty container or vehicle.  GenPOS provides
*	for weighing the container along with the material and then deducting
*	the weight of the container from the total weight as reported by the
*	scale. The tare table provides a list of the weight of the standard
*   containers used. The weight must be in scale units.
*	
*	See FSC_TARE function key handling as well as MOD_READ_SCALE_AT_TARE status indicator.
*	A PLU may have a tare address specified as part of the PLU data or it may be marked
*	as Require Tare with status of PLU_REQ_TARE to force tare address entry when the PLU is rung up.
*   The PLU must also be marked as scalable, PLU_SCALABLE
\*------------------------------------------------------------------------*/

#define TARE_NO1_ADR             1
#define TARE_NO2_ADR             2
#define TARE_NO3_ADR             3
#define TARE_NO4_ADR             4
#define TARE_NO5_ADR             5
#define TARE_NO6_ADR             6
#define TARE_NO7_ADR             7
#define TARE_NO8_ADR             8
#define TARE_NO9_ADR             9
#define TARE_NO10_ADR           10
#define TARE_NO11_ADR           11
#define TARE_NO12_ADR           12
#define TARE_NO13_ADR           13
#define TARE_NO14_ADR           14
#define TARE_NO15_ADR           15
#define TARE_NO16_ADR           16
#define TARE_NO17_ADR           17
#define TARE_NO18_ADR           18
#define TARE_NO19_ADR           19
#define TARE_NO20_ADR           20
#define TARE_NO21_ADR           21
#define TARE_NO22_ADR           22
#define TARE_NO23_ADR           23
#define TARE_NO24_ADR           24
#define TARE_NO25_ADR           25
#define TARE_NO26_ADR           26
#define TARE_NO27_ADR           27
#define TARE_NO28_ADR			28
#define TARE_NO29_ADR           29
#define TARE_NO30_ADR           30

#define TARE_ADR_MAX      MAX_TARE_SIZE


/*------------------------------------------------------------------------*\
*   Define - Set Menu Array Address (A/C 116)
\*------------------------------------------------------------------------*/

#define SMENU_SETADR_MAX     20


/* Warning !!! You have to change this Define When Above Max. address is changed */

#define SMENU_ARYADR_MAX         5        /* Max Array Address */

/*------------------------------------------------------------------------*\
*   Define - Set Tax / Service / Reciprocal Rates Address (A/C 127)
*------------------------------------------------------------------------*/
                                         /*  US   /  CANADA                 */
#define TXRT_NO1_ADR            1        /* Tax#1 / Goods And Service Tax   */
#define TXRT_NO2_ADR            2        /* Tax#2 / Provincial Sales Tax #1 */
#define TXRT_NO3_ADR            3        /* Tax#3 / Provincial Sales Tax #2 */
#define TXRT_NO4_ADR            4        /*       / Provincial Sales Tax #3 */
#define TXRT_NO5_ADR            5        /*       / Tax Exempt Limit        */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define TXRT_ADR_MAX            5        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Set Misc Parameter Address (A/C 128)
*------------------------------------------------------------------------*/

#define MISC_FOODDENOMI_ADR     1        /* Food Stamp Denomination Limit */
#define MISC_FOODLIMIT_ADR      2        /* Food Stamp Purchase Limit */
#define MISC_TRANSCREDIT_ADR    3        /* Transaction Credit Limit */
#define MISC_CPNFACE2_ADR       4        /* Max. Amt.of CPN(face)value to double */
#define MISC_CPNFACE3_ADR       5        /* Max. Amt.of CPN(face)value to triple */
#define MISC_MAXCPNAMT_ADR      6        /* Max. Credit Amt. allowed/transaction   */
#define MISC_CASHLIMIT_ADR      7        /* Cash Drawer Limit */
#define MISC_CREDITFLOOR_ADR    8        /* floor amount below which no receipt printed  */
#define MISC_CREDITFLOORSTATUS_ADR      9        /* Status bits see A128_CC_ENABLE, A128_CC_MERC, A128_CC_CUST */
#define MISC_SF_SINGLE_ADR      10        /* Store and Forward single transaction limit */
#define MISC_SF_CUMMUL_ADR      11        /* Store and Forward cummulative transaction Limit */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define MISC_ADR_MAX            18        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Pig Rule Table Address (A/C 130)
*------------------------------------------------------------------------*/

#define PIG_CO_LIMIT_ADR        1        /* Counter Limit for Non GST Tax */
#define PIG_RULE1_ADR           2        /* Pig Rule #1 */
#define PIG_RULE2_ADR           3        /* Pig Rule #2 */
#define PIG_RULE3_ADR           4        /* Pig Rule #3 */
#define PIG_RULE4_ADR           5        /* Pig Rule #4 */
#define PIG_RULE5_ADR           6        /* Pig Rule #5 */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define PIGRULE_ADR_MAX         6        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Service Time Parameter Address (A/C 133)          R3.1
*------------------------------------------------------------------------*/

#define SERV_TIME1_ADR          1        /* Service Time Border #1 */
#define SERV_TIME2_ADR          2        /* Service Time Border #2 */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define SERVTIME_ADR_MAX        2        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Beverage Dispenser Parameter Address (A/C 137)          R3.1
*------------------------------------------------------------------------*/

#define DISPENS_PLU1_ADR        1        /* Dispenser Code #1 */

#define DISPENS_PLU256_ADR    256        /* Dispenser Code #256 */
#define DISPENS_OFFSET_ADR    257        /* Dispenser Offset Code */
#define DISPENS_TOTAL1_ADR    258        /* Dispenser Code for Total Key */
#define DISPENS_TOTAL2_ADR    259        /* Dispenser Code for Total Key */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define DISPENS_ADR_MAX       259        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Labor Cost Value Address (A/C 154)          R3.1
*   see uses of MAX_LABORCOST_SIZE which is max number of job codes.
*------------------------------------------------------------------------*/

#define LABOR_COST1_ADR         1        /* Labor Cost Value #1 */
#define LABOR_COST100_ADR     100        /* Labor Cost Value #100, used as default job code in RptCalWageTotal() */


/*------------------------------------------------------------------------*\
*   Define - Adjust Tone Volume Address (A/C 169)
\*------------------------------------------------------------------------*/

#define TONE_NO1_ADR            1        /* Tone Address */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define TONE_ADR_MAX            1        /* Max Address */


/*------------------------------------------------------------------------*\
*   Define - KDS IP Address (Prog 51)          2172
*------------------------------------------------------------------------*/

//*********************
//	SR 14 ESMITH
#define KDS_IP1_ADR             1        /* KDS IP Address #1 */
#define KDS_IP2_ADR             4        /* KDS IP Address #2 */
#define KDS_IP3_ADR             7        /* KDS IP Address #3 */
#define KDS_IP4_ADR             10        /* KDS IP Address #4 */
#define KDS_IP5_ADR             13       /* KDS IP Address #5 */
#define KDS_IP6_ADR             16        /* KDS IP Address #6 */
#define KDS_IP7_ADR             19        /* KDS IP Address #7 */
#define KDS_IP8_ADR             22        /* KDS IP Address #8 */
#define KDS_PORT1_ADR           2        /* KDS PORT #1 */
#define KDS_PORT2_ADR           5       /* KDS PORT #2 */
#define KDS_PORT3_ADR           8       /* KDS PORT #3 */
#define KDS_PORT4_ADR           11       /* KDS PORT #4 */
#define KDS_PORT5_ADR           14       /* KDS PORT #5 */
#define KDS_PORT6_ADR           17       /* KDS PORT #6 */
#define KDS_PORT7_ADR           20       /* KDS PORT #7 */
#define KDS_PORT8_ADR           23       /* KDS PORT #8 */
#define KDS_INT_PORT1           3        /* Interface PORT #1 */
#define KDS_INT_PORT2           6       /* Interface PORT #2 */
#define KDS_INT_PORT3           9       /* Interface PORT #3 */
#define KDS_INT_PORT4           12      /* Interface PORT #4 */
#define KDS_INT_PORT5           15       /* Interface PORT #5 */
#define KDS_INT_PORT6           18       /* Interface PORT #6 */
#define KDS_INT_PORT7           21       /* Interface PORT #7 */
#define KDS_INT_PORT8           24       /* Interface PORT #8 */
//	END	SR 14
//*********************


/* Warning !!! You have to change this Define When Above Max. address is changed */

#define KDS_IP_ADR_MAX          4        /* Max Address */
#define KDS_ADR_MAX             24        /* Max Address */ //ESMITH
#define KDS_NUM_ADR             8        /* Max Address */ //ESMITH
#define KDS_NUM_PRT             8        /* Max Address */ //ESMITH


/*------------------------------------------------------------------------*\
*   Define - Set PLU Sales Restriction Table Address (A/C 170)
\*------------------------------------------------------------------------*/

#define REST_CODE1_ADR          1        /* Restriction Code 1 */
#define REST_CODE2_ADR          2        /* Restriction Code 2 */
#define REST_CODE3_ADR          3        /* Restriction Code 3 */
#define REST_CODE4_ADR          4        /* Restriction Code 4 */
#define REST_CODE5_ADR          5        /* Restriction Code 5 */
#define REST_CODE6_ADR          6        /* Restriction Code 6 */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define REST_ADR_MAX            6        /* Max Address */

#define REST_DATE_FIELD         1        /* Date Restriction */
#define REST_WEEK_FIELD         2        /* Day of Week Restriction */
#define REST_HOUR1_FIELD        3        /* Start Hour */
#define REST_MINUTE1_FIELD      4        /* Start Minute */
#define REST_HOUR2_FIELD        5        /* End Hour */
#define REST_MINUTE2_FIELD      6        /* End Minute */

/* Warning !!! You have to change this Define When Above Max. field is changed */

#define REST_FIELD_MAX          6        /* Max Field */

/*------------------------------------------------------------------------*\
*   Define - Set Boundary Age Type Address (A/C 208)
\*------------------------------------------------------------------------*/

#define AGE_TYPE1_ADR           1        /* Boundary Age Tye1 */
#define AGE_TYPE2_ADR           2        /* Boundary Age Tye2 */
#define AGE_TYPE3_ADR           3        /* Boundary Age Tye3 */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define AGE_ADR_MAX             3        /* Max Address */

/*------------------------------------------------------------------------*\
*   Define - Set PLU No. On Menu Page Address (A/C 115)
\*------------------------------------------------------------------------*/

#define DEPTPG_PAGE1_ADR         1        /* Menu Page 1 */
#define DEPTPG_PAGE2_ADR         2        /* Menu Page 2 */
#define DEPTPG_PAGE3_ADR         3        /* Menu Page 3 */
#define DEPTPG_PAGE4_ADR         4        /* Menu Page 4 */
#define DEPTPG_PAGE5_ADR         5        /* Menu Page 5 */
#define DEPTPG_PAGE6_ADR         6        /* Menu Page 6 */
#define DEPTPG_PAGE7_ADR         7        /* Menu Page 7 */
#define DEPTPG_PAGE8_ADR         8        /* Menu Page 8 */
#define DEPTPG_PAGE9_ADR         9        /* Menu Page 9 */
#define DEPTPG_PAGE10_ADR       10        /* Menu Page 10 RPH 12-2-3 TOUCHSCREEN*/
#define DEPTPG_PAGE11_ADR       11        /* Menu Page 11 RPH 12-2-3 TOUCHSCREEN*/

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define DEPTPG_ADR_MAX           11        /* Max Address */


/*------------------------------------------------------------------------*\
*   SPECIAL COUNTER DEFINE
\*------------------------------------------------------------------------*/

#define SPCO_CONSEC_ADR         1        /* Consecutive Counter */
#define SPCO_PRG_ADR            2        /* Program Counter */
#define SPCO_EODRST_ADR         3        /* EOD Reset Counter */
#define SPCO_PTDRST_ADR         4        /* PTD Reset Counter */

/* Warning !!! You have to change this Define When Above Max. address is changed */

#define SPCO_ADR_MAX            4        /* Max Address */

/*------------------------------------------------------------------------*\
*   PARAMETER - MNEMONIC LENGTH DEFINE

  ***  WARNING:  Also check sizes in regstrct.h which are used for Item Tender
				in BusinessLogic subsystem.
\*------------------------------------------------------------------------*/

// See WARNING about use of PARA_SOFTCHK_LEN as largest mnemonic length.

#define PARA_MNEMONIC_LEN       42       // max mnemonic length used for PARAMNEMONIC struct

#define PARA_DEPT_LEN           20       /* ParaDEPT, 2172 */
#define PARA_PLU_LEN            20       /* ParaPLU, 2172 */
#define PARA_CPN_LEN            20       /* Para Combination Coupon */   //Changed from 12 to 20  ***PDINU
#define PARA_PLU_STATUS_LEN		10		// PLU_STATUS_LEN sizeof member auchStatus2 of PARAPLU, RECPLU

#if defined(R20_DATABASE_CHANGES)
// These are database size changes that were proposed for GenPOS Release 2.x
// but which were never implemented. We were going to increase the number of
// characters for various mnemonics and that was never actually implemented
// due to dpendencies on Third Party applications for these sizes.
// Changing these sizes would also require changes to the ISP remote application
// messages and without some business case and funding, the Third Party vendors
// were reluctant to make any such changes.
//    Richard Chambers, Nov-19-2020

// Following are new sizes for specific mnemonics in the data base.
//
// **   WARNING:
//      These sizes must match sizes specified for PEP version of paraequ.h
#define PARA_PROMOTION_LEN      36       /* ParaPromotion */
#define PARA_TRANSMNEMO_LEN     20       /* ParaTransMnemo */
#define PARA_LEADTHRU_LEN       20       /* ParaLeadThru */
#define PARA_REPORTNAME_LEN     12       /* ParaReportName */
#define PARA_SPEMNEMO_LEN       12       /* ParaSpeMnemo */
#define PARA_ADJMNEMO_LEN       12       /* ParaAdjMnemo */
#define PARA_PRTMODI_LEN        10       /* ParaPrtModi */
#define PARA_MAJORDEPT_LEN      20       /* ParaMajorDEPT */
#define PARA_CHAR24_LEN         36       /* ParaChar24 */
#else

// Following are the old sizes.  See WARNING about use of PARA_SOFTCHK_LEN as largest mnemonic length.

#define PARA_PROMOTION_LEN      24       /* ParaPromotion - AC88 Sales Promotion Message Mnemonics */
#define PARA_TRANSMNEMO_LEN     20       /* ParaTransMnemo - P20 Transaction Mnemonics, Database Changes JHHJ 3-29-04*/
#define PARA_LEADTHRU_LEN       20       /* ParaLeadThru - P21 Lead Thru Mnemonics */
#define PARA_REPORTNAME_LEN      6       /* ParaReportName - P22 Report Mnemonics */
#define PARA_SPEMNEMO_LEN        4       /* ParaSpeMnemo - P23 Special Mnemonics */
#define PARA_ADJMNEMO_LEN        4       /* ParaAdjMnemo - P46 Adjective Mnemonics */
#define PARA_PRTMODI_LEN        10       /* ParaPrtModi - P47 Print Modifier Mnemonics */
#define PARA_MAJORDEPT_LEN      20       /* ParaMajorDEPT - P48 Major Department Mnemonics, Database changes JHHJ*/
#define PARA_CHAR24_LEN         24       /* ParaChar24 - P57 Header/Special Message Mnemonics */
#endif

/* #define PARA_CHAR44_LEN      44  */     /* ParaChar44 */
#define PARA_SOFTCHK_LEN        42       /* ParaSoftCheck.  WARNING: used in places to define large enough buffer for any mnemonic. */
#define PARA_CASHIER_LEN        20       // number of characters in the cashier mnemonic
#define PARA_LEN_CASHIER_STATUS  6       // number of bytes in the status section of cashier record
#define PARA_PCIF_LEN           42       /* ParaPCIF */
#define PARA_PASSWORD_LEN       10       /* paraPCIF */
#define PARA_COMMAND_LEN        40       /* ParaPCIF */
#define PARA_HOTELID_LEN        2        /* ParaHotelId Length */
#define PARA_SLD_LEN            4        /* ParaHotelId's Sld Length */
#define PARA_PDT_LEN           10        /* ParaHotelId's Product Length */
#define PARA_TEND_LEN           1        /* Tender Length */
#define MAX_TALLY_MNEMO        15        /* Tally Report Mnemonics */
#define PARA_APVL_CODE_LEN      2        /* ParaHotelId Length */
#define PARA_MLDMNEMO_LEN           32        /* multi-line display length */
#define PARA_PLU_LINKNO_LEN      3       /* ParaPLU, 2172 */

#define PARA_UNLOCK_LEN        48        /* len of aszUnlockNo of PARAUNLOCKNO */
/*------------------------------------------------------------------------*\
*   PARAMETER - OTHER DEFINE
\*------------------------------------------------------------------------*/

/* Define - ParaMenuPLUWrite   uchAction  */

#define SET_DATA                 0       /* set data */
#define RESET_DATA               1       /* reset data */

/*------------------------------------------------------------------------*\
*   PARAMETER - DEPT/PLU STATUS DEFINE
\*------------------------------------------------------------------------*/

/* PLU Type by Reg */

#define PLU_OPEN_TYP            0x40        /* Open PLU */
#define PLU_NON_ADJ_TYP         0x80        /* Non Adjective */
#define PLU_ADJ_TYP             0xc0        /* One Adjective */
#define PLU_TYPE_MASK_REG       0xc0        /* PLU type mask data */

#define ADJ_PLU                 PLU_OPEN_TYP    /* Open PLU */
#define PRESET_PLU              PLU_NON_ADJ_TYP /* Non Adjective */

/* PLU Type by Super/Program */

#define PLU_OPEN              1           /* Open PLU */
#define PLU_NONADJ            2           /* Non Adjective */
#define PLU_ONEADJ            3           /* One Adjective */
#define PLU_DEPTNO_MASK       0x3F        /* Department Number */
#define PLU_REPORT_MASK       0x0F        /* Report Code */

/* PLU Max Data Define */

#define PLU_TYPE_MAX            3
#define PLU_DEPTNO_MAX         50
#define PLU_REPORT_MAX         10
#define PLU_OEPTBL_MAX         99           /* V3.3 */
#define PLU_OEPGRP_MAX         90
#define PLU_PRIORITY_MAX       15
#define PLU_PPICODE_MAX       300 //ESMITH
#define PLU_LINKNO_MAX       9999       /* 2172 */
#define PLU_RESTRICT_MAX        7       /* 2172 */
#define PLU_PM_MAX             99       /* 2172 */
#define PLU_PRICE_DIGITMAX      7       /* max digit of price (9999999) */
#define PLU_FAMILY_MAX        999       /* 2172 */


// To generalize the following defines which require knowledge of which array element a particular
// indicator resides we are using a bitmap in a ULONG that is generated by
// function ItemSalesItemPluTypeInfo() to allow ease of use and ease of notation in the source code.
// See also the defines PLU_BM_MINUS_PLU and others that begin with PLU_BM_ which define the various
// bit indicators in the bitmap of the ULONG.

/* PLU Record Address 3,4 also known as ParaPlu.ContPlu.auchContOther[0] (PLU_CONTROLCODE_ADR_0) or pRecPlu->aucStatus[2] */
#define PLU_MINUS               0x01    /* Minus */
#define PLU_SPECIAL             0x02    /* Special Print remote device. see also PRT_SPCL_PRINT */
#define PLU_REQ_SUPINT          0x04    /* Request Supervisor Intervention */
#define PLU_COMP_MODKEY         0x08    /* Compulsory Print Modifier Key, see also SALES_PM_COMPLSRY */
#define PLU_COMP_CODKEY         0x10    /* Compulsory Print Modifier or Condiment Key, see also SALES_COND_COMPLSRY */
#define PLU_PRT_VAL             0x20    /* Print Validation */
#define PLU_SINGREC             0x40    /* Issue Single or Double Receipt */
#define PLU_DBLREC              0x80    /* Print Double Receipt */

/* PLU Record Address 5,6 also known as ParaPlu.ContPlu.auchContOther[1] (PLU_CONTROLCODE_ADR_1) or pRecPlu->aucStatus[3] */
// DEPENDENCY WARNING: ParaPlu.ContPlu.auchContOther[1] is assigned to ITEMCOUPONSEARCH.auchStatus[0] in TrnStoCpnUPCSearch().
#define PLU_AFFECT_TAX1         0x01    /* Affect US Taxable Itemizer #1 or Canadian tax index bit 0 or VAT Int'l tax index bit 0 */
#define PLU_AFFECT_TAX2         0x02    /* Affect US Taxable Itemizer #2 or Canadian tax index bit 1 or VAT Int'l tax index bit 1 */
#define PLU_AFFECT_TAX3         0x04    /* Affect US Taxable Itemizer #3 or Canadian tax index bit 2 */
#define PLU_AFFECT_FS           0x08    /* Affect US Food Stamp Itemizer or Canadian tax index bit 3 or Affect VAT Int'l Servicable Itemizer */
#define PLU_AFFECT_VAT_SERVICE  0x08    /* Affect VAT Int'l Serviceable Itemizer or Affect US Food Stamp Itemizer or Canadian tax index bit 3 */
//#define PLU_AFFECT_TAX4         0x10    /* Affect US Taxable Itemizer #4, CPN_STATUS0_AUTOCOUPON for COMCOUPONPARA.uchCouponStatus[0] */
//#define PLU_AFFECT_TAX5         0x20    /* Affect US Taxable Itemizer #5, CPN_STATUS0_SINGLE_TYPE for COMCOUPONPARA.uchCouponStatus[0] */
#define PLU_AFFECT_DISC1        0x40    /* Affect Discountable Itemizer #1 */
#define PLU_AFFECT_DISC2        0x80    /* Affect Discountable Itemizer #2 */

#define PLU_DISC_TAX_MASK		0x07    // US Taxable Itemizer indicators.
#define TRN_MASKSTATCAN         0x0f    // Canadian tax index mask for ControlCode.auchPluStatus[1], see also PRT_MASKSTATCAN

/* PLU Record Address 7,8 also known as ParaPlu.ContPlu.auchContOther[2] (PLU_CONTROLCODE_ADR_2) or pRecPlu->aucStatus[4] */

#define PLU_SCALABLE            0x01    /* Scalable PLU */
#define PLU_CONDIMENT           0x02    /* Condiment */
#define PLU_HASH                0x04    /* Hash PLU. See the comments titled What is a Hash Department in rptdept.c */
#define PLU_USE_DEPTCTL         0x08    /* Use Department Control Key */
#define PLU_SND_KITCH1          0x10    /* Send Kitchen Printer #1 */
#define PLU_SND_KITCH2          0x20    /* Send Kitchen Printer #2 */
#define PLU_SND_KITCH3          0x40    /* Send Kitchen Printer #3 */
#define PLU_SND_KITCH4          0x80    /* Send Kitchen Printer #4 */
#define PLU_SALES_KP_NO_1_4     0xf0    // Mask to isolate indicators to use kitchen printer or CRT numbers 1 through 4

/* PLU Record Address 9,10 also known as ParaPlu.ContPlu.auchContOther[3] (PLU_CONTROLCODE_ADR_3) or pRecPlu->aucStatus[5] */

// On investigating this what it appears is that originally there were eight bonus totals
// that could be affected by a PLU. Within this area of auchPLUStatus[3] there were
// eight bits that could be used to indicate which of the eight bonus totals were
// affected by the PLU. This bit mask with each of the eight bonus totals being indicated
// by one of the eight bits in the MDC field allowed a PLU to affect all or none or some
// of the eight possible bonus totals. This is how it worked with the NCR 2170 and the
// NCR 7448 and NHPOS.
// However with the donations of the software to Georgia Southern University in 2002 along with
// changes requested by the NCR Product Manager Dan Parliman to have more than eight bonus totals
// we have gone from a bit map allowing any combination of bonus totals to be assigned to a PLU
// to allowing only a single bonus total to be assigned to a PLU but there are 100 possible bonus totals now.
//
// See as well function MaintPLUEdit() which allows the bonus total for a PLU to be edited.
// See places where define PLU_FIELD9_ADR is used with the display of PLU data.
//    Richard Chambers, Mar-03-2020

#define PLU_NOT_BONUS              0    /* Not Affect Bonus Total */
#define PLU_BONUS_IDX1             1    /* Bonus Total Index #1 */
#define PLU_BONUS_IDX2             2    /* Bonus Total Index #2 */
#define PLU_BONUS_IDX3             3    /* Bonus Total Index #3 */
#define PLU_BONUS_IDX4             4    /* Bonus Total Index #4 */
#define PLU_BONUS_IDX5             5    /* Bonus Total Index #5 */
#define PLU_BONUS_IDX6             6    /* Bonus Total Index #6 */
#define PLU_BONUS_IDX7             7    /* Bonus Total Index #7 */
#define PLU_BONUS_IDX8             8    /* Bonus Total Index #8 */
#define BONUS_MASK_DATA            0x0f /* bonus index mask data */

#define PLU_USE_ADJG1           0x10    /* Use Adjective Group #1 */
#define PLU_USE_ADJG2           0x20    /* Use Adjective Group #2 */
#define PLU_USE_ADJG3           0x40    /* Use Adjective Group #3 */
#define PLU_USE_ADJG4           0x80    /* Use Adjective Group #4 */
#define PLU_USE_ADJG_MASK       0xf0    /* Use Adjective Group #4 */

/* PLU Record Address 11,12 also known as ParaPlu.ContPlu.auchContOther[4] (PLU_CONTROLCODE_ADR_4) or pRecPlu->aucStatus[6] */
#define PLU_COMP_ADJKEY            0       /* Adjective Key is Compulsory */
#define PLU_IMP_ADJKEY_VAL1        0x01    /* Implied Adjective Key Variation #1 */
#define PLU_IMP_ADJKEY_VAL2        0x02    /* Implied Adjective Key Variation #2 */
#define PLU_IMP_ADJKEY_VAL3        0x03    /* Implied Adjective Key Variation #3 */
#define PLU_IMP_ADJKEY_VAL4        0x04    /* Implied Adjective Key Variation #4 */
#define PLU_IMP_ADJKEY_VAL5        0x05    /* Implied Adjective Key Variation #5 */
#define PLU_IMP_ADJKEY_MASK        0x07    /* Implied Adjective Key Mask Data */

#define PLU_PROHIBIT_VAL1       0x08    /* Prohibit Validation #1 */
#define PLU_PROHIBIT_VAL2       0x10    /* Prohibit Validation #2 */
#define PLU_PROHIBIT_VAL3       0x20    /* Prohibit Validation #3 */
#define PLU_PROHIBIT_VAL4       0x40    /* Prohibit Validation #4 */
#define PLU_PROHIBIT_VAL5       0x80    /* Prohibit Validation #5 */
#define PLU_PROH_VAL_MASK       0xf8    /* Prohibit Validation Mask Data */

// Following status flags are used with ControlCode.auchStatus[5] (PLU_CONTROLCODE_ADR_5) of ITEMSALES
// The settings in this byte of the status comes from several different MDC settings and are not contained
// in the PLU record:
//  - MDC 27 settings, "Department/PLU Item 1" in PEP.
//  - MDC 29 settings, "Scale/Tare (Scale)" in PEP
//  - MDC 37 settings, "Item Disc./Surcharge 2 key"
// See function ItemSalesCommonIF() for an example.
#define ITM_SALES_HOURLY            0x01            /* hourly affection, set based on MDC 27, MDC_DEPT1_ADR */
#define ITM_HASH_FINANC             0x02            /* hourly affection, set based on MDC 27, MDC_DEPT1_ADR */
#define ITM_AFFECT_DEV_BY_10        0x04            /* scalable item affects by 0.01 LB/ Kg, set based on PLU_SCALABLE and MDC 29, MDC_SCALE_ADR */
#define ITM_PLU_LINK_SCALE_DEPT     0x08            /* PLUlinks to scalable dept */
    /* for modified discount */
#define ITM_MOD_DISC_SUR            0x10            /* modified surcharge, set based on MDC 37, MDC_MODID23_ADR */
#define ITM_MOD_AFFECT_DEPT_PLU     0x20            /* mod disc affection, set based on MDC 37, MDC_MODID23_ADR */


/* PLU Record Address 18 R3.1 ControlCode.auchStatus[PLU_CONTROLCODE_ADR_6] */
#define PLU_SND_KITCH5          0x01    /* Send Kitchen Printer #5 */
#define PLU_SND_KITCH6          0x02    /* Send Kitchen Printer #6 */
#define PLU_SND_KITCH7          0x04    /* Send Kitchen Printer #7 */
#define PLU_SND_KITCH8          0x08    /* Send Kitchen Printer #8 */
#define PLU_SALES_KP_NO_5_8     0x0f    // Mask to isolate indicators to use kitchen printer or CRT numbers 5 through 8
  // PLU Record Address 19 R3.1 ControlCode.auchStatus[PLU_CONTROLCODE_ADR_6] */
#define PLU_BIRTHDAY_CONTROL_M  0x30    /* Mask used to isolate Age Type setting */ 
#define PLU_BIRTHDAY_CONTROL1   0x10    /* Age Type setting, Birthday Control 1 */
#define PLU_BIRTHDAY_CONTROL2   0x20    /* Age Type setting, Birthday Control 2 */
#define PLU_BIRTHDAY_CONTROL3   0x30    /* Age Type setting, Birthday Control 3 */
#define PLU_SALES_WIC_ITEM      0x40    /* this is a WIC item PLU */

/* PLU Record Address 20 R3.1 ControlCode.auchStatus[PLU_CONTROLCODE_ADR_7] */
#define PLU_ALLOW_0_TEND		0x01    // Allows tender of $0
#define PLU_NOT_ALLOW_QR		0x02	//Quantity Restriction 
#define PLU_REQUIRE_QR			0x04	//Require Quantity
#define PLU_GIFT_CARD			0x08	//Gift Card
#define PLU_CTRL_COUPON			0x10	// Used for Combination Coupon Precedence, ITM_PRECEDENCE_APPLIED
#define PLU_EPAYMENT			0x20	// Used for Electronic Payment such as Employee Payroll Deduction or E-Meal
#define PLU_GIFT_RELOAD			0x80	//reload

/* PLU record Tare Information 2.1.X 1.29.06 */
//new define for requiring a tare, we use the high value because we are using the lower for a numerical value. JHHJ
#define PLU_REQ_TARE		0xf000
#define PLU_REQ_TARE_MASK	0x001f    // max value is 30 but we use the masking of the lower 5 bits

// Plu Controll Code index (used in ControlCode.auchStatus)   ***PDINU
// These indexes have not been implemented for auchStatus[0] 
// through auchStatus[5].  Modifications have only been made to
// auchStatus[6]
#define	PLU_CONTROLCODE_ADR_0	0		//Index of ControlCode.auchStatus (A68_CTRL_0 in PEP)
#define	PLU_CONTROLCODE_ADR_1	1		//Index of ControlCode.auchStatus (A68_CTRL_1 in PEP)
#define	PLU_CONTROLCODE_ADR_2	2		//Index of ControlCode.auchStatus (A68_CTRL_2 in PEP)
#define	PLU_CONTROLCODE_ADR_3	3		//Index of ControlCode.auchStatus (A68_CTRL_3 in PEP)
#define	PLU_CONTROLCODE_ADR_4	4		//Index of ControlCode.auchStatus (A68_CTRL_4 in PEP)
#define	PLU_CONTROLCODE_ADR_5	5		//Index of ControlCode.auchStatus set based on MDC 27/29/37, MDC_DEPT1_ADR, MDC_SCALE_ADR, MDC_MODID23_ADR
#define	PLU_CONTROLCODE_ADR_6	6		//Index of ControlCode.auchStatus for address 18 and address 19 (A68_CTRL_5 in PEP)
#define	PLU_CONTROLCODE_ADR_7	7		//Index of ControlCode.auchStatus for address 20 (A68_CTRL_6 in PEP)

/*------------------------------------------------------------------------*\
*   PARAMETER - ERROR STATUS DEFINE
\*------------------------------------------------------------------------*/

#define PARA_NOTOVER_RAMSIZE          (-1)     /* Received Buffer is not over RAM SIZE */
#define PARA_OFFOVER_RAMSIZE          (-2)     /* Offset is over RAM SIZE */
#define PARA_ERR_INVALID_MESSAGE     (-31)     /* Class type is invalid, same numeric code as ISP_ERR_INVALID_MESSAGE */

/*------------------------------------------------------------------------*\
*   PARAMETER - ELECTORIC JOURNAL DEFINE
\*------------------------------------------------------------------------*/

#define EJ_REPORT_WIDE                48     /* EJ 1 Line Record Data */
#define EJ_EJLEN_MIN                  50     /* Minimum Line of EJ */

/*------------------------------------------------------------------------*\
*   PARAMETER - ELECTORIC JOURNAL DEFINE
\*------------------------------------------------------------------------*/
#define PARA_STR_FWR_DISABLE		0
#define PARA_STR_FWR_ENABLE			1

/*------------------------------------------------------------------------*\
*   PARAMETER - FLEXIBLE DRIVE THROUGH/MULTI STORE RECALL DEFINE - AC162
*
*  Terminals in Flexible Drive Thru may be programmed as a drive-through terminal or a counter terminal.
*    Drive-Through Terminals:
*      - are comprised of drive-through side 1 (DT1) and/or drive-through side 2 (DT2).
*      - may split the three functionalities: order, payment, and delivery between different terminals within the cluster
*      - may recall an order by depressing the Recall Order key.
*      - may be used instead of counter terminals.
*
*   Counter Terminals
*      - may not split the three functionalities: order, payment, and delivery between different terminals within the cluster
*      - require entry of order # to recall a stored order.
*
*   The first four addresses define multi-terminal Drive-through set-up
*      - Provides direction of where to send an order if there is more than one terminal in a drive-through side.
*      - Set to 0 if there is only 1 terminal in DT1/DT2 or counter terminals only.
*      - Otherwise enter in addresses 1-4 the terminal # for payment/delivery:
*
*      Addr#1 = Term # of Delivery Terminal for DT Side#1, FX_DRIVE_DELIVERY_1
*      Addr#2 = Term # of Delivery Terminal for DT Side#2, FX_DRIVE_DELIVERY_2
*      Addr#3 = Term # of PAYMENT Terminal for DT Side#1, FX_DRIVE_PAYMENT_1
*      Addr#4 = Term # of PAYMENT Terminal for DT Side#2, FX_DRIVE_PAYMENT_2
*
*   The next 32 addresses define the System Type and the Terminal Type as a pair of values for each specific terminal in the cluster.
*   The first address of the pair, System Type, indicates the basic Order, Pay, Deliver functionality or system type of the terminal.
*   The second address of the pair, Terminal Type, indicates the role of the terminal within the basic functionality or system.
*   
*   System Type (0 - 2)
*      - 0 = Ordering/Payment System - Most common, Used for all counter terminals
*      - 1 = Ordering & Payment - Used with two window drive thru installations, Ordering at one terminal and Pay at another. 
*      - 2 = Ordering/Payment & Delivery - Used with two window drive thru installations, Order & pay at one terminal, deliver at another
*
*   Terminal Type (0 - 13)
*      There are two types of transaction display:
*         - FULL Screen uses the standard Single Receipt window which shows the current transaction data
*            - in the MLD subsystem functionality denoted by MLD_DRIVE_THROUGH_1SCROLL
*         - 3 Part Screen uses the multiple Receipt window to show current transaction and next transaction
*            - in the MLD subsystem functionality denoted by MLD_DRIVE_THROUGH_3SCROLL
*
*      -  0 = Order Taking DT Terminal for DT Side#1 - 3 part Screen used with System Type = {0, 1, 2}
*      -  1 = Order/Payment DT Terminal for DT Side#1 - 3 part Screen used with System Type = {0, 2}
*      -  2 = Payment DT Terminal for DT Side#1 - 3 part Screen used with System Type = {1}
*      -  3 = Delivery DT Terminal for DT Side#1 - 3 part Screen used with System Type = {2}
*      -  4 = Order Taking DT Terminal for DT Side#2 - 3 part Screen used with System Type = {0, 1, 2}
*      -  5 = Order/Payment DT Terminal for DT Side#2 - 3 part Screen used with System Type = {0, 2})
*      -  6 = Payment DT Terminal for DT Side#2 - 3 part Screen used with System Type = {1}
*      -  7 = Delivery DT Terminal for DT Side#2 - 3 part Screen used with System Type = {2}
*      -  8 = Counter Terminal, UNPAID Orders to the right, 3 part Screen used with System Type = {0}
*      -  9 = Counter Terminal, PAID Orders to the right, 3 part Screen used with System Type = {0}
*      - 10 = Order Taking DT Terminal for DT Side#1 - FULL Screen used with System Type = {1}
*      - 11 = Order Taking DT Terminal for DT Side#2 - FULL Screen used with System Type = {1}
*      - 12 = COUNTER Terminal for UNPAID TRANSACTION - FULL Screen used with System Type = {0}
*      - 13 = COUNTER Terminal for PAID TRANSACTION - FULL Screen used with System Type = {0}
\*------------------------------------------------------------------------*/

//  Following defines used in PARAFXDRIVE member uchTblData[] offset calculations
#define FX_DRIVE_DELIVERY_1              1   /* Delivery Terminal #1 Node Address */
#define FX_DRIVE_DELIVERY_2              2   /* Delivery Terminal #2 Node Address */
#define FX_DRIVE_PAYMENT_1               3   /* Payment  Terminal #1 Node Address */
#define FX_DRIVE_PAYMENT_2               4   /* Payment  Terminal #2 Node Address */

#define FX_DRIVE_SYSTERM_1               5   /* System   Type of Terminal #1      */
#define FX_DRIVE_TERMTYPE_1              6   /* Terminal Type of Terminal #1      */
#define FX_DRIVE_SYSTERM_2               7   /* System   Type of Terminal #2      */
#define FX_DRIVE_TERMTYPE_2              8   /* Terminal Type of Terminal #2      */
#define FX_DRIVE_SYSTERM_3               9   /* System   Type of Terminal #3      */
#define FX_DRIVE_TERMTYPE_3             10   /* Terminal Type of Terminal #3      */
#define FX_DRIVE_SYSTERM_4              11   /* System   Type of Terminal #4      */
#define FX_DRIVE_TERMTYPE_4             12   /* Terminal Type of Terminal #4      */
#define FX_DRIVE_SYSTERM_5              13   /* System   Type of Terminal #5      */
#define FX_DRIVE_TERMTYPE_5             14   /* Terminal Type of Terminal #5      */
#define FX_DRIVE_SYSTERM_6              15   /* System   Type of Terminal #6      */
#define FX_DRIVE_TERMTYPE_6             16   /* Terminal Type of Terminal #6      */
#define FX_DRIVE_SYSTERM_7              17   /* System   Type of Terminal #7      */
#define FX_DRIVE_TERMTYPE_7             18   /* Terminal Type of Terminal #7      */
#define FX_DRIVE_SYSTERM_8              19   /* System   Type of Terminal #8      */
#define FX_DRIVE_TERMTYPE_8             20   /* Terminal Type of Terminal #8      */
#define FX_DRIVE_SYSTERM_9              21   /* System   Type of Terminal #9      */
#define FX_DRIVE_TERMTYPE_9             22   /* Terminal Type of Terminal #9      */
#define FX_DRIVE_SYSTERM_10             23   /* System   Type of Terminal #10     */
#define FX_DRIVE_TERMTYPE_10            24   /* Terminal Type of Terminal #10     */
#define FX_DRIVE_SYSTERM_11             25   /* System   Type of Terminal #11     */
#define FX_DRIVE_TERMTYPE_11            26   /* Terminal Type of Terminal #11     */
#define FX_DRIVE_SYSTERM_12             27   /* System   Type of Terminal #12     */
#define FX_DRIVE_TERMTYPE_12            28   /* Terminal Type of Terminal #12     */
#define FX_DRIVE_SYSTERM_13             29   /* System   Type of Terminal #13     */
#define FX_DRIVE_TERMTYPE_13            30   /* Terminal Type of Terminal #13     */
#define FX_DRIVE_SYSTERM_14             31   /* System   Type of Terminal #14     */
#define FX_DRIVE_TERMTYPE_14            32   /* Terminal Type of Terminal #14     */
#define FX_DRIVE_SYSTERM_15             33   /* System   Type of Terminal #15     */
#define FX_DRIVE_TERMTYPE_15            34   /* Terminal Type of Terminal #15     */
#define FX_DRIVE_SYSTERM_16             35   /* System   Type of Terminal #16     */
#define FX_DRIVE_TERMTYPE_16            36   /* Terminal Type of Terminal #16     */

// following defines are used with struct FDTPARA, member uchTypeTerm to indicate terminal role
#define FX_DRIVE_ORDER_TERM_1           0    /* Order Terminal #1 */
#define FX_DRIVE_ORDER_PAYMENT_TERM_1   1    /* Order/Payment Terminal #1 */
#define FX_DRIVE_PAYMENT_TERM_1         2    /* Payment Terminal #1 */
#define FX_DRIVE_DELIV_1                3    /* Delivery Terminal #1 */
#define FX_DRIVE_ORDER_TERM_2           4    /* Order Terminal #2 */
#define FX_DRIVE_ORDER_PAYMENT_TERM_2   5    /* Order/Payment Terminal #2 */
#define FX_DRIVE_PAYMENT_TERM_2         6    /* Payment Terminal #2 */
#define FX_DRIVE_DELIV_2                7    /* Delivery Terminal #2 */
#define FX_DRIVE_COUNTER_STORE          8    /* Counter Terminal (Store) */
#define FX_DRIVE_COUNTER_STORE_PAY      9    /* Counter Terminal (Store/Payment) */
#define FX_DRIVE_ORDER_FUL_TERM_1      10    /* Order Full Screen Terminal #1 */
#define FX_DRIVE_ORDER_FUL_TERM_2      11    /* Order Full Screen Terminal #2 */
#define FX_DRIVE_COUNTER_FUL_STORE     12    /* Counter Full Screen Terminal (Store) */
#define FX_DRIVE_COUNTER_FUL_STORE_PAY 13    /* Counter Full Screen Terminal (Store/Payment) */

#define FX_DRIVE_SYSTYPE_1T_OP          0    /* Ordering/Payment System (counter or single drive thru) */
#define FX_DRIVE_SYSTYPE_2T_OP          1    /* Ordering & Payment System (two terminal drive thru) */
#define FX_DRIVE_SYSTYPE_2T_OP_D        2    /* Ordering/Payment & Delivery System (two terminal drive thru) */

/*------------------------------------------------------------------------*\
*   Define - Mld Mnemonics Address (PRG # 65), V3.3
*     These mnemonics are used with MLDMENU structs to specify the mnemonics
*     to be used in the Supervisor Mode menus for action codes.
\*------------------------------------------------------------------------*/

#define LCD_AC1_ADR             1       /* AC 1 */
#define LCD_AC2_ADR             2       /* AC 2 */
#define LCD_AC3_ADR             3       /* AC 3 */
#define LCD_AC4_ADR             4       /* AC 4 */
#define LCD_AC5_ADR             5       /* AC 5 */
#define LCD_AC6_ADR             6       /* AC 6 */
#define LCD_AC7_ADR             7       /* AC 7 */
#define LCD_AC15_ADR            8       /* AC 15 */
#define LCD_AC17_ADR            9       /* AC 17 */
#define LCD_AC18_ADR           10       /* AC 18 */
#define LCD_AC19_ADR           11       /* AC 19 */
#define LCD_AC20_ADR           12       /* AC 20 */
#define LCD_AC21_ADR           13       /* AC 21 */
#define LCD_AC23_ADR           14       /* AC 23 */
#define LCD_AC24_ADR           15       /* AC 24 */
#define LCD_AC26_ADR           16       /* AC 26 */
#define LCD_AC29_ADR           17       /* AC 29 */
#define LCD_AC30_ADR           18       /* AC 30 */
#define LCD_AC33_ADR           19       /* AC 31 */
#define LCD_AC39_ADR           20       /* AC 39 */
#define LCD_AC40_ADR           21       /* AC 40 */
#define LCD_AC42_ADR           22       /* AC 42 */
#define LCD_AC63_ADR           23       /* AC 63 */
#define LCD_AC64_ADR           24       /* AC 64 */
#define LCD_AC66_ADR           25       /* AC 66 */
#define LCD_AC68_ADR           26       /* AC 68 */
#define LCD_AC70_ADR           27       /* AC 70 */
#define LCD_AC71_ADR           28       /* AC 71 */
#define LCD_AC72_ADR           29       /* AC 72 */
#define LCD_AC75_ADR           30       /* AC 75 */
#define LCD_AC78_ADR           31       /* AC 78 */
#define LCD_AC82_ADR           32       /* AC 82 */
#define LCD_AC84_ADR           33       /* AC 84 */
#define LCD_AC85_ADR           34       /* AC 85 */
#define LCD_AC86_ADR           35       /* AC 86 */
#define LCD_AC87_ADR           36       /* AC 87 */
#define LCD_AC88_ADR           37       /* AC 88 */
#define LCD_AC89_ADR           38       /* AC 89 */
#define LCD_AC97_ADR           39       /* AC 97 */
#define LCD_AC99_ADR           40       /* AC 99 */
#define LCD_AC100_ADR          41       /* AC 100 */
#define LCD_AC103_ADR          42       /* AC 103 */
#define LCD_AC111_ADR          43       /* AC 111 */
#define LCD_AC114_ADR          44       /* AC 114 */
#define LCD_AC116_ADR          45       /* AC 116 */
#define LCD_AC119_ADR          46       /* AC 119 */
#define LCD_AC122_ADR          47       /* AC 122 */
#define LCD_AC123_ADR          48       /* AC 123 */
#define LCD_AC124_ADR          49       /* AC 124 */
#define LCD_AC125_ADR          50       /* AC 125 */
#define LCD_AC126_ADR          51       /* AC 126 */
#define LCD_AC127_ADR          52       /* AC 127 */
#define LCD_AC129_ADR          53       /* AC 129 */
#define LCD_AC130_ADR          54       /* AC 130 */
#define LCD_AC131_ADR          55       /* AC 131 */
#define LCD_AC132_ADR          56       /* AC 132 */
#define LCD_AC133_ADR          57       /* AC 133 */
#define LCD_AC135_ADR          58       /* AC 135 */
#define LCD_AC136_ADR          59       /* AC 136 */
#define LCD_AC137_ADR          60       /* AC 137 */
#define LCD_AC150_ADR          61       /* AC 150 */
#define LCD_AC151_ADR          62       /* AC 151 */
#define LCD_AC152_ADR          63       /* AC 152 */
#define LCD_AC153_ADR          64       /* AC 153 */
#define LCD_AC154_ADR          65       /* AC 154 */
#define LCD_AC160_ADR          66       /* AC 160 */
#define LCD_AC161_ADR          67       /* AC 161 */
#define LCD_AC162_ADR          68       /* AC 162 */
#define LCD_AC169_ADR          69       /* AC 169 */
#define LCD_AC175_ADR          70       /* AC 175 */
#define LCD_AC176_ADR          71       /* AC 176 */
#define LCD_AC223_ADR          72       /* AC 223 */
#define LCD_AC233_ADR          73       /* AC 233 */
#define LCD_RPT_DAILY_ADR      74       /* Daily Report Common, see also LCD_RPT_DAILYSAVE_ADR */
#define LCD_RPT_PDT_ADR        75       /* PTD Report Common, see also LCD_RPT_PDTSAVE_ADR */
#define LCD_RPT_SAVED_ADR      76       /* Saved Total Report Common */
#define LCD_RPT_READ_RESET_ADR 77       /* Read & Reset Report Common */
#define LCD_RPT_ONLY_RESET_ADR 78       /* Only Reset Report Common */
#define LCD_RPT_ALL_READ_ADR   79       /* All Read Report Common */
#define LCD_RPT_IND_READ_ADR   80       /* Ind. Read Report Common */
#define LCD_RPT_ALL_RESET      81       /* All Reset Common */
#define LCD_RPT_IND_RESET      82       /* Ind. Reset Common */
#define LCD_RPT_SERV_ADR       83       /* Read by Server */
#define LCD_RPT_RANGE_ADR      84       /* Read by Range */
#define LCD_RPT_MAJOR_ADR      85       /* Major Dept. Read */
#define LCD_RPT_DEPT_ADR       86       /* Dept. Read */
#define LCD_RPT_CODE_ADR       87       /* Report Code Read */
#define LCD_CPM_START_ADR      88       /* Start CPM */
#define LCD_CPM_STOP_ADR       89       /* Stop CPM */
#define LCD_CPM_READ_ADR       90       /* CPM Tally Read */
#define LCD_CPM_RESET_ADR      91       /* CPM Tally Reset */
#define LCD_EPT_READ_ADR       92       /* EPT Tally Read */
#define LCD_EPT_RESET_ADR      93       /* EPT Tally Reset */
#define LCD_CAS_ADDR1_ADR      94       /* AC 20, Address 1 */
#define LCD_CAS_ADDR2_ADR      95       /* AC 20, Address 2 */
#define LCD_CAS_ADDR3_ADR      96       /* AC 20, Address 3 */
#define LCD_CAS_ADDR4_ADR      97       /* AC 20, Address 4 */
#define LCD_CAS_ADDR5_ADR      98       /* AC 20, Address 5 */
#define LCD_CAS_ADDR6_ADR      99       /* AC 20, Address 6 */
#define LCD_CAS_ADDR7_ADR     100       /* AC 20, Address 7 */
#define LCD_CAS_ADDR8_ADR     101       /* AC 20, Address 8 */
#define LCD_CAS_ADDR9_ADR     102       /* AC 20, Address 9 */
#define LCD_CAS_ADDR10_ADR    103       /* AC 20, Address 10 */
#define LCD_PLU_ADDR1_ADR     104       /* AC 63, Address 1 */
#define LCD_PLU_ADDR2_ADR     105       /* AC 63, Address 2 */
#define LCD_PLU_ADDR3_ADR     106       /* AC 63, Address 3 */
#define LCD_PLU_ADDR4_ADR     107       /* AC 63, Address 4 */
#define LCD_PLU_ADDR5_ADR     108       /* AC 63, Address 5 */
#define LCD_PLU_ADDR6_ADR     109       /* AC 63, Address 6 */
#define LCD_PLU_ADDR7_ADR     110       /* AC 63, Address 7 */
#define LCD_PLU_ADDR8_ADR     111       /* AC 63, Address 8 */
#define LCD_PLU_ADDR9_ADR     112       /* AC 63, Address 9 */
#define LCD_PLU_ADDR10_ADR    113       /* AC 63, Address 10 */
#define LCD_PLU_ADDR11_ADR    114       /* AC 63, Address 11 */
#define LCD_PLU_ADDR12_ADR    115       /* AC 63, Address 12 */
#define LCD_PLU_ADDR13_ADR    116       /* AC 63, Address 13 */
#define LCD_PLU_ADDR14_ADR    117       /* AC 63, Address 14 */
#define LCD_PLU_ADDR15_ADR    118       /* AC 63, Address 15 */
#define LCD_PLU_ADDR16_ADR    119       /* AC 63, Address 16 */
#define LCD_PLU_ADDR17_ADR    120       /* AC 63, Address 17 */
#define LCD_PLU_ADDR18_ADR    121       /* AC 63, Address 18 */
#define LCD_PLU_ADDR_24_ADR    122       /* AC 63, Address 19, 2172 */
#define LCD_PLU_ADDR_25_ADR    123       /* AC 63, Address 20, 2172 */
#define LCD_PLU_ADDR_26_ADR    124       /* AC 63, Address 21, 2172 */
#define LCD_PLU_ADDR_27_ADR    125       /* AC 63, Address 22, 2172 */
#define LCD_PLU_ADDR_28_ADR    126       /* AC 63, Address 23, 2172 */
#define LCD_PLU_TYPE1_ADR     127       /* AC 68, Type 1 */
#define LCD_PLU_TYPE2_ADR     128       /* AC 68, Type 2 */
#define LCD_PLU_TYPE3_ADR     129       /* AC 68, Type 3 */
#define LCD_DPT_ADDR1_ADR     130       /* AC 114, Address 1 */
#define LCD_DPT_ADDR2_ADR     131       /* AC 114, Address 2 */
#define LCD_DPT_ADDR3_ADR     132       /* AC 114, Address 3 */
#define LCD_DPT_ADDR4_ADR     133       /* AC 114, Address 4 */
#define LCD_DPT_ADDR5_ADR     134       /* AC 114, Address 5 */
#define LCD_DPT_ADDR6_ADR     135       /* AC 114, Address 6 */
#define LCD_DPT_ADDR7_ADR     136       /* AC 114, Address 7 */
#define LCD_DPT_ADDR8_ADR     137       /* AC 114, Address 8 */
#define LCD_DPT_ADDR9_ADR     138       /* AC 114, Address 9 */
#define LCD_DPT_ADDR10_ADR    139       /* AC 114, Address 10 */
#define LCD_DPT_ADDR11_ADR    140       /* AC 114, Address 11 */
#define LCD_DPT_ADDR12_ADR    141       /* AC 114, Address 12 */
#define LCD_DPT_ADDR13_ADR    142       /* AC 114, Address 13 */
#define LCD_CPN_ADDR1_ADR     143       /* AC 161, Address 1 */
#define LCD_CPN_ADDR2_ADR     144       /* AC 161, Address 2 */
#define LCD_CPN_ADDR3_ADR     145       /* AC 161, Address 3 */
#define LCD_CPN_ADDR4_ADR     146       /* AC 161, Address 4 */
#define LCD_CPN_ADDR5_ADR     147       /* AC 161, Address 5 */
#define LCD_CPN_ADDR6_ADR     148       /* AC 161, Address 6 */
#define LCD_CPN_ADDR7_ADR     149       /* AC 161, Address 7 */
#define LCD_CPN_ADDR8_ADR     150       /* AC 161, Address 8 */
#define LCD_CPN_ADDR9_ADR     151       /* AC 161, Address 9 */
#define LCD_CPN_ADDR10_ADR    152       /* AC 161, Address 10 */
#define LCD_CPN_ADDR11_ADR    153       /* AC 161, Address 11 */
#define LCD_CPN_ADDR12_ADR    154       /* AC 161, Address 12 */
#define LCD_CPN_ADDR13_ADR    155       /* AC 161, Address 13 */
#define LCD_CPN_ADDR14_ADR    156       /* AC 161, Address 14 */
#define LCD_CPN_ADDR15_ADR    157       /* AC 161, Address 15 */
#define LCD_CPN_ADDR16_ADR    158       /* AC 161, Address 16 */
#define LCD_CPN_ADDR17_ADR    159       /* AC 161, Address 17 */
#define LCD_CPN_ADDR18_ADR    160       /* AC 161, Address 18 */
#define LCD_CPN_ADDR19_ADR    161       /* AC 161, Address 19 */
#define LCD_CPN_ADDR20_ADR    162       /* AC 161, Address 20 */
#define LCD_PG1_ADR           163       /* PG 1 */
#define LCD_PG2_ADR           164       /* PG 2 */
#define LCD_PG3_ADR           165       /* PG 3 */
#define LCD_PG6_ADR           166       /* PG 6 */
#define LCD_PG7_ADR           167       /* PG 7 */
#define LCD_PG8_ADR           168       /* PG 8 */
#define LCD_PG9_ADR           169       /* PG 9 */
#define LCD_PG10_ADR          170       /* PG 10 */
#define LCD_PG15_ADR          171       /* PG 15 */
#define LCD_PG17_ADR          172       /* PG 17 */
#define LCD_PG18_ADR          173       /* PG 18 */
#define LCD_PG20_ADR          174       /* PG 20 */
#define LCD_PG21_ADR          175       /* PG 21 */
#define LCD_PG22_ADR          176       /* PG 22 */
#define LCD_PG23_ADR          177       /* PG 23 */
#define LCD_PG39_ADR          178       /* PG 39 */
#define LCD_PG46_ADR          179       /* PG 46 */
#define LCD_PG47_ADR          180       /* PG 47 */
#define LCD_PG48_ADR          181       /* PG 48 */
#define LCD_PG49_ADR          182       /* PG 49 */
#define LCD_PG50_ADR          183       /* PG 50 */
#define LCD_PG54_ADR          184       /* PG 54 */
#define LCD_PG57_ADR          185       /* PG 57 */
#define LCD_PG60_ADR          186       /* PG 60 */
#define LCD_PG61_ADR          187       /* PG 61 */
#define LCD_PG62_ADR          188       /* PG 62 */
#define LCD_PG65_ADR          189       /* PG 65 */
#define LCD_AC10_ADR          190       /* AC 10, 2172 */
#define LCD_AC11_ADR          191       /* AC 11, 2172 */
#define LCD_AC115_ADR         192       /* AC 115, 2172 */
#define LCD_AC170_ADR         193       /* AC 170, 2172 */
#define LCD_AC208_ADR         194       /* AC 208, 2172 */
#define LCD_PLU_ADDR19_ADR    195       /* AC 63, Address 19, 2172 */
#define LCD_PLU_ADDR20_ADR    196       /* AC 63, Address 20, 2172 */
#define LCD_PLU_ADDR21_ADR    197       /* AC 63, Address 21, 2172 */
#define LCD_PLU_ADDR23_ADR    198       /* AC 63, Address 23, 2172 */
#define LCD_DPT_ADDR14_ADR    199       /* AC 114, Address 14, 2172 */
#define LCD_PG51_ADR          200       /* PG 51, 2172 */
#define LCD_AC128_ADR         201       /* AC 128, 2172 */
#define LCD_PLU_ADDR22_ADR    202       /* AC 63, Address 22, 2172 */
#define LCD_PG99_ADR          203       /* PG 99, 2172 */
#define LCD_PG95_ADR          204       /* PG 95, Software Security NHP 1.4 3/28/03 cwunn */
#define LCD_AC777_ADR         205       /* AC 777 Change terminal display layout */
#define LCD_AC888_ADR         206       /* AC 888 Print/Display Version information */
#define LCD_CPN_ADDR21_ADR    207		/* AC 161, Address 21*/
#define LCD_OPTIMIZE_ADR	  208		/* Optimize File */
//GroupReport - CSMALL
#define LCD_AC31_ADR		  209		/* AC 31, PLU Group Summary */
#define LCD_CPN_ADDR22_ADR    210		/* AC 161, Address 22*/
#define LCD_AC194_ADR		  211		/* AC 194, Dynamic Window Reload */
#define LCD_CAS_ADDR11_ADR    212		// AC 20, Set Operator functions
#define LCD_AC102_ADR         213		// AC 102, Mnemonic, CID_AC102FUNCTION, AC_STORE_FORWARD, Status
#define LCD_AC102_SF_TOGGLE	  214		// Toggle Store and Forward 
#define LCD_AC102_SF_REPORT	  215		// Store and Forward Report
#define LCD_AC102_SF_FORWARD  216		// Forward Stored Transactions
#define LCD_AC102_SF_DELETE	  217		// AC 102 Delete failed transactions  Store and Forward
#define LCD_AC102_SF_FAILEDRPT   218	// AC 102 Delete failed transactions  Store and Forward
#define LCD_AC102_SF_DETAILEDRPT 219
#define LCD_AC444_ADR         220		// AC 444, Mnemonic, CID_AC444FUNCTION, AC_JOIN_CLUSTER, Status
#define LCD_AC444_STATUS      221		// AC 444, Status, CID_AC444FUNCTION, AC_JOIN_CLUSTER, Status
#define LCD_AC444_UNJOIN	  222		// AC 444, Unjoin, CID_AC444FUNCTION, AC_JOIN_CLUSTER, Unjoin/Detach/Disconnect/Clear 
#define LCD_AC444_JOIN		  223		// AC 444, Join, CID_AC444FUNCTION, AC_JOIN_CLUSTER, Join/Attach/Connect/Transfer Totals
#define LCD_AC102_ALT_CREDIT  224		// AC102, Electronic Payment Type 1 for Credit etc. which allow Store and Forward
#define LCD_AC102_ALT_EPD     225		// AC102 Electronic Payment Type 2 for EPD, etc. which allow Store and Forward
#define LCD_RPT_DAILYSAVE_ADR 226       /* Previous or Saved Daily Report Common, see also LCD_RPT_DAILY_ADR */
#define LCD_RPT_PDTSAVE_ADR   227       /* Previous or Saved PTD Report Common, see also LCD_RPT_PDT_ADR */

/* Max Address, saratoga, see MAX_MLDMNEMO_SIZE */

#endif
/*====== End of Source ======*/