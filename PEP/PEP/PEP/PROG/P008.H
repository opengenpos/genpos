/*
* ---------------------------------------------------------------------------
* Title         :   Supervisor Number/Level Header File (Prog. 8)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P008.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
*
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/
/*
* ===========================================================================
*       Define Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Dialog ID 
* ===========================================================================
*/
#define P08_SPIN_OFFSET    75   /* Offset of SpinButton             */

                /* ----- EditText Controls ----- */
#define IDD_P08_LEV_0   20801   /* The Highest Sup. # for Level 0   */
#define IDD_P08_LEV_1   20802   /* The Highest Sup. # for Level 1   */
#define IDD_P08_01      20803   /* Supervisor # 1,  no longer used with Rel 2.2 and later */
#define IDD_P08_02      20804   /* Supervisor # 2,  no longer used with Rel 2.2 and later */
#define IDD_P08_03      20805   /* Supervisor # 3,  no longer used with Rel 2.2 and later */
#define IDD_P08_04      20806   /* Supervisor # 4,  no longer used with Rel 2.2 and later */
#define IDD_P08_05      20807   /* Supervisor # 5,  no longer used with Rel 2.2 and later */
#define IDD_P08_06      20808   /* Supervisor # 6,  no longer used with Rel 2.2 and later */
#define IDD_P08_07      20809   /* Supervisor # 7,  no longer used with Rel 2.2 and later */
#define IDD_P08_08      20810   /* Supervisor # 8,  no longer used with Rel 2.2 and later */
#define IDD_P08_09      20811   /* Supervisor # 9,  no longer used with Rel 2.2 and later */
#define IDD_P08_10      20812   /* Supervisor # 10,  no longer used with Rel 2.2 and later */
#define IDD_P08_11      20813   /* Supervisor # 11,  no longer used with Rel 2.2 and later */
#define IDD_P08_12      20814   /* Supervisor # 12,  no longer used with Rel 2.2 and later */
#define IDD_P08_13      20815   /* Supervisor # 13,  no longer used with Rel 2.2 and later */
#define IDD_P08_14      20816   /* Supervisor # 14,  no longer used with Rel 2.2 and later */
#define IDD_P08_15      20817   /* Supervisor # 15,  no longer used with Rel 2.2 and later */
#define IDD_P08_16      20818   /* Supervisor # 16,  no longer used with Rel 2.2 and later */
#define IDD_P08_17      20819   /* Supervisor # 17,  no longer used with Rel 2.2 and later */
#define IDD_P08_18      20820   /* Supervisor # 18,  no longer used with Rel 2.2 and later */
#define IDD_P08_19      20821   /* Supervisor # 19,  no longer used with Rel 2.2 and later */
#define IDD_P08_20      20822   /* Supervisor # 20,  no longer used with Rel 2.2 and later */

                /* ----- StaticText Controls ----- */
#define IDD_P08_LEVEL0	20823   /* Description of Supervisor Lev. 0 */
#define IDD_P08_LEVEL1	20824   /* Description of Supervisor Lev. 1 */
#define IDD_P08_LEVEL2	20825   /* Description of Supervisor Lev. 2 */
#define IDD_P08_01T     20826   /* Current Level of Supervisor # 1  */
#define IDD_P08_02T     20827   /* Current Level of Supervisor # 2  */
#define IDD_P08_03T     20828   /* Current Level of Supervisor # 3  */
#define IDD_P08_04T     20829   /* Current Level of Supervisor # 4  */
#define IDD_P08_05T     20830   /* Current Level of Supervisor # 5  */
#define IDD_P08_06T     20831   /* Current Level of Supervisor # 6  */
#define IDD_P08_07T     20832   /* Current Level of Supervisor # 7  */
#define IDD_P08_08T     20833   /* Current Level of Supervisor # 8  */
#define IDD_P08_09T     20834   /* Current Level of Supervisor # 9  */
#define IDD_P08_10T     20835   /* Current Level of Supervisor # 10 */
#define IDD_P08_11T     20836   /* Current Level of Supervisor # 11 */
#define IDD_P08_12T     20837   /* Current Level of Supervisor # 12 */
#define IDD_P08_13T     20838   /* Current Level of Supervisor # 13 */
#define IDD_P08_14T     20839   /* Current Level of Supervisor # 14 */
#define IDD_P08_15T     20840   /* Current Level of Supervisor # 15 */
#define IDD_P08_16T     20841   /* Current Level of Supervisor # 16 */
#define IDD_P08_17T     20842   /* Current Level of Supervisor # 17 */
#define IDD_P08_18T     20843   /* Current Level of Supervisor # 18 */
#define IDD_P08_19T     20844   /* Current Level of Supervisor # 19 */
#define IDD_P08_20T     20845   /* Current Level of Supervisor # 20 */
#define IDD_P08_HIGH0	20846	//multilingual
#define IDD_P08_HIGH1	20847	//multilingual
#define IDD_P08_LEVEL	20848

                /* ----- SpinButton Controls ----- */
#define IDD_P08_SPL0    (IDD_P08_LEV_0 + P08_SPIN_OFFSET)
#define IDD_P08_SPL1    (IDD_P08_LEV_1 + P08_SPIN_OFFSET)
#define IDD_P08_SP1     (IDD_P08_01    + P08_SPIN_OFFSET)
#define IDD_P08_SP2     (IDD_P08_02    + P08_SPIN_OFFSET)
#define IDD_P08_SP3     (IDD_P08_03    + P08_SPIN_OFFSET)
#define IDD_P08_SP4     (IDD_P08_04    + P08_SPIN_OFFSET)
#define IDD_P08_SP5     (IDD_P08_05    + P08_SPIN_OFFSET)
#define IDD_P08_SP6     (IDD_P08_06    + P08_SPIN_OFFSET)
#define IDD_P08_SP7     (IDD_P08_07    + P08_SPIN_OFFSET)
#define IDD_P08_SP8     (IDD_P08_08    + P08_SPIN_OFFSET)
#define IDD_P08_SP9     (IDD_P08_09    + P08_SPIN_OFFSET)
#define IDD_P08_SP10    (IDD_P08_10    + P08_SPIN_OFFSET)
#define IDD_P08_SP11    (IDD_P08_11    + P08_SPIN_OFFSET)
#define IDD_P08_SP12    (IDD_P08_12    + P08_SPIN_OFFSET)
#define IDD_P08_SP13    (IDD_P08_13    + P08_SPIN_OFFSET)
#define IDD_P08_SP14    (IDD_P08_14    + P08_SPIN_OFFSET)
#define IDD_P08_SP15    (IDD_P08_15    + P08_SPIN_OFFSET)
#define IDD_P08_SP16    (IDD_P08_16    + P08_SPIN_OFFSET)
#define IDD_P08_SP17    (IDD_P08_17    + P08_SPIN_OFFSET)
#define IDD_P08_SP18    (IDD_P08_18    + P08_SPIN_OFFSET)
#define IDD_P08_SP19    (IDD_P08_19    + P08_SPIN_OFFSET)
#define IDD_P08_SP20    (IDD_P08_20    + P08_SPIN_OFFSET)

//StaticText
#define IDD_P08_NUM			20850
#define IDD_P08_SUP1		20851
#define IDD_P08_SUP2		20852
#define IDD_P08_SUP3		20853
#define IDD_P08_SUP4		20854
#define IDD_P08_SUP5		20855
#define IDD_P08_SUP6		20856
#define IDD_P08_SUP7		20857
#define IDD_P08_SUP8		20858
#define IDD_P08_SUP9		20859
#define IDD_P08_SUP10		20860
#define IDD_P08_SUP11		20861
#define IDD_P08_SUP12		20862
#define IDD_P08_SUP13		20863
#define IDD_P08_SUP14		20864
#define IDD_P08_SUP15		20865
#define IDD_P08_SUP16		20866
#define IDD_P08_SUP17		20867
#define IDD_P08_SUP18		20868
#define IDD_P08_SUP19		20869
#define IDD_P08_SUP20		20870
#define IDD_P08_INVALIDATE	20871
#define IDD_P08_MINNUM		20872
#define IDD_P08_MAXNUM		20873
#define	IDD_P08_NUM2		20875
//#define IDD_P08_LEV_0_STATIC	20868	***No more static supervisor Levels ***PDINU
//#define IDD_P08_LEV_1_STATIC	20869	***No more static supervisor Levels ***PDINU
/*
* ===========================================================================
*       String ID Definisions
* ===========================================================================
*/
#define IDS_P08_NONE    IDS_P08

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P08_ADDR_MAX    MAX_SUPLEVEL_SIZE   /* Maximum of Program Address   */
#define P08_SUPR_MAX    20                  /* Number of Supervisor         */

#define P08_SUPDATA     100                 /* Minimum of Superviser Data   */
#define P08_SUP_LEN     8                   /* Length of SupData Strings    */
#define P08_DATA_MAX    999                 /* Maximum of Entry Data        */
#define P08_DATA_MIN    100                 /* Minimum of Entry Data        */
#define P08_DATA_LEN    3                   /* Maximum Length of Entry Data */
#define P08_SPIN_STEP   1
#define P08_SPIN_TURBO  0

#define P08_SECRET      0                   /* Secret Number                */

#define P08_SUP1_OFFSET 2                   /* Supervisor #1 Offset         */

#define P08_LEVEL_0     '0'                 /* Supervisor Level 0           */
#define P08_LEVEL_1     '1'                 /* Supervisor Level 1           */
#define P08_LEVEL_2     '2'                 /* Supervisor Level 2           */

#define P08_ERR_LEN     PEP_OVER_LEN
#define P08_CAP_LEN     PEP_CAPTION_LEN

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef SUPLEVEL FAR*   LPSUPLEVEL;

/*
* ===========================================================================
*       External Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/
BOOL    P08InitDlg(HWND);               /* Initialize DialogBox         */
VOID    P08ChkData(HWND);               /* Check Inputed Data           */
VOID    P08SetLevel(HWND);              /* Text Out SuperVisor Level    */

VOID	InitP08DlgStrings(HWND hDlg); //Multilingual RPH 4-22-03
/* ===== End of P008.H ===== */
