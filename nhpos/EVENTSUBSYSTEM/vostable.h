/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Local Header File for Memory Manager 2
* Category    : Viode Output Service, 2170 System Application
* Program Name: VOSMEM2.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver 6.00A by Microsoft Corp.
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstract:     This file contains:
*                   1. Symbol Definitions
*                   2. Data type Definitions
*
* --------------------------------------------------------------------------
* Update Histories
*
* Data       Ver.      Name         Description
* Feb-23-95  G0        O.Nakada     Initial
*
** NCR2172 **
*
* Oct-05-99  01.00.00  M.Teraki     Added #pragma(...)
*
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

#if     (_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

/*
*===========================================================================
*   Define Declarations
*===========================================================================
*/
#define VOS_NUM_MEM2_RESERVED        2      /* number of reserved memory */


/*
*===========================================================================
*   Common Window Structure, #001 ~ #008
*===========================================================================
*/
typedef struct _VOSMEMORY_COMMON {
#if LCDWIN_LEN_COMMON001
    TCHAR   auchBuf001[LCDWIN_LEN_COMMON001];
#endif
#if LCDWIN_LEN_COMMON002
    TCHAR   auchBuf002[LCDWIN_LEN_COMMON002];
#endif
#if LCDWIN_LEN_COMMON003
    TCHAR   auchBuf003[LCDWIN_LEN_COMMON003];
#endif
#if LCDWIN_LEN_COMMON004
    TCHAR   auchBuf004[LCDWIN_LEN_COMMON004];
#endif
#if LCDWIN_LEN_COMMON005
    TCHAR   auchBuf005[LCDWIN_LEN_COMMON005];
#endif
#if LCDWIN_LEN_COMMON006
    TCHAR   auchBuf006[LCDWIN_LEN_COMMON006];
#endif
#if LCDWIN_LEN_COMMON007
    TCHAR   auchBuf007[LCDWIN_LEN_COMMON007];
#endif
#if LCDWIN_LEN_COMMON008
    TCHAR   auchBuf008[LCDWIN_LEN_COMMON008];
#endif
    TCHAR   auchReserved[VOS_NUM_MEM2_RESERVED];
} VOSMEMORY_COMMON;


/*
*===========================================================================
*   Lock Mode Window Structure, #050 ~ #057
*===========================================================================
*/
typedef struct _VOSMEMORY_LOCK {
#if LCDWIN_LEN_LOCK050
    TCHAR   auchBuf050[LCDWIN_LEN_LOCK050];
#endif
#if LCDWIN_LEN_LOCK051
    TCHAR   auchBuf051[LCDWIN_LEN_LOCK051];
#endif
#if LCDWIN_LEN_LOCK052
    TCHAR   auchBuf052[LCDWIN_LEN_LOCK052];
#endif
#if LCDWIN_LEN_LOCK053
    TCHAR   auchBuf053[LCDWIN_LEN_LOCK053];
#endif
#if LCDWIN_LEN_LOCK054
    TCHAR   auchBuf054[LCDWIN_LEN_LOCK054];
#endif
#if LCDWIN_LEN_LOCK055
    TCHAR   auchBuf055[LCDWIN_LEN_LOCK055];
#endif
#if LCDWIN_LEN_LOCK056
    TCHAR   auchBuf056[LCDWIN_LEN_LOCK056];
#endif
#if LCDWIN_LEN_LOCK057
    TCHAR   auchBuf057[LCDWIN_LEN_LOCK057];
#endif
    TCHAR   auchReserved[VOS_NUM_MEM2_RESERVED];
} VOSMEMORY_LOCK;


/*
*===========================================================================
*   Register Mode (2 division) Window Structure, #100 ~ #115
*===========================================================================
*/
typedef struct _VOSMEMORY_REG1 {
#if LCDWIN_LEN_REG100
    TCHAR   auchBuf100[LCDWIN_LEN_REG100];
#endif
#if LCDWIN_LEN_REG101
    TCHAR   auchBuf101[LCDWIN_LEN_REG101];
#endif
#if LCDWIN_LEN_REG102
    TCHAR   auchBuf102[LCDWIN_LEN_REG102];
#endif
#if LCDWIN_LEN_REG103
    TCHAR   auchBuf103[LCDWIN_LEN_REG103];
#endif
#if LCDWIN_LEN_REG104
    TCHAR   auchBuf104[LCDWIN_LEN_REG104];
#endif
#if LCDWIN_LEN_REG105
    TCHAR   auchBuf105[LCDWIN_LEN_REG105];
#endif
#if LCDWIN_LEN_REG106
    TCHAR   auchBuf106[LCDWIN_LEN_REG106];
#endif
#if LCDWIN_LEN_REG107
    TCHAR   auchBuf107[LCDWIN_LEN_REG107];
#endif
#if LCDWIN_LEN_REG108
    TCHAR   auchBuf108[LCDWIN_LEN_REG108];
#endif
#if LCDWIN_LEN_REG109
    TCHAR   auchBuf109[LCDWIN_LEN_REG109];
#endif
#if LCDWIN_LEN_REG110
    TCHAR   auchBuf110[LCDWIN_LEN_REG110];
#endif
#if LCDWIN_LEN_REG111
    TCHAR   auchBuf111[LCDWIN_LEN_REG111];
#endif
#if LCDWIN_LEN_REG112
    TCHAR   auchBuf112[LCDWIN_LEN_REG112];
#endif
#if LCDWIN_LEN_REG113
    TCHAR   auchBuf113[LCDWIN_LEN_REG113];
#endif
#if LCDWIN_LEN_REG114
    TCHAR   auchBuf114[LCDWIN_LEN_REG114];
#endif
#if LCDWIN_LEN_REG115
    TCHAR   auchBuf115[LCDWIN_LEN_REG115];
#endif
    TCHAR   auchReserved[VOS_NUM_MEM2_RESERVED];
} VOSMEMORY_REG1;


/*
*===========================================================================
*   Register Mode (3 division) Window Structure, #200 ~ #215
*===========================================================================
*/
typedef struct _VOSMEMORY_REG2 {
#if LCDWIN_LEN_REG200
    TCHAR   auchBuf200[LCDWIN_LEN_REG200];
#endif
#if LCDWIN_LEN_REG201
    TCHAR   auchBuf201[LCDWIN_LEN_REG201];
#endif
#if LCDWIN_LEN_REG202
    TCHAR   auchBuf202[LCDWIN_LEN_REG202];
#endif
#if LCDWIN_LEN_REG203
    TCHAR   auchBuf203[LCDWIN_LEN_REG203];
#endif
#if LCDWIN_LEN_REG204
    TCHAR   auchBuf204[LCDWIN_LEN_REG204];
#endif
#if LCDWIN_LEN_REG205
    TCHAR   auchBuf205[LCDWIN_LEN_REG205];
#endif
#if LCDWIN_LEN_REG206
    TCHAR   auchBuf206[LCDWIN_LEN_REG206];
#endif
#if LCDWIN_LEN_REG207
    TCHAR   auchBuf207[LCDWIN_LEN_REG207];
#endif
#if LCDWIN_LEN_REG208
    TCHAR   auchBuf208[LCDWIN_LEN_REG208];
#endif
#if LCDWIN_LEN_REG209
    TCHAR   auchBuf209[LCDWIN_LEN_REG209];
#endif
#if LCDWIN_LEN_REG210
    TCHAR   auchBuf210[LCDWIN_LEN_REG210];
#endif
#if LCDWIN_LEN_REG211
    TCHAR   auchBuf211[LCDWIN_LEN_REG211];
#endif
#if LCDWIN_LEN_REG212
    TCHAR   auchBuf212[LCDWIN_LEN_REG212];
#endif
#if LCDWIN_LEN_REG213
    TCHAR   auchBuf213[LCDWIN_LEN_REG213];
#endif
#if LCDWIN_LEN_REG214
    TCHAR   auchBuf214[LCDWIN_LEN_REG214];
#endif
#if LCDWIN_LEN_REG215
    TCHAR   auchBuf215[LCDWIN_LEN_REG215];
#endif
    TCHAR   auchReserved[VOS_NUM_MEM2_RESERVED];
} VOSMEMORY_REG2;


/*
*===========================================================================
*   Supervisor Mode Window Structure, #301 ~ #315
*===========================================================================
*/
typedef struct _VOSMEMORY_SUPER {
#if LCDWIN_LEN_SUPER300
    TCHAR   auchBuf300[LCDWIN_LEN_SUPER300];
#endif
#if LCDWIN_LEN_SUPER301
    TCHAR   auchBuf301[LCDWIN_LEN_SUPER301];
#endif
#if LCDWIN_LEN_SUPER302
    TCHAR   auchBuf302[LCDWIN_LEN_SUPER302];
#endif
#if LCDWIN_LEN_SUPER303
    TCHAR   auchBuf303[LCDWIN_LEN_SUPER303];
#endif
#if LCDWIN_LEN_SUPER304
    TCHAR   auchBuf304[LCDWIN_LEN_SUPER304];
#endif
#if LCDWIN_LEN_SUPER305
    TCHAR   auchBuf305[LCDWIN_LEN_SUPER305];
#endif
#if LCDWIN_LEN_SUPER306
    TCHAR   auchBuf306[LCDWIN_LEN_SUPER306];
#endif
#if LCDWIN_LEN_SUPER307
    TCHAR   auchBuf307[LCDWIN_LEN_SUPER307];
#endif
#if LCDWIN_LEN_SUPER308
    TCHAR   auchBuf308[LCDWIN_LEN_SUPER308];
#endif
#if LCDWIN_LEN_SUPER309
    TCHAR   auchBuf309[LCDWIN_LEN_SUPER309];
#endif
#if LCDWIN_LEN_SUPER310
    TCHAR   auchBuf310[LCDWIN_LEN_SUPER310];
#endif
#if LCDWIN_LEN_SUPER311
    TCHAR   auchBuf311[LCDWIN_LEN_SUPER311];
#endif
#if LCDWIN_LEN_SUPER312
    TCHAR   auchBuf312[LCDWIN_LEN_SUPER312];
#endif
#if LCDWIN_LEN_SUPER313
    TCHAR   auchBuf313[LCDWIN_LEN_SUPER313];
#endif
#if LCDWIN_LEN_SUPER314
    TCHAR   auchBuf314[LCDWIN_LEN_SUPER314];
#endif
#if LCDWIN_LEN_SUPER315
    TCHAR   auchBuf315[LCDWIN_LEN_SUPER315];
#endif
    TCHAR   auchReserved[VOS_NUM_MEM2_RESERVED];
} VOSMEMORY_SUPER;


/*
*===========================================================================
*   Program Mode Window Structure, #401 ~ #415
*===========================================================================
*/
typedef struct _VOSMEMORY_PROG {
#if LCDWIN_LEN_PROG400
    TCHAR   auchBuf400[LCDWIN_LEN_PROG400];
#endif
#if LCDWIN_LEN_PROG401
    TCHAR   auchBuf401[LCDWIN_LEN_PROG401];
#endif
#if LCDWIN_LEN_PROG402
    TCHAR   auchBuf402[LCDWIN_LEN_PROG402];
#endif
#if LCDWIN_LEN_PROG403
    TCHAR   auchBuf403[LCDWIN_LEN_PROG403];
#endif
#if LCDWIN_LEN_PROG404
    TCHAR   auchBuf404[LCDWIN_LEN_PROG404];
#endif
#if LCDWIN_LEN_PROG405
    TCHAR   auchBuf405[LCDWIN_LEN_PROG405];
#endif
#if LCDWIN_LEN_PROG406
    TCHAR   auchBuf406[LCDWIN_LEN_PROG406];
#endif
#if LCDWIN_LEN_PROG407
    TCHAR   auchBuf407[LCDWIN_LEN_PROG407];
#endif
#if LCDWIN_LEN_PROG408
    TCHAR   auchBuf408[LCDWIN_LEN_PROG408];
#endif
#if LCDWIN_LEN_PROG409
    TCHAR   auchBuf409[LCDWIN_LEN_PROG409];
#endif
#if LCDWIN_LEN_PROG410
    TCHAR   auchBuf410[LCDWIN_LEN_PROG410];
#endif
#if LCDWIN_LEN_PROG411
    TCHAR   auchBuf411[LCDWIN_LEN_PROG411];
#endif
#if LCDWIN_LEN_PROG412
    TCHAR   auchBuf412[LCDWIN_LEN_PROG412];
#endif
#if LCDWIN_LEN_PROG413
    TCHAR   auchBuf413[LCDWIN_LEN_PROG413];
#endif
#if LCDWIN_LEN_PROG414
    TCHAR   auchBuf414[LCDWIN_LEN_PROG414];
#endif
#if LCDWIN_LEN_PROG415
    TCHAR   auchBuf415[LCDWIN_LEN_PROG415];
#endif
    TCHAR   auchReserved[VOS_NUM_MEM2_RESERVED];
} VOSMEMORY_PROG;


/*
*===========================================================================
*   Window Structure
*===========================================================================
*/
typedef struct _VOSMEMORY {
    VOSMEMORY_COMMON    Common;             /* common mode          */
    union {
        VOSMEMORY_LOCK  Lock;               /* lock mode            */
        VOSMEMORY_REG1  Reg1;               /* register mode #1     */
        VOSMEMORY_REG2  Reg2;               /* register mode #2     */
        VOSMEMORY_SUPER Super;              /* supervisor mode      */
        VOSMEMORY_PROG  Prog;               /* program mode         */
    } Mode;
} VOSMEMORY;


typedef struct _VOSMEM_INFO {
    USHORT  usID;                           /* window ID            */
    VOID    FAR *pvAddr;                    /* address of memory    */
    USHORT  usBytes;                        /* memory size          */
} VOSMEMORY_INFO;


/*
*===========================================================================
*    External Data Declarations
*===========================================================================
*/
/* --- VosTable.C --- */
extern  VOSMEMORY       FAR         aVosMem2;
extern  VOSMEMORY_INFO  FARCONST    aVosMem2Info[];

#if     (_WIN32_WCE || WIN32) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif

/* ======================================= */
/* ========== End of VosTable.H ========== */
/* ======================================= */
