/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-1999       **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, STUB Source File                        
* Category    : Client/Server STUB, US Hospitality Model                                                 
* Program Name: CSSTBSTB.C                                            
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               CliInitialize()      C  Initialize CLIENT/SERVER STUB
*               CliReadUaddr()          Read Unique Address
*               CliCheckAsMaster()      Check as master or not
*               CliCreateFile()      C  Create Files
*               CliCheckCreateFile() C  Check and create files
*               CliCheckDeleteFile() C  Check and delete files
*               CliDispDescriptor()     Descriptor control
*               
*               CstReqOpePara()         Ope. Parameter (send message) handl.
*               SstReqBackUp()          Backup (send message) handling
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* May-07-92:H.Nakashima: initial                                   
* Jun-04-93:H.Yamaguchi: Modified same as HP Int'l
* Apr-07-94:K.You      : Add Flex GC File Feature
*          :           : (Mod. CliCreateFile, CliCheckCreateFile,
*          :           : CliCheckDeleteFile, CliInitialize).
*          :           : (add SstReqBackUpFH). 
* Jan-17-95:M.Ozawa    : Modify CliInitialize() for Display on the fly
* Feb-28-95:H.Ishida   : Add FLEX_CSTRING_ADR,FLEX_CPN_ADR process
* Apl-06-95:M.Ozawa    : Add Multiline Display Storage File Creation
* Apl-17-95:hkato      : Add FDT File Creation
* Dec-01-95:hkato      : R3.1 Create Split Check File.
* Jan-08-96:T.Nakahata : R3.1 Initial
*   Add New Total Files (Service Time Total and Individual Financial)
*   Add Programmable Report File
* Feb-09-95:M.Ozawa    : Distingush FDT File and Split Check File Creation by
*                       Guest Check Sytem Parameter
* Apl-22-96:M.Ozawa    : Add PrtInitialize() for split check control.
* Apr-26-95:T.Nakahata : GusInit is called after TtlInit
* Aug-11-99:M.Teraki   : R3.5 remove WAITER_MODEL
* Dec-07-99:hrkato     : Saratoga
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
#include	<tchar.h>
#include    <math.h>
#include    <stdio.h>
#include    <memory.h>
#include	<string.h>

#include    <ecr.h>
#include    <pif.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <rfl.h>
#include    <log.h>
#include    <appllog.h>
#include    <paraequ.h>
#include    <para.h>
#include    <cscas.h>
#include    <cswai.h>
#include    <csgcs.h>
#include    <csttl.h>
#include    <csop.h>
#include    <cskp.h>
#include    <nb.h>
#include    <uie.h>
#include    <uic.h>
#include    <isp.h>
#include    <csstbfcc.h>
#include    <csstbstb.h>
#include    <csstbopr.h>
#include    <csserm.h>
#include    <ej.h>
#include    <csetk.h>
#include    <shr.h>
#include    <dfl.h>
#include    <prt.h>
#include    <mld.h>
#include    <fdt.h>
#include    <dif.h>
#include    <csprgrpt.h>
#include    <enhkds.h>
#include    <ttl.h>
#include    "csstubin.h"
#include	<rmt.h>
#include	<pararam.h>
#include	<BlFWif.h>
//#include	"DFPRExports.h"
#include	<csstbpar.h>

XGRAM           CliSndBuf;              /* Send message buffer */       
XGRAM           CliRcvBuf;              /* Receive message buffer */
CLICOMIF        CliMsg;                 /* common message structure */

PifSemHandle    husCliWaitDone = PIF_SEM_INVALID_HANDLE;         /* semapho, STUB - SERVER */
PifSemHandle    husCliExeNet = PIF_SEM_INVALID_HANDLE;           /* semapho, APPLI - ISP */
PifSemHandle    husCliExePara = PIF_SEM_INVALID_HANDLE;          /* Semapho, ISP - APPL, para */
PifSemHandle    husCliComDesc = PIF_SEM_INVALID_HANDLE;          /* Semapho, ISP - APPL, descriptor */
CLINETCONFIG    CliNetConfig;           /* NET control structure */
USHORT          fsCliComDesc;           /* Descriptor control */
USHORT          usCliCpmReqNo;          /* CPM Request counter */
CLIPLUSAVHAN    CliMMMHand;             /* Save MMM handel, Saratoga */
CLIPLUSAVHAN    SerMMMHand;             /* Save handles,    Saratoga */


/*
*===========================================================================
** Synopsis:    VOID    CstInitialize(VOID);
*
** Return:      none.
*
** Description: This function initilizes STUB function.
*===========================================================================
*/
VOID    CstInitialize(VOID)
{
    CstNetOpen();
    husCliWaitDone  = PifCreateSem(0);
    husCliExeNet    = PifCreateSem(1);
	PifSetTimeoutSem(husCliExeNet, 5000);
    husCliExePara   = PifCreateSem(1);
    husCliComDesc   = PifCreateSem(1);
    CliDispDescriptor(CLI_MODULE_STUB, CLI_DESC_OFF);
}

/*
*===========================================================================
** Synopsis:    VOID    CliInitialize(USHORT usMode);
*
*     Input:    usMode - Power Up Mode
*
** Return:      none.      
*
** Description: This function initializes all CLIENT/SERVER STUB functions.
*===========================================================================
*/
VOID    CliInitialize(USHORT usMode)
{
	{
		char xBuff[128];
		sprintf (xBuff, "==STARTUP STATE: CliInitialize() Start. usMode = %d  0x%x", usMode, usMode);
		NHPOS_NONASSERT_NOTE("==STATE", xBuff);
	}
    CstInitialize();                    /* STUB initialize */
    OpInit();                           /* Op. Para initialize */
    NbInit();                           /* Notice Board initialize */
    UicInit();                          /* User Interface Control init */
    /* ItemMiscCreatePostFile(); */     /* Create Post File */
#ifdef DEMO_VERSION
    SstInitialize(POWER_UP_CLEAR_TOTAL);              /* Server module initialize */
#else
    SstInitialize(usMode);              /* Server module initialize */
#endif

    if (CLI_IAM_MASTER || CLI_IAM_BMASTER || CLI_IAM_DISCONSAT) {
		NHPOS_NONASSERT_NOTE("==STATE", "     CliInitialize() Master/Backup/DisconSat.");
        TtlTUMRestoreTtlData();
        CasInit();                      /* Cashier initialize */
        TtlBaseCreate();                /* base total creation */
        /* ===== Add Prog Report (Release 3.1) BEGIN ===== */
        ProgRptInitialize();            /* Programmable Report Init,      R3.1 */
        /* ===== Add Prog Report (Release 3.1) END ===== */
        /* TtlTtlUpdCreate();           update file creation */
#ifdef DEMO_VERSION
        TtlInitial(POWER_UP_CLEAR_TOTAL);             /* Total Initialize */
#else
        TtlInitial(usMode);             /* Total Initialize */
#endif
        /******* following function is called after TtlInitial *****/
        /******* because of CREATE_TTL mode (4/26/96)      ****/
#ifdef DEMO_VERSION
        GusInitialize(POWER_UP_CLEAR_TOTAL);          /* GCF initialize */
#else
        GusInitialize(usMode);          /* GCF initialize */
#endif
        /* ===== Add Individual Financial (Release 3.1) BEGIN ===== */
        TtlIndFinCheckDelete();         /* Individual Financial Deletion, R3.1*/
        TtlIndFinCreate();              /* Individual Financial Creation, R3.1*/
        /* ===== Add Individual Financial (Release 3.1) END ===== */
        /******* 4/26/96 these function are called after TtlInitial ****/
        EtkInit();                      /* ETK initialize  */
        DflInit();                      /* DFL initialize  */
//RJC        CpmInit();                      /* Cpm initialize  */
    }
#ifdef DEMO_VERSION
	else{
		return;
	}
#endif

    KpsInit();                          /* KPS initialize,  Saratoga */
    TrnInit();                          /* Transaction module initialize, V3.3 */
    PrtInitialize();                    /* Print module initialize R3.1 */
    ShrInit();                          /* Shr initialize */
    EJInit();                           /* EJ initialize  */
    IspInit();                          /* ISP server initialize */
#ifdef DEMO_VERSION
    FDTInit(POWER_UP_CLEAR_TOTAL);                    /* FDT initialize,  R3.0 */
#else
    FDTInit(usMode);                    /* FDT initialize,  R3.0 */
#endif
    KdsInit();                          /* Enhanced KDS */

	NHPOS_NONASSERT_NOTE("==STATE", "==STARTUP STATE:  CliInitialize() completed.");

	CliParaSaveFarData();    // BlFinalizeAndClose() update memory resident data snapshot and the ASSRTLOG file offset pointers.
}

/*
*===========================================================================
** Synopsis:    UCHAR   CliReadUAddr(VOID);
*
** Return:      Unique Address
*
** Description: This function reads unique address of this terminal folded into
*               the standard terminal address range of 1 through 16.  If the
*               terminal number is greater than 16 then we fold it back into
*               terminal number 1, the Master Terminal.  This is used with the
*               Disconnected Satellite functionality in order to make a
*               Disconnected Satellite look like a standard terminal in all the
*               various places where things depend on a valid terminal number
*               such as the number of windows and other places.
*===========================================================================
*/
UCHAR   CliReadUAddr(VOID)
{
	UCHAR  uchTerminalPosition = 1;
#if 0
	uchTerminalPosition = CliNetConfig.auchFaddr[CLI_POS_UA];
#else
	uchTerminalPosition = PIF_CLUSTER_MASK_TERMINAL_NO(PifSysConfig()->usTerminalPositionFromName);
#endif

	if (uchTerminalPosition < 1 || uchTerminalPosition > PIF_NET_MAX_IP)
		uchTerminalPosition = 1;

    return uchTerminalPosition;
}


/*
*===========================================================================
** Synopsis:    UCHAR   CliReadUAddrReal(VOID);
*
** Return:      Unique Address
*
** Description: This function reads unique address of this terminal.
*               This is the real terminal number that can be used to detect if
*               a terminal is a Disconnected Satellite since they will have
*               terminal numbers greater than 16.
*===========================================================================
*/
UCHAR   CliReadUAddrReal(VOID)
{
	UCHAR  uchTerminalPosition = CliNetConfig.auchFaddr[CLI_POS_UA];

	if (uchTerminalPosition < 1 || uchTerminalPosition > PIF_NET_MAX_IP)
		uchTerminalPosition = 1;

    return uchTerminalPosition;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliCheckAsMaster(VOID);
*
** Return:      STUB_SUCCESS:       I am as master
*               STUB_NOT_MASTER:    I am satellite or back up master
*               STUB_ILLEGAL:       I am out of date
*
** Description:
*   This function checks whether I am as master or not.
*
*               MT      BM          MT          BM          ST
*               ---     ---    =>   ---------   ----------  -----------
*               U/D     U/D         SUCCESS     NOT_MASTER  NOT_MASTER
*               U/D     O/D         SUCCESS     ILLEGAL     NOT_MASTER
*               O/D     U/D         ILLEGAL     SUCCESS     NOT_MASTER
*               U/D     none.       SUCCESS     NOT_MASTER  NOT_MASTER
*
*===========================================================================
*/
SHORT   CliCheckAsMaster(VOID)
{
    USHORT  fsComStatus;

    if (CLI_IAM_MASTER) {
        fsComStatus = CstComReadStatus();
        if (fsComStatus & CLI_STS_INQUIRY) {
            return STUB_ILLEGAL;
        }
        if (fsComStatus & CLI_STS_M_UPDATE) {
            return STUB_SUCCESS;
        } else {
            return STUB_ILLEGAL;
        }
    }
    if (CLI_IAM_BMASTER) {
        fsComStatus = CstComReadStatus();
        if (fsComStatus & CLI_STS_INQUIRY) {
            return STUB_ILLEGAL;
        }
        if (0 == (fsComStatus & CLI_STS_BM_UPDATE)) {
            return STUB_ILLEGAL;
        }
        if (0 == (fsComStatus & CLI_STS_M_UPDATE)) {
            return STUB_SUCCESS;
        }
    }
    return STUB_NOT_MASTER;
}


/*
*===========================================================================
** Synopsis:    SHORT   CliCreateFile(UCHAR uchType,
*                                       ULONG ulSize,
*                                       UCHAR uchPTD);
*     Input:    uchType - what kind of file you want to create
*               usSize  - size of file, number of person
*               uchPTD    - with PTD/ GCF type
*
** Return:      0:              successful creation
*               other:          error      
*
** Description: This function creates specified files.              Saratoga
*===========================================================================
*/
SHORT   CliCreateFile(UCHAR uchType, ULONG ulSize, UCHAR uchPTD)
{
    SHORT           sError = SUCCESS;

    if (STUB_NOT_MASTER == CliCheckAsMaster() && ! CLI_IAM_DISCONSAT) {
                                            /*=== SATELLITE TERMINAL ===*/
        switch(uchType) {
        case FLEX_DEPT_ADR:                 /* dept file creation */
            sError = OpDeptCreatFile((USHORT)ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_DEPT_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            break;

        case FLEX_PLU_ADR:                  /* PLU file creation */
			{
				USHORT  usI = 0;
				for (usI = 0; usI < 4; usI++) {
					sError = OpOepCreatFile(usI, ulSize, 0);   /* only oep file, Saratoga */
					if (sError != 0) {
						PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_PLU_ADR);
						PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
						PifLog(MODULE_DATA_VALUE(MODULE_STB_ABORT), usI);
					}
				}
			}
            break;

        case FLEX_EJ_ADR:                   /* EJ creation */
            sError = EJCreat((USHORT)ulSize, uchPTD);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_EJ_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            break;

        case FLEX_ITEMSTORAGE_ADR:          /* item storage creation */
            sError = KpsCreateFile((USHORT)ulSize); /* satellite remote printer, sarataga */
            if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_ITEMSTORAGE_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
			if (sError == LDT_PROHBT_ADR) {
                return sError;
            } else if (sError == KPS_PERFORM) {
                if ((sError = TrnCreateFile((USHORT)ulSize, FLEX_ITEMSTORAGE_ADR)) == TRN_SUCCESS) {
                    return sError;
                }
            }
            sError = LDT_FLFUL_ADR;
            break;

        case FLEX_CONSSTORAGE_ADR:          /* cons. storage creation */
            sError = SerCreateTmpFile((USHORT)ulSize);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_CONSSTORAGE_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
				PifLog(MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)(__LINE__));
			}
            sError = TrnCreateFile((USHORT)ulSize, FLEX_CONSSTORAGE_ADR);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_CONSSTORAGE_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
				PifLog(MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)(__LINE__));
			}
            if (sError == LDT_PROHBT_ADR) {
                return sError;
            } else if (sError == TRN_SUCCESS) { /* multiline display file creation */
                sError = MldCreateFile((USHORT)ulSize, FLEX_CONSSTORAGE_ADR);
                if (sError == LDT_PROHBT_ADR) {
                    return sError;
                }

				// determine the System Type and create the necessary transaction data
				// storage files used by the various System Type configurations.
				// address FLEX_GC_ADR indicates the System Type and address FLEX_CONSSTORAGE_ADR
				// indicates the max number of items a guest check can contain.
				// These are all temporary files to hold transaction data when entering the transaction.
				switch (RflGetSystemType ()) {
				case FLEX_POST_CHK:                             // CliCreateFile() for Satellite Terminal
                    FDTChkAndDelFile(0);                        /* no flexible drive thru so delete FDT File if exist */
                    sError = TrnCreateFileSpl((USHORT)ulSize);  /* create split check file */
					break;

				case FLEX_STORE_RECALL:                         // CliCreateFile() for Satellite Terminal
                    TrnChkAndDelFileSpl(0);                     /* no split checks so delete Split check File if exist */
                    sError = FDTCreateFile((USHORT)ulSize);     /* create flexible drive thru file */
					break;

				default:
                    FDTChkAndDelFile(0);                /* no flexible drive thru so delete FDT File if exist */
                    TrnChkAndDelFileSpl(0);             /* no split checks so delete Split check File if exist */
                    sError = SUCCESS;
					break;
				}
            }
            break;

        case FLEX_WT_ADR:                   /* Waiter creation */
        case FLEX_CAS_ADR:                  /* Cashier creation */
        case FLEX_GC_ADR:                   /* GCF creation */
        case FLEX_ETK_ADR:                  /* ETK creation */
        /* ===== Add Programmable Report Creation (Release 3.1) BEGIN ===== */
        case FLEX_PROGRPT_ADR:              /* Programmable Report, R3.1 */
        /* ===== Add Programmable Report Creation (Release 3.1) END ===== */
            sError = STUB_SUCCESS;
            break;

        case FLEX_CPN_ADR:                 /* Combination Coupon file creation */
            sError = OpCpnCreatFile((USHORT)ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_CPN_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            break;

        case FLEX_CSTRING_ADR:              /* Control String Size */
            sError = OpCstrCreatFile((USHORT)ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_CSTRING_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            break;

        /* ===== Add PPI File Creation (Release 3.1) BEGIN ===== */
        case FLEX_PPI_ADR:                  /* Promotional Pricing Item, R3.1 */
            sError = OpPpiCreatFile((USHORT)ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_PPI_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            break;
        /* ===== Add PPI File Creation (Release 3.1) END ===== */

        case FLEX_MLD_ADR:                  /* Mld Mnemonics File, V3.3 */
            sError = OpMldCreatFile((USHORT)ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_MLD_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
				PifLog (MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)__LINE__);
			}
            sError = OpMnemonicsFileCreate(OPMNEMONICFILE_ID_REASON, (USHORT)ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_MLD_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
				PifLog (MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)__LINE__);
			}
            break;

        default:                            /* not use */
            PifAbort(MODULE_STB_ABORT, FAULT_INVALID_ARGUMENTS);
            break;
        }

    } else {                                /*=== MASTER or BM ===*/
        switch(uchType) {
        case FLEX_DEPT_ADR:                 /* dept file creation */
            sError = OpDeptCreatFile((USHORT)ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_DEPT_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
				PifLog (MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)__LINE__);
			}
            if (OP_PERFORM == sError) {
                sError = TtlDeptCreate((USHORT)ulSize, uchPTD, TTL_COMP_CREATE);
				if (sError != 0) {
					PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_DEPT_ADR);
					PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
					PifLog (MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)__LINE__);
				}
            }
            break;

        case FLEX_PLU_ADR:                  /* PLU file creation */                       
            sError = OpPluCreatFile(ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_PLU_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
				PifLog (MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)__LINE__);
			}
            if (OP_PERFORM == sError) {
                sError = TtlPLUCreate(ulSize, uchPTD, TTL_COMP_CREATE);
				if (sError != 0) {
					PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_PLU_ADR);
					PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
					PifLog (MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)__LINE__);
				}
            }
            break;

        case FLEX_WT_ADR:                   /* Waiter file creation */
            if (ulSize == 0L) {
                sError = WAI_PERFORM;
            } else {
                sError = WAI_FILE_NOT_FOUND;
            }
            break;
                                            
        case FLEX_CAS_ADR:                  /* Cashier File creation */
            sError = CasCreatFile((USHORT)ulSize);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_CAS_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
				PifLog (MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)__LINE__);
			}
            if (CAS_PERFORM == sError) {
                sError = TtlCasCreate((USHORT)ulSize, uchPTD, TTL_COMP_CREATE); /* V3.3 */
				if (sError != 0) {
					PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_CAS_ADR);
					PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
					PifLog (MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)__LINE__);
				}
            }
            break;
                                            
        case FLEX_GC_ADR:                   /* GCF creation */
            sError = GusCreateGCF((USHORT)ulSize, uchPTD);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_GC_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            /* ===== Add Service Time File Creation (Release 3.1) BEGIN ===== */
            if (GCF_SUCCESS == sError) {
				USHORT          usRecord;

                if (FLEX_STORE_RECALL == uchPTD) {
                    sError = TtlSerTimeCreate(TTL_COMP_CREATE);
					if (sError < 0) {
						PifLog(MODULE_UI, LOG_ERROR_UIC_TF_TOTAL_FILE);
						PifLog(MODULE_DATA_VALUE(MODULE_UI), uchType);
						PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
						PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
					}
                } else {
                    sError = TtlSerTimeDeleteFile();
					if (sError < 0) {
						PifLog(MODULE_UI, LOG_ERROR_UIC_TF_TOTAL_FILE);
						PifLog(MODULE_DATA_VALUE(MODULE_UI), uchType);
						PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
						PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
					}
                }

                usRecord = (USHORT)RflGetMaxRecordNumberByType(FLEX_CONSSTORAGE_ADR);
				// determine the System Type and create the necessary transaction data
				// storage files used by the various System Type configurations.
				switch (uchPTD) {
				case FLEX_POST_CHK:
                    FDTChkAndDelFile(0);                /* delete FDT File */
                    sError = TrnCreateFileSpl(usRecord);    /* Saratoga */
					break;
				case FLEX_STORE_RECALL:
                    TrnChkAndDelFileSpl(0);             /* delete Split check File */
                    sError = FDTCreateFile(usRecord);   /* Saratoga */
					break;
				default:
                    FDTChkAndDelFile(0);                /* delete FDT File */
                    TrnChkAndDelFileSpl(0);             /* delete Split check File */
                    sError = SUCCESS;
					break;
				}
            }
            /* ===== Add Service Time File Creation (Release 3.1) END ===== */
            break;

        case FLEX_EJ_ADR:                   /* EJ creation */
            sError = EJCreat((USHORT)ulSize, uchPTD);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_EJ_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            break;

        case FLEX_ETK_ADR:                  /* ETK creation */
            sError = EtkCreatFile((USHORT)ulSize);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_ETK_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            break;

        case FLEX_ITEMSTORAGE_ADR:          /* item storage creation */
            sError = TtlTtlUpdCreate((USHORT)ulSize);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_ITEMSTORAGE_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            if (sError == LDT_PROHBT_ADR) {
                return sError;

            } else if (sError == TTL_SUCCESS) {
                sError = KpsCreateFile((USHORT)ulSize);
                if (sError == LDT_PROHBT_ADR) {
                    return sError;

                } else if (sError == KPS_PERFORM) {
                    if ((sError = TrnCreateFile((USHORT)ulSize, FLEX_ITEMSTORAGE_ADR)) == TRN_SUCCESS) {
                        return sError;
                    }
                }
            }
            sError = LDT_FLFUL_ADR;
            break;

        case FLEX_CONSSTORAGE_ADR:          /* cons. storage creation */
            sError = SerCreateTmpFile((USHORT)ulSize);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_CONSSTORAGE_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
				PifLog (MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)__LINE__);
			}
            if (sError == LDT_PROHBT_ADR) {
                return sError;
            } else if (sError == SERV_PERFORM) {
                sError = TrnCreateFile((USHORT)ulSize, FLEX_CONSSTORAGE_ADR);
                if (sError != TRN_SUCCESS) {
                    return sError;
                } else {                /* multiline display file creation */
                    sError = MldCreateFile((USHORT)ulSize, FLEX_CONSSTORAGE_ADR);
                    if (sError != MLD_SUCCESS) {
                        return sError;
                    }

					// determine the System Type and create the necessary transaction data
					// storage files used by the various System Type configurations.
					switch (RflGetSystemType ()) {
					case FLEX_POST_CHK:                     // CliCreateFile() for Master Terminal
                        FDTChkAndDelFile(0);                /* delete FDT File */
                        sError = TrnCreateFileSpl((USHORT)ulSize); /* R3.1 */
						break;
					case FLEX_STORE_RECALL:                 // CliCreateFile() for Master Terminal
                        TrnChkAndDelFileSpl(0);             /* delete Split check File */
                        sError = FDTCreateFile((USHORT)ulSize);    /* R3.0 */
						break;
					default:
                        FDTChkAndDelFile(0);                /* delete FDT File */
                        TrnChkAndDelFileSpl(0);             /* delete Split check File */
                        sError = SUCCESS;
						break;
					}
                }
            }
            break;

        case FLEX_CPN_ADR:                 /* Combination Coupon file creation */
            sError = OpCpnCreatFile((USHORT)ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_CPN_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            if (OP_PERFORM == sError) {
                sError = TtlCpnCreate((USHORT)ulSize, uchPTD, TTL_COMP_CREATE);
            }
            break;

        case FLEX_CSTRING_ADR:              /* Control String Size */
            sError = OpCstrCreatFile((USHORT)ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_CSTRING_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            break;

        /* ===== Add Programmable Report Creation (Release 3.1) BEGIN ===== */
        case FLEX_PROGRPT_ADR:              /* Programmable Report, R3.1 */
            sError = ProgRptCreate((UCHAR)ulSize);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_PROGRPT_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            break;
        /* ===== Add Programmable Report Creation (Release 3.1) END ===== */

        /* ===== Add PPI File Creation (Release 3.1) BEGIN ===== */
        case FLEX_PPI_ADR:                  /* Promotional Pricing Item, R3.1 */
            sError = OpPpiCreatFile((USHORT)ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_PPI_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            break;
        /* ===== Add PPI File Creation (Release 3.1) END ===== */

        case FLEX_MLD_ADR:                  /* MLD Mnemonics File, V3.3 */
            sError = OpMldCreatFile((USHORT)ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_MLD_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            sError = OpMnemonicsFileCreate(OPMNEMONICFILE_ID_REASON, (USHORT)ulSize, 0);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_MLD_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
			}
            break;

        default:                            /* not use */
            PifAbort(MODULE_STB_ABORT, FAULT_INVALID_ARGUMENTS);
            break;
        }
    }
    return sError;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliCheckCreateFile(UCHAR uchType,
*                                          ULONG ulSize,
*                                          UCHAR uchPTD);
*     Input:    uchType - what kind of file you want to create
*               usSize  - size of file, number of person
*               uchPTD    - with PTD/ GCF type
*
** Return:      0:              successful creation
*               other:          error      
*
** Description:
*   This function creates specified files if the file is not exist.
*   This function should be called at Power Up after loading parameters
*   from the Loader B/D. Furthermore, the function 'CliCheckDeleteFile'
*   must be called before executing this function.              Saratoga
*===========================================================================
*/
SHORT   CliCheckCreateFile(UCHAR uchType, ULONG ulSize, UCHAR uchPTD)
{
    SHORT           sError = SUCCESS;
	USHORT			usSize = (USHORT)ulSize;                /* Saratoga */

    if (CLI_IAM_SATELLITE) {    /* read satellite status only terminal no, saratoga */
        switch(uchType) {
        case FLEX_DEPT_ADR:                 /* dept file creation */
            sError = OpDeptCheckAndCreatFile(usSize, 0);
            break;

        case FLEX_PLU_ADR:                  /* PLU file creation */
			{
				USHORT  usI = 0;
				for (usI = 0; usI < 4; usI++) {
					sError = OpOepCheckAndCreatFile(usI, ulSize, 0);   /* only oep file, Saratoga */
					if (sError != 0) {
						PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_PLU_ADR);
						PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
						PifLog(MODULE_DATA_VALUE(MODULE_STB_ABORT), usI);
					}
				}
			}
            /* sError = OpPluCheckAndCreatFile(ulSize, 0);   / Saratoga */
            break;

        case FLEX_EJ_ADR:                   /* EJ creation */
            sError = EJCheckC(usSize, uchPTD);
            break;

        case FLEX_ITEMSTORAGE_ADR:          /* item storage creation */
            sError = KpsCreateFile(usSize); /* for satellite remote printer, saratoga */
            if (sError == LDT_PROHBT_ADR) {
                return sError;
            } else if (sError == KPS_PERFORM) {
                if ((sError = TrnChkAndCreFile(usSize, FLEX_ITEMSTORAGE_ADR)) == TRN_SUCCESS) {
					TrnChkAndCreFile(usSize, FLEX_TMP_FILE_ID);  //SR 1090 JHHJ
                    return sError;
                }
            }
            sError = LDT_FLFUL_ADR;
            break;

        case FLEX_CONSSTORAGE_ADR:          /* cons. storage creation */
            sError = SerCreateTmpFile(usSize);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_CONSSTORAGE_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
				PifLog (MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)__LINE__);
			}
            if (sError == LDT_PROHBT_ADR) {
                return sError;
            }
            sError = TrnChkAndCreFile(usSize, FLEX_CONSSTORAGE_ADR);
            if (sError == TRN_SUCCESS) {    /* multiline display storage creation */
                sError = MldChkAndCreFile(usSize, FLEX_CONSSTORAGE_ADR);
            }
            if (sError == MLD_SUCCESS) {    /* FDT File creation */
				//create temp file for use with condiment editing for satelites JHHJ
				sError = MldChkAndCreFile(usSize, FLEX_TMP_FILE_ID);
                if (sError != MLD_SUCCESS) {
                    return sError;
                }

				switch (RflGetSystemType ()) {
				case FLEX_POST_CHK:
                    FDTChkAndDelFile(0);                /* delete FDT File */
                    sError = TrnChkAndCreFileSpl(usSize); /* R3.1 */
					break;
				case FLEX_STORE_RECALL:
                    TrnChkAndDelFileSpl(0);             /* delete Split check File */
                    sError = FDTChkAndCreFile(usSize);    /* R3.0 */
					break;
				default:
                    FDTChkAndDelFile(0);                /* delete FDT File */
                    TrnChkAndDelFileSpl(0);             /* delete Split check File */
                    sError = SUCCESS;
					break;
				}
            }
            break;

        case FLEX_CAS_ADR:                  /* Cashier creation */
//			if (! DFPRGetFileHeader (0)) {
//				NHPOS_ASSERT_TEXT(0, "calling DFPRInitFile.");
				//initialize the fingerprint file
//				DFPRInitFile(L"DigitalPersona 4500",3);
//			}
            sError = STUB_SUCCESS;
            break;

        case FLEX_WT_ADR:                   /* Waiter creation */
        case FLEX_GC_ADR:                   /* GCF creation */
        case FLEX_ETK_ADR:                  /* ETK creation */
        /* ===== Add Programmable Report Creation (Release 3.1) BEGIN ===== */
        case FLEX_PROGRPT_ADR:              /* Programmable Report, R3.1 */
        /* ===== Add Programmable Report Creation (Release 3.1) END ===== */
            sError = STUB_SUCCESS;
            break;

        case FLEX_CPN_ADR:                 /* Combination Coupon file creation */
            sError = OpCpnCheckAndCreatFile(usSize, 0);
            break;

        case FLEX_CSTRING_ADR:              /* Control String Size */
            sError = OpCstrCheckAndCreatFile(usSize, 0);
            break;

        /* ===== Add PPI File Creation (Release 3.1) BEGIN ===== */
        case FLEX_PPI_ADR:                  /* Promotional Pricing Item, R3.1 */
            sError = OpPpiCheckAndCreatFile(usSize, 0);
            break;
        /* ===== Add PPI File Creation (Release 3.1) END ===== */

        case FLEX_MLD_ADR:                  /* MLD Mnemonics File, V3.3 */
            sError = OpMldCheckAndCreatFile(usSize, 0);
            sError = OpMnemonicsFileCheckAndCreatFile(OPMNEMONICFILE_ID_REASON, usSize, 0);
            break;

        default:                            /* not use */
            PifAbort(MODULE_STB_ABORT, FAULT_INVALID_ARGUMENTS);
            break;
        }

    } else {                                /*=== MASTER or BM ===*/
        switch(uchType) {
        case FLEX_DEPT_ADR:                 /* dept file creation */
            sError = OpDeptCheckAndCreatFile(usSize, 0);
            if (OP_PERFORM == sError) {
                sError = TtlDeptCreate(usSize, uchPTD, TTL_CHECK_CREATE);
				if (sError < 0) {
					PifLog(MODULE_UI, LOG_ERROR_UIC_TF_TOTAL_FILE);
					PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
					PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
				}
            }
            break;

        case FLEX_PLU_ADR:                  /* PLU file creation */                       
            sError = OpPluCheckAndCreatFile(ulSize, 0);
            if (OP_PERFORM == sError) {
				USHORT  usMyLockHnd = 0;
				OpPluFileUpdate(usMyLockHnd);
                sError = TtlPLUCreate(ulSize, uchPTD, TTL_CHECK_CREATE);
				if (sError < 0) {
					PifLog(MODULE_UI, LOG_ERROR_UIC_TF_TOTAL_FILE);
					PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
					PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
				}
            }
            break;

        case FLEX_WT_ADR:                   /* Waiter file creation */
            sError = WAI_PERFORM;
            break;
                                            
        case FLEX_CAS_ADR:                  /* Cashier File creation */
            sError = CasCheckAndCreatFile(usSize);
            if (CAS_PERFORM == sError) {
                sError = TtlCasCreate(usSize, uchPTD, TTL_CHECK_CREATE);  /* V3.3 */
            }
            break;
                                            
        case FLEX_GC_ADR:                   /* GCF creation */
            sError = GusCheckAndCreateGCF(usSize, uchPTD);
            /* ===== Add Service Time File Creation (Release 3.1) BEGIN ===== */
            if (GCF_SUCCESS == sError) {
				USHORT          usRecord;

                if (FLEX_STORE_RECALL == uchPTD) {
                    sError = TtlSerTimeCreate(TTL_CHECK_CREATE);
					if (sError < 0) {
						PifLog(MODULE_UI, LOG_ERROR_UIC_TF_TOTAL_FILE);
						PifLog(MODULE_DATA_VALUE(MODULE_UI), uchType);
						PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
						PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
					}
                } else {
                    sError = TtlSerTimeDeleteFile();
					if (sError < 0) {
						PifLog(MODULE_UI, LOG_ERROR_UIC_TF_TOTAL_FILE);
						PifLog(MODULE_DATA_VALUE(MODULE_UI), uchType);
						PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
						PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
					}
                }

                usRecord = (USHORT)RflGetMaxRecordNumberByType(FLEX_CONSSTORAGE_ADR);
				switch (uchPTD) {
				case FLEX_POST_CHK:
                    FDTChkAndDelFile(0);                /* delete FDT File */
                    sError = TrnCreateFileSpl(usRecord);    /* Saratoga */
					break;
				case FLEX_STORE_RECALL:
                    TrnChkAndDelFileSpl(0);             /* delete Split check File */
                    sError = FDTCreateFile(usRecord);   /* Saratoga */
					break;
				default:
                    FDTChkAndDelFile(0);                /* delete FDT File */
                    TrnChkAndDelFileSpl(0);             /* delete Split check File */
                    sError = SUCCESS;
					break;
				}
            }
            /* ===== Add Service Time File Creation (Release 3.1) END ===== */
            break;

        case FLEX_EJ_ADR:                   /* EJ creation */
            sError = EJCheckC(usSize, uchPTD);
            break;

        case FLEX_ETK_ADR:                  /* ETK creation */
            sError = EtkCheckAndCreatFile(usSize);
            break;

        case FLEX_ITEMSTORAGE_ADR:          /* item storage creation */
            if (!uchMaintDelayBalanceFlag)	//JMASON -- If delayed balance is on then do NOT create file.
				TtlTtlUpdCreate(usSize);
            sError = KpsCreateFile(usSize);
            if (sError == LDT_PROHBT_ADR) {
                return sError;

            } else if (sError == KPS_PERFORM) {
                if ((sError = TrnChkAndCreFile(usSize, FLEX_ITEMSTORAGE_ADR)) == TRN_SUCCESS) {
					TrnChkAndCreFile(usSize, FLEX_TMP_FILE_ID);
                    return sError;
                }
            }
            sError = LDT_FLFUL_ADR;
            break;

        case FLEX_CONSSTORAGE_ADR:          /* cons. storage creation */
            sError = SerCreateTmpFile(usSize);
			if (sError != 0) {
				PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_CONSSTORAGE_ADR);
				PifLog (MODULE_ERROR_NO(MODULE_STB_ABORT), (USHORT)abs(sError));
				PifLog (MODULE_LINE_NO(MODULE_STB_ABORT), (USHORT)__LINE__);
			}
            if (sError == LDT_PROHBT_ADR) {
                return sError;
            } else if (sError == SERV_PERFORM) {
                sError = TrnChkAndCreFile(usSize, FLEX_CONSSTORAGE_ADR);
            }
            if (sError == TRN_SUCCESS) {    /* multiline display storage creation */
                sError = MldChkAndCreFile(usSize, FLEX_CONSSTORAGE_ADR);
                if (sError != MLD_SUCCESS) {
                    return sError;
                }
				sError = MldChkAndCreFile(usSize, FLEX_TMP_FILE_ID);
                if (sError != MLD_SUCCESS) {
                    return sError;
                }

				switch (RflGetSystemType ()) {
				case FLEX_POST_CHK:
                    FDTChkAndDelFile(0);                /* delete FDT File */
                    sError = TrnCreateFileSpl(usSize); /* R3.1 */
					break;
				case FLEX_STORE_RECALL:
                    TrnChkAndDelFileSpl(0);             /* delete Split check File */
                    sError = FDTCreateFile(usSize);    /* R3.0 */
					break;
				default:
                    FDTChkAndDelFile(0);                /* delete FDT File */
                    TrnChkAndDelFileSpl(0);             /* delete Split check File */
                    sError = SUCCESS;
					break;
				}
            }
            break;

        case FLEX_CSTRING_ADR:              /* Control String Size */
            sError = OpCstrCheckAndCreatFile(usSize, 0);
            break;

        case FLEX_CPN_ADR:                 /* Combination Coupon file creation */
            sError = OpCpnCheckAndCreatFile(usSize, 0);
            if (OP_PERFORM == sError) {
                sError = TtlCpnCreate(usSize, uchPTD, TTL_CHECK_CREATE);
				if (sError < 0) {
					PifLog(MODULE_UI, LOG_ERROR_UIC_TF_TOTAL_FILE);
					PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)abs(sError));
					PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
				}
            }
            break;

        /* ===== Add Programmable Report Creation (Release 3.1) BEGIN ===== */
        case FLEX_PROGRPT_ADR:              /* Programmable Report, R3.1 */
            sError = ProgRptCheckAndCreate((UCHAR)usSize);
            break;
        /* ===== Add Programmable Report Creation (Release 3.1) END ===== */

        /* ===== Add PPI File Creation (Release 3.1) BEGIN ===== */
        case FLEX_PPI_ADR:                  /* Promotional Pricing Item, R3.1 */
            sError = OpPpiCheckAndCreatFile(usSize, 0);
            break;
        /* ===== Add PPI File Creation (Release 3.1) END ===== */

        case FLEX_MLD_ADR:                  /* MLD Mnemonics File, V3.3 */
            sError = OpMldCheckAndCreatFile(usSize, 0);
            sError = OpMnemonicsFileCheckAndCreatFile(OPMNEMONICFILE_ID_REASON, usSize, 0);
            break;

        default:                            /* not use */
            PifAbort(MODULE_STB_ABORT, FAULT_INVALID_ARGUMENTS);
            break;
        }
    }

	if (sError < 0) {
        PifLog(MODULE_UI, LOG_ERROR_UIC_TF_FLEX_FILE);
        PifLog(MODULE_DATA_VALUE(MODULE_UI), uchType);
        PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
        PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)(abs(sError)));
	}
    return sError;
}

/*
*===========================================================================
** Synopsis:    SHORT   CliCheckDeleteFile(UCHAR uchType,
*                                          ULONG ulSize,
*                                          UCHAR uchPTD);
*     Input:    uchType - what kind of file you want to create
*               usSize  - size of file, number of person
*               uchPTD    - with PTD/ GCF type
*
** Return:      0:              successful creation
*               other:          error      
*
** Description:
*   This function deletes specified files if the size of the file is
*   different from the parameter 'usSize'.
*   This function should be called at Power Up after loading parameters
*   from the Loader B/D. Furthermore, the function 'CliCheckCreateFile'
*   must be called after executing this function.               Saratoga
*===========================================================================
*/
SHORT   CliCheckDeleteFile(UCHAR uchType, ULONG ulSize, UCHAR uchPTD)
{
    SHORT       sError = SUCCESS;
    USHORT      usSize;
    BOOL        fDeleted;

    usSize = (USHORT)ulSize;                /* Saratoga */

    if (CLI_IAM_SATELLITE) {    /* read satellite status only terminal no, saratoga */
/*    if (STUB_NOT_MASTER == CliCheckAsMaster()) { */
                                            /*=== SATELLITE TERMINAL ===*/
        switch(uchType) {
        case FLEX_DEPT_ADR:                 /* dept file deletion */
            sError = OpDeptCheckAndDeleteFile(usSize, 0);
            break;

        case FLEX_PLU_ADR:                  /* PLU file deletion */
			{
				USHORT  usI = 0;
				for (usI = 0; usI < 4; usI++) {
					sError = OpOepCheckAndDeleteFile(usI, ulSize, 0);  /* Saratoga */
					if (sError != 0) {
						PifLog(MODULE_STB_ABORT, LOG_EVENT_STB_CF_FLEX_PLU_ADR);
						PifLog(MODULE_DATA_VALUE(MODULE_STB_ABORT), usI);
					}
				}
			}
            sError = STUB_SUCCESS;
            /* sError = OpPluCheckAndDeleteFile(ulSize, 0);  / Saratoga */
            break;

        case FLEX_EJ_ADR:                   /* EJ creation */
            sError = EJCheckD(usSize, uchPTD);
            break;

        case FLEX_ITEMSTORAGE_ADR:          /* item storage creation */
            KpsDeleteFile();                /* for satellite remote printer, saratoga */
            /* break */                     /* not break */
            
        case FLEX_CONSSTORAGE_ADR:          /* cons. storage creation */
            TrnChkAndDelFile(usSize, FLEX_CONSSTORAGE_ADR);
            MldChkAndDelFile(usSize, FLEX_CONSSTORAGE_ADR);  /* multiline display storage */

			switch (RflGetSystemType ()) {
			case FLEX_POST_CHK:
                FDTChkAndDelFile(0);          /* FDT File,   R3.0 */
                TrnChkAndDelFileSpl(usSize);  /* Split File, R3.1 */
				break;

			case FLEX_STORE_RECALL:
                TrnChkAndDelFileSpl(0);       /* Split File, R3.1 */
                FDTChkAndDelFile(usSize);     /* FDT File,   R3.0 */
				break;

			default:
                FDTChkAndDelFile(0);          /* FDT File,   R3.0 */
                TrnChkAndDelFileSpl(0);       /* Split File, R3.1 */
				break;
			}
            sError = STUB_SUCCESS;
            break;

        case FLEX_WT_ADR:                   /* Waiter deletion */
        case FLEX_CAS_ADR:                  /* Cashier deletion */
        case FLEX_GC_ADR:                   /* GCF deletion */
        case FLEX_ETK_ADR:                  /* ETK creation */
        /* ===== Add Programmable Report Deletion (Release 3.1) BEGIN ===== */
        case FLEX_PROGRPT_ADR:              /* Programmable Report, R3.1 */
        /* ===== Add Programmable Report Deletion (Release 3.1) END ===== */
            break;

        case FLEX_CPN_ADR:                 /* Combination Coupon file deletion */
            sError = OpCpnCheckAndDeleteFile(usSize, 0);
            break;

        case FLEX_CSTRING_ADR:              /* Control String Size */
            sError = OpCstrCheckAndDeleteFile(usSize, 0);
            break;

        /* ===== Add PPI File Deletion (Release 3.1) BEGIN ===== */
        case FLEX_PPI_ADR:                  /* Promotional Pricing Item, R3.1 */
            sError = OpPpiCheckAndDeleteFile(usSize, 0);
            break;
        /* ===== Add PPI File Deletion (Release 3.1) END ===== */

        case FLEX_MLD_ADR:                  /* Mld Mnemonics File, V3.3 */
            sError = OpMldCheckAndDeleteFile(usSize, 0);
            sError = OpMnemonicsFileCheckAndDeleteFile(OPMNEMONICFILE_ID_REASON, usSize, 0);
            break;

        default:                            /* not use */
            PifAbort(MODULE_STB_ABORT, FAULT_INVALID_ARGUMENTS);
            break;
        }
        sError = STUB_SUCCESS;

    } else {                                /*=== MASTER or BM ===*/
        switch(uchType) {
        case FLEX_DEPT_ADR:                 /* dept file deletion */
            sError = OpDeptCheckAndDeleteFile(usSize, 0);
            if (OP_PERFORM == sError) {
                sError = TtlDeptDeleteFile(usSize, uchPTD, TTL_CHECK_DEL);
            } else {
                sError = TtlDeptDeleteFile(usSize, uchPTD, TTL_COMP_DEL);
            }
            break;

        case FLEX_PLU_ADR:                  /* PLU file deletion */                       
            sError = OpPluCheckAndDeleteFile(ulSize, 0);
            if (OP_PERFORM == sError) {
				NHPOS_NONASSERT_TEXT("==NOTE: OpPluCheckAndDeleteFile() for FLEX_PLU_ADR success.  call TtlPLUDeleteFile()");
                sError = TtlPLUDeleteFile(usSize, uchPTD, TTL_CHECK_DEL);
            } else {
				NHPOS_NONASSERT_TEXT("==NOTE: OpPluCheckAndDeleteFile() for FLEX_PLU_ADR fail.");
//                sError = TtlPLUDeleteFile(usSize, uchPTD, TTL_COMP_DEL);
            }
            break;
                                            
        case FLEX_WT_ADR:                   /* Waiter file deletion */
            sError = WAI_PERFORM;
            break;
                                            
        case FLEX_CAS_ADR:                  /* Cashier File deletion */
            sError = CasCheckAndDeleteFile(usSize);
            if (CAS_PERFORM == sError) {
                sError = TtlCasDeleteFile(usSize, uchPTD, TTL_CHECK_DEL); /* V3.3 */
            } else {
                sError = TtlCasDeleteFile(usSize, uchPTD, TTL_COMP_DEL);  /* V3.3 */
            }
            break;
                                            
        case FLEX_GC_ADR:                   /* GCF deletion */
            sError = GusCheckAndDeleteGCF(usSize, uchPTD, &fDeleted);
            /* ===== Add Service Time File Deletion (Release 3.1) BEGIN ===== */
            if ((GCF_SUCCESS == sError) && ( fDeleted )) {
                sError = TtlSerTimeDeleteFile();
            }
            /* ===== Add Service Time File Deletion (Release 3.1) END ===== */
			switch (uchPTD) {
			case FLEX_POST_CHK:
                FDTChkAndDelFile(0);                /* FDT File,   R3.0 */
				break;
			case FLEX_STORE_RECALL:
                TrnChkAndDelFileSpl(0);             /* Split File, R3.1 */
				break;
			default:
                FDTChkAndDelFile(0);                /* FDT File,   R3.0 */
                TrnChkAndDelFileSpl(0);             /* Split File, R3.1 */
				break;
			}
            break;

        case FLEX_EJ_ADR:                   /* EJ creation */
            sError = EJCheckD(usSize, uchPTD);
            break;

        case FLEX_ETK_ADR:                  /* ETK creation */
            sError = EtkCheckAndDeleteFile(usSize);
            break;

        case FLEX_ITEMSTORAGE_ADR:          /* item storage creation */
            TrnChkAndDelFile(usSize, FLEX_ITEMSTORAGE_ADR);
			TrnChkAndDelFile(usSize, FLEX_TMP_FILE_ID);
            if (!uchMaintDelayBalanceFlag)	//JMASON -- If delayed balance is on then do NOT create file.
				TtlTtlUpdDeleteFile();
            KpsDeleteFile();
            sError = STUB_SUCCESS;
            break;

        case FLEX_CONSSTORAGE_ADR:          /* cons. storage creation */
            TrnChkAndDelFile(usSize, FLEX_CONSSTORAGE_ADR);
            SerDeleteTmpFile();
            MldChkAndDelFile(usSize, FLEX_CONSSTORAGE_ADR);  /* multiline display storage */

			// determine the System Type and create the necessary transaction data
			// storage files used by the various System Type configurations.
			switch (RflGetSystemType ()) {
			case FLEX_POST_CHK:
                FDTChkAndDelFile(0);           /* FDT File,   R3.0 */
                TrnChkAndDelFileSpl(usSize);   /* Split File, R3.1 */
				break;
			case FLEX_STORE_RECALL:
                TrnChkAndDelFileSpl(0);        /* Split File, R3.1 */
                FDTChkAndDelFile(usSize);      /* FDT File,   R3.0 */
				break;
			default:
                FDTChkAndDelFile(0);           /* FDT File,   R3.0 */
                TrnChkAndDelFileSpl(0);        /* Split File, R3.1 */
				break;
			}
            sError = STUB_SUCCESS;
            break;

        case FLEX_CPN_ADR:                 /* Combination Coupon file deletion */
            sError = OpCpnCheckAndDeleteFile(usSize, 0);
            if (OP_PERFORM == sError) {
                sError = TtlCpnDeleteFile(usSize, uchPTD, TTL_CHECK_DEL);
            } else {
                sError = TtlCpnDeleteFile(usSize, uchPTD, TTL_COMP_DEL);
            }
            break;

        case FLEX_CSTRING_ADR:              /* Control String Size */
            OpCstrCheckAndDeleteFile(usSize, 0);
            sError = STUB_SUCCESS;
            break;

        /* ===== Add Programmable Report Deletion (Release 3.1) BEGIN ===== */
        case FLEX_PROGRPT_ADR:              /* Programmable Report, R3.1 */
            ProgRptCheckAndDelete((UCHAR)usSize);
            sError = STUB_SUCCESS;
            break;
        /* ===== Add Programmable Report Deletion (Release 3.1) END ===== */

        /* ===== Add PPI File Delete (Release 3.1) BEGIN ===== */
        case FLEX_PPI_ADR:                  /* Promotional Pricing Item, R3.1 */
            OpPpiCheckAndDeleteFile(usSize, 0);
            sError = STUB_SUCCESS;
            break;
        /* ===== Add PPI File Delete (Release 3.1) END ===== */

        case FLEX_MLD_ADR:                  /* Mld Mnemonics File, V3.3 */
            sError = OpMldCheckAndDeleteFile(usSize, 0);
            sError = OpMnemonicsFileCheckAndDeleteFile(OPMNEMONICFILE_ID_REASON, usSize, 0);
            break;

        default:                            /* not use */
            PifAbort(MODULE_STB_ABORT, FAULT_INVALID_ARGUMENTS);
            break;
        }
    }

	if (sError < 0) {
        PifLog(MODULE_UI, LOG_ERROR_UIC_TF_FLEX_FILE);
        PifLog(MODULE_DATA_VALUE(MODULE_UI), uchType);
        PifLog(MODULE_ERROR_NO(MODULE_UI), (USHORT)(abs(sError)));
        PifLog(MODULE_LINE_NO(MODULE_UI), (USHORT)__LINE__);
	}
    return sError;
}

/*
*===========================================================================
** Synopsis:    VOID CliDispDescriptor(USHORT usModuleID, 
*                                      USHORT usContDes)
*     Input:    usModuleID   - Module ID
*               usContDes    - Control to descriptor
*
** Return:      nothing
*
** Description: Display the descriptor on 2x20 (10N10D) operator display.
*
*               Priority: BLINK  is higher than ON.
*
*               ON     B0  = Stub
*                      B1  = Server
*                      B2  = ISP
*                      B3 -  B7  = Reserved
*
*               BLINK  B8  = Stub
*                      B9  = Server
*                      B10 = ISP
*                      B11 - B15 = Reserved
*
*===========================================================================
*/
VOID  CliDispDescriptor(USHORT usModleID, USHORT usContDes)
{
    UCHAR    uchCon;

    PifRequestSem(husCliComDesc);

    switch (usContDes) {

    case CLI_DESC_OFF :       /* Reset both flag */
        fsCliComDesc &= ~( usModleID + ( usModleID << 8 ) ) ;  
        break;

    case CLI_DESC_START_ON :  /* Set ON flag */
        fsCliComDesc |=  usModleID  ;    /* Set COMM flag */
        break;

    case CLI_DESC_STOP_ON :   /* Reset ON flag */ 
        fsCliComDesc &= ~(usModleID) ;   /* Reset COMM flag */
        break;

    case CLI_DESC_START_BLINK :  /* Set Blink flag */
        fsCliComDesc |=  usModleID << 8 ;     /* Set COMM Blink flag */
        break;

    case CLI_DESC_STOP_BLINK :  /* Reset Blink flag */
        fsCliComDesc &= ~(usModleID << 8 ) ;  /* Reset COMM BLINK flag */
        break;

    default:
		PifLog(MODULE_STB_LOG, LOG_ERROR_CLIENT_INVALID_FUNCODE);
        break;
    }

    uchCon = UIE_DESC_OFF;                              /* Default stop COMM Desc. */

    if ( fsCliComDesc &  CLI_DESC_CONT_ON ) {
        uchCon = UIE_DESC_ON;                           /* Display COMM Desc.*/
    }

    if ( fsCliComDesc &  CLI_DESC_CONT_BLINK ) {
        uchCon = UIE_DESC_BLINK;                        /* Blink COMM Desc. */
    }

    UieDescriptor(UIE_OPER, CLI_DESC_COMM, uchCon);     /* Disply Master Desc */

    PifReleaseSem(husCliComDesc);
}

/*
*===========================================================================
** Synopsis:    SHORT    CstReqOpePara(USHORT usType,
*                                       UCHAR *puchReqBuf,
*                                       USHORT usSize;
*                                       UCHAR *puchRcvBuf,
*                                       USHORT *pusRcvLen);
*    Input:     usType  - function code
*               *puchReqBuf - Request data buffer
*               usSize  - Request Data Length
*   OutPut:     *puchRcvBuf - Receive data buffer
*    InOut:     *pusRcvLen  - Received buffer/ data length    
*
** Return:      OP_PERFORM:         success to read op. para
*               OP_FILE_NOT_FOUND:  specified file does not exist
*
** Description:
*   This function supports to read operational parameter from master.
*===========================================================================
*/
SHORT   CstReqOpePara(USHORT usType,
                        UCHAR  *puchReqBuf,
                        USHORT usSize,
                        UCHAR  *puchRcvBuf,
                        USHORT *pusRcvLen)
{
    CLIREQOPPARA    ReqMsgH;
    CLIRESOPPARA    ResMsgH;
    SHORT           sError;

    memset(&ReqMsgH, 0, sizeof(CLIREQOPPARA));
    memset(&ResMsgH, 0, sizeof(CLIRESOPPARA));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = usType;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQOPPARA);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESOPPARA);
    CliMsg.sRetCode     = OP_PERFORM;
    CliMsg.pauchReqData = puchReqBuf;
    CliMsg.usReqLen     = usSize;
    CliMsg.pauchResData = puchRcvBuf;
    CliMsg.usResLen     = *pusRcvLen;

    sError = SstReadFAsMaster();
	if (sError < 0) {
		char xBuff[128];

		sprintf(xBuff, "CstReqOpePara(): SstReadFAsMaster = %d  CliMsg %d  %d", sError, CliMsg.sError, CliMsg.sRetCode);
		NHPOS_ASSERT_TEXT((sError >= 0), xBuff);
	}

    *pusRcvLen = CliMsg.usAuxLen;

	if (sError >= 0)
		sError = CliMsg.sError;
    return sError;
}

/*
*===========================================================================
** Synopsis:    SHORT    SstReqBackUp(USHORT usType,
*                                       UCHAR *puchReqBuf,
*                                       USHORT usSize;
*                                       UCHAR *puchRcvBuf,
*                                       USHORT *pusRcvLen);
*    Input:     usType  - function code
*               *puchReqBuf - Request data buffer
*               usSize  - Request Data Length
*   OutPut:     *puchRcvBuf - Receive data buffer
*               *pusRcvLen  - Reqceived data length    
*
** Return:      each modules have
*               
** Description:
*   This function supports to send request back up to as master.
*   This terminal must be Master or Back Up Master which is Out of Date.
*   The terget terminal must be Master or Back Up Master which is Up To Date.
*===========================================================================
*/
#if defined(SstReqBackUp)
USHORT   SstReqBackUp_Special(USHORT usType,
                        VOID   *puchReqBuf,
                        USHORT usSize,
                        VOID   *puchRcvBuf,
                        USHORT *pusRcvLen);

USHORT   SstReqBackUp_Debug(USHORT usType,
                        VOID   *puchReqBuf,
                        USHORT usSize,
                        VOID   *puchRcvBuf,
                        USHORT *pusRcvLen, char *aszFilePath, int nLineNo)
{
	SHORT  sError;

	sError = SstReqBackUp_Special(usType, puchReqBuf, usSize, puchRcvBuf, pusRcvLen);
	if (sError < 0) {
		char  xBuffer[256];

		sprintf (xBuffer, "SstReqBackUp_Debug(): usType = %d  sError = %d  File %s  lineno = %d", usType, sError, RflPathChop(aszFilePath), nLineNo);
		NHPOS_ASSERT_TEXT(0, xBuffer);
	}
	return sError;
}

USHORT   SstReqBackUp_Special(USHORT usType,
                        VOID   *puchReqBuf,
                        USHORT usSize,
                        VOID   *puchRcvBuf,
                        USHORT *pusRcvLen)
#else
SHORT   SstReqBackUp(USHORT usType,
                        VOID   *puchReqBuf,
                        USHORT usSize,
                        VOID   *puchRcvBuf,
                        USHORT *pusRcvLen)
#endif
{
	CLIREQBACKUP    ReqMsgH = {0};
    CLIRESBACKUP    ResMsgH = {0};
	SHORT           sError;
	SHORT           sRetry = 3;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = usType;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQBACKUP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESBACKUP);
    CliMsg.pauchReqData = puchReqBuf;
    CliMsg.usReqLen     = usSize;
    CliMsg.pauchResData = puchRcvBuf;
    CliMsg.usResLen     = *pusRcvLen;

	do {
		if (CLI_IAM_MASTER) {
			sError = CstSendBMaster();                   
		} else {
			sError = CstSendMaster();
		}
		*pusRcvLen = CliMsg.usAuxLen;

		if (sError < 0 || CliMsg.sRetCode < 0) {
			char xBuffer[128];
			sprintf (xBuffer, "  SstReqBackUp(): sRetry = %d sError = %d  CliMsg: sRetCode = %d  sError = %d", sRetry, sError, CliMsg.sRetCode, CliMsg.sError);
			NHPOS_ASSERT_TEXT (0, xBuffer);
		}
		sRetry--;
	} while ((sRetry > 0) && (sError == STUB_TIME_OUT || CliMsg.sRetCode == STUB_TIME_OUT));

	if (sError >= 0)
		sError = CliMsg.sError;
    return sError;
}

/*
*===========================================================================
** Synopsis:    SHORT    SstReqBackUpFH(USHORT usType,
*                                       UCHAR *puchReqBuf,
*                                       USHORT usSize;
*                                       SHORT hsFileHandle,
*                                       USHORT *pusRcvLen);
*    Input:     usType  - function code
*               *puchReqBuf - Request data buffer
*               usSize  - Request Data Length
*   OutPut:     hsFileHandle - File handle to be written
*               *pusRcvLen  - Reqceived data length    
*
** Return:      each modules have
*               
** Description:
*   This function supports to send request back up to as master.
*   This terminal must be Master or Back Up Master which is Out of Date.
*   The terget terminal must be Master or Back Up Master which is Up To Date.
*===========================================================================
*/
SHORT   SstReqBackUpFH(USHORT usType,
                        VOID   *puchReqBuf,
                        USHORT usSize,
                        SHORT hsFileHandle,
                        USHORT *pusRcvLen)
{
	CLIREQBACKUP    ReqMsgH = {0};
    CLIRESBACKUP    ResMsgH = {0};
	SHORT           sError;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = usType;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQBACKUP);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESBACKUP);
    CliMsg.pauchReqData = puchReqBuf;
    CliMsg.usReqLen     = usSize;
    CliMsg.pauchResData = ( VOID *)&hsFileHandle;
    CliMsg.usResLen     = *pusRcvLen;

    if (CLI_IAM_MASTER) {
        sError = CstSendBMasterFH(CLI_FCBAKGCF);                   
    } else {
        sError = CstSendMasterFH(CLI_FCBAKGCF);
    }
    *pusRcvLen = CliMsg.usAuxLen;

	if (sError < 0 || CliMsg.sRetCode < 0) {
		char xBuffer[128];
		sprintf (xBuffer, "  SstReqBackUpFH()  sError = %d  CliMsg.sError = %d  CliMsg.sRetCode = %d", sError, CliMsg.sError, CliMsg.sRetCode);
		NHPOS_ASSERT_TEXT (0, xBuffer);
	}

	if (sError >= 0)
		sError = CliMsg.sError;
    return sError;
}

/*
*===========================================================================
** Synopsis:    VOID    CliSleep(VOID);
*
** Return:      none.
*
** Description:  This function sleeps. R3.3
*===========================================================================
*/
VOID    CliSleep(VOID)
{
    CstSleep(); /* call sleep */
}

/*
*===========================================================================
** Synopsis:    SHORT  CliIsMasterOrBMaster(VOID);
*
** Return:      non-zero : i am master or b-master
*               zero     : i am not (master or b-master)
*
** Description:  This function returns if i am (backup-)master or not.
*===========================================================================
*/
SHORT CliIsMasterOrBMaster(VOID)
{
    if (CLI_IAM_MASTER || CLI_IAM_BMASTER) {
        return -1;
    }
    return 0;
}

SHORT CstOpRetrieveOperatorMessage(USHORT  *pusMessageBufferEnum)
{
	CLIREQMSGBUFFER      ReqMsg = {0};
    CLIRESMSGBUFFER      ResMsg = {0};
	TCHAR                tcsMessage[512] = {0};
	ULONG                ulMessageLength = 0;
	USHORT               usOpFlags = 0;

    PifRequestSem(husCliExeNet);

    memset(&CliMsg, 0, sizeof(CLICOMIF));

	ReqMsg.usBufferEnum = *pusMessageBufferEnum;

   /* set the request and response message */
    CliMsg.usFunCode    = CLI_FCOPREQMSGBUFFER;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsg;
    CliMsg.usReqMsgHLen = sizeof(CLIREQMSGBUFFER);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsg;
    CliMsg.usResMsgHLen = sizeof(CLIRESMSGBUFFER);
    
	{
		SHORT  sError;
		sError = SstReadFAsMaster();
		if (sError < 0) {
			char xBuff[128];

			sprintf(xBuff, "CstOpRetrieveOperatorMessage(): SstReadFAsMaster = %d  CliMsg %d  %d", sError, CliMsg.sError, CliMsg.sRetCode);
			NHPOS_ASSERT_TEXT((sError >= 0), xBuff);
		}
	}

	if (ResMsg.sReturn >= 0) {
		// ensure that the message is will fit in our buffer and be zero terminated
		// for the end of string.  Zero out to ensure we have an end of string.
		ulMessageLength = ResMsg.usMsgLen * sizeof(ResMsg.usTextMessage[0]);
		if (ulMessageLength > sizeof(tcsMessage)) {
			ulMessageLength = sizeof(tcsMessage) - sizeof(tcsMessage[0]);
		}

		memcpy (tcsMessage, ResMsg.usTextMessage, ulMessageLength);

		if (ulMessageLength > 0)
			BlFwIfPopupMessageWindow (tcsMessage);

		if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_FORCE_SIGNOUT, ResMsg.ucOperationalFlags)) {
			NHPOS_NONASSERT_TEXT("Received message with Force Sign-out Set");
			// handled below after releasing the semaphore since the sign-out will
			// use the same semaphore doing the Cashier Sign-out.
			usOpFlags |= 0x0008;
		}
		else if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_AUTO_SIGNOUT, ResMsg.ucOperationalFlags)) {
			NHPOS_NONASSERT_TEXT("Received message with Auto Sign-out Set");
			BlFwIfSetAutoSignOutTime (50, BLFWIF_AUTO_MODE_NO_CHANGE);
			usOpFlags |= 0x0004;
		}

		if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_SIGNIN_UNBLOCK, ResMsg.ucOperationalFlags)) {
			NHPOS_NONASSERT_TEXT("Received message with Unblock Sign-in Set");
			OpSignInBlockOff ();
			usOpFlags |= 0x0020;
		}
		else if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_SIGNIN_BLOCK, ResMsg.ucOperationalFlags)) {
			NHPOS_NONASSERT_TEXT("Received message with Block Sign-in Set");
			OpSignInBlockOn ();
			usOpFlags |= 0x0010;
		}

		if (SEROPDISPLAY_OPTIONS_ISSET (SEROPDISPLAY_FORCE_SHUTDOWN, ResMsg.ucOperationalFlags)) {
			NHPOS_NONASSERT_TEXT("Received message with Force Shutdown Set");
			// handled below after releasing the semaphore since the sign-out will
			// use the same semaphore doing the Cashier Sign-out.
			usOpFlags |= 0x0040;
		}

		*pusMessageBufferEnum = ResMsg.usBufferEnum;
	}

    PifReleaseSem(husCliExeNet);

	if ((usOpFlags & 0x0048) != 0) {
		BlFwIfSetAutoSignOutTime (0, BLFWIF_AUTO_MODE_NO_CHANGE);  // Clear timer if set
		PifSleep (50);
		CasForceAutoSignOut(1);

		if ((usOpFlags & 0x0040) != 0) {
			BlFwIfForceShutdown();
		}
	}

	PifLog (MODULE_STB_LOG, LOG_EVENT_STB_OP_MSG_PROCESSED);
	PifLog (MODULE_DATA_VALUE(MODULE_STB_LOG), usOpFlags);  // indicate which flags were set in this message
	if (ResMsg.sReturn < 0)
		PifLog (MODULE_ERROR_NO(MODULE_STB_LOG), (USHORT)abs(ResMsg.sReturn));  // indicate which flags were set in this message

	return ResMsg.sReturn;
}

/*
*===========================================================================
** Synopsis:    SHORT    CstReqReadFile(USHORT usFile, 
*                                       VOID *pBuffer, 
*										USHORT usBytes);
*    Input:     usType  - function code
*               *puchReqBuf - Request data buffer
*               usSize  - Request Data Length
*   OutPut:     *puchRcvBuf - Receive data buffer
*    InOut:     *pusRcvLen  - Received buffer/ data length    
*
** Return:      OP_PERFORM:         success to read op. para
*               OP_FILE_NOT_FOUND:  specified file does not exist
*
** Description:
*   This function supports to read operational parameter from master.
*===========================================================================
*/ //ESMITH LAYOUT
LONG CstReqReadFile(CONST USHORT* paszFileName,
					 CONST CHAR* paszMode,
					 VOID *pBuffer, 
					 USHORT usBytes,
					 ULONG ulFilePosition)
{
    UCHAR           *puchPosition;
    SHORT           sBytes, sReadBytes, sCurrentBytes, sResult;
	CLIREQREAD      ReqMsg = {0};
	CLIRESREAD      ResMsg = {0};

    /* check the read data size */
    if (usBytes == 0 || usBytes > CLI_MAXBYTES_SIZE) {
        return(RMT_ERROR_INVALID_BYTES);
    }

    /* set the request and response message */
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    wcsncpy(ReqMsg.aszFileName, paszFileName, CLI_FILENAME_SIZE);
    strncpy(ReqMsg.aszMode, paszMode, CLI_MODE_SIZE);
    CliMsg.usFunCode    = CLI_FCRMTREADFILE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsg;
    CliMsg.usReqMsgHLen = sizeof(CLIREQREAD);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsg;
    CliMsg.usResMsgHLen = sizeof(CLIRESREAD);
    
    /* initial  */
    puchPosition  = (UCHAR *)pBuffer;
    sBytes        = (SHORT)usBytes;
    sCurrentBytes = 0;
	((CLIREQREAD*)pBuffer)->ulPosition = ulFilePosition;

    while (sBytes > 0) {

        /* check the read data size */
        if (sBytes > CLI_MAXBYTES_SIZE)  sReadBytes = CLI_MAXBYTES_SIZE;
        else                             sReadBytes = sBytes;

        /* set the length of the read data  */
        ReqMsg.usBytes    = (USHORT)sReadBytes;
        ReqMsg.ulPosition = ulFilePosition;

        /* send 2170    */
		{
			SHORT  sError;
			sError = SstReadFAsMaster();
			if (sError < 0) {
				char xBuff[128];

				sprintf(xBuff, "CstReqReadFile(): SstReadFAsMaster = %d  CliMsg %d  %d  %d", sError, CliMsg.sError, CliMsg.sRetCode, ResMsg.lReturn);
				NHPOS_ASSERT_TEXT((sError >= 0), xBuff);
			}
		}

        /* error ocuurs */
        if ((sResult = CstErrCtl(CliMsg.sRetCode, ResMsg.lReturn)) < 0) {
			char xBuff[128];

			sprintf(xBuff, "CstReqReadFile(): CstErrCtl = %d  CliMsg %d  %d  %d", sResult, CliMsg.sError, CliMsg.sRetCode, ResMsg.lReturn);
			NHPOS_ASSERT_TEXT((sResult >= 0), xBuff);
            return(sResult);
        }

        if (sResult > sReadBytes || ResMsg.ulPosition != ReqMsg.ulPosition) {
			return TRUE;
        /* end file */
        } else if (sResult < sReadBytes) {
            memcpy(puchPosition, ResMsg.auchData, sResult);
            return((SHORT)(sResult + sCurrentBytes));

        /* continue */
        } else /* if (sResult == sReadBytes) */ {
            memcpy(puchPosition, ResMsg.auchData, sResult);
            sCurrentBytes += sResult;
            puchPosition  += sResult;
            sBytes        -= sResult;
        }
        
        /* loop */
    }

    return(sCurrentBytes);
}

LONG CstReqTransferFile(USHORT usTerminalPosition, CONST USHORT* paszFileName,
					 CONST CHAR* paszMode,
					 VOID *pBuffer, 
					 USHORT usBytes,
					 ULONG ulFilePosition)
{
	CLIREQREAD      ReqMsg = {0};
    CLIRESREAD      ResMsg = {0};
    UCHAR           *puchPosition;
    SHORT           sBytes, sReadBytes, sCurrentBytes, sResult;

    /* check the read data size */
    if (usBytes == 0 || usBytes > CLI_MAXBYTES_SIZE) {
        return(RMT_ERROR_INVALID_BYTES);
    }

    /* set the request and response message */
    memset(&CliMsg, 0, sizeof(CLICOMIF));
    wcsncpy(ReqMsg.aszFileName, paszFileName, CLI_FILENAME_SIZE);
    strncpy(ReqMsg.aszMode, paszMode, CLI_MODE_SIZE);
    CliMsg.usFunCode    = CLI_FCRMTREADFILE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsg;
    CliMsg.usReqMsgHLen = sizeof(CLIREQREAD);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsg;
    CliMsg.usResMsgHLen = sizeof(CLIRESREAD);
    
    /* initial  */
    puchPosition  = (UCHAR *)pBuffer;
    sBytes        = (SHORT)usBytes;
    sCurrentBytes = 0;
	((CLIREQREAD*)pBuffer)->ulPosition = ulFilePosition;

    while (sBytes > 0) {

        /* check the read data size */
        if (sBytes > CLI_MAXBYTES_SIZE)  sReadBytes = CLI_MAXBYTES_SIZE;
        else                             sReadBytes = sBytes;

        /* set the length of the read data  */
        ReqMsg.usBytes    = (USHORT)sReadBytes;
        ReqMsg.ulPosition = ulFilePosition;

        /* send 2170    */
        CstSendTerminal(usTerminalPosition);

        /* error ocuurs */
        if ((sResult = CstErrCtl(CliMsg.sRetCode, ResMsg.lReturn)) < 0) {
			char xBuff[128];

			sprintf(xBuff, "CstReqTransferFile(): CstErrCtl = %d  CliMsg %d  %d  %d", sResult, CliMsg.sError, CliMsg.sRetCode, ResMsg.lReturn);
			NHPOS_ASSERT_TEXT((sResult >= 0), xBuff);
            return(sResult);
        }

        if (sResult > sReadBytes || ResMsg.ulPosition != ReqMsg.ulPosition) {
			return TRUE;
        /* end file */
        } else if (sResult < sReadBytes) {
            memcpy(puchPosition, ResMsg.auchData, sResult);
            return((SHORT)(sResult + sCurrentBytes));

        /* continue */
        } else /* if (sResult == sReadBytes) */ {
            memcpy(puchPosition, ResMsg.auchData, sResult);
            sCurrentBytes += sResult;
            puchPosition  += sResult;
            sBytes        -= sResult;
        }
        
        /* loop */
    }

    return(sCurrentBytes);
}

/*
*===========================================================================
** Synopsis:    SHORT  CstErrCtl(VOID);
*
** Return:      non-zero : i am master or b-master
*               zero     : i am not (master or b-master)
*
** Description:  This function returns if i am (backup-)master or not.
*===========================================================================
*/ //ESMITH LAYOUT
SHORT CstErrCtl(SHORT sStubErr, LONG sPifErr)
{
    /* error occurs on STUB */
    if (sStubErr < 0)   return((SHORT) sStubErr);

    /* success              */
    if (sPifErr >= 0)   return((SHORT) sPifErr);

    /* error occurs on PIF  */
    switch(sPifErr) {
    case PIF_ERROR_FILE_NOT_FOUND:
        return(RMT_ERROR_FILE_NOT_FOUND);

    case PIF_ERROR_FILE_EXIST:
        return(RMT_ERROR_FILE_EXIST);

    case PIF_ERROR_FILE_EOF:
        return(RMT_ERROR_FILE_EOF);

    case PIF_ERROR_FILE_DISK_FULL:
        return(RMT_ERROR_FILE_DISK_FULL);

    default:
        return((SHORT) sPifErr);
    }
}
/*===== END OF SOURCE =====*/
