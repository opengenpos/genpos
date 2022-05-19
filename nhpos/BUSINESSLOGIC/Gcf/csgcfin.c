/*************************************************************************\
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO        ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992-9         ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                                         ||
||                                                                       ||
\*************************************************************************/
/*========================================================================*\
*   Title              : Client/Server Guest Check module, Program List                        
*   Category           : Client/Server Guest Check module, NCR2170 US HOSPITALITY MODEL 
*   Program Name       : CSGCFIN.C                                            
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since.
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*  ------------------------------------------------------------------------  
*   Discussion about the contents of this file.
		This file contains most of the lower level functions used to implement the
		Guest Check functionality.  The functionality for Guest Check actually used
		depends on the type of Guest Check system being used, Store/Recall, Pre-buffering,
		Post-buffering, etc.  The type of Guest Check system being used in turn depends
		on the type of store, Quick Service (Store/Recall), Table Service (Pre or Post buffering),
		or Retail.

		The Guest Check file is actually composed of two files, the Guest Check Index file
		which is used to keep a list of the Guest Check numbers and the Guest Check Data
		file which is used to store the actual transaction information.

		The Guest Check Index file is a sorted list of guest check index entries, sorted by
		the guest check number.  Each guest check index entry is inserted into the Guest
		Check Index file using an insertion sort meaning that the position for the guest
		check index entry is located in the Guest Check Index file and then the guest check
		index entry which is at the position along with all other guest check index entries
		after the position are moved to open up a guest check index entry location into which
		the new guest check index entry is copied.  When a guest check index entry is deleted,
		the guest check index entry is located in the file and then the guest check index entries
		after that index entry to be deleted are copied over the space where the guest check
		index entry to be deleted is located, basically rippling down the subsequent guest check
		index entries.
		
		See Gcf_Index (), Gcf_InsertTableinFile (), Gcf_SearchIndexinFile (), and Gcf_DeleteTableinFile().

		The Guest Check Data file is a large file which is divided into blocks of a given size.  Each
		of the blocks may be either in use by a guest check or may be on the free list and available
		for use.  The blocks are threaded in a series of lists.  Each block is assigned a block number
		or block index and this block number is used to physically locate the block in the Guest Check
		Data file.

		See Gcf_ReserveQueReturn(), Gcf_FreeQueReturn(), and Gcf_FreeQueTake().

*  ------------------------------------------------------------------------
*   Abstract           : The provided function names are as flows.
*
*    Gcf_ReadFile                      * Read data in GCf. 
*    Gcf_WriteFile                     * Write Data in GCF. 
*    Gcf_Index                         * Search Index File and Assign When not assigned.
*    Gcf_IndexDel                      * Delete inputted GCF No. in GCF File.
*    Gcf_CompIndex                     * Compare routine for Gcf_SearchIndexinFile.
*    Gcf_ReserveQueReturn              * Return blocks from data queue  to reserve queue
*    Gcf_FreeQueReturn                 * Return Blocks W/data to Free que.
*    Gcf_FreeQueTake                   * Get a Block From Free que.
* #  Gcf_DriveIndexSer                 * Search Drive No in Index Search.
*    Gcf_SearchIndexinFile             * Search Key in Index table in GCF.
*    Gcf_InsertTableinFile             * Insert Key in Index table in GCF.
*    Gcf_DeleteTableinFile             * Delete Key in Index table in GCF.
*    Gcf_DataBlockCopy                 * Read a GCF Data from Blocks.
*    Gcf_CreatAllFreeQue               * Initiarize Free Que Chain.
*    Gcf_BlockReserve                  * Get free block to chain to reserve queue
*    Gcf_GetNumberofBlockTypical       * Block Number for GCF File Creation
*    Gcf_GetNumberofBlockAssign        * Block reserve for Register Mode
*    Gcf_StoreDataFH                   * Copy Gcf Data to GCF Data Blocks(File Handle I/F Version). 
*    Gcf_DataBlockCopyFH               * Read a GCF Data from Blocks(File Handle I/F Version).
*    Gcf_ReadFileFH                    * Read Data from Source File(File Handle I/F Version). 
*    Gcf_WriteFileFH                   * Write Data to Source File(File Handle I/F Version). 
* #  Gcf_CheckPayment                  * Check Paid Transction
* #  Gcf_QueDrive                      * Queued GC to Drive through queue 
* #  Gcf_RemoveQueDrive                * Remove GC from Drive through queue
* #  Gcf_DeletePaidTran                * Delete Paid Transaction
*       
*   # =  modified or added function at release 3.0
*
*  ------------------------------------------------------------------------
*   Update Histories
*   Date     :Ver.Rev.:  NAME    :Description
*   May-06-92:00.00.01:M.YAMAMOTO:Initial
*   Mar-11-94:00.00.04:K.YOU     :add flex GC feature.(mod. Gcf_GetNumberofBlockAssign,
*            :        :          : Gcf_BlockReserve)
*            :        :          :(add Gcf_StoreDataFH, Gcf_DataBlockCopyFH,
*            :        :          : Gcf_ReadFileFH, Gcf_WriteFileFH) 
*   Aug-12-99:03.05.00:M.Teraki  :Merge STORE_RECALL_MODEL/GUEST_CHECK_MODEL
*
** NCR2171 **
*   Sep-06-99:01.00.00:M.Teraki  :initial (for 2171)
*   Dec-27-99:01.00.00:hrkato    :Saratoga
\*=======================================================================*/
/*=======================================================================*\
:   PVCS ENTRY
:-------------------------------------------------------------------------
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/
#include	<tchar.h>
#include    <string.h>
#include    <math.h>
#include	<stdio.h>

#include    <ecr.h>
#include    <pif.h>
#include    <log.h>
#include    <rfl.h>
#include    <storage.h>
#include    <csgcs.h>
#include    <appllog.h>
#include    <regstrct.h>
#include    <transact.h>
#include    <paraequ.h>
#include    <para.h>
#include    <csstbpar.h>
#include    "csgcsin.h"
#include    "csstbstb.h"


static SHORT Gcf_FreeQueTakeChain (GCF_FILEHED *pGcf_FileHed, GCF_INDEXFILE *pGcf_IndexFile, USHORT usBlock, ULONG *pulHitPoint)
{
	SHORT sStatus, sDeleteTranDone = 0;

    sStatus = Gcf_FreeQueTake (pGcf_FileHed, (USHORT)(usBlock + 1), &(pGcf_IndexFile->offusReserveBlockNo));
    while (sStatus == GCF_FULL) {
		if (Gcf_DeletePaidTran(pGcf_FileHed, pGcf_IndexFile->usGCNO) == GCF_FULL) {
			NHPOS_ASSERT(!"Gcf_FreeQueTakeChain(0: GCF FULL ERROR, could not free que");
			PifLog(MODULE_GCF, LOG_ERROR_GCF_FULL_ERR_07);
			return(GCF_FULL);
		}
		sDeleteTranDone = 1;
		sStatus = Gcf_FreeQueTake (pGcf_FileHed, (USHORT)(usBlock + 1), &(pGcf_IndexFile->offusReserveBlockNo));
    }

	if (sDeleteTranDone) {
		// Redo the search for the guest check number since we have had to
		// delete a paid transaction which has in turn moved the indexes around.
		GCF_INDEXFILE   Gcf_IndexFileTemp = *pGcf_IndexFile;

		sStatus = Gcf_Index(pGcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFileTemp, pulHitPoint);
	}
	return sStatus;
}

// used to replace instances of memmove () functions which were used
// to move guest check number queues when guest check numbers were UCHARs
SHORT  Gcf_memmove (GCNUM *gcDest, GCNUM *gcSource, int nItems)
{
	if (nItems) {
		memmove (gcDest, gcSource, nItems * sizeof(GCNUM));
	}
	return nItems;
}


/*
*==========================================================================
**    Synopsis:   SHORT     Gcf_Index(GCF_FILEHED   *pGcf_FileHed, 
*                                     UCHAR         uchMode, 
*                                     GCF_INDEXFILE *pGcf_IndexFile, 
*                                     VOID          *pHitPoint)
*
*       Input:    GCF_FILEHED       *pGcf_Filehed       * file header *
*                 UCHAR             uchMode             * GCF_REGIST search and regist, GCF_SEARCH serch only *
*                 USHORT            usGCNumber          * Search GCNumber *
*                 UCHAR             uchSubMode          * Sub Mode *  GCF_READ_FLAG_MASK 
*
*      Output:    VOID              *pHit_Point 
*       InOut:    GCF_INDEXFILE     *pGcf_IndexFile     * Index table *
*
**  Return   :    GCF_SUCCESS   key is found.
*                 GCF_NOT_IN    key is not found. Assign New GCNO by Regist mode(GCF_REGIST). 
*
**  Description:
*               Search Index File and Assign when not assigned.              
*==========================================================================
*/
SHORT  Gcf_Index(GCF_FILEHED        *pGcf_FileHed, 
                 UCHAR              uchMode, 
                 GCF_INDEXFILE      *pGcf_Index, 
                 ULONG               *pHit_Point)
{
    SHORT   sStatus;
    
	//Find the correct guest check in the index
    sStatus = Gcf_SearchIndexinFile(pGcf_FileHed->offulGCIndFile, pGcf_FileHed->usCurGCN, pHit_Point, pGcf_Index);
	// If we have found it, the Guest Check index is already available since
	// Gcf_SearchIndexinFile() puts found index into pGcf_Index if successful.
	// If we are searching only, return the status
    if ((uchMode == GCF_SEARCH_ONLY) || (sStatus == GCF_SUCCESS)) {
        return(sStatus);
    }

    /* Below is Assign Process */
    /* make space 1 record in index table and write new index data in it  */
    if (pGcf_FileHed->usMaxGCN <= pGcf_FileHed->usCurGCN) {
		// if we have reached the full number of Guest checks, 
		// try and delete one so that we can continue
//        if (Gcf_DeletePaidTran(pGcf_FileHed, pGcf_Index->usGCNO) == GCF_FULL) {
			PifLog(MODULE_GCF, LOG_ERROR_GCF_MAX_LT_CUR);
            PifLog(MODULE_DATA_VALUE(MODULE_GCF), pGcf_Index->usGCNO);
            PifLog(MODULE_DATA_VALUE(MODULE_GCF), pGcf_FileHed->usMaxGCN);
            PifLog(MODULE_DATA_VALUE(MODULE_GCF), pGcf_FileHed->usCurGCN);
            PifLog(MODULE_DATA_VALUE(MODULE_GCF), pGcf_FileHed->usCurPayGC);
            PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);
            return(GCF_FULL);
//        }
//		sStatus = Gcf_SearchIndexinFile(pGcf_FileHed->offulGCIndFile, pGcf_FileHed->usCurGCN, pHit_Point, pGcf_Index);
    }

	// put the index creation date and time stamp into the index to provide a way to determine
	// when the index was created for guest checks that are open and locked due to GenPOS
	// crashing or some other similar issue.
	{
		DATE_TIME DtNow;

		PifGetDateTime(&DtNow);
		pGcf_Index->GcfCreateTime.uchGcfYear = (UCHAR)(DtNow.usYear % 100);
		pGcf_Index->GcfCreateTime.uchGcfMonth = (UCHAR)DtNow.usMonth;
		pGcf_Index->GcfCreateTime.uchGcfDay = (UCHAR)DtNow.usMDay;
		pGcf_Index->GcfCreateTime.uchGcfHour = (UCHAR)DtNow.usHour;
		pGcf_Index->GcfCreateTime.uchGcfMinute = (UCHAR)DtNow.usMin;
		pGcf_Index->GcfCreateTime.uchGcfSecond = (UCHAR)DtNow.usSec;
	}

	//Assign The GC into the file
    Gcf_InsertTableinFile(pGcf_FileHed->offulGCIndFile, pGcf_FileHed->usCurGCN, *pHit_Point, pGcf_Index, sizeof(GCF_INDEXFILE));
	pGcf_FileHed->usCurGCN++;
    return(GCF_NOT_IN);
}

/*
*==========================================================================
**    Synopsis:    SHORT   Gcf_IndexDel(GCF_FILEHED   *pGcf_FileHed, 
*                                       GCF_INDEXFILE *pGcf_Index)
*
*       Input:    GCF_FILEHED pGcf_FileHed
*                 
*               
*      Output:    Nothing
*       InOut:    GCF_INDEXFILE *pGcf_Index
*
**  Return    :   GCF_SUCCESS
*                 GCF_NOT_IN_FILE
*                 (file error is system halt)
*
**  Description:
*               Search in the index for the guest check whose number is
*				specified in the pGcf_Index->usGCNO.  If found, delete the
*				index entry from the index.  If the guest check entry does
*				exist in the index, we will return a copy of the index
*				entry information.
*==========================================================================
*/
SHORT   Gcf_IndexDel(GCF_FILEHED *pGcf_FileHed, GCF_INDEXFILE *pGcf_Index)
{
    ULONG   ulHitPoint;
    SHORT   sStatus;

    if (0 == pGcf_FileHed->usCurGCN) {
        PifLog(MODULE_GCF, LOG_ERROR_GCF_CUR_ZERO_ERR);
        return(GCF_NOT_IN);
    }

	// Search for the index and if found, return it in pGcf_Index.
	// If found, then delete the entry from the index in the Guest
	// Check File and decrement the count of guest checks in the file.
    sStatus = Gcf_SearchIndexinFile (pGcf_FileHed->offulGCIndFile, pGcf_FileHed->usCurGCN, &ulHitPoint, pGcf_Index);

    if (sStatus == GCF_SUCCESS) {
		if (pGcf_FileHed->usCurGCN > 0) {
			pGcf_FileHed->usCurGCN--;
			Gcf_DeleteTableinFile(pGcf_FileHed->offulGCIndFile, pGcf_FileHed->usCurGCN, ulHitPoint, sizeof(GCF_INDEXFILE));
		}
    }
    return(sStatus);
}
/*
*==========================================================================
**    Synopsis:   SHORT   Gcf_ReserveQueReturn(GCF_FILEHED    *pGcf_FileHed, 
*                                              GCF_INDEXFILE  *pGcf_Index)
*
*       Input:    GCF_FILEHED   *pGcf_FileHed
*               
*      Output:    Nothing
*       InOut:    GCF_INDEX          
*
**  Return    :   OP_PERFORM
*                 OP_NOT_IN_FILE
*                 (file error is system halt)
**  Description:
*               Return  Data Blocks to Reserve que.
*==========================================================================
*/
VOID    Gcf_ReserveQueReturn(GCF_FILEHED    *pGcf_FileHed, 
                             GCF_INDEXFILE  *pGcf_Index)
{
    GCF_DATAHEAD    GcfDataHedFirst,   GcfDataHedLast,   GcfDataHedDestFirst;
    ULONG           ulSeekSourceFirst, ulSeekSourceLast, ulSeekDestFirst;

	// If there is nothing in the in use block chain, there's nothing to do.
	// We assume all blocks are already on the reserved block chain.
    if (0 == pGcf_Index->offusBlockNo) {
        return;
    }

	// if there is nothing in the reserved block chain, then we recover
	// the space that is currently in use by putting it into the reserved
	// block chain.  this really means that all of the blocks are in use
	// so we just move the in-use chain to the reserve and we don't have
	// to do any block chain manipulation on disk since the block chain
	// is already connected together on disk.
    if (0 == pGcf_Index->offusReserveBlockNo) {
        pGcf_Index->offusReserveBlockNo = pGcf_Index->offusBlockNo;
        pGcf_Index->offusBlockNo = 0;
        return;
    }

	/*
		The two easy cases are taken care of.  We are here because some of the
		blocks reserved for the Guest Check are in use, pGcf_Index->offusBlockNo,
		and some are in the reserve chain, pGcf_Index->offusReserveBlockNo.  We
		now need to consolidate all of the blocks into a single reserve chain by
		moving the blocks in use to the reserve chain.  This means we will need
		to update the block chain structure on disk.

		If there is only a single block in the in use chain then we
		just need to update this single block on disk

		If there is more than one block in the in use chain then we
		need to update the first and last blocks on disk.
		
		In either case, we are going to incorporate the in use chain by
		setting the offusNextBKNO of the last block to point to the first block 
		of the reserve block chain.  If there is just a single block in the
		in use chain, the first block and the last block are the same block.
		If there is more than one block in the in use chain, we have to
		do a separate read/update/write operation on that last block in
		order to merge the in use chain into the reserved block chain.
	 */

    ulSeekSourceFirst = pGcf_FileHed->offulGCDataFile + ((pGcf_Index->offusBlockNo - 1) * GCF_DATA_BLOCK_SIZE);
    Gcf_ReadFile( ulSeekSourceFirst, &GcfDataHedFirst, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);

    /* check if 1 block only */
    if (0 == GcfDataHedFirst.offusNextBKNO) { /* only 1 block */
        GcfDataHedFirst.offusNextBKNO = pGcf_Index->offusReserveBlockNo;

		//Seek to the correct destination of the reserved block to read from
        ulSeekDestFirst = pGcf_FileHed->offulGCDataFile + ((pGcf_Index->offusReserveBlockNo - 1) * GCF_DATA_BLOCK_SIZE);
        Gcf_ReadFile( ulSeekDestFirst, &GcfDataHedDestFirst, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);

        GcfDataHedFirst.offusSelfBKNO = GcfDataHedDestFirst.offusSelfBKNO;
        GcfDataHedDestFirst.offusSelfBKNO = pGcf_Index->offusReserveBlockNo;
        Gcf_WriteFile( ulSeekDestFirst, &GcfDataHedDestFirst, sizeof(GCF_DATAHEAD));

    } else {
        ulSeekSourceLast = pGcf_FileHed->offulGCDataFile + ((GcfDataHedFirst.offusSelfBKNO - 1) * GCF_DATA_BLOCK_SIZE);
        Gcf_ReadFile( ulSeekSourceLast, &GcfDataHedLast, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);

        GcfDataHedLast.offusNextBKNO = pGcf_Index->offusReserveBlockNo;
        Gcf_WriteFile( ulSeekSourceLast, &GcfDataHedLast, sizeof(GCF_DATAHEAD));

        ulSeekDestFirst = pGcf_FileHed->offulGCDataFile + ((pGcf_Index->offusReserveBlockNo - 1) * GCF_DATA_BLOCK_SIZE);
        Gcf_ReadFile( ulSeekDestFirst, &GcfDataHedDestFirst, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);

        GcfDataHedFirst.offusSelfBKNO     = GcfDataHedDestFirst.offusSelfBKNO;
        GcfDataHedDestFirst.offusSelfBKNO = pGcf_Index->offusReserveBlockNo;
        Gcf_WriteFile( ulSeekDestFirst, &GcfDataHedDestFirst, sizeof(GCF_DATAHEAD));
    }

    Gcf_WriteFile( ulSeekSourceFirst, &GcfDataHedFirst, sizeof(GCF_DATAHEAD));
    pGcf_Index->offusReserveBlockNo = pGcf_Index->offusBlockNo;
    pGcf_Index->offusBlockNo = 0;
}
/*
*==========================================================================
**    Synopsis:   SHORT   Gcf_FreeQueReturn(GCF_FILEHED *pGcf_FileHed, 
*                                           GCF_INDEX   *pGcf_Index)
*
*       Input:    GCF_FILEHED   *pGcf_FileHed
*               
*      Output:    Nothing
*       InOut:    GCF_INDEX          
*
**  Return    :   OP_PERFORM
*                 OP_NOT_IN_FILE
*                 (file error is system halt)
**  Description:
*               Return  Data Blocks to Free que.
*==========================================================================
*/
VOID    Gcf_FreeQueReturn(GCF_FILEHED *pGcf_FileHed, USHORT *pusSourceQue)
{
    GCF_DATAHEAD    GcfDataHedFirst,    GcfDataHedLast;
    ULONG           ulSeekSourceFirst,  ulSeekSourceLast;

	// if the starting block number is zero, then there is nothing to do.
    if (0 == (*pusSourceQue)) {
        return;
    }

    ulSeekSourceFirst = pGcf_FileHed->offulGCDataFile + ((*pusSourceQue - 1) * GCF_DATA_BLOCK_SIZE);
    Gcf_ReadFile( ulSeekSourceFirst, &GcfDataHedFirst, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);

	// if the free guest check block chain is zero then there are no
	// free blocks so this chain being freed is the beginning of the free chain.
    if (0 == pGcf_FileHed->offusFreeGC) {
        pGcf_FileHed->offusFreeGC = *pusSourceQue;
        
        GcfDataHedFirst.offusSelfBKNO = *pusSourceQue;
        Gcf_WriteFile( ulSeekSourceFirst, &GcfDataHedFirst, sizeof(GCF_DATAHEAD));
        *pusSourceQue = 0;
        return;
    }
    
    /* Check if 1 block only */
    if (0 == GcfDataHedFirst.offusNextBKNO) { /* only 1 block  */
        GcfDataHedFirst.offusNextBKNO = pGcf_FileHed->offusFreeGC;
        GcfDataHedFirst.offusSelfBKNO = *pusSourceQue;
    } else {
        ulSeekSourceLast = pGcf_FileHed->offulGCDataFile + (GcfDataHedFirst.offusSelfBKNO - 1) * GCF_DATA_BLOCK_SIZE;
        Gcf_ReadFile( ulSeekSourceLast, &GcfDataHedLast, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);

        GcfDataHedLast.offusNextBKNO = pGcf_FileHed->offusFreeGC;
        Gcf_WriteFile( ulSeekSourceLast, &GcfDataHedLast, sizeof(GCF_DATAHEAD));
    
        GcfDataHedFirst.offusSelfBKNO = *pusSourceQue;
    }

    Gcf_WriteFile( ulSeekSourceFirst, &GcfDataHedFirst, sizeof(GCF_DATAHEAD));

    pGcf_FileHed->offusFreeGC = *pusSourceQue;
    *pusSourceQue = 0;
}
/*
*==========================================================================
**    Synopsis:   SHORT   Gcf_FreeQueTake(GCF_FILEHED *pGcf_FileHed,
*                                         USHORT      usNumberOfBlock,
*                                         USHORT      *pusFreeStartBlock);
*
*       Input:    GCF_FILEHED   *pGcf_FileHed
*                 USHORT        usNumberOfBlock     
*      Output:    USHORT        *pusFreeStartBlock
*       InOut:    Nothing
*
**  Return    :   GCF_SUCCESS
*                 GCF_FULL
*                 (file error is system halt)
**  Description:
*               Get a Block From Free que in obedience to usNumberOfBlock.

				We are now going to remove the number of blocks we need from
				the chain of free blocks.  The first block in the chain of
				free blocks is pointed to by pGcf_FileHed->offusFreeGC.

				Next we then walk the free block chain, block by block until
				we reach the count of the number of blocks requested.  Then
				we will update the first block and the last block in our chain
				of request blocks then we will update the first block in the
				chain of remaining free blocks.

				If there aren't enough blocks to fulfill the request then we
				don't take any from the free chain and return a GCF_FULL error.

*==========================================================================
*/
SHORT    Gcf_FreeQueTake(GCF_FILEHED    *pGcf_FileHed, 
                         USHORT         usNumberOfBlock, 
                         USHORT         *pusFreeStartBlock)
{
    GCF_DATAHEAD    DataHed,     DataHedFirst;
    ULONG           ulSeekPoint, ulFirstSeekPoint;
    USHORT          cusI, cusITemp = 1;
	USHORT          usChainOfBlocks[40];

    *pusFreeStartBlock = 0;

    if (0 == usNumberOfBlock) {
        return(GCF_SUCCESS);
    }

    if (0 == pGcf_FileHed->offusFreeGC) {
//		NHPOS_ASSERT(!"GCF FULL ERROR, no free blocks");
        return(GCF_FULL);
    }

    ulFirstSeekPoint = pGcf_FileHed->offulGCDataFile + ((pGcf_FileHed->offusFreeGC - 1) * GCF_DATA_BLOCK_SIZE);
    Gcf_ReadFile(ulFirstSeekPoint, &DataHedFirst, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);

    ulSeekPoint = ulFirstSeekPoint;
    DataHed = DataHedFirst;

	usChainOfBlocks[0] = pGcf_FileHed->offusFreeGC;

    for (cusI = 1; cusI < usNumberOfBlock; cusI++) {
        if (0 == DataHed.offusNextBKNO) {
			//	This is not an error.  Need to reclaim space from deleted transactions.
            return(GCF_FULL);
        }

		// Quality check to see if the next block is one that we already have visited.
		// The goal here is to catch chains that have looped back to the block that
		// is in the chain of blocks.
		for (cusITemp = 0; cusITemp < cusI; cusITemp++) {
			if (usChainOfBlocks[cusITemp] == DataHed.offusNextBKNO) {
				break;
			}
		}
		if (cusITemp < cusI) {
			DataHed.offusNextBKNO      = 0;                      // indicate this is last block in chain to be used
			DataHedFirst.usDataLen     = 0;                      // initialize the data length in the chain to be used at zero
			Gcf_WriteFile(ulSeekPoint, &DataHed, sizeof(GCF_DATAHEAD));
            return(GCF_FULL);
		}

		usChainOfBlocks[cusI] = DataHed.offusNextBKNO;
		// quality check passed so go to the next block in the chain
        ulSeekPoint = pGcf_FileHed->offulGCDataFile + ((DataHed.offusNextBKNO - 1) * GCF_DATA_BLOCK_SIZE);
        Gcf_ReadFile(ulSeekPoint, &DataHed, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);
    }

    *pusFreeStartBlock = pGcf_FileHed->offusFreeGC;
    pGcf_FileHed->offusFreeGC  = DataHed.offusNextBKNO;  // remember rest of chain of blocks that are free
    DataHed.offusNextBKNO      = 0;                      // indicate this is last block in chain to be used
    DataHedFirst.offusSelfBKNO = DataHed.offusSelfBKNO;  // remember the last block in the chain to be used
    DataHedFirst.usDataLen     = 0;                      // initialize the data length in the chain to be used at zero

    Gcf_WriteFile(ulFirstSeekPoint, &DataHedFirst, sizeof(GCF_DATAHEAD));
    
    Gcf_WriteFile(ulSeekPoint, &DataHed, sizeof(GCF_DATAHEAD));
    
    return(GCF_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   SHORT   Gcf_DriveIndexSer(USHORT      usCurNumber, 
*                                           USHORT      usGCNumber, 
*                                           VOID        *pHitPoint,
*                                           ULONG       ulPosition);
*
*       Input:    USHORT        usCurNumber     * Nubmber of Current *
*                 USHORT        usGCNumber      * Character to search for *
*                 ULONG         ulPosition      * Queue offset position *
*
*      Output:    VOID          *pHitPoint      * A pointer to the first locatkon of usGCNumber in GCF if successsful *
*       InOut:    Nothing
*
**  Return    :   GCF_SUCCESS
*                 GCF_NOT_IN
*                 (file error is system halt)
*
**  Description:
*               Search Drive No in Index Search.
*==========================================================================
*/
SHORT   Gcf_DriveIndexSer(GCNUM        usCurNumber, 
                          GCNUM        usGCNumber,
                          ULONG        *pHitPoint,
                          ULONG         ulPosition)
{
    GCNUM   auchWorkBuffer[GCF_MAX_DRIVE_NUMBER];
    ULONG   ulCount;

	*pHitPoint = 0;

	if (usCurNumber > GCF_MAX_DRIVE_NUMBER) {
        PifLog(MODULE_GCF, LOG_ERROR_GCNUM_GT_MAX_DRIVE);
		usCurNumber = GCF_MAX_DRIVE_NUMBER;
	}

    Gcf_ReadFile(ulPosition, auchWorkBuffer, usCurNumber * sizeof(GCNUM), GCF_SEEK_READ);
    
	//Read through the entire buffer to find whether the GC
	//exists in the file.  If so, set the location to the hit
	//point
    for ( ulCount = 0; ulCount < usCurNumber  ; ulCount++) {
        if ( auchWorkBuffer[ulCount] == usGCNumber ) {
            *pHitPoint = (ulCount * sizeof(GCNUM)) + ulPosition;
            return(GCF_SUCCESS);
        }
    }

    return(GCF_NOT_IN);
}

/*
	Gcf_DriveIndexDeleteStd - delete a guest check from the standard payment queue.

	This function deletes a guest check from the standard, non-drive thru, guest
	check queue.
 */

VOID  Gcf_DriveIndexDeleteStd (GCF_FILEHED *pGcf_FileHed, ULONG ulHitPoint)
{
	if (ulHitPoint) {
		if (pGcf_FileHed->usCurPayGC > 0) {   /* Delete Payment Queue */
			pGcf_FileHed->usCurPayGC--;
			Gcf_DeleteTableinFile(pGcf_FileHed->offulPayTranNO, pGcf_FileHed->usCurPayGC, ulHitPoint, sizeof(GCNUM));
		}
	}
}

/*
	Gcf_DriveIndexDeleteSpl - delete a guest check from the special payment queue.

	This function deletes a guest check from one of the special drive thru payment
	queues.  The variable usPos indicates which queue to remove the guest check from.
 */

VOID  Gcf_DriveIndexDeleteSpl (GCF_FILEHED *pGcf_FileHed, USHORT usPos, ULONG ulHitPoint)
{
	if (ulHitPoint) {
		if (pGcf_FileHed->usCurDrive[usPos] > 0) {
			pGcf_FileHed->usCurDrive[usPos]--;
			Gcf_DeleteTableinFile(pGcf_FileHed->offulDrvNOFile[usPos], pGcf_FileHed->usCurDrive[usPos], ulHitPoint, sizeof(GCNUM));
		}
	}
}

/*
*==========================================================================
**    Synopsis:   SHORT     Gcf_SearchIndexinFile(ULONG offulTableHead ,
*                                                 USHORT usNumberOfIndex ,
*                                                 ULONG *poffulHitPoint ,
*                                                 VOID  *pKey ,
*                                                 USHORT usKeySize ,
*                                                 SHORT (*Comp)(VOID *pKey, 
*                                                               VOID *pusIndexKey)
*
*       Input:    ULONG  offulTableHead       Number of Bytes from File Head to index table to Search for
*                 USHORT usNumberOfIndex      Number of Key in a Table to Search for.
*                 VOID   *pKey                Pointer to object to search for.
*                 USHORT usKeySize            Width of key
*                 SHORT  (*Comp)(VOID *pKey, VOID *pusIndexKey)  Comparison function
*
*      Output:    ULONG *poffulHitPoint       Number of bytes from file head.
*                                             A pointer to the first occurrence of the object pointer
*                                             to by key in the table that offulTableHead points to.
*       InOut:    Nothing
*
**  Return    :   GCF_SUCCESS   key is found.
*                 GCF_NOT_IN    key is not found.
*
**  Description:
*               Search the Guest Check Index file for a specified guest check
				number, specified in pKey->usGCNO.  If found, return GCF_SUCCESS
				as the function return value and return the found key data through
				the interface variable pKey.
*==========================================================================
*/
SHORT     Gcf_SearchIndexinFile(  ULONG     offulTableHead,
                                  USHORT    usNumberOfIndex ,
                                  ULONG     *poffulHitPoint ,
                                  GCF_INDEXFILE      *pKey)
{
	GCF_INDEXFILE   gcfIndexArray[90];
	GCF_INDEXFILE   CompKey;
	GCF_INDEXFILE  *pKeyTail, *pKeyHead;
    SHORT           sCompStatus;

	// if there are no entries in the table then we will just return
	// the first slot in the table.
    if (usNumberOfIndex < 1) {
        *poffulHitPoint = offulTableHead;
        return(GCF_NOT_IN);
    }

	pKeyHead = (GCF_INDEXFILE *)offulTableHead;   // Set head pointer to beginning of the index area
	pKeyTail = pKeyHead + usNumberOfIndex - 1;    // Set the tail pointer to point to the last index in the area

    while (pKeyHead <= pKeyTail) {
		if (pKeyTail - pKeyHead < sizeof(gcfIndexArray)/sizeof(gcfIndexArray[0]) ) {
			int    iIndex = pKeyTail - pKeyHead;
			ULONG  offulTableTail = (ULONG)pKeyTail;

			offulTableHead = (ULONG)pKeyHead;
			offulTableTail = (ULONG)pKeyTail;

			// read the information at that offset, and set the info to the index array.
			// then figure out the array index allowing the compiler to convert bytes to index structs
			// we add one more GCF_INDEXFILE element since offulTableTail points to last index and we need to read it in as well.
			Gcf_ReadFile(offulTableHead, gcfIndexArray, offulTableTail - offulTableHead + sizeof(GCF_INDEXFILE), GCF_SEEK_READ);
			*poffulHitPoint = offulTableTail;
			iIndex = pKeyTail - pKeyHead;
			sCompStatus = ((SHORT)pKey->usGCNO - (SHORT)gcfIndexArray[iIndex].usGCNO);
			if (sCompStatus == 0){
				*pKey = gcfIndexArray[iIndex];
				return(GCF_SUCCESS);
			} else if (sCompStatus > 0) {
				*poffulHitPoint += sizeof(GCF_INDEXFILE);
				return(GCF_NOT_IN);
			}

			*poffulHitPoint = offulTableHead;
			for (iIndex = 0; *poffulHitPoint <= offulTableTail; *poffulHitPoint += sizeof(GCF_INDEXFILE), iIndex++) {
				sCompStatus = ( (SHORT)(pKey->usGCNO) - (SHORT)(gcfIndexArray[iIndex].usGCNO) );
				if (sCompStatus == 0){
					*pKey = gcfIndexArray[iIndex];
					return(GCF_SUCCESS);
				} else if (sCompStatus < 0) {
					break;
				}
			}
			return(GCF_NOT_IN);
		}
		else {
			int  iMiddle = 0;

			//set an offset of where to read in the file, which
			//starts at the beginning of GC
			iMiddle = (pKeyTail - pKeyHead)/2;
			offulTableHead = (ULONG)pKeyHead;
			*poffulHitPoint = ( iMiddle * sizeof(GCF_INDEXFILE) ) + offulTableHead;

			//read the information at that offset, and set the info to CompKey
			Gcf_ReadFile(*poffulHitPoint, &CompKey, sizeof(GCF_INDEXFILE), GCF_SEEK_READ);

			//CompKey is the same structure as pKey, so we check to see if they are the same
			//of if it is greater than or less than the current Guest Check
			sCompStatus = ((SHORT)pKey->usGCNO - (SHORT)CompKey.usGCNO);
			if(sCompStatus == 0){
				*pKey = CompKey;
				return(GCF_SUCCESS);
				break;
			} else if (sCompStatus > 0) {
				pKeyHead += iMiddle;
			} else {
				pKeyTail -= iMiddle;
			}
		}
    }
	//move the hitpoint to read more
    if (sCompStatus > 0) {
        *poffulHitPoint += sizeof(GCF_INDEXFILE);
    }
    return(GCF_NOT_IN);
}
/*
*==========================================================================
**    Synopsis:   VOID      Gcf_InsertTableinFile(ULONG  offulTableHead ,
*                                                 USHORT usNumberOfIndex ,
*                                                 ULONG  offulInsertPoint,
*                                                 VOID   *pInsertData ,
*                                                 USHORT usInsertDataSize
*
*       Input:    ULONG  offulTableHead       Offset from File Head to beginning of index table
*                 USHORT usNumberOfIndex      Number of Keys in the index table of the file.
*                 ULONG  offulInsertPoint     Offset from File Head into index table to insert *pInsertData
*                 VOID   *pInsertData         Pointer to data to be inserted
*                 USHORT usInsertDataSize     Size of index entry in bytes
*
*      Output:    Nothing
*                 
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Insert Key in Index table in GCF.
*==========================================================================
*/
VOID     Gcf_InsertTableinFile(   ULONG     offulTableHead ,
                                  USHORT    usNumberOfIndex ,
                                  ULONG     offulInsertPoint ,
                                  VOID      *pInsertData ,
                                  USHORT    usInsertDataSize)
{
    GCNUM   auchWorkBuffer[GCF_MAXWORK_SIZE];
    ULONG   ulMoveByte , offulTail;

	/* 
		offulInsertPoint is the offset at which the pInsertData data is to be inserted.
		offulTableHead is the offset in the file where the index table begins.
		offulTail is the offset within the index where the last index item is located

		If there are items in the table, then we need to do an insertion by moving everything
		from this insertion point and after the insertion point down to make room for the new
		item we are inserting.

		If the number of items in the table is zero, meaning there aren't any, then we
		just go ahead and put this item into the table at the requested insertion point.
	 */

    offulTail = ((ULONG)usNumberOfIndex * (ULONG)usInsertDataSize) + offulTableHead;

    if (offulInsertPoint < offulTail) {
		ULONG  ulSizeToMove = 0;
        ulMoveByte = offulTail - offulInsertPoint;
		ulSizeToMove = sizeof(auchWorkBuffer) - usInsertDataSize;
        while (ulMoveByte > sizeof(auchWorkBuffer))
        {
            offulTail -= sizeof(auchWorkBuffer);
            Gcf_ReadFile(offulTail, auchWorkBuffer, ulSizeToMove, GCF_SEEK_READ);
            Gcf_WriteFile(offulTail + usInsertDataSize, auchWorkBuffer, ulSizeToMove);
            ulMoveByte -= sizeof(auchWorkBuffer);
			ulSizeToMove = sizeof(auchWorkBuffer);
        }

		memset(auchWorkBuffer, 0x00, sizeof(auchWorkBuffer));
        if (ulMoveByte) {
            Gcf_ReadFile(offulInsertPoint, auchWorkBuffer, (USHORT)ulMoveByte, GCF_SEEK_READ);
            Gcf_WriteFile(offulInsertPoint + usInsertDataSize, auchWorkBuffer, (USHORT)ulMoveByte);
        }
    }
    Gcf_WriteFile(offulInsertPoint, pInsertData, usInsertDataSize);
}

/*
*==========================================================================
**    Synopsis:   VOID     Gcf_DeleteTableinFile (ULONG  offulTableHead ,
*                                                 USHORT usNumberOfIndex ,
*                                                 ULONG  offulDeletePoint,
*                                                 USHORT usDeleteDataSize )
*
*       Input:    ULONG  offulTableHead       Offset from File Head to beginning of index table
*                 USHORT usNumberOfIndex      Number of Keys in the index table of the file.
*                 ULONG  offulDeletePoint     Offset from File Head into index table to delete index entry
*                 USHORT usDeleteDataSize     Size of index entry in bytes
*
*      Output:    Nothing
*                 
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Delete Key in Index table in GCF.  This function runs through
*				the GC file and deletes a table in the file and moves the information
*				that was after the deleted table to the spot where the deleted table was.
*==========================================================================
*/
VOID      Gcf_DeleteTableinFile(  ULONG     offulTableHead,
                                  USHORT    usNumberOfIndex ,
                                  ULONG     offulDeletePoint,
                                  USHORT    usDeleteDataSize)
{
    GCNUM   auchWorkBuffer[GCF_MAXWORK_SIZE];
    ULONG   ulMoveByte , offulTail;

	if (usNumberOfIndex < 1) {
		return;
	}

    offulTail = ((ULONG)usNumberOfIndex * (ULONG)usDeleteDataSize) + offulTableHead;

	NHPOS_ASSERT(offulTail >= offulDeletePoint);

    if (offulDeletePoint < offulTail) {
		ulMoveByte = offulTail - offulDeletePoint;

		while (ulMoveByte > sizeof(auchWorkBuffer))
		{
			Gcf_ReadFile(offulDeletePoint + usDeleteDataSize, auchWorkBuffer, sizeof(auchWorkBuffer), GCF_SEEK_READ);
			Gcf_WriteFile(offulDeletePoint, auchWorkBuffer, sizeof(auchWorkBuffer));

			ulMoveByte -= sizeof(auchWorkBuffer);
			offulDeletePoint += sizeof(auchWorkBuffer);
		}

		memset(auchWorkBuffer, 0x00, sizeof(auchWorkBuffer));
		if (ulMoveByte) {
			Gcf_ReadFile(offulDeletePoint + usDeleteDataSize, auchWorkBuffer, ulMoveByte, GCF_SEEK_READ);
			Gcf_WriteFile(offulDeletePoint, auchWorkBuffer, (USHORT)ulMoveByte);
		}
	}
}
/*
*==========================================================================
**    Synopsis:   SHORT     Gcf_DataBlockCopy(GCF_FILEHED *pGcf_FileHed, 
*                                             USHORT      usStartBlockNO, 
*                                             UCHAR       *puchRcvBuffer,
*                                             USHORT      usRcvSize, 
*                                             USHORT      *pusCopyBlock)
*
*       Input:    GCF_FILEHED  *pGcf_FileHed  File Headder.
*                 USHORT usStartBlockNO       Start Block NO to be read.
*                 USHORT usSize               Maximum number of bytes to be read.
*
*      Output:    UCHAR  *puchRcvBuffer       Storage location for data.
*                 USHORT *pusCopyBlock        Number of Block to be copied.
*                 
*       InOut:    Nothing
*
**  Return    :   Read Size.
*
**  Description:
*               Read a GCF Data from Blocks. 
*==========================================================================
*/
SHORT     Gcf_DataBlockCopy(GCF_FILEHED     *pGcf_FileHed, 
                            GCNUM          usStartBlockNO, 
                            UCHAR           *puchRcvBuffer, 
                            USHORT          usRcvSize,
                            USHORT          *pusCopyBlock)
{
    GCF_DATAHEAD    GcfDataHead;
    ULONG           offulBlockPoint;
    USHORT          usWorkSize,  usWorkRetSize;

    *pusCopyBlock = 0;
    if (0 == usStartBlockNO) {
        return(0);
    }

    offulBlockPoint = pGcf_FileHed->offulGCDataFile + ((usStartBlockNO - 1) * GCF_DATA_BLOCK_SIZE);
    Gcf_ReadFile(offulBlockPoint, &GcfDataHead, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);
    if (0 == GcfDataHead.usDataLen){
        return(0);                 /* Return Len = 0 */
    }

    if (0 == usRcvSize ) {   /* If lock only, then get block count */
        for ( usWorkSize = 0; GcfDataHead.usDataLen != 0 ; usWorkSize++ ){
            (*pusCopyBlock)++;
            if (usWorkSize) {
                if ( GcfDataHead.usDataLen < (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC)) ) {
                    break;
                }
                GcfDataHead.usDataLen -= (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC));
            }  else {
                if ( GcfDataHead.usDataLen < (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD)) ) {
                    break;
                }
                GcfDataHead.usDataLen -= (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD));
           }
        }
        return(0);    /* Return len = 0 */
    }

    if (GcfDataHead.usDataLen > usRcvSize) {
        usWorkSize = usWorkRetSize = usRcvSize;
    } else {
        usWorkSize = usWorkRetSize = GcfDataHead.usDataLen;
    }

    if (usWorkSize <= (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD))) {
        (*pusCopyBlock)++;
        Gcf_ReadFile(0, puchRcvBuffer, usWorkSize, GCF_CONT_READ);
        return(usWorkRetSize);
    }

    Gcf_ReadFile(0L, puchRcvBuffer, GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD), GCF_CONT_READ);
    usWorkSize    -= GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD);
    puchRcvBuffer += GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD);
    (*pusCopyBlock)++;

    for (;;) {
        (*pusCopyBlock)++;

        if (!(GcfDataHead.offusNextBKNO)) {
			NHPOS_ASSERT(0 != GcfDataHead.offusNextBKNO);
            PifLog(MODULE_GCF, FAULT_ERROR_BLOCK_FULL);  /* BLOCK IS FULL */
            PifLog(MODULE_DATA_VALUE(MODULE_GCF), usStartBlockNO);  /* BLOCK IS FULL */
            PifLog(MODULE_DATA_VALUE(MODULE_GCF), *pusCopyBlock);  /* BLOCK IS FULL */
            PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);  /* BLOCK IS FULL */
            PifAbort(MODULE_GCF, FAULT_ERROR_BLOCK_FULL);  /* BLOCK IS FULL */
        }

        offulBlockPoint = pGcf_FileHed->offulGCDataFile + ((GcfDataHead.offusNextBKNO - 1) * GCF_DATA_BLOCK_SIZE);

        Gcf_ReadFile(offulBlockPoint, &GcfDataHead, sizeof(GCF_DATAHEAD_SEC), GCF_SEEK_READ);

        if (usWorkSize <= (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC))) {
            Gcf_ReadFile(0L, puchRcvBuffer, usWorkSize, GCF_CONT_READ);
            break;
        }

        Gcf_ReadFile(0, puchRcvBuffer, GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC), GCF_CONT_READ);
        usWorkSize    -= GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC);
        puchRcvBuffer += GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC);
    }
    return(usWorkRetSize);
}
/*
*==========================================================================
**    Synopsis:   VOID     Gcf_CreatAllFreeQue(ULONG  offulPoint, 
*                                              USHORT usNumberOfBlock)
*
*       Input:    ULONG  offulPoint           Free Que top address (Number of bytes from file head).
*                 USHORT usNumberOfBlock      Number of Block.
*
*      Output:    Nothing
*                 
*       InOut:    Nothing
*
**  Return    :   Nothing
*
**  Description:
*               Initiarize Free Que Chain. 
*==========================================================================
*/
VOID     Gcf_CreatAllFreeQue(ULONG offulPoint, USHORT usNumberOfBlock)
{
    GCF_DATAHEAD_SEC    FreeQue;
    USHORT              usQueNo;

    for (usQueNo = 1; usQueNo < usNumberOfBlock; usQueNo++) {
        FreeQue.offusSelfBKNO = usQueNo;
        FreeQue.offusNextBKNO = usQueNo + 1;  /* usNextPointer = usBlockNo + 1 */

        Gcf_WriteFile(offulPoint, &FreeQue, sizeof(GCF_DATAHEAD_SEC));
        offulPoint += GCF_DATA_BLOCK_SIZE;
    }
    FreeQue.offusSelfBKNO = usQueNo;
    FreeQue.offusNextBKNO = 0;              /* Set Last Que Id  */

    Gcf_WriteFile(offulPoint, &FreeQue, sizeof(GCF_DATAHEAD_SEC));
}

/*
*==========================================================================
**    Synopsis:   SHORT      Gcf_BlockReserve(GCF_FILEHED    *pGcf_FileHed,
*                                             USHORT         *pusFreeStartBlock)
*
*       Input:  GCF_FILEHED     *pGcf_FileHed     
*
*      Output:  USHORT          *pusFreeStartBlock
*                 
*       InOut:  Nothing
*
**  Return    :     normal end   : GCF_SUCCESS
*                   abnormal end : GCF_FULL
*
**  Description:
*               Get Blocks From Free Que in order to usNumberOfBlock.
*==========================================================================
*/
SHORT   Gcf_BlockReserve(GCF_FILEHED *pGcf_FileHed, USHORT *pusFreeStartBlock)
{
    USHORT      usNumberOfTakeQue = 0;
	SHORT       sStatus;

	Gcf_GetNumberofBlockAssign(pGcf_FileHed->usSystemtype, &usNumberOfTakeQue);

    sStatus = Gcf_FreeQueTake(pGcf_FileHed, usNumberOfTakeQue, pusFreeStartBlock);

    return(sStatus);
}
/*
*==========================================================================
**    Synopsis:   VOID Gcf_GetNumberofBlockTypical(UCHAR uchType, 
*                                                  UCHAR *puchBlockNo);
*
*       Input:  UCHAR       uchType          FLEX_PRECHK_BUFFER
*                                            FLEX_PRECHK_UNBUFFER
*                                            FLEX_POST_CHK
*                                            FLEX_STORE_RECALL 
*
*      Output:  UCHAR       *puchBlockNo     Typical number of blocks by 1 GCf
*                 
*       InOut:  Nothing
*
**  Return    :     normal end   : GCF_SUCCESS
*                   abnormal end : GCF_FULL
*
**  Description:
*               Get Typical Number of Blocks typically used in a particular type
*               of system (pre-buffering, post-buffering, store/recall, etc).  We
*               assume that most checks of store/recall type buffering will use 2/3
*               of the max number of items per transaction specified in P2.  No matter
*               what, we cap the max number of blocks per transaction to be 11 to
*               ensure block numbers will fit in USHORT data type (0 - 65,000).
*==========================================================================
*/
VOID Gcf_GetNumberofBlockTypical(USHORT usType, USHORT *pusAssignBlock)
{
	Gcf_GetNumberofBlockAssign(usType, pusAssignBlock);
	if (GCF_TYPICAL_QUE_TYPE23 < (*pusAssignBlock)) {
		*pusAssignBlock *= 3;
		*pusAssignBlock /= 4;
		if (GCF_TYPICAL_QUE_TYPE23 > (*pusAssignBlock)) {
			(*pusAssignBlock) = GCF_TYPICAL_QUE_TYPE23;
		}
	}
	if (11 < (*pusAssignBlock)) {
		*pusAssignBlock = 11;
	}
}
/*
*==========================================================================
**    Synopsis:   VOID Gcf_GetNumberofBlockAssign(GCF__FILEHD  *pGcf_FileHed,
*                                                 USHORT     *pusAssignBlock);
*
*       Input:  GCF_FILEHED     *pGcf_FileHed          
*               USHORT          *pusAssignBlock
*
*      Output:  Nothing
*                 
*       InOut:  Nothing
*
**  Return    :     normal end   : GCF_SUCCESS
*                   abnormal end : GCF_FULL
*
**  Description:
*               Calculate real Number of Guest Check file Blocks which
*               depends on the System Type.  The System Type determines how
*               much storage space is needed for the average Guest Check data.
*==========================================================================
*/
VOID Gcf_GetNumberofBlockAssign(USHORT usType, USHORT *pusAssignBlock)
{
    switch (usType) {
    case FLEX_PRECHK_BUFFER:
    case FLEX_PRECHK_UNBUFFER:
        *pusAssignBlock = GCF_TAKEQUE_TYPE01;
        break;

    case FLEX_POST_CHK:
    case FLEX_STORE_RECALL:
		{
			ULONG       ulConsSize;

			/* --- Saratoga --- */
			// find out the size of the transaction data temporary file used for a single
			// transaction and then calculate the number of Guest Check file blocks needed
			// to hold that size of data.
			ulConsSize = TrnCalStoSize(RflGetMaxRecordNumberByType(FLEX_CONSSTORAGE_ADR), FLEX_CONSSTORAGE_ADR);
			*pusAssignBlock = (USHORT)((ulConsSize + GCF_DATA_BLOCK_SIZE - 1) / GCF_DATA_BLOCK_SIZE) + 1;
		}
        break;

    default:
		NHPOS_ASSERT_TEXT(0, "**WARNING: Gcf_GetNumberofBlockAssign() invalid uchType specified.");
		break;
    }
}


/*
*==========================================================================
**    Synopsis:   SHORT      Gcf_StoreDataFH(GCF_FILEHED *pGcf_FileHed,
*                                            USHORT      usStatus,
*                                            USHORT      usGCNumber
*                                            SHORT       hsFileHandle,
*                                            USHORT      usStartPoint,
*                                            USHORT      usSize,
*                                            USHORT      usType)
*                                                       
*       Input:  GCF_FILEHED *pGcf_FileHed     
*               USHORT      usStatus            0: GCF, 1: DriveThrough
*               USHORT      usGCNumber          Guest Check NO.
*               SHORT       hsFileHandle        File Handle 
*               USHORT      usStartPoint        Start pointer of stored data 
*               USHORT      usSize              Data size
*               USHORT      usType              Type
*
*      Output:    Nothing
*                 
*       InOut:    Nothing
*
**  Return    :     GCF_SUCCESS
*                   GCF_NOT_LOCKED
*                   GCF_FULL
*
**  Description:
*               Copy Gcf Data to GCF Data Blocks(File Handle Version). 
*==========================================================================
*/
SHORT   Gcf_StoreDataFH(GCF_FILEHED *pGcf_FileHed,
                        USHORT      usStatus,
                        GCNUM      usGCNumber,
                        SHORT       hsFileHandle,
                        ULONG       ulStartPoint,
                        ULONG       ulSize,
                        USHORT      usType)
{                                   

    UCHAR           uchSndBuffer[GCF_DATABLOCK];
    UCHAR           cuchi;
    SHORT           sStatus, sqStatus, i;
    USHORT          usFreeStartBlock, usLastBlockNo, usBlock;
    LONG            offulBlockPosition, offulFirstBlock;
    ULONG           ulHitPoint;
    ULONG           offulReadPoint;
    GCF_INDEXFILE   Gcf_IndexFile;
    GCF_DATAHEAD    GcfDataHead,    GcfDataHeadFirst;

#if 0
#pragma message("  WARNING:  **********      Gcf_WalkGcfDataFile () is ENABLED.  Should it?  *****");
//  Following code is inserted here in order to provide a place to set a breakpoint
//  when testing the Guest Check tally statistics collection.
//  This code should not be enabled for a 
	{
		GCF_TALLY_INDEX  TallyArray[50];
		usBlock = Gcf_WalkGcfDataFile (pGcf_FileHed, TallyArray, 50);
	}
#endif

    Gcf_IndexFile.usGCNO = usGCNumber;
    Gcf_IndexFile.fbContFlag = GCF_READ_FLAG_MASK;
    Gcf_IndexFile.offusBlockNo = 0;
    Gcf_IndexFile.offusReserveBlockNo = 0;
    offulReadPoint = ulStartPoint;

    /* Search Index Table And Assign by GC Number */
    sStatus = Gcf_Index(pGcf_FileHed, GCF_REGIST, &Gcf_IndexFile, &ulHitPoint);

	//if there is no guest check in the file with that guest check number
    usBlock = ( USHORT)(ulSize / (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC)));
    if (ulSize % (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC))) {
        usBlock++;
    }

    if (sStatus == GCF_SUCCESS) {
		//look into the index and see if the guest check exists
        if (usStatus & GCF_BACKUP_STORE_FILE) { /* Is this call a  Backup function ?  */
            return(GCF_BACKUP_ERROR);
        }
        if ((Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) == 0) {   /* if the read/locked flag is not set */
            if (Gcf_IndexFile.offusBlockNo) {
				//if there is an offset read the guest check in that location
                Gcf_ReadFile(pGcf_FileHed->offulGCDataFile + ((ULONG)(Gcf_IndexFile.offusBlockNo - 1) * GCF_DATA_BLOCK_SIZE), 
                             &GcfDataHeadFirst, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);
                if (GcfDataHeadFirst.usDataLen == ulSize) {
                    return(GCF_SUCCESS);
                }
				PifLog(MODULE_GCF, LOG_ERROR_GCF_NOTLOCKED_ERR);
				PifLog(MODULE_DATA_VALUE(MODULE_GCF), usGCNumber);
				PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);
            } else {
                if (ulSize == 0) {
                    return(GCF_SUCCESS);
                }
				PifLog(MODULE_GCF, LOG_ERROR_GCF_NOTLOCKED_ERR);
				PifLog(MODULE_DATA_VALUE(MODULE_GCF), usGCNumber);
				PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);
            }
            return(GCF_NOT_LOCKED);
        }
		/*
			We reset the read flag to indicate this guest check is no
			longer locked.
		 */
        Gcf_IndexFile.fbContFlag &= ~GCF_READ_FLAG_MASK;       /* reset read flag */
		Gcf_ReserveQueReturn(pGcf_FileHed, &Gcf_IndexFile);
        Gcf_FreeQueReturn(pGcf_FileHed, &Gcf_IndexFile.offusReserveBlockNo);
		sqStatus = Gcf_FreeQueTakeChain (pGcf_FileHed, &Gcf_IndexFile, usBlock, &ulHitPoint);

		/* If old data is drive type, then delete drive id no from drive table */
        if ((Gcf_IndexFile.fbContFlag & GCF_DRIVE_THROUGH_MASK) != 0) {     /* Gcf_IndexFile is before */
			SHORT   sdStatus;
			ULONG   ulDriveIndexSerHitPoint;

            for (i = 0; i < GCF_MAX_QUEUE; i++) {
                sdStatus = Gcf_DriveIndexSer(pGcf_FileHed->usCurDrive[i], usGCNumber, &ulDriveIndexSerHitPoint, pGcf_FileHed->offulDrvNOFile[i]);
                if (sdStatus == GCF_SUCCESS) {
					Gcf_DriveIndexDeleteSpl (pGcf_FileHed, i, ulDriveIndexSerHitPoint);
                }
            }
        }

		/*
			Check if it is a paid transaction.
			If so, then we decrement the count of not paid transactions
			in the guest check file.

		 */
		if ((usStatus & GCF_PAYMENT_TRAN_MASK) && !(Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK)) {
			if (pGcf_FileHed->usCurNoPayGCN > 0) {
				pGcf_FileHed->usCurNoPayGCN--;
			}
		}
    } else if (sStatus == GCF_NOT_IN) {
		char xBuff[128];
		sprintf(xBuff, "Gcf_StoreDataFH(): Unexpected GCF_NOT_IN found GCNO = %d.", Gcf_IndexFile.usGCNO);
		NHPOS_ASSERT_TEXT(0, xBuff);
		PifLog(MODULE_GCF, LOG_ERROR_GCF_NOTLOCKED_ERR);
		PifLog(MODULE_DATA_VALUE(MODULE_GCF), usGCNumber);
		PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);

		// Try and free a que, if we cant, the we will return that the file
		// is full
		sqStatus = Gcf_FreeQueTakeChain (pGcf_FileHed, &Gcf_IndexFile, usBlock, &ulHitPoint);
		if (sqStatus == GCF_FULL) {
			NHPOS_ASSERT(!"GCF FULL ERROR, Gcf_FreeQueTakeChain () could not free up space.");
			PifLog(MODULE_GCF, LOG_ERROR_GCF_FULL_ERR_09);
			return sqStatus;
		}

        if (0 == (usStatus & GCF_PAYMENT_TRAN_MASK)) {       /* Check paid transaction */
            pGcf_FileHed->usCurNoPayGCN++;
        }
    } else {
        if (usStatus & GCF_PAYMENT_TRAN_MASK) {
			PifLog(MODULE_GCF, LOG_ERROR_GCF_FULL_ERR_09);
            return(GCF_SUCCESS);
        } else {
			NHPOS_ASSERT(!"GCF FULL ERROR, unknown status");
			PifLog(MODULE_GCF, LOG_ERROR_GCF_FULL_ERR_08);
			PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);
            return(GCF_FULL);
        }
    }

	if (0 >= Gcf_IndexFile.offusReserveBlockNo) {
		// Try and free a que, if we cant, the we will return that the file
		// is full
        usBlock = ( USHORT)(ulSize / (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC)));
        if (ulSize % (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC))) {
            usBlock++;
        }

		sqStatus = Gcf_FreeQueTakeChain (pGcf_FileHed, &Gcf_IndexFile, usBlock, &ulHitPoint);
		if (sqStatus == GCF_FULL) {
			NHPOS_ASSERT(!"GCF FULL ERROR, Gcf_FreeQueTakeChain () could not free up space.");
			PifLog(MODULE_GCF, LOG_ERROR_GCF_FULL_ERR_08);
			PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);
			return sqStatus;
		}
	}

    //Set appropriate flag for the index flag
    if ((usStatus & GCF_DRIVE_THROUGH_MASK) && (usType != GCF_DRIVE_TH_TENDER1) && (usType != GCF_DRIVE_TH_TENDER2)){
        Gcf_IndexFile.fbContFlag |= GCF_DRIVE_THROUGH_MASK;  /* Set DRIVE flag */
    }  else {
        Gcf_IndexFile.fbContFlag &= ~GCF_DRIVE_THROUGH_MASK; /* Reset DRIVE flag */
    }
    
    if (0 != (usStatus & GCF_PAYMENT_TRAN_MASK)) {
        Gcf_IndexFile.fbContFlag |= GCF_PAYMENT_TRAN_MASK;  /* Set paid transaction */
    }  else {
        Gcf_IndexFile.fbContFlag &= ~GCF_PAYMENT_TRAN_MASK; /* Reset paid transaction flag */
    }
    
	/* Copy Receive data to the Guest Check File  */
    if (ulSize) {
        /*
			Find the first buffer in the reserve chain for this guest check
			entry and then copy the transaction data from the temporary file
			into the Guest Check File, one block at a time.
		 */
        offulFirstBlock = pGcf_FileHed->offulGCDataFile + ((ULONG)(Gcf_IndexFile.offusReserveBlockNo - 1) * GCF_DATA_BLOCK_SIZE);
        offulBlockPosition = offulFirstBlock;

        Gcf_ReadFile(offulFirstBlock, &GcfDataHeadFirst, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);

        usFreeStartBlock = GcfDataHeadFirst.offusNextBKNO;  /* next que id */
        usLastBlockNo    = GcfDataHeadFirst.offusSelfBKNO;  /* save last block no */
        GcfDataHeadFirst.offusSelfBKNO = Gcf_IndexFile.offusReserveBlockNo; /* set self block no */
        GcfDataHeadFirst.usDataLen = ulSize;

        Gcf_ReadFileFH(offulReadPoint, uchSndBuffer + sizeof(GCF_DATAHEAD), (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD)), hsFileHandle);

		offulReadPoint += GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD);

		*((GCF_DATAHEAD *)uchSndBuffer) = GcfDataHeadFirst;
		GcfDataHead = GcfDataHeadFirst;

        Gcf_WriteFile(offulFirstBlock, uchSndBuffer, GCF_DATA_BLOCK_SIZE);

		if (ulSize <= GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD)) {
			ulSize = 0;
		}
		else {
			ulSize -= GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD);
		}

		// continue copying any remaining transaction data into the Guest Check File 
        for ( cuchi = 1; ulSize > 0; cuchi++) {
            if (0 == usFreeStartBlock) {
				NHPOS_ASSERT(0 != usFreeStartBlock);
				PifLog(MODULE_GCF, FAULT_ERROR_BLOCK_FULL);  /* BLOCK IS FULL */
				PifLog(MODULE_DATA_VALUE(MODULE_GCF), usGCNumber);
				PifLog(MODULE_DATA_VALUE(MODULE_GCF), cuchi);  /* BLOCK IS FULL */
				PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);  /* BLOCK IS FULL */
                PifAbort(MODULE_GCF, FAULT_ERROR_BLOCK_FULL);  /* BLOCK IS FULL */
            }

            offulBlockPosition = pGcf_FileHed->offulGCDataFile + ((ULONG)(usFreeStartBlock - 1) * GCF_DATA_BLOCK_SIZE);
            
            Gcf_ReadFile(offulBlockPosition, &GcfDataHead, sizeof(GCF_DATAHEAD_SEC), GCF_SEEK_READ);

            usFreeStartBlock = GcfDataHead.offusNextBKNO;          /* next que id */

            if (ulSize > (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC))) {
                Gcf_ReadFileFH(offulReadPoint, uchSndBuffer, (SHORT)(GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC)), hsFileHandle);
                offulReadPoint += GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC); 
                PifWriteFile(hsGcfFile, uchSndBuffer, (USHORT)(GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC))); 
                ulSize -= GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC);
            } else {
                Gcf_ReadFileFH(offulReadPoint, uchSndBuffer, ulSize, hsFileHandle);
                PifWriteFile(hsGcfFile, uchSndBuffer, ulSize); 
                break;
            }
        }

        GcfDataHeadFirst.offusSelfBKNO = GcfDataHead.offusSelfBKNO;  /* set last block no */                
        Gcf_WriteFile(offulFirstBlock, &GcfDataHeadFirst, sizeof(GCF_DATAHEAD_SEC)); 

        Gcf_IndexFile.offusBlockNo = Gcf_IndexFile.offusReserveBlockNo;
        Gcf_IndexFile.offusReserveBlockNo = GcfDataHead.offusNextBKNO;

        if ( 0 !=  Gcf_IndexFile.offusReserveBlockNo )  { /* set last block no for reserve queue */
            offulFirstBlock = pGcf_FileHed->offulGCDataFile + ((ULONG)(Gcf_IndexFile.offusReserveBlockNo - 1) * GCF_DATA_BLOCK_SIZE);
            Gcf_ReadFile(offulFirstBlock, &GcfDataHeadFirst, sizeof(GCF_DATAHEAD_SEC), GCF_SEEK_READ);

            GcfDataHeadFirst.offusSelfBKNO = usLastBlockNo;
            Gcf_WriteFile(offulFirstBlock, &GcfDataHeadFirst, sizeof(GCF_DATAHEAD_SEC));
        }

        GcfDataHead.offusNextBKNO = 0;          /* last queue id */
        Gcf_WriteFile(offulBlockPosition, &GcfDataHead, sizeof(GCF_DATAHEAD_SEC));

    }
    if (0 == (Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK )) {
        Gcf_FreeQueReturn(pGcf_FileHed, &Gcf_IndexFile.offusReserveBlockNo);
    }

    Gcf_WriteFile(ulHitPoint, &Gcf_IndexFile, sizeof(GCF_INDEXFILE));       /* Write Index File */
    
    /* Check Data type ****  works fine up to here  ****  */
        if (0 != (usStatus & GCF_DRIVE_THROUGH_MASK)) {
            switch (usType) {
            case GCF_DRIVE_THROUGH_STORE:
                Gcf_QueDrive(pGcf_FileHed, usGCNumber, GCF_PAYMENT_1);
                break;

            case GCF_DRIVE_DELIVERY1:
                Gcf_QueDrive(pGcf_FileHed, usGCNumber, GCF_PAYMENT_1);
                Gcf_QueDrive(pGcf_FileHed, usGCNumber, GCF_DELIVERY_1);
                break;

            case GCF_DRIVE_THROUGH2:
                Gcf_QueDrive(pGcf_FileHed, usGCNumber, GCF_PAYMENT_2);
                break;

            case GCF_DRIVE_DELIVERY2:
                Gcf_QueDrive(pGcf_FileHed, usGCNumber, GCF_PAYMENT_2);
                Gcf_QueDrive(pGcf_FileHed, usGCNumber, GCF_DELIVERY_2);
                break;

            case GCF_DRIVE_TH_TENDER1:
                Gcf_RemoveQueDrive(pGcf_FileHed, usGCNumber, GCF_PAYMENT_1);
                Gcf_RemoveQueDrive(pGcf_FileHed, usGCNumber, GCF_DELIVERY_1);
                break;

            case GCF_DRIVE_DE_TENDER1:
                Gcf_RemoveQueDrive(pGcf_FileHed, usGCNumber, GCF_PAYMENT_1);
                if (Gcf_DriveIndexSer(pGcf_FileHed->usCurDrive[GCF_DELIVERY_1], 
                                      usGCNumber, &ulHitPoint, pGcf_FileHed->offulDrvNOFile[GCF_DELIVERY_1])
                    == GCF_NOT_IN) {
                    Gcf_QueDrive(pGcf_FileHed, usGCNumber, GCF_DELIVERY_1);
                }
                break;

            case GCF_DRIVE_TH_TENDER2:
                Gcf_RemoveQueDrive(pGcf_FileHed, usGCNumber, GCF_PAYMENT_2);
                Gcf_RemoveQueDrive(pGcf_FileHed, usGCNumber, GCF_DELIVERY_2);
                break;

            case GCF_DRIVE_DE_TENDER2:
                Gcf_RemoveQueDrive(pGcf_FileHed, usGCNumber, GCF_PAYMENT_2);
                if (Gcf_DriveIndexSer(pGcf_FileHed->usCurDrive[GCF_DELIVERY_2], 
                                      usGCNumber, &ulHitPoint, pGcf_FileHed->offulDrvNOFile[GCF_DELIVERY_2])
                    == GCF_NOT_IN) {
                    Gcf_QueDrive(pGcf_FileHed, usGCNumber, GCF_DELIVERY_2);
                }
                break;

            case GCF_DRIVE_PUSH_THROUGH_STORE:
                Gcf_PrependQueDrive(pGcf_FileHed, usGCNumber, GCF_PAYMENT_1);
                break;

            case GCF_DRIVE_PUSH_DELIVERY1:
                Gcf_PrependQueDrive(pGcf_FileHed, usGCNumber, GCF_PAYMENT_1);
                Gcf_PrependQueDrive(pGcf_FileHed, usGCNumber, GCF_DELIVERY_1);
                break;

            case GCF_DRIVE_PUSH_THROUGH2:
                Gcf_PrependQueDrive(pGcf_FileHed, usGCNumber, GCF_PAYMENT_2);
                break;

            case GCF_DRIVE_PUSH_DELIVERY2:
                Gcf_PrependQueDrive(pGcf_FileHed, usGCNumber, GCF_PAYMENT_2);
                Gcf_PrependQueDrive(pGcf_FileHed, usGCNumber, GCF_DELIVERY_2);
                break;

			default:
				PifLog (MODULE_GCF, LOG_ERROR_GCF_INVALID_DRIVETHRU);
				break;
            }
            return(GCF_SUCCESS);
        } 

    return(GCF_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   SHORT     Gcf_DataBlockCopyFH(GCF_FILEHED *pGcf_FileHed, 
*                                               USHORT      usStartBlockNO, 
*                                               SHORT       hsFileHandle,
*                                               USHORT      usStartPoint,
*                                               USHORT      usRcvSize, 
*                                               USHORT      *pusCopyBlock)
*
*       Input:    GCF_FILEHED  *pGcf_FileHed  File Headder.
*                 USHORT usStartBlockNO       Start Block NO to be read.
*                 USHORT usStartPoint         Start pointer to be stored.
*                 USHORT usRcvSize            Maximum number of bytes to be read.
*
*      Output:    SHORT  hsFileHandle         Storage file handle.
*                 USHORT *pusCopyBlock        Number of Block to be copied.
*                 
*       InOut:    Nothing
*
**  Return    :   Read Size.
*
**  Description:
*               Read a guest check transaction from the Guest Check File block by block
*               writing each block, without the block header, to the specified file
*               creating a file with the complete transaction, de-blocked.. 
*==========================================================================
*/
SHORT   Gcf_DataBlockCopyFH(GCF_FILEHED *pGcf_FileHed, 
                            GCNUM      usStartBlockNO, 
                            SHORT       hsFileHandle, 
                            USHORT      usStartPoint, 
                            USHORT      usRcvSize,
                            USHORT      *pusCopyBlock,
							ULONG		*pulBytesRead)
{                                       

    UCHAR           uchRcvBuffer[GCF_DATABLOCK];
    UCHAR           *puchRcvBuffer;
    ULONG           ulWorkSize,  ulWorkRetSize;
    ULONG           offulBlockPoint;
    ULONG           offulWritePoint;
    GCF_DATAHEAD    GcfDataHead;
    GCF_DATAHEAD        *pGcfDataHead = (GCF_DATAHEAD *) uchRcvBuffer;
    GCF_DATAHEAD_SEC    *pGcfDataHeadSec = (GCF_DATAHEAD_SEC *) uchRcvBuffer;

    offulWritePoint = ( ULONG)usStartPoint;
    *pusCopyBlock = 0;
	//RPH SR# 201
	*pulBytesRead = 0;

    if (0 == usStartBlockNO) {
		return(0);
    }

    offulBlockPoint = pGcf_FileHed->offulGCDataFile + ((usStartBlockNO - 1) * GCF_DATA_BLOCK_SIZE);

    Gcf_ReadFile(offulBlockPoint, &GcfDataHead, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);

    if (0 == GcfDataHead.usDataLen){
		return(0);                 /* Return Len = 0 */
    }

    if (0 == usRcvSize ) {   /* If lock only, then get block count */
        for ( ulWorkSize = 0; GcfDataHead.usDataLen != 0 ; ulWorkSize++ ){
            (*pusCopyBlock)++;
            if (ulWorkSize) {
                if ( GcfDataHead.usDataLen < (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC)) ) {
                    break;
                }
                GcfDataHead.usDataLen -= (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC));
            }  else {
                if ( GcfDataHead.usDataLen < (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD)) ) {
                    break;
                }
                GcfDataHead.usDataLen -= (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD));
           }
        }
		return(0);    /* Return len = 0 */
    }

	NHPOS_ASSERT(GcfDataHead.usDataLen <= usRcvSize);

    if (GcfDataHead.usDataLen > usRcvSize) {
        ulWorkSize = ulWorkRetSize = usRcvSize;
    } else {
        ulWorkSize = ulWorkRetSize = GcfDataHead.usDataLen;
    }
    if (ulWorkSize <= (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD))) {
        (*pusCopyBlock)++;
        Gcf_ReadFile(0L, uchRcvBuffer, ulWorkSize, GCF_CONT_READ);
        Gcf_WriteFileFH(offulWritePoint, uchRcvBuffer, ulWorkSize, hsFileHandle);
        //RPH SR# 201
		*pulBytesRead = ulWorkRetSize;
		NHPOS_ASSERT(ulWorkRetSize <= 0x7fff);//MAXSHORT
		return (SHORT)(ulWorkRetSize & 0x7fff);
    }

    Gcf_ReadFile(0L, uchRcvBuffer, GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD), GCF_CONT_READ);
    Gcf_WriteFileFH(offulWritePoint, uchRcvBuffer, GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD), hsFileHandle);
    ulWorkSize    -= GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD);
    offulWritePoint += GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD);
    (*pusCopyBlock)++;

	puchRcvBuffer = uchRcvBuffer + sizeof(GCF_DATAHEAD_SEC);

    for (;;) {
        (*pusCopyBlock)++;

        if (0 == GcfDataHead.offusNextBKNO) {
			NHPOS_ASSERT(0 != GcfDataHead.offusNextBKNO);
            PifLog(MODULE_GCF, FAULT_ERROR_BLOCK_FULL);  /* BLOCK IS FULL */
            PifLog(MODULE_DATA_VALUE(MODULE_GCF), usStartBlockNO);  /* BLOCK IS FULL */
            PifLog(MODULE_DATA_VALUE(MODULE_GCF), *pusCopyBlock);  /* BLOCK IS FULL */
            PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);  /* BLOCK IS FULL */
            PifAbort(MODULE_GCF, FAULT_ERROR_BLOCK_FULL);  /* BLOCK IS FULL */
        }

        offulBlockPoint = pGcf_FileHed->offulGCDataFile + ((GcfDataHead.offusNextBKNO - 1) * GCF_DATA_BLOCK_SIZE);

        Gcf_ReadFile(offulBlockPoint, uchRcvBuffer, GCF_DATA_BLOCK_SIZE, GCF_SEEK_READ);

		GcfDataHead = *pGcfDataHead;

        if (ulWorkSize <= (GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC))) {
            Gcf_WriteFileFH(offulWritePoint, puchRcvBuffer, ulWorkSize, hsFileHandle);
            break;
        }

        Gcf_WriteFileFH(offulWritePoint, puchRcvBuffer, GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC), hsFileHandle);

        ulWorkSize -= GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC);
        offulWritePoint += GCF_DATA_BLOCK_SIZE - sizeof(GCF_DATAHEAD_SEC);
    }
	//RPH SR# 201
	*pulBytesRead = ulWorkRetSize;
	NHPOS_ASSERT(ulWorkRetSize <= 0x7fff);//MAXSHORT
	return(SHORT)(ulWorkRetSize & 0x7fff);
}

/*
*==========================================================================
**    Synopsis:   VOID Gcf_ReadFileFH(ULONG   offulSeekPos, 
*                                     VOID    *pReadBuffer, 
*                                     ULONG   ulReadSize, 
*                                     SHORT   hsFileHandle)
*
*       Input:    ULONG     offulSeekPos   Seek value from file head
*                 ULONG     ulReadSize     Read buffer size 
*                 SHORT     hsFileHandle   File handle of file to be read data
*                 
*      Output:    VOID      *pReadBuffer   Read buffer
*       InOut:    Nothing
*
**  Return   :    GCF_SUCCESS
*
**  Description:
*               Read data from GCF.              
*==========================================================================
*/
SHORT   Gcf_ReadFileFH(ULONG      offulSeekPos,
                       VOID       *pReadBuffer, 
                       ULONG      ulReadSize,
                       SHORT      hsFileHandle)
{
	SHORT   sRetStatus = 0;
    ULONG   ulActualPosition;
	ULONG	ulActualBytesRead; //11-10-3 RPH

    sRetStatus = PifSeekFile(hsFileHandle, offulSeekPos, &ulActualPosition);
    if (sRetStatus < 0) {
        PifLog(MODULE_GCF, FAULT_ERROR_FILE_SEEK);
        PifLog(MODULE_GCF, (USHORT)abs(sRetStatus));
        PifAbort(MODULE_GCF, FAULT_ERROR_FILE_SEEK);
		return(GCF_FATAL);
    }

	//11-10-3 RPH Changes for PifReadFile
	PifReadFile(hsFileHandle, pReadBuffer, ulReadSize, &ulActualBytesRead);
    if ( ulActualBytesRead != ulReadSize) {
        PifAbort(MODULE_GCF, FAULT_ERROR_FILE_READ);
		return(GCF_FATAL);
    }

    return(GCF_SUCCESS);
}

/*
*==========================================================================
**    Synopsis:   VOID Gcf_WriteFileFH(ULONG  offulSeekPos, 
*                                      VOID   *pWriteBuffer, 
*                                      USHORT usWriteSize
*                                      SHORT  hsFileHandle)
*
*       Input:    ULONG     offulSeekPos   Seek value from file head for writing.
*                 VOID      *pWriteBuffer  Write buffer
*                 USHORT    usWriteSize    Write buffer size 
*                 
*      Output:    Nothing 
*       InOut:    SHORT     hsFileHandle   File handle of file to be written data 
*
**  Return   :    Nothing
*
**  Description:
*               Write Data to Source File(File Handle I/F Version).               
*==========================================================================
*/                          
SHORT Gcf_WriteFileFH(ULONG  offulSeekPos,
                     VOID   *pWriteBuffer,
                     ULONG  ulWriteSize,
                     SHORT  hsFileHandle)
{
	SHORT   sRetStatus = 0;
    ULONG   ulActualPosition;           /* for seek */

    sRetStatus = PifSeekFile(hsFileHandle, offulSeekPos, &ulActualPosition);
    if (sRetStatus < 0) {
        PifLog(MODULE_GCF, FAULT_ERROR_FILE_SEEK);
        PifLog(MODULE_GCF, (USHORT)abs(sRetStatus));
        PifAbort(MODULE_GCF, FAULT_ERROR_FILE_SEEK);
		return(GCF_FATAL);
    }
    if (!ulWriteSize) {      /* PifWriteFile() is occurred system-error by read-size = 0 */
        return(GCF_SUCCESS);
    }
    PifWriteFile(hsFileHandle, pWriteBuffer, ulWriteSize);
    return(GCF_SUCCESS);
}

/******************************** Add R3.0 ****************************/
/*
*==========================================================================
**    Synopsis:   USHORT Gcf_CheckPayment(ULONG  offulSeekPos, 
*                                       SHORT  hsFileHandle)
*
*       Input:    ULONG     offulSeekPos   Seek value from file head for writing.
*                 SHORT     hsFileHandle   File Handle 
*                 
*      Output:    Nothing 
*
**  Return   :    GCF_PAID      Paid transaction
*                 GCF_NOT_PAID  Not paid transaction
*
**  Description:
*               Check Paid Transaction.               
*==========================================================================
*/                          
USHORT Gcf_CheckPayment(ULONG  offulSeekPos, SHORT  hsFileHandle)
{
    TRANGCFQUAL TranQual;

    Gcf_ReadFileFH(offulSeekPos, &TranQual, sizeof(TRANGCFQUAL), hsFileHandle);
    if (TranQual.fsGCFStatus2 & GCFQUAL_PAYMENT_TRANS) {
        return(GCF_PAID);
    }
    return(GCF_NOT_PAID);
}

/*
*==========================================================================
**    Synopsis:   SHORT Gcf_QueDrive(GCF_FILEHED *pGcf_FileHed, 
*                                   USHORT  usGCNumber,
*                                   USHORT  usQueNumber)
*
*       Input:    GCF_FILEHED  *pGcf_FileHed  File Headder.
*                 USHORT       usGCNumber     GC Number 
*                 USHORT       usQueNumber    Queue Number
*                 
*      Output:    Nothing 
*
**  Return   :    status from Gcf_DriveIndexSer() search for guest check number
*                 GCF_SUCCESS -> indicates GC was in queue and was removed
*                 GCF_NOT_IN  -> indicates GC was not in queue
*
**  Description:
*               Queued GC to Drive through queue.               
*==========================================================================
*/                          
SHORT Gcf_QueDrive(GCF_FILEHED *pGcf_FileHed, 
                  GCNUM  usGCNumber,
                  USHORT  usQueNumber)
{
    ULONG           ulHitPoint;
	SHORT    sRetStatus;

    sRetStatus = Gcf_DriveIndexSer(pGcf_FileHed->usCurDrive[usQueNumber], usGCNumber, &ulHitPoint, pGcf_FileHed->offulDrvNOFile[usQueNumber]);

    if (sRetStatus == GCF_SUCCESS) {
		// if the guest check number is already in the queue then we
		// delete it from where it is because we are going to add it
		// to the end of the queue.
		Gcf_DriveIndexDeleteSpl (pGcf_FileHed, usQueNumber, ulHitPoint);
    }

	// write the GC that is in the Drive thru que to the file
	// by adding it to the end of the queue.
    Gcf_WriteFile(pGcf_FileHed->offulDrvNOFile[usQueNumber] + (pGcf_FileHed->usCurDrive[usQueNumber] * sizeof(GCNUM)), &usGCNumber, sizeof(GCNUM));
	NHPOS_ASSERT(pGcf_FileHed->usCurDrive[usQueNumber] < GCF_MAX_DRIVE_NUMBER);
    pGcf_FileHed->usCurDrive[usQueNumber]++;

	return sRetStatus;
}

SHORT Gcf_PrependQueDrive(GCF_FILEHED *pGcf_FileHed, GCNUM  usGCNumber, USHORT  usQueNumber)
{
    ULONG    ulHitPoint;
	SHORT    sRetStatus;

    sRetStatus = Gcf_DriveIndexSer(pGcf_FileHed->usCurDrive[usQueNumber], usGCNumber, &ulHitPoint, pGcf_FileHed->offulDrvNOFile[usQueNumber]);

    if (sRetStatus == GCF_SUCCESS) {
		// if the guest check number is already in the queue then we
		// delete it from where it is because we are going to add it
		// to the beginning of the queue.
		Gcf_DriveIndexDeleteSpl (pGcf_FileHed, usQueNumber, ulHitPoint);
    }

	// write the GC that is in the Drive thru que to the file
	// by adding it to the beginning of the queue.
	Gcf_InsertTableinFile (pGcf_FileHed->offulDrvNOFile[usQueNumber], pGcf_FileHed->usCurDrive[usQueNumber], pGcf_FileHed->offulDrvNOFile[usQueNumber], &usGCNumber, sizeof(GCNUM));

	NHPOS_ASSERT(pGcf_FileHed->usCurDrive[usQueNumber] < GCF_MAX_DRIVE_NUMBER);
    pGcf_FileHed->usCurDrive[usQueNumber]++;

	return sRetStatus;
}

/*
*==========================================================================
**    Synopsis:   SHORT Gcf_RemoveQueDrive(GCF_FILEHED *pGcf_FileHed, 
*                                         USHORT  usGCNumber,
*                                         USHORT  usQueNumber)
*
*       Input:    GCF_FILEHED  *pGcf_FileHed  File Headder.
*                 USHORT       usGCNumber     GC Number 
*                 USHORT       usQueNumber    Queue Number
*                 
*      Output:    Nothing 
*
**  Return   :    status from Gcf_DriveIndexSer() search for guest check number
*                 GCF_SUCCESS -> indicates GC was in queue and was removed
*                 GCF_NOT_IN  -> indicates GC was not in queue
*
**  Description:
*               Queued GC to Drive through queue.               
*==========================================================================
*/                          
SHORT Gcf_RemoveQueDrive(GCF_FILEHED *pGcf_FileHed, GCNUM  usGCNumber, USHORT  usQueNumber)
{
    ULONG           ulHitPoint;
	SHORT    sRetStatus;

	//look into the index and see if the guest check exists
    sRetStatus = Gcf_DriveIndexSer(pGcf_FileHed->usCurDrive[usQueNumber], usGCNumber, &ulHitPoint, pGcf_FileHed->offulDrvNOFile[usQueNumber]);

    if (sRetStatus == GCF_SUCCESS) {
		Gcf_DriveIndexDeleteSpl (pGcf_FileHed, usQueNumber, ulHitPoint);
    }

	return sRetStatus;
}

/*
*==========================================================================
**    Synopsis:   SHORT Gcf_DeletePaidTran(GCF_FILEHED *pGcf_FileHed, 
*                                         USHORT  usGCNumber)
*
*       Input:    GCF_FILEHED  *pGcf_FileHed  File Headder.
*                 USHORT       usGCNumber     GC Number 
*
*      Output:    Nothing 
*
**  Return   :    Nothing
*
**  Description:
*               Delete paid transaction.  This function is used in a number
*               of places to delete paid transaction when the guest check
*               file is full.  This operation will then allow additional
*               guest checks to be stored.
*
*				The payed guest check numbers are put onto the payed guest check
*				queue at the end of the queue (take a look in routine GusStoreFileFH ()
*				above in which payed items are added to the payed check queue.
*
*				There are two reasons this function is called.
*				1. There is insufficient free space for a new guest check to be stored.
*				2. The max number of guest checks exists so one must be deleted.
*==========================================================================
*/
static Gcf_CountBlocksInChain (USHORT usBlockNo, ULONG offulGCDataFile)
{
	GCF_DATAHEAD    GcfDataHed;
	ULONG           ulOffset;
	USHORT          usBlockCount = 0;

	// Lets ensure that we do not end up in an infinite loop
	// if there should be some kind of a problem with the links
	// in the chain of blocks.
	while (usBlockNo && usBlockCount < 20) {
		usBlockCount++;
		ulOffset = offulGCDataFile + ((usBlockNo - 1) * GCF_DATA_BLOCK_SIZE);
		Gcf_ReadFile( ulOffset, &GcfDataHed, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);
		usBlockNo = GcfDataHed.offusNextBKNO;
	}

	return usBlockCount;
}

SHORT Gcf_DeletePaidTran(GCF_FILEHED *pGcf_FileHed, GCNUM  usGCNumber)
{
	GCNUM           GcnumArray[10];
    GCF_INDEXFILE   Gcf_IndexFile;
    ULONG           offulIndexPoint;
    GCNUM           usPayGCNumber;
	USHORT			usNoBlock, usBlockCount = 0;
	SHORT			i, iIndex;

    if (pGcf_FileHed->usSystemtype != FLEX_STORE_RECALL || pGcf_FileHed->offulPayTranNO == 0) {
		NHPOS_ASSERT(pGcf_FileHed->offulPayTranNO != 0);
		NHPOS_ASSERT(!"Gcf_DeletePaidTran(), not drive through system.");
        return(GCF_FULL);
    }

	// We will determine the number of blocks needed for a guest check
	// then determine how many blocks are on the free chain.
	Gcf_GetNumberofBlockAssign(pGcf_FileHed->usSystemtype, &usNoBlock);
	usBlockCount = Gcf_CountBlocksInChain (pGcf_FileHed->offusFreeGC, pGcf_FileHed->offulGCDataFile);

	if ((usBlockCount >= usNoBlock) && (pGcf_FileHed->usMaxGCN > pGcf_FileHed->usCurGCN)) {
		return(GCF_SUCCESS);
	}

    Gcf_ReadFile(pGcf_FileHed->offulPayTranNO, GcnumArray, sizeof(GcnumArray), GCF_SEEK_READ);

	// Now go through and delete the number of guest checks we need to delete in order
	// to be able to provide the number of blocks needed to service this request.
    for ( iIndex = i = 0; i < pGcf_FileHed->usCurPayGC; i++, iIndex++) {
        usPayGCNumber = GcnumArray[iIndex];

        Gcf_IndexFile.usGCNO = usPayGCNumber;
		if (usPayGCNumber == usGCNumber) {
			continue;
		}

        if (Gcf_Index(pGcf_FileHed, GCF_SEARCH_ONLY, &Gcf_IndexFile, &offulIndexPoint) == GCF_SUCCESS) {
            if ((Gcf_IndexFile.fbContFlag & GCF_PAYMENT_TRAN_MASK) &&
                !(Gcf_IndexFile.fbContFlag & GCF_READ_FLAG_MASK) &&
                !(Gcf_IndexFile.fbContFlag & GCF_DRIVE_THROUGH_MASK)) {

				usBlockCount += Gcf_CountBlocksInChain (Gcf_IndexFile.offusReserveBlockNo, pGcf_FileHed->offulGCDataFile);
                Gcf_FreeQueReturn(pGcf_FileHed, &Gcf_IndexFile.offusReserveBlockNo);
				usBlockCount += Gcf_CountBlocksInChain (Gcf_IndexFile.offusBlockNo, pGcf_FileHed->offulGCDataFile);
                Gcf_FreeQueReturn(pGcf_FileHed, &Gcf_IndexFile.offusBlockNo);

                // Gcf_IndexDel(pGcf_FileHed, &Gcf_IndexFile);
				// Go ahead and delete the index rather than going through the lookup
				// procedure all over again.  We already know the offset for this index.
				NHPOS_ASSERT(pGcf_FileHed->usCurGCN > 0);
				if (pGcf_FileHed->usCurGCN > 0) {
					pGcf_FileHed->usCurGCN--;
					Gcf_DeleteTableinFile(pGcf_FileHed->offulGCIndFile, pGcf_FileHed->usCurGCN, offulIndexPoint, sizeof(GCF_INDEXFILE));
				}

				NHPOS_ASSERT(pGcf_FileHed->usCurPayGC > 0);
				if (pGcf_FileHed->usCurPayGC > 0) {   /* Delete Payment Queue */
					pGcf_FileHed->usCurPayGC--;
					Gcf_DeleteTableinFile(pGcf_FileHed->offulPayTranNO, pGcf_FileHed->usCurPayGC, pGcf_FileHed->offulPayTranNO + (i * sizeof(GCNUM)), sizeof(GCNUM));
					// Since we have deleted the current index, we need to decrement
					// the counter so that we do not skip over the index that has been
					// shifted down in the queue.
					i--;
				}
            }

			if ((usBlockCount >= usNoBlock) && (pGcf_FileHed->usMaxGCN > pGcf_FileHed->usCurGCN)) {
				return(GCF_SUCCESS);
			}
        }
		else {
			// if this guest check number is in the payed out queue but is not in the guest check file
			// then we will delete it from the payed out queu as well.
			NHPOS_ASSERT(pGcf_FileHed->usCurPayGC > 0);
			if (pGcf_FileHed->usCurPayGC > 0) {   /* Delete Payment Queue */
				pGcf_FileHed->usCurPayGC--;
				Gcf_DeleteTableinFile(pGcf_FileHed->offulPayTranNO, pGcf_FileHed->usCurPayGC, pGcf_FileHed->offulPayTranNO + (i * sizeof(GCNUM)), sizeof(GCNUM));
				// Since we have deleted the current index, we need to decrement
				// the counter so that we do not skip over the index that has been
				// shifted down in the queue.
				i--;
			}
		}
    }

	if ((usBlockCount >= usNoBlock) && (pGcf_FileHed->usMaxGCN > pGcf_FileHed->usCurGCN)) {
		return(GCF_SUCCESS);
	}

	NHPOS_ASSERT(!"Gcf_DeletePaidTran(), could not find corresponding GC Number");

	PifLog(MODULE_GCF, LOG_ERROR_GCF_MAX_LT_CUR);
    PifLog(MODULE_DATA_VALUE(MODULE_GCF), pGcf_FileHed->usMaxGCN);
    PifLog(MODULE_DATA_VALUE(MODULE_GCF), pGcf_FileHed->usCurGCN);
    PifLog(MODULE_DATA_VALUE(MODULE_GCF), pGcf_FileHed->usCurPayGC);
    PifLog(MODULE_LINE_NO(MODULE_GCF), (USHORT)__LINE__);
	return(GCF_FULL);
}

/*
	Gcf_WalkGcfDataFile - walk the Guest Check File data structures to measure various
	statistics on the usage of the guest check data storage.

	Usage:  Insert a call to this function where ever you would like to collect
			statistics on the use of the guest check data file.  Statistics measured
			include:
				- copy of the Guest Check Index entries
				- copy of header of first block in the Guest Check In-Use chain
				- copy of header of first block in the Guest Check Reserve chain
				- count of number of blocks in the In-Use chain
				- count of number of blocks in the Reserve chain
				- count of number of fragments in In-use chain
				- count of number of fragments in Reserve chain
				- widest distance between blocks in In-Use chain
				- widest distance between blocks in Reserve chain

	What the statistics collected mean.
		The number of block counts indicate the number of blocks on the particular
		chain of blocks.  This measure indicates the number of blocks which are
		in use by a particular guest check.  The lifecycle of a guest check
		will mean that in cases such as when a guest check is finalized, one or the
		other chains will have zero blocks because the excess blocks have been released
		back to be reused.

		The cont of number of fragments provides an idea as to how many of the blocks
		in a chain are in sequential order.  The premise is that if the blocks in a
		chain are in sequential order, the disk I/O will be more efficient especially
		when a requested block is already in a disk cache because it was read as part
		of reading a previously requested block.

		The same premise underlies the widest distance statistics.  If the distance
		between blocks in a chain are small, the chances of a cache hit is greater
		than if the distances are great.
 */
USHORT Gcf_OptimizeGcfDataFile (GCF_FILEHED *pGcf_FileHed,
							GCF_TALLY_INDEX *pTallyArray, USHORT nArrayItems, USHORT usOptimize);

USHORT Gcf_WalkGcfDataFile (GCF_FILEHED *pGcf_FileHed,
							GCF_TALLY_INDEX *pTallyArray, USHORT nArrayItems)
{
	return Gcf_OptimizeGcfDataFile (pGcf_FileHed, pTallyArray, nArrayItems, 0);
}
	
USHORT Gcf_OptimizeGcfDataFile (GCF_FILEHED *pGcf_FileHed,
							GCF_TALLY_INDEX *pTallyArray, USHORT nArrayItems, USHORT usOptimize)
{
	ULONG           ulOffset;
	GCF_INDEXFILE   gcf_Index;
	GCF_DATAHEAD    GcfDataHed;
	USHORT          usCount = 0, usTemp = 0, usTemp1, usTemp2;
	USHORT          usBlockNo = 0;
	USHORT          usBlockList[40], usBlockListCount;
	USHORT			usGlobalUnPaidCount = 0;
	USHORT			usGlobalPaidCount = 0;
	USHORT			usGlobalBlocksCountInUse = 0;
	USHORT			usGlobalBlocksCountReserved = 0;
	USHORT			usGlobalBlocksCountFree = 0;
	USHORT			usGlobalTotalBlocks = 0;
	ULONG           *ulIndexStatus = 0;
	ULONG			ulGlobalTotalBytes = 0;
	ULONG           ulGcfAreaEnd = 0;

	// The area where guest check blocks are stored are in the area
	// between Gcf_FileHed.offulGCDataFile and Gcf_FileHed.offulDrvNOFile[0] or
	// between Gcf_FileHed.offulGCDataFile and Gcf_FileHed.ulGCFSize depending on
	// whether the system has drive thru or not.
	// See GusCreateGCF() for details about the guest check creation procedure.
	if (pGcf_FileHed->offulDrvNOFile[0])
	{
		ulGcfAreaEnd = pGcf_FileHed->offulDrvNOFile[0];
	}
	else
	{
		ulGcfAreaEnd = pGcf_FileHed->ulGCFSize;
	}
	ulGlobalTotalBytes = (ulGcfAreaEnd - pGcf_FileHed->offulGCDataFile);
	NHPOS_ASSERT((ulGlobalTotalBytes / GCF_DATA_BLOCK_SIZE) < 0xffff);
	usGlobalTotalBlocks = (ulGlobalTotalBytes / GCF_DATA_BLOCK_SIZE);

	if (pGcf_FileHed->offulGCIndFile != sizeof(GCF_FILEHED))
	{
		PifLog(MODULE_GCF, LOG_ERROR_GCF_OPT_INDEX_BADOFFSET);
	}

	if (pGcf_FileHed->offulGCDataFile != (pGcf_FileHed->offulGCIndFile + pGcf_FileHed->usMaxGCN * sizeof(GCF_INDEXFILE)))
	{
		PifLog(MODULE_GCF, LOG_ERROR_GCF_OPT_DATA_BADOFFSET);
	}

	ulIndexStatus = (ULONG *)_alloca (sizeof(ULONG) * usGlobalTotalBlocks);
	memset (ulIndexStatus, 0, sizeof(ULONG) * usGlobalTotalBlocks);

	// First of all read in the index entries for the guest check file
	ulOffset = pGcf_FileHed->offulGCIndFile;
	for (usCount = 0; usCount < pGcf_FileHed->usCurGCN && usCount < nArrayItems; usCount++) {
		memset (pTallyArray + usCount, 0, sizeof(GCF_TALLY_INDEX));
		Gcf_ReadFile(ulOffset, &(pTallyArray[usCount].gcf_Index), sizeof(GCF_INDEXFILE), GCF_SEEK_READ);
		ulOffset += sizeof(GCF_INDEXFILE);
	}

	for (usTemp = 0; usTemp < usCount; usTemp++) {
		gcf_Index = pTallyArray[usTemp].gcf_Index;
		if (gcf_Index.fbContFlag & GCF_PAYMENT_TRAN_MASK)
			usGlobalPaidCount++;
		else
			usGlobalUnPaidCount++;

		// Now walk the In-use chain of blocks
		usBlockNo = gcf_Index.offusBlockNo;
		usBlockListCount = 0;
		while (usBlockNo > 0) {
			usBlockList[usBlockListCount++] = usBlockNo;
			pTallyArray[usTemp].usNoBlocksInUse++;
			ulOffset = pGcf_FileHed->offulGCDataFile + ((usBlockNo - 1) * GCF_DATA_BLOCK_SIZE);
			Gcf_ReadFile( ulOffset, &GcfDataHed, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);
			if (pTallyArray[usTemp].usNoBlocksInUse == 1) {
				pTallyArray[usTemp].gcf_InUseChain = GcfDataHed;
			}
			if (usBlockNo && (usBlockNo + 1 != GcfDataHed.offusNextBKNO)) {
				pTallyArray[usTemp].usNoFragsInUse++;
			}
			usGlobalBlocksCountInUse++;
			ulIndexStatus[usBlockNo] = usTemp | (0x10000);  // this block is known and in use
			usBlockNo = GcfDataHed.offusNextBKNO;
		}

		for (usTemp1 = 0; usTemp1 < usBlockListCount; usTemp1++) {
			USHORT usWide = 0;
			for (usTemp2 = 0; usTemp2 < usBlockListCount; usTemp2++) {
				if (usBlockList[usTemp1] < usBlockList[usTemp2]) {
					usWide = usBlockList[usTemp2] - usBlockList[usTemp1];
				}
				else {
					usWide = usBlockList[usTemp1] - usBlockList[usTemp2];
				}
				if (usWide > pTallyArray[usTemp].usWidestInUse) {
					pTallyArray[usTemp].usWidestInUse = usWide;
				}
			}
		}

		// Now walk the Reserve chain of blocks
		// We will also check to see if this is an upaid transaction
		// that has no more blocks on its reserve chain.  If so, log it.
		usBlockNo = gcf_Index.offusReserveBlockNo;
		usBlockListCount = 0;
		if (! (gcf_Index.fbContFlag & GCF_PAYMENT_TRAN_MASK) && usBlockNo == 0)
		{
			PifLog(MODULE_GCF, LOG_ERROR_GCF_OPT_UPAID_NO_RESERVE);
		}
		while (usBlockNo > 0) {
			usBlockList[usBlockListCount++] = usBlockNo;
			pTallyArray[usTemp].usNoBlocksReserve++;
			ulOffset = pGcf_FileHed->offulGCDataFile + ((usBlockNo - 1) * GCF_DATA_BLOCK_SIZE);
			Gcf_ReadFile( ulOffset, &GcfDataHed, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);
			if (pTallyArray[usTemp].usNoBlocksReserve == 1) {
				pTallyArray[usTemp].gcf_ReserveChain = GcfDataHed;
			}
			if (usBlockNo && (usBlockNo + 1 != GcfDataHed.offusNextBKNO)) {
				pTallyArray[usTemp].usNoFragsReserve++;
			}
			usGlobalBlocksCountReserved++;
			ulIndexStatus[usBlockNo] = usTemp | (0x20000);  // this block is known and reserved
			usBlockNo = GcfDataHed.offusNextBKNO;
		}

		for (usTemp1 = 0; usTemp1 < usBlockListCount; usTemp1++) {
			USHORT usWide = 0;
			for (usTemp2 = 0; usTemp2 < usBlockListCount; usTemp2++) {
				if (usBlockList[usTemp1] < usBlockList[usTemp2]) {
					usWide = usBlockList[usTemp2] - usBlockList[usTemp1];
				}
				else {
					usWide = usBlockList[usTemp1] - usBlockList[usTemp2];
				}
				if (usWide > pTallyArray[usTemp].usWidestReserve) {
					pTallyArray[usTemp].usWidestReserve = usWide;
				}
			}
		}
	}

	// Now walk the Free chain of blocks .
	usBlockNo = pGcf_FileHed->offusFreeGC;
	if (usBlockNo == 0)
	{
		PifLog(MODULE_GCF, LOG_ERROR_GCF_OPT_NO_FREE_BLOCKS);
	}
	while (usBlockNo > 0) {
		ulOffset = pGcf_FileHed->offulGCDataFile + ((usBlockNo - 1) * GCF_DATA_BLOCK_SIZE);
		Gcf_ReadFile (ulOffset, &GcfDataHed, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);
		usGlobalBlocksCountFree++;
		ulIndexStatus[usBlockNo] = 0x40000;  // this block is known and free
		usBlockNo = GcfDataHed.offusNextBKNO;
	}

	// Lets test to see if number of unpaid transactions found equals counts in header.
	if (usGlobalUnPaidCount != pGcf_FileHed->usCurNoPayGCN)
	{
		PifLog(MODULE_GCF, LOG_ERROR_GCF_OPT_UNPAID_NOBAL);
	}

	// Lets test to see if number of unpaid transactions found greater than max in header.
	if (usGlobalUnPaidCount > pGcf_FileHed->usMaxGCN)
	{
		PifLog(MODULE_GCF, LOG_ERROR_GCF_OPT_UPAID_GT_MAX);
	}

	// Lets test to see if number of paid transactions found equals counts in header.
	if (usGlobalPaidCount != pGcf_FileHed->usCurPayGC)
	{
		PifLog(MODULE_GCF, LOG_ERROR_GCF_OPT_PAID_NOBAL);
	}

	// Lets see if there are any blocks that are not on any of the 
	// block chains.  If so, then issue a PifLog ().
	if (usGlobalTotalBlocks != usGlobalBlocksCountInUse + usGlobalBlocksCountReserved + usGlobalBlocksCountFree)
	{
		PifLog(MODULE_GCF, LOG_ERROR_GCF_OPT_BLOCKS_NOBAL);
	}

	// Lets test the integrity of the block structure of the Guest Check File
	if ((ulGlobalTotalBytes % GCF_DATA_BLOCK_SIZE) != 0)
	{
		PifLog(MODULE_GCF, LOG_ERROR_GCF_OPT_BYTES_BADSIZE);
	}

	if (usOptimize)
	{
		if (usGlobalUnPaidCount > pGcf_FileHed->usMaxGCN)
		{
			usGlobalUnPaidCount = pGcf_FileHed->usMaxGCN;
		}

		if (usGlobalUnPaidCount != pGcf_FileHed->usCurNoPayGCN)
		{
			pGcf_FileHed->usCurNoPayGCN = usGlobalUnPaidCount;
		}

		if (usGlobalPaidCount != pGcf_FileHed->usCurPayGC)
		{
			pGcf_FileHed->usCurPayGC = usGlobalPaidCount;
		}

		// Check to see if there are possibly any blocks missing from the
		// various block chains.  Basically the caculated number of blocks
		// available should be equal to the blocks found while walking through
		// the various chains.  If there are any missing, we need to discover them
		// and then put them on the chain of free blocks.
		if (usGlobalTotalBlocks != usGlobalBlocksCountInUse + usGlobalBlocksCountReserved + usGlobalBlocksCountFree)
		{
			for (usBlockNo = 1, ulOffset = pGcf_FileHed->offulGCDataFile;
				ulOffset < ulGcfAreaEnd;
				ulOffset += GCF_DATA_BLOCK_SIZE, usBlockNo++)
			{
				if ((ulIndexStatus[usBlockNo] & 0xf0000) == 0)
				{
					// This block is not on any chain.  It is a fragment.
					// We will put it on the free block chain.
					Gcf_ReadFile( ulOffset, &GcfDataHed, sizeof(GCF_DATAHEAD), GCF_SEEK_READ);
					GcfDataHed.offusNextBKNO = pGcf_FileHed->offusFreeGC;
					GcfDataHed.offusSelfBKNO = usBlockNo;
					GcfDataHed.usDataLen = 0;
					Gcf_WriteFile (ulOffset, &GcfDataHed, sizeof(GCF_DATAHEAD));
					pGcf_FileHed->offusFreeGC = usBlockNo;
				}
			}
		}
	}
	return usCount;
}


/******************************** End Add R3.0 ************************/
/*=========== End of File =========*/