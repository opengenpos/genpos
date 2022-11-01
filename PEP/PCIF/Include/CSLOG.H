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
*   Title              : Client/Server CASHIER module, Header file for LOG                        
*   Category           : Client/Server CASHIER module, NCR2170 US HOSPITALITY APPLICATION
*   Program Name       : CSCASLOG.H                                            
*  ------------------------------------------------------------------------  
*   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              
*   Memory Model       : Midium Model
*   Options            : /c /AM /Gs /Os /Za /Zp /W4
*  ------------------------------------------------------------------------
*   Abstract           : define
*
*  ------------------------------------------------------------------------
*   Update Histories   :
*
*    Date                Ver.Rev    :NAME         :Description
*    May-06-92         : 00.00.00   :M.YAMAMOTO   :Initial
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

/*
===========================================================================
    DEFINE
===========================================================================
*/

/* MODULE ID */

#define     MODULE_CASHIER             370      
#define     MODULE_WAITER              360      
#define     MODULE_PLU                 450
#define     MODULE_DEPT                451 
#define     MODULE_BACKUP              452
#define     MODULE_GCF                 410
#define     MODULE_KP                  400

/* LOG FOR PifAbort */

#define    FAULT_ERROR_FILE_OPEN        20      
#define    FAULT_ERROR_FILE_SEEK        21      
#define    FAULT_ERROR_FILE_READ        22      
#define    FAULT_ERROR_FILE_WRITE       23
#define    FAULT_ERROR_BLOCK_FULL       24


