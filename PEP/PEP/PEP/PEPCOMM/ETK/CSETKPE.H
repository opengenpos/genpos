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
*   Title              : Client/Server ETK module, Header file for internal                        
*   Category           : Client/Server ETK module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSETKIN.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            :  /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define,typedef,struct,prototype,memory
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*    Date                Ver.Rev    :NAME         :Description
*    Oct-12-93         : 00.00.01   :H.YAMAGUCHI  :Modified FVT
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

#define    ETK_SEARCH_ONLY              1        /* FOR Etk_Index() */
#define    ETK_REGIST                   2        /* FOR Etk_Index() */
#if defined(POSSIBLE_DEAD_CODE)
#define    ETK_CONT                     1

#define    ETK_END                      2

/* #define    RFL_HIT                      0  */
/* #define    RFL_HEAD_OF_TABLE           -1  */
/* #define    RFL_DONT_HIT                -2  */
/* #define    RFL_TAIL_OF_TABLE           -3  */

/* #define    ETK_MAX_WORK_BUFFER       1000  */
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

SHORT  Etk_CopyFile(ETK_FILEHEAD     *pEtkFileHedNew,
                    ETK_FILEHEAD     *pEtkFileHedOld);

VOID   Etk_WriteNew(ULONG  offulFileSeek, 
                    VOID   *pTableHeadAddress, 
                    USHORT usSizeofWrite);
/*========= End of Define =========*/
#endif