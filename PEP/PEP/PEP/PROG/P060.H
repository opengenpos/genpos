/*
* ---------------------------------------------------------------------------
* Title         :   Total Key Type/Control Definition Header File (Prog. 60, 61)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P060.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Jul-27-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Sep-16-98 : 03.03.00 : A.Mitsui   : V3.3 Add International VAT
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
#define IDD_P60_LIST        26001   /* listbox for item selection       */
#define IDD_P60_TYPE_DSC    26002   /* type selection description       */
#define IDD_P60_TYPE_CB     26003   /* type selection combo-box         */
#define IDD_P60_TYPE_ST     26004   /* type selection static text       */
#define IDD_P60_TYPE_BF     26005   /* type selection black frame       */
#define IDD_P60_INDEX_DSC   26006   /* index selection description      */
#define IDD_P60_INDEX_CB    26007   /* index selection combo-box        */
#define IDD_P60_INDEX_ST    26008   /* index selection static text      */
#define IDD_P60_INDEX_BF    26009   /* index selection black frame      */

#define IDD_P60_TOTALNUM	26010
#define IDD_P60_TYPE		26011
#define IDD_P60_INDEX		26012
#define IDD_P60_KEYFUNC		26013

#define IDD_P60_CTRL        26020   /* dialog template for control code */

#define IDD_P60_CTRL_01     26021   /* tax 1 calculate                  */
#define IDD_P60_CTRL_02     26022   /* use taxable itemizer No.1        */
#define IDD_P60_CTRL_03     26023   /* use taxable itemizer No.2        */
#define IDD_P60_CTRL_04     26024   /* use taxable itemizer No.3        */
#define IDD_P60_CTRL_05     26025   /* tax 2 calculate                  */
#define IDD_P60_CTRL_06     26026   /* use taxable itemizer No.1        */
#define IDD_P60_CTRL_07     26027   /* use taxable itemizer No.2        */
#define IDD_P60_CTRL_08     26028   /* use taxable itemizer No.3        */
#define IDD_P60_CTRL_09     26029   /* tax 3 calculate                  */
#define IDD_P60_CTRL_10     26030   /* use taxable itemizer No.1        */
#define IDD_P60_CTRL_11     26031   /* use taxable itemizer No.2        */
#define IDD_P60_CTRL_12     26032   /* use taxable itemizer No.3        */
#define IDD_P60_CTRL_13     26033   /* clear taxable itemizer           */
#define IDD_P60_CTRL_14     26034   /* executed buffering print at postcheck */
#define IDD_P60_CTRL_15     26035   /* evaluate auto coupon check box   */
#define IDD_P60_CTRL_16     26036   /* open cash drawer                 */
#define IDD_P60_CTRL_17     26037   /* send to kitchen device No.1      */
#define IDD_P60_CTRL_18     26038   /* send to kitchen device No.2      */
#define IDD_P60_CTRL_19     26039   /* send to kitchen device No.3      */
#define IDD_P60_CTRL_20     26040   /* send to kitchen device No.4      */
#define IDD_P60_CTRL_21     26041   /* by parameter of total key or each menu */
#define IDD_P60_CTRL_22     26042   /* only when both parameter of total ...  */
#define IDD_P60_CTRL_23     26043   /* total line single wide print     */
#define IDD_P60_CTRL_24     26044   /* total line double wide print     */
#define IDD_P60_CTRL_25     26045   /* compulsory validation            */
#define IDD_P60_CTRL_26     26046   /* compulsory P23 slip insertion    */
#define IDD_P60_CTRL_27     26047   /* sub total key is buffered        */
#define IDD_P60_CTRL_28     26048   /* sub total key is not buffered    */
#define IDD_P60_CTRL_29     26049   /* affect to service itemizer No.1  */
#define IDD_P60_CTRL_30     26050   /* affect to service itemizer No.2  */
#define IDD_P60_CTRL_31     26051   /* affect to service itemizer No.3  */
#define IDD_P60_CTRL_32     26052   /* not use */
#define IDD_P60_CTRL_33     26053   /* Not Perform Pre-Authorization    */
#define IDD_P60_CTRL_34     26054   /* Perform Pre-Authorization        */
#define IDD_P60_CTRL_35     26055   /* menu is not consolidation    R3.0  */
#define IDD_P60_CTRL_36     26056   /* display order#       R3.0        */
#define IDD_P60_CTRL_37     26057   /* not use,    R3.1                 */
#define IDD_P60_CTRL_38     26058   /* send to kitchen device No.5  R3.1 */
#define IDD_P60_CTRL_39     26059   /* send to kitchen device No.6  R3.1 */
#define IDD_P60_CTRL_40     26060   /* send to kitchen device No.7  R3.1 */
#define IDD_P60_CTRL_41     26061   /* send to kitchen device No.8  R3.1 */
#define IDD_P60_CTRL_42     26062   /* use charge posting,    R3.1                 */
#define IDD_P60_CTRL_43     26063   /* use electronic payment,    R3.1                 */
#define IDD_P60_CTRL_44     26064   /* push back.    R3.1                 */
#define IDD_P60_CTRL_45     26065   /* not use,    R3.1                 */
#define IDD_P60_CTRL_46     26066   /* not use,    R3.1                 */
#define IDD_P60_CTRL_47     26067   /* not use,    R3.1                 */
#define IDD_P60_CTRL_48     26068   /* not use,    R3.1                 */
#define IDD_P60_CTRL_49     26069   /* not use,    R3.1                 */
#define IDD_P60_CTRL_50     26070   /* not use,    R3.1                 */
#define IDD_P60_CTRL_51     26071   /* not use,    R3.1                 */
#define IDD_P60_GROUP_01    26072   /* group box of tax1/canadian tax   */
#define IDD_P60_GROUP_02    26073   /* group box of tax2/not used(CA)   */
#define IDD_P60_GROUP_03    26074   /* group box of tax3/not used(CA)   */
#define IDD_P60_MNEMONIC    26075   /* total key mnemonic edit box      */

#define IDD_P60_SERVITEM		26076
#define IDD_P60_RMT_PTR			26077
#define IDD_P60_KIT_PTR			26078
#define IDD_P60_PTR_WIDTH		26079
#define IDD_P60_SUB_TTL			26080
#define IDD_P60_PREAUTH			26081
#define	IDD_P60_PREAUTH_TYPE	26082

#define IDC_P60_STATIC2			26083
#define IDC_P60_STATIC3			26084

#define IDD_P60_TYPE_ODT		26085
#define IDD_P60_ODT_CB			26086

#define IDC_P60_STATIC_MNEMONIC    26087   /* total key mnemonic edit box      */


/*
* ===========================================================================
*       String ID 
* ===========================================================================
*/
#define IDS_P60_TYPE0       (IDS_P60 + 0)
#define IDS_P60_TYPE1       (IDS_P60 + 1)
#define IDS_P60_TYPE2       (IDS_P60 + 2)
#define IDS_P60_TYPE3       (IDS_P60 + 3)
#define IDS_P60_TYPE4       (IDS_P60 + 4)
#define IDS_P60_TYPE5       (IDS_P60 + 5)
#define IDS_P60_TYPE6       (IDS_P60 + 6)
#define IDS_P60_TYPE7       (IDS_P60 + 7)
#define IDS_P60_TYPE8       (IDS_P60 + 8)
#define IDS_P60_TYPE9       (IDS_P60 + 9)
#define IDS_P60_TYPE10      (IDS_P60 + 10)      /* ### ADD Saratoga (052400) */

#define IDS_P60_INDEX0      (IDS_P60 + 16)
#define IDS_P60_INDEX1      (IDS_P60 + 17)
#define IDS_P60_INDEX2      (IDS_P60 + 18)
#define IDS_P60_INDEX3      (IDS_P60 + 19)
#define IDS_P60_INDEX4      (IDS_P60 + 20)
#define IDS_P60_INDEX5      (IDS_P60 + 21)
#define IDS_P60_INDEX6      (IDS_P60 + 22)
#define IDS_P60_INDEX7      (IDS_P60 + 23)
#define IDS_P60_INDEX8      (IDS_P60 + 24)
#define IDS_P60_INDEX9	    (IDS_P60 + 25)
#define IDS_P60_INDEX10      (IDS_P60 + 26)
#define IDS_P60_INDEX11      (IDS_P60 + 27)
#define IDS_P60_INDEX12      (IDS_P60 + 28)
#define IDS_P60_INDEX13      (IDS_P60 + 29)
#define IDS_P60_INDEX14      (IDS_P60 + 30)
#define IDS_P60_INDEX15      (IDS_P60 + 31)
#define IDS_P60_INDEX16      (IDS_P60 + 32)
#define IDS_P60_INDEX17      (IDS_P60 + 33)
#define IDS_P60_INDEX18      (IDS_P60 + 34)
#define IDS_P60_INDEX19      (IDS_P60 + 35)
#define IDS_P60_INDEX20      (IDS_P60 + 36)


#define IDS_P60_LIST_TYPE0  (IDS_P60 + 38)
#define IDS_P60_LIST_TYPE1  (IDS_P60 + 39)
#define IDS_P60_LIST_TYPE2  (IDS_P60 + 40)
#define IDS_P60_LIST_TYPE3  (IDS_P60 + 41)
#define IDS_P60_LIST_TYPE4  (IDS_P60 + 42)
#define IDS_P60_LIST_TYPE5  (IDS_P60 + 43)
#define IDS_P60_LIST_TYPE6  (IDS_P60 + 44)
#define IDS_P60_LIST_TYPE7  (IDS_P60 + 45)
#define IDS_P60_LIST_TYPE8  (IDS_P60 + 46)
#define IDS_P60_LIST_TYPE9  (IDS_P60 + 47)
#define IDS_P60_LIST_TYPE10 (IDS_P60 + 48)		/* ### ADD Saratoga (052400) */
#define IDS_P60_LIST_STR    (IDS_P60 + 49)

#define IDS_P60_GROUP_01US  (IDS_P60 + 50)
#define IDS_P60_GROUP_02US  (IDS_P60 + 51)
#define IDS_P60_GROUP_03US  (IDS_P60 + 52)

#define IDS_P60_GROUP_01CA  (IDS_P60 + 60)
#define IDS_P60_GROUP_02CA  (IDS_P60 + 61)
#define IDS_P60_GROUP_03CA  (IDS_P60 + 62)

#define IDS_P60_CTRL_01US   (IDS_P60 + 70)
#define IDS_P60_CTRL_02US   (IDS_P60 + 71)
#define IDS_P60_CTRL_03US   (IDS_P60 + 72)
#define IDS_P60_CTRL_04US   (IDS_P60 + 73)
#define IDS_P60_CTRL_05US   (IDS_P60 + 74)
#define IDS_P60_CTRL_06US   (IDS_P60 + 75)
#define IDS_P60_CTRL_07US   (IDS_P60 + 76)
#define IDS_P60_CTRL_08US   (IDS_P60 + 77)
#define IDS_P60_CTRL_09US   (IDS_P60 + 78)
#define IDS_P60_CTRL_10US   (IDS_P60 + 79)
#define IDS_P60_CTRL_11US   (IDS_P60 + 80)
#define IDS_P60_CTRL_12US   (IDS_P60 + 81)

#define IDS_P60_CTRL_01CA   (IDS_P60 + 90)
#define IDS_P60_CTRL_02CA   (IDS_P60 + 91)
#define IDS_P60_CTRL_03CA   (IDS_P60 + 92)
#define IDS_P60_CTRL_04CA   (IDS_P60 + 93)
#define IDS_P60_CTRL_05CA   (IDS_P60 + 94)
#define IDS_P60_CTRL_06CA   (IDS_P60 + 95)
#define IDS_P60_CTRL_07CA   (IDS_P60 + 96)
#define IDS_P60_CTRL_08CA   (IDS_P60 + 97)
#define IDS_P60_CTRL_09CA   (IDS_P60 + 98)
#define IDS_P60_CTRL_10CA   (IDS_P60 + 99)
#define IDS_P60_CTRL_11CA   (IDS_P60 + 100)
#define IDS_P60_CTRL_12CA   (IDS_P60 + 101)

/* add V3.3 */
#define IDS_P60_GROUP_01VT  (IDS_P60 + 110)
#define IDS_P60_GROUP_02VT  (IDS_P60 + 111)
#define IDS_P60_GROUP_03VT  (IDS_P60 + 112)

#define IDS_P60_CTRL_01VT   (IDS_P60 + 120)
#define IDS_P60_CTRL_02VT   (IDS_P60 + 121)
#define IDS_P60_CTRL_03VT   (IDS_P60 + 122)
#define IDS_P60_CTRL_04VT   (IDS_P60 + 123)
#define IDS_P60_CTRL_05VT   (IDS_P60 + 124)
#define IDS_P60_CTRL_06VT   (IDS_P60 + 125)
#define IDS_P60_CTRL_07VT   (IDS_P60 + 126)
#define IDS_P60_CTRL_08VT   (IDS_P60 + 127)
#define IDS_P60_CTRL_09VT   (IDS_P60 + 128)
#define IDS_P60_CTRL_10VT   (IDS_P60 + 129)
#define IDS_P60_CTRL_11VT   (IDS_P60 + 130)
#define IDS_P60_CTRL_12VT   (IDS_P60 + 131)



/*
* ===========================================================================
*       Number definition
* ===========================================================================
*/
#define P60_NORECORD    (MAX_KEYTYPE_SIZE * 2)  /* Number of record     */
#define P60_TYPE_NUM            8   /* No. of type of total data        */
#define P60_INDEX_NUM           21   /* No. of index of financial tender */
#define P60_FIELD_NUM           6   /* No. of key type field            *//* ### MOD Saratoga (5 --> 6)(052400) */
                                    /* --> added Food Stamp Type        */
#define P60_ODT_NUM				5   /* No of Order Declaration Types (including "Not Used") JHHJ
#define P60_CTRLITEM_NUM       35   /* No. of key control item          */
#define P60_CTRLTEXT_NUM       51   /* No. of key control text          */
#define P60_BUFF_LEN           64   /* length of buffer                 */
#define P60_BITFORBYTE          8   /* bits for 1 byte                  */
#define P60_BITCHK           0x01   /* for bit check or mask            */
#define P60_SUBTTL_BUFFER    0x01   /* bit for Subttl Buffering, used with Subtotal Key type only!  R3.0  */
#define P60_DISP_ORDER       0x01   /* bit for Display Order#, conflicts with P60_SUBTTL_BUFFER !    R3.0  */

#define P60_TAX_ADDR           15   /* MDC Tax Address (US/Canada), MDC_TAX_ADR      */
#define P60_TAX_ADDR2          11   /* MDC Tax Address (Canada/VAT), MDC_TAXINTL_ADR  V3.3 */
#define P60_TAX_CHK          0x01   /* MDC Tax Check Bit                */
#define P60_TAXGROUP_NUM        3   /* No. of group box for tax control */
#define P60_TAXCTRL_NUM        12   /* No. of check box for tax control */

#define P60_TAX_US              0   /* check Tax mode of US     , V3.3  */
#define P60_TAX_CA              1   /* check Tax mode of Canada , V3.3  */
#define P60_TAX_VT              2   /* check Tax mode of VAT    , V3.3  */

#define P60_ADDR_1              0   /* address 1                        */
#define P60_ADDR_2              1   /* address 2                        */
#define P60_ADDR_9              8   /* address 9 Food Stamp             *//* ### ADD Saratoga (052400) */

#define P60_TYPE_NOTUSED        0   /* not used this key                */
#define P60_TYPE_NOFINAL        1   /* no finalize total                */
#define P60_TYPE_SERVICEWO      2   /* service total w/o stub           */
#define P60_TYPE_SERVICEW       3   /* service total w/ stub            */
#define P60_TYPE_FINALWO        4   /* finalize total w/o stub          */
#define P60_TYPE_FINALW         5   /* finalize total w/ stub           */
#define P60_TYPE_TRAYWO         6   /* tray total w/o stub              */
#define P60_TYPE_TRAYW          7   /* tray total w/ stub               */
#define P60_TYPE_SUBTOTAL       8   /* sub total key                    */ 
#define P60_TYPE_CHECKTOTAL     9   /* check total key                  */
#define P60_TYPE_FOODSTAMPTOTAL 10  /* food stamp total                 *//* ### ADD Saratoga (052400) */

#define P60_FIELD0          65535   /* not used                         */
#define P60_FIELD1              0   /* sub total key                    */
#define P60_FIELD2              1   /* check/non-finalize total key     */
#define P60_FIELD3              2   /* service total key                */
#define P60_FIELD4              3   /* finalize key                     */
#define P60_FIELD5              4   /* tray total key                   */
#define P60_FIELD6              5   /* Food Stamp Total Key             *//* ### ADD Saratoga (052400) */

#define P60_NU                  0   /* not used                         */
#define P60_CB                  1   /* checkbox                         */
#define P60_RB                  2   /* radio button (alternative)       */
#define P60_R3                  3   /* radio button (1 for 3)           */
#define P60_TABNO               3
#define P60_TAB_1               4
#define P60_TAB_2              16
#define P60_TAB_3             108   

/*
* ===========================================================================
*       Macro definition
* ===========================================================================
*/
#define P60GETTYPE(x)       (BYTE)((UCHAR)(x) / 21)
#define P60GETINDEX(x)      (BYTE)((UCHAR)(x) % 21)
#define P60SETTYPE(x)       (UCHAR)((BYTE)(x) * 21) 
#define P60SETINDEX(x)      (UCHAR)((BYTE)(x))

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  _P60DATA {
    BYTE    bType;                      /* key type and index   */
    BYTE    abCtrl[MAX_TTLKEY_DATA];    /* key control field    */
	UCHAR	uchOrderDecType;
} P60DATA, FAR *LPP60DATA;

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
VOID    P60InitDlg(HWND);
VOID    P60ReadParaData(HWND);
VOID    P60InitItemList(HWND);
VOID    P60SetItemData(HWND);
VOID    P60GetItemData(HWND);
VOID    P60CtrlSetBtn(HWND);
VOID    P60SetItemList(HWND);
VOID    P60FinDlg(HWND, WPARAM);
VOID    P60WriteParaData(HWND);
VOID    P60InitCtrlDlg(HWND);
VOID    P60SetCtrlData(HWND);
VOID    P60GetCtrlData(HWND);
VOID	P60GetOrderDecType(HWND);

/* ===== End of P060.H ===== */
