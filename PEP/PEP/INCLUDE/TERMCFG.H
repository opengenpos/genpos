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
* Category    : Header File, NCR 2170
* Program Name: TermCfg.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A/7.00 by Microsoft Corp.                         
* Memory Model: 
* Options     :
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
*
* Update Histories
*
* Data       Ver      Name        Description
* Nov-25-92  0.00.01  O.Nakada    Initial
* Sep-03-98  3.03.00  A.Mitsui    V3.3 Initial
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
#ifndef _INC_TERMCFG
#define _INC_TERMCFG


/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/
#define MODEL_HPUS                          /* Hospitality U.S. Model */

#define PCIF_WINDOWS                        /* PC I/F for Windows */

/*
*===========================================================================
*   2170 Hospitality Model Define,                          V3.3
*===========================================================================
*/

/*
*===========================================================================
*   Priority of Thread
*===========================================================================
*/
#define PRIO_UIE_KEYB       30              /* UI Engine Keyboard Input */
#define PRIO_UIE_SCAN       28              /* UI Engine Scanner Input */
#define PRIO_UIE_WAIT       26              /* UI Engine Waiter Lock Input */
#define PRIO_UIE_MSR        24              /* UI Engine MSR Input */
#define PRIO_UIE_DRAW       22              /* UI Engine Drawer Monitor */
#define PRIO_NOTICEBD       20              /* Notice Board */
#define PRIO_PMG_IN         18              /* Print Manager Input */
#define PRIO_PMG_OUT        16              /* Print Manager Output */
#define PRIO_DIF_RCV        15              /* Beverage Dispenser I/F        */
#define PRIO_KPS_PRT        14              /* Kitchen Printer Manager */
#define PRIO_SHARED_P       13              /* Shared Printer Manager */
#define PRIO_KPS_MON        12              /* Kitchen Printer Monitor */
#define PRIO_DISP_FLY       11              /* Dislay on the Fly */
#define PRIO_SERVER_T       10              /* Server for Terminals */
#define PRIO_CPM_RCV        9               /* Charge Posting Manager(HOTEL)*/
#define PRIO_SERVER_W       8               /* Server for Warkstation */
#define PRIO_CPM_SEND       7               /* Charge Posting Manager(HOTEL)*/
#define PRIO_UIC            6               /* User Interface Control */
#define PRIO_FDT            5               /* Flexible Drive Through */
#define PRIO_MAIN           4               /* Main Application */
#define PRIO_VOS            3               /* Video Output Service */
#define PRIO_TOTALS         2               /* Totals Update */


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
#define PERIPHERAL_DEVICE_SCANNER
#define PERIPHERAL_DEVICE_WAITER_LOCK
===== Reserved ===== 
*/


/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/


/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/


/*
*===========================================================================
*   LCD Interface for Video Output Service
*===========================================================================
*/
/* ----- Window ID ----- */
#define LCDWIN_ID_COMMON001     1           /* UIE - echo back window  */
#define LCDWIN_ID_COMMON002     2           /* UIE - descriptor window */
#define LCDWIN_ID_COMMON003     3           /* ----- free window ----- */
#define LCDWIN_ID_COMMON004     4           /* ----- free window ----- */
#define LCDWIN_ID_COMMON005     5           /* ----- free window ----- */
#define LCDWIN_ID_COMMON006     6           /* ----- free window ----- */
#define LCDWIN_ID_COMMON007     7           /* ----- free window ----- */
#define LCDWIN_ID_COMMON008     8           /* ----- free window ----- */
#define LCDWIN_ID_LOCK050       50          /* ----- free window ----- */
#define LCDWIN_ID_LOCK051       51          /* ----- free window ----- */
#define LCDWIN_ID_LOCK052       52          /* ----- free window ----- */
#define LCDWIN_ID_LOCK053       53          /* ----- free window ----- */
#define LCDWIN_ID_LOCK054       54          /* ----- free window ----- */
#define LCDWIN_ID_LOCK055       55          /* ----- free window ----- */
#define LCDWIN_ID_LOCK056       56          /* ----- free window ----- */
#define LCDWIN_ID_LOCK057       57          /* ----- free window ----- */
#define LCDWIN_ID_REG100        100         /* MLD - main scroll       */
#define LCDWIN_ID_REG101        101         /* MLD - sub display       */
#define LCDWIN_ID_REG102        102         /* MLD - popup display     */
#define LCDWIN_ID_REG103        103         /* MLD - cursor display    */
#define LCDWIN_ID_REG104        104         /* ----- free window ----- */
#define LCDWIN_ID_REG105        105         /* ----- free window ----- */
#define LCDWIN_ID_REG106        106         /* ----- free window ----- */
#define LCDWIN_ID_REG107        107         /* ----- free window ----- */
#define LCDWIN_ID_REG108        108         /* ----- free window ----- */
#define LCDWIN_ID_REG109        109         /* ----- free window ----- */
#define LCDWIN_ID_REG110        110         /* ----- free window ----- */
#define LCDWIN_ID_REG111        111         /* ----- free window ----- */
#define LCDWIN_ID_REG112        112         /* ----- free window ----- */
#define LCDWIN_ID_REG113        113         /* ----- free window ----- */
#define LCDWIN_ID_REG114        114         /* ----- free window ----- */
#define LCDWIN_ID_REG115        115         /* ----- free window ----- */
#define LCDWIN_ID_REG200        200         /* MLD - display #1        */
#define LCDWIN_ID_REG201        201         /* MLD - display #2        */
#define LCDWIN_ID_REG202        202         /* MLD - display #3        */
#define LCDWIN_ID_REG203        203         /* MLD - display #1 sub1   */
#define LCDWIN_ID_REG204        204         /* MLD - display #2 sub1   */
#define LCDWIN_ID_REG205        205         /* MLD - display #3 sub1   */
#define LCDWIN_ID_REG206        206         /* MLD - display #1 sub2   */
#define LCDWIN_ID_REG207        207         /* MLD - display #2 sub2   */
#define LCDWIN_ID_REG208        208         /* MLD - display #3 sub3   */
#define LCDWIN_ID_REG209        209         /* MLD - popup display      */
#define LCDWIN_ID_REG210        210         /* ----- free window ----- */
#define LCDWIN_ID_REG211        211         /* ----- free window ----- */
#define LCDWIN_ID_REG212        212         /* ----- free window ----- */
#define LCDWIN_ID_REG213        213         /* ----- free window ----- */
#define LCDWIN_ID_REG214        214         /* ----- free window ----- */
#define LCDWIN_ID_REG215        215         /* ----- free window ----- */
#define LCDWIN_ID_SUPER300      300         /* MLD - supervisor mode   */
#define LCDWIN_ID_SUPER301      301         /* ----- free window ----- */
#define LCDWIN_ID_SUPER302      302         /* ----- free window ----- */
#define LCDWIN_ID_SUPER303      303         /* ----- free window ----- */
#define LCDWIN_ID_SUPER304      304         /* ----- free window ----- */
#define LCDWIN_ID_SUPER305      305         /* ----- free window ----- */
#define LCDWIN_ID_SUPER306      306         /* ----- free window ----- */
#define LCDWIN_ID_SUPER307      307         /* ----- free window ----- */
#define LCDWIN_ID_SUPER308      308         /* ----- free window ----- */
#define LCDWIN_ID_SUPER309      309         /* ----- free window ----- */
#define LCDWIN_ID_SUPER310      310         /* ----- free window ----- */
#define LCDWIN_ID_SUPER311      311         /* ----- free window ----- */
#define LCDWIN_ID_SUPER312      312         /* ----- free window ----- */
#define LCDWIN_ID_SUPER313      313         /* ----- free window ----- */
#define LCDWIN_ID_SUPER314      314         /* ----- free window ----- */
#define LCDWIN_ID_SUPER315      315         /* ----- free window ----- */
#define LCDWIN_ID_PROG400       400         /* MLD -                   */
#define LCDWIN_ID_PROG401       401         /* ----- free window ----- */
#define LCDWIN_ID_PROG402       402         /* ----- free window ----- */
#define LCDWIN_ID_PROG403       403         /* ----- free window ----- */
#define LCDWIN_ID_PROG404       404         /* ----- free window ----- */
#define LCDWIN_ID_PROG405       405         /* ----- free window ----- */
#define LCDWIN_ID_PROG406       406         /* ----- free window ----- */
#define LCDWIN_ID_PROG407       407         /* ----- free window ----- */
#define LCDWIN_ID_PROG408       408         /* ----- free window ----- */
#define LCDWIN_ID_PROG409       409         /* ----- free window ----- */
#define LCDWIN_ID_PROG410       410         /* ----- free window ----- */
#define LCDWIN_ID_PROG411       411         /* ----- free window ----- */
#define LCDWIN_ID_PROG412       412         /* ----- free window ----- */
#define LCDWIN_ID_PROG413       413         /* ----- free window ----- */
#define LCDWIN_ID_PROG414       414         /* ----- free window ----- */
#define LCDWIN_ID_PROG415       415         /* ----- free window ----- */

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
