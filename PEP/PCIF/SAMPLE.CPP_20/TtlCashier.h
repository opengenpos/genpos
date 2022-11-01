/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlCashier.h
//
//  PURPOSE:    Cashier Total class definitions and declarations.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//            D E F I N I T I O N s    A N D    I N C L U D E s
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TTLCASHIER_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_TTLCASHIER_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if !defined(AFX_TOTAL_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
	#error include 'Total.h' before including this file
#endif

/////////////////////////////////////////////////////////////////////////////
//
//                  C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CTtlCashier
//---------------------------------------------------------------------------

class CTtlCashier : public CTotal
{
public:
    CTtlCashier();
    ~CTtlCashier();

    BOOL    Read( const UCHAR  uchClassToRead, const ULONG  ulCashierNo );
    BOOL    Reset( const ULONG  ulCashierNo, const BOOL fIsEODReset = TRUE, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    IsResetOK( const ULONG  ulCashierNo );
    BOOL    ResetAsOK( const ULONG  ulCashierNo );

    const   DATE&       getFromDate() const;
    const   DATE&       getToDate() const;
            ULONG       getCashierNo() const;
            DCURRENCY   getDailyGrossTotal() const;
    const   TOTAL&      getPlusVoid() const;
    const   TOTAL&      getPreselectVoid() const;
    const   TOTAL&      getTransactionReturn() const;
    const   TOTAL&      getTransactionExchng() const;
    const   TOTAL&      getTransactionRefund() const;

    const   TOTAL&      getConsolidatedCoupon() const;
    const   TOTAL&      getItemDiscount(int iDisc = 1) const;
    const   TOTAL&      getModifiedDiscount() const;
    const   TOTAL&      getRegularDiscount( const USHORT usDiscountNo ) const;
    const   TOTAL&      getCombinationCoupon() const;
    const   TOTAL&      getPaidOut() const;
    const   TOTAL&      getReceivedOnAccount() const;
    const   TOTAL&      getTipsPaidOut() const;
/* del, Saratoga */
//    const   TOTAL&      getCashTender() const;
//    const   TOTAL&      getCheckTender() const;
//    const   TOTAL&      getChargeTender() const;
//    const   TOTAL&      getMiscTender( const USHORT usMiscNo ) const;
//    const   TOTAL&      getForeginTotal( const USHORT usTotalNo ) const;
    const   TENDHT&      getCashTender() const;                           /* Saratoga */
    const   TENDHT&      getCheckTender() const;                          /* Saratoga */ 
    const   TENDHT&      getChargeTender() const;                         /* Saratoga */ 
    const   TENDHT&      getMiscTender( const USHORT usMiscNo ) const;    /* Saratoga */ 
	const   TENDHT&      getNormalizedTender(const USHORT usTenderNo) const;    /* Saratoga */
    const   TENDHT&      getForeginTotal( const USHORT usTotalNo ) const; /* Saratoga */

    
	const   TOTAL&      getServiceTotal( const USHORT usTotalNo ) const;
    const   TOTAL&      getAddCheckTotal( const USHORT usTotalNo ) const;
    const   TOTAL&      getChargeTips( const USHORT usTotalNo ) const;
    const   TOTAL&      getDeclaredTips() const;
            DCURRENCY   getConsolidationTax() const;
    const   TOTAL&      getBonus( const USHORT usBonusNo ) const;
    const   TOTAL&      getHashDepartment() const;
    const   TOTAL&      getTransactionCancel() const;
    const   TOTAL&      getMiscVoid() const;
    const   TOTAL&      getAudaction() const;
            SHORT       getNoSaleCount() const;
            LONG        getItemProductivity() const;
            SHORT       getNoOfPerson() const;
            SHORT       getNoOfCustomer() const;
            SHORT       getCheckOpened() const;
            SHORT       getCheckClosed() const;
            USHORT      getPostReceipt() const;
            DCURRENCY   getVATServiceTotal() const;
    const   TOTAL&      getCheckTransTo() const;
    const   TOTAL&      getCheckTransFrom() const;
	const   TOTAL&      getLoan() const;
	const   TOTAL&      getPickup() const;
    const   TOTAL&      getFoodStampCredit() const;              // ### ADD Saratoga (052900)
    const   TOTAL&      getUPCCoupon( const USHORT usNo ) const; // ### ADD Saratoga (052900)

private:
    BOOL    Read( const UCHAR uchClassToRead );
    BOOL    Read( const UCHAR uchClassToRead, const USHORT usRecordNo, const UCHAR  uchSubRecordNo );
    BOOL    Read( const UCHAR  uchClassToRead, const USHORT usRecordNo );
    BOOL    Reset( const BOOL fIsEODReset, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    BOOL    IsResetOK();
    BOOL    IsResetOK( const USHORT usRecordNo );
    BOOL    ResetAsOK();
    BOOL    ResetAsOK( const USHORT usRecordNo );
    BOOL    Reset( const USHORT  usRecordNo, const BOOL fIsEODReset = TRUE, const UCHAR uchMajorClass = CLASS_TTLEODRESET );

public:
    TTLCASHIER  m_ttlCashier;
};

/////////////////////////////////////////////////////////////////////////////
//
//                  I N L I N E    F U N C T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//  RESERVED FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CTtlCashier::Read()
//===========================================================================

inline BOOL CTtlCashier::Read(
    const UCHAR uchClassToRead
    )
{
    return ( CTotal::Read( uchClassToRead ));
}

//===========================================================================
//  FUNCTION :  CTtlCashier::Read()
//===========================================================================

inline BOOL CTtlCashier::Read(
    const UCHAR uchClassToRead,
    const USHORT usRecordNo,
    const UCHAR  uchSubRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, usRecordNo, uchSubRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlCashier::Reset()
//===========================================================================

inline BOOL CTtlCashier::Reset(
    const BOOL fIsEODReset, const UCHAR uchMajorClass
    )
{
    return ( CTotal::Reset( fIsEODReset, uchMajorClass ));
}

//===========================================================================
//  FUNCTION :  CTtlCashier::IsResetOK()
//===========================================================================

inline BOOL CTtlCashier::IsResetOK()
{
    return ( CTotal::IsResetOK());
}

//===========================================================================
//  FUNCTION :  CTtlCashier::ReadAsOK()
//===========================================================================

inline BOOL CTtlCashier::ResetAsOK()
{
    return ( CTotal::ResetAsOK());
}

//---------------------------------------------------------------------------
//
//  MEMBER VARIABLE ACCESS FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CTtlCashier::getFromDate()
//===========================================================================

inline const DATE& CTtlCashier::getFromDate() const
{
    return ( m_ttlCashier.FromDate );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getToDate()
//===========================================================================

inline const DATE& CTtlCashier::getToDate() const
{
    return ( m_ttlCashier.ToDate );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getCashierNo()
//===========================================================================

inline ULONG CTtlCashier::getCashierNo() const
{
    return ( m_ttlCashier.ulCashierNumber );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getDailyGrossTotal()
//===========================================================================

inline DCURRENCY CTtlCashier::getDailyGrossTotal() const
{
    return ( m_ttlCashier.lDGGtotal );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getPlusVoid()
//===========================================================================

inline const TOTAL& CTtlCashier::getPlusVoid() const
{
    return ( m_ttlCashier.PlusVoid );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getPreselectVoid()
//===========================================================================

inline const TOTAL& CTtlCashier::getPreselectVoid() const
{
    return ( m_ttlCashier.PreselectVoid );
}

inline const TOTAL& CTtlCashier::getTransactionReturn() const
{
    return ( m_ttlCashier.TransactionReturn );
}

inline const TOTAL& CTtlCashier::getTransactionExchng() const
{
    return ( m_ttlCashier.TransactionExchng );
}

inline const TOTAL& CTtlCashier::getTransactionRefund() const
{
	return ( m_ttlCashier.TransactionRefund );
}


//===========================================================================
//  FUNCTION :  CTtlCashier::getConsolidatedCoupon()
//===========================================================================

inline const TOTAL& CTtlCashier::getConsolidatedCoupon() const
{
    return ( m_ttlCashier.ConsCoupon );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getItemDiscount()
//===========================================================================

inline const TOTAL& CTtlCashier::getItemDiscount(int iDisc) const
{
	// allow us to select the item discount numbers 1 through 6.
	switch (iDisc) {
		case 2:
			return m_ttlCashier.ModiDisc;
		case 3:
			return m_ttlCashier.ItemDiscExtra.TtlIAmount[0];
		case 4:
			return m_ttlCashier.ItemDiscExtra.TtlIAmount[1];
		case 5:
			return m_ttlCashier.ItemDiscExtra.TtlIAmount[2];
		case 6:
			return m_ttlCashier.ItemDiscExtra.TtlIAmount[3];
		case 1:
		default:
			// just return the default of item discount #1
			return m_ttlCashier.ItemDisc;
	}
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getModifiedDiscount()
//===========================================================================

inline const TOTAL& CTtlCashier::getModifiedDiscount() const
{
    return ( m_ttlCashier.ModiDisc );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getRegularDiscount()
//===========================================================================

inline const TOTAL& CTtlCashier::getRegularDiscount(
    const USHORT usDiscountNo
    ) const
{
    ASSERT(( 0 < usDiscountNo ) && ( usDiscountNo <= FSC_RDISC_MAX ));

    return ( m_ttlCashier.RegDisc[ usDiscountNo - 1 ] );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getCombinationCoupon()
//===========================================================================

inline const TOTAL& CTtlCashier::getCombinationCoupon() const
{
    return ( m_ttlCashier.Coupon );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getPaidOut()
//===========================================================================

inline const TOTAL& CTtlCashier::getPaidOut() const
{
    return ( m_ttlCashier.PaidOut);
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getReceivedOnAccount()
//===========================================================================

inline const TOTAL& CTtlCashier::getReceivedOnAccount() const
{
    return ( m_ttlCashier.RecvAcount );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getTipsPaidOut()
//===========================================================================

inline const TOTAL& CTtlCashier::getTipsPaidOut() const
{
    return ( m_ttlCashier.TipsPaid );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getCashTender()
//===========================================================================

//inline const TOTAL& CTtlCashier::getCashTender() const /* Saratoga */
inline const TENDHT& CTtlCashier::getCashTender() const
{
    return ( m_ttlCashier.CashTender );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getCheckTender()
//===========================================================================

//inline const TOTAL& CTtlCashier::getCheckTender() const /* Saratoga */
inline const TENDHT& CTtlCashier::getCheckTender() const
{
    return ( m_ttlCashier.CheckTender );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getChargeTender()
//===========================================================================

//inline const TOTAL& CTtlCashier::getChargeTender() const /* Saratoga */
inline const TENDHT& CTtlCashier::getChargeTender() const
{
    return ( m_ttlCashier.ChargeTender );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getMiscTender()
//===========================================================================

//inline const TOTAL& CTtlCashier::getMiscTender(          /* Saratoga */
inline const TENDHT& CTtlCashier::getMiscTender(
    const USHORT usMiscNo
    ) const
{
    ASSERT(( 0 < usMiscNo ) && ( usMiscNo <= pcttl::MAX_MISC_TENDER ));

    return ( m_ttlCashier.MiscTender[ usMiscNo - 1 ] );
}

inline const TENDHT& CTtlCashier::getNormalizedTender( const USHORT usTenderNo) const
{
	switch (usTenderNo) {
		case 1:
			return (m_ttlCashier.CashTender);
		case 2:
			return (m_ttlCashier.CheckTender);
		case 3:
			return (m_ttlCashier.ChargeTender);
		default:
			// miscellanous tenders
			if (usTenderNo >= 4 && usTenderNo <= 20) {
				return (m_ttlCashier.MiscTender[usTenderNo - 4]);
			}
			break;
	}

	return (m_ttlCashier.CashTender);
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getForeginTotal()
//===========================================================================

//inline const TOTAL& CTtlCashier::getForeginTotal(        /* Saratoga */
inline const TENDHT& CTtlCashier::getForeginTotal(
    const USHORT usTotalNo
    ) const
{
    ASSERT(( 0 < usTotalNo ) && ( usTotalNo <= pcttl::MAX_FOREIGN_TOTAL ));

    return ( m_ttlCashier.ForeignTotal[ usTotalNo - 1 ] );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getServiceTotal()
//===========================================================================

inline const TOTAL& CTtlCashier::getServiceTotal(
    const USHORT usTotalNo
    ) const
{
    ASSERT(( 0 < usTotalNo ) && ( usTotalNo <= pcttl::MAX_SERVICE_TOTAL ));

    return ( m_ttlCashier.ServiceTotal[ usTotalNo - 1 ] );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getAddCheckTotal()
//===========================================================================

inline const TOTAL& CTtlCashier::getAddCheckTotal(
    const USHORT usTotalNo
    ) const
{
    ASSERT(( 0 < usTotalNo ) && ( usTotalNo <= FSC_ADDCHK_MAX ));

    return ( m_ttlCashier.AddCheckTotal[ usTotalNo - 1 ] );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getChargeTips()
//===========================================================================

inline const TOTAL& CTtlCashier::getChargeTips(
    const USHORT usTotalNo
    ) const
{
    ASSERT(( 0 < usTotalNo ) && ( usTotalNo <= FSC_CHGTIP_MAX ));

    return ( m_ttlCashier.ChargeTips[ usTotalNo - 1 ] );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getDeclaredTips()
//===========================================================================

inline const TOTAL& CTtlCashier::getDeclaredTips() const
{
    return ( m_ttlCashier.DeclaredTips );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getConsolidationTax()
//===========================================================================

inline DCURRENCY CTtlCashier::getConsolidationTax() const
{
    return ( m_ttlCashier.lConsTax );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getBonus()
//===========================================================================

inline const TOTAL& CTtlCashier::getBonus(
    const USHORT usBonusNo
    ) const
{
    ASSERT(( 0 < usBonusNo ) && ( usBonusNo <= pcttl::MAX_BONUS_TYPE ));

    return ( m_ttlCashier.Bonus[ usBonusNo - 1 ] );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getHashDepartment()
//===========================================================================

inline const TOTAL& CTtlCashier::getHashDepartment() const
{
    return ( m_ttlCashier.HashDepartment );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getTransactionCancel()
//===========================================================================

inline const TOTAL& CTtlCashier::getTransactionCancel() const
{
    return ( m_ttlCashier.TransCancel );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getMiscVoid()
//===========================================================================

inline const TOTAL& CTtlCashier::getMiscVoid() const
{
    return ( m_ttlCashier.MiscVoid );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getAudaction()
//===========================================================================

inline const TOTAL& CTtlCashier::getAudaction() const
{
    return ( m_ttlCashier.Audaction );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getNoSaleCount()
//===========================================================================

inline SHORT CTtlCashier::getNoSaleCount() const
{
    return ( m_ttlCashier.sNoSaleCount );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getItemProductivity()
//===========================================================================

inline LONG CTtlCashier::getItemProductivity() const
{
    return ( m_ttlCashier.lItemProductivityCount );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getNoOfPerson()
//===========================================================================

inline SHORT CTtlCashier::getNoOfPerson() const
{
    return ( m_ttlCashier.sNoOfPerson );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getNoOfCustomer()
//===========================================================================

inline SHORT CTtlCashier::getNoOfCustomer() const
{
    return ( m_ttlCashier.sCustomerCount );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getCheckOpened()
//===========================================================================

inline SHORT CTtlCashier::getCheckOpened() const
{
    return ( m_ttlCashier.sNoOfChkOpen );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getCheckClosed()
//===========================================================================

inline SHORT CTtlCashier::getCheckClosed() const
{
    return ( m_ttlCashier.sNoOfChkClose );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getPostReceipt()
//===========================================================================

inline USHORT CTtlCashier::getPostReceipt() const
{
    return ( m_ttlCashier.usPostRecCo );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getVATServiceTotal()
//===========================================================================

inline DCURRENCY CTtlCashier::getVATServiceTotal() const
{
    return ( m_ttlCashier.lVATServiceTotal );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getCheckTransTo()
//===========================================================================

inline const TOTAL& CTtlCashier::getCheckTransTo() const
{
    return ( m_ttlCashier.CheckTransTo );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getCheckTransFrom()
//===========================================================================

inline const TOTAL& CTtlCashier::getCheckTransFrom() const
{
    return ( m_ttlCashier.CheckTransFrom );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getLoan()
//===========================================================================
inline const TOTAL& CTtlCashier::getLoan() const
{
	return ( m_ttlCashier.Loan );
}

//===========================================================================
//  FUNCTION :  CTtlCashier::getPickup()
//===========================================================================
inline const TOTAL& CTtlCashier::getPickup() const
{
	return ( m_ttlCashier.Pickup );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::get(), ### ADD Saratoga (052900)
//===========================================================================

inline const TOTAL& CTtlCashier::getFoodStampCredit() const
{
	return (m_ttlCashier.FoodStampCredit);
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::get(), ### ADD Saratoga (052900)
//===========================================================================

inline const TOTAL& CTtlCashier::getUPCCoupon(
    const USHORT usNo
    ) const
{
    ASSERT(( 0 < usNo ) && ( usNo <= pcttl::MAX_COUPON ));

    return (m_ttlCashier.aUPCCoupon[ usNo - 1 ] );
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TTLCASHIER_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

//////////////////// END OF FILE ( TtlCashier.h ) ///////////////////////////
