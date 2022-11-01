/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server STUB module, Header file for user                        
*   Category           : Client/Server STUB Electoric Journal interface                                                
*   Program Name       : PCSTBEJ.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Medium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*
*    Date       :Ver       :NAME         :Description
*    Feb-03-93  :00.00.01  :K.YOU        :Initial
*    Jun-07-93  :00.00.01  :H.Yamaguchi  :Adds function
*    Sep-07-98  :          :O.Nakada     :Add V3.3
*
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

/*
===========================================================================
    DEFINE
===========================================================================
*/

/*
===========================================================================
    TYPEDEF
===========================================================================
*/

/*
===========================================================================
    PROTOTYPE

        NOTE:  New functions added to this library
         may also need to have an entry point
		 added to the file NCR2172Stub.def in
		 order to make the entry point visible.

         You will know to do this if you see
		 link errors with your new function specifed
		 as not found.

===========================================================================
*/

/*---------------------------------------
    Support for EJ Function
----------------------------------------*/
SHORT   STUBENTRY SerEJClear(VOID);
SHORT   STUBENTRY SerEJReadFile(ULONG  ulOffset,  VOID   FAR  *pData, 
                                ULONG ulDataLen, ULONG FAR  *pulRetLen);
/* V3.3 */
SHORT   STUBENTRY   SerEJNoOfTrans(VOID);
SHORT   STUBENTRY   SerEJReadResetTransOver(ULONG FAR *pulOffset,
                            VOID FAR *pData, ULONG ulDataLen, ULONG FAR *pulRetLen);
SHORT   STUBENTRY   SerEJReadResetTransStart(ULONG FAR *pulOffset,
                            VOID FAR *pData, ULONG ulDataLen, ULONG FAR *pulRetLen,
                            USHORT FAR *pusNumTrans);
SHORT   STUBENTRY   SerEJReadResetTrans(ULONG FAR *pulOffset, VOID FAR *pData,
                            ULONG ulDataLen, ULONG FAR *pulRetLen, USHORT FAR *pusNumTrans);
SHORT   STUBENTRY   SerEJReadResetTransEnd(USHORT usNumTrans);
/* V3.3 */

/******* End od Source ****************************/
