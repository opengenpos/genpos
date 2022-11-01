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
*   Title              : Client/Server CASHIER module, Header file for internal                        
*   Category           : Client/Server CASHIER module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSCASPE.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*    Date                Ver.Rev    :NAME         :Description
*    May-06-92         : 00.00.01   :M.YAMAMOTO   :Initial
*    Apr-10-93         : 00.00.01   :H.YAMAGUCHI  :Modified
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

#define    CAS_MAX_WORK_BUFFER       1000
#define    CAS_PARAENTRY_NUM           30       /* sizeof(CAS_PARAENTRY)  */
#define    CAS_INDEXENTRY_NUM           4       /* sizeof(CAS_INDEXENTRY) */
#define    CAS_OFFSET_TRAINING          0       /* OFFSET PARAENTRY of Training */
#define    CAS_OLDTRAINING_BIT       0x01       /* R3.1 Training Bit      */
#define    CAS_NEWTRAINING_BIT       0x08       /* V3.3 Training Bit      */
/*
===========================================================================
    TYPEDEF
===========================================================================
*/


/*
===========================================================================
    PROTOTYPE
===========================================================================
*/

/*----------------------
    Intarnal Prototype
----------------------*/

SHORT   Cas_NewCashierCreatFile(USHORT usNumberOfCashier);
SHORT   Cas_ExpandCashierFile(USHORT usNumberOfCashier);
SHORT   Cas_CompressCashierFile(USHORT usNewNumberOfCashier);
VOID    Cas_CreatCashierFileHeader(CAS_FILEHED FAR *pCasFileHed, USHORT usNumberOfCashier);
SHORT   Cas_CompressParaTable(USHORT usNewNumberOfCashier);
VOID    Cas_MoveTableExpand(USHORT offusSrcTable, USHORT offusObjTable, USHORT usMoveByte);
VOID    Cas_MoveTableCompress(USHORT offusSrcTable, USHORT offusObjTable, USHORT usMoveByte);
SHORT   Cas_MigrCopyTable(USHORT offusSrcTable, USHORT offusObjTable,USHORT usSize);
VOID    Cas_MigrNewWrite(ULONG offulFileSeek, VOID  *pTableHeadAddress, ULONG usSizeofWrite);
SHORT   Cas_ChangeIndexTable(USHORT offusSrcTable, USHORT offusObjTable, USHORT usSize);
SHORT   Cas_ChangeParaTable(USHORT offusSrcTable, USHORT offusObjTable, USHORT usSize);
/*
===========================================================================
    MEMORY
===========================================================================
*/

/*========= End of Define =========*/
