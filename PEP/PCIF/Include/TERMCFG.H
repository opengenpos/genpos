/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1998       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Header File of Terminal Configuration
* Category    : 2170 Hospitality US Application
* Program Name: TermCfg.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstract:     
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jun-24-92:00.00.01:O.Nakada   : Initial
* Sep-07-92:00.01.01:O.Nakada   : Removed Waiter Lock Thread         
* Sep-22-92:        :I.Shimizu  : Add Lowest Priority Thread for RAM M/C
*          :        :I.Shimizu  : Delete PRIO_LOW_MAIN               
* Oct-08-92:00.01.07:T.Koyama   : Add PRIO_DAG_SLIP and PRIO_DAG_KEY               
* Oct-26-92:00.01.0?:H.Shibamoto: change PRIORITIES
* Oct-25-93:00.00.01:Yoshiko.Jim: Add PRIO_CPM_XXX               
*                                                                       
* +-------------------+-------+-------+--------------------------------+
* | define            | OLD   | NEW   |                                |
* +-------------------+-------+-------+--------------------------------+
* | PRIO_SERVER_T     | 18    | 10    |    Server for Terminals        |
* | PRIO_SERVER_W     | 16    |  8    |    Server for Warkstation      |
* | PRIO_PMG_IN       | 10    | 18    |    Print Manager Input         |
* | PRIO_PMG_OUT      | 8     | 16    |    Print Manager Output        |
* +-------------------+-------+-------+--------------------------------+
*
* Nov-09-92:00.01.11:O.Nakada    : Add MODEL_HPUS
* Nov-26-92:01.00.01:O.Nakada    : Add PERIPHERAL_DEVICE_SCALE
* Mar-19-93:01.00.11:O.Nakada    : Removed Operating System definition
* Jun-18-93:01.00.12:NAKAJIMA,Ken: Add PRIO_SHARED_P
*                                : Add PRIO_DISP_FLY
* Mar-27-95:03.00.00:O.Nakada    : Add Video Output Service Control
* Apr-18-95:03.00.00:hkato       : add PRIO_FDT
* Feb-14-96:03.01.00:O.Nakada    : Add PERIPHERAL_DEVICE_OTHER
* Aug-06-98:03.03.00:hrkato      : Support Scanner
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
#ifndef _INC_TERMCFG
#define _INC_TERMCFG


/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/
#define MODEL_HPUS                          /* Hospitality U.S. Model */


/*
*===========================================================================
*   2170 Hospitality Model Define,                          V3.3
*===========================================================================
*/
/* #define WAITER_MODEL         */

/*
*===========================================================================
*   Priority of Thread
*===========================================================================
*/
#define PRIO_UIE_KEYB       30              /* UI Engine Keyboard Input      */
#define PRIO_UIE_SCAN       28              /* UI Engine Scanner Input       */
#define PRIO_UIE_WAIT       26              /* UI Engine Waiter Lock Input   */
#define PRIO_UIE_MSR        24              /* UI Engine MSR Input           */
#define PRIO_UIE_DRAW       22              /* UI Engine Drawer Monitor      */
#define PRIO_NOTICEBD       20              /* Notice Board                  */
#define PRIO_PMG_IN         18              /* Print Manager Input           */
#define PRIO_PMG_OUT        17              /* Print Manager Output          */
#define PRIO_DIF_RCV        16              /* Beverage Dispenser I/F        */
#define PRIO_KPS_PRT        15              /* Kitchen Printer Manager       */
#define PRIO_SHARED_P       14              /* Shared Printer Manager        */
#define PRIO_KPS_MON        13              /* Kitchen Printer Monitor       */
#define PRIO_ENH_KDS        12               /* Enhanced KDS, 2172            */
#define PRIO_DISP_FLY       11              /* Dislay on the Fly             */
#define PRIO_SERVER_T       10              /* Server for Terminals          */
#define PRIO_CPM_RCV        9               /* Charge Posting Manager(HOTEL) */
#define PRIO_EEPT_RCV       9               /* EPT Manager, R2.0 */
#define PRIO_SERVER_W       8               /* Server for Warkstation        */
#define PRIO_CPM_SEND       7               /* Charge Posting Manager(HOTEL) */
#define PRIO_EEPT_SEND      6               /* EPT Manager, R2.0 */
#define PRIO_UIC            6               /* User Interface Control        */
#define PRIO_FDT            5               /* Flexible Drive Through        */
#define PRIO_MAIN           4               /* Main Application              */
#define PRIO_VOS            3               /* Video Output Service          */
#define PRIO_TOTALS         2               /* Totals Update                 */


/*
*===========================================================================
*   Support Pheripheral Device
*===========================================================================
*/
#define PERIPHERAL_DEVICE_SCANNER                       /* V3.3 */
#define PERIPHERAL_DEVICE_SCALE
#define PERIPHERAL_DEVICE_MSR
#define PERIPHERAL_DEVICE_WAITER_LOCK
#define PERIPHERAL_DEVICE_OTHER     7

/*
===== Reserved ===== 
===== Reserved ===== 
*/


/*
*===========================================================================
*   LCD Interface for Video Output Service
*===========================================================================
*/
/* ----- CAUTION !! ----- */

#define LCDWIN_ID_BIAS          (0x0400 + 0x0300)       /* WM_USER + 0x0300 */

/* ----- Window ID ----- */
#define LCDWIN_ID_COMMON001     (LCDWIN_ID_BIAS + 1)    /* UIE - echo back window  */
#define LCDWIN_ID_COMMON002     (LCDWIN_ID_BIAS + 2)    /* UIE - descriptor window */
#define LCDWIN_ID_COMMON003     (LCDWIN_ID_BIAS + 3)    /* ----- free window ----- */
#define LCDWIN_ID_COMMON004     (LCDWIN_ID_BIAS + 4)    /* ----- free window ----- */
#define LCDWIN_ID_COMMON005     (LCDWIN_ID_BIAS + 5)    /* ----- free window ----- */
#define LCDWIN_ID_COMMON006     (LCDWIN_ID_BIAS + 6)    /* ----- free window ----- */
#define LCDWIN_ID_COMMON007     (LCDWIN_ID_BIAS + 7)    /* ----- free window ----- */
#define LCDWIN_ID_COMMON008     (LCDWIN_ID_BIAS + 8)    /* ----- free window ----- */
#define LCDWIN_ID_LOCK050       (LCDWIN_ID_BIAS + 50)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK051       (LCDWIN_ID_BIAS + 51)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK052       (LCDWIN_ID_BIAS + 52)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK053       (LCDWIN_ID_BIAS + 53)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK054       (LCDWIN_ID_BIAS + 54)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK055       (LCDWIN_ID_BIAS + 55)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK056       (LCDWIN_ID_BIAS + 56)   /* ----- free window ----- */
#define LCDWIN_ID_LOCK057       (LCDWIN_ID_BIAS + 57)   /* ----- free window ----- */
#define LCDWIN_ID_REG100        (LCDWIN_ID_BIAS + 100)  /* MLD - main scroll       */
#define LCDWIN_ID_REG101        (LCDWIN_ID_BIAS + 101)  /* MLD - sub display       */
#define LCDWIN_ID_REG102        (LCDWIN_ID_BIAS + 102)  /* MLD - popup display     */
#define LCDWIN_ID_REG103        (LCDWIN_ID_BIAS + 103)  /* MLD - cursor display    */
#define LCDWIN_ID_REG104        (LCDWIN_ID_BIAS + 104)  /* ----- free window ----- */
#define LCDWIN_ID_REG105        (LCDWIN_ID_BIAS + 105)  /* ----- free window ----- */
#define LCDWIN_ID_REG106        (LCDWIN_ID_BIAS + 106)  /* ----- free window ----- */
#define LCDWIN_ID_REG107        (LCDWIN_ID_BIAS + 107)  /* ----- free window ----- */
#define LCDWIN_ID_REG108        (LCDWIN_ID_BIAS + 108)  /* ----- free window ----- */
#define LCDWIN_ID_REG109        (LCDWIN_ID_BIAS + 109)  /* ----- free window ----- */
#define LCDWIN_ID_REG110        (LCDWIN_ID_BIAS + 110)  /* ----- free window ----- */
#define LCDWIN_ID_REG111        (LCDWIN_ID_BIAS + 111)  /* ----- free window ----- */
#define LCDWIN_ID_REG112        (LCDWIN_ID_BIAS + 112)  /* ----- free window ----- */
#define LCDWIN_ID_REG113        (LCDWIN_ID_BIAS + 113)  /* ----- free window ----- */
#define LCDWIN_ID_REG114        (LCDWIN_ID_BIAS + 114)  /* ----- free window ----- */
#define LCDWIN_ID_REG115        (LCDWIN_ID_BIAS + 115)  /* ----- free window ----- */
#define LCDWIN_ID_REG200        (LCDWIN_ID_BIAS + 200)  /* MLD - display #1        */
#define LCDWIN_ID_REG201        (LCDWIN_ID_BIAS + 201)  /* MLD - display #2        */
#define LCDWIN_ID_REG202        (LCDWIN_ID_BIAS + 202)  /* MLD - display #3        */
#define LCDWIN_ID_REG203        (LCDWIN_ID_BIAS + 203)  /* MLD - display #1 sub1   */
#define LCDWIN_ID_REG204        (LCDWIN_ID_BIAS + 204)  /* MLD - display #2 sub1   */
#define LCDWIN_ID_REG205        (LCDWIN_ID_BIAS + 205)  /* MLD - display #3 sub1   */
#define LCDWIN_ID_REG206        (LCDWIN_ID_BIAS + 206)  /* MLD - display #1 sub2   */
#define LCDWIN_ID_REG207        (LCDWIN_ID_BIAS + 207)  /* MLD - display #2 sub2   */
#define LCDWIN_ID_REG208        (LCDWIN_ID_BIAS + 208)  /* MLD - display #3 sub3   */
#define LCDWIN_ID_REG209        (LCDWIN_ID_BIAS + 209)  /* MLD - popup display      */
#define LCDWIN_ID_REG210        (LCDWIN_ID_BIAS + 210)  /* ----- free window ----- */
#define LCDWIN_ID_REG211        (LCDWIN_ID_BIAS + 211)  /* ----- free window ----- */
#define LCDWIN_ID_REG212        (LCDWIN_ID_BIAS + 212)  /* ----- free window ----- */
#define LCDWIN_ID_REG213        (LCDWIN_ID_BIAS + 213)  /* ----- free window ----- */
#define LCDWIN_ID_REG214        (LCDWIN_ID_BIAS + 214)  /* ----- free window ----- */
#define LCDWIN_ID_REG215        (LCDWIN_ID_BIAS + 215)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER300      (LCDWIN_ID_BIAS + 300)  /* MLD - supervisor mode   */
#define LCDWIN_ID_SUPER301      (LCDWIN_ID_BIAS + 301)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER302      (LCDWIN_ID_BIAS + 302)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER303      (LCDWIN_ID_BIAS + 303)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER304      (LCDWIN_ID_BIAS + 304)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER305      (LCDWIN_ID_BIAS + 305)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER306      (LCDWIN_ID_BIAS + 306)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER307      (LCDWIN_ID_BIAS + 307)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER308      (LCDWIN_ID_BIAS + 308)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER309      (LCDWIN_ID_BIAS + 309)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER310      (LCDWIN_ID_BIAS + 310)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER311      (LCDWIN_ID_BIAS + 311)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER312      (LCDWIN_ID_BIAS + 312)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER313      (LCDWIN_ID_BIAS + 313)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER314      (LCDWIN_ID_BIAS + 314)  /* ----- free window ----- */
#define LCDWIN_ID_SUPER315      (LCDWIN_ID_BIAS + 315)  /* ----- free window ----- */
#define LCDWIN_ID_PROG400       (LCDWIN_ID_BIAS + 400)  /* MLD -                   */
#define LCDWIN_ID_PROG401       (LCDWIN_ID_BIAS + 401)  /* ----- free window ----- */
#define LCDWIN_ID_PROG402       (LCDWIN_ID_BIAS + 402)  /* ----- free window ----- */
#define LCDWIN_ID_PROG403       (LCDWIN_ID_BIAS + 403)  /* ----- free window ----- */
#define LCDWIN_ID_PROG404       (LCDWIN_ID_BIAS + 404)  /* ----- free window ----- */
#define LCDWIN_ID_PROG405       (LCDWIN_ID_BIAS + 405)  /* ----- free window ----- */
#define LCDWIN_ID_PROG406       (LCDWIN_ID_BIAS + 406)  /* ----- free window ----- */
#define LCDWIN_ID_PROG407       (LCDWIN_ID_BIAS + 407)  /* ----- free window ----- */
#define LCDWIN_ID_PROG408       (LCDWIN_ID_BIAS + 408)  /* ----- free window ----- */
#define LCDWIN_ID_PROG409       (LCDWIN_ID_BIAS + 409)  /* ----- free window ----- */
#define LCDWIN_ID_PROG410       (LCDWIN_ID_BIAS + 410)  /* ----- free window ----- */
#define LCDWIN_ID_PROG411       (LCDWIN_ID_BIAS + 411)  /* ----- free window ----- */
#define LCDWIN_ID_PROG412       (LCDWIN_ID_BIAS + 412)  /* ----- free window ----- */
#define LCDWIN_ID_PROG413       (LCDWIN_ID_BIAS + 413)  /* ----- free window ----- */
#define LCDWIN_ID_PROG414       (LCDWIN_ID_BIAS + 414)  /* ----- free window ----- */
#define LCDWIN_ID_PROG415       (LCDWIN_ID_BIAS + 415)  /* ----- free window ----- */

/* ----- Window Buffer Length ----- */
#define LCDWIN_LEN_COMMON001    160
#define LCDWIN_LEN_COMMON002    160
#define LCDWIN_LEN_COMMON003    0           /* ------ not used ------- */
#define LCDWIN_LEN_COMMON004    0           /* ------ not used ------- */
#define LCDWIN_LEN_COMMON005    0           /* ------ not used ------- */
#define LCDWIN_LEN_COMMON006    0           /* ------ not used ------- */
#define LCDWIN_LEN_COMMON007    0           /* ------ not used ------- */
#define LCDWIN_LEN_COMMON008    0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK050      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK051      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK052      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK053      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK054      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK055      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK056      0           /* ------ not used ------- */
#define LCDWIN_LEN_LOCK057      0           /* ------ not used ------- */
#define LCDWIN_LEN_REG100       1092
#define LCDWIN_LEN_REG101       546
#define LCDWIN_LEN_REG102       1134
#define LCDWIN_LEN_REG103       42
#define LCDWIN_LEN_REG104       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG105       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG106       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG107       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG108       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG109       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG110       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG111       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG112       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG113       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG114       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG115       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG200       456
#define LCDWIN_LEN_REG201       494
#define LCDWIN_LEN_REG202       494
#define LCDWIN_LEN_REG203       24
#define LCDWIN_LEN_REG204       26
#define LCDWIN_LEN_REG205       26
#define LCDWIN_LEN_REG206       24
#define LCDWIN_LEN_REG207       26
#define LCDWIN_LEN_REG208       26
#define LCDWIN_LEN_REG209       588
#define LCDWIN_LEN_REG210       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG211       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG212       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG213       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG214       0           /* ------ not used ------- */
#define LCDWIN_LEN_REG215       0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER300     1680
#define LCDWIN_LEN_SUPER301     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER302     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER303     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER304     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER305     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER306     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER307     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER308     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER309     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER310     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER311     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER312     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER313     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER314     0           /* ------ not used ------- */
#define LCDWIN_LEN_SUPER315     0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG400      1680
#define LCDWIN_LEN_PROG401      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG402      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG403      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG404      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG405      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG406      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG407      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG408      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG409      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG410      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG411      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG412      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG413      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG414      0           /* ------ not used ------- */
#define LCDWIN_LEN_PROG415      0           /* ------ not used ------- */


#endif                                      /* _INC_TERMCFG */
/* ====================================== */
/* ========== End of TermCfg.H ========== */
/* ====================================== */
