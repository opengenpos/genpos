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
*   Title              : Client/Server OPERATIONAL PARAMATER , Header file for internal
*   Category           : Client/Server OPERATIONAL PARAMATER  module, HOSPITAL FOR US
*   Program Name       : CSOPPEIN.H
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*    Update Histories  :
*
*   Date                 Ver.Rev    :NAME         :Description
*   May-06-92          : 00.00.01   :M.YAMAMOTO   :Initial
*   Mar-08-95          : 03.00.00   :H.Ishida     :R3.0
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
   DEFINE FOR ERROR 
===========================================================================
*/

/*
===========================================================================
   DEFINE FOR ERROR CODE
===========================================================================
*/

/*
===========================================================================
   DEFINE FOR OTHER
===========================================================================
*/
/*
===========================================================================
    STRUCT
===========================================================================
*/
/*
===========================================================================
   PROTOTYPE
===========================================================================
*/
SHORT  Op_NewPluCreatFile(USHORT usNumberOfPlu);
SHORT  Op_CompressPluFile(USHORT usNewNumberOfPlu);
SHORT  Op_ExpandPluFile(USHORT usNumberOfPlu);
SHORT  Op_NewCstrCreatFile(USHORT usNumberOfCstr);
SHORT  Op_CompressCstrFile(USHORT usNewNumberOfCstr);
SHORT  Op_ExpandCstrFile(USHORT usNumberOfCstr);
VOID   Op_CreatPluFileHeader(OPPLU_FILEHED FAR *PluFileHed, 
                             USHORT        usNumberOfPlu);
SHORT  Op_ExpandPluFile(USHORT usNumberOfPlu);
VOID   Op_MoveTableCompress(ULONG offulSrcTable, 
                            ULONG offulObjTable, 
                            ULONG ulMoveByte);
VOID   Op_MoveTableExpand(ULONG offulSrcTable, 
                          ULONG offulObjTable, 
                          ULONG ulMoveByte);
SHORT  Op_CompressPluFile(USHORT usNewNumberOfPlu);
SHORT  Op_CompressParaTable(USHORT usNewNumberOfPlu);
SHORT  Op_BlkCheck(OPPLU_FILEHED FAR *pPluFileHed, 
                   USHORT            usParaBlockNo);
SHORT  Op_CompParaBlock(VOID FAR *pusParaArg, 
                        VOID FAR *pusParaTbl);

SHORT   Op_CopyFile(OPPLU_FILEHED     *pPluFileHedNew,
                    OPPLU_FILEHED_THO *pPluFileHedOld);

SHORT   Op_Copy25File(OPPLU_FILEHED     *pPluFileHedNew,
                      OPPLU_FILEHED_R25 *pPluFileHedOld);

SHORT   Op_Copy30File(OPPLU_FILEHED     *pPluFileHedNew,
                      OPPLU_FILEHED     *pPluFileHedOld);

VOID   Op_WriteNewPlu(ULONG  offulFileSeek, 
                      VOID   *pTableHeadAddress, 
                      USHORT usSizeofWrite);
VOID   Op_CreateEmptyTbl(OPPLU_FILEHED  *pPluFileHedNew);
SHORT  Op_CopyFileBlock(ULONG  ulSouOff, ULONG  ulDesOff, ULONG  ulSize);
VOID   Op_ChgFileBlock(ULONG  ulSouOff, ULONG  ulDesOff,
                       USHORT usSouLen, USHORT usDesLen,
                       USHORT usSouFirstLen, USHORT usDesFirstLen,
                       USHORT usSize);

/*** NCR2172 (nwoppein.c) ***/
SHORT   Op_NewDeptCreatFile(USHORT usNumberOfDept);
SHORT   Op_ExpandDeptFile(USHORT usNumberOfDept);
SHORT   Op_CompressDeptFile(USHORT usNewNumberOfDept);
VOID    Op_CreatDeptFileHeader(OPDEPT_FILEHED FAR *pDeptFileHed, 
                              USHORT usNumberOfDept);
/*
===========================================================================
    MEMORY
===========================================================================
*/


/*==================== end of csoppein.h ================*/