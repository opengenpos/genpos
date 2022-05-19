/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 1.0              ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Electronic Journal module, Header file for user (Non-Thermal System)                       
*   Category           : E.J. module                                                 
*   Program Name       : EJ_NT.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*   Date                 Ver.Rev    :NAME         :Description
*   Feb-16-93          : 00.00.01   :K.You        :Initial
*
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
#define        EJ_THERMAL_EXIST     0
#define        EJ_THERMAL_NOT_EXIST 1
#define        EJ_PERFORM           0
#define        EJ_FILE_NOT_EXIST   -101

/*
==================================================
   PROTOTYPE
==================================================
*/
SHORT   EJCreat(VOID);
SHORT   EJCheckC(VOID);
SHORT   EJCheckD(VOID);
SHORT   EJWrite(VOID);
SHORT   EJRead(VOID);
SHORT   EJReverse(VOID);
SHORT   EJClear(VOID);
UCHAR   EJGetFlag(VOID);
USHORT  EJConvertError(VOID);
USHORT  EJGetStatus(VOID);
SHORT   EJInit(VOID);
SHORT   EJReadFile(VOID);
SHORT   EJNoOfTrans(VOID);
SHORT   EJReadResetTransOver(VOID);
SHORT   EJReadResetTransStart(VOID);
SHORT   EJReadResetTrans(VOID);
SHORT   EJReadResetTransEnd(VOID);
USHORT  EJReadTrans(VOID);
SHORT   EJCheckSpace(VOID);
SHORT   EJLock( VOID );
SHORT   EJUnLock( VOID );


