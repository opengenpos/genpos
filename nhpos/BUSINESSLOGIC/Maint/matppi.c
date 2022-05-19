/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : MaintPPI Module  ( SUPER & PROGRAM MODE )                       
* Category    : Maintenance, NCR 2170 US Hospitality Application Program        
* Program Name: MATPPI.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: This function controls PPI PARAMETER FILE.
*
*           The provided function names are as follows: 
* 
*             MaintPPIRead()          : read & display parameter from ppi file 
*             MaintPPIEdit()          : check input data from UI & set data in self-work area 
*             MaintPPIWrite()         : write ppi parameter record in ppi file & print 
*             MaintPPIDelete()        : delete corresponding ppi record from ppi file  
*             MaintPPIReport()        : print ppi report from ppi file  
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver.Rev. :   Name    : Description
* Nov-28-95: 03.01.00 : M.Ozawa   : initial
*          :          :           : 
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

#include "ecr.h"
#include "uie.h"
#include "paraequ.h"
#include "para.h"
#include "rfl.h"
#include "maint.h"
#include "display.h"
#include "regstrct.h"
#include "transact.h"
#include "prt.h"
#include "csop.h"
#include "csstbpar.h"
#include "csstbopr.h"
#include "mld.h"
#include "maintram.h" 

/*
*===========================================================================
*   Work Area Declartion 
*===========================================================================
*/


/*
*===========================================================================
** Synopsis:    SHORT MaintPPIRead( MAINTPPI *pData )
*               
*     Input:    *pData         : pointer to structure for PARAPPI
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               LDT_SEQERR_ADR : sequence error
*               LDT_KEYOVER_ADR: wrong data
*               sReturn        : error status
*
** Description: This function reads & displays parameter from PPI FILE in self-work area.
*===========================================================================
*/

SHORT MaintPPIRead( MAINTPPI *pData, USHORT pPage )  
{

    SHORT         sReturn, i;
	PPIIF         PpiIf = { 0 };
	MAINTPPI      MaintPPI = { 0 };
                
    /* initialize */

    memset(&MaintWork, 0, sizeof(MaintWork));

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                        /* without data */
        return(LDT_SEQERR_ADR);                                         /* sequence error */
    } 

    /* check PPI code range */
    if ((pData->uchPPINumber == 0)||(pData->uchPPINumber > MAINT_PPI_NO_MAX)) {
        return(LDT_KEYOVER_ADR);
    }
    PpiIf.uchPpiCode = pData->uchPPINumber;

    /* read data */
    sReturn = CliOpPpiIndRead(&PpiIf, usMaintLockHnd);

    /* distinguish return value */
    if (sReturn == OP_PERFORM){
        /* MaintWork Set */
        MaintWork.PPI.uchMajorClass = CLASS_PARAPPI;
        MaintWork.PPI.uchPPINumber = PpiIf.uchPpiCode;
        MaintWork.PPI.uchAddr = 1;
		if ( (pPage == 1) || (pPage==2 && PpiIf.ParaPpi.PpiRec[21].uchQTY!=0))	// PPI Enhancement - CSMALL
		{
			memcpy(&MaintWork.PPI.ParaPPI[0], &PpiIf.ParaPpi.PpiRec[0], sizeof(MaintWork.PPI.ParaPPI));
			for (i = 0; i < MAX_PARAPPI_SIZE; i++) {
				if (PpiIf.ParaPpi.PpiRec[i].uchQTY == 0) {
					i++;
					break;
				}
			}
		} else
		{
			// The following assignments look weird but see the function MaintPPIWriteAddtlSettings()
			// below where the saved values are unpacked back into a PpiIf.ParaPpi struct.
			pPage = 3;
			MaintWork.PPI.ParaPPI[0].uchQTY = PpiIf.ParaPpi.ucAfterMaxReached;
			MaintWork.PPI.ParaPPI[1].uchQTY = PpiIf.ParaPpi.ucBeforeInitialQty;
			MaintWork.PPI.ParaPPI[2].ulPrice = PpiIf.ParaPpi.ulMinTotalSale;
			i = 3;

		}
        MaintWork.PPI.uchPPILength = (UCHAR)i;    /* Length get */

    } else if (sReturn == OP_NOT_IN_FILE) {
        MaintWork.PPI.uchMajorClass = CLASS_PARAPPI;
        MaintWork.PPI.uchPPINumber = pData->uchPPINumber;
        MaintWork.PPI.uchAddr = 1;
        MaintWork.PPI.uchPPILength = 1;
    } else {
        /* convert error status */
        return(OpConvertError(sReturn));
    }

    /******************/
    /* Display to MLD */
    /******************/
    MldDispItem(&MaintWork.PPI, pPage);
    /* read & set LEADTHRU */
	RflGetLead(MaintPPI.aszMnemonics, LDT_DATA_ADR);

    MaintPPI.uchMajorClass = CLASS_PARAPPI;
    MaintPPI.uchMinorClass = CLASS_PARAPPI_MAINT_READ;
    MaintPPI.uchPPINumber = MaintWork.PPI.uchPPINumber;
    MaintPPI.uchACNumber = AC_PPI_MAINT;
    MaintPPI.uchAddr = 1;
    MaintPPI.uchQTY = MaintWork.PPI.ParaPPI[0].uchQTY;
    MaintPPI.ulPrice = MaintWork.PPI.ParaPPI[0].ulPrice;

	DispWrite(&MaintPPI);
    return(SUCCESS);
}


/*** PPI Enhancement function - CSMALL ***/
/*
*===========================================================================
** Synopsis:    SHORT MaintPPICheckLength( MAINTPPI *pData )
*               
*     Input:    *pData              : pointer to structure for MAINTPPI
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS             : successful
*               LDT_SEQERR_ADR      : sequence error
*               LDT_KEYOVER_ADR     : wrong data
*               MAINT_PPI_MNEMO_ADR : set mnemonics 
*
** Description: This function checks # of PPI codes (length).
*===========================================================================
*/
SHORT MaintPPICheckLength( VOID )
{
	return (MaintWork.PPI.uchPPILength);
}
/*** END PPI Enhancement function - CSMALL ***/

/*** PPI Enhancement function - CSMALL ***/
/*
*===========================================================================
** Synopsis:    SHORT MaintPPIEditAddtlSettings( MAINTPPI *pData )
*               
*     Input:    *pData              : pointer to structure for MAINTPPI
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS             : successful
*               LDT_SEQERR_ADR      : sequence error
*               LDT_KEYOVER_ADR     : wrong data
*               MAINT_PPI_MNEMO_ADR : set mnemonics 
*
** Description: This function reads & displays parameter from PPI FILE in self-work area.
*===========================================================================
*/

SHORT MaintPPIEditAddtlSettings( MAINTPPI *pData )
{
	MAINTPPI      MaintPPI = { 0 };

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                    /* without data */
        /* distinguish minor class */
        switch(pData->uchMinorClass) {
        case CLASS_PARAPPI_MAINT_READ:  

            /* check uncompleted table */
            if ((MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY == 0) &&
                    (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice)) {
                return(LDT_KEYOVER_ADR);
            }
            if (MaintWork.PPI.uchAddr > 2) {
                if ((MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice == 0) &&
                        (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY)) {

                    if (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-2].ulPrice) {
                        return(LDT_KEYOVER_ADR);
                    }
                }
            }

            MaintWork.PPI.uchAddr++;
            if (MaintWork.PPI.uchAddr > MaintWork.PPI.uchPPILength) {

                MaintWork.PPI.uchAddr = 1;
            }
            MaintPPI.uchAddr = MaintWork.PPI.uchAddr;

            /* set data for display next old data */
            /* read & set LEADTHRU */
			RflGetLead(MaintPPI.aszMnemonics, LDT_DATA_ADR);

            MaintPPI.uchMajorClass = CLASS_PARAPPI;
            MaintPPI.uchMinorClass = CLASS_PARAPPI_MAINT_READ;
            MaintPPI.uchPPINumber = MaintWork.PPI.uchPPINumber;
            MaintPPI.uchACNumber = AC_PPI_MAINT;
            MaintPPI.uchAddr = MaintWork.PPI.uchAddr;
            MaintPPI.uchQTY = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY;
            MaintPPI.ulPrice = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice;
            DispWrite(&MaintPPI);

            return(SUCCESS);

        default: 
/*            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
            return(LDT_SEQERR_ADR);

        }
    } else {                /* with data */

		if ((MaintWork.PPI.uchAddr == 1 || MaintWork.PPI.uchAddr == 2) && pData->uchMinorClass != CLASS_PARAPPI_MAINT_READ)
		{
			pData->uchMinorClass = CLASS_PARAPPI_MAINT_QTY_WRITE;
		}

        /* distinguish minor class */
        switch(pData->uchMinorClass) {

        case CLASS_PARAPPI_MAINT_READ:
            /* check PPI address */
            if ((pData->uchAddr < 1) ||
                (pData->uchAddr > MaintWork.PPI.uchPPILength)) {
                return(LDT_KEYOVER_ADR);
            }

            MaintPPI.uchAddr = pData->uchAddr;
            MaintWork.PPI.uchAddr = pData->uchAddr;

            /* set data for display next old data */
            /* read & set LEADTHRU */
			RflGetLead(MaintPPI.aszMnemonics, LDT_DATA_ADR);

            MaintPPI.uchMajorClass = CLASS_PARAPPI;
            MaintPPI.uchMinorClass = CLASS_PARAPPI_MAINT_READ;
            MaintPPI.uchPPINumber = MaintWork.PPI.uchPPINumber;
            MaintPPI.uchACNumber = AC_PPI_MAINT;
            MaintPPI.uchAddr = MaintWork.PPI.uchAddr;
            MaintPPI.uchQTY = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY;
            MaintPPI.ulPrice = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice;
            DispWrite(&MaintPPI);

            return(SUCCESS);

        case CLASS_PARAPPI_MAINT_QTY_WRITE:

			if (pData->ulInputData != 1 && pData->ulInputData != 0)
			{
				return (LDT_KEYOVER_ADR);
			}

            MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY = (UCHAR)pData->ulInputData;

            /******************/
            /* Display to MLD */
            /******************/
            MldDispItem(&MaintWork.PPI, 3);

            /* ---- increment to next address ---- */
            MaintWork.PPI.uchAddr++;

            /******************/
            /* Display to MLD */
            /******************/
			//MldDispItem(&MaintWork.PPI, 3);

            /* read & set LEADTHRU */
			RflGetLead(MaintPPI.aszMnemonics, LDT_DATA_ADR);

            /* Next Old Data Display */
            MaintPPI.uchMajorClass = pData->uchMajorClass;
            MaintPPI.uchMinorClass = pData->uchMinorClass;
            MaintPPI.uchACNumber = AC_PPI_MAINT;
            MaintPPI.uchPPINumber = MaintWork.PPI.uchPPINumber;
            MaintPPI.uchAddr = MaintWork.PPI.uchAddr;
            MaintPPI.uchQTY = (UCHAR)pData->ulInputData;
            MaintPPI.ulPrice = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice;
            DispWrite(&MaintPPI);
            return(SUCCESS);

        case CLASS_PARAPPI_MAINT_PRICE_WRITE:
            
            if (pData->ulInputData > MAINT_PPI_PRICE_MAX) {
                return(LDT_KEYOVER_ADR);
            }

            MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice = pData->ulInputData;

            /******************/
            /* Display to MLD */
            /******************/
            MldDispItem(&MaintWork.PPI, 3);

            /* ---- increment to next address if qty is entered ---- */
            if (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY) {
                MaintWork.PPI.uchAddr++;
                if (MaintWork.PPI.uchAddr > PPI_ADR_MAX) {
                    MaintWork.PPI.uchAddr = 1;             /* initialize address */
                } else {
                    MaintWork.PPI.uchPPILength = MaintWork.PPI.uchAddr;

                    /******************/
                    /* Display to MLD */
                    /******************/
                    MldDispItem(&MaintWork.PPI, 3);
                }
            }

            /* read & set LEADTHRU */
			RflGetLead(MaintPPI.aszMnemonics, LDT_DATA_ADR);

            /* Next Old Data Display */
            MaintPPI.uchMajorClass = pData->uchMajorClass;
            MaintPPI.uchMinorClass = pData->uchMinorClass;
            MaintPPI.uchACNumber = AC_PPI_MAINT;
            MaintPPI.uchPPINumber = MaintWork.PPI.uchPPINumber;
            MaintPPI.uchAddr = MaintWork.PPI.uchAddr;
            MaintPPI.uchQTY = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY;
            MaintPPI.ulPrice = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice;
            DispWrite(&MaintPPI);
            return(SUCCESS);

        default:
/*            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
            return(LDT_KEYOVER_ADR);
        }
    }
}
/*** END PPI Enhancement function - CSMALL ***/

/*
*===========================================================================
** Synopsis:    SHORT MaintPPIEdit( MAINTPPI *pData )
*               
*     Input:    *pData              : pointer to structure for MAINTPPI
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS             : successful
*               LDT_SEQERR_ADR      : sequence error
*               LDT_KEYOVER_ADR     : wrong data
*               MAINT_PPI_MNEMO_ADR : set mnemonics 
*
** Description: This function reads & displays parameter from PPI FILE in self-work area.
*===========================================================================
*/

SHORT MaintPPIEdit( MAINTPPI *pData )
{
	MAINTPPI      MaintPPI = { 0 };

    /* check status */
    if (pData->uchStatus & MAINT_WITHOUT_DATA) {                    /* without data */

        /* distinguish minor class */
        switch(pData->uchMinorClass) {
        case CLASS_PARAPPI_MAINT_READ:  

            /* check uncompleted table */
            if ((MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY == 0) &&
                    (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice)) {
                return(LDT_KEYOVER_ADR);
            }
            if (MaintWork.PPI.uchAddr > 2) {
                if ((MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice == 0) &&
                        (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY)) {

                    if (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-2].ulPrice) {
                        return(LDT_KEYOVER_ADR);
                    }
                }
            }

            MaintWork.PPI.uchAddr++;
            if (MaintWork.PPI.uchAddr > MaintWork.PPI.uchPPILength) {
                MaintWork.PPI.uchAddr = 1;
            }
            MaintPPI.uchAddr = MaintWork.PPI.uchAddr;

            /* set data for display next old data */
            /* read & set LEADTHRU */
			RflGetLead(MaintPPI.aszMnemonics, LDT_DATA_ADR);

            MaintPPI.uchMajorClass = CLASS_PARAPPI;
            MaintPPI.uchMinorClass = CLASS_PARAPPI_MAINT_READ;
            MaintPPI.uchPPINumber = MaintWork.PPI.uchPPINumber;
            MaintPPI.uchACNumber = AC_PPI_MAINT;
            MaintPPI.uchAddr = MaintWork.PPI.uchAddr;
            MaintPPI.uchQTY = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY;
            MaintPPI.ulPrice = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice;
            DispWrite(&MaintPPI);

            return(SUCCESS);

        default: 
/*            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
            return(LDT_SEQERR_ADR);

        }
    } else {                /* with data */
        /* distinguish minor class */
        switch(pData->uchMinorClass) {

        case CLASS_PARAPPI_MAINT_READ:
            /* check PPI address */
            if ((pData->uchAddr < 1) ||
                (pData->uchAddr > MaintWork.PPI.uchPPILength)) {
				// Fix to allow edit of PPI address, if address before it exists - CSMALL
				// if a QTY does not exist at the address before requested address, error out
				if (!MaintWork.PPI.ParaPPI[pData->uchAddr-1].uchQTY) 
				{
					return(LDT_KEYOVER_ADR);
				}
            }

            /* check uncompleted table */
            if ((MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY == 0) &&
                    (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice)) {
                return(LDT_KEYOVER_ADR);
            }
            if (MaintWork.PPI.uchAddr > 2) {
                if ((MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice == 0) &&
                        (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY)) {

                    if (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-2].ulPrice) {
                        return(LDT_KEYOVER_ADR);
                    }
                }
            }

            MaintPPI.uchAddr = pData->uchAddr;
            MaintWork.PPI.uchAddr = pData->uchAddr;

            /* set data for display next old data */
            /* read & set LEADTHRU */
			RflGetLead(MaintPPI.aszMnemonics, LDT_DATA_ADR);

            MaintPPI.uchMajorClass = CLASS_PARAPPI;
            MaintPPI.uchMinorClass = CLASS_PARAPPI_MAINT_READ;
            MaintPPI.uchPPINumber = MaintWork.PPI.uchPPINumber;
            MaintPPI.uchACNumber = AC_PPI_MAINT;
            MaintPPI.uchAddr = MaintWork.PPI.uchAddr;
            MaintPPI.uchQTY = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY;
            MaintPPI.ulPrice = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice;
            DispWrite(&MaintPPI);

            return(SUCCESS);

        case CLASS_PARAPPI_MAINT_QTY_WRITE:
            if ((pData->ulInputData < 1) ||
                (pData->ulInputData > MAINT_PPI_QTY_MAX)) {
                return(LDT_KEYOVER_ADR);
            }

            /* compare qty with privious address data */
            if (MaintWork.PPI.uchAddr > PPI_PRICE1_ADR) {
                if (( UCHAR)pData->ulInputData <= 
                        MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-2].uchQTY) {
                    return(LDT_KEYOVER_ADR);
                }
            }

            /* compare qty with next address data */
            if (MaintWork.PPI.uchAddr < PPI_ADR_MAX) {
                if (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr].uchQTY) {
                    if (( UCHAR)pData->ulInputData >= 
                            MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr].uchQTY) {
                        return(LDT_KEYOVER_ADR);
                    }
                }
            }

            MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY = (UCHAR)pData->ulInputData;

            /******************/
            /* Display to MLD */
            /******************/
            MldDispItem(&MaintWork.PPI, 0);

            /* ---- increment to next address if price is entered ---- */
            if (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice) {
                MaintWork.PPI.uchAddr++;
                if (MaintWork.PPI.uchAddr > PPI_ADR_MAX) {
                    MaintWork.PPI.uchAddr = 1;             /* initialize address */
                } else {
                    MaintWork.PPI.uchPPILength = MaintWork.PPI.uchAddr;

                    /******************/
                    /* Display to MLD */
                    /******************/
                    MldDispItem(&MaintWork.PPI, 0);
                }
            }

            /* read & set LEADTHRU */
			RflGetLead(MaintPPI.aszMnemonics, LDT_DATA_ADR);

            /* Next Old Data Display */
            MaintPPI.uchMajorClass = pData->uchMajorClass;
            MaintPPI.uchMinorClass = pData->uchMinorClass;
            MaintPPI.uchACNumber = AC_PPI_MAINT;
            MaintPPI.uchPPINumber = MaintWork.PPI.uchPPINumber;
            MaintPPI.uchAddr = MaintWork.PPI.uchAddr;
            MaintPPI.uchQTY = (UCHAR)pData->ulInputData;
            MaintPPI.ulPrice = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice;
            DispWrite(&MaintPPI);
            return(SUCCESS);

        case CLASS_PARAPPI_MAINT_PRICE_WRITE:
            
            if (pData->ulInputData > MAINT_PPI_PRICE_MAX) {
                return(LDT_KEYOVER_ADR);
            }

            /* compare price to privious address data */
            if (MaintWork.PPI.uchAddr > PPI_PRICE1_ADR) {
                if (pData->ulInputData < 
                        MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-2].ulPrice) {
                    return(LDT_KEYOVER_ADR);
                }
            }

            /* compare price multiple with next address data */
            if (MaintWork.PPI.uchAddr < PPI_ADR_MAX) {
                if (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr].ulPrice) {
                    if (pData->ulInputData > 
                            MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr].ulPrice) {
                        return(LDT_KEYOVER_ADR);
                    }
                }
            }

            MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice = pData->ulInputData;

            /******************/
            /* Display to MLD */
            /******************/
            MldDispItem(&MaintWork.PPI, 0);

            /* ---- increment to next address if qty is entered ---- */
            if (MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY) {
                MaintWork.PPI.uchAddr++;
                if (MaintWork.PPI.uchAddr > PPI_ADR_MAX) {
                    MaintWork.PPI.uchAddr = 1;             /* initialize address */
                } else {
                    MaintWork.PPI.uchPPILength = MaintWork.PPI.uchAddr;

                    /******************/
                    /* Display to MLD */
                    /******************/
					MldDispItem(&MaintWork.PPI, 0);
                }
            }

            /* read & set LEADTHRU */
			RflGetLead(MaintPPI.aszMnemonics, LDT_DATA_ADR);

            /* Next Old Data Display */
            MaintPPI.uchMajorClass = pData->uchMajorClass;
            MaintPPI.uchMinorClass = pData->uchMinorClass;
            MaintPPI.uchACNumber = AC_PPI_MAINT;
            MaintPPI.uchPPINumber = MaintWork.PPI.uchPPINumber;
            MaintPPI.uchAddr = MaintWork.PPI.uchAddr;
            MaintPPI.uchQTY = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].uchQTY;
            MaintPPI.ulPrice = MaintWork.PPI.ParaPPI[MaintWork.PPI.uchAddr-1].ulPrice;
            DispWrite(&MaintPPI);
            return(SUCCESS);

        default:
/*            PifAbort(MODULE_MAINT_LOG_ID, FAULT_INVALID_DATA); */
            return(LDT_KEYOVER_ADR);
        }
    }
}


/*
*===========================================================================
** Synopsis:    SHORT MaintPPIReport( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS : successful
*               sReturn : error
*
** Description: This function prints PPI report from PPI file. 
*===========================================================================
*/

SHORT MaintPPIReport( VOID )
{
    SHORT           sReturn = SUCCESS;
	PPIIF           PpiIf = { 0 };
	PARAPPI         ParaPPI = { 0 };
	ULONG           ulRecordNumberPpi = 0;

    if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
        MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */
        /* make header */
        MaintMakeHeader(CLASS_MAINTHEADER_RPT,    /* minor class */
                        RPT_ACT_ADR,              /* "ACT #" */
                        0,
                        0,                        
                        RPT_FILE_ADR,             /* "FILE" */
                        0,
                        AC_PPI_MAINT,            /* action number(71) */
                        0,
                        PRT_RECEIPT | PRT_JOURNAL); /* print control */
         uchMaintOpeCo |= MAINT_ALREADYOPERATE;
    }

    ParaPPI.uchMajorClass = CLASS_PARAPPI;
    ParaPPI.uchMinorClass = CLASS_PARAPPI_MAINT_REPORT;

    /* set print control based on MDC settings */
    ParaPPI.usPrintControl = 0;
    if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT0))) {
        ParaPPI.usPrintControl |= PRT_JOURNAL;   /* print out on journal */
    } 
    if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT1))) {
        ParaPPI.usPrintControl |= PRT_RECEIPT;   /* print out receipt */
    }

	ulRecordNumberPpi = RflGetMaxRecordNumberByType(FLEX_PPI_ADR);
    for (PpiIf.uchPpiCode = 1; PpiIf.uchPpiCode <= (USHORT)ulRecordNumberPpi; PpiIf.uchPpiCode++){ /* 2172 */
        if((sReturn = CliOpPpiIndRead(&PpiIf, usMaintLockHnd)) == OP_NOT_IN_FILE) {
            continue;
        } else if (sReturn != OP_PERFORM) {
			MAINTERRORCODE  MaintErrorCode = { 0 };

            /* convert error status & print error code */
            MaintErrorCode.uchMajorClass = CLASS_MAINTERRORCODE;
            MaintErrorCode.sErrorCode = OpConvertError(sReturn);

            /* set print control as indicated by MDC settings */
            MaintErrorCode.usPrtControl = 0;
            if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT0))) {
                MaintErrorCode.usPrtControl |= PRT_JOURNAL;   /* print out on journal */
            } 

            if (!(CliParaMDCCheck(MDC_PARA_ADR, ODD_MDC_BIT1))) {
                MaintErrorCode.usPrtControl |= PRT_RECEIPT;
            }
            PrtPrintItem(NULL, &MaintErrorCode);
            break;
        }
        if(PpiIf.ParaPpi.PpiRec[0].uchQTY == 0){
            continue;
        }

        ParaPPI.uchPPINumber = PpiIf.uchPpiCode;
		memcpy(&ParaPPI.ParaPPI[0], &PpiIf.ParaPpi.PpiRec[0], sizeof(ParaPPI.ParaPPI));

        PrtPrintItem(NULL, &ParaPPI);

        /* send print data to shared module */
        PrtShrPartialSend(ParaPPI.usPrintControl);
        if ((sReturn = UieReadAbortKey()) == UIE_ENABLE) {                  /* depress abort key */
            MaintMakeAbortKey();
            break;
        }
        if(sReturn == UIE_ENABLE){
            sReturn = SUCCESS;
            break;
        }
        MaintFeed();
    }
    /* make trailer */

    MaintMakeTrailer(CLASS_MAINTTRAILER_PRTSUP);
    return(OpConvertError(sReturn));
}

/*** PPI Enhancement function - CSMALL ***/
/*
*===========================================================================
** Synopsis:    SHORT MaintPPIWriteAddtlSettings( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               sReturn        : error
*
** Description: This function writes & prints PPI PARAMETER RECORD of self-work area in PPI PARAMETER FILE.
*===========================================================================
*/

SHORT MaintPPIWriteAddtlSettings( VOID )
{
    USHORT          usLength;
    SHORT           sReturn;
    SHORT           sError;
	PPIIF           PpiIf = { 0 };
	MAINTPPI		PPIData = { 0 };

    if (uchMaintOpeCo & MAINT_WRITECOMPLETE) {
        return(SUCCESS);        /* not write uncompleted data */
    }

	usLength = (USHORT)MaintWork.PPI.uchPPILength;

	// retrieve the saved values that were stored into MaintWork in function
	// MaintPPIRead() above.
    PpiIf.uchPpiCode = MaintWork.PPI.uchPPINumber;
	PpiIf.ParaPpi.ucAfterMaxReached = MaintWork.PPI.ParaPPI[0].uchQTY;
	PpiIf.ParaPpi.ucBeforeInitialQty = MaintWork.PPI.ParaPPI[1].uchQTY;
	PpiIf.ParaPpi.ulMinTotalSale = MaintWork.PPI.ParaPPI[2].ulPrice;

	// reset 'MaintWork' with regular PPI codes and copy into 'PpiIf.ParaPpi.PpiRec',
	//	this will save all settings for a particular PPI code - CSMALL
	PPIData.uchMajorClass = CLASS_PARAPPI;         /* Set Major Class */
	PPIData.uchAddr = 1;                           /* set initial address */
	PPIData.uchStatus = 0;                         /* Set W/ Amount Status */
	PPIData.uchPPINumber = ( USHORT)PpiIf.uchPpiCode;
	if ((sError = MaintPPIRead(&PPIData,1)) != SUCCESS) {             /* Read Function Completed */
		return(!SUCCESS);
	}
	memcpy(&PpiIf.ParaPpi.PpiRec[0], &MaintWork.PPI.ParaPPI[0], sizeof(PpiIf.ParaPpi.PpiRec) );


    sReturn = SerOpPpiAssign(&PpiIf, usMaintLockHnd);

    if ((sReturn == OP_PERFORM)) {
        /* make header */
        if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
            MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */

            MaintMakeHeader(CLASS_MAINTHEADER_RPT,    /* minor class */
                            RPT_ACT_ADR,              /* "ACT #" */
                            0,
                            0,                        
                            RPT_MAINT_ADR,             /* "MAINT" */
                            0,
                            AC_PPI_MAINT,            /* action number(71) */
                            0,
                            PRT_RECEIPT | PRT_JOURNAL); /* print control */
        } else {
            MaintFeed();
        }
        uchMaintOpeCo |= MAINT_ALREADYOPERATE;

        /* print out */
        MaintWork.PPI.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;
        PrtPrintItem(NULL, &MaintWork.PPI);

        /* send print data to shared module */
        PrtShrPartialSend(MaintWork.PPI.usPrintControl);

        return(SUCCESS);
    }
    return(OpConvertError(sReturn));
}

/*** End PPI Enhancement function - CSMALL ***/

/*
*===========================================================================
** Synopsis:    SHORT MaintPPIWrite( VOID )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      SUCCESS        : successful
*               sReturn        : error
*
** Description: This function writes & prints PPI PARAMETER RECORD of self-work area in PPI PARAMETER FILE.
*===========================================================================
*/

SHORT MaintPPIWrite( VOID )
{
    USHORT          usLength;
    SHORT           sReturn;
	PPIIF           PpiIf = { 0 };

    if (uchMaintOpeCo & MAINT_WRITECOMPLETE) {
        return(SUCCESS);        /* not write uncompleted data */
    }

	/* check uncompleted table */
	if ((MaintWork.PPI.ParaPPI[0].uchQTY ==0) && (MaintWork.PPI.ParaPPI[0].ulPrice == 0)) {
		return(LDT_KEYOVER_ADR);
	}

	usLength = (USHORT)MaintWork.PPI.uchPPILength;
    if (usLength) usLength--;
    if (usLength) {
        if ((MaintWork.PPI.ParaPPI[usLength].uchQTY ==0) && (MaintWork.PPI.ParaPPI[usLength].ulPrice == 0)) {
            usLength--;
        }
    }
    if ((MaintWork.PPI.ParaPPI[usLength].uchQTY == 0) && (MaintWork.PPI.ParaPPI[usLength].ulPrice)) {
        return(LDT_KEYOVER_ADR);
    }
    if (usLength) {
        if ((MaintWork.PPI.ParaPPI[usLength].ulPrice == 0) && (MaintWork.PPI.ParaPPI[usLength].uchQTY)) {

            if (MaintWork.PPI.ParaPPI[usLength-1].ulPrice) {
                return(LDT_KEYOVER_ADR);
            }
        }
    }

    PpiIf.uchPpiCode = MaintWork.PPI.uchPPINumber;
    memcpy(&PpiIf.ParaPpi.PpiRec[0], &MaintWork.PPI.ParaPPI[0], sizeof(PpiIf.ParaPpi.PpiRec));
    sReturn = SerOpPpiAssign(&PpiIf, usMaintLockHnd);

    if ((sReturn == OP_PERFORM)) {
        /* make header */
        if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
            MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */

            MaintMakeHeader(CLASS_MAINTHEADER_RPT,    /* minor class */
                            RPT_ACT_ADR,              /* "ACT #" */
                            0,
                            0,                        
                            RPT_MAINT_ADR,             /* "MAINT" */
                            0,
                            AC_PPI_MAINT,            /* action number(71) */
                            0,
                            PRT_RECEIPT | PRT_JOURNAL); /* print control */
        } else {
            MaintFeed();
        }
        uchMaintOpeCo |= MAINT_ALREADYOPERATE;

        /* print out */
        MaintWork.PPI.usPrintControl = PRT_RECEIPT | PRT_JOURNAL;
        PrtPrintItem(NULL, &MaintWork.PPI);

        /* send print data to shared module */
        PrtShrPartialSend(MaintWork.PPI.usPrintControl);

        return(SUCCESS);
    }
    return(OpConvertError(sReturn));
}

/*
*===========================================================================
** Synopsis:    SHORT MaintPPIDelete( PARAPPI )
*               
*     Input:    Nothing
*    Output:    Nothing 
*     InOut:    Nothing
*
** Return:      sReturn     : error 
*               LDT_ERR_ADR : error
*
** Description: This function deletes corresponding PPI record from PPI file.  
*===========================================================================
*/

SHORT MaintPPIDelete( MAINTPPI *pData)
{
    SHORT           sReturn;
	PPIIF           PpiIf = { 0 };
	MAINTTRANS      MaintTrans = { 0 };

    /* read string data is existed or not */
    PpiIf.uchPpiCode = pData->uchPPINumber;

    if((sReturn = CliOpPpiIndRead(&PpiIf, usMaintLockHnd)) != OP_PERFORM){
        return(OpConvertError(sReturn));
    }
    if(PpiIf.ParaPpi.PpiRec[0].uchQTY == 0){
        return (LDT_NTINFL_ADR);
    }

    PpiIf.uchPpiCode = pData->uchPPINumber;

    sReturn = SerOpPpiDelete(&PpiIf, usMaintLockHnd);

    if (sReturn == OP_PERFORM) {

        /* make header */
        if (!(uchMaintOpeCo & MAINT_ALREADYOPERATE)) {              /* in case of 1st operation */
            MaintIncreSpcCo(SPCO_CONSEC_ADR);                       /* increase consecutive counter */

            MaintMakeHeader(CLASS_MAINTHEADER_RPT,    /* minor class */
                            RPT_ACT_ADR,              /* "ACT #" */
                            0,
                            0,                        
                            RPT_MAINT_ADR,             /* "MAINT" */
                            0,
                            AC_PPI_MAINT,            /* action number(71) */
                            0,
                            PRT_RECEIPT | PRT_JOURNAL); /* print control */
            uchMaintOpeCo |= MAINT_ALREADYOPERATE;
        }

        /* print "DELETE" using structure MAINTTRANS */
        MaintTrans.uchMajorClass = CLASS_MAINTTRANS;
        MaintTrans.usPrtControl = ( PRT_JOURNAL | PRT_RECEIPT );
		RflGetTranMnem(MaintTrans.aszTransMnemo, TRN_DEL_ADR);
        PrtPrintItem(NULL, &MaintTrans);
        /* send print data to shared module */
        PrtShrPartialSend(MaintTrans.usPrtControl);

        /* Deleted Number Data Print Out */
        MaintWork.PPI.uchMajorClass = CLASS_PARAPPI;
        MaintWork.PPI.uchMinorClass = CLASS_PARAPPI_MAINT_DELETE;
        MaintWork.PPI.uchPPINumber = pData->uchPPINumber;
        memcpy(&MaintWork.PPI.ParaPPI[0], &PpiIf.ParaPpi.PpiRec[0], sizeof(MaintWork.PPI.ParaPPI));
        MaintWork.PPI.usPrintControl = ( PRT_JOURNAL | PRT_RECEIPT );
        PrtPrintItem(NULL, &MaintWork.PPI);
        /* send print data to shared module */
        PrtShrPartialSend(MaintWork.PPI.usPrintControl);

    }
    return(OpConvertError(sReturn));
}    
/*
*===========================================================================
** Synopsis:    SHORT  MaintPPILock( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*
** Return:      SUCCESS         : Successful
*               LDT_LOCK_ADR    : During Lock
*
** Description: Lock PPI File and Save This Lock Handle to Own Save Area
*===========================================================================
*/

SHORT MaintPPILock()
{
    SHORT    sError;
    
    if((sError = SerOpLock()) != OP_LOCK){                /* Success            */
        usMaintLockHnd = sError;                        /* Save Lock Handle    */
        return(SUCCESS);
    }
    return(LDT_LOCK_ADR);
}

/*
*===========================================================================
** Synopsis:    VOID  MaintPPIUnLock( VOID )
*
*     Input:    Nothing
*    Output:    Nothing
*
** Return:      Nothing
*
** Description: UnLock PPI File
*===========================================================================
*/

VOID MaintPPIUnLock()
{
    SerOpUnLock();
    usMaintLockHnd = 0;
}

/**** End of File ****/
