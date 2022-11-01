/*
* ---------------------------------------------------------------------------
* Title         :   2170 PEP File Size Calculation Header File
* Category      :   NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   PEPCALC.H
* Copyright (C) :   1993, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /c /d /G2sw /Od /W4 /Zpi
* ---------------------------------------------------------------------------
* Abstract :
*
*
* ---------------------------------------------------------------------------
* Update Histories
*
*   Date    : Ver.Rev. :   Name     : Description
* Apr-27-94 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Apr-06-95 : 03.00.00 : H.Ishida   : Add R3.0
* Aug-23-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Sep-13-95 : 01.00.03 : T.Nakahata : correct calcuration (GC, Item, Cons)
* Oct-20-95 : 01.00.04 : T.Nakahata : increase GC file size
* Apl-10-96 : 03.00.05 : M.Suzuki   : ADD R3.1
* Oct-16-98 : 03.03.00 : A.Mitsui   : ADD V3.3 and Bugfix
* Oct-23-98 : 03.03.01 : A.Mitsui   : Bugfix
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
*       Define Declarations
* ===========================================================================
*/
#ifndef PEPCALC_INCLUDED_DEFINED
#define PEPCALC_INCLUDED_DEFINED


/*
* ===========================================================================
*       Value definition
* ===========================================================================
*/
                                        /* 1 block = 512 bytes (file), 1 block = 256 bytes (index file) */
                                        /* add indextbl block(1block size) when over 6 block */
#define PEP_CALC_FILE_BASETTL      20   /* base total file blocks = financial file + hourly file = 1996 + 8132 = 10128 bytes */ /* V3.3 6->20 */
                                        /* financial file (sizeof(TTLCSREGFIN)) * ((Daily, PTD)*(Current, Saved)) = 499*(2*2) = 1996 */
                                        /* hourly file    (sizeof(TTLCSHOURLY)) * ((Daily, PTD)*(Current, Saved)) = 2033*(2*2) = 8132 */
#define PEP_CALC_FILE_TTLUD         8   /* total/update file(4096bytes) blocks  */
#define PEP_CALC_FILE_SHARBUF      10   /* shared buffer block (5K)             */
#define PEP_CALC_FILE_POSTREC       9   /* post receipt block               */
#define PEP_CALC_FILE_IDX           2   /* inform. block of directory, index file */
#define PEP_CALC_FILE_IDXBLK        1   /* number of block for index file   */
#define PEP_CALC_FILE_SERVTIMETTL   7   /* service time total file blocks(sizeof(TTLCSSERTIME)*4=3524bytes)*/ /* V3.3 4->7 */
#define PEP_CALC_FILE_MLDBLK       14   /* MLD file blocks (PEPCALCMLDPARA(200) = 6402bytes) */

/* calculate for department file */
/*** NCR2172 ***/
/* #define PEP_CALC_DEPT_PARA_MLT     20L  sizeof(DEPTPARA) = 1+5+1+1+12      */
/* #define PEP_CALC_DEPT_PARA_ADD      2L  sizeof(OPDEPT_FILEHED) = 2       */
#define PEP_CALC_DEPT_PARA_MLT    32L  /* sizeof(OPDEPT_INDEXENTRY)+OP_DEPT_INDEXBLK_SIZE */
                                        /*     +sizeof(OPDEPT_PARAENTRY) = (2+2+28) */
                                        /* +sizeof(OPPLU_INDEXENTRYBYGROUPNO) = (3+2+3+3+27+3)     */
#define PEP_CALC_DEPT_PARA_ADD     32L  /* sizeof(OPDEPT_FILEHED)+OPDEPT_PARA_EMPTY_TABLE_SIZE = (28+32) */
#define PEP_CALC_DEPT_TTL_MLT      20L  /* (sizeof(TTLCSDEPTINDX)+sizeof(USHORT)+sizeof(TOTAL))*2 = (2+2+6)*2 */
#define PEP_CALC_DEPT_TTL_ADD     208L  /* sizeof(TTLCSDEPTHEAD)+(sizeof(TTLCSDEPTMISC)                   */
                                        /* +OPDEPT_PARA_EMPTY_TABLE_SIZE)*2 = 94+(25+32)*2 */
#define PEP_CALC_DEPT_TTL_MLT_PTD  20L  /* (sizeof(TTLCSDEPTINDX)+sizeof(USHORT)+sizeof(TOTAL))*2 = (2+2+6)*2 */
#define PEP_CALC_DEPT_TTL_ADD_PTD  114L /* (sizeof(TTLCSDEPTMISC)+OPDEPT_PARA_EMPTY_TABLE_SIZE)*2 = (25+32)*2 */

/* calculate for PLU regular file, saratoga */
#define PEP_CALC_PLU_PARA_MLT      269L  /* sizeof(RECPLU)[record]+sizeof(RECCNT)[system(hash)] = 264+5 */
#define PEP_CALC_PLU_PARA_ADD       72L  /* sizeof(FILEHDR) = 72                 */
/* calculate for PLU index table */
#define PEP_CALC_BIT                 8   /* index table size (1 byte)            */

/* calculate for PLU index file, saratoga */
#define PEP_CALC_PLU_IDX_MLT        29L  /* sizeof(RECIDX) = 29                   */
#define PEP_CALC_PLU_IDX_ADD        72L  /* sizeof(FILEHDR) = 72                 */

/* calculate for OEP file, saratoga */
#define PEP_CALC_OEP_PARA_MLT       37L  /* sizeof(OPOEP_INDEXENTRY) */
#define PEP_CALC_OEP_PARA_ADD       18L  /* sizeof(OPOEP_FILEHED) */
#define PEP_CALC_OEP_MAX          8000L
#define PEP_CALC_OEP_FIXED_SIZE 120012L  /* sizeof(OPOEP_INDEXENTRY)*PEP_CALC_OEP_MAX + sizeof(OPOEP_FILEHED) = 14*8000+12 */

/* calculate for PLU file */
/* #define PEP_CALC_PLU_PARA_MLT      41L  / sizeof(OPPLU_INDEXENTRY)+OP_PLU_INDEXBLK_SIZE+sizeof(OPPLU_INDEXENTRYBYREPORT) */
                                        /*     +sizeof(OPPLU_INDEXENTRYBYDEPT)+sizeof(OPPLU_PARAENTRY) */
                                        /* +sizeof(OPPLU_INDEXENTRYBYGROUPNO) = (3+2+3+3+27+3)     */
/* #define PEP_CALC_PLU_PARA_ADD     415L  /* sizeof(OPPLU_FILEHED)+OPPLU_PARA_EMPTY_TABLE_SIZE = (40+375) */
#define PEP_CALC_PLU_TTL_MLT       84L  /* (sizeof(TTLCSPLUINDX)+sizeof(USHORT)+sizeof(TOTAL))*2 = (3+2+6)*2 */
#define PEP_CALC_PLU_TTL_ADD        0L  /* sizeof(TTLCSPLUHEAD)+(sizeof(TTLCSPLUMISC)                   */
                                        /* +OPPLU_PARA_EMPTY_TABLE_SIZE)*2 = 94+(25+375)*2 */
#define PEP_CALC_PLU_TTL_MLT_PTD   84L  /* (sizeof(TTLCSPLUINDX)+sizeof(USHORT)+sizeof(TOTAL))*2 = (3+2+6)*2 */
#define PEP_CALC_PLU_TTL_ADD_PTD    0L  /* (sizeof(TTLCSPLUMISC)+OPPLU_PARA_EMPTY_TABLE_SIZE)*2 = (25+375)*2 */

/**
#define PEP_CALC_PLU_TTL_MLT       22L  / (sizeof(TTLCSPLUINDX)+sizeof(USHORT)+sizeof(TOTAL))*2 = (3+2+6)*2 /
#define PEP_CALC_PLU_TTL_ADD      894L  / sizeof(TTLCSPLUHEAD)+(sizeof(TTLCSPLUMISC)                   /
                                        / +OPPLU_PARA_EMPTY_TABLE_SIZE)*2 = 94+(25+375)*2 /
#define PEP_CALC_PLU_TTL_MLT_PTD   22L  / (sizeof(TTLCSPLUINDX)+sizeof(USHORT)+sizeof(TOTAL))*2 = (3+2+6)*2 /
#define PEP_CALC_PLU_TTL_ADD_PTD  800L  / (sizeof(TTLCSPLUMISC)+OPPLU_PARA_EMPTY_TABLE_SIZE)*2 = (25+375)*2 /
**/
/* calculate for server file (not used)*/
#define PEP_CALC_SER_PARA_MLT      19L  /* sizeof(WAI_INDEXENTRY)+sizeof(WAI_PARAENTRY) = (3+16) */
#define PEP_CALC_SER_PARA_ADD      25L  /* sizeof(WAI_FILEHED)+WAI_PARA_EMPTY_TABLE_SIZE = (12+13) */
#define PEP_CALC_SER_TTL_MLT      528L  /* (sizeof(TTLCSWAIINDEX)+sizeof(TTLCSWAITOTAL))*2 = (3+261)*2 */
#define PEP_CALC_SER_TTL_ADD       88L  /* sizeof(TTLCSWAIHEAD)+TTL_WAIEMP_SIZE*2 = 62+13*2 */
#define PEP_CALC_SER_TTL_MLT_PTD  528L  /* (sizeof(TTLCSWAIINDEX)+sizeof(TTLCSWAITOTAL))*2 = (3+261)*2 */
#define PEP_CALC_SER_TTL_ADD_PTD   26L  /* TTL_WAIEMP_SIZE*2 = 13*2 */

/* calculate for operator file , V3.3 */
#define PEP_CALC_CASH_PARA_MLT     34L  /* sizeof(CAS_INDEXENTRY)+sizeof(CAS_PARAENTRY) = (4+30) */
#define PEP_CALC_CASH_PARA_ADD     52L  /* sizeof(CAS_FILEHED)+CAS_PARA_EMPTY_TABLE_SIZE = (14+38) */
#define PEP_CALC_CASH_TTL_MLT     1046L  /* (sizeof(TTLCSCASINDX)+sizeof(TTLCSCASTOTAL))*2 = (4+495)*2, saratoga */
#define PEP_CALC_CASH_TTL_ADD     138L  /* sizeof(TTLCSCASHEAD)+CAS_PARA_EMPTY_TABLE_SIZE*2 = (62+38*2) */
#define PEP_CALC_CASH_TTL_MLT_PTD 1044L  /* (sizeof(TTLCSCASINDX)+sizeof(TTLCSCASTOTAL))*2 = (3+495)*2, saratoga */
#define PEP_CALC_CASH_TTL_ADD_PTD  76L  /* CAS_PARA_EMPTY_TABLE_SIZE*2 = 38*2 */

/* calculate for electronic journal file */
#define PEP_CALC_EJ_TTL_MLT       512L  /* EJ_BLOCKSIZE = 512 */
#define PEP_CALC_EJ_TTL_ADD         0L  /*                    */

/* calculate for employee time keeping file */
#define PEP_CALC_ETK_PARA_MLT      23L  /* sizeof(ETK_INDEX) = 23 */
#define PEP_CALC_ETK_PARA_ADD       6L  /* sizeof(ETK_FILEHEAD) = 6 */
#define PEP_CALC_ETK_TTL_MLT      404L  /* (sizeof(ETK_TOTALINDEX)+(sizeof(ETK_FILED_IN_FILE)*ETK_NUMBER_OF_FILED))*2 = (7+(13*15))*2 */
#define PEP_CALC_ETK_TTL_ADD      115L  /* sizeof(ETK_TOTALHEAD)+ETK_PARA_EMPTY_TABLE_SIZE*2 = 51+32*2 */

/* calculate for guest check file (pre),  R3.0 FVT */
#define PEP_CALC_GC_TTL_MLT_PRE  1807L  /* sizeof(GCF_INDEXFILE)+GCF_DATA_BLOCK_SIZE = (7+1800), saratoga */
#define PEP_CALC_GC_TTL_ADD_PRE    54L  /* sizeof(GCF_FILEHED) = 54             */

/* calculate for guest check file (post)  R3.0 FVT */
#define PEP_CALC_GC_TTL_MLT_POST 1811L  /* sizeof(GCF_INDEXFILE)+GCF_DATA_BLOCK_SIZE*4 = (7+1800*4), saratoga */
#define PEP_CALC_GC_TTL_ADD_POST   54L  /* sizeof(GCF_FILEHED) = 54             */

/* calculate for guest check file (store)  R3.0 FVT */
#define PEP_CALC_GC_TTL_MLT_STR  7207L  /* sizeof(GCF_INDEXFILE)+sizeof(UCHAR)*5+GCF_DATA_BLOCK_SIZE*4 = (7+1*5+1800*4), saratoga */
#define PEP_CALC_GC_TTL_ADD_STR    54L  /* sizeof(GCF_FILEHED) = 54             */
/* #### */

/* calculate for Item Storage file (store) */
#define PEP_CALC_ITEM_ADD  255L                                                      /* V3.3 228->255 */
    /* ...sizeof(TRANITEMSTORAGEHEADER) + TRN_STOR_BASE_BYTES - 1 = 96 + 160 -1 */
    /*  = header(4) + VLI(2) + sizeof(TRANMODEQUAL) + sizeof(TRANCURQUAL) + TRN_STOR_BASE_BYTES(160) - 1 */
#define PEP_CALC_CONS_ADD  725L                                                      /* V3.3 604->649 */
    /* ...sizeof(TRANCONSSTORAGEHEADER) + TRN_STOR_BASE_BYTES - 1 = 566 + 160 -1, saratoga */
    /*  = header(4) + VLI(2) + sizeof(TRANITEMIZERS) + sizeof(TRANGCFQUAL) + TRN_STOR_BASE_BYTES(160) - 1 */
/* #### */

/* calculate for print priority index for item/cons file */
#define PEP_CALC_PRT_MLT    29L     /* sizeof(PRTIDX), saratoga */
#define PEP_CALC_PRT_ADD   590L     /* sizeof(PRTIDXHDR)+sizeof(PRTIDX)*20, saratoga */

/* calculate for Combination Coupon file */
/*#define PEP_CALC_CPN_PARA_MLT      39L   sizeof(COMCOUPONPARA) = 2+4+12+14+7  */
#define PEP_CALC_CPN_PARA_MLT     116L  /* sizeof(COMCOUPONPARA) NCR2172 */
#define PEP_CALC_CPN_PARA_ADD       2L  /* sizeof(OPCPN_FILEHED) = 2            */
#define PEP_CALC_CPN_TTL_MLT       12L  /* sizeof(TOTAL)*2 = 6*2                */
#define PEP_CALC_CPN_TTL_ADD       46L  /* */
#define PEP_CALC_CPN_TTL_MLT_PTD   12L  /* sizeof(TOTAL)*2 = 6*2                */
#define PEP_CALC_CPN_TTL_ADD_PTD   34L  /* */

/* calculate for Control String file */
#define PEP_CALC_CSTR_PARA_MLT      2L  /* sizeof(OPCSTR_PARAENTRY) = 2         */
#define PEP_CALC_CSTR_PARA_ADD    3204L  /* sizeof(OPCSTR_FILEHEAD) + sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE= 4+4*400  ESMITH*/ 
//#define PEP_CALC_CSTR_PARA_ADD    804L  /* sizeof(OPCSTR_FILEHEAD) + sizeof(OPCSTR_INDEXENTRY) * OP_CSTR_INDEX_SIZE= 4+4*200 */ 

/* calculate for programmable report file */
#define PEP_CALC_PRGRPT_TTL_MLT  1024L  /* PRGRPT_BLOCKSIZE = 1024 */
#define PEP_CALC_PRGRPT_TTL_ADD     0L  /*                    */

/* calculate for PPI file */
#define PEP_CALC_PPI_PARA_MLT     100L  /* sizeof(OPPPI_PARAENTRY) = 100       */
#define PEP_CALC_PPI_PARA_ADD       2L  /* sizeof(OPPPI_FILEHED) = 2           */ 

/* calculate for Ind Financial file */
#define PEP_CALC_IND_FIN_TTL_MLT 1204L  /* (sizeof(TTLCSINDINDX)+sizeof(TTLCSREGFIN))*2 = (3+547)*2 ,saratoga */
#define PEP_CALC_IND_FIN_TTL_ADD   24L  /* TTL_INDHEAD_SIZE+TTL_INDEMP_SIZE*2 = 20+2*2 */

/* calculate for MLD file , V3.3 */
#define PEP_CALC_MLD_PARA_MLT      32L  /* sizeof(MLD_RECORD) = 32             */
#define PEP_CALC_MLD_PARA_ADD       2L  /* sizeof(OPMLD_FILEHED) = 2           */

/*
* ===========================================================================
*       Macro definition
* ===========================================================================
*/
/*** NCR2172 
#define PEPCALCDEPTPARA(x)   (DWORD)((DWORD)(PEP_CALC_DEPT_PARA_MLT * (DWORD)(x)) + PEP_CALC_DEPT_PARA_ADD)
#define PEPCALCDEPTTTL(x, y) (DWORD)(((DWORD)(PEP_CALC_DEPT_TTL_MLT * (DWORD)(x)) + PEP_CALC_DEPT_TTL_ADD) + (((DWORD)(PEP_CALC_DEPT_TTL_MLT_PTD * (DWORD)(x)) + PEP_CALC_DEPT_TTL_ADD_PTD) * (DWORD)(y)))
***/
#define PEPCALCDEPTPARA(x)    (DWORD)((DWORD)(PEP_CALC_DEPT_PARA_MLT * (DWORD)(x)) + PEP_CALC_DEPT_PARA_ADD + (DWORD)(x))  /* V3.3 BugFix */
#define PEPCALCDEPTTTL(x, y)  (DWORD)(((DWORD)(PEP_CALC_DEPT_TTL_MLT * (DWORD)(x)) + PEP_CALC_DEPT_TTL_ADD) + (((DWORD)(PEP_CALC_DEPT_TTL_MLT_PTD * (DWORD)(x)) + PEP_CALC_DEPT_TTL_ADD_PTD) * (DWORD)(y)))

#define PEPCALCINDEXTBL(x)   (DWORD)((DWORD)(((DWORD)(x) / PEP_CALC_BIT) + (DWORD)(((DWORD)(x) % PEP_CALC_BIT) ? 1 : 0)))
#define PEPCALCPLUPARA(x)    (DWORD)(((DWORD)(PEP_CALC_PLU_PARA_MLT * (DWORD)(x)) + PEP_CALC_PLU_PARA_ADD) + PEPCALCINDEXTBL(x))
#define PEPCALCPLUIDX(x)     (DWORD)((DWORD)(PEP_CALC_PLU_IDX_MLT * (DWORD)(x)) + PEP_CALC_PLU_IDX_ADD)
#define PEPCALCPLUTTL(x, y)  (DWORD)(((DWORD)(PEP_CALC_PLU_TTL_MLT * (DWORD)(x)) + PEP_CALC_PLU_TTL_ADD) + (((DWORD)(PEP_CALC_PLU_TTL_MLT_PTD * (DWORD)(x)) + PEP_CALC_PLU_TTL_ADD_PTD) * (DWORD)(y)))

#define PEPCALCOEPPARA(x)    (DWORD)((x > PEP_CALC_OEP_MAX) ? PEP_CALC_OEP_FIXED_SIZE : ((DWORD)(PEP_CALC_OEP_PARA_MLT * (DWORD)(x)) + PEP_CALC_OEP_PARA_ADD + (DWORD)(x)))

/****
#define PEPCALCPLUPARA(x)    (DWORD)((DWORD)(PEP_CALC_PLU_PARA_MLT * (DWORD)(x)) + PEP_CALC_PLU_PARA_ADD + (DWORD)(x))  /* V3.3 BugFix /
#define PEPCALCPLUTTL(x, y)  (DWORD)(((DWORD)(PEP_CALC_PLU_TTL_MLT * (DWORD)(x)) + PEP_CALC_PLU_TTL_ADD) + (((DWORD)(PEP_CALC_PLU_TTL_MLT_PTD * (DWORD)(x)) + PEP_CALC_PLU_TTL_ADD_PTD) * (DWORD)(y)))
***/
#define PEPCALCSERPARA(x)    (DWORD)((DWORD)(PEP_CALC_SER_PARA_MLT * (DWORD)(x)) + PEP_CALC_SER_PARA_ADD)
#define PEPCALCSERTTL(x, y)  (DWORD)(((DWORD)(PEP_CALC_SER_TTL_MLT * (DWORD)(x)) + PEP_CALC_SER_TTL_ADD) + (((DWORD)(PEP_CALC_SER_TTL_MLT_PTD * (DWORD)(x)) + PEP_CALC_SER_TTL_ADD_PTD) * (DWORD)(y)))
#define PEPCALCCASHPARA(x)   (DWORD)((DWORD)(PEP_CALC_CASH_PARA_MLT * (DWORD)(x)) + PEP_CALC_CASH_PARA_ADD)
#define PEPCALCCASHTTL(x, y) (DWORD)(((DWORD)(PEP_CALC_CASH_TTL_MLT * (DWORD)(x)) + PEP_CALC_CASH_TTL_ADD) + (((DWORD)(PEP_CALC_CASH_TTL_MLT_PTD * (DWORD)(x)) + PEP_CALC_CASH_TTL_ADD_PTD) * (DWORD)(y)))
#define PEPCALCETKPARA(x)    (DWORD)((DWORD)(PEP_CALC_ETK_PARA_MLT * (DWORD)(x)) + PEP_CALC_ETK_PARA_ADD)
#define PEPCALCETKTTL(x)     (DWORD)((DWORD)(PEP_CALC_ETK_TTL_MLT * (DWORD)(x)) + PEP_CALC_ETK_TTL_ADD)
#define PEPCALCEJTTL(x)      (DWORD)((DWORD)(PEP_CALC_EJ_TTL_MLT * (DWORD)(x)) + PEP_CALC_EJ_TTL_ADD)
#define PEPCALCCPNPARA(x)    (DWORD)((DWORD)(PEP_CALC_CPN_PARA_MLT * (DWORD)(x)) + PEP_CALC_CPN_PARA_ADD)
#define PEPCALCCPNTTL(x, y)  (DWORD)((DWORD)(PEP_CALC_CPN_TTL_MLT * (DWORD)(x) + PEP_CALC_CPN_TTL_ADD) + ((DWORD)(PEP_CALC_CPN_TTL_MLT_PTD * (DWORD)(x)) * (DWORD)(y)))
#define PEPCALCCSTRPARA(x)    (DWORD)((DWORD)(PEP_CALC_CSTR_PARA_MLT * (DWORD)(x)) + PEP_CALC_CSTR_PARA_ADD) //ESMITH SR160
#define PEPCALCPREGC(x)      (DWORD)((DWORD)(PEP_CALC_GC_TTL_MLT_PRE * (DWORD)(x)) + PEP_CALC_GC_TTL_ADD_PRE)
#define PEPCALCPOSTGC(x)     (DWORD)((DWORD)(PEP_CALC_GC_TTL_MLT_POST * (DWORD)(x)) + PEP_CALC_GC_TTL_ADD_POST)
#define PEPCALCSTRGC(x)      (DWORD)((DWORD)(PEP_CALC_GC_TTL_MLT_STR * (DWORD)(x)) + PEP_CALC_GC_TTL_ADD_STR)
#define PEPCALCRPTPARA(x)    (DWORD)((DWORD)(PEP_CALC_PRGRPT_TTL_MLT * (DWORD)(x)) + PEP_CALC_PRGRPT_TTL_ADD)
#define PEPCALCPPIPARA(x)    (DWORD)((DWORD)(PEP_CALC_PPI_PARA_MLT * (DWORD)(x)) + PEP_CALC_PPI_PARA_ADD)
#define PEPCALCMLDPARA(x)    (DWORD)((DWORD)(PEP_CALC_MLD_PARA_MLT * (DWORD)(x)) + PEP_CALC_MLD_PARA_ADD)   /* V3.3 */
#define PEPCALCINDTTL(x)     (DWORD)((DWORD)(PEP_CALC_IND_FIN_TTL_MLT * (DWORD)(x)) + PEP_CALC_IND_FIN_TTL_ADD)

#define PEPCALC512BUND(x)    (DWORD)((DWORD)(((x)/512 + 1 ) * 512))
#define PEPCALCPRTPRTY(x)    (DWORD)(((DWORD)(x) * PEP_CALC_PRT_MLT ) + PEP_CALC_PRT_ADD )
#define PEPCALCITEM(x)       (DWORD)((DWORD)(128 * (DWORD)(x)) + PEP_CALC_ITEM_ADD) /* 128 = TRN_1ITEM_BYTES */
#define PEPCALCCONS(x)       (DWORD)((DWORD)(128 * (DWORD)(x)) +\
                                     (DWORD)(PEPCALCPRTPRTY(x)) + PEP_CALC_CONS_ADD) /* 128 = TRN_1ITEM_BYTES */
#define PEPCALCITEMBOUND(x)  (DWORD)((DWORD)(PEPCALC512BUND(PEPCALCITEM(x))))
#define PEPCALCCONSBOUND(x)  (DWORD)((DWORD)(PEPCALC512BUND(PEPCALCCONS(x))))
#define PEPCALCRMTPTR(x)     (DWORD)((x) * 144)
#define PEPCALCSERV(x)       (DWORD)(PEPCALCCONSBOUND(x) + 512)
#define PEPCALCPOSTRCT(x)    (DWORD)(PEPCALCCONSBOUND(x) + 22) /* ConsSize + sizeof(TRNCURQUAL) */
#define PEPCALCTOTALUP(x)    (PEPCALCITEMBOUND(x)) 
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
/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/

#endif
/* ===== End of PEPCALC.H ===== */
