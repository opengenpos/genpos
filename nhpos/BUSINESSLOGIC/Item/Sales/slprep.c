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
* Title       : Dept./ PLU sales preparation module                          
* Category    : Item Module, NCR 2170 US Hospitality Model Application         
* Program Name: SLPREP.C
* --------------------------------------------------------------------------
* Abstract: ItemSalesPrepare() : Item sales preparation   
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  : Ver. Rev. :   Name    : Description
* May-16-92: 00.00.01  : K.Maeda   : Initial issue                                   
* May-29-92: 00.00.01  : K.Maeda   : Imprement PLU functions for Jun. 6 link 
*                                    version.                                   
* Jun- 4-92: 00.00.01  : K.Maeda   : 2nd code inspection held on Jun. 3rd                                     
* Jun- 8-92: 00.00.01  : K.Maeda   : 3rd code inspection held on Jun. 6th                                     
* Jun- 9-92: 00.00.01  : K.Maeda   : 4th code inspection held on Jun. 9th                                     
* Mar- 2-95: 03.00.00  : hkato     : R3.0
* Nov-22-95: 03.01.00  : M.Ozawa   : R3.1 (add ppi code)
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
#include    <string.h>
#include    <stdio.h>

#include    "ecr.h"
#include    "regstrct.h"
#include    "rfl.h"
#include    "pif.h"
#include    "log.h"
#include    "appllog.h"
#include    "uie.h"
#include    "uireg.h"
#include    "transact.h"
#include    "item.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "csop.h"
#include    "csstbopr.h"
#include    "display.h"
#include    "itmlocal.h"

// CONST UCHAR auchItemSalesMenuPLUHigh[] = "0000009999999"; /* limit of velocity number, 2172 */

/*
*===========================================================================
**Synopsis: SHORT   ItemSalesPrepare(UIFREGSALES *pUifRegSales,
*                                    ITEMSALES *pItemSales)
*                                    
*    Input: UIFREGSALES *pUifRegSales
*           
*   Output: None
*
*    InOut: ITEMSALES *pItemSales
*
**Return: ITM_SUCCESS    : Function Performed succesfully
*         LDT_SEQERR_ADR : Function aborted with Illegal operation sequense 
*                          error
*         LDT_PROHBT_ADR : Function prohibited by parameter option.
*
** Description: This function prepares ITEMSALES data.  Dec/14/2000                
*===========================================================================
*/
SHORT   ItemSalesPrepare(UIFREGSALES *pUifRegSales, ITEMSALES *pItemSales,
                         UCHAR *puchRestrict, USHORT *pusLinkNo, UCHAR *puchTable)
{
    SHORT  sReturnStatus;
	DEPTIF     DeptRecRcvBuffer = {0};       /* dept record receive buffer */
    PLUIF      PLURecRcvBuffer = {0};        /* PLU record receive buffer */    

    /*----- BRANCH OUT DEPT AND PLU CORRESPOND WITH MINOR CLASS -----*/
    switch (pItemSales->uchMinorClass) {
	case CLASS_ITEMORDERDEC:
			RflGetTranMnem (pItemSales->aszMnemonic, (TRN_ORDER_DEC_OFFSET + pUifRegSales->uchFsc));
			break;
    case CLASS_DEPT:
    case CLASS_DEPTMODDISC:                 /* modefied discounted dept */
        if (pUifRegSales->usDeptNo == 0) {   /* keyed Dept entry */
			PARADEPTNOMENU     DeptNoRcvBuff = {0};   /* Dept No. assigned a key saved buffer */
			PARACTLTBLMENU     MenuRcvBuff = {0};    /* Menu page data receive buffer */

            /* check availability of current page */
            MenuRcvBuff.uchMajorClass = CLASS_PARACTLTBLMENU;
            MenuRcvBuff.uchAddress = (UCHAR) (uchDispCurrMenuPage + 1);
            CliParaRead(&MenuRcvBuff);

            if (MenuRcvBuff.uchPageNumber == 1) {
                return(LDT_PROHBT_ADR);     /* error if prohibited to use */
            }

			if (uchDispCurrMenuPage >= STD_TOUCH_MENU_PAGE) {
				// menu page usage is only allowed with keyboard type systems and
				// not with touchscreen installations.
                return(LDT_PROHBT_ADR);     /* error if prohibited to use */
			}

            /* get Dept No. assigned to uchFSC */
            DeptNoRcvBuff.uchMajorClass = CLASS_PARADEPTNOMENU;
            DeptNoRcvBuff.uchMenuPageNumber = uchDispCurrMenuPage;
            DeptNoRcvBuff.uchMinorFSCData = pUifRegSales->uchFsc;    /* extended FSC */
            CliParaRead(&DeptNoRcvBuff);

            if (DeptNoRcvBuff.usDeptNumber == 0) {
                return(LDT_PROHBT_ADR);     /* error if not preset */
            }

            pItemSales->usDeptNo = DeptNoRcvBuff.usDeptNumber;       /* set returned Dept No. */
        } else {                            /* coded Dept entry */
            pItemSales->usDeptNo = pUifRegSales->usDeptNo;
        }
        pItemSales->lUnitPrice = pUifRegSales->lUnitPrice;
        pItemSales->lQTY = pUifRegSales->lQTY;
        DeptRecRcvBuffer.usDeptNo = pItemSales->usDeptNo;

        /*----- GET DEPT. RECORD -----*/
        if ((sReturnStatus = CliOpDeptIndRead(&DeptRecRcvBuffer, 0)) != OP_PERFORM) {
			char  xBuff[128];

			sprintf (xBuff, "==PROVISION: PLU Dept read error %d dept %d.", sReturnStatus, pItemSales->usDeptNo);
			NHPOS_ASSERT_TEXT((sReturnStatus == OP_PERFORM), xBuff);
            return(OpConvertError(sReturnStatus));
        }

        /* COPY CONTROL CODE INTO ITEMSALES */
        memcpy(pItemSales->ControlCode.auchPluStatus, DeptRecRcvBuffer.ParaDept.auchControlCode, OP_PLU_SHARED_STATUS_LEN);
        pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] = DeptRecRcvBuffer.ParaDept.auchControlCode[4]; /* CRT No5 - No 8 see ItemSendKdsSetCRTNo() */

		pItemSales->ControlCode.usBonusIndex = DeptRecRcvBuffer.ParaDept.usBonusIndex;
        
        /* pItemSales->ControlCode.uchDeptNo = (UCHAR) (DeptRecRcvBuffer.ParaDept.uchMdept & 0x0f);  */

        /* Print Priority, R3.0 */
        pItemSales->uchPrintPriority = DeptRecRcvBuffer.ParaDept.uchPrintPriority;
                                            /* set major dept No. */
        /* copy dept's mnemonics to pItemSales*/
        _tcsncpy(pItemSales->aszMnemonic, DeptRecRcvBuffer.ParaDept.auchMnemonic, OP_DEPT_NAME_SIZE);
		pItemSales->usTareInformation = DeptRecRcvBuffer.ParaDept.usTareInformation;
        
        /*--- TURN OFF STATUS NOT USED ---*/
        pItemSales->ControlCode.auchPluStatus[2] &= ~PLU_CONDIMENT;    /* turn off condiment status */
        pItemSales->ControlCode.auchPluStatus[3] &= ~(PLU_USE_ADJG1 | PLU_USE_ADJG2 | PLU_USE_ADJG3 | PLU_USE_ADJG4);   /* turn off adjective status */

        *puchRestrict = 0;
        *pusLinkNo = 0;
        break;

    case CLASS_PLU:
    case CLASS_PLUMODDISC:                  /* modified discounted PLU */
        /*----- PLU PREPARATION ----- */
        /*----- GET PLU RECORD with pItemSales->usPLUNo as key -----*/
        /* prepair PLU record receive buffer */

        _tcsncpy(PLURecRcvBuffer.auchPluNo, pItemSales->auchPLUNo, NUM_PLU_LEN);
        if ((pUifRegSales->uchAdjective - 1) % 5 + 1) {
	        PLURecRcvBuffer.uchPluAdj = (pUifRegSales->uchAdjective - 1) % 5 + 1;
	    } else {
	        PLURecRcvBuffer.uchPluAdj = 1;      /* dummy adjective valiation */
	    }
                                           
        if ((sReturnStatus = CliOpPluIndRead(&PLURecRcvBuffer, 0)) != OP_PERFORM) {
            if (sReturnStatus == OP_NOT_IN_FILE) {

                sReturnStatus = ItemSalesPLUBuilding(pUifRegSales, pItemSales, &PLURecRcvBuffer);

                if (sReturnStatus != ITM_SUCCESS) {
                    if (sReturnStatus == ITM_CANCEL) {
                        ItemOtherClear();
                        return (UIF_CANCEL);
                    } else {
						USHORT    usControl;
						
						/* cyclic tone at not on file, 02/19/01 */
	    
					    if (CliParaMDCCheck(MDC_PLU4_ADR, ODD_MDC_BIT1)) {
							STANZA          Stanza[10];

							PifPlayStanza(ItemSalesMakeStanza(Stanza)); /* make a pulse tone */
						} else {
							PifBeep(ITM_BEEP);                   /* make an error tone */
						}
    					usControl = UieNotonFile(UIE_ENABLE);                   /* disable using scanner */
						UieErrorMsg(LDT_NTINFL_ADR, UIE_WITHOUT|UIE_EM_TONE);
		                ItemOtherClear();
				        UieNotonFile(usControl);                      /* enable scanner */

						return (UIF_CANCEL);
					}
                }
            } else {
                return(OpConvertError(sReturnStatus));
            }
        } 

        /* move dept No. into pItemSales->uchDeptNo */
        pItemSales->usDeptNo = PLURecRcvBuffer.ParaPlu.ContPlu.usDept;
        /* pItemSales->uchDeptNo = (UCHAR) (PLURecRcvBuffer.ParaPlu.ContPlu.uchDept & ~PLU_TYPE_MASK_REG); */
        pItemSales->ControlCode.uchItemType = PLURecRcvBuffer.ParaPlu.ContPlu.uchItemType; 
        /* pItemSales->ControlCode.uchDeptNo = PLURecRcvBuffer.ParaPlu.ContPlu.uchDept;  */
                                            /* set PLU type */
        /* --- Decrease No. of Communication in case of OEP,    Dec/13/2000 --- */
        *puchTable = PLURecRcvBuffer.ParaPlu.uchTableNumber;

        if (pItemSales->usDeptNo == 0) {
            return(LDT_PROHBT_ADR);         /* error if dept isn't preset */
        }
        
        /* copy report code */
        pItemSales->ControlCode.uchReportCode = (UCHAR) (PLURecRcvBuffer.ParaPlu.ContPlu.uchRept);
        pItemSales->uchPrintPriority = PLURecRcvBuffer.ParaPlu.uchPrintPriority;  /* R3.0 */
        _tcsncpy(pItemSales->aszMnemonic, PLURecRcvBuffer.ParaPlu.auchPluName, STD_PLU_MNEMONIC_LEN);
        _tcsncpy(pItemSales->aszMnemonicAlt, PLURecRcvBuffer.ParaPlu.auchAltPluName, STD_PLU_MNEMONIC_LEN);
		pItemSales->ControlCode.usBonusIndex = PLURecRcvBuffer.ParaPlu.ContPlu.usBonusIndex;
		pItemSales->ControlCode.uchPPICode = PLURecRcvBuffer.ParaPlu.uchLinkPPI;

        /* Print Priority, R3.0 */
        pItemSales->uchPrintPriority = PLURecRcvBuffer.ParaPlu.uchPrintPriority; 
		pItemSales->uchColorPaletteCode = PLURecRcvBuffer.ParaPlu.uchColorPaletteCode;
		pItemSales->usTareInformation = PLURecRcvBuffer.ParaPlu.usTareInformation;

		// if this is a voided item, color should be VOIDED ITEM color, instead of the PLU color
		if (pUifRegSales->fbModifier & VOID_MODIFIER) // colorpalette-changes-voided_items
		{
			pItemSales->uchColorPaletteCode = 0x02;
		}

		/* copy control code - remember ControlCode.auchStatus[5] used for ItemSales status info */
        memcpy(pItemSales->ControlCode.auchPluStatus, PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther, OP_PLU_PLU_SHARED_STATUS_LEN);
        memcpy(pItemSales->ControlCode.auchPluStatus + 6, PLURecRcvBuffer.ParaPlu.ContPlu.auchContOther + 5, OP_PLU_OPTIONS_STATUS_LEN);

        if ((pItemSales->ControlCode.uchItemType & PLU_TYPE_MASK_REG) == 0) {
            pItemSales->ControlCode.uchItemType |= PLU_OPEN_TYP;
			pItemSales->fsLabelStatus |= ITM_CONTROL_NO_CONSOL;
        }                                   /* type 0 is regarded as open PLU */
        /***
        if ((pItemSales->ControlCode.uchDeptNo & PLU_TYPE_MASK_REG) == 0) {
            pItemSales->ControlCode.uchDeptNo |= PLU_OPEN_TYP;
        }                                   / type 0 is regarded as open PLU */
        
        /* set QTY and unit price */
        pItemSales->lQTY = pUifRegSales->lQTY;
        pItemSales->lUnitPrice = pUifRegSales->lUnitPrice;
        /* not adjust at random weight label */
        if ((pItemSales->uchRndType & (ITM_TYPE_PRICE|ITM_TYPE_QTY | ITM_TYPE_WEIGHT)) == 0) {
            if ((pUifRegSales->lUnitPrice != 0) || (ItemSalesLocal.fbSalesStatus & SALES_ZERO_AMT)) {
                                            /* amount entered */
                if (pItemSales->ControlCode.uchItemType & PRESET_PLU) { 
                /* if (pItemSales->ControlCode.uchSalesType & PRESET_PLU) {  */
                                            /* if not open PLU */
                    if (pItemSales->lQTY != 0) {    /* amount entered */
                        return(LDT_SEQERR_ADR); 
                                                /* because of price entry for preset PLU */
                    }                           /* ulUnitPrice is QTY without D.P */

                    if (pUifRegSales->lUnitPrice == 0L) {
                        return(LDT_KEYOVER_ADR);/* return illegal data error */
                    }  
                
                    /* entered from PLU No. key or Scanner */
                    if (pUifRegSales->uchFsc == 0) {
                        return(LDT_SEQERR_ADR); 
                    }
                    pItemSales->lQTY = pUifRegSales->lUnitPrice * 1000L;
                } 
            }
        }
        if (pItemSales->uchMinorClass == CLASS_UIDISPENSER) {
            pItemSales->lQTY = 1000L;   /* set 1 qty, if beverage dispenser sales */
        }

        if (pItemSales->ControlCode.uchItemType & PRESET_PLU) {
                                            /* if not open PLU */
            /* set record into pItemSales */
            RflConv3bto4b(&pItemSales->lUnitPrice , PLURecRcvBuffer.ParaPlu.auchPrice);
        }

        /* ramdom weight label, saratoga */
        if (pItemSales->uchRndType & ITM_TYPE_PRICE) {  /* selling on price */
            pItemSales->lQTY = pUifRegSales->lQTY;
            pItemSales->lUnitPrice = pUifRegSales->lUnitPrice;
        }
        if (pItemSales->uchRndType & (ITM_TYPE_QTY | ITM_TYPE_WEIGHT)) {  /* selling on qty, weight */
            pItemSales->lQTY = pUifRegSales->lQTY;
        }

        /* price multiple, 2172 */
        pItemSales->uchPM = PLURecRcvBuffer.ParaPlu.uchPriceMulWeight;
        if (pItemSales->uchPM == 1) {
            pItemSales->uchPM = 0;
        }

			//SR 603, removed scalable out of the statement
			//SCALABLE items should be able to be price multiplied
        if (pItemSales->ControlCode.auchPluStatus[2] & (PLU_CONDIMENT/*|PLU_SCALABLE*/)) {
            pItemSales->uchPM = 0;
        }
        if (CliParaMDCCheck(MDC_PLU2_ADR,ODD_MDC_BIT1)) {
            /* assume as QTY, 2172 */
            if (pItemSales->uchPM) {
                if (pItemSales->lQTY == 0L) {    /* if not entered qty */
                    pItemSales->lQTY = (LONG)(pItemSales->uchPM * 1000L);
                }
            }
        }
        
        /*----- INHERIT DEPT'S CONTROL STATUS -----*/
        /* get dept record */
        DeptRecRcvBuffer.usDeptNo = pItemSales->usDeptNo;
        if ((sReturnStatus = CliOpDeptIndRead(&DeptRecRcvBuffer, 0)) != OP_PERFORM) {
			char  xBuff[128];

			sprintf (xBuff, "==PROVISION: PLU Dept read error %d dept %d.", sReturnStatus, pItemSales->usDeptNo);
			NHPOS_ASSERT_TEXT((sReturnStatus == OP_PERFORM), xBuff);
            return(OpConvertError(sReturnStatus));
        }
            
        /* Print Priority, R3.0 */
        if (CliParaMDCCheck(MDC_PRINTPRI_ADR, EVEN_MDC_BIT1)) {
            pItemSales->uchPrintPriority = DeptRecRcvBuffer.ParaDept.uchPrintPriority;
        }

        ItemSalesSetupStatus(DeptRecRcvBuffer.ParaDept.auchControlCode);   /* setup dept's control status */
        /* turn on Hash bit for PLU */
        pItemSales->ControlCode.auchPluStatus[2] &= ~PLU_HASH;             /* turn off flag set by maintemance */
        if (pItemSales->ControlCode.auchPluStatus[2] & PLU_USE_DEPTCTL) {  /* use dept's control code */
            /* copy dept's control code into pItemSales */
            memcpy(pItemSales->ControlCode.auchPluStatus, DeptRecRcvBuffer.ParaDept.auchControlCode, OP_PLU_SHARED_STATUS_LEN);
            pItemSales->ControlCode.auchPluStatus[PLU_CONTROLCODE_ADR_6] = DeptRecRcvBuffer.ParaDept.auchControlCode[4]; /* CRT No5 - No 8 see ItemSendKdsSetCRTNo()*/
        } else {                            /* turn on hash bit if need */
            if (DeptRecRcvBuffer.ParaDept.auchControlCode[2] & PLU_HASH) {
                pItemSales->ControlCode.auchPluStatus[2] |= PLU_HASH;
            }
        }

        pItemSales->ControlCode.uchItemType &= PLU_TYPE_MASK_REG;    /* clear dept No. "0xc0" */
        /* pItemSales->ControlCode.uchDeptNo |= DeptRecRcvBuffer.ParaDept.uchMdept; */
                                            /* set dept's major dept No. */

        /*----- SET UP PLU STATUS -----*/
        if (!(pItemSales->ControlCode.uchItemType & PRESET_PLU)) {
            pItemSales->ControlCode.auchPluStatus[2] &= ~PLU_CONDIMENT; 
        } else if (pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) {
            pItemSales->ControlCode.auchPluStatus[2] &=~ PLU_SCALABLE;  /* turn off scalable status bit if condiment */ 
        }
        pItemSales->usLinkNo = PLURecRcvBuffer.ParaPlu.usLinkNo;

        pItemSales->usFamilyCode = PLURecRcvBuffer.ParaPlu.usFamilyCode;


        /*----- SET UP PLU STATUS for Label PLU-----*/
#if 0
        if (_tcsncmp(pItemSales->auchPLUNo, auchItemSalesMenuPLUHigh, NUM_PLU_LEN) > 0) {
            
            if ((pItemSales->ControlCode.uchItemType & PLU_TYPE_MASK_REG) == PLU_ADJ_TYP) {
                
                if ((pItemSales->ControlCode.auchStatus[3] & PLU_USE_ADJG_MASK) == 0) {
                    
                    pItemSales->ControlCode.auchStatus[3] |= PLU_USE_ADJG_MASK;
                }
            }
        }
#endif
        *puchRestrict = PLURecRcvBuffer.ParaPlu.uchRestrict;
        *pusLinkNo = PLURecRcvBuffer.ParaPlu.usLinkNo;

        pItemSales->uchGroupNumber = PLURecRcvBuffer.ParaPlu.uchGroupNumber;
        pItemSales->uchTableNumber = PLURecRcvBuffer.ParaPlu.uchTableNumber;

        break;
    
    default:
        break;
    }    

    /*----- SET UP COMMON STATUS -----*/
    ItemSalesSetupStatus(pItemSales->ControlCode.auchPluStatus);

    if ((pItemSales->uchMinorClass == CLASS_PLU) || (pItemSales->uchMinorClass == CLASS_PLUMODDISC)) {
        if (!(pItemSales->ControlCode.auchPluStatus[2] & PLU_SCALABLE) && (DeptRecRcvBuffer.ParaDept.auchControlCode[2] & PLU_SCALABLE)) { 
                                            /* not identical */
            return(LDT_PROHBT_ADR); /*  PLU       DEPT
                                        none     scalable -> prohibit operation
                                    */
        }

        /* set scalable flag for affection */
        if (DeptRecRcvBuffer.ParaDept.auchControlCode[2] & PLU_SCALABLE) {
            pItemSales->ControlCode.auchPluStatus[5] |= ITM_PLU_LINK_SCALE_DEPT;
        }
    }

    /*----- SET UP PPI CODE R3.1 -----*/
    if (pItemSales->uchMinorClass == CLASS_PLU) {
        if (pItemSales->ControlCode.uchItemType & PRESET_PLU) {
        /* if (pItemSales->ControlCode.uchDeptNo & PRESET_PLU) { */
            if (!(pItemSales->ControlCode.auchPluStatus[0] & PLU_MINUS)) {
                if (!(pItemSales->ControlCode.auchPluStatus[2] & (PLU_CONDIMENT | PLU_SCALABLE))) {
                    pItemSales->uchPPICode = PLURecRcvBuffer.ParaPlu.uchLinkPPI;
                }
            }
        }
    }

    /* ---- SET UP BEVERAGE DISPENSER SALES STATUS R3.1 ---- */
    if (pUifRegSales->uchMinorClass == CLASS_UIDISPENSER) {
        ItemSalesLocal.fbSalesStatus |= SALES_DISPENSER_SALES;
        ItemSalesLocal.fbSalesStatus &= ~ ( SALES_PM_COMPLSRY | SALES_COND_COMPLSRY); /* ignor compulsory option */
    } else {
        if ((pItemSales->ControlCode.auchPluStatus[2] & PLU_CONDIMENT) == 0) {
            ItemSalesLocal.fbSalesStatus &= ~SALES_DISPENSER_SALES;
        }
    }
    
    return(ITM_SUCCESS);
}


/*
*===========================================================================
**Synopsis: VOID   ItemSalesSetupStatus(UCHAR  *pauchStatus)
*                                    
*    Input: Nothing
*   Output: Nothing
*    InOut: pauchStatus
**Return: VOID
*
** Description: Setup control status. 
*===========================================================================
*/
VOID   ItemSalesSetupStatus(UCHAR  *pauchStatus)
{
    /* FOR HASH DEPT/ PLU. See the comments titled What is a Hash Department in rptdept.c */
    if (pauchStatus[2] & PLU_HASH) {
        pauchStatus[0] &= ~(PLU_COMP_MODKEY | PLU_COMP_CODKEY);

/*** not reset PLU_MINUS status, V3.3 **
//        pauchStatus[0] &= ~(PLU_MINUS | PLU_COMP_MODKEY | PLU_COMP_CODKEY);
***/
                                            /* turn off minus, compulsory bit */ 
        pauchStatus[2] &= ~(PLU_SCALABLE | PLU_CONDIMENT);  
                                            /* turn off scalable status bit */ 
    }

    /* FOR MINUS DEPT/ PLU */
    if (pauchStatus[0] & PLU_MINUS) {
        pauchStatus[0] &= ~(PLU_COMP_MODKEY | PLU_COMP_CODKEY);
                                            /* turn off compulsory bit */ 
        pauchStatus[2] &= ~PLU_SCALABLE;   
                                            /* turn off scalable status bit */ 
/*        pauchStatus[3] &= ~BONUS_MASK_DATA;   V3.3,   FVT Comment#11
*/                                            /* set 0 to bonus total index */
    }
}

/****** end of file ******/
