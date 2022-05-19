/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Compress & Decompress memory                
* Category    : Reentrant Functions Library, NCR 2170  US hospitarity Application
* Program Name: RflCmp.C                                                      
* --------------------------------------------------------------------------
* Abstruct:
*    RflZip() compress or zip the ITEM data into a TRANS data for transaction file record
*    RflZipByType() front end to RflZip() that does some preprocessing and address calculation.
*    RflUnZip() uncompress or unzip transaction file record, TRANS data, into ITEM data
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-08-92 : 00.00.01 : K.Nakajima :                                    
* 
*** NCR2171 **
* Aug-30-99 : 01.00.00  : M.Teraki   : initial (for Win32)
*
*** GENPOS **
* May-05-16 : 02.03.00  : R.Chambers : added new function RflZipByType().
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
;========================================================================
;+                                                                      +
;+                  I N C L U D E     F I L E s                         +
;+                                                                      +
;========================================================================
**/
#include <tchar.h>
#include <memory.h>

#include <ecr.h>
#include <rfl.h>
#include "rflin.h"

/**
;========================================================================
;+                                                                      +
;+        P R O G R A M    D E C L A R A T I O N s                      +
;+                                                                      +
;========================================================================
**/
/**fh
;============================================================================
;
;   function : Memory  Compaction
;
;	Format   : SHORT RflZip(VOID  *pDest,       
;                           USHORT usDestLen,    
;                           VOID  *pSource,     
;                           USHORT usSourceLen); 
;
;   input    : VOID   *pDest,         -Compressed buffer  pointer
;              USHORT usDestLen,      -Compressed buffer length
;              VOID   *pSource,       -Original buffer pointer
;              USHORT usSourceLen);   -Original buffer length
;
;   output   : VOID   *pDest          -Compresed data
;
;   return   : SHORT sWriteLeng       -RFL_FAIL       //if compaction fail
;                                     -other          //compaction data length
;
;   synopis  : This function compreses memory using "NULL suppress". 
;              Compresed data has following structure.
;
;               1 byte
;               -----------------------------
;               | bitmap | char | char |    |   ...
;               -----------------------------
;============================================================================
fh**/
SHORT RflZip(VOID *pDest, USHORT usDestLen, CONST VOID *pSource, USHORT usSourceLen)
{
    CHAR        *pchDestTop = pDest;         /* Destination buffer top */
    CONST CHAR  *pchSorcTop = pSource;       /* Source      buffer top */

    for (;;) {
		USHORT   i;                       /* Counter */
		USHORT   usWrite = 0;             /* Write byte counter */
		UCHAR    uchBitmap = 0;           /* BitMap */

        pchDestTop++;       /* for bitmap area */
        if ( usSourceLen < RFL_BYTELEN) {
            if (usSourceLen == 0) {
                break;
            }
            for ( i = 0; i < usSourceLen; i++) {
                /* check Written length */
                if ( usDestLen <= (USHORT)(pchDestTop - (CHAR *)pDest) ) {
                    return(RFL_FAIL);
                }
                if ( *(pchSorcTop) != '\0' )  {
                    /* 0x80  = "10000000" */
                    uchBitmap |= (UCHAR)(0x80 >> i);      
                    *pchDestTop = *pchSorcTop;
                    pchDestTop++;
                    usWrite++;
                }
                pchSorcTop++;        /* move read point to next byte */
            }
            /* --- write bitmap    1 for bitmap  area --- */
            *(pchDestTop - usWrite - 1) = uchBitmap;
            break;
        }

        for ( i = 0; i < RFL_BYTELEN; i++) {
            /* check Written length */
            if ( usDestLen <= (USHORT)(pchDestTop - (CHAR *)pDest) ) { 
                return(RFL_FAIL);
            }
            if ( *(pchSorcTop) != '\0' )  {
                uchBitmap |= (UCHAR)(0x80 >> i);      /* 0x80  = "10000000" */
                *pchDestTop = *pchSorcTop;
                pchDestTop++;
                usWrite++;
            }
            pchSorcTop++;        /* move read point to next byte */
        }

        /* --- write bitmap    1 for bitmap  area --- */
        *(pchDestTop - usWrite - 1) = uchBitmap;
        usSourceLen -= RFL_BYTELEN;
    }

    return(pchDestTop - (CHAR *)pDest);      /* return write length */
}

/*
 * RflZipByType() - create a transaction record in the buffer pointed to by pBuffer
 *                  by putting the non-compressed data into the proper place and then compressing
 *                  the remaining data and putting it into the correct place. Then update the sizes.
 *
 *                  This function is used similar to the following:
 *     srecLen = RflZipByType (junk, TRN_WORKBUFFER, ItemTransOpen, sizeof(TRANSTORAGETRANSOPENNON), sizeof(TRANSTORAGETRANSOPEN));
 *
 * RETURNS:  if an error then return RFL_FAIL
 *           otherwise return the number of bytes in the complete transaction record, compressed and non-compressed.
 *
*/
SHORT  RflZipByType (VOID *pBuffer, USHORT BuffSize, CONST VOID *pData, USHORT NoCmpSize, USHORT CmpSize)
{
	UCHAR  *pBuff = pBuffer;
	UCHAR  *pBuffNon = pBuff + 1;                        // point to the non-compressed section begins
	USHORT *pCmpSize = (VOID *)(pBuffNon + NoCmpSize);   // point to compressed size area, a USHORT
	UCHAR  *pBuffCmp = (VOID *)(pCmpSize + 1);           // point to the compressed section begins after USHORT size.
	UCHAR  uchNonCompressedSize = 0;
	SHORT  sCompressedSize = 0;
	SHORT  sTotalSize = RFL_FAIL;

	// do some basic safety checks on sizes to prevent egregious errors
	// NoCmpSize - non-compressed data size should fit in a UCHAR.
	// BuffSize  - buffer to receive non-compressed and compressed record should be a mimimum size.
	if (NoCmpSize > 0x00F0) {
		NHPOS_ASSERT(NoCmpSize <= 0x00F0);
		return RFL_FAIL;
	}

	// perform a check that the uchMajorClass is a known class type.
	NHPOS_ASSERT(RflGetStorageItemClassLen (pData) != 0);

	// perform a check on the destination buffer size is a minimum size.  we approximate with
	// non-compressed size plus size of the compressed length variable + 20 bytes for compressed size.
	// RflZip() will ensure that the buffer is not overrun during compression and this check is for the
	// minimum size including non-compressed part since RflZip() does not know about non-compressed section.
	if (BuffSize < NoCmpSize + sizeof(*pCmpSize) + 20) {
		NHPOS_ASSERT((BuffSize >= NoCmpSize + sizeof(*pCmpSize) + 20));
		return RFL_FAIL;
	}

	*pBuff = uchNonCompressedSize = (UCHAR)NoCmpSize + sizeof(UCHAR);  // offset in bytes to end of non-compressed area.
	memcpy (pBuffNon, pData, NoCmpSize);
	sCompressedSize = RflZip (pBuffCmp, BuffSize - (pBuffCmp - pBuff), ((UCHAR *)pData) + NoCmpSize, CmpSize);

	if (sCompressedSize != RFL_FAIL) {
		// if compression succeeded then set the compressed size length and figure out
		// the total record size which is the size of the compressed section plus the
		// size of the non-compressed section and the sizes of the two parts of the record
		// where the size of the non-compressed and the compressed sections are stored.
		*pCmpSize = sTotalSize = sCompressedSize + sizeof(*pCmpSize);  // size of compressed section and its header.
		sTotalSize += NoCmpSize + sizeof(UCHAR);                       // size of the non-compressed section and its header.
	}

	return sTotalSize;
}

/**fh
;============================================================================
;
;   function : Restore compressed memory 
;
;	Format   : USHORT RflUnZip(UCHAR *pchDest,       
;                              USHORT usDestLen,    
;                              UCHAR *pchSource,     
;                              USHORT usSourceLen); 
;
;   input    : UCHAR *pchDest,         -Restore buffer  pointer
;              USHORT usDestLen,       -Restore buffer length
;              UCHAR *pchSource,       -Compressed buffer pointer
;              USHORT usSourceLen);    -Compressed buffer length
;
;   output   : UCHAR  *pchDest         -Restored data
;              USHORT usDestLen        -Restored data length
;
;   return   : SHORT  sWriteLeng       -RFL_FAIL       //if restore fail
;                                      -other          //Restored data length
;
;   synopis  : This function restores  memory that compressed by RflZip(). 
;
;============================================================================
fh**/
SHORT RflUnZip(VOID *pDest, USHORT usDestLen, CONST VOID *pSource, USHORT usSourceLen)
{
    CONST CHAR    *pchSorcTop = pSource;  /* Source buffer pointer */
          CHAR    *pchDestTop = pDest;    /* Destination buffer pointer */

    if (usDestLen == 0) {
        return(RFL_FAIL);
    }

    memset(pDest, '\0', usDestLen);   /* inititalize write area */
        
	//For each character in the source, for each bit in the RFL byte (character).
	//
    for (;;) {
		USHORT  i;                   /* loop counter */
		CHAR    chBitMap;            /* BitMap */

        chBitMap = *pchSorcTop;
        pchSorcTop++;
        for (i = 0; i < RFL_BYTELEN; i++) {
            /* write data if  bit on */
            if ( chBitMap & (UCHAR)(0x80 >> i) ) {
                *pchDestTop = *pchSorcTop;
                pchSorcTop++;
            }
            pchDestTop++;                    /* move write point to next byte */
            /* --- all data read --- */
            if ( usSourceLen <= (USHORT)( (pchSorcTop-1) - (CHAR *)pSource) )  {
                return((SHORT)(pchDestTop - (CHAR *)pDest));
            }
            /* --- over specified  length --- */
            if ( usDestLen <= (USHORT)(pchDestTop - (CHAR *)pDest) )  {
                return(RFL_FAIL);
            }
        }
    }
}
