/*
* ---------------------------------------------------------------------------
* Title         :   Program Mode Set Hotel Id & SLD Header File (Prog. 54)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P054.H
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
* Dec-07-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
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
#define P54_SPIN_OFFSET         30


#define IDD_P54_HOTEL         25401    /* EditText1 for Security Code      */
#define IDD_P54_SLD           25402    /* EditText2 for Security Code      */
#define IDD_P54_PRODUCT_ID    25403    /* EditText3 for Security Code      */
#define IDD_P54_TEND1         25404    /* EditText4 for Security Code      */
#define IDD_P54_TEND2         25405    /* EditText5 for Security Code      */
#define IDD_P54_TEND3         25406    /* EditText6 for Security Code      */
#define IDD_P54_TEND4         25407    /* EditText7 for Security Code      */
#define IDD_P54_TEND5         25408    /* EditText8 for Security Code      */
#define IDD_P54_TEND6         25409    /* EditText9 for Security Code      */
#define IDD_P54_TEND7         25410    /* EditText10 for Security Code     */
#define IDD_P54_TEND8         25411    /* EditText11 for Security Code     */
#define IDD_P54_TEND9         25412    /* EditText12 for Security Code     */
#define IDD_P54_TEND10        25413    /* EditText13 for Security Code     */
#define IDD_P54_TEND11        25414    /* EditText14 for Security Code     */
#define IDD_P54_TEND12        25415
#define IDD_P54_TEND13        25416
#define IDD_P54_TEND14        25417
#define IDD_P54_TEND15        25418
#define IDD_P54_TEND16        25419
#define IDD_P54_TEND17        25420
#define IDD_P54_TEND18        25421
#define IDD_P54_TEND19        25422
#define IDD_P54_TEND20        25423
#define IDD_P54_APRVL_CODE    25424    /* EditText18 for Security Code     */



#define IDD_P54_SPIN1   (IDD_P54_HOTEL + P54_SPIN_OFFSET)  
#define IDD_P54_SPIN2   (IDD_P54_SLD + P54_SPIN_OFFSET)
#define IDD_P54_SPIN3   (IDD_P54_PRODUCT_ID + P54_SPIN_OFFSET)
#define IDD_P54_SPIN4   (IDD_P54_TEND1 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN5   (IDD_P54_TEND2 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN6   (IDD_P54_TEND3 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN7   (IDD_P54_TEND4 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN8   (IDD_P54_TEND5 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN9   (IDD_P54_TEND6 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN10  (IDD_P54_TEND7 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN11  (IDD_P54_TEND8 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN12  (IDD_P54_TEND9 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN13  (IDD_P54_TEND10 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN14  (IDD_P54_TEND11 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN15  (IDD_P54_TEND12 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN16  (IDD_P54_TEND13 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN17  (IDD_P54_TEND14 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN18  (IDD_P54_TEND15 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN19  (IDD_P54_TEND16 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN20  (IDD_P54_TEND17 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN21  (IDD_P54_TEND18 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN22  (IDD_P54_TEND19 + P54_SPIN_OFFSET)
#define IDD_P54_SPIN23  (IDD_P54_TEND20 + P54_SPIN_OFFSET)

#define IDD_P54_HOTELID	25454
#define IDD_P54_HID_RANGE1	25455
#define IDD_P54_SLD_TEXT	25456
#define IDD_P54_PRODUCTID_TEXT	25457
#define IDD_P54_ID_TEXT	25458
#define IDD_P54_DATA_RANGE1	25459
#define IDD_P54_AC_TENDKEY	25460
#define IDD_P54_DATA_RANGE2	25461
#define IDD_P54_CASH	25462
#define IDD_P54_CHECK	25463
#define IDD_P54_CHARGE	25464
#define IDD_P54_MISC1	25465
#define IDD_P54_MISC2	25466
#define IDD_P54_MISC3	25467
#define IDD_P54_MISC4	25468
#define IDD_P54_MISC5	25469
#define IDD_P54_MISC6	25470
#define IDD_P54_MISC7	25471
#define IDD_P54_MISC8	25472
#define IDD_P54_MISC9   25473
#define IDD_P54_MISC10  25474
#define IDD_P54_MISC11  25475
#define IDD_P54_MISC12  25476
#define IDD_P54_MISC13  25477
#define IDD_P54_MISC14  25478
#define IDD_P54_MISC15  25479
#define IDD_P54_MISC16  25480
#define IDD_P54_MISC17  25481
#define IDD_P54_APRVLCODE_TEXT	25482
#define IDD_P54_HID_RANGE2	25483
#define IDD_P54_CRGPOST	25484
#define IDD_P54_EPT		25485
#define IDD_P54_SLD_RANGE		25486

/*
* ===========================================================================
*       String ID 
* ===========================================================================
*/
#define IDS_P54_OVER    (IDS_P54)

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P54_SLD_MAX      9999       /* Maximum Value of SLD Code   */
#define P54_SLD_MIN         0       /* Minimum Value of SLD Code   */
#define P54_HOTELID_MAX    99       /* Maximun Value of Hotel Id Code   */
#define P54_HOTELID_MIN     0       /* Minimum Value of Hotel Id Code   */
#define P54_APRVL_CODE_MAX 99       /* Maximun Value of Aprvl Code   */
#define P54_APRVL_CODE_MIN  0       /* Minimum Value of Aprvl Code   */
#define P54_PRODUCTID_MAX  '9999999999' /* Maximum Value of Security Code */
#define P54_PRODUCTID_MIN   0       /* Minimum Valuse of Security Code  */
#define P54_TEND_MAX        9       /* Maximum Value of Security Code   */
#define P54_TEND_MIN        0       /* Maximum Value of Security Code   */
#define P54_HOTELID_LEN     2       /* Length of the Test in IDD_HOTEL  */
#define P54_SLD_LEN         4       /* Length of the Text in IDD_SLD    */
#define P54_PRODUCTID_LEN  10       /* Length of the Text in IDD_PRODUCTID */
#define P54_SPIN_STEP       1       /* Normal Step of Spin Button       */
#define P54_SPIN_TURBO     30       /* Turbo Point of Spin Button       */
#define P54_SPIN_TURSTEP   10       /* Turbo Step of Spin Button        */
#define P54_NO_TEND        STD_TENDER_MAX      /* Number of Tenders *//* JHHJ 3-16-04 Tender 11->20*/

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
BOOL    P54SaveData(HWND hDlg);
BOOL    P54ChkRng(HWND hDlg, WORD wEditID, WCHAR *puchData);
VOID    P54DispError(HWND hDlg, WORD wErrID);

/* ===== End of File (P006.H) ===== */

