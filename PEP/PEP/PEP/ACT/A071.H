/*
* ---------------------------------------------------------------------------
* Title         :   Set PPI Table Header File (AC 71)
* Category      :   Maintenance, NCR 2170 PEP for Windows (HP US Model)
* Program Name  :   A071.H
* Copyright (C) :   1996, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Feb-02-96 : 00.00.01 : M.Suzuki   : Initial
* Sep-14-98 : 03.03.00 : A.Mitsui   : V3.3
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
*       Dialog ID definition
* ===========================================================================
*/
#define IDD_A71EDIT                  2072

#define IDD_A71_LIST                17100

#define IDD_A71_QTY01        (IDD_A71_LIST +  1)
#define IDD_A71_QTY02        (IDD_A71_LIST +  2)
#define IDD_A71_QTY03        (IDD_A71_LIST +  3)
#define IDD_A71_QTY04        (IDD_A71_LIST +  4)
#define IDD_A71_QTY05        (IDD_A71_LIST +  5)
#define IDD_A71_QTY06        (IDD_A71_LIST +  6)
#define IDD_A71_QTY07        (IDD_A71_LIST +  7)
#define IDD_A71_QTY08        (IDD_A71_LIST +  8)
#define IDD_A71_QTY09        (IDD_A71_LIST +  9)
#define IDD_A71_QTY10        (IDD_A71_LIST + 10)
#define IDD_A71_QTY11        (IDD_A71_LIST + 11)
#define IDD_A71_QTY12        (IDD_A71_LIST + 12)
#define IDD_A71_QTY13        (IDD_A71_LIST + 13)
#define IDD_A71_QTY14        (IDD_A71_LIST + 14)
#define IDD_A71_QTY15        (IDD_A71_LIST + 15)
#define IDD_A71_QTY16        (IDD_A71_LIST + 16)
#define IDD_A71_QTY17        (IDD_A71_LIST + 17)
#define IDD_A71_QTY18        (IDD_A71_LIST + 18)
#define IDD_A71_QTY19        (IDD_A71_LIST + 19)
#define IDD_A71_QTY20        (IDD_A71_LIST + 20)
// adding 20 more PPI addresses
#define IDD_A71_QTY21        (IDD_A71_LIST + 21)
#define IDD_A71_QTY22        (IDD_A71_LIST + 22)
#define IDD_A71_QTY23        (IDD_A71_LIST + 23)
#define IDD_A71_QTY24        (IDD_A71_LIST + 24)
#define IDD_A71_QTY25        (IDD_A71_LIST + 25)
#define IDD_A71_QTY26        (IDD_A71_LIST + 26)
#define IDD_A71_QTY27        (IDD_A71_LIST + 27)
#define IDD_A71_QTY28        (IDD_A71_LIST + 28)
#define IDD_A71_QTY29        (IDD_A71_LIST + 29)
#define IDD_A71_QTY30        (IDD_A71_LIST + 30)
#define IDD_A71_QTY31        (IDD_A71_LIST + 31)
#define IDD_A71_QTY32        (IDD_A71_LIST + 32)
#define IDD_A71_QTY33        (IDD_A71_LIST + 33)
#define IDD_A71_QTY34        (IDD_A71_LIST + 34)
#define IDD_A71_QTY35        (IDD_A71_LIST + 35)
#define IDD_A71_QTY36        (IDD_A71_LIST + 36)
#define IDD_A71_QTY37        (IDD_A71_LIST + 37)
#define IDD_A71_QTY38        (IDD_A71_LIST + 38)
#define IDD_A71_QTY39        (IDD_A71_LIST + 39)
#define IDD_A71_QTY40        (IDD_A71_LIST + 40)

#define IDD_A71_PRC01        (IDD_A71_LIST + 41)
#define IDD_A71_PRC02        (IDD_A71_LIST + 42)
#define IDD_A71_PRC03        (IDD_A71_LIST + 43)
#define IDD_A71_PRC04        (IDD_A71_LIST + 44)
#define IDD_A71_PRC05        (IDD_A71_LIST + 45)
#define IDD_A71_PRC06        (IDD_A71_LIST + 46)
#define IDD_A71_PRC07        (IDD_A71_LIST + 47)
#define IDD_A71_PRC08        (IDD_A71_LIST + 48)
#define IDD_A71_PRC09        (IDD_A71_LIST + 49)
#define IDD_A71_PRC10        (IDD_A71_LIST + 50)
#define IDD_A71_PRC11        (IDD_A71_LIST + 51)
#define IDD_A71_PRC12        (IDD_A71_LIST + 52)
#define IDD_A71_PRC13        (IDD_A71_LIST + 53)
#define IDD_A71_PRC14        (IDD_A71_LIST + 54)
#define IDD_A71_PRC15        (IDD_A71_LIST + 55)
#define IDD_A71_PRC16        (IDD_A71_LIST + 56)
#define IDD_A71_PRC17        (IDD_A71_LIST + 57)
#define IDD_A71_PRC18        (IDD_A71_LIST + 58)
#define IDD_A71_PRC19        (IDD_A71_LIST + 59)
#define IDD_A71_PRC20        (IDD_A71_LIST + 60)
// adding 20 more PPI addresses
#define IDD_A71_PRC21        (IDD_A71_LIST + 61)
#define IDD_A71_PRC22        (IDD_A71_LIST + 62)
#define IDD_A71_PRC23        (IDD_A71_LIST + 63)
#define IDD_A71_PRC24        (IDD_A71_LIST + 64)
#define IDD_A71_PRC25        (IDD_A71_LIST + 65)
#define IDD_A71_PRC26        (IDD_A71_LIST + 66)
#define IDD_A71_PRC27        (IDD_A71_LIST + 67)
#define IDD_A71_PRC28        (IDD_A71_LIST + 68)
#define IDD_A71_PRC29        (IDD_A71_LIST + 69)
#define IDD_A71_PRC30        (IDD_A71_LIST + 70)
#define IDD_A71_PRC31        (IDD_A71_LIST + 71)
#define IDD_A71_PRC32        (IDD_A71_LIST + 72)
#define IDD_A71_PRC33        (IDD_A71_LIST + 73)
#define IDD_A71_PRC34        (IDD_A71_LIST + 74)
#define IDD_A71_PRC35        (IDD_A71_LIST + 75)
#define IDD_A71_PRC36        (IDD_A71_LIST + 76)
#define IDD_A71_PRC37        (IDD_A71_LIST + 77)
#define IDD_A71_PRC38        (IDD_A71_LIST + 78)
#define IDD_A71_PRC39        (IDD_A71_LIST + 79)
#define IDD_A71_PRC40        (IDD_A71_LIST + 80)

#define IDD_A71_SPIN_QTY01   (IDD_A71_LIST + 81)
#define IDD_A71_SPIN_QTY02   (IDD_A71_LIST + 82)
#define IDD_A71_SPIN_QTY03   (IDD_A71_LIST + 83)
#define IDD_A71_SPIN_QTY04   (IDD_A71_LIST + 84)
#define IDD_A71_SPIN_QTY05   (IDD_A71_LIST + 85)
#define IDD_A71_SPIN_QTY06   (IDD_A71_LIST + 86)
#define IDD_A71_SPIN_QTY07   (IDD_A71_LIST + 87)
#define IDD_A71_SPIN_QTY08   (IDD_A71_LIST + 88)
#define IDD_A71_SPIN_QTY09   (IDD_A71_LIST + 89)
#define IDD_A71_SPIN_QTY10   (IDD_A71_LIST + 90)
#define IDD_A71_SPIN_QTY11   (IDD_A71_LIST + 91)
#define IDD_A71_SPIN_QTY12   (IDD_A71_LIST + 92)
#define IDD_A71_SPIN_QTY13   (IDD_A71_LIST + 93)
#define IDD_A71_SPIN_QTY14   (IDD_A71_LIST + 94)
#define IDD_A71_SPIN_QTY15   (IDD_A71_LIST + 95)
#define IDD_A71_SPIN_QTY16   (IDD_A71_LIST + 96)
#define IDD_A71_SPIN_QTY17   (IDD_A71_LIST + 97)
#define IDD_A71_SPIN_QTY18   (IDD_A71_LIST + 98)
#define IDD_A71_SPIN_QTY19   (IDD_A71_LIST + 99)
#define IDD_A71_SPIN_QTY20   (IDD_A71_LIST + 100)
// adding 20 more PPI addresses
#define IDD_A71_SPIN_QTY21   (IDD_A71_LIST + 101)
#define IDD_A71_SPIN_QTY22   (IDD_A71_LIST + 102)
#define IDD_A71_SPIN_QTY23   (IDD_A71_LIST + 103)
#define IDD_A71_SPIN_QTY24   (IDD_A71_LIST + 104)
#define IDD_A71_SPIN_QTY25   (IDD_A71_LIST + 105)
#define IDD_A71_SPIN_QTY26   (IDD_A71_LIST + 106)
#define IDD_A71_SPIN_QTY27   (IDD_A71_LIST + 107)
#define IDD_A71_SPIN_QTY28   (IDD_A71_LIST + 108)
#define IDD_A71_SPIN_QTY29   (IDD_A71_LIST + 109)
#define IDD_A71_SPIN_QTY30   (IDD_A71_LIST + 110)
#define IDD_A71_SPIN_QTY31   (IDD_A71_LIST + 111)
#define IDD_A71_SPIN_QTY32   (IDD_A71_LIST + 112)
#define IDD_A71_SPIN_QTY33   (IDD_A71_LIST + 113)
#define IDD_A71_SPIN_QTY34   (IDD_A71_LIST + 114)
#define IDD_A71_SPIN_QTY35   (IDD_A71_LIST + 115)
#define IDD_A71_SPIN_QTY36   (IDD_A71_LIST + 116)
#define IDD_A71_SPIN_QTY37   (IDD_A71_LIST + 117)
#define IDD_A71_SPIN_QTY38   (IDD_A71_LIST + 118)
#define IDD_A71_SPIN_QTY39   (IDD_A71_LIST + 119)
#define IDD_A71_SPIN_QTY40   (IDD_A71_LIST + 120)

#define IDD_A71_SPIN_PRC01   (IDD_A71_LIST + 121)
#define IDD_A71_SPIN_PRC02   (IDD_A71_LIST + 122)
#define IDD_A71_SPIN_PRC03   (IDD_A71_LIST + 123)
#define IDD_A71_SPIN_PRC04   (IDD_A71_LIST + 124)
#define IDD_A71_SPIN_PRC05   (IDD_A71_LIST + 125)
#define IDD_A71_SPIN_PRC06   (IDD_A71_LIST + 126)
#define IDD_A71_SPIN_PRC07   (IDD_A71_LIST + 127)
#define IDD_A71_SPIN_PRC08   (IDD_A71_LIST + 128)
#define IDD_A71_SPIN_PRC09   (IDD_A71_LIST + 129)
#define IDD_A71_SPIN_PRC10   (IDD_A71_LIST + 130)
#define IDD_A71_SPIN_PRC11   (IDD_A71_LIST + 131)
#define IDD_A71_SPIN_PRC12   (IDD_A71_LIST + 132)
#define IDD_A71_SPIN_PRC13   (IDD_A71_LIST + 133)
#define IDD_A71_SPIN_PRC14   (IDD_A71_LIST + 134)
#define IDD_A71_SPIN_PRC15   (IDD_A71_LIST + 135)
#define IDD_A71_SPIN_PRC16   (IDD_A71_LIST + 136)
#define IDD_A71_SPIN_PRC17   (IDD_A71_LIST + 137)
#define IDD_A71_SPIN_PRC18   (IDD_A71_LIST + 138)
#define IDD_A71_SPIN_PRC19   (IDD_A71_LIST + 139)
#define IDD_A71_SPIN_PRC20   (IDD_A71_LIST + 140)
// adding 20 more PPI addresses
#define IDD_A71_SPIN_PRC21   (IDD_A71_LIST + 141)
#define IDD_A71_SPIN_PRC22   (IDD_A71_LIST + 142)
#define IDD_A71_SPIN_PRC23   (IDD_A71_LIST + 143)
#define IDD_A71_SPIN_PRC24   (IDD_A71_LIST + 144)
#define IDD_A71_SPIN_PRC25   (IDD_A71_LIST + 145)
#define IDD_A71_SPIN_PRC26   (IDD_A71_LIST + 146)
#define IDD_A71_SPIN_PRC27   (IDD_A71_LIST + 147)
#define IDD_A71_SPIN_PRC28   (IDD_A71_LIST + 148)
#define IDD_A71_SPIN_PRC29   (IDD_A71_LIST + 149)
#define IDD_A71_SPIN_PRC30   (IDD_A71_LIST + 150)
#define IDD_A71_SPIN_PRC31   (IDD_A71_LIST + 151)
#define IDD_A71_SPIN_PRC32   (IDD_A71_LIST + 152)
#define IDD_A71_SPIN_PRC33   (IDD_A71_LIST + 153)
#define IDD_A71_SPIN_PRC34   (IDD_A71_LIST + 154)
#define IDD_A71_SPIN_PRC35   (IDD_A71_LIST + 155)
#define IDD_A71_SPIN_PRC36   (IDD_A71_LIST + 156)
#define IDD_A71_SPIN_PRC37   (IDD_A71_LIST + 157)
#define IDD_A71_SPIN_PRC38   (IDD_A71_LIST + 158)
#define IDD_A71_SPIN_PRC39   (IDD_A71_LIST + 159)
#define IDD_A71_SPIN_PRC40   (IDD_A71_LIST + 160)

#define IDD_A71_GROUP        (IDD_A71_LIST + 161)

#define IDD_A71_CHANGE       (IDD_A71_LIST + 162)
#define IDD_A71_CLEAR        (IDD_A71_LIST + 163)
#define IDD_A71_NORECORDS    (IDD_A71_LIST + 164)

#define IDD_A71_STRADDR      (IDD_A71_LIST + 164)
#define IDD_A71_STRQTY       (IDD_A71_LIST + 165)
#define IDD_A71_STRPRICE     (IDD_A71_LIST + 166)
#define IDD_A71_STRQTYLEN    (IDD_A71_LIST + 167)
#define IDD_A71_STRPRICELEN  (IDD_A71_LIST + 168)
#define IDD_A71_STRADDR2     (IDD_A71_LIST + 169)
#define IDD_A71_STRQTY2      (IDD_A71_LIST + 170)
#define IDD_A71_STRPRICE2    (IDD_A71_LIST + 171)
#define IDD_A71_STRQTY2LEN   (IDD_A71_LIST + 172)
#define IDD_A71_STRPRICE2LEN (IDD_A71_LIST + 173)
#define IDD_A71_STRADDR3     (IDD_A71_LIST + 174)
#define IDD_A71_STRQTY3      (IDD_A71_LIST + 175)
#define IDD_A71_STRPRICE3    (IDD_A71_LIST + 176)
#define IDD_A71_STRQTY3LEN   (IDD_A71_LIST + 177)
#define IDD_A71_STRPRICE3LEN (IDD_A71_LIST + 178)
#define IDD_A71_STRADDR4     (IDD_A71_LIST + 179)
#define IDD_A71_STRQTY4      (IDD_A71_LIST + 180)
#define IDD_A71_STRPRICE4    (IDD_A71_LIST + 181)
#define IDD_A71_STRQTY4LEN   (IDD_A71_LIST + 182)
#define IDD_A71_STRPRICE4LEN (IDD_A71_LIST + 183)

#define IDD_A71_ADDRESS01        (IDD_A71_LIST +  184)
#define IDD_A71_ADDRESS02        (IDD_A71_LIST +  185)
#define IDD_A71_ADDRESS03        (IDD_A71_LIST +  186)
#define IDD_A71_ADDRESS04        (IDD_A71_LIST +  187)
#define IDD_A71_ADDRESS05        (IDD_A71_LIST +  188)
#define IDD_A71_ADDRESS06        (IDD_A71_LIST +  189)
#define IDD_A71_ADDRESS07        (IDD_A71_LIST +  190)
#define IDD_A71_ADDRESS08        (IDD_A71_LIST +  191)
#define IDD_A71_ADDRESS09        (IDD_A71_LIST +  192)
#define IDD_A71_ADDRESS10        (IDD_A71_LIST + 193)
#define IDD_A71_ADDRESS11        (IDD_A71_LIST + 194)
#define IDD_A71_ADDRESS12        (IDD_A71_LIST + 195)
#define IDD_A71_ADDRESS13        (IDD_A71_LIST + 196)
#define IDD_A71_ADDRESS14        (IDD_A71_LIST + 197)
#define IDD_A71_ADDRESS15        (IDD_A71_LIST + 198)
#define IDD_A71_ADDRESS16        (IDD_A71_LIST + 199)
#define IDD_A71_ADDRESS17        (IDD_A71_LIST + 200)
#define IDD_A71_ADDRESS18        (IDD_A71_LIST + 201)
#define IDD_A71_ADDRESS19        (IDD_A71_LIST + 202)
#define IDD_A71_ADDRESS20        (IDD_A71_LIST + 203)
// add 20 more PPI addresses
#define IDD_A71_ADDRESS21        (IDD_A71_LIST +  204)
#define IDD_A71_ADDRESS22        (IDD_A71_LIST +  205)
#define IDD_A71_ADDRESS23        (IDD_A71_LIST +  206)
#define IDD_A71_ADDRESS24        (IDD_A71_LIST +  207)
#define IDD_A71_ADDRESS25        (IDD_A71_LIST +  208)
#define IDD_A71_ADDRESS26        (IDD_A71_LIST +  209)
#define IDD_A71_ADDRESS27        (IDD_A71_LIST +  210)
#define IDD_A71_ADDRESS28        (IDD_A71_LIST +  211)
#define IDD_A71_ADDRESS29        (IDD_A71_LIST +  212)
#define IDD_A71_ADDRESS30        (IDD_A71_LIST + 213)
#define IDD_A71_ADDRESS31        (IDD_A71_LIST + 214)
#define IDD_A71_ADDRESS32        (IDD_A71_LIST + 215)
#define IDD_A71_ADDRESS33        (IDD_A71_LIST + 216)
#define IDD_A71_ADDRESS34        (IDD_A71_LIST + 217)
#define IDD_A71_ADDRESS35        (IDD_A71_LIST + 218)
#define IDD_A71_ADDRESS36        (IDD_A71_LIST + 219)
#define IDD_A71_ADDRESS37        (IDD_A71_LIST + 220)
#define IDD_A71_ADDRESS38        (IDD_A71_LIST + 221)
#define IDD_A71_ADDRESS39        (IDD_A71_LIST + 222)
#define IDD_A71_ADDRESS40        (IDD_A71_LIST + 223)

#define IDD_A71_AFTERMAX		(IDD_A71_LIST + 224)
#define IDD_A71_BEFOREQTY		(IDD_A71_LIST + 225)
#define IDD_A71_MINTOTALSALE	(IDD_A71_LIST + 226)
#define IDD_A71_COMBO_AFTERMAX		(IDD_A71_LIST + 227)
#define IDD_A71_COMBO_BEFOREQTY		(IDD_A71_LIST + 228)
#define IDD_A71_EDIT_MINTOTALSALE	(IDD_A71_LIST + 229)
#define IDD_A71_ADVOPTIONS		(IDD_A71_LIST + 230)
#define IDD_A71_CHK_OVERRIDEPLU		(IDD_A71_LIST + 231)
#define IDD_A71_CHK_PRICEDISCPREM	(IDD_A71_LIST + 232)
#define IDD_A71_CHK_DISCOUNTPLU		(IDD_A71_LIST + 233)
#define IDD_A71_CHK_PREMIUMPLU		(IDD_A71_LIST + 234)
#define IDD_A71_PRICEOPTIONS		(IDD_A71_LIST + 235)

/*
* ===========================================================================
*       String ID
* ===========================================================================
*/
#define IDS_A71_DESC            IDS_A71
#define IDS_A71_GROUP           (IDS_A71 + 1)
#define IDS_A71_FLEX_EMPTY      (IDS_A71 + 2)
#define IDS_A71_PRC_NOT_ORDER   (IDS_A71 + 3)
#define IDS_A71_QTY_NOT_ORDER   (IDS_A71 + 4)
#define IDS_A71_NODATA          (IDS_A71 + 5)
#define IDS_A71_NORECORDS       (IDS_A71 + 6)
#define IDS_A71_NOT_FOUND       (IDS_A71 + 7)

#define IDS_A71_STRADDR         (IDS_A71 + 8)
#define IDS_A71_STRQTY          (IDS_A71 + 9)
#define IDS_A71_STRPRICE        (IDS_A71 +10)
#define IDS_A71_STRQTYLEN       (IDS_A71 +11)
#define IDS_A71_STRPRICELEN     (IDS_A71 +12)
#define IDS_A71_STRADDR2        (IDS_A71 +13)
#define IDS_A71_STRQTY2         (IDS_A71 +14)
#define IDS_A71_STRPRICE2       (IDS_A71 +15)
#define IDS_A71_STRQTY2LEN      (IDS_A71 +16)
#define IDS_A71_STRPRICE2LEN    (IDS_A71 +17)
#define IDS_A71_STRADDR3        (IDS_A71 +18)
#define IDS_A71_STRQTY3         (IDS_A71 +19)
#define IDS_A71_STRPRICE3       (IDS_A71 +20)
#define IDS_A71_STRQTY3LEN      (IDS_A71 +21)
#define IDS_A71_STRPRICE3LEN    (IDS_A71 +22)
#define IDS_A71_STRADDR4        (IDS_A71 +23)
#define IDS_A71_STRQTY4         (IDS_A71 +24)
#define IDS_A71_STRPRICE4       (IDS_A71 +25)
#define IDS_A71_STRQTY4LEN      (IDS_A71 +26)
#define IDS_A71_STRPRICE4LEN    (IDS_A71 +27)

#define IDS_A71_CHANGE          (IDS_A71 +28)
#define IDS_A71_CLEAR           (IDS_A71 +29)

#define IDS_A71_CAPTION         (IDS_A71 +30)
#define IDS_A71_EDITCAP         (IDS_A71 +31)
#define IDS_A71_STAFTERMAX0		(IDS_A71 +32)
#define IDS_A71_STAFTERMAX1		(IDS_A71 +33)
#define IDS_A71_STBEFOREQTY0	(IDS_A71 +34)
#define IDS_A71_STBEFOREQTY1	(IDS_A71 +35)

#define IDS_A71_CHK_OVERRIDEPLU	(IDS_A71 +36)
#define IDS_A71_CHK_DISCOUNTPLU	(IDS_A71 +37)
#define IDS_A71_CHK_PREMIUMPLU	(IDS_A71 +38)
#define IDS_A71_PRICEOPTIONS	(IDS_A71 +39)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define A71_CODE_MAX    FLEX_PPI_MAX
#define A71_TABLE_MAX   MAX_PARAPPI_SIZE
#define A71_CODE_MIN        1

#define A71_PRC_MAX    999999L
#define A71_PRC_MIN         0L
#define A71_QTY_MAX       255
#define A71_QTY_MIN         0
#define A71_CONVERT        10
#define A71_CURREC_MIN      0

#define A71_SPIN_STEP       1
#define A71_SPIN_TURBO     30
#define A71_SPIN_TURSTEP   10

#define A71_QTY_LEN         3
#define A71_PRC_LEN         6
#define A71_BUFF_LEN      128

/*
* ===========================================================================
*       Macro Define Declarations
* ===========================================================================
*/
#define A71_GET_ADDR_MIN(x) ((UINT)((x) * A71_TABLE_MAX + 1))
#define A71_GET_ADDR_MAX(x) ((UINT)((x) * A71_TABLE_MAX + A71_TABLE_MAX))

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
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

HWND A071ModeLessDialog (HWND hParentWnd, int nCmdShow);
HWND  A071ModeLessDialogWnd (void);

/* ===== End of File ===== */
