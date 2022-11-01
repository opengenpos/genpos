/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993-1998      ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server OPERATIONAL PARAMATER , Header file for internal                        
*   Category           : Client/Server OPERATIONAL PARAMATER  module, HOSPITAL FOR US                                                 
*   Program Name       : CSOPv34.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Medium Model
*   Options            :  /c /AM /G1s /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*
*  ------------------------------------------------------------------------
*   Update Histories  :
*          
*   Date      :Ver.Rev :NAME       :Description
*
*   Jan-31-00 :        :K.Yannagida:Saratoga Initial
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

/*=======================================================================*\
:   define
\*=======================================================================*/

#define    OP_PLU_NAME_SIZE        20   /* Saratoga */
#define    OP_PLU_NAME_SIZE_V31    12   /* Saratoga */
#define    OP_PLU_PRICE_SIZE        3
#define    OP_PLU_CONT_OTHER       12  /* Send Remote Printer #5-8 (Saratoga) */
#if defined(POSSIBLE_DEAD_CODE)
#define    OP_PLU_CONT_OTHER_V31    5

#define    OP_PLU_NAME_SIZE_V34    12
#define    OP_PLU_CONT_OTHER_V34    6   /* Send Remote Printer #5-8(R3.1) */
#define    OP_PLU_PRICE_SIZE_V34    3

#define    TYPE_OPEN             0x01   /* PLU type definition */
#define    TYPE_NONADJ           0x02   /* PLU type definition */
#define    TYPE_ONEADJ           0x03   /* PLU type definition */

#define    OPEN_PLU              0x40   /* PLU Code Check (Open Price)  */
#define    NON_PLU               0x80   /* PLU Code Check (Non Adj.)    */
#define    ONE_PLU               0xC0   /* PLU Code Check (One Adj.)    */

#define    WORK_SIZE               64
#endif
/*=======================================================================*\
:   struct
\*=======================================================================*/

/*** V3.3/V3.4 Struct ***/

#pragma pack(push, 1)

typedef  struct {
    UCHAR   uchDept;
    UCHAR   uchRept;
    UCHAR   auchContOther[OP_PLU_CONT_OTHER_V34];
}PLU_CONTROL_V34;

typedef    struct {
    PLU_CONTROL_V34  ContPlu;
    UCHAR            auchPluName[OP_PLU_NAME_SIZE_V34];
    UCHAR            auchPrice[OP_PLU_PRICE_SIZE_V34];
    UCHAR            uchTableNumber;
    UCHAR            uchGroupNumber;
    UCHAR            uchPrintPriority;
    UCHAR            uchLinkPPI;         /* Link PPI Code,   Release 3.1 */
}OPPLU_PARAENTRY_V34;

typedef  struct {
   USHORT   usPluNo;
   UCHAR    uchAdjective;
} OPPLU_INDEXENTRY_V34;

typedef    struct {
    USHORT                 usPluNo;
    UCHAR                  uchPluAdj;
    ULONG                  ulCounter;
    OPPLU_PARAENTRY_V34    ParaPlu;
}PLUIF_V34;


		// This code is a candidate for removal with the next release of source code
/*** V3.1 Struct ***/

typedef  struct {
    UCHAR   uchDept;
    UCHAR   uchRept;
    UCHAR   auchContOther[OP_PLU_CONT_OTHER_V31];
}PLU_CONTROL_V31;

typedef    struct {
    PLU_CONTROL_OLD  ContPlu;
    UCHAR   auchPluName[OP_PLU_NAME_SIZE];
    UCHAR   auchPrice[OP_PLU_PRICE_SIZE];
    UCHAR   uchTableNumber;
    UCHAR   uchGroupNumber;
    UCHAR   uchPrintPriority;
}OPPLU_PARAENTRY_V31;

#endif

#pragma pack(pop)

/*=======================================================================*\
:   prototype
\*=======================================================================*/

#if defined(POSSIBLE_DEAD_CODE)
SHORT   OpPluAllRead_V34(PLUIF_V34*, USHORT);
SHORT   OpPluAssign_V34(PLUIF*, USHORT);
SHORT   Op_ReadPluFile_V34(ULONG, VOID*, USHORT);
VOID    Op_WritePluFile_V34(ULONG, VOID*, USHORT);
SHORT   Op_GetPlu_V34(OPPLU_FILEHED*, ULONG, PLUIF_V34*);
SHORT   Op_LockCheck_V34(USHORT);
#endif

#define GETDEPTNO(x)        ((unsigned char)((unsigned char)x & 0x3f))
#define GETPLUTYPE(x)       ((unsigned char)((unsigned char)x >> 6))
