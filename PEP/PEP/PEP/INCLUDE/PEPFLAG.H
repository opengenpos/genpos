/*
* ---------------------------------------------------------------------------
* Title         :   File Modefy Control Flag Header File
* Category      :   NCR 2170 PEP for Windows    (Hotel US Model)
* Program Name  :   PEPFLAG.H
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All right reserved.
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
* Nov-15-93 : 00.00.01 : T.Yamamoto : Initial (Migrate from HP US Model)
* Mar-13-95 : 03.00.00 : H.Ishida   : Add PEP_MF_CPN
* Apr-06-95 : 03.00.00 : H.Ishida   : Add PEP_MF_CSTR
* Feb-01-96 : 03.00.05 : M.Suzuki   : Add PEP_MF_RPT,PPI
* Sep-25-98 : 03.03.00 : A.Mitsui   : Add PEP_MF_MLD
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
#ifndef PEPFLAG_INCLUDED_DEFINED
#define PEPFLAG_INCLUDED_DEFINED
/*
* ===========================================================================
*       Define Declarations
* ===========================================================================
*/
//RJC  null out PEPENTRY for 32 bit conversion
#ifndef PEPENTRY
#define     PEPENTRY    
#endif

/*
* ===========================================================================
*       Value definition for File Modification Control Procedure
* ===========================================================================
*/
#define PEP_MF_ALL          0x00003bff          /* all file            */ //ESMITH LAYOUT
#define PEP_MF_PROG         0x00000001          /* change Para. file   */
#define PEP_MF_ACT          0x00000002          /* change Para. file   */
#define PEP_MF_DEPT         0x00000004          /* change Dept. file   */
#define PEP_MF_PLU          0x00000008          /* change PLU file     */
#define PEP_MF_SERV         0x00000010          /* change Server file  */
#define PEP_MF_CASH         0x00000020          /* change Cashier file */
#define PEP_MF_ETK          0x00000040          /* change ETK file     */
#define PEP_MF_CPN          0x00000080          /* change CPN file     */
#define PEP_MF_TRANS        0x00000100          /* change Tansfer file */
#define PEP_MF_CONF         0x00000200          /* change configuration*/
#define PEP_MF_OPEN         0x00000400          /* open file */
#define PEP_MF_CSTR         0x00000800          /* change C.String file*/
#define PEP_MF_RPT          0x00001000          /* change Prog. Report file*/
#define PEP_MF_PPI          0x00002000          /* change PPI file*/
#define PEP_MF_MLD          0x00004000          /* change MLD file*//* V3.3 */
#define PEP_MF_LAYTOUCH		0x00008000			/* change Touch Layout File */ //ESMITH LAYOUT
#define PEP_MF_LAYKEYBRD	0x00010000			/* change Keyboard Layout File */
#define PEP_MF_LOGO			0x00020000			/* change Logo File JHALL */
#define PEP_MF_ICON			0x00040000			/* change Icon Directory JHALL */


/*  short   nPepChkFlag;    checking flag for file control
/*    11111111:11111111:11111111:11111111
*                     | |||||||| |||||||+->     modify setup parameter file
*                     | |||||||| ||||||+-->     modify maintenance parameter file
*                     | |||||||| |||||+--->     modify dept. file
*					  | |||||||| ||||+---->     modify plu file
*					  |	|||||||| |||+----->     modify server file
*					  | |||||||| ||+------>     modify cashier file
*					  | |||||||| |+------->     modify ETK file
*					  | |||||||| +-------->     modify Combination Coupon file
*					  |	|||||||+---------->     modify transfer file
*					  |	||||||+----------->     configulation file flag
*					  |	|||||+------------>     open file flag
*					  |	||||+------------->     modify Control String file
*					  |	|||+-------------->     modify Prog. Report file
*					  |	||+--------------->     modify PPI file
*					  |	|+---------------->     modify MLD file
*					  |	+----------------->     modify Touch Layout File
*					  +------------------->     modify keyboard layout file
*/
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
extern  ULONG   nPepChkFlag;    /* checking flag for file control       */ //ESMITH LAYOUT

/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/
//void    PEPENTRY PepSetModFlag(HWND, UINT, UINT);
void    PEPENTRY PepSetModFlag(HWND, ULONG, UINT);
void    PEPENTRY PepResetModFlag(UINT, UINT);
UINT    PEPENTRY PepQueryModFlag(UINT, UINT);

#endif
/* ===== End of File ===== */
