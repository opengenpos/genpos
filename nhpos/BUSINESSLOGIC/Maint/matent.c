/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Enter Program/Super Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATENT.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*                     
*               MaintEntSup()   : Check Supervisor Entry Condition  
*               MaintEntPrg()   : Check Program Entry Condition
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Jun-18-92: 00.00.01 : K.You     : initial                                   
* Nov-19-92: 01.00.00 : K.you     : Adds AC 111,176 handling.                                   
* Jun-23-93: 01.00.12 : J.IKEDA   : Adds PG_SHR_PRT
* Jun-30-93: 01.00.12 : M.Yamamoto: Add AC 150,152,152,87,129.
* Jul-02-93: 01.00.12 : K.You     : Add PROG. 15.
* Jul-09-93: 01.00.12 : K.You     : Add A/C 130.
* Aug-23-93: 01.00.13 : J.IKEDA   : Add AC_SOFTCHK_MSG
* Oct-05-93: 02.00.01 : J.IKEDA   : Add AC_ETK_MAINT
* Aug-11-99: 03.05.00 : M.Teraki  : Remove WAITER_MODEL
* Dec-28-99: 01.00.00 : hrkato    : Saratoga
* Dec-19-02:          :R.Chambers : SR 15 for Software Security
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

/**
==============================================================================
;                      I N C L U D E     F I L E s                         
=============================================================================
**/
#include	<tchar.h>
#include <string.h>

#include <ecr.h>
/* #include <uie.h> */
#include <paraequ.h>
#include <para.h>
#include <plu.h>
#include <csstbfcc.h>
#include <csstbstb.h>
#include <csstbbak.h>
#include <csstbpar.h>
#include <maint.h>
#include <csttl.h>
#include <csop.h>
#include <report.h>

#include "maintram.h"

/*
*===========================================================================
** Synopsis:    SHORT MaintEntSup( MAINTENT *pData )
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      PERFORM: permit operarion
*               LDT_PROHBT_ADR: prohibit operation
*               LDT_SEQERR_ADR: sequence error
*
** Description: This function checks supervisor security level.
*				The Supervisor Security Level is set in the P9 dialog
*               of PEP.  The security levels are actually specified
*               using a bit pattern in nibbles of the memory resident
*               database in table ParaActCodeSec[].
*
*               The security address is usually but not always the
*               Supervisor Action Code specified in pData->usSuperNo.
*               In some cases the action code number is out of the range
*               of the array ParaActCodeSec[] so a number different from
*               the action code is used for the security address
*===========================================================================
*/
struct  s_SecurityTableStruct {
	USHORT   usActionCode;
	USHORT   usSecurityAddress;
	USHORT   usSpecialFlags;
};

#define SPECIAL_FLAG_MASTER       0x0001   // Prohibit if not done on a Master Terminal
#define SPECIAL_FLAG_BACKUP       0x0002   // Prohibit if not done on a Backup Terminal
#define SPECIAL_FLAG_EXCEPTION    0x0004   // AC is exception to general rules, handle special

static struct s_SecurityTableStruct  mySecurityTable [] = {
    {AC_GCNO_ASSIN,           AC_GCNO_ASSIN,           SPECIAL_FLAG_MASTER},
    {AC_GCFL_READ_RPT,        AC_GCFL_READ_RPT,        0},
    {AC_GCFL_RESET_RPT,       AC_GCFL_RESET_RPT,       SPECIAL_FLAG_MASTER},
    {AC_PLU_MENU,             AC_PLU_MENU,             0},
    {AC_SET_CTLPG,            AC_SET_CTLPG,            0},
    {AC_MANU_KITCH,           AC_MANU_KITCH,           SPECIAL_FLAG_MASTER},
    {AC_CASH_ABKEY,           AC_CASH_ABKEY,           0},
    {AC_LOAN,                 AC_LOAN,                 0},
    {AC_PICKUP,               AC_PICKUP,               0},
    {AC_SUPSEC_NO,            AC_SUPSEC_NO,            SPECIAL_FLAG_EXCEPTION},

    {AC_TOD,                  AC_TOD,                  0},
    {AC_PTD_RPT,              AC_PTD_RPT,              SPECIAL_FLAG_MASTER},
    {AC_CASH_RESET_RPT,       AC_CASH_RESET_RPT,       SPECIAL_FLAG_MASTER},
    {AC_CASH_ASSIN,           AC_CASH_ASSIN,           SPECIAL_FLAG_MASTER},
    {AC_CASH_READ_RPT,        AC_CASH_READ_RPT,        0},
    {AC_REGFIN_READ_RPT,      AC_REGFIN_READ_RPT,      0},
    {AC_HOURLY_READ_RPT,      AC_HOURLY_READ_RPT,      0},
    {AC_DEPTSALE_READ_RPT,    AC_DEPTSALE_READ_RPT,    0},
    {AC_PLUSALE_READ_RPT,     AC_PLUSALE_READ_RPT,     0},
    {AC_CPN_READ_RPT,         AC_CPN_READ_RPT,         SPECIAL_FLAG_MASTER},

    {AC_CSTR_MAINT,           AC_CSTR_MAINT,           SPECIAL_FLAG_MASTER},
    {AC_PLU_RESET_RPT,        AC_PLU_RESET_RPT,        SPECIAL_FLAG_MASTER},
    {AC_CPN_RESET_RPT,        AC_CPN_RESET_RPT,        SPECIAL_FLAG_MASTER},
    {AC_COPY_TTL,             AC_COPY_TTL,             SPECIAL_FLAG_MASTER},
    {AC_PLU_MAINT,            AC_PLU_MAINT,            SPECIAL_FLAG_MASTER},
    {AC_PLU_PRICE_CHG,        AC_PLU_PRICE_CHG,        SPECIAL_FLAG_MASTER},
    {AC_PLU_ADDDEL,           AC_PLU_ADDDEL,           SPECIAL_FLAG_MASTER},
    {AC_EJ_READ_RPT,          AC_EJ_READ_RPT,          0},
    {AC_PPI_MAINT,            AC_PPI_MAINT,            SPECIAL_FLAG_MASTER},
    {AC_PLUPARA_READ_RPT,     AC_PLUPARA_READ_RPT,     SPECIAL_FLAG_MASTER},

    {AC_ALL_DOWN,             AC_ALL_DOWN,             0},
    {AC_START_STOP_CPM,       AC_START_STOP_CPM,       SPECIAL_FLAG_MASTER},
    {AC_PLU_MNEMO_CHG,        AC_PLU_MNEMO_CHG,        SPECIAL_FLAG_MASTER},
    {AC_ROUND,                AC_ROUND,                SPECIAL_FLAG_MASTER},
    {AC_DISCON_MAST,          AC_DISCON_MAST,          SPECIAL_FLAG_EXCEPTION},
    {AC_DISC_BONUS,           AC_DISC_BONUS,           SPECIAL_FLAG_MASTER},
    {AC_SOFTCHK_MSG,          AC_SOFTCHK_MSG,          SPECIAL_FLAG_MASTER},
    {AC_SALES_PROMOTION,      AC_SALES_PROMOTION,      SPECIAL_FLAG_MASTER},
    {AC_CURRENCY,             AC_CURRENCY,             SPECIAL_FLAG_MASTER},
    {AC_TALLY_CPM_EPT,        AC_TALLY_CPM_EPT,        0},

    {AC_EOD_RPT,              AC_EOD_RPT,              SPECIAL_FLAG_MASTER},
    {AC_EJ_RESET_RPT,         AC_EJ_RESET_RPT,         0},
    {AC_EMG_GCFL_CLS,         AC_EMG_GCFL_CLS,         SPECIAL_FLAG_MASTER},
    {AC_TARE_TABLE,           AC_TARE_TABLE,           SPECIAL_FLAG_MASTER},
    {AC_DEPT_MAINT,           AC_DEPT_MAINT,           SPECIAL_FLAG_MASTER},
    {AC_SET_PLU,              AC_SET_PLU,              SPECIAL_FLAG_MASTER},
    {AC_EMG_CASHFL_CLS,       AC_EMG_CASHFL_CLS,       SPECIAL_FLAG_MASTER},
    {AC_MAJDEPT_RPT,          AC_MAJDEPT_RPT,          SPECIAL_FLAG_MASTER},
    {AC_MEDIA_RPT,            AC_MEDIA_RPT,            0},
    {AC_TAXTBL1,              AC_TAXTBL1,              SPECIAL_FLAG_MASTER},

    {AC_TAXTBL2,              AC_TAXTBL2,              SPECIAL_FLAG_MASTER},
    {AC_TAXTBL3,              AC_TAXTBL3,              SPECIAL_FLAG_MASTER},
    {AC_TAX_RATE,             AC_TAX_RATE,             SPECIAL_FLAG_MASTER},
    {AC_MISC_PARA,            AC_MISC_PARA,            SPECIAL_FLAG_MASTER},
    {AC_TAXTBL4,              AC_TAXTBL4,              SPECIAL_FLAG_MASTER},
    {AC_PIGRULE,              AC_PIGRULE,              SPECIAL_FLAG_MASTER},
    {AC_SERVICE_READ_RPT,     AC_SERVICE_READ_RPT,     SPECIAL_FLAG_MASTER},
    {AC_SERVICE_RESET_RPT,    AC_SERVICE_RESET_RPT,    SPECIAL_FLAG_MASTER},
    {AC_SERVICE_MAINT,        AC_SERVICE_MAINT,        SPECIAL_FLAG_MASTER},
    {AC_PROG_READ_RPT,        AC_PROG_READ_RPT,        SPECIAL_FLAG_MASTER},

    {AC_PROG_RESET_RPT,       AC_PROG_RESET_RPT,       SPECIAL_FLAG_MASTER},
    {AC_DISPENSER_PARA,       AC_DISPENSER_PARA,       SPECIAL_FLAG_MASTER},
    {AC_ETK_READ_RPT,         AC_ETK_READ_RPT,         0},
    {AC_ETK_RESET_RPT,        AC_ETK_RESET_RPT,        SPECIAL_FLAG_MASTER},
    {AC_ETK_ASSIN,            AC_ETK_ASSIN,            SPECIAL_FLAG_MASTER},
    {AC_ETK_MAINT,            AC_ETK_MAINT,            SPECIAL_FLAG_MASTER},
    {AC_ETK_LABOR_COST,       AC_ETK_LABOR_COST,       SPECIAL_FLAG_MASTER},
    {AC_OEP_MAINT,            AC_OEP_MAINT,            SPECIAL_FLAG_MASTER},
    {AC_CPN_MAINT,            AC_CPN_MAINT,            SPECIAL_FLAG_MASTER},
    {AC_FLEX_DRIVE,           AC_FLEX_DRIVE,           0},

    {AC_TONE_CTL,             AC_TONE_CTL,             0},
    {AC_PLU_SLREST,           AC_PLU_SLREST,           SPECIAL_FLAG_MASTER},
    {AC_SUP_DOWN,             AC_SUP_DOWN,             0},
    {AC_CSOPN_RPT,            AC_CSOPN_RPT,            SPECIAL_FLAG_MASTER},
    {AC_BOUNDARY_AGE,         AC_BOUNDARY_AGE,         SPECIAL_FLAG_MASTER},
    {AC_IND_READ_RPT,         AC_IND_READ_RPT,         0},
    {AC_IND_RESET_RPT,        AC_IND_RESET_RPT,        SPECIAL_FLAG_MASTER},
    {AC_SET_DELAYED_BAL,      PARA_SEC_AC_SET_DELAYED_BAL, SPECIAL_FLAG_MASTER},
    {AC_STORE_FORWARD,        AC_STORE_FORWARD,        0},
    {AC_JOIN_CLUSTER,         PARA_SEC_AC_JOIN_CLUSTER,    SPECIAL_FLAG_EXCEPTION},

	// Following codes are special in that they are not provisioned with a security level
	// in PEP.  Instead they either use the security level of some other Action Code or they
	// use the default level of 0 which means any Supervisor.  Some also have special flags.
    {AC_PLU_DOWN,             0,                       0},
    {AC_SET_DELAYED_BAL_OFF,  PARA_SEC_AC_SET_DELAYED_BAL, SPECIAL_FLAG_MASTER},
    {AC_RELOADOEP,            0,                       0},
    {AC_FORCE_MAST,           AC_DISCON_MAST,          SPECIAL_FLAG_EXCEPTION},
    {AC_CONFIG_INFO,          0,                       0},
    {AC_SHUTDOWN,             0,                       0},
    {AC_SET_LAYOUT,           AC_PLU_MAINT,            0},
    {AC_PLUGROUP_READ_RPT,    0,                       SPECIAL_FLAG_MASTER}
};

SHORT   MaintEntSup( MAINTENT *pData )
{
    PARAACTCODESEC ParaActCodeSec;

    /* Check W/o Amount Case */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* W/o Amount Input Case */
        return(LDT_SEQERR_ADR);
    }

	ParaActCodeSec.usAddress = 0;  // initialize.  default is to indicate no security check needed.

	{
		int iLoop;

		for (iLoop = 0; iLoop < sizeof(mySecurityTable)/sizeof(mySecurityTable[0]); iLoop++) {
			if (mySecurityTable[iLoop].usActionCode == pData->usSuperNo) {
				ParaActCodeSec.usAddress = mySecurityTable[iLoop].usSecurityAddress;

				if (mySecurityTable[iLoop].usSpecialFlags != 0) {
					if ((mySecurityTable[iLoop].usSpecialFlags & SPECIAL_FLAG_MASTER) != 0 && CliCheckAsMaster() != STUB_SUCCESS) {
						return(LDT_PROHBT_ADR);
					}

					if ((mySecurityTable[iLoop].usSpecialFlags & SPECIAL_FLAG_EXCEPTION) != 0) {
						// This particular Action Code has some exceptions that need to be checked.
						// We will do the specific check by jumping to a specific case for the code.
						switch (mySecurityTable[iLoop].usActionCode) {
							case AC_FORCE_MAST:
							case AC_DISCON_MAST:
								if (CliReadUAddr() > 2) {               /* Not Master or Backup Master */         
									return(LDT_PROHBT_ADR);                         
								}                              
								break;  

							case AC_SUPSEC_NO:
								if (usMaintSupNumberInternal == 800) {          /* Entry Supervisor Mode W/o  No. */
									return(LDT_PROHBT_ADR);                         
								}
								break;

							case AC_JOIN_CLUSTER:
								if (CstIamDisconnectedSatellite() != STUB_SUCCESS) {
									return(LDT_PROHBT_ADR);
								}
								break;

							default:
								break;
						}
					}
				}
				break;
			}
		}

		if (iLoop >= sizeof(mySecurityTable)/sizeof(mySecurityTable[0])) {
			return(LDT_KEYOVER_ADR);    /* Wrong Data */
		}
	}

    /* Check File Exist or Not */
    switch(pData->usSuperNo) {
    case AC_GCNO_ASSIN:                 
    case AC_GCFL_RESET_RPT:      
    case AC_EMG_GCFL_CLS:        
        if (RptEODChkFileExist(FLEX_GC_ADR) == RPT_FILE_NOTEXIST) { /* File not Exist */
			NHPOS_NONASSERT_NOTE("==NOTE - LDT_FILE_EMPTY", "==NOTE: RptEODChkFileExist(FLEX_GC_ADR) == RPT_FILE_NOTEXIST");
			return(LDT_FILE_EMPTY);
        }
        break;

    case AC_CASH_RESET_RPT:      
    case AC_CASH_ASSIN:          
    case AC_CASH_READ_RPT:       
    case AC_EMG_CASHFL_CLS:      
        if (RptEODChkFileExist(FLEX_CAS_ADR) == RPT_FILE_NOTEXIST) { /* File not Exist */
			NHPOS_NONASSERT_NOTE("==NOTE - LDT_FILE_EMPTY", "==NOTE: RptEODChkFileExist(FLEX_CAS_ADR) == RPT_FILE_NOTEXIST");
			return(LDT_FILE_EMPTY);
        }
        break;

    case AC_PLUSALE_READ_RPT:    
    case AC_PLU_RESET_RPT:       
    case AC_PLU_MAINT:           
    case AC_PLU_PRICE_CHG:       
/*     case AC_PLU_DOWN:        Saratoga */
    case AC_PLU_ADDDEL:          
    case AC_PLUPARA_READ_RPT:    
    case AC_PLU_MNEMO_CHG:       
		if (RptEODChkFileExist(FLEX_PLU_ADR) == RPT_FILE_NOTEXIST) {
			NHPOS_NONASSERT_NOTE("==NOTE - LDT_FILE_EMPTY", "==NOTE: RptEODChkFileExist(FLEX_PLU_ADR) == RPT_FILE_NOTEXIST");
			return(LDT_FILE_EMPTY);
		}
        if (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_NOTEXIST) { /* File not Exist */
			NHPOS_NONASSERT_NOTE("==NOTE - LDT_FILE_EMPTY", "==NOTE: RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_NOTEXIST");
			return(LDT_FILE_EMPTY);
        }
        break;

    case AC_CPN_MAINT:           
    case AC_CPN_READ_RPT:       
    case AC_CPN_RESET_RPT:          
        if (RptEODChkFileExist(FLEX_CPN_ADR) == RPT_FILE_NOTEXIST){
			NHPOS_NONASSERT_NOTE("==NOTE - LDT_FILE_EMPTY", "==NOTE: RptEODChkFileExist(FLEX_CPN_ADR) == RPT_FILE_NOTEXIST");
			return(LDT_FILE_EMPTY);
        }
        break;

    case AC_CSTR_MAINT:          
        if (RptEODChkFileExist(FLEX_CSTRING_ADR) == RPT_FILE_NOTEXIST){
			NHPOS_NONASSERT_NOTE("==NOTE - LDT_FILE_EMPTY", "==NOTE: RptEODChkFileExist(FLEX_CSTRING_ADR) == RPT_FILE_NOTEXIST");
			return(LDT_FILE_EMPTY);
        }
        break;

    case AC_DEPTSALE_READ_RPT:   
    case AC_DEPT_MAINT:          
    case AC_MAJDEPT_RPT:         
        if (RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_NOTEXIST) { /* File not Exist */
			NHPOS_NONASSERT_NOTE("==NOTE - LDT_FILE_EMPTY", "==NOTE: RptEODChkFileExist(FLEX_DEPT_ADR) == RPT_FILE_NOTEXIST");
			return(LDT_FILE_EMPTY);
        }
        break;

    case AC_EJ_READ_RPT:      
    case AC_EJ_RESET_RPT:          
        if (RptEODChkFileExist(FLEX_EJ_ADR) == RPT_FILE_NOTEXIST) {     /* File not Exist */
			NHPOS_NONASSERT_NOTE("==NOTE - LDT_FILE_EMPTY", "==NOTE: RptEODChkFileExist(FLEX_EJ_ADR) == RPT_FILE_NOTEXIST");
			return(LDT_FILE_EMPTY);
        }
        break;
    case AC_ETK_RESET_RPT:
    case AC_ETK_ASSIN:
    case AC_ETK_MAINT:
    case AC_ETK_LABOR_COST:
        if (RptEODChkFileExist(FLEX_ETK_ADR) == RPT_FILE_NOTEXIST) {     /* File not Exist */
			NHPOS_NONASSERT_NOTE("==NOTE - LDT_FILE_EMPTY", "==NOTE: RptEODChkFileExist(FLEX_ETK_ADR) == RPT_FILE_NOTEXIST");
			return(LDT_FILE_EMPTY);
        }
        break;

    case AC_PPI_MAINT:           
        if (RptEODChkFileExist(FLEX_PPI_ADR) == RPT_FILE_NOTEXIST) {     /* File not Exist */
			NHPOS_NONASSERT_NOTE("==NOTE - LDT_FILE_EMPTY", "==NOTE: RptEODChkFileExist(FLEX_PPI_ADR) == RPT_FILE_NOTEXIST");
			return(LDT_FILE_EMPTY);
        }
        break;

    case AC_SERVICE_READ_RPT:       /* Only Store/Recall System */
    case AC_SERVICE_RESET_RPT:
        if (RptPTDChkFileExist(FLEX_GC_ADR) == RPT_FILE_NOTEXIST) {     /* File not Exist */
			NHPOS_NONASSERT_NOTE("==NOTE - LDT_FILE_EMPTY", "==NOTE: RptPTDChkFileExist(FLEX_GC_ADR) == RPT_FILE_NOTEXIST");
            return(LDT_FILE_EMPTY);
        }
        break;

    case AC_PROG_READ_RPT:
    case AC_PROG_RESET_RPT:
        if (RptEODChkFileExist(FLEX_PROGRPT_ADR) == RPT_FILE_NOTEXIST) {     /* File not Exist */
			NHPOS_NONASSERT_NOTE("==NOTE - LDT_FILE_EMPTY", "==NOTE: RptEODChkFileExist(FLEX_PROGRPT_ADR) == RPT_FILE_NOTEXIST");
			return(LDT_FILE_EMPTY);
        }
        break;

    case AC_GCFL_READ_RPT: //Removed for US CUSTOMS SCER for Rel 2.x JHHJ 7-25-05        
    case AC_ETK_READ_RPT:  //Removed for US CUSTOMS SCER for Rel 2.x JHHJ 7-25-05        
    default:
        break;

    }

	if (ParaActCodeSec.usAddress > 0) {
		/* Set Major Class */
		ParaActCodeSec.uchMajorClass = CLASS_PARAACTCODESEC;

		CliParaRead(&ParaActCodeSec);   /* call ParaActCodeSecRead() */

		/* check supervisor level */
		ParaActCodeSec.uchSecurityBitPatern >>= usMaintSupLevel;
    
		/* check odd/even number */
		if ((ParaActCodeSec.usAddress & 0x0001) == 0) {
			ParaActCodeSec.uchSecurityBitPatern &= 0x01;
		} else {  /* super number is odd number */
			ParaActCodeSec.uchSecurityBitPatern &= 0x10;
		}

		/* check action code security */
		if (ParaActCodeSec.uchSecurityBitPatern != 0x00) {  /* action code operation is permitted */
			NHPOS_NONASSERT_NOTE("==PROVISIONING", "==PROVISIONING: LDT_PROHBT_ADR - Supervisor level insufficient.");
			return(LDT_PROHBT_ADR);
		}
	}

    /* clear maint and report module self-work */
    MaintInit();    
    RptInitialize();
    usMaintACNumber = pData->usSuperNo;                 /* Saratoga */

    return(SUCCESS);

}

/*
*===========================================================================
** Synopsis:    SHORT MaintEntPrg( MAINTENT *pData )
*               
*     Input:    *pData
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      PERFORM: permit operarion
*               LDT_PROHBT_ADR: prohibit operation
*
** Description: This function checks program number.
*===========================================================================
*/

SHORT MaintEntPrg( MAINTENT *pData )
{

    /* Check W/o Amount Case */

    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                /* W/o Amount Input Case */
        return(LDT_SEQERR_ADR);
    }

   /* Check As Master Function */

    switch(pData->usSuperNo) {
    case PG_FLX_MEM:           
    case PG_SEC_PROG:          
    case PG_SUPLEVEL:          
    case PG_ACT_SEC:           
    case PG_TRAN_HALO:         
    case PG_PRESET_AMT:         
    case PG_HOURLY_TIME:       
    case PG_SLIP_CTL:          
    case PG_TRAN_MNEMO:        
    case PG_LEAD_MNEMO:        
    case PG_RPT_NAME:          
    case PG_SPEC_MNEMO:        
    case PG_PCIF:
    case PG_ADJ_MNEMO:         
    case PG_PRT_MNEMO:         
    case PG_MAJDEPT_MNEMO:     
    case PG_AUTO_KITCH:        
    case PG_CHA24_MNEMO:       
    case PG_TTLKEY_TYP:        
    case PG_TEND_PARA:  /* V3.3 */
    case PG_MLD_MNEMO:  /* V3.3 */
    case PG_KDS_IP:     /* 2172 */
	case PG_AUTO_CPN:
        if (CliCheckAsMaster() != STUB_SUCCESS) {           
            return(LDT_PROHBT_ADR);                         
        }                                     
        break; 

    case PG_MDC:               
    case PG_FSC:               
    case PG_STOREG_NO:         
    case PG_TTLKEY_CTL:  
    case PG_SHR_PRT:
    case PG_HOTELID:
    case PG_UNLOCK_NO:
    case PG_HOSTS_IP:   /* 2172 */
        break;

    default:
        return(LDT_KEYOVER_ADR);  /* Wrong Data */

    }

    /* clear Maintenance and report self-work */

    MaintInit();    
    RptInitialize();

    return(SUCCESS);

}

/****** End of Source ******/