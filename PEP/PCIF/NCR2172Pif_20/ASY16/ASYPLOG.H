/*
*===========================================================================
* Title       : Logging Function Header File
* Category    : Asynchornous Driver for PC, NCR 2170 Operating System
* Program Name: ASYPLOG.H
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model: Compact Model
* Options     : /c /Asfu /W4 /G2s /Zp
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*    Date  :Ver.Rev.:   Name    : Description
* Aug-27-92:00.00.01:O.Nakada   : Initial
*          :        :           :
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
*   Define Declarations 
*===========================================================================
*/
/*
*===========================================================================
*   Logging Function
*===========================================================================
*/
#define ASY_LOG_LENGTH                  512

#define MODULE_ASY_INIT                 0x0011
#define MODULE_ASY_OPEN                 0x0021
#define MODULE_ASY_CLOSE                0x0031
#define MODULE_ASY_READ                 0x0041
#define MODULE_ASY_WRITE                0x0051
#define MODULE_ASY_WRITE_SELECT         0x0052
#define MODULE_ASY_WRITE_SELECT_DATA    0x0053
#define MODULE_ASY_READ_FIRST_CHAR      0x0061
#define MODULE_ASY_READ_REMAIN_MES      0x0061
#define MODULE_ASY_READ_REMAIN_LEN      0x0062
#define MODULE_ASY_READ_REMAIN_DATA     0x0063
#define MODULE_ASY_READ_REMAIN_CRC      0x0064
#define MODULE_ASY_WRITE_MES_DATA       0x0071

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
extern SHORT   ausAsyLog[ASY_LOG_LENGTH];
extern USHORT  iusAsyWrite;


/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/* ASYPLOG.C */
VOID AsyWriteLog(SHORT sModuleId, SHORT sStatus);

