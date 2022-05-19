/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, Backup Copy Source File                        
* Category    : Client/Server STUB, US General Purpose Model
* Program Name: CSBAKETK.C                                            
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*
*               CliReqBakupETK()        Request  Back up MMM module
*               CliResBakupETK()        Response Back up MMM module
*               CliConvertErrorETK()    Convert MMM error to Opepara error.
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :   Name    : Description
* Jun-23-93:H.Yamaguchi: initial                                   
* Oct-05-93:H.Yamaguchi: Adds Total file                                   
*          :           :                                    
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
#include    <memory.h>

#include    <ecr.h>
#include    <pif.h>
#include    <rfl.h>
#include    <csop.h>
#include    <csstbstb.h>
#include    <plu.h>
#include    <csstbfcc.h>
#include    <csstbbak.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    <csserm.h>
#include    <csetk.h>
#include    "csstubin.h"

/*
*==========================================================================
**    Synopsis:   SHORT   CliReqBackupETK(USHORT usFun, USHORT usFile)
*
*       Input:    usFun
*                 usFile
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :   STUB_SUCESS
*                 CLI_ERROR_BAK_COMM
*                 CLI_ERROR_BAK_FULL
*
**  Description: Request ETK File Data for Backup.
*==========================================================================
*/
SHORT   CliReqBackupETK(USHORT usFun, USHORT usFile)
{
	CLI_FILE_BACKUP  *pETK_BakRcv;
	CLI_FILE_BACKUP  ETK_BakSnd = {0};
    SHORT           phsEtk;
	SHORT			sStatus;
    UCHAR           auchRcvBuf[(OP_BACKUP_WORK_SIZE + sizeof(CLI_FILE_BACKUP))];

    EtkAllUnLock();       /* Clear Flag */

    /* Open ETK file */
    sStatus = EtkOpenFile(auchOLD_FILE_READ_WRITE, (UCHAR)usFile, &phsEtk);
    if ( ETK_SUCCESS != sStatus ) {
        if ( ETK_FILE_NOT_FOUND == sStatus ) {
            return(STUB_SUCCESS);             /* if file not found, then ok */
        }
        return(sStatus);
    }

    ETK_BakSnd.usMaxRecord = (USHORT)RflGetMaxRecordNumberByType(FLEX_ETK_ADR);    /* Saratoga */
    ETK_BakSnd.usFile = usFile;
    ETK_BakSnd.lFilePosition = 0L;
    ETK_BakSnd.usSeqNo = 1;

    pETK_BakRcv = (CLI_FILE_BACKUP *)auchRcvBuf;
    for (;;) {
		USHORT usRcvLen = (OP_BACKUP_WORK_SIZE + sizeof(CLI_FILE_BACKUP));
        if  ((sStatus = SstReqBackUp(usFun, (UCHAR *)&ETK_BakSnd, sizeof(CLI_FILE_BACKUP), auchRcvBuf, &usRcvLen)) != 0 ) {
            break;
        }

        if ( sizeof(CLI_FILE_BACKUP) >  usRcvLen ) {
            sStatus = CLI_ERROR_BAK_COMM;
            break;
        }

        if (pETK_BakRcv->usSeqNo != ETK_BakSnd.usSeqNo ) {   
            sStatus = CLI_ERROR_BAK_COMM;
            break;
        }

        if ( 0 == pETK_BakRcv->usDataLen ) {   /* End of File */
            break;
        }

        /* Wrte Data */
        EtkWriteFile(phsEtk, ETK_BakSnd.lFilePosition, pETK_BakRcv->usDataLen, &auchRcvBuf[sizeof(CLI_FILE_BACKUP)]);
        if ( OP_BACKUP_WORK_SIZE != pETK_BakRcv->usDataLen ) {   /* End of File */
            sStatus = STUB_SUCCESS;
            break;
        }

        ETK_BakSnd.lFilePosition += (LONG)pETK_BakRcv->usDataLen;
        ETK_BakSnd.usSeqNo++;
    }

    EtkCloseFile(phsEtk);
    return(sStatus);
}

/*
*==========================================================================
**    Synopsis:    SHORT   CliResBackupETK(UCHAR   *puchRcvData,
*                                         USHORT  usRcvLen,
*                                         UCHAR   *puchSndData,
*                                         UCHAR   *pusSndLen)
*
*       Input:    *puchRcvData        * Pointer to Request Data *
*                 usRcvLen            * Width of puchRcvData *
*      Output:    *puchSndData        * Storage location for backup data. *
*       InOut:    *pusSndLen          * Width of puchSndData *
*
**  Return    :   STUB_SUCCESS
*                 CLI_ERROR_BAK_COMM
*
**  Description: Response ETK File Data for Backup. 
*==========================================================================
*/
SHORT   CliResBackupETK(UCHAR  *puchRcvData,
                        USHORT   usRcvLen,
                        UCHAR  *puchSndData,
                        USHORT  *pusSndLen)
{
    CLI_FILE_BACKUP  *pETK_BakRcv = (CLI_FILE_BACKUP *)puchRcvData;
    CLI_FILE_BACKUP  *pETK_BakSnd = (CLI_FILE_BACKUP *)puchSndData;
	ULONG			ulActualBytesRead;
    SHORT           sStatus;
	SHORT			phsEtk;

    *pusSndLen = 0;
    pETK_BakSnd->usFile        = pETK_BakRcv->usFile;
    pETK_BakSnd->lFilePosition = pETK_BakRcv->lFilePosition;
    pETK_BakSnd->usSeqNo       = pETK_BakRcv->usSeqNo;

    if (usRcvLen < sizeof(CLI_FILE_BACKUP)) {
        return (CLI_ERROR_BAK_COMM);
    }

    /* Check current record number */
    if ( 1 == pETK_BakRcv->usSeqNo ) {
        if ( pETK_BakRcv->usMaxRecord != (USHORT)RflGetMaxRecordNumberByType(FLEX_ETK_ADR) ) { /* Saratoga */
            return(CLI_ERROR_BAK_FULL); 
        }
    }

    /* Open ETK file */
    sStatus = EtkOpenFile(auchOLD_FILE_READ_WRITE, (UCHAR)pETK_BakRcv->usFile, &phsEtk);
    if ( ETK_SUCCESS != sStatus ) {
        return(sStatus);
    }

    *pusSndLen = sizeof(CLI_FILE_BACKUP);      

    /* Read ETK Data */
    sStatus = EtkReadFile(phsEtk, pETK_BakRcv->lFilePosition, OP_BACKUP_WORK_SIZE, puchSndData + sizeof(CLI_FILE_BACKUP), &ulActualBytesRead);
    if (  sStatus < 0 )  {
        EtkCloseFile(phsEtk);
        return(sStatus);
    } 
    //RPH SR#201
	NHPOS_ASSERT(ulActualBytesRead <= 0xFFFF/*MAXWORD*/);
    pETK_BakSnd->usDataLen = (USHORT)ulActualBytesRead;//sStatus;      
    *pusSndLen += pETK_BakSnd->usDataLen;      
    EtkCloseFile(phsEtk);
    return (STUB_SUCCESS);   
}

/*===== END OF SOURCE =====*/

