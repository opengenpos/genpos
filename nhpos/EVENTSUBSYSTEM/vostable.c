/*
*===========================================================================
*
*   Copyright (c) 1995, AT&T Global Information Solutions Corporation,
*   Retail Systems Engineering. All rights reserved.
*
*===========================================================================
* Title       : Memory Allocation Table
* Category    : Viode Output Service, 2170 System Application
* Program Name: VosTable.C
* --------------------------------------------------------------------------
* Abstract:     
*
* --------------------------------------------------------------------------
* Update Histories
*
* Data       Ver.      Name           Description
* Feb-23-95  G0        : O.Nakada   : Initial
* Sep-29-15 : 2.02.01  : R.Chambers : added reflection of messages to customer display windows.
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


/*
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <tchar.h>
#include "ecr.h"
#include "vos.h"
#include "vostable.h"


/*
*===========================================================================
*   General Declarations
*===========================================================================
*/
VOSMEMORY       FAR         aVosMem2;
VOSMEMORY_INFO  FARCONST    aVosMem2Info[] =
{

/* ----- Common Mode Window ----- */
#if LCDWIN_LEN_COMMON001
#pragma message("Support for Common Window #001")
    {LCDWIN_ID_COMMON001,   aVosMem2.Common.auchBuf001,     LCDWIN_LEN_COMMON001},
#endif
#if LCDWIN_LEN_COMMON002
#pragma message("Support for Common Window #002")
    {LCDWIN_ID_COMMON002,   aVosMem2.Common.auchBuf002,     LCDWIN_LEN_COMMON002},
#endif
#if LCDWIN_LEN_COMMON003
#pragma message("Support for Common Window #003")
    {LCDWIN_ID_COMMON003,   aVosMem2.Common.auchBuf003,     LCDWIN_LEN_COMMON003},
#endif
#if LCDWIN_LEN_COMMON004
#pragma message("Support for Common Window #004")
    {LCDWIN_ID_COMMON004,   aVosMem2.Common.auchBuf004,     LCDWIN_LEN_COMMON004},
#endif
#if LCDWIN_LEN_COMMON005
#pragma message("Support for Common Window #005")
    {LCDWIN_ID_COMMON005,   aVosMem2.Common.auchBuf005,     LCDWIN_LEN_COMMON005},
#endif
#if LCDWIN_LEN_COMMON006
#pragma message("Support for Common Window #006")
    {LCDWIN_ID_COMMON006,   aVosMem2.Common.auchBuf006,     LCDWIN_LEN_COMMON006},
#endif
#if LCDWIN_LEN_COMMON007
#pragma message("Support for Common Window #007")
    {LCDWIN_ID_COMMON007,   aVosMem2.Common.auchBuf007,     LCDWIN_LEN_COMMON007},
#endif
#if LCDWIN_LEN_COMMON008
#pragma message("Support for Common Window #008")
    {LCDWIN_ID_COMMON008,   aVosMem2.Common.auchBuf008,     LCDWIN_LEN_COMMON008},
#endif

/* ----- Lock Mode Window #1 ----- */
#if LCDWIN_LEN_LOCK050
#pragma message("Support for Lock Mode Window #050")
    {LCDWIN_ID_LOCK050,      aVosMem2.Mode.Lock.auchBuf050,  LCDWIN_LEN_LOCK050},
#endif
#if LCDWIN_LEN_LOCK051
#pragma message("Support for Lock Mode Window #051")
    {LCDWIN_ID_LOCK051,      aVosMem2.Mode.Lock.auchBuf051,  LCDWIN_LEN_LOCK051},
#endif
#if LCDWIN_LEN_LOCK052
#pragma message("Support for Lock Mode Window #052")
    {LCDWIN_ID_LOCK052,      aVosMem2.Mode.Lock.auchBuf052,  LCDWIN_LEN_LOCK052},
#endif
#if LCDWIN_LEN_LOCK053
#pragma message("Support for Lock Mode Window #053")
    {LCDWIN_ID_LOCK053,      aVosMem2.Mode.Lock.auchBuf053,  LCDWIN_LEN_LOCK053},
#endif
#if LCDWIN_LEN_LOCK054
#pragma message("Support for Lock Mode Window #054")
    {LCDWIN_ID_LOCK054,      aVosMem2.Mode.Lock.auchBuf054,  LCDWIN_LEN_LOCK054},
#endif
#if LCDWIN_LEN_LOCK055
#pragma message("Support for Lock Mode Window #055")
    {LCDWIN_ID_LOCK055,      aVosMem2.Mode.Lock.auchBuf055,  LCDWIN_LEN_LOCK055},
#endif
#if LCDWIN_LEN_LOCK056
#pragma message("Support for Lock Mode Window #056")
    {LCDWIN_ID_LOCK056,      aVosMem2.Mode.Lock.auchBuf056,  LCDWIN_LEN_LOCK056},
#endif
#if LCDWIN_LEN_LOCK057
#pragma message("Support for Lock Mode Window #057")
    {LCDWIN_ID_LOCK057,      aVosMem2.Mode.Lock.auchBuf057,  LCDWIN_LEN_LOCK057},
#endif

/* ----- Register Mode Window #1 ----- */
#if LCDWIN_LEN_REG100
#pragma message("Support for Register Mode Window #100")
    {LCDWIN_ID_REG100,      aVosMem2.Mode.Reg1.auchBuf100,  LCDWIN_LEN_REG100},  // VOSMEMORY_INFO CONST aVosMem2Info
#endif
#if LCDWIN_LEN_REG101
#pragma message("Support for Register Mode Window #101")
#if ! defined (REDUNDANT_CHECK)
    {LCDWIN_ID_REG101,      aVosMem2.Mode.Reg1.auchBuf101,  LCDWIN_LEN_REG101 - 21 * 1 * 2},  // VOSMEMORY_INFO CONST aVosMem2Info
#else
    {LCDWIN_ID_REG101,      aVosMem2.Mode.Reg1.auchBuf101,  LCDWIN_LEN_REG101},
#endif
#endif
#if LCDWIN_LEN_REG102
#pragma message("Support for Register Mode Window #102")
    {LCDWIN_ID_REG102,      aVosMem2.Mode.Reg1.auchBuf102,  LCDWIN_LEN_REG102},  // VOSMEMORY_INFO CONST aVosMem2Info
#endif
#if LCDWIN_LEN_REG103
#pragma message("Support for Register Mode Window #103")
    {LCDWIN_ID_REG103,      aVosMem2.Mode.Reg1.auchBuf103,  LCDWIN_LEN_REG103},  // VOSMEMORY_INFO CONST aVosMem2Info
#endif
#if LCDWIN_LEN_REG104
#pragma message("Support for Register Mode Window #104")
    {LCDWIN_ID_REG104,      aVosMem2.Mode.Reg1.auchBuf104,  LCDWIN_LEN_REG104},
#endif
#if LCDWIN_LEN_REG105
#pragma message("Support for Register Mode Window #105")
    {LCDWIN_ID_REG105,      aVosMem2.Mode.Reg1.auchBuf105,  LCDWIN_LEN_REG105},
#endif
#if LCDWIN_LEN_REG106
#pragma message("Support for Register Mode Window #106")
    {LCDWIN_ID_REG106,      aVosMem2.Mode.Reg1.auchBuf106,  LCDWIN_LEN_REG106},
#endif
#if LCDWIN_LEN_REG107
#pragma message("Support for Register Mode Window #107")
    {LCDWIN_ID_REG107,      aVosMem2.Mode.Reg1.auchBuf107,  LCDWIN_LEN_REG107},
#endif
#if LCDWIN_LEN_REG108
#pragma message("Support for Register Mode Window #108")
    {LCDWIN_ID_REG108,      aVosMem2.Mode.Reg1.auchBuf108,  LCDWIN_LEN_REG108},
#endif
#if LCDWIN_LEN_REG109
#pragma message("Support for Register Mode Window #109")
    {LCDWIN_ID_REG109,      aVosMem2.Mode.Reg1.auchBuf109,  LCDWIN_LEN_REG109},
#endif
#if LCDWIN_LEN_REG110
#pragma message("Support for Register Mode Window #110")
    {LCDWIN_ID_REG110,      aVosMem2.Mode.Reg1.auchBuf110,  LCDWIN_LEN_REG110},
#endif
#if LCDWIN_LEN_REG111
#pragma message("Support for Register Mode Window #111")
    {LCDWIN_ID_REG111,      aVosMem2.Mode.Reg1.auchBuf111,  LCDWIN_LEN_REG111},
#endif
#if LCDWIN_LEN_REG112
#pragma message("Support for Register Mode Window #112")
    {LCDWIN_ID_REG112,      aVosMem2.Mode.Reg1.auchBuf112,  LCDWIN_LEN_REG112},
#endif
#if LCDWIN_LEN_REG113
#pragma message("Support for Register Mode Window #113")
    {LCDWIN_ID_REG113,      aVosMem2.Mode.Reg1.auchBuf113,  LCDWIN_LEN_REG113},
#endif
#if LCDWIN_LEN_REG114
#pragma message("Support for Register Mode Window #114")
    {LCDWIN_ID_REG114,      aVosMem2.Mode.Reg1.auchBuf114,  LCDWIN_LEN_REG114},
#endif
#if LCDWIN_LEN_REG115
#pragma message("Support for Register Mode Window #115")
    {LCDWIN_ID_REG115,      aVosMem2.Mode.Reg1.auchBuf115,  LCDWIN_LEN_REG115},
#endif

/* ----- Register Mode Window #2 ----- */
#if LCDWIN_LEN_REG200
#pragma message("Support for Register Mode Window #200")
    {LCDWIN_ID_REG200,      aVosMem2.Mode.Reg2.auchBuf200,  LCDWIN_LEN_REG200},
#endif
#if LCDWIN_LEN_REG201
#pragma message("Support for Register Mode Window #201")
#if ! defined (REDUNDANT_CHECK)
    {LCDWIN_ID_REG201,      aVosMem2.Mode.Reg2.auchBuf201,  LCDWIN_LEN_REG201 - 19 * 1 * 2},
#else
    {LCDWIN_ID_REG201,      aVosMem2.Mode.Reg2.auchBuf201,  LCDWIN_LEN_REG201},
#endif
#endif
#if LCDWIN_LEN_REG202
#pragma message("Support for Register Mode Window #202")
#if ! defined (REDUNDANT_CHECK)
    {LCDWIN_ID_REG202,      aVosMem2.Mode.Reg2.auchBuf202,  LCDWIN_LEN_REG202 - 19 * 1 * 2},
#else
    {LCDWIN_ID_REG202,      aVosMem2.Mode.Reg2.auchBuf202,  LCDWIN_LEN_REG202},
#endif
#endif
#if LCDWIN_LEN_REG203
#pragma message("Support for Register Mode Window #203")
    {LCDWIN_ID_REG203,      aVosMem2.Mode.Reg2.auchBuf203,  LCDWIN_LEN_REG203},
#endif
#if LCDWIN_LEN_REG204
#pragma message("Support for Register Mode Window #204")
#if ! defined (REDUNDANT_CHECK)
    {LCDWIN_ID_REG204,      aVosMem2.Mode.Reg2.auchBuf204,  LCDWIN_LEN_REG204 - 1 * 1 * 2},
#else
    {LCDWIN_ID_REG204,      aVosMem2.Mode.Reg2.auchBuf204,  LCDWIN_LEN_REG204},
#endif
#endif
#if LCDWIN_LEN_REG205
#pragma message("Support for Register Mode Window #205")
#if ! defined (REDUNDANT_CHECK)
    {LCDWIN_ID_REG205,      aVosMem2.Mode.Reg2.auchBuf205,  LCDWIN_LEN_REG205 - 1 * 1 * 2},
#else
    {LCDWIN_ID_REG205,      aVosMem2.Mode.Reg2.auchBuf205,  LCDWIN_LEN_REG205},
#endif
#endif
#if LCDWIN_LEN_REG206
#pragma message("Support for Register Mode Window #206")
    {LCDWIN_ID_REG206,      aVosMem2.Mode.Reg2.auchBuf206,  LCDWIN_LEN_REG206},
#endif
#if LCDWIN_LEN_REG207
#pragma message("Support for Register Mode Window #207")
#if ! defined (REDUNDANT_CHECK)
    {LCDWIN_ID_REG207,      aVosMem2.Mode.Reg2.auchBuf207,  LCDWIN_LEN_REG207 - 1 * 1 * 2},
#else
    {LCDWIN_ID_REG207,      aVosMem2.Mode.Reg2.auchBuf207,  LCDWIN_LEN_REG207},
#endif
#endif
#if LCDWIN_LEN_REG208
#pragma message("Support for Register Mode Window #208")
#if ! defined (REDUNDANT_CHECK)
    {LCDWIN_ID_REG208,      aVosMem2.Mode.Reg2.auchBuf208,  LCDWIN_LEN_REG208 - 1 * 1 * 2},
#else
    {LCDWIN_ID_REG208,      aVosMem2.Mode.Reg2.auchBuf208,  LCDWIN_LEN_REG208},
#endif
#endif
#if LCDWIN_LEN_REG209
#pragma message("Support for Register Mode Window #209")
    {LCDWIN_ID_REG209,      aVosMem2.Mode.Reg2.auchBuf209,  LCDWIN_LEN_REG209},
#endif
#if LCDWIN_LEN_REG210
#pragma message("Support for Register Mode Window #210")
    {LCDWIN_ID_REG210,      aVosMem2.Mode.Reg2.auchBuf210,  LCDWIN_LEN_REG210},
#endif
#if LCDWIN_LEN_REG211
#pragma message("Support for Register Mode Window #211")
    {LCDWIN_ID_REG211,      aVosMem2.Mode.Reg2.auchBuf211,  LCDWIN_LEN_REG211},
#endif
#if LCDWIN_LEN_REG212
#pragma message("Support for Register Mode Window #212")
    {LCDWIN_ID_REG212,      aVosMem2.Mode.Reg2.auchBuf212,  LCDWIN_LEN_REG212},
#endif
#if LCDWIN_LEN_REG213
#pragma message("Support for Register Mode Window #213")
    {LCDWIN_ID_REG213,      aVosMem2.Mode.Reg2.auchBuf213,  LCDWIN_LEN_REG213},
#endif
#if LCDWIN_LEN_REG214
#pragma message("Support for Register Mode Window #214")
    {LCDWIN_ID_REG214,      aVosMem2.Mode.Reg2.auchBuf214,  LCDWIN_LEN_REG214},
#endif
#if LCDWIN_LEN_REG215
#pragma message("Support for Register Mode Window #215")
    {LCDWIN_ID_REG215,      aVosMem2.Mode.Reg2.auchBuf215,  LCDWIN_LEN_REG215},
#endif

/* ----- Supervisor Mode Window ----- */
#if LCDWIN_LEN_SUPER300
#pragma message("Support for Supervisor Mode Window #300")
    {LCDWIN_ID_SUPER300,    aVosMem2.Mode.Super.auchBuf300, LCDWIN_LEN_SUPER300},
#endif
#if LCDWIN_LEN_SUPER301
#pragma message("Support for Supervisor Mode Window #301")
    {LCDWIN_ID_SUPER301,    aVosMem2.Mode.Super.auchBuf301, LCDWIN_LEN_SUPER301},
#endif
#if LCDWIN_LEN_SUPER302
#pragma message("Support for Supervisor Mode Window #302")
    {LCDWIN_ID_SUPER302,    aVosMem2.Mode.Super.auchBuf302, LCDWIN_LEN_SUPER302},
#endif
#if LCDWIN_LEN_SUPER303
#pragma message("Support for Supervisor Mode Window #303")
    {LCDWIN_ID_SUPER303,    aVosMem2.Mode.Super.auchBuf303, LCDWIN_LEN_SUPER303},
#endif
#if LCDWIN_LEN_SUPER304
#pragma message("Support for Supervisor Mode Window #304")
    {LCDWIN_ID_SUPER304,    aVosMem2.Mode.Super.auchBuf304, LCDWIN_LEN_SUPER304},
#endif
#if LCDWIN_LEN_SUPER305
#pragma message("Support for Supervisor Mode Window #305")
    {LCDWIN_ID_SUPER305,    aVosMem2.Mode.Super.auchBuf305, LCDWIN_LEN_SUPER305},
#endif
#if LCDWIN_LEN_SUPER306
#pragma message("Support for Supervisor Mode Window #306")
    {LCDWIN_ID_SUPER306,    aVosMem2.Mode.Super.auchBuf306, LCDWIN_LEN_SUPER306},
#endif
#if LCDWIN_LEN_SUPER307
#pragma message("Support for Supervisor Mode Window #307")
    {LCDWIN_ID_SUPER307,    aVosMem2.Mode.Super.auchBuf307, LCDWIN_LEN_SUPER307},
#endif
#if LCDWIN_LEN_SUPER308
#pragma message("Support for Supervisor Mode Window #308")
    {LCDWIN_ID_SUPER308,    aVosMem2.Mode.Super.auchBuf308, LCDWIN_LEN_SUPER308},
#endif
#if LCDWIN_LEN_SUPER309
#pragma message("Support for Supervisor Mode Window #309")
    {LCDWIN_ID_SUPER309,    aVosMem2.Mode.Super.auchBuf309, LCDWIN_LEN_SUPER309},
#endif
#if LCDWIN_LEN_SUPER310
#pragma message("Support for Supervisor Mode Window #310")
    {LCDWIN_ID_SUPER310,    aVosMem2.Mode.Super.auchBuf310, LCDWIN_LEN_SUPER310},
#endif
#if LCDWIN_LEN_SUPER311
#pragma message("Support for Supervisor Mode Window #311")
    {LCDWIN_ID_SUPER311,    aVosMem2.Mode.Super.auchBuf311, LCDWIN_LEN_SUPER311},
#endif
#if LCDWIN_LEN_SUPER312
#pragma message("Support for Supervisor Mode Window #312")
    {LCDWIN_ID_SUPER312,    aVosMem2.Mode.Super.auchBuf312, LCDWIN_LEN_SUPER312},
#endif
#if LCDWIN_LEN_SUPER313
#pragma message("Support for Supervisor Mode Window #313")
    {LCDWIN_ID_SUPER313,    aVosMem2.Mode.Super.auchBuf313, LCDWIN_LEN_SUPER313},
#endif
#if LCDWIN_LEN_SUPER314
#pragma message("Support for Supervisor Mode Window #314")
    {LCDWIN_ID_SUPER314,    aVosMem2.Mode.Super.auchBuf314, LCDWIN_LEN_SUPER314},
#endif
#if LCDWIN_LEN_SUPER315
#pragma message("Support for Supervisor Mode Window #315")
    {LCDWIN_ID_SUPER315,    aVosMem2.Mode.Super.auchBuf315, LCDWIN_LEN_SUPER315},
#endif

/* ----- Program Mode Window ----- */
#if LCDWIN_LEN_PROG400
#pragma message("Support for Program Mode Window #400")
    {LCDWIN_ID_PROG400,     aVosMem2.Mode.Prog.auchBuf400,  LCDWIN_LEN_PROG400},
#endif
#if LCDWIN_LEN_PROG401
#pragma message("Support for Program Mode Window #401")
    {LCDWIN_ID_PROG401,     aVosMem2.Mode.Prog.auchBuf401,  LCDWIN_LEN_PROG401},
#endif
#if LCDWIN_LEN_PROG402
#pragma message("Support for Program Mode Window #402")
    {LCDWIN_ID_PROG402,     aVosMem2.Mode.Prog.auchBuf402,  LCDWIN_LEN_PROG402},
#endif
#if LCDWIN_LEN_PROG403
#pragma message("Support for Program Mode Window #403")
    {LCDWIN_ID_PROG403,     aVosMem2.Mode.Prog.auchBuf403,  LCDWIN_LEN_PROG403},
#endif
#if LCDWIN_LEN_PROG404
#pragma message("Support for Program Mode Window #404")
    {LCDWIN_ID_PROG404,     aVosMem2.Mode.Prog.auchBuf404,  LCDWIN_LEN_PROG404},
#endif
#if LCDWIN_LEN_PROG405
#pragma message("Support for Program Mode Window #405")
    {LCDWIN_ID_PROG405,     aVosMem2.Mode.Prog.auchBuf405,  LCDWIN_LEN_PROG405},
#endif
#if LCDWIN_LEN_PROG406
#pragma message("Support for Program Mode Window #406")
    {LCDWIN_ID_PROG406,     aVosMem2.Mode.Prog.auchBuf406,  LCDWIN_LEN_PROG406},
#endif
#if LCDWIN_LEN_PROG407
#pragma message("Support for Program Mode Window #407")
    {LCDWIN_ID_PROG407,     aVosMem2.Mode.Prog.auchBuf407,  LCDWIN_LEN_PROG407},
#endif
#if LCDWIN_LEN_PROG408
#pragma message("Support for Program Mode Window #408")
    {LCDWIN_ID_PROG408,     aVosMem2.Mode.Prog.auchBuf408,  LCDWIN_LEN_PROG408},
#endif
#if LCDWIN_LEN_PROG409
#pragma message("Support for Program Mode Window #409")
    {LCDWIN_ID_PROG409,     aVosMem2.Mode.Prog.auchBuf409,  LCDWIN_LEN_PROG409},
#endif
#if LCDWIN_LEN_PROG410
#pragma message("Support for Program Mode Window #410")
    {LCDWIN_ID_PROG410,     aVosMem2.Mode.Prog.auchBuf410,  LCDWIN_LEN_PROG410},
#endif
#if LCDWIN_LEN_PROG411
#pragma message("Support for Program Mode Window #411")
    {LCDWIN_ID_PROG411,     aVosMem2.Mode.Prog.auchBuf411,  LCDWIN_LEN_PROG411},
#endif
#if LCDWIN_LEN_PROG412
#pragma message("Support for Program Mode Window #412")
    {LCDWIN_ID_PROG412,     aVosMem2.Mode.Prog.auchBuf412,  LCDWIN_LEN_PROG412},
#endif
#if LCDWIN_LEN_PROG413
#pragma message("Support for Program Mode Window #413")
    {LCDWIN_ID_PROG413,     aVosMem2.Mode.Prog.auchBuf413,  LCDWIN_LEN_PROG413},
#endif
#if LCDWIN_LEN_PROG414
#pragma message("Support for Program Mode Window #414")
    {LCDWIN_ID_PROG414,     aVosMem2.Mode.Prog.auchBuf414,  LCDWIN_LEN_PROG414},
#endif
#if LCDWIN_LEN_PROG415
#pragma message("Support for Program Mode Window #415")
    {LCDWIN_ID_PROG415,     aVosMem2.Mode.Prog.auchBuf415,  LCDWIN_LEN_PROG415},
#endif

    {0,                     NULL,                           0}
};


/* ======================================= */
/* ========== End of VosTable.C ========== */
/* ======================================= */
