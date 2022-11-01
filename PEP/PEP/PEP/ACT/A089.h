/*
* ---------------------------------------------------------------------------
* Title         :   Set Currency Conversion Rates Header File (AC 89)
* Category      :   Maintenance, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   A089.H
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
* Jun-16-99 : 03.04.01 : hrkato     : Euro Enhancements
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
*       Dialog ID 
* ===========================================================================
*/
#define IDD_A89EDIT1   18901   /* editbox for address 1                    */
#define IDD_A89EDIT2   18902   /* editbox for address 2                    */
#define IDD_A89EDIT3   18903   /* editbox for address 3, Saratoga          */
#define IDD_A89EDIT4   18904   /* editbox for address 4, Saratoga          */
#define IDD_A89EDIT5   18905   /* editbox for address 5, Saratoga          */
#define IDD_A89EDIT6   18906   /* editbox for address 6, Saratoga          */
#define IDD_A89EDIT7   18907   /* editbox for address 7, Saratoga          */
#define IDD_A89EDIT8   18908   /* editbox for address 8, Saratoga          */

#define IDD_A89_ADDR	18909
#define IDD_A89_DATA	18910
#define IDD_A89_DATARANGE	18911
#define IDD_A89_DATARANGE1	18912
#define IDD_A89_DATARANGE2	18913
#define IDD_A89_ADD1	18914
#define IDD_A89_ADD2	18915
#define IDD_A89_ADD3	18916
#define IDD_A89_ADD4	18917
#define IDD_A89_ADD5	18918
#define IDD_A89_ADD6	18919
#define IDD_A89_ADD7	18920
#define IDD_A89_ADD8	18921

#define IDD_A89EDRANGE1   18931   /* data range prompt text for address 1 */
#define IDD_A89EDRANGE2   18932   /* data range prompt text for address 2 */
#define IDD_A89EDRANGE3   18933   /* data range prompt text for address 3 */
#define IDD_A89EDRANGE4   18934   /* data range prompt text for address 4 */
#define IDD_A89EDRANGE5   18935   /* data range prompt text for address 5 */
#define IDD_A89EDRANGE6   18936   /* data range prompt text for address 6 */
#define IDD_A89EDRANGE7   18937   /* data range prompt text for address 7 */
#define IDD_A89EDRANGE8   18938   /* data range prompt text for address 8 */

#define IDS_A89_RNGOVR  (IDS_A89 + 0)
#define IDS_A89_RNGOVR2 (IDS_A89 + 1)  /* V3.4 */
#define IDS_A89_CLMOVR  (IDS_A89 + 2)
#define IDS_A89_CLMOVR2 (IDS_A89 + 3)  /* V3.4 */

/*
* ===========================================================================
*       Number
* ===========================================================================
*/
#define A89_LIMITLEN       10   /* limit text length                    */
#define A89_CNVLONG   100000L   /* for convert to long type             */
#define A89_CNVLONG2 1000000L   /* for convert to long type             */
#define A89_DECPNT         46   /* ASCII code of decimal point"."       */
#define A89_DECCOLUMN       5   /* data column under decimal point      */
#define A89_DECCOLUMN2      6   /* data column under decimal point      */
#define A89_CNVDEC        10L   /* for convert by decimal               */
#define A89_CNVDEC2      100L   /* for convert by decimal               */
#define A89_MAX         9999L   /* data max by long type                */
#define A89_MAX2         999L   /* data max by long type                */
#define A89_ERR_LEN       128   /* length of error message buffer       */


/* ===== End of A089.H ===== */
