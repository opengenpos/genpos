/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Maint Module                         
* Category    : Maintenance, NCR 2170 Application Program        
* Program Name: MAINTRAM.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct: This function 
*                 
*           The provided function names are as follows:  
*
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev   :   Name    : Description
* May-04-92: 00.00.01  : J.Ikeda   : initial                                   
* Jun-29-93: 01.00.12  : J.IKEDA   : Add uchMaintTermNo                                   
* Oct-05-93: 02.00.01  : J.IKEDA   : Add uchMaintEtkCtl  usMaintFiledNo
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*=========================================================================== 
*/
/**
==============================================================================
;                  R A M   A R E A   D E C L A R A T I O Ns                         
=============================================================================
**/

extern MAINTWORK MaintWork;

/*------------------------------------------------------------------------*\

            A R G U M E N T   D E C L A R A T I O N s

\*------------------------------------------------------------------------*/

extern UCHAR   uchMaintOpeCo;               /* operational counter */
extern UCHAR   uchMaintMenuPage;            /* Menu Shift Page */
extern UCHAR   uchMaintTermNo;              /* terminal number with shared printer */

extern ULONG   ulMaintSupNumber;            /* supervisor number */
extern USHORT  usMaintSupNumberInternal;    /* supervisor internal number mapped to Supervisor Number in range of 100 to 899 */
extern USHORT  usMaintSupLevel;	            /* supervisor level */
extern USHORT  usMaintLockHnd;              /* Lock Handle */

extern UCHAR   uchMaintTblOff;              /* tax table offset */
extern UCHAR   uchMaintTaxCtl;              /* control flag */      
extern USHORT  usMaintTbl1Len;              /* tax table #1 length */
extern USHORT  usMaintTbl2Len;              /* tax table #2 length */
extern USHORT  usMaintTbl3Len;              /* tax table #3 length */
extern USHORT  usMaintTbl4Len;              /* tax table #4 length */

extern UCHAR   uchMaintEtkCtl;              /* for ETK File Maintenance */
extern USHORT  usMaintFieldNo;              /* for ETK File Maintenance */
extern USHORT  usMaintACNumber;             /* action code save area,   Saratoga */

/* --- End of Header File --- */