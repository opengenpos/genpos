/*========================================================================*\
||                                                                        ||
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                Release 1.0               ||
||                                                                        ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:   Title              : NCR 2170 Header Definition                        :
:   Category           : SUPER & PROGRAM MODE,ECR 2170                     :
:   Program Name       : uifsup.h                                          :
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments              :
:   Program Written    : 92-05-04 : 00.00.00     :                         :
:   Update Histories   : 92-11-19 : 01.00.00     : Adds A/C 111,176 Func.  :
:                      : 93-10-05 : 02.00.01     : Adds A/C153             :
:                      : 95-03-01 : 03.00.00     : Adds A/C 161 prototype  :
:                      :          :              : Adds A/C 161 Function   :
:                      : 95-03-14 : 03.00.00     : Adds CID_AC161ADJECTIVE :
:                      : 15-04-04 : 02.02.01     : UifAC1011FscLookupTable name change :
:  ----------------------------------------------------------------------  :
:    Georgia Southern University Research Services Foundation              :
:    donated by NCR to the research foundation in 2002 and maintained here :
:    since.                                                                :
:       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448)                  :
:       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen)        :
:       2012  -> GenPOS Rel 2.2.0                                          :
:       2014  -> GenPOS Rel 2.2.1                                          :
:                                                                          :
:    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0     :
\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*------------------------------------------------------------------------*/
/**
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
**/
/*------------------------------------------------------------------------*\

            T Y P E D E F     S T R U C T U R Es

\*------------------------------------------------------------------------*/

/**
;========================================================================
            E X T E R N A L    D E C L A R A T I O N s                        
;========================================================================
**/

extern UCHAR   uchMaintMenuPage;            /* Menu Shift Page */

typedef struct {
	int nFsc;
	int nVal;
	int nMaint;
}  TenderFscLookup;

extern TenderFscLookup UifAC1011FscLookupTable [];


/*------------------------------------------------------------------------*\

            D E F I N E    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*\
    Define - Function Name 
\*------------------------------------------------------------------------*/
                                            
#define CID_ACMODE              1               /* UifACMode() */            
#define CID_ACMODEIN            2               /* UifACModeIn() */
#define CID_ACENTER             3               /* UifACEnter() */
#define CID_AC1FUNCTION         4               /* UifAC1Function() */
#define CID_AC1ENTERDATA        5               /* UifAC1EnterData() */
#define CID_AC2FUNCTION         6               /* UifAC2Function() */
#define CID_AC2ENTERTYPE        7               /* UifAC2EnterType() */
#define CID_AC2ENTERDATA1       8               /* UifAC2EnterData1() */
#define CID_AC2ENTERDATA2       9               /* UifAC2EnterData2() */
#define CID_AC3FUNCTION         10              /* UifAC3Function() */
#define CID_AC3ENTERTYPE        11              /* UifAC3EnterType() */
#define CID_AC3ENTERDATA1       12              /* UifAC3EnterData1() */
#define CID_AC3ENTERDATA2       13              /* UifAC3EnterData2() */
#define CID_AC3ENTERKEY         14              /* UifAC3EnterKey() */
#define CID_AC4FUNCTION         15              /* UifAC4Function() */
#define CID_AC4ISSUERPT         16              /* UifAC4IssueRpt() */
#define CID_AC4ENTERDATA1       17              /* UifAC4EnterData1() */
#define CID_AC4ENTERDATA2       18              /* UifAC4EnterData2() */
#define CID_AC4EXITFUNCTION     19              /* UifAC4ExitFunction() */
#define CID_AC5FUNCTION         20              /* UifAC5Function() */
#define CID_AC5ENTERADDR        21              /* UifAC5EnterAddr() */
#define CID_AC5ENTERDATA1       22              /* UifAC5EnterData1() */
#define CID_AC5ENTERDATA2       23              /* UifAC5EnterData2() */
#define CID_AC6FUNCTION         24              /* UifAC6Function() */
#define CID_AC6ENTERADDR        25              /* UifAC6EnterAddr() */
#define CID_AC6ENTERDATA1       26              /* UifAC6EnterData1() */
#define CID_AC6ENTERDATA2       27              /* UifAC6EnterData2() */
#define CID_AC7FUNCTION         28              /* UifAC7Function() */
#define CID_AC7ISSUERPT         29              /* UifAC7IssueRpt() */
#define CID_AC7ENTERDATA1       30              /* UifAC7EnterData1() */
#define CID_AC7ENTERDATA2       31              /* UifAC7EnterData2() */
#define CID_AC9FUNCTION         32              /* UifAC9Function() */
#define CID_AC9ENTERDATA1       33              /* UifAC9EnterData1() */
#define CID_AC9ENTERDATA2       34              /* UifAC9EnterData2() */
#define CID_AC9ENTERDATA3       35              /* UifAC9EnterData3() */
#define CID_AC9ENTERDATA4       36              /* UifAC9EnterData4() */
#define CID_AC15FUNCTION        37              /* UifAC15Function() */
#define CID_AC15ENTERDATA       38              /* UifAC15EnterData() */
#define CID_AC17FUNCTION        39              /* UifAC17Function() */
#define CID_AC17ENTERADDR       40              /* UifAC17EnterAddr() */
#define CID_AC17ENTERDATA1      41              /* UifAC17EnterData1() */
#define CID_AC17ENTERDATA2      42              /* UifAC17EnterData2() */
#define CID_AC18FUNCTION        43              /* UifAC18Function() */
#define CID_AC18ENTERRSTTYPE    44              /* UifAC18EnterRstType() */
#define CID_AC18ENTERKEY        45              /* UifAC18EnterKey() */
#define CID_AC19FUNCTION        46              /* UifAC19Function() */
#define CID_AC19ENTERRSTTYPE    47              /* UifAC19EnterRstType() */
#define CID_AC19ENTERRPTTYPE    48              /* UifAC19EnterRptType() */
#define CID_AC19ENTERDATA       49              /* UifAC19EnterData() */
#define CID_AC19ENTERKEY        50              /* UifAC19EnterKey() */
#define CID_AC21FUNCTION        56              /* UifAC21Function() */
#define CID_AC21ENTERDATA       57              /* UifAC21EnterData() */
#define CID_AC23FUNCTION        58              /* UifAC23Function() */
#define CID_AC23ENTERDATATYPE   59              /* UifAC23EnterDataType() */
#define CID_AC24FUNCTION        60              /* UifAC24Function() */
#define CID_AC24ENTERDATATYPE   61              /* UifAC24EnterDataType() */
#define CID_AC24ENTERRPTTYPE    62              /* UifAC24EnterRptType() */
#define CID_AC24ENTERDATA1      63              /* UifAC24EnterData1() */
#define CID_AC24ENTERDATA2      64              /* UifAC24EnterData2() */
#define CID_AC26FUNCTION        65              /* UifAC26Function() */
#define CID_AC26ENTERDATATYPE   66              /* UifAC26EnterDataType() */
#define CID_AC26ENTERRPTTYPE    67              /* UifAC26EnterRptType() */
#define CID_AC26ISSUEALLREPORT  68              /* UifAC26IssueAllReport() */
#define CID_AC26ENTERMDEPT1     69              /* UifAC26EnterMDEPT1() */
#define CID_AC26ENTERDEPT1      70              /* UifAC26EnterDEPT1() */
#define CID_AC26ENTERMDEPT2     71              /* UifAC26EnterMDEPT2() */
#define CID_AC26ENTERDEPT2      72              /* UifAC26EnterDEPT2() */
#define CID_AC29FUNCTION        73              /* UifAC29Function() */
#define CID_AC29ENTERDATATYPE   74              /* UifAC29EnterDataType() */
#define CID_AC29ENTERRPTTYPE    75              /* UifAC29EnterRptType() */
#define CID_AC29ISSUEALLREPORT  76              /* UifAC29IssueAllReport() */
#define CID_AC29ENTERDEPT1      77              /* UifAC29EnterDEPT1() */
#define CID_AC29ENTERRPTCODE1   78              /* UifAC29EnterRptCode1() */
#define CID_AC29ENTERPLU1       79              /* UifAC29EnterPLU1() */
#define CID_AC29ENTERDEPT2      80              /* UifAC29EnterDEPT2() */
#define CID_AC29ENTERRPTCODE2   81              /* UifAC29EnterRptCode2() */
#define CID_AC29ENTERPLU2       82              /* UifAC29EnterPLU2() */
#define CID_AC39FUNCTION        83              /* UifAC39Function() */
#define CID_AC39ENTERDATATYPE   84              /* UifAC39EnterDataType() */
#define CID_AC39ENTERRSTTYPE    85              /* UifAC39EnterRstType() */
#define CID_AC39ENTERKEY        86              /* UifAC39EnterKey() */
#define CID_AC41FUNCTION        87              /* UifAC41Function() */
#define CID_AC41ENTERDATATYPE   88              /* UifAC41EnterDataType() */
#define CID_AC41ENTERRPTTYPE    89              /* UifAC41EnterRptType() */
#define CID_AC41ENTERDATA       90              /* UifAC41EnterData() */
#define CID_AC42FUNCTION        91              /* UifAC42Function() */
#define CID_AC42ENTERKEY        92              /* UifAC42EnterKey() */
#define CID_AC46FUNCTION        93              /* UifAC46Function() */
#define CID_AC46ENTERDATATYPE   94              /* UifAC46EnterDataType() */
#define CID_AC46ENTERRSTTYPE    95              /* UifAC46EnterRstType() */
#define CID_AC46ENTERRPTTYPE    96              /* UifAC46EnterRptType() */
#define CID_AC46ENTERDATA       97              /* UifAC46EnterData() */
#define CID_AC46ENTERKEY        98              /* UifAC46EnterKey() */
#define CID_AC50FUNCTION        99              /* UifAC50Function() */
#define CID_AC50ENTERDATA1      100             /* UifAC50EnterData1() */
#define CID_AC50ENTERALPHA      101             /* UifAC50EnterALPHA() */
#define CID_AC50CHANGEKB        102             /* UifAC50ChangeKB() */
#define CID_AC50ENTERDATA2      103             /* UifAC50EnterData2() */
#define CID_AC50ENTERDATA3      104             /* UifAC50EnterData3() */
#define CID_AC50ENTERDATA4      105             /* UifAC50EnterData4() */
#define CID_AC63FUNCTION        106             /* UifAC63Function() */
#define CID_AC63ENTERDATA1      107             /* UifAC63EnterData1() */
#define CID_AC63ENTERDATA2      108             /* UifAC63EnterData2() */
#define CID_AC63INTERVATION     109             /* UifAC63Intervation() */
#define CID_AC63CANCELFUNCTION  110             /* UifAC63CancelFunction() */
#define CID_AC63ENTERALPHA      111             /* UifAC63EnterALPHA() */
#define CID_AC63EXITFUNCTION    112             /* UifAC63ExitFunction() */
#define CID_AC64FUNCTION        113             /* UifAC64Function() */
#define CID_AC64ENTERDATA1      114             /* UifAC64EnterData1() */
#define CID_AC64ENTERDATA2      115             /* UifAC64EnterData2() */
#define CID_AC64ERRORCORRECT    117             /* UifAC64ErrorCorrect() */
#define CID_AC66FUNCTION        118             /* UifAC66Function() */
#define CID_AC66ENTERKEY        119             /* UifAC66EnterKey() */
#define CID_AC68FUNCTION        120             /* UifAC68Function() */
#define CID_AC68ENTERDATA1      121             /* UifAC68EnterData1() */
#define CID_AC68ENTERPLUTYPE    122             /* UifAC68EnterPLUType() */
#define CID_AC68ENTERDATA3      123             /* UifAC68EnterData3() */
#define CID_AC68INTERVATION     124             /* UifAC68Intervation() */
#define CID_AC68ENTERALPHA      125             /* UifAC68EnterALPHA() */
#define CID_AC70FUNCTION        126             /* UifAC70Function() */
#define CID_AC72FUNCTION        127             /* UifAC72Function() */
#define CID_AC75FUNCTION        128             /* UifAC75Function() */
#define CID_AC75ENTERKEY        129             /* UifAC75EnterKey() */
#define CID_AC82FUNCTION        130             /* UifAC82Function() */
#define CID_AC82ENTERDATA       131             /* UifAC82EnterData() */
#define CID_AC82ENTERALPHA      132             /* UifAC82EnterALPHA() */
#define CID_AC84FUNCTION        133             /* UifAC84Function() */
#define CID_AC84ENTERADDR       134             /* UifAC84EnterAddr() */
#define CID_AC84ENTERDELIMIT1   135             /* UifAC84EnterDelimit1() */
#define CID_AC84ENTERDELIMIT2   136             /* UifAC84EnterDelimit2() */
#define CID_AC84ENTERPOSITION   137             /* UifAC84EnterPosition() */
#define CID_AC84ENTERDATA2      138             /* UifAC84EnterData2() */
#define CID_AC85FUNCTION        139             /* UifAC85Function() */
#define CID_AC85ENTERKEY        140             /* UifAC85EnterKey() */
#define CID_AC86FUNCTION        141             /* UifAC86Function() */
#define CID_AC86ENTERADDR       142             /* UifAC86EnterAddr() */
#define CID_AC86ENTERDATA1      143             /* UifAC86EnterData1() */
#define CID_AC86ENTERDATA2      144             /* UifAC86EnterData2() */
#define CID_AC88FUNCTION        145             /* UifAC88Function() */
#define CID_AC88ENTERALPHA      146             /* UifAC88EnterALPHA() */
#define CID_AC88CHANGEKB1       147             /* UifAC88ChangeKB1() */
#define CID_AC88ISSUERPT        148             /* UifAC88IssueRpt() */
#define CID_AC88ENTERADDR       149             /* UifAC88EnterAddr() */
#define CID_AC88CHANGEKB2       150             /* UifAC88ChangeKB2() */
#define CID_AC89FUNCTION        151             /* UifAC89Function() */
#define CID_AC89ENTERADDR       152             /* UifAC89EnterAddr() */
#define CID_AC89ENTERDATA1      153             /* UifAC89EnterData1() */
#define CID_AC89ENTERDATA2      154             /* UifAC89EnterData2() */
#define CID_AC99FUNCTION        155             /* UifAC99Function() */
#define CID_AC99ENTERRSTTYPE    156             /* UifAC99EnterRstType() */
#define CID_AC99ENTERKEY        157             /* UifAC99EnterKey() */
#define CID_AC100FUNCTION       158             /* UifAC100Function() */
#define CID_AC100ENTERKEY       159                /* UifAC100EnterKey() */
#define CID_AC103FUNCTION       160             /* UifAC103Function() */
#define CID_AC103ENTERDATA      161             /* UifAC103EnterData() */
#define CID_AC103EXITFUNCTION   162             /* UifAC103ExitFunction() */
#define CID_AC111FUNCTION       163             /* UifAC111Function() */
#define CID_AC111ENTERADDR      164             /* UifAC111EnterAddr() */
#define CID_AC111ENTERDATA1     165             /* UifAC111EnterData1() */
#define CID_AC111ENTERDATA2     166             /* UifAC111EnterData2() */
#define CID_AC114FUNCTION       167             /* UifAC114Function() */
#define CID_AC114ENTERDATA1     168             /* UifAC114EnterData1() */
#define CID_AC114ENTERDATA2     169             /* UifAC114EnterData2() */
#define CID_AC114ENTERDATA3     170             /* UifAC114EnterData3() */
#define CID_AC114ENTERALPHA     171             /* UifAC114EnterALPHA() */
#define CID_AC114ERRORCORRECT   172             /* UifAC114ErrorCorrect() */
#define CID_AC116FUNCTION       173             /* UifAC116Function() */
#define CID_AC116ISSUERPT       174             /* UifAC116IssueRpt() */
#define CID_AC116ENTERDATA1     175             /* UifAC116EnterData1() */
#define CID_AC116ENTERDATA2     176             /* UifAC116EnterData2() */
#define CID_AC116ENTERADJECT1   177             /* UifAC116EnterAdject1() */
#define CID_AC116ENTERADJECT2   178             /* UifAC116EnterAdject2() */
#define CID_AC116EXITFUNCTION   179             /* UifAC116ExitFunction() */
#define CID_AC119FUNCTION       180             /* UifAC119Function() */
#define CID_AC119ENTERDATA      181             /* UifAC119EnterData() */
#define CID_AC119EXITFUNCTION   182             /* UifAC119ExitFunction() */
#define CID_AC122FUNCTION       183             /* UifAC122Function() */
#define CID_AC122ENTERDATATYPE  184             /* UifAC123EnterDataType() */ 
#define CID_AC123FUNCTION       185             /* UifAC123Function() */
#define CID_AC123ENTERDATATYPE  186             /* UifAC123EnterDataType() */ 
#define CID_AC124FUNCTION       187             /* UifAC124Function() */
#define CID_AC124ENTERRATE      188             /* UifAC124EnterRate() */
#define CID_AC124ENTERAMOUNT1   189             /* UifAC124EnterAmount1() */
#define CID_AC124ENTERTAX       190             /* UifAC124EnterTax() */
#define CID_AC124ENTERKEY       191             /* UifAC124EnterKey() */
#define CID_AC124ENTERAMOUNT2   192             /* UifAC124EnterAmount2() */
#define CID_AC124ENTERTEST      193             /* UifAC124EnterTest() */
#define CID_AC125FUNCTION       194             /* UifAC125Function() */
#define CID_AC125ENTERRATE      195             /* UifAC125EnterRate() */
#define CID_AC125ENTERAMOUNT1   196             /* UifAC125EnterAmount1() */
#define CID_AC125ENTERTAX       197             /* UifAC125EnterTax() */
#define CID_AC125ENTERKEY       198             /* UifAC125EnterKey() */
#define CID_AC125ENTERAMOUNT2   199             /* UifAC125EnterAmount2() */
#define CID_AC125ENTERTEST      200             /* UifAC125EnterTest() */
#define CID_AC126FUNCTION       201             /* UifAC126Function() */
#define CID_AC126ENTERRATE      202             /* UifAC126EnterRate() */
#define CID_AC126ENTERAMOUNT1   203             /* UifAC126EnterAmount1() */
#define CID_AC126ENTERTAX       204             /* UifAC126EnterTax() */
#define CID_AC126ENTERKEY       205             /* UifAC126EnterKey() */
#define CID_AC126ENTERAMOUNT2   206             /* UifAC126EnterAmount2() */
#define CID_AC126ENTERTEST      207             /* UifAC126EnterTest() */
#define CID_AC127FUNCTION       208             /* UifAC127Function() */
#define CID_AC127ENTERADDR      209             /* UifAC127EnterAddr() */
#define CID_AC127ENTERDATA1     210             /* UifAC127EnterData1() */
#define CID_AC127ENTERDATA2     211             /* UifAC127EnterData2() */
#define CID_AC146FUNCTION       212             /* UifAC146Function() */
#define CID_AC146ENTERDATA      213             /* UifAC146EnterData() */
#define CID_AC146EXITFUNCTION   214             /* UifAC146ExitFunction() */
#define CID_AC169FUNCTION       215             /* UifAC169Function() */
#define CID_AC169ENTERADDR      216             /* UifAC169EnterAddr() */
#define CID_AC169ENTERDATA1     217             /* UifAC169EnterData1() */
#define CID_AC175FUNCTION       218             /* UifAC175Function() */
#define CID_AC175ENTERKEY       219             /* UifAC175EnterKey() */
#define CID_AC176FUNCTION       220             /* UifAC176Function() */
#define CID_ACCHANGEKB1         221             /* UifACChangeKB1() */
#define CID_ACCHANGEKB2         222             /* UifACChangeKB2() */
#define CID_ACEXITFUNCTION      223             /* UifACExitFunction() */
#define CID_ACSHIFT             224             /* UifACShift() */
#define CID_AC63ERRORFUNCTION   225             /* UifAC63ErrorFunction() */
#define CID_AC64ERRORFUNCTION   226             /* UifAC64ErrorFunction() */
#define CID_AC114CANCELFUNCTION 227             /* UifAC114CancelFunction() */
#define CID_ACCANCEL            228             /* UifACCancel() */
#define CID_AC150FUNCTION       229             /* UifAC150Function () */
#define CID_AC150ENTERDATA      230             /* UifAC150Function() */
#define CID_AC150ENTERDATA1     231             /* UifAC150Function() */
#define CID_AC151FUNCTION       232             /* UifAC151Function() */
#define CID_AC151ENTERKEY       233             /* UifAC151Function() */
#define CID_AC151ENTERRSTTYPE   234             /* UifAC151EnterRstType */
#define CID_AC152FUNCTION       235             /* UifAC152Function() */
#define CID_AC152ENTERDATA1     236             /* UifAC152Function() */
#define CID_AC152ENTERDATA2     237             /* UifAC152EnterData2 */ 
#define CID_AC152ENTERDATA3     238             /* UifAC152EnterData3 */
#define CID_AC152ENTERDATA4     239             /* UifAC152EnterData4 */
#define CID_AC87FUNCTION        240             /* UifAC87Function() */
#define CID_AC87ENTERALPHA      241             /* UifAC87EnterALPHA() */
#define CID_AC87CHANGEKB1       242             /* UifAC87ChangeKB1() */
#define CID_AC87ISSUERPT        243             /* UifAC87IssueRpt() */
#define CID_AC87ENTERADDR       244             /* UifAC87EnterAddr() */
#define CID_AC87CHANGEKB2       245             /* UifAC87ChangeKB2() */
#define CID_AC153FUNCTION       246             /* UifAC153Function() */
#define CID_AC153ENTERDATA      247             /* UifAC153EnterData() */
#define CID_AC153ENTEREMPNO     248             /* UifAC153EnterEmpNo() */
#define CID_AC153CANCELFUNCTION 249             /* UifAC153CancelFunction() */

#define CID_AC78FUNCTION        250             /* UifAC78Fucntino() */
#define CID_AC78ENTERDATA       251             /* UifAC78EnterData() */
#define CID_AC97FUNCTION        252             /* UifAC97Fucntion() */
#define CID_AC97ENTERRSTTYPE    253             /* UifAC97EnterRstType() */
#define CID_AC97ENTERKEY        254             /* UifAC97EnterKey() */

#define CID_AC160FUNCTION       255             /* UifAC160Fucntion() */
#define CID_AC160ENTERADDR      256             /* UifAC160EnterAddr() */
#define CID_AC160ENTERDATA      257             /* UifAC160EnterData() */

#define CID_AC161FUNCTION       258             /* UifAC161Function() */
#define CID_AC161ENTERDATA1     259             /* UifAC161EnterData1() */
#define CID_AC161ENTERDATA2     260             /* UifAC161EnterData2() */
#define CID_AC161ENTERALPHA     261             /* UifAC161EnterALPHA() */
#define CID_AC161ERRORFUNCTION  262             /* UifAC161ErrorFunction() */
#define CID_AC161CANCELFUNCTION 263             /* UifAC161CancelFunction() */
#define CID_AC161EXITFUNCTION   264             /* UifAC161ExitFunction() */
#define CID_AC161ADJECTIVE      265             /* UifAC161Adjective() */

#define CID_AC30FUNCTION        266             /* UifAC30Function() */
#define CID_AC30ENTERRPTTYPE    267             /* UifAC30EnterRptType() */
#define CID_AC30ENTERDATATYPE   268             /* UifAC30EnterDataType() */
#define CID_AC30ENTERDATA       269             /* UifAC30EnterData() */

#define CID_AC40FUNCTION        270             /* UifAC40Function() */
#define CID_AC40ENTERRSTTYPE    271             /* UifAC40EnterRstType() */
#define CID_AC40ENTERDATATYPE   272             /* UifAC40EnterDataType() */
#define CID_AC40ENTERKEY        273             /* UifAC40EnterKey() */

#define CID_AC162FUNCTION       274             /* UifAC162Fucntion()  */
#define CID_AC162ENTERADDR      275             /* UifAC162EnterAddr() */
#define CID_AC162ENTERDATA      276             /* UifAC162EnterData() */

#define CID_AC33FUNCTION        277             /* UifAC33Function() */
#define CID_AC33ENTERDATA1      278             /* UifAC33EnterData1() */
#define CID_AC33ENTERDATA2      279             /* UifAC33EnterData2() */
#define CID_AC33ENTERDATA3      280             /* UifAC33EnterData3() */
#define CID_AC33ERRORFUNCTION   281             /* UifAC33ErrorFunction() */
#define CID_AC33CANCELFUNCTION  282             /* UifAC33CancelFunction() */

#define CID_AC152ENTERALPHA     283             /* UifAC152EnterALPHA() */

#define CID_AC71FUNCTION        284             /* UifAC71Function() */
#define CID_AC71ENTERDATA1      285             /* UifAC71EnterData1() */
#define CID_AC71ENTERDATA2      286             /* UifAC71EnterData2() */
#define CID_AC71ERRORCORRECT    287             /* UifAC71ErrorCorrect() */
#define CID_AC71CANCELFUNCTION  288             /* UifAC71CancelFunction() */

#define CID_AC133FUNCTION       289             /* UifAC133Function() */
#define CID_AC133ENTERADDR      290             /* UifAC133EnterAddr() */
#define CID_AC133ENTERDATA1     291             /* UifAC133EnterData1() */
#define CID_AC133ENTERDATA2     292             /* UifAC133EnterData2() */

#define CID_AC131FUNCTION       293             /* UifAC131Function() */
#define CID_AC131ENTERDATATYPE  294             /* UifAC131EnterDataType() */

#define CID_AC132FUNCTION       295             /* UifAC132Function() */
#define CID_AC132ENTERDATATYPE  296             /* UifAC132EnterDataType() */
#define CID_AC132ENTERRSTTYPE   297             /* UifAC132EnterRstType() */
#define CID_AC132ENTERKEY       298             /* UifAC132EnterKey() */

#define CID_AC223FUNCTION       299             /* UifAC223Function() */
#define CID_AC223ENTERDATA      300             /* UifAC22EnterData() */

#define CID_AC233FUNCTION       301              /* UifAC233Function() */
#define CID_AC233ENTERRSTTYPE   302              /* UifAC233EnterRstType() */
#define CID_AC233ENTERRPTTYPE   303              /* UifAC233EnterRptType() */
#define CID_AC233ENTERDATA      304              /* UifAC233EnterData() */
#define CID_AC233ENTERKEY       305              /* UifAC233EnterKey() */

#define CID_AC129FUNCTION       306             /* UifAC129Function() */
#define CID_AC129ENTERRATE      307             /* UifAC129EnterRate() */
#define CID_AC129ENTERAMOUNT1   308             /* UifAC129EnterAmount1() */
#define CID_AC129ENTERTAX       309             /* UifAC129EnterTax() */
#define CID_AC129ENTERKEY       310             /* UifAC129EnterKey() */
#define CID_AC129ENTERAMOUNT2   311             /* UifAC129EnterAmount2() */
#define CID_AC129ENTERTEST      312             /* UifAC129EnterTest() */

#define CID_AC130FUNCTION       313             /* UifAC130Function() */
#define CID_AC130ENTERADDR      314
#define CID_AC130ENTERDATA1     315             /* UifAC130ENTERDATA1() */
#define CID_AC130ENTERDATA2     316             /* UifAC130ENTERDATA2() */

#define CID_AC154FUNCTION       317             /* UifAC154Function() */
#define CID_AC154ENTERADDR      318             /* UifAC154EnterAddr() */
#define CID_AC154ENTERDATA1     319             /* UifAC154EnterData1() */
#define CID_AC154ENTERDATA2     320             /* UifAC154EnterData2() */

#define CID_AC135FUNCTION       321             /* UifAC135Function() */
#define CID_AC135ENTERDAILYPTD  322             /* UifAC135EnterDailyPTD() */
#define CID_AC135ENTERFORMATTYPE 323            /* UifAC135EnterFormatType() */
#define CID_AC135ENTERDEPTTYPE  324             /* UifAC135EnterDeptType() */
#define CID_AC135ENTERPLUTYPE   325             /* UifAC135EnterPLUType() */
#define CID_AC135ENTERCPNTYPE   326             /* UifAC135EnterCpnType() */
#define CID_AC135ENTERCASTYPE   327             /* UifAC135EnterCasType() */
#define CID_AC135ENTERWAITYPE   328             /* UifAC135EnterWaiType() */
#define CID_AC135ENTERINDFINTYPE 329            /* UifAC135EnterIndFinType() */
#define CID_AC135ENTERMDEPT1    330             /* UifAC135EnterMDEPT1() */
#define CID_AC135ENTERDEPT1     331             /* UifAC135EnterDEPT1() */
#define CID_AC135ENTERDEPTPLU1  332             /* UifAC135EnterDEPTPLU1() */
#define CID_AC135ENTERPLUCODE1  333             /* UifAC135EnterPLUCode1() */
#define CID_AC135ENTERPLU1      334             /* UifAC135EnterPLU1() */

#define CID_AC136FUNCTION       335             /* UifAC136Function() */
#define CID_AC136ENTERDAILYPTD  336             /* UifAC136EnterDailyPTD() */
#define CID_AC136ENTERFORMATTYPE 337            /* UifAC136EnterFormatType() */
#define CID_AC136ENTERRSTTYPE 338               /* UifAC136EnterRstType() */
#define CID_AC136ENTERCASTYPE   339             /* UifAC136EnterCasType() */
#define CID_AC136ENTERWAITYPE   340             /* UifAC136EnterWaiType() */
#define CID_AC136ENTERINDFINTYPE 341            /* UifAC136EnterIndFinType() */

#define CID_AC137FUNCTION       342             /* UifAC137Function() */
#define CID_AC137ENTERADDR      343             /* UifAC137EnterAddr() */
#define CID_AC137ENTERDATA1     344             /* UifAC137EnterData1() */
#define CID_AC137ENTERDATA2     345             /* UifAC137EnterData2() */
#define CID_AC137ENTERDATA3     346             /* UifAC137EnterData3() */

#define CID_AC20FUNCTION        347              /* UifAC20Function() */
#define CID_AC20ENTERDATA1      348              /* UifAC20EnterData1() */
#define CID_AC20ENTERDATA2      349              /* UifAC20EnterData2() */
#define CID_AC20ENTERDATA3      350              /* UifAC20EnterData3() */
#define CID_AC20ENTERDATA4      351              /* UifAC20EnterData4() */
#define CID_AC20GCSNUMBER       352              /* UifAC20EnterGstChkStartNo() */
#define CID_AC20GCENUMBER       353              /* UifAC20EnterGstChkEndNo() */
#define CID_AC20CHARGETIP       354              /* UifAC20EnterChgTipRate() */
#define CID_AC20ENTERTEAMNO     355              /* UifAC20EnterTeamNo() */
#define CID_AC20ENTERTERMINAL   356              /* UifAC20EnterTerminal() */
#define CID_AC20ENTERALPHA      357              /* UifAC20EnterALPHA() */
#define CID_AC20ERRORCORRECT    358              /* UifAC20ErrorCorrect() */
#define CID_AC20CANCELFUNCTION  359              /* UifAC20CANCELFUNC() */

#define CID_AC19ENTERDATATYPE   360              /* UifAC19EnterDataType() */
#define CID_AC21ENTERDATATYPE   361              /* UifAC21EnterDataType() */

#define CID_AC115FUNCTION       362              /* UifAC115Function(), 2172 */
#define CID_AC115ISSUERPT       363              /* UifAC115IssueRpt(), 2172 */
#define CID_AC115ENTERDATA1     364              /* UifAC115EnterData1(), 2172 */
#define CID_AC115ENTERDATA2     365              /* UifAC115EnterData2(), 2172 */
#define CID_AC115EXITFUNCTION   366              /* UifAC115ExitFunction(), 2172 */

#define CID_AC170FUNCTION       367              /* UifAC170Function() 2172 */
#define CID_AC170ENTERADDR      368              /* UifAC170EnterAddr() 2172 */
#define CID_AC170ENTERDATA1     369              /* UifAC170EnterData1() 2172 */
#define CID_AC170ENTERDATA2     370              /* UifAC170EnterData2() 2172 */

#define CID_AC208FUNCTION       371              /* UifAC208Function() 2172 */
#define CID_AC208ENTERADDR      372              /* UifAC208EnterAddr() 2172 */
#define CID_AC208ENTERDATA1     373              /* UifAC208EnterData1() 2172 */
#define CID_AC208ENTERDATA2     374              /* UifAC208EnterData2() 2172 */

#define CID_AC10FUNCTION        375              /* UifAC10Function(),  Saratoga */
#define CID_AC10ENTERCASHIER    376              /* UifAC10EnterCashier() */
#define CID_AC10ENTERAMOUNT     377              /* UifAC10EnterAmount() */
#define CID_AC10ERRORCORRECT    378              /* UifAC10ErrorCorrect() */
#define CID_AC11FUNCTION        379              /* UifAC11Function() */
#define CID_AC11ENTERCASHIER    380              /* UifAC11EnterCashier() */
#define CID_AC11ENTERAMOUNT     381              /* UifAC11EnterAmount() */
#define CID_AC11ERRORCORRECT    382              /* UifAC11ErrorCorrect() */
#define CID_AC10EXIT            383              /* UifAC10Exit() */
#define CID_AC11EXIT            384              /* UifAC11Exit() */

#define CID_AC128FUNCTION       385              /* UifAC128Function() */
#define CID_AC128ENTERDATA1     386              /* UifAC128EnterData1() */
#define CID_AC128ENTERDATA2     387              /* UifAC128EnterData2() */
#define CID_AC128ENTERADDR      388              /* UifAC128EnterAddr() */

#define CID_AC999FUNCTION		389				 // UifAC999Function()
#define CID_AC999ENTERKEY		390				 // UifAC999EnterKey()

#define CID_AC888FUNCTION		391				 // UifAC888Function()
#define CID_AC777FUNCTION		392				 // UifAC777Function()

#define CID_AC985FUNCTION		393				 // UifAC985Function()
#define CID_AC985ENTERKEY		394				 // UifAC985EnterKey()

#define CID_AC275FUNCTION		395				 // UifAC275Function() DB EOD Function JHHJ
#define CID_AC275ENTERKEY		396				 // UifAC275EnterKey() DB EOD Function JHHJ

#define CID_AC21ENTERUSERTYPE   397              /* UifAC21EnterUserType() JHHJ SR 856*/ 

#define CID_AC3ENTERDATA4       398              /* UifAC3EnterData4() */

//GroupReport - CSMALL
#define CID_AC31FUNCTION        399             /* UifAC31Function() */
#define CID_AC31ENTERRPTTYPE    400             /* UifAC31EnterRptType() */
#define CID_AC31ENTERDATATYPE   401             /* UifAC31EnterDataType() */
#define CID_AC31ENTERDATA       402             /* UifAC31EnterData() */
#define CID_AC276FUNCTION		403				 // UifAC276Function() DB EOD Function JHHJ
#define CID_AC276ENTERKEY		404				 // UifAC276EnterKey() DB EOD Function JHHJ			
// Dynamic Window Reload
#define CID_AC194FUNCTION		405				// UifAC194Function() CSMALL
#define CID_AC444FUNCTION       406             // UifAC444Function() Join/Disjoin from a cluster, ACNo444[]
#define CID_AC444ENTERKEY       407             // UifAC444EnterKey()
// Store and Forward
#define CID_AC102FUNCTION       408             /* UifAC102Function() */
#define CID_AC102ENTERTYPE      409				/* UifAC102EnterType() */
#define CID_AC102ENTERDATA1     410             /* UifAC102Report() */
#define CID_AC102ENTERDATA2     411             /* UifAC102Report() */
#define CID_AC102ENTERSUBMENU   412				/* UifAC102EnterTypeSubMenu() */
/*------------------------------------------------------------------------*\
            REDISPLAY MODE
\*------------------------------------------------------------------------*/
#define STS_AC_MODE         1                   /* Redisplay "SUPER" */
#define STS_AC_ENTER        2                   /* Redisplay "ENTER A/C" */
/*------------------------------------------------------------------------*\
            CONTROL STRING EXECUTION MODE (AT ACMODEIN)
\*------------------------------------------------------------------------*/
#define STS_CSTRING_EXEC    0                   /* Execute OK */
#define STS_CSTRING_ERROR   1                   /* Execute Error */
/*------------------------------------------------------------------------*\
            AC 102 Store and Forward
\*------------------------------------------------------------------------*/
#define ERROR_STORED_TRANS  (-1)
#define ERROR_DELETE		(-2)


/* Define Control Bit state indicators used with
       usUifPGControl - see file uifpgmod.c (Program Mode)
	   usUifACControl - see file uifacmod.c (Supervisor Mode)
*/

#define UIF_SHIFT_PAGE          0x0001          /* Shift Page Case */
#define UIF_VOID                0x0002          /* Void Key Case */
#define UIF_NUM_TYPE            0x0004          /* Number Type Key Case */
#define UIF_EC                  0x0008          /* Error Correct Key Case */
#define UIF_AC68_DEL            0x0010          /* A/C 68 Deletion Case */
#define UIF_ACCESSED            0x0020          /* After Maintenance Case */


/*------------------------------------------------------------------------*\

             P R O T O T Y P E    D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

SHORT   UifACMode(KEYMSG *pKeyMsg);
SHORT   UifACModeIn(KEYMSG *pKeyMsg);
SHORT   UifACEnter(KEYMSG *pKeyMsg);
SHORT   UifACCancel(KEYMSG *pKeyMsg);
SHORT   UifAC1Function(KEYMSG *pKeyMsg);
SHORT   UifAC1EnterData(KEYMSG *pKeyMsg);
SHORT   UifAC2Function(KEYMSG *pKeyMsg);
SHORT   UifAC2EnterType(KEYMSG *pKeyMsg); 
SHORT   UifAC2EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC2EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC3Function(KEYMSG *pKeyMsg);
SHORT   UifAC3EnterType(KEYMSG *pKeyMsg); 
SHORT   UifAC3EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC3EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC3EnterData4(KEYMSG *pKeyMsg);
SHORT   UifAC3EnterKey(KEYMSG *pKeyMsg); 
SHORT   UifAC4Function(KEYMSG *pKeyMsg);
SHORT   UifAC4IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifAC4EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC4EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC4ExitFunction(KEYMSG *pKeyMsg);
SHORT   UifAC5Function(KEYMSG *pKeyMsg);
SHORT   UifAC5EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC5EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC5EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC6Function(KEYMSG *pKeyMsg);
SHORT   UifAC6EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC6EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC6EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC7Function(KEYMSG *pKeyMsg);
SHORT   UifAC7IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifAC7EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC7EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC9Function(KEYMSG *pKeyMsg);
SHORT   UifAC9EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC9EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC9EnterData3(KEYMSG *pKeyMsg);
SHORT   UifAC9EnterData4(KEYMSG *pKeyMsg);
SHORT   UifAC15Function(KEYMSG *pKeyMsg);
SHORT   UifAC15EnterData(KEYMSG *pKeyMsg);
SHORT   UifAC17Function(KEYMSG *pKeyMsg);
SHORT   UifAC17EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC17EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC17EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC18Function(KEYMSG *pKeyMsg);
SHORT   UifAC18EnterRstType(KEYMSG *pKeyMsg); 
SHORT   UifAC18EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC19Function(KEYMSG *pKeyMsg);
SHORT   UifAC19EnterRstType(KEYMSG *pKeyMsg);
SHORT   UifAC19EnterRptType(KEYMSG *pKeyMsg);
SHORT   UifAC19EnterData(KEYMSG *pKeyMsg); 
SHORT   UifAC19EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC19EnterDataType(KEYMSG *pKeyMsg); 
/*
SHORT   UifAC20Function(KEYMSG *pKeyMsg);
SHORT   UifAC20EnterData1(KEYMSG *pKeyMsg); 
SHORT   UifAC20EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifAC20EnterData2(KEYMSG *pKeyMsg); 
*/
SHORT   UifAC20Function( KEYMSG *pKeyMsg );         /* uifc20.c */
SHORT   UifAC20EnterData1( KEYMSG *pKeyMsg );           /* uifc20.c */
SHORT   UifAC20EnterData2( KEYMSG *pKeyMsg );           /* uifc20.c */
SHORT   UifAC20EnterData3( KEYMSG *pKeyMsg );           /* uifc20.c */
SHORT   UifAC20EnterData4( KEYMSG *pKeyMsg );           /* uifc20.c */
SHORT   UifAC20EnterGstChkStartNo( KEYMSG *pKeyMsg );   /* uifc20.c */
SHORT   UifAC20EnterGstChkEndNo( KEYMSG *pKeyMsg ); /* uifc20.c */
SHORT   UifAC20EnterChgTipRate( KEYMSG *pKeyMsg );      /* uifc20.c */
SHORT   UifAC20EnterTeamNo( KEYMSG *pKeyMsg );          /* uifc20.c */
SHORT   UifAC20EnterTerminal( KEYMSG *pKeyMsg );        /* uifc20.c */
SHORT   UifAC20EnterALPHA( KEYMSG *pKeyMsg );           /* uifc20.c */
SHORT   UifAC20ErrorCorrect( KEYMSG *pKetMsg );         /* uifc20.c */
SHORT   UifAC20CancelFunction( KEYMSG *pKeyMsg );       /* uifc20.c */

SHORT   UifAC21Function(KEYMSG *pKeyMsg);
SHORT   UifAC21EnterData(KEYMSG *pKeyMsg); 
SHORT   UifAC21EnterDataType(KEYMSG *pKeyMsg);
SHORT   UifAC21EnterUserType(KEYMSG *pKeyMsg);
SHORT   UifAC23Function(KEYMSG *pKeyMsg);
SHORT   UifAC23EnterDataType(KEYMSG *pKeyMsg); 
SHORT   UifAC24Function(KEYMSG *pKeyMsg);
SHORT   UifAC24EnterDataType(KEYMSG *pKeyMsg); 
SHORT   UifAC24EnterRptType(KEYMSG *pKeyMsg);
SHORT   UifAC24EnterData1(KEYMSG *pKeyMsg); 
SHORT   UifAC24EnterData2(KEYMSG *pKeyMsg); 
SHORT   UifAC26Function(KEYMSG *pKeyMsg);
SHORT   UifAC26EnterDataType(KEYMSG *pKeyMsg); 
SHORT   UifAC26EnterRptType(KEYMSG *pKeyMsg);
SHORT   UifAC26IssueAllReport(KEYMSG *pKeyMsg);
SHORT   UifAC26EnterMDEPT1(KEYMSG *pKeyMsg); 
SHORT   UifAC26EnterDEPT1(KEYMSG *pKeyMsg); 
SHORT   UifAC26EnterMDEPT2(KEYMSG *pKeyMsg); 
SHORT   UifAC26EnterDEPT2(KEYMSG *pKeyMsg); 
SHORT   UifAC29Function(KEYMSG *pKeyMsg);
SHORT   UifAC29EnterDataType(KEYMSG *pKeyMsg); 
SHORT   UifAC29EnterRptType(KEYMSG *pKeyMsg);
SHORT   UifAC29IssueAllReport(KEYMSG *pKeyMsg);
SHORT   UifAC29EnterDEPT1(KEYMSG *pKeyMsg); 
SHORT   UifAC29EnterRptCode1(KEYMSG *pKeyMsg); 
SHORT   UifAC29EnterPLU1(KEYMSG *pKeyMsg); 
SHORT   UifAC29EnterDEPT2(KEYMSG *pKeyMsg); 
SHORT   UifAC29EnterRptCode2(KEYMSG *pKeyMsg); 
SHORT   UifAC29EnterPLU2(KEYMSG *pKeyMsg); 
//GroupReport - CSMALL
SHORT   UifAC31Function(KEYMSG *pKeyMsg);
SHORT   UifAC31EnterData(KEYMSG *pKeyMsg); 
SHORT   UifAC31EnterDataType(KEYMSG *pKeyMsg);
SHORT   UifAC31EnterRptType(KEYMSG *pKeyMsg);

SHORT   UifAC39Function(KEYMSG *pKeyMsg);
SHORT   UifAC39EnterDataType(KEYMSG *pKeyMsg);
SHORT   UifAC39EnterRstType(KEYMSG *pKeyMsg); 
SHORT   UifAC39EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC41Function(KEYMSG *pKeyMsg);
SHORT   UifAC41EnterDataType(KEYMSG *pKeyMsg); 
SHORT   UifAC41EnterRptType(KEYMSG *pKeyMsg);
SHORT   UifAC41EnterData(KEYMSG *pKeyMsg);  
SHORT   UifAC42Function(KEYMSG *pKeyMsg);
SHORT   UifAC42EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC46Function(KEYMSG *pKeyMsg);
SHORT   UifAC46EnterDataType(KEYMSG *pKeyMsg); 
SHORT   UifAC46EnterRstType(KEYMSG *pKeyMsg); 
SHORT   UifAC46EnterRptType(KEYMSG *pKeyMsg);
SHORT   UifAC46EnterData(KEYMSG *pKeyMsg);  
SHORT   UifAC46EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC50Function(KEYMSG *pKeyMsg);
SHORT   UifAC50EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC50EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifAC50ChangeKB(KEYMSG *pKeyMsg); 
SHORT   UifAC50EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC50EnterData3(KEYMSG *pKeyMsg);
SHORT   UifAC50EnterData4(KEYMSG *pKeyMsg);
SHORT   UifAC63Function(KEYMSG *pKeyMsg);
SHORT   UifAC63EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC63EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC63Intervation(KEYMSG *pKeyMsg);
SHORT   UifAC63ErrorCorrect(KEYMSG *pKeyMsg);
SHORT   UifAC63EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifAC63ExitFunction(KEYMSG *pKeyMsg);
SHORT   UifAC63ErrorFunction(KEYMSG *pKeyMsg);
SHORT   UifAC63CancelFunction(KEYMSG *pKeyMsg);
SHORT   UifAC64Function(KEYMSG *pKeyMsg);
SHORT   UifAC64EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC64EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC64ErrorCorrect(KEYMSG *pKeyMsg);
SHORT   UifAC64ErrorFunction(KEYMSG *pKeyMsg);
SHORT   UifAC66Function(KEYMSG *pKeyMsg);
SHORT   UifAC66EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC68Function(KEYMSG *pKeyMsg);
SHORT   UifAC70Function( VOID );
SHORT   UifAC72Function( VOID );
SHORT   UifAC75Function(KEYMSG *pKeyMsg);
SHORT   UifAC75EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC78Function(KEYMSG *pKeyMsg);
SHORT   UifAC78EnterData(KEYMSG *pKeyMsg); 
SHORT   UifAC82Function(KEYMSG *pKeyMsg);
SHORT   UifAC82EnterData(KEYMSG *pKeyMsg);
SHORT   UifAC82EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifAC82ChangeKB(KEYMSG *pKeyMsg); 
SHORT   UifAC82ExitFunction(KEYMSG *pKeyMsg);
SHORT   UifAC85Function(KEYMSG *pKeyMsg);
SHORT   UifAC85EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC84Function(KEYMSG *pKeyMsg);
SHORT   UifAC84EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC84EnterDelimit1(KEYMSG *pKeyMsg);
SHORT   UifAC84EnterDelimit2(KEYMSG *pKeyMsg);
SHORT   UifAC84EnterPosition(KEYMSG *pKeyMsg);
SHORT   UifAC86Function(KEYMSG *pKeyMsg);
SHORT   UifAC86EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC86EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC86EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC87Function(KEYMSG *pKeyMsg);
SHORT   UifAC87EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifAC87ChangeKB1(KEYMSG *pKeyMsg);
SHORT   UifAC87IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifAC87EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC87ChangeKB2(KEYMSG *pKeyMsg);
SHORT   UifAC88Function(KEYMSG *pKeyMsg);
SHORT   UifAC88EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifAC88ChangeKB1(KEYMSG *pKeyMsg);
SHORT   UifAC88IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifAC88EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC88ChangeKB2(KEYMSG *pKeyMsg);
SHORT   UifAC89Function(KEYMSG *pKeyMsg);
SHORT   UifAC89EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC89EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC89EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC97Function(KEYMSG *pKeyMsg);
SHORT   UifAC97EnterRstType(KEYMSG *pKeyMsg);
SHORT   UifAC97EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC99Function(KEYMSG *pKeyMsg);
SHORT   UifAC99EnterRstType(KEYMSG *pKeyMsg); 
SHORT   UifAC99EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC100Function(KEYMSG *pKeyMsg);
SHORT   UifAC100EnterKey(KEYMSG *pKeyMsg);
SHORT	UifAC102Function(KEYMSG *pKeyMsg);
SHORT	UifAC102EnterType(KEYMSG *pKeyMsg);
SHORT   UifAC103Function(KEYMSG *pKeyMsg);
SHORT   UifAC103EnterData(KEYMSG *pKeyMsg);  
SHORT   UifAC103ExitFunction(KEYMSG *pKeyMsg);  
SHORT   UifAC111Function(KEYMSG *pKeyMsg);
SHORT   UifAC111EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC111EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC111EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC114Function(KEYMSG *pKeyMsg);
SHORT   UifAC114EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC114EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC114EnterData3(KEYMSG *pKeyMsg);
SHORT   UifAC114EnterALPHA(KEYMSG *pKeyMsg); 
SHORT   UifAC114ChangeKB(KEYMSG *pKeyMsg); 
SHORT   UifAC114ErrorCorrect(KEYMSG *pKeyMsg); 
SHORT   UifAC114CancelFunction(KEYMSG *pKeyMsg);
SHORT   UifAC116Function(KEYMSG *pKeyMsg);
SHORT   UifAC116IssueRpt(KEYMSG *pKeyMsg);
SHORT   UifAC116EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC116EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC116EnterAdject1(KEYMSG *pKeyMsg);
SHORT   UifAC116EnterAdject2(KEYMSG *pKeyMsg);
SHORT   UifAC116ExitFunction(KEYMSG *pKeyMsg);
SHORT   UifAC119Function(KEYMSG *pKeyMsg);
SHORT   UifAC119EnterData(KEYMSG *pKeyMsg);  
SHORT   UifAC119ExitFunction(KEYMSG *pKeyMsg);  
SHORT   UifAC122Function(KEYMSG *pKeyMsg);
SHORT   UifAC122EnterDataType(KEYMSG *pKeyMsg); 
SHORT   UifAC123Function(KEYMSG *pKeyMsg);
SHORT   UifAC123EnterDataType(KEYMSG *pKeyMsg); 
SHORT   UifAC124Function(KEYMSG *pKeyMsg);
SHORT   UifAC124EnterRate(KEYMSG *pKeyMsg);  
SHORT   UifAC124EnterAmount1(KEYMSG *pKeyMsg); 
SHORT   UifAC124EnterTax(KEYMSG *pKeyMsg);  
SHORT   UifAC124EnterKey(KEYMSG *pKeyMsg);  
SHORT   UifAC124EnterAmount2(KEYMSG *pKeyMsg); 
SHORT   UifAC124EnterTest(KEYMSG *pKeyMsg);  
SHORT   UifAC125Function(KEYMSG *pKeyMsg);
SHORT   UifAC125EnterRate(KEYMSG *pKeyMsg);  
SHORT   UifAC125EnterAmount1(KEYMSG *pKeyMsg); 
SHORT   UifAC125EnterTax(KEYMSG *pKeyMsg);  
SHORT   UifAC125EnterKey(KEYMSG *pKeyMsg);  
SHORT   UifAC125EnterAmount2(KEYMSG *pKeyMsg); 
SHORT   UifAC125EnterTest(KEYMSG *pKeyMsg);  
SHORT   UifAC126Function(KEYMSG *pKeyMsg);
SHORT   UifAC126EnterRate(KEYMSG *pKeyMsg);  
SHORT   UifAC126EnterAmount1(KEYMSG *pKeyMsg); 
SHORT   UifAC126EnterTax(KEYMSG *pKeyMsg);  
SHORT   UifAC126EnterKey(KEYMSG *pKeyMsg);  
SHORT   UifAC126EnterAmount2(KEYMSG *pKeyMsg); 
SHORT   UifAC126EnterTest(KEYMSG *pKeyMsg);  
SHORT   UifAC127Function(KEYMSG *pKeyMsg);
SHORT   UifAC127EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC127EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC127EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC129Function(KEYMSG *pKeyMsg);
SHORT   UifAC129EnterRate(KEYMSG *pKeyMsg);  
SHORT   UifAC129EnterAmount1(KEYMSG *pKeyMsg); 
SHORT   UifAC129EnterTax(KEYMSG *pKeyMsg);  
SHORT   UifAC129EnterKey(KEYMSG *pKeyMsg);  
SHORT   UifAC129EnterAmount2(KEYMSG *pKeyMsg); 
SHORT   UifAC129EnterTest(KEYMSG *pKeyMsg);  
SHORT   UifAC130Function(KEYMSG *pKeyMsg);
SHORT   UifAC130EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC130EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC130EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC146Function(KEYMSG *pKeyMsg);
SHORT   UifAC146EnterData(KEYMSG *pKeyMsg);  
SHORT   UifAC146ExitFunction(KEYMSG *pKeyMsg);  
SHORT   UifAC150Function(KEYMSG *pKeyMsg);
SHORT   UifAC150EnterData(KEYMSG *pKeyMsg);
SHORT   UifAC150EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC151Function(KEYMSG *pKeyMsg);
SHORT   UifAC151EnterRstType(KEYMSG *pKeyMsg);
SHORT   UifAC151EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC152Function(KEYMSG *pKeyMsg);
SHORT   UifAC152EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC152EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC152EnterData3(KEYMSG *pKeyMsg);
SHORT   UifAC152EnterData4(KEYMSG *pKeyMsg); 
SHORT   UifAC152EnterALPHA(KEYMSG *pKeyMsg); 
SHORT   UifAC153Function(KEYMSG *pKeyMsg); 
SHORT   UifAC153EnterEmpNo(KEYMSG *pKeyMsg); 
SHORT   UifAC153EnterData(KEYMSG *pKeyMsg);
SHORT   UifAC153CancelFunction(KEYMSG *pKeyMsg); 
SHORT   UifAC160Function(KEYMSG *pKeyMsg);
SHORT   UifAC160EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC160EnterData(KEYMSG *pKeyMsg);            
SHORT   UifAC169Function(KEYMSG *pKeyMsg);
SHORT   UifAC169EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC169EnterData1(KEYMSG *pKeyMsg);            
SHORT   UifAC175Function(KEYMSG *pKeyMsg);
SHORT   UifAC175EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC176Function(KEYMSG *pKeyMsg);
SHORT   UifACDefProc(KEYMSG *pKeyMsg);            
SHORT   UifACChangeKB1(KEYMSG *pKeyMsg); 
SHORT   UifACChangeKB2(KEYMSG *pKeyMsg); 
SHORT   UifACExitFunction(KEYMSG *pKeyMsg);
SHORT   UifACShift(KEYMSG *pKeyMsg);
USHORT  UifChangeKeyboardType(UCHAR uchKBType);
USHORT  UifACChgKBType(UCHAR uchKBType);
VOID    UifACInit( VOID );
SHORT   UifAC161Function(KEYMSG *pKeyMsg);
SHORT   UifAC161EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC161EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC161EnterALPHA(KEYMSG *pKeyMsg);
SHORT   UifAC161ExitFunction(KEYMSG *pKeyMsg);
SHORT   UifAC161CancelFunction(KEYMSG *pKeyMsg);
SHORT   UifAC161ErrorFunction(KEYMSG *pKeyMsg);
SHORT   UifAC161Adjective(KEYMSG *pKeyMsg);
SHORT   UifAC30Function(KEYMSG *pKeyMsg); 
SHORT   UifAC30EnterDataType(KEYMSG *pKeyMsg); 
SHORT   UifAC30EnterRptType(KEYMSG *pKeyMsg); 
SHORT   UifAC30EnterData(KEYMSG *pKeyMsg); 
SHORT   UifAC40Function(KEYMSG *pKeyMsg); 
SHORT   UifAC40EnterDataType(KEYMSG *pKeyMsg); 
SHORT   UifAC40EnterRstType(KEYMSG *pKeyMsg); 
SHORT   UifAC40EnterKey(KEYMSG *pKeyMsg); 
SHORT   UifAC162Function(KEYMSG  *pKeyMsg);
SHORT   UifAC162EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC162EnterData(KEYMSG *pKeyMsg);            
SHORT   UifAC33Function(KEYMSG *pKeyMsg);
SHORT   UifAC33EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC33EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC33EnterData3(KEYMSG *pKeyMsg);
SHORT   UifAC33ErrorFunction(KEYMSG *pKeyMsg);
SHORT   UifAC33CancelFunction(KEYMSG *pKeyMsg);
SHORT   UifAC71Function(KEYMSG *pKeyMsg);
SHORT   UifAC71EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC71EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC71EnterData3(KEYMSG *pKeyMsg);
SHORT   UifAC71ErrorCorrect(KEYMSG *pKeyMsg);
SHORT   UifAC71CancelFunction(KEYMSG *pKeyMsg);
SHORT   UifAC133Function(KEYMSG *pKeyMsg);
SHORT   UifAC133EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC133EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC133EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC131Function(KEYMSG *pKeyMsg);
SHORT   UifAC131EnterDataType(KEYMSG *pKeyMsg); 
SHORT   UifAC132Function(KEYMSG *pKeyMsg);
SHORT   UifAC132EnterDataType(KEYMSG *pKeyMsg);
SHORT   UifAC132EnterRstType(KEYMSG *pKeyMsg); 
SHORT   UifAC132EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC223Function(KEYMSG *pKeyMsg);
SHORT   UifAC223EnterData(KEYMSG *pKeyMsg); 
SHORT   UifAC233Function(KEYMSG *pKeyMsg);
SHORT   UifAC233EnterRstType(KEYMSG *pKeyMsg);
SHORT   UifAC233EnterRptType(KEYMSG *pKeyMsg);
SHORT   UifAC233EnterData(KEYMSG *pKeyMsg); 
SHORT   UifAC233EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC154Function(KEYMSG *pKeyMsg);
SHORT   UifAC154EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC154EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC154EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC135Function(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterFormatType(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterDailyPTD(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterDeptType(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterMDEPT1(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterDEPT1(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterPLUType(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterDEPTPLU1(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterPLUCode1(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterPLU1(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterCpnType(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterCasType(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterWaiType(KEYMSG *pKeyMsg);
SHORT   UifAC135EnterIndFinType(KEYMSG *pKeyMsg);
SHORT   UifAC136Function(KEYMSG *pKeyMsg);
SHORT   UifAC136EnterFormatType(KEYMSG *pKeyMsg);
SHORT   UifAC136EnterRstType(KEYMSG *pKeyMsg);
SHORT   UifAC136EnterDailyPTD(KEYMSG *pKeyMsg);
SHORT   UifAC136EnterCasType(KEYMSG *pKeyMsg);
SHORT   UifAC136EnterWaiType(KEYMSG *pKeyMsg);
SHORT   UifAC136EnterIndFinType(KEYMSG *pKeyMsg);
SHORT   UifAC137Function(KEYMSG *pKeyMsg);
SHORT   UifAC137EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC137EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC137EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC137EnterData3(KEYMSG *pKeyMsg);
VOID    UifACRedisp( VOID );
SHORT   UifAC115Function(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifAC115IssueRpt(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifAC115EnterData1(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifAC115EnterData2(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifAC115ExitFunction(KEYMSG *pKeyMsg); /* 2172 */
SHORT UifAC170Function(KEYMSG *pKeyMsg); /* 2172 */
SHORT UifAC170EnterAddr(KEYMSG *pKeyMsg); /* 2172 */
SHORT UifAC170EnterData1(KEYMSG *pKeyMsg); /* 2172 */
SHORT UifAC170EnterData2(KEYMSG *pKeyMsg); /* 2172 */
SHORT UifAC170ACCheck(KEYMSG *pKeyMsg); /* 2172 */
SHORT UifAC208Function(KEYMSG *pKeyMsg); /* 2172 */
SHORT UifAC208EnterAddr(KEYMSG *pKeyMsg); /* 2172 */
SHORT UifAC208EnterData1(KEYMSG *pKeyMsg); /* 2172 */
SHORT UifAC208EnterData2(KEYMSG *pKeyMsg); /* 2172 */
SHORT   UifAC10Function(KEYMSG *pKeyMsg);   /* Saratoga */
SHORT   UifAC10EnterCashier(KEYMSG *pKeyMsg);
SHORT   UifAC10EnterAmount(KEYMSG *pKeyMsg);
SHORT   UifAC10ErrorCorrect(KEYMSG *pKeyMsg);
SHORT   UifAC10Exit(KEYMSG *pKeyMsg);
SHORT   UifAC11Function(KEYMSG *pKeyMsg);
SHORT   UifAC11EnterCashier(KEYMSG *pKeyMsg);
SHORT   UifAC11EnterAmount(KEYMSG *pKeyMsg);
SHORT   UifAC11ErrorCorrect(KEYMSG *pKeyMsg);
SHORT   UifAC11Exit(KEYMSG *pKeyMsg);
VOID    UifAC1011KeyStatusClear(VOID);
SHORT   UifAC128Function(KEYMSG *pKeyMsg);
SHORT   UifAC128EnterAddr(KEYMSG *pKeyMsg);
SHORT   UifAC128EnterData1(KEYMSG *pKeyMsg);
SHORT   UifAC128EnterData2(KEYMSG *pKeyMsg);
SHORT   UifAC777Function(KEYMSG *pKeyMsg);
SHORT	UifAC888Function(KEYMSG *pKeyMsg);
SHORT   UifAC985Function(KEYMSG *pKeyMsg);
SHORT   UifAC985EnterKey(KEYMSG *pKeyMsg);
SHORT	UifAC999Function(KEYMSG *pKeyMsg);
SHORT	UifAC999EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC275Function(KEYMSG *pKeyMsg);
SHORT	UifAC275EnterKey(KEYMSG *pKeyMsg);
SHORT   UifAC276Function(KEYMSG *pKeyMsg);
SHORT	UifAC276EnterKey(KEYMSG *pKeyMsg);
SHORT	UifAC194Function(KEYMSG *pKeyMsg); // for Dynamic Window Reload
SHORT	UifAC444Function(KEYMSG *pKeyMsg); // CID_AC444FUNCTION, AC_JOIN_CLUSTER, Join cluster
SHORT	UifAC444EnterKey(KEYMSG *pKeyMsg); // 
SHORT   UifAC102Report(KEYMSG *pKeyMsg);   //Store and Forward Report
/* End of Header File --- */
