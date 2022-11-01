/*
* ---------------------------------------------------------------------------
* Title         :   Set Transaction Limit Header File (AC 128)
* Category      :   Maintenance, NCR 2170 PEP for Windows (GP US Model)
* Program Name  :   A128.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
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
* Apr-13-93 : 00.00.01 : T.Nakahata : Initial
* Mar-09-94 : 00.01.00 : S.Liu      : R2.0 Enhance Model
* Nov-12-98 : 02.01.02 : A.Mitsui   : fix data minimum
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
// Following addresses are mapped to ParaMiscPara[MAX_MISC_SIZE]
#define IDD_A128EDIT1   12801   /* editbox for address 1                    */
#define IDD_A128EDIT2   12802   /* editbox for address 2                    */
#define IDD_A128EDIT3   12803   /* editbox for address 3                    */
#define IDD_A128EDIT4   12804   /* editbox for address 4                    */
#define IDD_A128EDIT5   12805   /* editbox for address 5                    */
#define IDD_A128EDIT6   12806   /* editbox for address 6                    */
#define IDD_A128EDIT7   12807   /* editbox for address 7                    */
#define IDD_A128EDIT8	12808	// editbox for address 8, Credit Card Floor Amount	***PDINU

#define	A128_CC_INFO	12809	// This is reserved for the Status bits used in the Credit card floor amount
								// The reason I reserved this ID number is because when the GetData() function
								// gets the info from the editboxes, it will think this space is an edit box
								// because the for loop goes from 0 to A128_ADDR_MAX
#define IDD_A128EDIT9	12809	// editbox for address 9, used for status bits for Credit Card floor amount
// Following addresses are mapped to ParaStoreForward[MAX_MISC_SIZE]
#define IDD_A128EDIT10	12810	// editbox for address 10,
#define IDD_A128EDIT11	12811	// editbox for address 11,
#define IDD_A128EDIT12	12812	// editbox for address 12,
#define IDD_A128EDIT13	12813	// editbox for address 13,
#define IDD_A128EDIT14	12814	// editbox for address 14,
#define IDD_A128EDIT15	12815	// editbox for address 15,
#define IDD_A128EDIT16	12816	// editbox for address 16,
#define IDD_A128EDIT17	12817	// editbox for address 17,
#define IDD_A128EDIT18	12818	// editbox for address 18,

#define IDD_A128_ADDR1  12821
#define IDD_A128_ADDR2  12822
#define IDD_A128_ADDR3  12823
#define IDD_A128_ADDR4  12824
#define IDD_A128_ADDR5  12825
#define IDD_A128_ADDR6  12826
#define IDD_A128_ADDR7  12827
#define IDD_A128_ADDR8	12828	//Credit Card Floor Amount	***PDINU
#define IDD_A128_ADDR9	12829	//
#define IDD_A128_ADDR10	12830	//
#define IDD_A128_ADDR11	12831	//
#define IDD_A128_ADDR12	12832	//
#define IDD_A128_ADDR13	12833	//
#define IDD_A128_ADDR14	12834	//
#define IDD_A128_ADDR15	12835	//
#define IDD_A128_ADDR16	12836	//
#define IDD_A128_ADDR17	12837	//
#define IDD_A128_ADDR18	12838	//

#define IDD_A128_ADDR1_RNG  12841
#define IDD_A128_ADDR2_RNG  12842
#define IDD_A128_ADDR3_RNG  12843
#define IDD_A128_ADDR4_RNG  12844
#define IDD_A128_ADDR5_RNG  12845
#define IDD_A128_ADDR6_RNG  12846
#define IDD_A128_ADDR7_RNG  12847
#define	IDD_A128_ADDR8_RNG	12848		//Credit Card Floor Amount	***PDINU
#define	IDD_A128_ADDR9_RNG	12849		//
#define	IDD_A128_ADDR10_RNG	12850		//
#define	IDD_A128_ADDR11_RNG	12851		//
#define	IDD_A128_ADDR12_RNG	12852		//
#define	IDD_A128_ADDR13_RNG	12853		//
#define	IDD_A128_ADDR14_RNG	12854		//
#define	IDD_A128_ADDR15_RNG	12855		//
#define	IDD_A128_ADDR16_RNG	12856		//
#define	IDD_A128_ADDR17_RNG	12857		//
#define	IDD_A128_ADDR18_RNG	12858		//

#define IDD_A128_GRP_FOOD   12860
#define IDD_A128_GRP_CPN    12861
#define	IDD_A128_GRP_CC		12862		//Credit Card Floor Amount	***PDINU
#define	IDD_A128_GRP_SF		12863		//Store and Forward transaction limits


#define	IDD_A128ENABLE		12864		//Credit Card Floor Amount	***PDINU
#define	IDD_A128CUST		12865		//Credit Card Floor Amount	***PDINU
#define	IDD_A128MERC		12866		//Credit Card Floor Amount	***PDINU
#define	IDD_A128CUST_AUTH	12867		//Credit Card Floor Amount	***PDINU
#define	IDD_A128MERC_AUTH	12868		//Credit Card Floor Amount	***PDINU

/*
* ===========================================================================
*       Resource ID definition
* ===========================================================================
*/
#define IDS_A128_RNGOVER_1  IDS_A128            /* Range over Message */
#define IDS_A128_RNGOVER_2  (IDS_A128 + 1)      /* Range over Message */

#define IDS_A128_ADDR1      (IDS_A128 + 2)
#define IDS_A128_ADDR2      (IDS_A128 + 3)
#define IDS_A128_ADDR3      (IDS_A128 + 4)
#define IDS_A128_ADDR4      (IDS_A128 + 5)
#define IDS_A128_ADDR5      (IDS_A128 + 6)
#define IDS_A128_ADDR6      (IDS_A128 + 7)
#define IDS_A128_ADDR7      (IDS_A128 + 8)
#define IDS_A128_ADDR8      (IDS_A128 + 9)
#define IDS_A128_ADDR9      (IDS_A128 + 10)
#define IDS_A128_ADDR10     (IDS_A128 + 11)
#define IDS_A128_ADDR11     (IDS_A128 + 12)
#define IDS_A128_ADDR12     (IDS_A128 + 13)
#define IDS_A128_ADDR13     (IDS_A128 + 14)
#define IDS_A128_ADDR14     (IDS_A128 + 15)
#define IDS_A128_ADDR15     (IDS_A128 + 16)
#define IDS_A128_ADDR16     (IDS_A128 + 17)
#define IDS_A128_ADDR17     (IDS_A128 + 18)
#define IDS_A128_ADDR18     (IDS_A128 + 19)

#define IDS_A128_GRP_FOOD   (IDS_A128 + 21)
#define IDS_A128_GRP_CPN    (IDS_A128 + 22)
#define	IDS_A128_GRP_CC		(IDS_A128 + 23)	//Credit Card Floor Amount	***PDINU
#define IDS_A128_GRP_SF     (IDS_A128 + 24)	//Store and Forward transaction limits
/*
* ===========================================================================
*       Value definition
* ===========================================================================
*/
#define A128_ADDR_MAX   (IDD_A128EDIT11 - IDD_A128EDIT1 + 1)
                         
/* <HQ> add act 128 */                         
#define A128_DATA_MIN   0L       /* data minimum by long type              *//* bug-fix */
/*#define A128_DATA_MAX1  9999L    data maximum by long type               */
/*#define A128_DATA_MAX1  999999999L*/ /* data maximum by long type        */
/*#define A128_DATA_MAX2  999999999L*/ /* data maximum by long type        */
//changed back to 9999 to be congruent with NHPOS JHHJ 3-28-05
#define A128_DATA_MAX1  9999L   /* data maximum by long type            *//* ### MOD Saratoga (9char --> 7char)(052900) */
#define A128_DATA_MAX2  9999999L   /* data maximum by long type            *//* ### MOD Saratoga (9char --> 7char)(052900) */

/*#define A128_COLUMN_1       5    data column(including of decimal point)  */
#define A128_COLUMN_1       8   /* data column(including of decimal point)  *//* ### MOD Saratoga (9char --> 7char)(052900) */
/*#define A128_COLUMN_2      10    data column(including of decimal point)  */
#define A128_COLUMN_2       8   /* data column(including of decimal point)  *//* ### MOD Saratoga (9char --> 7char)(052900) */
/*#define A128_DECCOLUMN      2   data column under decimal point          */
#define A128_DECCOLUMN      0   /* data column under decimal point          */
#define A128_DATA_LEN      16   /* length of data                           */

#define A128_DECPNT        46   /* ASCII code of decimal point"."           */

/*#define A128_CNVLONG     100L   for convert to long type                 */
/*#define A128_CNVDEC       10L   for convert by decimal                   */

#define A128_CNVLONG       1L   /* for convert to long type                 */
#define A128_CNVDEC        1L   /* for convert by decimal                   */

#define A128_CC_BITS		8	//Location where the status bits below will be stored

#define A128_CC_ENABLE		0x01	// Status bit to determine if the properties should be enabled	***PDINU
#define A128_CC_CUST		0x02	// Status bit to not print cutomer receipt	***PDINU
#define A128_CC_MERC		0x04	// Status bit to not print merchant receipt	***PDINU
#define A128_CC_CUST_AUTH	0x08	// Status bit to not print card authorization text on cutomer receipt	***PDINU
#define A128_CC_MERC_AUTH	0x10	// Status bit to not print card authorization text on merchant receipt	***PDINU
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
static  VOID    InitDlg(HWND, ULONG FAR *, WORD);
static  VOID    FinDlg(HWND, ULONG FAR *, WORD);
static  VOID    SetData(HWND, ULONG FAR *);
static  BOOL    GetData(HWND, ULONG FAR *);
static  DWORD   ConvData(LPWSTR, WPARAM);
static  BOOL    ChkRng(HWND, WORD wEditID, WPARAM);
static  void    DispError(HWND, WORD wEditID);

/* ===== end of file ===== */
