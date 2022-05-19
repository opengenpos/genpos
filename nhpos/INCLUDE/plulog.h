/*************************************************************************************************
 *
 *        *=*=*=*=*=*=*=*=*
 *        *  NCR 2172     *             AT&T GIS Corporation, E&M OISO
 *     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 2000-
 *    <|\/~               ~\/|>
 *   _/^\_                 _/^\_
 *
 *************************************************************************************************/
/*========================================================================*\
*   Title        : Totaldata (background/backup) copy module
*   Category     : Plutotal module, NCR2172 Saratoga application
*   Program Name : plulog.h
*  ------------------------------------------------------------------------
*   Compiler     : MS-VC++ Ver.6.00 by Microsoft Corp.
*   Options      :
*  ------------------------------------------------------------------------
*   Abstract     :  TBC (Totaldata Backup/Background Copy) internal header file
*  ------------------------------------------------------------------------
*    Update Histories   :
*
*   Date        Ver.Rev  :NAME         :Description
*   May-31-00 : 00.00.01 :m.teraki     :Initial
\*=======================================================================*/

/*
 *  This file contains some #define-ed symbols related to PluTotal.dll
 *  All of following symbols are used to calling PifAbort().
 *
 *  +----------------------------------+
 *  | PifAbort                 [ok][x] |
 *  +----------------------------------+
 *  | FAULT xxxxxx Module=yyy Code=zzz |
 *  | TURN OFF AND ON                  |
 *  +----------------------------------+
 *
 *    xxxxx: [module name] this string is assigned at creating thread (see PifCreateThread())
 *    yyy  : [module number] this value is choosed from following #define-ed symbols.
 *    zzz  : [fault code] this value is choosed from log.h (scan FAULT_XXXX_XXXX)
 *
 */
#define MODULE_PLUTOTAL_BACKGROUND_COPY (600)   /* tbc.lib/tbcthrd.c in PluTotal build tree */
#define MODULE_PLUTOTAL_TBCAPI          (601)   /* tbc.lib/tbcapi.c  in PluTotal build tree */
/* #define MODULE_PLUTOTAL_RESERVED        (602) */
/* #define MODULE_PLUTOTAL_RESERVED        (603) */
/* #define MODULE_PLUTOTAL_RESERVED        (604) */
/* #define MODULE_PLUTOTAL_RESERVED        (605) */
/* #define MODULE_PLUTOTAL_RESERVED        (606) */
/* #define MODULE_PLUTOTAL_RESERVED        (607) */
/* #define MODULE_PLUTOTAL_RESERVED        (608) */
/* #define MODULE_PLUTOTAL_RESERVED        (609) */
/* #define MODULE_PLUTOTAL_RESERVED        (610) */
/* #define MODULE_PLUTOTAL_RESERVED        (611) */
/* #define MODULE_PLUTOTAL_RESERVED        (612) */
/* #define MODULE_PLUTOTAL_RESERVED        (613) */
/* #define MODULE_PLUTOTAL_RESERVED        (614) */
/* #define MODULE_PLUTOTAL_RESERVED        (615) */
/* #define MODULE_PLUTOTAL_RESERVED        (616) */
/* #define MODULE_PLUTOTAL_RESERVED        (618) */
/* #define MODULE_PLUTOTAL_RESERVED        (619) */

/*************************************************************************************************
 * end of file
 *************************************************************************************************/
