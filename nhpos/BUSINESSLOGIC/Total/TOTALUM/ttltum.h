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
* Title       : TOTAL, Header File                         
* Category    : TOTAL, NCR 2170 Hospitality Application         
* Program Name: TTLTUM.H                                                      
* --------------------------------------------------------------------------
* Abstract:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-06-92:00.00.01:           :                                    
* Apr-08-94:00.00.04: Yoshiko.J : change offline tender
* Mar-03-95:03.00.00: hkato     : R3.0
* Mar-12-96:03.01.00: T.Nakahata: R3.1 Initial
*       Add Service Time Total (Daily/PTD) and Individual Financial (Daily)
* Mar-21-96:03.01.01: T.Nakahata: Cashier Total Offset USHORT to ULONG
* Aug-12-99:03.05.00:K.Iwata    : R3.5
*       Remove WAITER_MODEL
*
** NCR2172 **
*
* Oct-05-99:01.00.00:M.Teraki   : Added #pragma(...)
* Dec-07-99:01.00.00:hrkato     : Saratoga
*
** GenPOS **
*
* Aug-29-17 : 02.02.02 : R.Chambers : TtlTUMUpRead() now static in totaltum.c.
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

#if !defined(TTLTUM_H_INCLUDED)
#define TTLTUM_H_INCLUDED

#include <ttl.h>


/*
*===========================================================================
*   Type Declarations 
*===========================================================================
*/
#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct {
    UCHAR               uchMajorClass;      /* Major Class for Read */
    UCHAR               uchMinorClass;      /* Minor Class for Read */
} TTLTUMCLASS;

typedef struct {
    UCHAR               uchNonCompLen;      /* Non Compless Length */
    UCHAR               uchMajorClass;      /* Major Class for Read */
    UCHAR               uchMinorClass;      /* Minor Class for Read */
} TTLTUMCOMPCLASS;

typedef union {
    TTLTUMCOMPCLASS     TtlCompClass;       /* Compless Data For Class Read */
    TTLTUMCLASS         TtlClass;           /* For Class Read */
    ITEMSALES           TtlItemSales;       /* Sales Data */
    ITEMDISC            TtlItemDisc;        /* Discount Data */
    ITEMCOUPON          TtlItemCoupon;      /* Coupon Data, R3.0 */
    ITEMTOTAL           TtlItemTotal;       /* Total Data */
    ITEMTENDER          TtlItemTender;      /* Tender Data */
    ITEMMISC            TtlItemMisc;        /* Misc Transaction Data */
    ITEMAFFECTION       TtlItemAffect;      /* Affection Data */
    ITEMOPECLOSE        TtlItemClose;       /* Operator Close Data */
} TTLTUMTRANUPDATE;


typedef struct {
    TTLCSCPNMISC CpnMiscTotal;              /* Daily Coupon Status, R3.0 */
    TOTAL        CpnDayTotal[FLEX_CPN_MAX];  /* Coupon Total */
} TTLCPNDAYREADTOTAL;


typedef struct {
    TTLCSCPNMISC CpnMiscTotal;              /* PTD Coupon Status, R3.0 */
    TOTAL        CpnPTDTotal[FLEX_CPN_MAX];  /* Coupon Total */
} TTLCPNPTDREADTOTAL;

#if     (defined(_WIN32_WCE) || defined(WIN32)) && _MSC_VER >= 800
#pragma pack(pop)
#else
#pragma pack()
#endif
/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/
                                        /* Transaction Qualifier Size */
#define     TTL_AFFECT           0      /* Affection Data */
#define     TTL_NOTAFFECT       -1      /* Not Affection Data */
#define     TTL_ONLINE           0      /* Online data          *add '94Apr */
#define     TTL_OFFLINE          1      /* Offline Data         *add '94Apr */


#define     TTL_TUM_ERR_FIN_AFFECT       0x01     /* Finanicial File Affect Error */
#define     TTL_TUM_ERR_HOUR_AFFECT      0x02     /* Hourly file Affect Error */
#define     TTL_TUM_ERR_DEPT_AFFECT      0x04     /* Dept file Affect Error */
#define     TTL_TUM_ERR_PLU_AFFECT       0x08     /* PLU file Affect Error */
#define     TTL_TUM_ERR_CAS_AFFECT       0x10     /* Cashier file Affect Error */
#define     TTL_TUM_ERR_WAI_AFFECT       0x20     /* Waiter file Affect Error */
#define     TTL_TUM_ERR_CPN_AFFECT       0x80     /* Coupon file Affect Error, R3.0 */


#define     TTL_MDC_SURCHARGE  0x01     /* Discount/Surcharge Bit */
#define     TTL_MDC_NOTAFFECTDEPTPLU 0x02
                                        /* Not Affect Dept/PLU Bit */
/* For Update Status */
/* - Common - */
#define     TTL_TUP_PVOID              0x0001      /* Common: Preselect Void Status, indicates CURQUAL_PVOID status in TranCurQual.fsCurStatus */
#define     TTL_TUP_TRAIN              0x0002      /* Common: Training Status */
#define     TTL_TUP_MVOID              0x0004      /* Common: Momentary Void Status */
#define     TTL_TUP_BARTENDER          0x0008      /* Common: Bartender Operation R3.1 */
#define     TTL_TUP_TRETURN            0x1000      /* Common: transaction void, indicates CURQUAL_TRETURN status in TranCurQual.fsCurStatus */
#define     TTL_TUP_MRETURN            0x2000      /* Common: transaction void, indicates Returns status in TtlItemSales.fbModifier */
#define     TTL_TUP_IGNORE             0x4000      // ignore this data when calculating totals

/* - Sales Only - */
#define     TTL_TUP_HASH               0x0010      /* Sales Only: Hash Dept/PLU Status */
#define     TTL_TUP_HASHAFFECT         0x0020      /* Sales Only: Hash Dept/PLU Not Affect Status */
#define     TTL_TUP_SCALE              0x0040      /* Sales Only: Scalable Status */
#define     TTL_TUP_CREDT              0x0080      /* Sales Only: Credit Dept/PLU Status */
#define     TTL_TUP_DEBITCASHBACK      0x0100      /* Sales Only: Scalable Status */
#define     TTL_TUP_SALES_RETURN_1     0x0200      /* Sales Only: Return #1 */
#define     TTL_TUP_SALES_RETURN_2     0x0400      /* Sales Only: Return #2 */
#define     TTL_TUP_SALES_RETURN_3     0x0800      /* Sales Only: Return #3 */
/* - Discount Only - */
#define     TTL_TUP_SURCHARGE          0x0010      /* Discount Only: Surchage Operation */
/* - VAT/SER Only - */
#define     TTL_TUP_EXCLUDE            0x0010      /* VAT/SER Only: exclude VAT/ Service */

/* - Condiment - */
#define     TTL_COND_DEPT_MASK 0x3f     /* Condiment Dept Number Mask */
#define     TTL_COND_ADJ_MASK  0xc0     /* Condiment Adjective # Mask */
#define     TTL_COND_ADJECT    0xc0     /* Condiment Adjective */

#define     TTL_UPTOTAL_FIN    0x01     /* Financial Total Updated */
#define     TTL_UPTOTAL_CAS    0x02     /* Cashier Total Updated */
#define     TTL_UPTOTAL_DPT    0x04     /* Dept Total Updated */
#define     TTL_UPTOTAL_WAI    0x08     /* Waiter Total Updated */
#define     TTL_UPTOTAL_CPN    0x80     /* Coupon Total Updated,     R3.0 */
/* ===== New Totals (Release 3.1) BEGIN ===== */
#define     TTL_UPTOTAL_SERTIME 0x40    /* Service Time Total Updated, R3.1 */
/* ===== New Totals (Release 3.1) END ===== */

#define     TTL_UPPTD_NOT      0x01     /* PTD is not Exist */
#define     TTL_UPDPT_ERR      0x80     /* Illigual Dept number */
#define     TTL_UPCPN_ERR      0x80     /* Illigual Coupon number,   R3.0 */


/* new error codes for total update aborting*/
#define TTL_UPTOTAL_WAI_ERR	0x00000001
#define TTL_UPTOTAL_PLU_ERR	0x00000002
#define TTL_UPTOTAL_DEP_ERR	0x00000004
#define TTL_UPTOTAL_CAS_ERR 0x00000008
#define TTL_UPTOTAL_CPN_ERR 0x00000010

/*
*===========================================================================
*   Extern Declarations 
*===========================================================================
*/
extern      USHORT          TtlUpTotalFlag; /* Total Update Flag */
extern      TTLCSREGFIN     TtlFinDayWork;  /* Daily Financial Work Area */
extern      TTLCSREGFIN     TtlFinPTDWork;  /* PTD Financial Work Area */
extern      ULONG           ulTtlCasDayPosition;/* Save Cashier Position */
extern      TTLCSCASTOTAL   TtlCasDayWork;  /* Cashier Work Area */
extern      ULONG           ulTtlCasPTDPosition;/* Save Cashier Position, V3.3 */
extern      TTLCSCASTOTAL   TtlCasPTDWork;  /* Cashier Work Area, V3.3 */
#if 0
extern      UCHAR           uchTtlDptStatus;/* Dept File Status */
extern      TTLDEPTDAYREADTOTAL FAR TtlDptDayWork;  /* Daily Dept Work Area */
extern      TTLDEPTPTDREADTOTAL FAR TtlDptPTDWork;  /* Daily PTD Work Area */
#endif
extern      UCHAR           uchTtlCpnStatus;   /* Coupon File Status,   R3.0 */
extern      TTLCPNDAYREADTOTAL     TtlCpnDayWork;  /* Daily Coupon Work Area,R3.0 */
extern      TTLCPNPTDREADTOTAL     TtlCpnPTDWork;  /* Daily PTD Work Area,  R3.0 */
/* ===== New Totals (Release 3.1) BEGIN ===== */
extern      TTLCSREGFIN     TtlIndFinWork;          /* Individual Financial Work Area, R3.1 */
/* ===== New Totals (Release 3.1) END ===== */

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
/*
*--------------------------------------
*           Totaltum.c
*--------------------------------------
*/


VOID TtlTumUpdate(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCS_TMPBUF_WORKING pTmpBuff, ULONG *puchAffectAbort);
SHORT TtlCheckAffectClass(TTLTUMTRANUPDATE  *pTmpBuff);

/* ===== Individual Financial Total (Release 3.1) BEGIN ===== */
VOID TtlTumFileUpdate(ULONG  ulCashierID, USHORT usTerminalNo, ULONG *puchAffectAbort);
VOID TtlTumReadTLWork(ULONG  ulCashierID, USHORT usTerminalNo, USHORT fsCurStatus, ULONG *puchAffectAbort, TTLCS_TMPBUF_WORKING pTmpBuff);
/* ===== Individual Financial Total (Release 3.1) END ===== */

/*
*--------------------------------------
*           Ttlumup.c
*--------------------------------------
*/
VOID TtlWUpdateFin(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);
SHORT TtlTReadUpdateFin(USHORT usTerminalNo, TTLCS_TMPBUF_WORKING pTmpBuff);
/* --- Individual Financial Total (Release 3.1) --- */
SHORT TtlTUpdateFin(USHORT usTerminalNo);
/* SHORT TtlTUpdateFin(VOID);*/
SHORT TtlCheckFinAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);

SHORT TtlTUpdateHour(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff);
SHORT TtlCheckHourAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);

SHORT TtlTUpdateDept(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff);

VOID TtlWUpdateDept(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);
SHORT TtlTReadUpdateDept(VOID);
/* SHORT TtlTUpdateDept(VOID);  Saratoga */
VOID TtlSetDeptCounter(TTLTUMTRANUPDATE *pTtlTranUpBuff, USHORT *pusStatus, LONG *plCounter);
SHORT TtlCheckDeptAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);

SHORT TtlTUpdatePLU(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff);
SHORT TtlCheckPLUAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);

VOID TtlWUpdateCas(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);
SHORT TtlTReadUpdateCas(ULONG ulCashierNumber, VOID *pTmpBuff, UCHAR uchMinorClass);
SHORT TtlTUpdateCas(ULONG ulCashierNumber, UCHAR uchMinorClass);
SHORT TtlCheckCasAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);     /* R3.1 */
SHORT TtlCheckCasNorAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);     /* R3.1 */
SHORT TtlCheckCasBarAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);     /* R3.1 */

SHORT TtlWUpdateCasWai(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff);  /* V3.3 */
SHORT TtlWUpdateCasWaiSub(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff, ULONG ulCashierNumber, UCHAR uchMinorClass);  /* V3.3 */
SHORT TtlCheckCasWaiAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);         /* V3.3 */
SHORT TtlCheckCasWaiBarAffectClass(TTLTUMTRANUPDATE *pTtlTranUpBuff);   /* V3.3 */
SHORT TtlCheckCasWaiNorAffectClass(TTLTUMTRANUPDATE *pTtlTranUpBuff);   /* V3.3 */

SHORT TtlWUpdateWai(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff);
SHORT TtlTUpdateWai(USHORT usWaiterNumber);
SHORT TtlCheckWaiAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);     /* R3.1 */
SHORT TtlCheckWaiNorAffectClass(TTLTUMTRANUPDATE *pTtlTranUpBuff);  /* R3.1 */
SHORT TtlCheckWaiBarAffectClass(TTLTUMTRANUPDATE *pTtlTranUpBuff);  /* R3.1 */

/*----- R3.0 -----*/
VOID TtlWUpdateCpn(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);
SHORT TtlCheckCpnAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);
SHORT TtlTReadUpdateCpn(VOID);
SHORT TtlTUpdateCpn(VOID);

/* ===== New Functions (Release 3.1) BEGIN ===== */
SHORT TtlTUpdateSerTime(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff, VOID *pTmpBuff);
SHORT TtlCheckSerTimeAffectClass(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff);
/* ===== New Functions (Release 3.1) END ===== */

/*
*--------------------------------------
*           Ttlumsub.c
*--------------------------------------
*/
/* --- Write Each Dept Total & Gross Dept Total --- */
SHORT TtlTwriteDept(TTLDEPT *pTotal, ULONG ulPositionMsc, ULONG ulPositionTtl);

/* --- Write Each PLU Total & Gross PLU Total --- */
SHORT TtlTwritePLU(TTLPLU *pTotal, ULONG ulPositionMsc, ULONG ulPositionTtl);

/*
*--------------------------------------
*           Ttlsbfup.c
*--------------------------------------
*/
VOID TtlFinupdate(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSREGFIN FAR *pTmpBuff);
VOID TtlFSales(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sCounter, USHORT uchBOffset, TTLCSREGFIN  *pTmpBuff);
VOID TtlFModDiscount(USHORT usStatus, DCURRENCY lAmount, SHORT sSign, TTLCSREGFIN  *pTmpBuff);
VOID TtlFDiscount(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sSign, UCHAR uchMinorClass, TTLCSREGFIN *pTmpBuff);
VOID TtlFTotal(USHORT usStatus, SHORT sSign, TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSREGFIN *pTmpBuff);
VOID TtlFTender(USHORT usStatus, SHORT sSign, TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSREGFIN *pTmpBuff);
VOID TtlFmisc(USHORT usStatus, DCURRENCY lAmount, SHORT sSign, UCHAR uchMinorClass, TTLCSREGFIN *pTmpBuff);
VOID TtlFAffect(USHORT usStatus, SHORT sSign, TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSREGFIN FAR *pTmpBuff);
VOID TtlUpFinCGGTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpFinTaxable(TTLCSREGFIN  *pTmpBuff, DCURRENCY lTaxable, DCURRENCY lTaxAmount, DCURRENCY lTaxExempt, DCURRENCY lFSExempt, UCHAR uchOffset);     /* Saratoga */
VOID TtlUpFinTGGTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpFinDeclaredTips(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpFinDGGTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpFinPlusVoid(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpFinPreVoid(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpFinTransactionReturn(USHORT fbMod, TTLCSREGFIN *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpFinConsCoupon(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpFinItemDisc(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpFinModiDisc(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpFinRegDisc(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOffset);
VOID TtlUpFinPaidOut(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpFinRecvAcount(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpFinTipsPaid(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpFinCashTender(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOnOff );
VOID TtlUpFinCheckTender(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOnOff);
VOID TtlUpFinFSChange(TTLCSREGFIN  *pBuff, ITEMTENDER *pItem, SHORT sSign);  /* Saratoga */
VOID TtlUpFinChargeTender(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOnOff);
VOID TtlUpFinMiscTender(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOffset, UCHAR uchOnOff);
VOID TtlUpFinForeignTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOffset);
VOID TtlUpFinServiceTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOffset);
VOID TtlUpFinAddCheckTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOffset);
VOID TtlUpFinChargeTips(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOffset);
VOID TtlUpFinTransCancel(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpFinMiscVoid(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpFinAudaction(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpFinNoSaleCount(TTLCSREGFIN  *pTmpBuff);
VOID TtlUpFinItemProduct(TTLCSREGFIN  *pTmpBuff, LONG lCounter);
VOID TtlUpFinNoOfPerson(TTLCSREGFIN  *pTmpBuff, SHORT sCounter);
VOID TtlUpFinNoOfChkOpen(TTLCSREGFIN  *pTmpBuff, SHORT sCounter);
VOID TtlUpFinNoOfChkClose(TTLCSREGFIN  *pTmpBuff, SHORT sCounter);
VOID TtlUpFinCustomer(TTLCSREGFIN  *pTmpBuff, SHORT sCounter);
VOID TtlUpFinHashDept(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpFinBonus(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, USHORT uchOffset);
VOID TtlUpFinConsTax(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount);

/*----- R3.0 -----*/
VOID TtlFCpn(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sSign, TTLCSREGFIN  *pTmpBuff);
VOID TtlUpFinCpn(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);

/* ===== New Functions (Release 3.1) BEGIN ===== */
VOID TtlUpFinNetTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount);
/* ===== New Functions (Release 3.1) END ===== */
/* V3.3 */
VOID TtlFVatService(USHORT usStatus, SHORT sSign, TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSREGFIN  *pTmpBuff);
VOID TtlUpFinVATServiceTotal(TTLCSREGFIN  *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlFLoanPickClose(TTLTUMTRANUPDATE *pClass, TTLCSREGFIN  *pFin);    /* Saratoga */
VOID TtlFLoanPick(TTLTUMTRANUPDATE *pClass, TTLCSREGFIN  *pFin);         /* Saratoga */
VOID TtlFEptError(TTLTUMTRANUPDATE *pClass, TTLCSREGFIN *pFin);

VOID TtlFUCpn(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sSign, TTLCSREGFIN FAR *pTmpBuff);
VOID TtlUpFinUCpn(TTLCSREGFIN *pTmpBuff, DCURRENCY lAmount, SHORT sCounter); /* saratoga */

/*                                                                       
*--------------------------------------
*           Ttlsbhup.c
*--------------------------------------
*/

SHORT TtlHourupdate(HOURLY *pHourlyTotal, UCHAR uchOffset, HOURLY *pTmpBuff);
/*
*--------------------------------------
*           Ttlsbdup.c
*--------------------------------------
*/
SHORT TtlDeptupdate(USHORT usStatus, USHORT usDeptNo, DCURRENCY lAmount, LONG lCounter, VOID *pTmpBuff);
/*
*--------------------------------------
*           Ttlsbpup.c
*--------------------------------------
*/
SHORT TtlPLUupdate(USHORT usStatus, TCHAR *puchPLUNo, UCHAR uchAjectNo, DCURRENCY lAmount, LONG lCounter, VOID *pTmpBuff);
SHORT TtlPLU_AddTotal (USHORT usPluTtlType, TCHAR *puchPLUNo, UCHAR uchAjectNo, DCURRENCY lAmount, LONG lCounter, VOID *pTmpBuff);

/*
*--------------------------------------
*           Ttlsbcup.c
*--------------------------------------
*/
VOID TtlCasupdate(TTLTUMTRANQUAL *pTtlTranQualBuff, TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff);
VOID TtlCSales(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sCounter, USHORT uchBOffset, TTLCSCASTOTAL *pTmpBuff);
VOID TtlCModDiscount(USHORT usStatus, DCURRENCY lAmount, SHORT sSign, TTLCSCASTOTAL *pTmpBuff);
VOID TtlCDiscount(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sSign, UCHAR uchMinorClass, TTLCSCASTOTAL *pTmpBuff);
VOID TtlCTotal(USHORT usStatus, SHORT sSign, TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff);
VOID TtlCTender(USHORT usStatus, SHORT sSign, TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff);
VOID TtlCmisc(USHORT usStatus, DCURRENCY lAmount, SHORT sSign, UCHAR uchMinorClass, TTLCSCASTOTAL *pTmpBuff);
VOID TtlCAffect(USHORT usStatus, SHORT sSign, TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff);
VOID TtlUpCasCGGTotal(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpCasTaxable(TTLCSCASTOTAL FAR *pTmpBuff, DCURRENCY lTaxable, DCURRENCY lTaxAmount, DCURRENCY lTaxExempt, DCURRENCY lFSExempt, UCHAR uchOffset);
VOID TtlUpCasTGGTotal(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpCasDeclaredTips(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasMoney(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, UCHAR uchTenderClass);/* Saratoga */
VOID TtlUpCasDGGTotal(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpCasPlusVoid(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpCasPreVoid(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasTransactionReturn(USHORT fbMod, TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasConsCoupon(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);

VOID TtlUpCasItemDisc(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasModiDisc(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasRegDisc(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOffset);

TOTAL  *TtlUpdateDiscountTotal (TOTAL *pTotal, DCURRENCY lAmount, SHORT sCounter);

VOID TtlUpCasPaidOut(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasRecvAcount(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasTipsPaid(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasDeclaredPaid(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasCashTender(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasCheckTender(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasFSChange(TTLCSCASTOTAL FAR *pBuff, ITEMTENDER *pItem, SHORT sSign);/* Saratoga */
VOID TtlUpCasChargeTender(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasMiscTender(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOffset);
VOID TtlUpCasForeignTotal(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOffset);
VOID TtlUpCasServiceTotal(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOffset);
VOID TtlUpCasAddCheckTotal(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOffset);
VOID TtlUpCasChargeTips(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, UCHAR uchOffset);
VOID TtlUpCasTransCancel(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpCasMiscVoid(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpCasAudaction(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount);
VOID TtlUpCasNoSaleCount(TTLCSCASTOTAL *pTmpBuff);
VOID TtlUpCasItemProduct(TTLCSCASTOTAL *pTmpBuff, LONG lCounter);
VOID TtlUpCasNoOfPerson(TTLCSCASTOTAL *pTmpBuff, SHORT sCounter);
VOID TtlUpCasNoOfChkOpen(TTLCSCASTOTAL *pTmpBuff, SHORT sCounter);
VOID TtlUpCasNoOfChkClose(TTLCSCASTOTAL *pTmpBuff, SHORT sCounter);
VOID TtlUpCasCustomer(TTLCSCASTOTAL *pTmpBuff, SHORT sCounter);
VOID TtlUpCasHashDept(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasBonus(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter, USHORT uchOffset);
VOID TtlUpCasConsTax(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount);


/*----- R3.0 -----*/
VOID TtlCCpn(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sSign, TTLCSCASTOTAL *pTmpBuff);
VOID TtlUpCasCpn(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);

/* ----- Add New Functions (Release 3.1) BEGIN ----- */
VOID TtlCPostReceipt(TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff);
VOID TtlUpCasPostReceipt(TTLCSCASTOTAL *pTmpBuff);
/* ----- Add New Functions (Release 3.1) END ----- */

/* ----- V3.3 ---- */
VOID TtlUpCasCheckTransTo(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlUpCasCheckTransFrom(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);

VOID TtlCVatService(USHORT usStatus, SHORT sSign, TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff);
VOID TtlUpCasVATServiceTotal(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);
VOID TtlCLoanPick(TTLTUMTRANUPDATE *pTtlTranUpBuff, TTLCSCASTOTAL *pTmpBuff);
VOID TtlCLoanPickTender(UCHAR uchMinorClass, DCURRENCY lAmount, TTLCSCASTOTAL *pCas);
VOID    TtlCLoanPickClose(TTLTUMTRANUPDATE *pClass, TTLCSCASTOTAL *pCas);
VOID    TtlCEptError(TTLTUMTRANUPDATE *pClass, TTLCSCASTOTAL *pCas);

VOID TtlCUCpn(USHORT fbMod, USHORT usStatus, DCURRENCY lAmount, SHORT sSign, TTLCSCASTOTAL *pTmpBuff);
VOID TtlUpCasUCpn(TTLCSCASTOTAL *pTmpBuff, DCURRENCY lAmount, SHORT sCounter);   /* saratoga */

/*
*--------------------------------------
*           Ttlsbwup.c
*--------------------------------------
*/
/*
*--------------------------------------
*           Ttlcpnup.c,    R3.0
*--------------------------------------
*/
/* ===== Fix a glitch (11/16/95, Rel3.1) BEGIN ===== */
VOID TtlCpnupdate(USHORT usStatus, UCHAR uchCpnNo, DCURRENCY lAmount, SHORT sCounter);
/* VOID TtlCpnupdate(UCHAR uchCpnNo, LONG lAmount, SHORT sCounter);*/
/* ===== Fix a glitch (11/16/95, Rel3.1) END ===== */


/* ===== New Functions (Release 3.1) BEGIN ===== */
/*                                                                       
*--------------------------------------
*           TtlTimup.c,     R3.1
*--------------------------------------
*/
SHORT TtlSerTimeupdate(TOTAL *pTotal, UCHAR uchHourOffset, UCHAR uchTimeBorder, TOTAL *pTempBuff);

/* ===== New Functions (Release 3.1) END ===== */

/*
*--------------------------------------
*           TtlBkup.c
*--------------------------------------
*/
VOID TtlTUMTryToUpdateBackup(VOID);
/****** end of definition ******/

#endif