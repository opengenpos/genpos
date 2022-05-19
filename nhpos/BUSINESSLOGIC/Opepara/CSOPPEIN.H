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
SHORT  Op_NewCreatPluFile(USHORT usNumberOfPlu);
SHORT  Op_CompressPluFile(USHORT usNewNumberOfPlu);
SHORT  Op_ExpandPluFile(USHORT usNumberOfPlu);
VOID   Op_CreatPluFileHeader(OPPLU_FILEHED *PluFileHed, USHORT usNumberOfPlu);
SHORT  Op_ExpandPluFile(USHORT usNumberOfPlu);
VOID   Op_MoveTable(USHORT offusSrcTable, USHORT offusObjTable, USHORT usMoveByte);
SHORT  Op_CompressPluFile(USHORT usNewNumberOfPlu);
SHORT  Op_CompressParaTable(USHORT usNewNumberOfPlu);
SHORT  Op_BlkCheck(OPPLU_FILEHED *pPluFileHed, USHORT usParaBlockNo);
SHORT  Op_CompParaBlock(USHORT *pusParaArg, USHORT *pusParaTbl);

/*
===========================================================================
    MEMORY
===========================================================================
*/


