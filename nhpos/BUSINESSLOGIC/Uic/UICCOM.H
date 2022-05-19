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
* Title       : UIC Common Header File for Master Terminal                        
* Category    : User Interface Control, NCR 2170 US Hospitality Model         
* Program Name: UICCOM.H                                                      
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
* --------------------------------------------------------------------------
* Abstruct:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data   : Ver.Rev. :   Name      : Description
* May-26-92 : 00.00.01 : H.Yamaguchi : initial                                   
* Apr-17-93 : 00.00.01 : H.Yamaguchi : Deleted UIC_RST????  defines.
* Apr-21-15 : 02.02.01 : R.Chambers  : deleted UIC_STACK define and usUicStack[UIC_STACK]
*          :        :           :                                    
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

/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/
#define UIC_IDLESTATE    0x0000         /* Terminal is idle state */
#define UIC_EXECSTATE    0x0001         /* Terminal is executing state */

#define UIC_FIXEDHND    60              /* fixed value for husUicHandle to handle zero seconds state.  See UicCheckAndSet() */

#define UIC_UADDMASTER   1              /* Master unique address */
#define UIC_UADDBMASTER  2              /* B-Master unique address */

/* #define UIC_RSTBACK      0x0001   */      /* Reset request backup Inf */
/* #define UIC_RSTPARA      0x0f     */      /* Reset request parameter Inf */
/* #define UIC_RSTSYSTEM    0x0003   */      /* Reset System Inf */

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Externel Define Declarations
*===========================================================================
*/
extern SHORT   husUicHandle;            /* Save a handle */
extern USHORT  fsUicExecuteFG;          /* Execute Request parameter and Backup copy */
extern USHORT  husUicFlagSem;           /* Control set/reset terminal  */
extern USHORT  husUicExecSem;           /* Control to start task       */
/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/*-------------------------------------------------------------------------
    UICMAIN.C
-------------------------------------------------------------------------*/

VOID   THREADENTRY UicMain(VOID);             /* Creates UIC Task  */
VOID   UicInitialize(VOID);                   /* Initialize UIC Task  */
SHORT  UicChkRequestJob(NBMESSAGE *pMes);     /* Checks request Job   */
SHORT  UicRBCHand(USHORT usMBack);            /* Executes Bcakup copy  */
SHORT  UicRPHand(UCHAR uchReq,USHORT usSys);  /* Executes Parameter request */
VOID   UicResetExecuteFG(VOID);               /* Reset fsUicExecuteFG */

// The following #if is used to enable or disable
// functionality to issues NHPOS_ASSERT log entries when
// the function UicDisplayErrorMsg () is called during NeighborhoodPOS
// execution.  This functionality provides a simple way to turn on the
// ability to have Assert logs created by calls to UicDisplayErrorMsg()
// that provide the file and line number in the file from whence the
// the function was called.  This can provide additional error debugging.
#if 0
// generate a warning so this place is easy to find from a compiler warning.
#pragma message(" \z")
#pragma message("  ====++++====   ----------------------------------    ====++++====")
#pragma message("  ====++++====   UicDisplayErrorMsg_Debug() is ENABLED     ====++++====")
#pragma message("  ====++++====   File: Include\\csstbbak.h              ====++++====")
#define UicDisplayErrorMsg(sError) UicDisplayErrorMsg_Debug(sError, __FILE__, __LINE__)
VOID   UicDisplayErrorMsg_Debug(SHORT sError, char *aszFilePath, int nLineNo);               /* Convert error code  */
#else
VOID   UicDisplayErrorMsg(SHORT sError);
#endif

/*========== END OF DEFINITION ============*/


