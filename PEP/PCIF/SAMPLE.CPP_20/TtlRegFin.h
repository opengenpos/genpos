/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TtlRegFin.h
//
//  PURPOSE:    Register Financial Total class definitions and declarations.
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

#if !defined(AFX_TTLREGFIN_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_TTLREGFIN_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

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
//  CTtlRegFin
//---------------------------------------------------------------------------

class CTtlRegFin : public CTotal
{
public:
    CTtlRegFin();
    virtual ~CTtlRegFin();

    virtual BOOL Read( const UCHAR  uchClassToRead );
    virtual BOOL Read( const UCHAR  uchClassToRead, const USHORT usRecordNo );
    virtual BOOL Reset( const BOOL fIsEODReset = TRUE, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    virtual BOOL Reset( const USHORT usRecordNo, const BOOL fIsEODReset, const UCHAR uchMajorClass = CLASS_TTLEODRESET );
    virtual BOOL IsResetOK();
    virtual BOOL ResetAsOK();
    virtual BOOL IsResetOK( const USHORT usTerminalNo );
    virtual BOOL ResetAsOK( const USHORT usTerminalNo );

    const   DATE&       getFromDate() const;
    const   DATE&       getToDate() const;
            USHORT      getTerminalNo() const;
    const   D13DIGITS&  getCurrentGrossTotal() const;
            DCURRENCY   getNetTotal() const;
            DCURRENCY   getDailyGrossTotal() const;
            DCURRENCY   getTrainingTotal() const;
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
    const   TOTAL&      getFoodStampCredit() const;  // ### ADD Saratoga (052900)
    const   TOTAL&      getCashTenderOnline() const;
    const   TOTAL&      getCashTenderOffline() const;
    const   TOTAL&      getCheckTenderOnline() const;
    const   TOTAL&      getCheckTenderOffline() const;
    const   TOTAL&      getChargeTenderOnline() const;
    const   TOTAL&      getChargeTenderOffline() const;
    const   TOTAL&      getMiscTenderOnline( const USHORT usMiscNo ) const;
    const   TOTAL&      getMiscTenderOffline( const USHORT usMiscNo ) const;
	const   TEND&       getNormalizedTender(const USHORT usTenderNo) const;
    const   TOTAL&      getForeginTotal( const USHORT usTotalNo ) const;
    const   TOTAL&      getServiceTotal( const USHORT usTotalNo ) const;
    const   TOTAL&      getAddCheckTotal( const USHORT usTotalNo ) const;
    const   TOTAL&      getChargeTips( const USHORT usTotalNo ) const;
	const   TOTAL&      getLoan() const;            // ADD Saratoga
	const   TOTAL&      getPickup() const;          // ADD Saratoga
    const   TOTAL&      getDeclaredTips() const;
    const   D13DIGITS&  getTaxGross( const USHORT usTaxNo ) const;
            DCURRENCY   getTaxAmount( const USHORT usTaxNo ) const;
            DCURRENCY   getTaxExempt( const USHORT usTaxNo ) const;
            DCURRENCY   getFSTaxExempt( const USHORT usTaxNo ) const; // ### ADD Saratoga (052900)
    const   D13DIGITS&  getNonTaxable() const;
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
    const   TOTAL&      getUPCCoupon( const USHORT usNo ) const; // ### ADD Saratoga (052900)

private:
    BOOL Read( const UCHAR  uchClassToRead,
               const USHORT usRecordNo,
               const UCHAR  uchSubRecordNo );
	BOOL Read( const UCHAR  uchClassToRead, const ULONG  ulRecordNo );

public:
    TTLREGFIN   m_ttlRegFin;
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
//  FUNCTION :  CTtlRegFin::Read()
//===========================================================================

inline BOOL CTtlRegFin::Read(
    const UCHAR uchClassToRead,
    const USHORT usRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, usRecordNo ));
}

inline BOOL CTtlRegFin::Read(
    const UCHAR uchClassToRead,
    const ULONG ulRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, ulRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::Read()
//===========================================================================

inline BOOL CTtlRegFin::Read(
    const UCHAR uchClassToRead,
    const USHORT usRecordNo,
    const UCHAR  uchSubRecordNo
    )
{
    return ( CTotal::Read( uchClassToRead, usRecordNo, uchSubRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::Reset()
//===========================================================================

inline BOOL CTtlRegFin::Reset(
    const USHORT usRecordNo,
    const BOOL fIsEODReset, const UCHAR uchMajorClass
    )
{
    return ( CTotal::Reset( usRecordNo, fIsEODReset, uchMajorClass ));
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::IsResetOK()
//===========================================================================

inline BOOL CTtlRegFin::IsResetOK(
    const USHORT usRecordNo
    )
{
    return ( CTotal::IsResetOK( usRecordNo ));
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::ResetAsOK()
//===========================================================================

inline BOOL CTtlRegFin::ResetAsOK(
    const USHORT usRecordNo
    )
{
    return ( CTotal::ResetAsOK( usRecordNo ));
}

//---------------------------------------------------------------------------
//
//  MEMBER VARIABLE ACCESS FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CTtlRegFin::getFromDate()
//===========================================================================

inline const DATE& CTtlRegFin::getFromDate() const
{
    return ( m_ttlRegFin.FromDate );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getToDate()
//===========================================================================

inline const DATE& CTtlRegFin::getToDate() const
{
    return ( m_ttlRegFin.ToDate );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getTerminalNo()
//===========================================================================

inline USHORT CTtlRegFin::getTerminalNo() const
{
    return ( m_ttlRegFin.usTerminalNumber );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getCurrentGrossTotal()
//===========================================================================

inline const D13DIGITS& CTtlRegFin::getCurrentGrossTotal() const
{
    return ( m_ttlRegFin.CGGTotal );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getNetTotal()
//===========================================================================

inline DCURRENCY CTtlRegFin::getNetTotal() const
{
    return ( m_ttlRegFin.lNetTotal );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getDailyGrossTotal()
//===========================================================================

inline DCURRENCY CTtlRegFin::getDailyGrossTotal() const
{
    return ( m_ttlRegFin.lDGGtotal );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getTrainingTotal()
//===========================================================================

inline DCURRENCY CTtlRegFin::getTrainingTotal() const
{
    return ( m_ttlRegFin.lTGGTotal );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getPlusVoid()
//===========================================================================

inline const TOTAL& CTtlRegFin::getPlusVoid() const
{
    return ( m_ttlRegFin.PlusVoid );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getPreselectVoid()
//===========================================================================

inline const TOTAL& CTtlRegFin::getPreselectVoid() const
{
    return ( m_ttlRegFin.PreselectVoid );
}

inline const TOTAL& CTtlRegFin::getTransactionReturn() const
{
    return ( m_ttlRegFin.TransactionReturn );
}

inline const TOTAL& CTtlRegFin::getTransactionExchng() const
{
	return ( m_ttlRegFin.TransactionExchng );
}

inline const TOTAL& CTtlRegFin::getTransactionRefund() const
{
    return ( m_ttlRegFin.TransactionRefund );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getConsolidatedCoupon()
//===========================================================================

inline const TOTAL& CTtlRegFin::getConsolidatedCoupon() const
{
    return ( m_ttlRegFin.ConsCoupon );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getItemDiscount()
//===========================================================================

inline const TOTAL& CTtlRegFin::getItemDiscount(int iDisc) const
{
	// allow us to select the item discount numbers 1 through 6.
	switch (iDisc) {
		case 2:
			return m_ttlRegFin.ModiDisc;
		case 3:
			return m_ttlRegFin.ItemDiscExtra.TtlIAmount[0];
		case 4:
			return m_ttlRegFin.ItemDiscExtra.TtlIAmount[1];
		case 5:
			return m_ttlRegFin.ItemDiscExtra.TtlIAmount[2];
		case 6:
			return m_ttlRegFin.ItemDiscExtra.TtlIAmount[3];
		case 1:
		default:
			// just return the default of item discount #1
			return m_ttlRegFin.ItemDisc;
	}
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getModifiedDiscount()
//===========================================================================

inline const TOTAL& CTtlRegFin::getModifiedDiscount() const
{
    return ( m_ttlRegFin.ModiDisc );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getRegularDiscount()
//===========================================================================

inline const TOTAL& CTtlRegFin::getRegularDiscount(
    const USHORT usDiscountNo
    ) const
{
    ASSERT(( 0 < usDiscountNo ) && ( usDiscountNo <= FSC_RDISC_MAX ));

    return ( m_ttlRegFin.RegDisc[ usDiscountNo - 1 ] );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getCombinationCoupon()
//===========================================================================

inline const TOTAL& CTtlRegFin::getCombinationCoupon() const
{
    return ( m_ttlRegFin.Coupon );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getPaidOut()
//===========================================================================

inline const TOTAL& CTtlRegFin::getPaidOut() const
{
    return ( m_ttlRegFin.PaidOut);
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getReceivedOnAccount()
//===========================================================================

inline const TOTAL& CTtlRegFin::getReceivedOnAccount() const
{
    return ( m_ttlRegFin.RecvAcount );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getTipsPaidOut()
//===========================================================================

inline const TOTAL& CTtlRegFin::getTipsPaidOut() const
{
    return ( m_ttlRegFin.TipsPaid );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getCashTenderOnline()
//===========================================================================

inline const TOTAL& CTtlRegFin::getCashTenderOnline() const
{
    return ( m_ttlRegFin.CashTender.OnlineTotal );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getCashTenderOffline()
//===========================================================================

inline const TOTAL& CTtlRegFin::getCashTenderOffline() const
{
    return ( m_ttlRegFin.CashTender.OfflineTotal );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getCheckTenderOnline()
//===========================================================================

inline const TOTAL& CTtlRegFin::getCheckTenderOnline() const
{
    return ( m_ttlRegFin.CheckTender.OnlineTotal );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getCheckTenderOffline()
//===========================================================================

inline const TOTAL& CTtlRegFin::getCheckTenderOffline() const
{
    return ( m_ttlRegFin.CheckTender.OfflineTotal );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getChargeTenderOnline()
//===========================================================================

inline const TOTAL& CTtlRegFin::getChargeTenderOnline() const
{
    return ( m_ttlRegFin.ChargeTender.OnlineTotal );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getChargeTenderOffline()
//===========================================================================

inline const TOTAL& CTtlRegFin::getChargeTenderOffline() const
{
    return ( m_ttlRegFin.ChargeTender.OfflineTotal );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getMiscTenderOnline()
//===========================================================================

inline const TOTAL& CTtlRegFin::getMiscTenderOnline(
    const USHORT usMiscNo
    ) const
{
    ASSERT(( 0 < usMiscNo ) && ( usMiscNo <= pcttl::MAX_MISC_TENDER ));

    return ( m_ttlRegFin.MiscTender[ usMiscNo - 1 ].OnlineTotal );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getMiscTenderOffline()
//===========================================================================

inline const TOTAL& CTtlRegFin::getMiscTenderOffline(
    const USHORT usMiscNo
    ) const
{
    ASSERT(( 0 < usMiscNo ) && ( usMiscNo <= pcttl::MAX_MISC_TENDER ));

    return ( m_ttlRegFin.MiscTender[ usMiscNo - 1 ].OfflineTotal );
}

inline const TEND& CTtlRegFin::getNormalizedTender(const USHORT usTenderNo) const
{
	switch (usTenderNo) {
	case 1:
		return (m_ttlRegFin.CashTender);
	case 2:
		return (m_ttlRegFin.CheckTender);
	case 3:
		return (m_ttlRegFin.ChargeTender);
	default:
		// miscellanous tenders
		if (usTenderNo >= 4 && usTenderNo <= 20) {
			return (m_ttlRegFin.MiscTender[usTenderNo - 4]);
		}
		break;
	}

	return (m_ttlRegFin.CashTender);
}
//===========================================================================
//  FUNCTION :  CTtlRegFin::getForeginTotal()
//===========================================================================

inline const TOTAL& CTtlRegFin::getForeginTotal(
    const USHORT usTotalNo
    ) const
{
    ASSERT(( 0 < usTotalNo ) && ( usTotalNo <= pcttl::MAX_FOREIGN_TOTAL ));

    return ( m_ttlRegFin.ForeignTotal[ usTotalNo - 1 ] );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getServiceTotal()
//===========================================================================

inline const TOTAL& CTtlRegFin::getServiceTotal(
    const USHORT usTotalNo
    ) const
{
    ASSERT(( 0 < usTotalNo ) && ( usTotalNo <= pcttl::MAX_SERVICE_TOTAL ));

    return ( m_ttlRegFin.ServiceTotal[ usTotalNo - 1 ] );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getAddCheckTotal()
//===========================================================================

inline const TOTAL& CTtlRegFin::getAddCheckTotal(
    const USHORT usTotalNo
    ) const
{
    ASSERT(( 0 < usTotalNo ) && ( usTotalNo <= FSC_ADDCHK_MAX ));

    return ( m_ttlRegFin.AddCheckTotal[ usTotalNo - 1 ] );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getChargeTips()
//===========================================================================

inline const TOTAL& CTtlRegFin::getChargeTips(
    const USHORT usTotalNo
    ) const
{
    ASSERT(( 0 < usTotalNo ) && ( usTotalNo <= FSC_CHGTIP_MAX ));

    return ( m_ttlRegFin.ChargeTips[ usTotalNo - 1 ] );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getDeclaredTips()
//===========================================================================

inline const TOTAL& CTtlRegFin::getDeclaredTips() const
{
    return ( m_ttlRegFin.DeclaredTips );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getTaxGross()
//===========================================================================

inline const D13DIGITS& CTtlRegFin::getTaxGross(
    const USHORT usTaxNo
    ) const
{
    ASSERT(( 0 < usTaxNo ) && ( usTaxNo <= pcttl::MAX_TAX_TYPE ));

    return ( m_ttlRegFin.Taxable[ usTaxNo - 1 ].mlTaxableAmount );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getTaxAmount()
//===========================================================================

inline DCURRENCY CTtlRegFin::getTaxAmount(
    const USHORT usTaxNo
    ) const
{
    ASSERT(( 0 < usTaxNo ) && ( usTaxNo <= pcttl::MAX_TAX_TYPE ));

    return ( m_ttlRegFin.Taxable[ usTaxNo - 1 ].lTaxAmount );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getTaxExempt()
//===========================================================================

inline DCURRENCY CTtlRegFin::getTaxExempt(
    const USHORT usTaxNo
    ) const
{
    ASSERT(( 0 < usTaxNo ) && ( usTaxNo <= pcttl::MAX_TAX_TYPE ));

    return ( m_ttlRegFin.Taxable[ usTaxNo - 1 ].lTaxExempt );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getNonTaxable()
//===========================================================================

inline const D13DIGITS& CTtlRegFin::getNonTaxable() const
{
    return ( m_ttlRegFin.NonTaxable );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getConsolidationTax()
//===========================================================================

inline DCURRENCY CTtlRegFin::getConsolidationTax() const
{
    return ( m_ttlRegFin.lConsTax );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getBonus()
//===========================================================================

inline const TOTAL& CTtlRegFin::getBonus(
    const USHORT usBonusNo
    ) const
{
    ASSERT(( 0 < usBonusNo ) && ( usBonusNo <= pcttl::MAX_BONUS_TYPE ));

    return ( m_ttlRegFin.Bonus[ usBonusNo - 1 ] );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getHashDepartment()
//===========================================================================

inline const TOTAL& CTtlRegFin::getHashDepartment() const
{
    return ( m_ttlRegFin.HashDepartment );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getTransactionCancel()
//===========================================================================

inline const TOTAL& CTtlRegFin::getTransactionCancel() const
{
    return ( m_ttlRegFin.TransCancel );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getMiscVoid()
//===========================================================================

inline const TOTAL& CTtlRegFin::getMiscVoid() const
{
    return ( m_ttlRegFin.MiscVoid );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getAudaction()
//===========================================================================

inline const TOTAL& CTtlRegFin::getAudaction() const
{
    return ( m_ttlRegFin.Audaction );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getNoSaleCount()
//===========================================================================

inline SHORT CTtlRegFin::getNoSaleCount() const
{
    return ( m_ttlRegFin.sNoSaleCount );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getItemProductivity()
//===========================================================================

inline LONG CTtlRegFin::getItemProductivity() const
{
    return ( m_ttlRegFin.lItemProductivityCount );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getNoOfPerson()
//===========================================================================

inline SHORT CTtlRegFin::getNoOfPerson() const
{
    return ( m_ttlRegFin.sNoOfPerson );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getNoOfCustomer()
//===========================================================================

inline SHORT CTtlRegFin::getNoOfCustomer() const
{
    return ( m_ttlRegFin.sCustomerCount );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getCheckOpened()
//===========================================================================

inline SHORT CTtlRegFin::getCheckOpened() const
{
    return ( m_ttlRegFin.sNoOfChkOpen );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getCheckClosed()
//===========================================================================

inline SHORT CTtlRegFin::getCheckClosed() const
{
    return ( m_ttlRegFin.sNoOfChkClose );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getLoan(), Saratoga
//===========================================================================

inline const TOTAL& CTtlRegFin::getLoan() const
{
	return ( m_ttlRegFin.Loan);
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getpickup(), Saratoga
//===========================================================================

inline const TOTAL& CTtlRegFin::getPickup() const
{
	return ( m_ttlRegFin.Pickup);
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getFSTaxExempt() ### ADD Saratoga (052900)
//===========================================================================

inline DCURRENCY CTtlRegFin::getFSTaxExempt(
    const USHORT usTaxNo
    ) const
{
    ASSERT(( 0 < usTaxNo ) && ( usTaxNo <= pcttl::MAX_TAX_TYPE ));

    return ( m_ttlRegFin.Taxable[ usTaxNo - 1 ].lFSTaxExempt );
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getFoodStampCredit(), ### ADD Saratoga (052900)
//===========================================================================

inline const TOTAL& CTtlRegFin::getFoodStampCredit() const
{
	return (m_ttlRegFin.FoodStampCredit);
}

//===========================================================================
//  FUNCTION :  CTtlRegFin::getUPCCoupon(), ### ADD Saratoga (052900)
//===========================================================================

inline const TOTAL& CTtlRegFin::getUPCCoupon(
    const USHORT usNo
    ) const
{
    ASSERT(( 0 < usNo ) && ( usNo <= pcttl::MAX_COUPON ));

    return ( m_ttlRegFin.aUPCCoupon[usNo - 1 ]);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TTLREGFIN_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

/////////////////// END OF FILE ( TtlRegFin.h ) /////////////////////////////