/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  Parameter.h
//
//  PURPOSE:    Parameter class definitions and declarations.
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

#if !defined(AFX_PARAMETER_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_PARAMETER_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
//
//            C O N S T A N T    V A L U E s
//
/////////////////////////////////////////////////////////////////////////////

namespace pcpara
{
const USHORT    MAX_STORE_NO = 9999;
const USHORT    MAX_REG_NO   = 999;

const UCHAR     TYPE_NORMAL = 0;
const UCHAR     TYPE_HASH   = 1;
const UCHAR     TYPE_MINUS  = 2;
}

/////////////////////////////////////////////////////////////////////////////
//
//              B A S E    C L A S S    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//  CParameter
//---------------------------------------------------------------------------

class CParameter
{
public:
    CParameter();
    virtual ~CParameter();

    virtual BOOL    Read() = 0;
    virtual BOOL    Write() = 0;
    virtual SHORT   GetLastError() const;

	bool    m_bDataRead;
	USHORT  m_usClassRead;

protected:
    BOOL    ReadAllPara( const UCHAR uchClass, LPVOID lpvBuffer, const USHORT usBytesRead );
	BOOL    ReadAllParaByBlocks(const UCHAR uchClass, LPVOID lpvBuffer, const USHORT usBytesRead);
    BOOL    WriteAllPara( const UCHAR uchClass, LPCVOID lpvBuffer, const USHORT usBytesWrite );
	BOOL    WriteAllParaByBlocks(const UCHAR uchClass, LPVOID lpvBuffer, const USHORT usBytesWrite);

private:
    SHORT   m_sLastError;
};

//---------------------------------------------------------------------------
//  CParaFlexMem
//---------------------------------------------------------------------------

class CParaFlexMem: public CParameter
{
public:
    CParaFlexMem();
    ~CParaFlexMem();

    BOOL    Read();
    BOOL    Write();

//    USHORT  getMemorySize( const USHORT usAddress ) const;  /* Saratoga */
    ULONG  getMemorySize( const USHORT usAddress ) const;

private:
    FLEXMEM m_paraFlexMem[ FLEX_ADR_MAX ];    
};

class CParaMdcMem : public CParameter
{
public:
	CParaMdcMem();
	~CParaMdcMem();

	BOOL    Read();
	BOOL    Write();

	// Same bit pattern as ODD_MDC_BIT3, ODD_MDC_BIT2, ODD_MDC_BIT1, ODD_MDC_BIT0.
	// Need to right shift m_ParaMdc[] to use EVEN_MDC_BIT3, EVEN_MDC_BIT2, EVEN_MDC_BIT1, EVEN_MDC_BIT0
	enum MdcBitFlags { MdcBitA = 0x08, MdcBitB = 0x04, MdcBitC = 0x02, MdcBitD = 0x01};

	BOOL    CheckMdcBit(USHORT usAddrs, MdcBitFlags bit) const;

private:
	UCHAR	m_ParaMDC[MAX_MDC_SIZE];				/* prog #1, beginning of the saved area of memory resident database */
};

class CParaHourlyTime : public CParameter
{
public:
	CParaHourlyTime();
	~CParaHourlyTime();

	BOOL    Read();
	BOOL    Write();

	struct HourlyTime {
		UCHAR  uchHour;
		UCHAR  uchMin;
	};

	BOOL    GetBlockTime(USHORT usAddrs, CParaHourlyTime::HourlyTime & blocktime) const;

private:
	UCHAR   m_ParaHourlyTime[MAX_HOUR_NO][MAX_HM_DATA];     /* prog #17, number of blocks in [0], block interval data in the rest, HOUR_TSEG1_ADR*/

public:
	const UCHAR &m_NoBlocks = m_ParaHourlyTime[0][0];       // provide access to the number of activity blocks. HOUR_ACTBLK_ADR
};

class CParaRound : public CParameter
{
public:
	CParaRound();
	~CParaRound();

	BOOL    Read();
	BOOL    Write();

	struct  RoundParms {
		UCHAR  uchRoundDelimit;       /* Delimit */
		UCHAR  uchRoundModules;       /* Modulus */
		CHAR   chRoundPosition;       /* Position */
	};

	SHORT  RflRoundDcurrency(DCURRENCY *plModAmount, D13DIGITS d13Amount, USHORT  usAddrs);
	SHORT  RflRoundDcurrency(DCURRENCY *plModAmount, D13DIGITS d13Amount, RoundParms &roundParms);
	SHORT  RflGetRoundParms (USHORT usAddrs, RoundParms &roundParms);
	VOID   RflGetRoundParmsTable(RoundParms &roundParms, UCHAR uchType);

private:
	UCHAR   m_ParaRound[MAX_RNDTBL_SIZE][MAX_RND_DATA];   /* A/C #84 */
};

class CParaTotalType : public CParameter
{
public:
	CParaTotalType();
	~CParaTotalType();

	BOOL    Read();
	BOOL    Write();

	enum {
		PRT_NOFINALIZE = 0x0001,      /* no finalaize total */
		PRT_SERVICE1 =   0x0002,      /* service total without stub */
		PRT_SERVICE2 =   0x0003,      /* service total with stub printed */
		PRT_FINALIZE1 =  0x0004,      /* finalize total without stub */
		PRT_FINALIZE2 =  0x0005,      /* finalize total with stub printed */
		PRT_TRAY1 =      0x0006,      /* tray total witout stub */
		PRT_TRAY2 =      0x0007,      /* tray total with stub printed */
		PRT_CASINT1 =    0x0008,      /* cashier interrupt total without stub (similar to PRT_SERVICE1 in most cases), R3.3 */
		PRT_CASINT2 =    0x0009      /* cashier interrupt total with stub (similar to PRT_SERVICE2 in most cases), R3.3 */
	};

	UCHAR  getTtlKeyCtl(USHORT usTtlKeyAddrs, USHORT usTtlKeyField);
	UCHAR  getTtlKeyType(USHORT usTtlKeyAddrs);
	BOOL   updateServiceTotal(USHORT usTotalNo, const TOTAL &ttlKeyTotal, TOTAL srvcTotal[3]);

private:
	UCHAR   m_ParaTtlKeyTyp[MAX_KEYTYPE_SIZE];        /* prog #60 */
	UCHAR   m_ParaTtlKeyCtl[MAX_TTLKEY_NO][MAX_TTLKEY_DATA];  /* prog #61 */
};

class CParaDiscountRate : public CParameter
{
public:
	CParaDiscountRate();
	~CParaDiscountRate();

	BOOL    Read();
	BOOL    Write();

	UCHAR    getRate(USHORT usAddrs) const;

private:
	UCHAR	m_ParaDisc[MAX_DISCRATE_SIZE];				/* storage area for A/C #86 discount rate data. */
};

class CParaCurrencyRate : public CParameter
{
public:
	CParaCurrencyRate();
	~CParaCurrencyRate();

	BOOL    Read();
	BOOL    Write();

	ULONG   getRate(USHORT usAddrs) const;

private:
	ULONG   m_ParaCurrency[MAX_FC_SIZE];				/* storage area for A/C #86 discount rate data. */
};

//---------------------------------------------------------------------------
//  CParaStoreNo
//---------------------------------------------------------------------------

class CParaStoreNo : public CParameter
{
public:
    CParaStoreNo();
    ~CParaStoreNo();

    BOOL    Read();
    BOOL    Write();

    USHORT  getStoreNo() const;
    USHORT  getRegNo() const;
    VOID    setStoreNo( const USHORT usStoreNo );
    VOID    setRegNo( const USHORT usRegNo );

private:
    USHORT  m_ausStoreRegNo[ MAX_STOREG_SIZE ];
};

//---------------------------------------------------------------------------
//  CParaPlu
//---------------------------------------------------------------------------

class CParaPlu
{
public:
    CParaPlu();
    ~CParaPlu();

    VOID    Initialize();
// ###DEL Saratoga
//    BOOL    Read( const USHORT usPluNo,
//                  const UCHAR uchAdjectiveNo );

	BOOL    Read( const TCHAR uchPluNo,
                  const UCHAR uchAdjectiveNo );
    BOOL    ReadPlural( USHORT& usNoOfRead );
// ###DEL Saratoga
//    BOOL    ReadPluralByDept( const UCHAR uchDeptNo,
//                              USHORT& usNoOfRead );
    BOOL    ReadPluralByDept( const USHORT uchDeptNo,
                              USHORT& usNoOfRead );
    SHORT   GetLastError() const;

//    USHORT  getPluNo( const USHORT usIndex = 0 ) const;      /* Saratoga */
    LPTSTR  getPluNo( LPTSTR lpszPluNo, const USHORT usIndex = 0 ) const;
    UCHAR   getAdjectiveNo( const USHORT usIndex = 0 ) const;
    UCHAR   getPluType( const USHORT usIndex = 0 ) const;
    LPTSTR  getName( LPTSTR lpszPluName, const USHORT usIndex = 0 ) const;
//    UCHAR   getDeptNo( const USHORT usIndex = 0 ) const;     /* Saratoga */
    USHORT   getDeptNo( const USHORT usIndex = 0 ) const;
    static void    convertPLU(TCHAR *pszDestPLU, const TCHAR *pszSourcePLU);

private:
    SHORT   m_sLastError;
    ULONG   m_ulPluralCounter;
	USHORT  m_usPluralAdjCo;	// ADD Saratoga
	USHORT  m_husPluralHandle;	// ADD Saratoga
    PLUIF   m_paraPlu[ CLI_PLU_MAX_NO ];
    PLUIF_DEP   m_paraPluByDept[ CLI_PLU_MAX_NO ];
};

//---------------------------------------------------------------------------
//  CParaDept
//---------------------------------------------------------------------------

class CParaDept
{
public:
    CParaDept();
    ~CParaDept();

    VOID    Initialize();
    BOOL    Read( const UCHAR uchMajorDeptNo );

//    UCHAR   getDeptNo() const;   /* Saratoga */
    USHORT   getDeptNo() const;
    UCHAR   getDeptType() const;
    UCHAR   getMajorDeptNo() const;
    LPTSTR  getName( LPTSTR lpszDeptName ) const; //was LPTSTR

    SHORT   GetLastError() const;

private:
    SHORT   m_sLastError;
    MDEPTIF m_paraDept;
};

//---------------------------------------------------------------------------
//  CParaCoupon
//---------------------------------------------------------------------------

class CParaCoupon
{
public:
    CParaCoupon();
    ~CParaCoupon();
    
    BOOL    Read( const USHORT usCouponNo );

    SHORT   GetLastError() const;

    TCHAR   getCouponNo() const;
    LPCTSTR  getName( LPTSTR lpszCouponName = 0 ) const; //was LPTSTR

private:
    SHORT   m_sLastError;
    CPNIF   m_paraCoupon;
};

//---------------------------------------------------------------------------
//  CParaCashier
//---------------------------------------------------------------------------

class CParaCashier
{
public:
    CParaCashier();
    ~CParaCashier();

    BOOL    Read( const ULONG ulCashierNo );
    BOOL    GetAllCashierNo( ULONG * pausCashierNo, const USHORT usBufferSize, USHORT& usNoOfPerson );
    BOOL    ClearSecretCode( const ULONG ulCashierNo );
    BOOL    IsSignIn( const ULONG ulCashierNo );
    BOOL    SignOutAllTerminal( const ULONG ulCashierNo ); 

    SHORT   GetLastError() const;

    ULONG   getCashierNo() const;
    ULONG   getSercretNo() const;
    LPTSTR  getName( LPTSTR lpszCashierName ) const; //was LPTSTR

private:
    SHORT   m_sLastError;
    CASIF   m_paraCashier;
};

//---------------------------------------------------------------------------
//  CParaETK
//---------------------------------------------------------------------------

class CParaETK
{
public:
    CParaETK();
    ~CParaETK();

    BOOL    Read( const ULONG ulETKNo );

    SHORT   GetLastError() const;

    LPWSTR  getName( LPWSTR lpszETKName ) const; //was LPTSTR

private:
    SHORT   m_sLastError;
    ETK_JOB m_jobETK;
    WCHAR   m_auchETKName[ ETK_MAX_NAME_SIZE + 1 ];
};

class CParaMnemonicTransaction : public CParameter
{
public:
	CParaMnemonicTransaction();
	~CParaMnemonicTransaction();

	BOOL    Read();
	BOOL    Write();
	LPTSTR  getMnemonic(UINT uiAddress); //was LPTSTR

	WCHAR  m_lastMnemonic[PARA_TRANSMNEMO_LEN + 1];

private:
	WCHAR   m_ParaTransMnemo[MAX_TRANSM_NO][PARA_TRANSMNEMO_LEN];   /* prog #20 */
};

/////////////////////////////////////////////////////////////////////////////
//
//                  I N L I N E    F U N C T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//  CParameter
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CParameter::GetLastError()
//===========================================================================

inline SHORT CParameter::GetLastError() const
{
    return ( m_sLastError );
}

//---------------------------------------------------------------------------
//
//  CParaFlexMem
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CParaFlexMem::GetLastError()
//===========================================================================

//inline USHORT CParaFlexMem::getMemorySize(                    /* Saratoga */
inline ULONG CParaFlexMem::getMemorySize(
    const USHORT usAddress
    ) const
{
    ASSERT(( 0 < usAddress ) && ( usAddress <= FLEX_ADR_MAX ));
//    return ( m_paraFlexMem[ usAddress - 1 ].usRecordNumber ); /* Saratoga */
    return ( m_paraFlexMem[ usAddress - 1 ].ulRecordNumber );
}

//---------------------------------------------------------------------------
//
//  CParaStoreNo
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CParaStoreNo::getStoreNo()
//===========================================================================

inline USHORT CParaStoreNo::getStoreNo() const
{
    return ( m_ausStoreRegNo[ 0 ] );
}

//===========================================================================
//  FUNCTION :  CParaStoreNo::getRegNo()
//===========================================================================

inline USHORT CParaStoreNo::getRegNo() const
{
    return ( m_ausStoreRegNo[ 1 ] );
}

//===========================================================================
//  FUNCTION :  CParaStoreNo::setStoreNo()
//===========================================================================

inline VOID CParaStoreNo::setStoreNo( const USHORT usStoreNo )
{
    ASSERT( usStoreNo <= pcpara::MAX_STORE_NO );
    m_ausStoreRegNo[ 0 ] = usStoreNo;
}

//===========================================================================
//  FUNCTION :  CParaStoreNo::setRegNo()
//===========================================================================

inline VOID CParaStoreNo::setRegNo( const USHORT usRegNo)
{
    ASSERT( usRegNo <= pcpara::MAX_REG_NO );
    m_ausStoreRegNo[ 1 ] = usRegNo;
}

//---------------------------------------------------------------------------
//
//  CParaPlu
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CParaPlu::GetLastError()
//===========================================================================

inline SHORT CParaPlu::GetLastError() const
{
    return ( m_sLastError );
}

//===========================================================================
//  FUNCTION :  CParaPlu::getPluNo()
//===========================================================================

//inline USHORT CParaPlu::getPluNo( const USHORT usIndex ) const /* Saratoga */
inline LPTSTR CParaPlu::getPluNo( LPTSTR lpszPluNo, const USHORT usIndex ) const
{
    ASSERT( usIndex < CLI_PLU_MAX_NO );
	/* Saratoga */
	_tcsncpy(lpszPluNo, m_paraPlu[ usIndex ].auchPluNo,
		              OP_PLU_LEN);//sizeof(m_paraPlu[ usIndex ].auchPluNo));


//    return ( m_paraPlu[ usIndex ].usPluNo );                   /* Saratoga */
    return ( (LPTSTR)m_paraPlu[ usIndex ].auchPluNo );                   /* Saratoga */
}

//===========================================================================
//  FUNCTION :  CParaPlu::getAdjectiveNo()
//===========================================================================

inline UCHAR CParaPlu::getAdjectiveNo( const USHORT usIndex ) const
{
    ASSERT( usIndex < CLI_PLU_MAX_NO );
    return ( m_paraPlu[ usIndex ].uchPluAdj );
}

//===========================================================================
//  FUNCTION :  CParaPlu::getPluType()
//===========================================================================

inline UCHAR CParaPlu::getPluType( const USHORT usIndex ) const
{
    ASSERT( usIndex < CLI_PLU_MAX_NO );

    UCHAR uchTypeOfPlu;

//    uchTypeOfPlu = m_paraPlu[ usIndex ].ParaPlu.ContPlu.uchDept; /* Saratoga */
//    uchTypeOfPlu &= PLU_TYPE_MASK;
    uchTypeOfPlu = m_paraPlu[ usIndex ].ParaPlu.ContPlu.uchItemType;
//    uchTypeOfPlu &= PLU_TYPE_MASK;

    return ( uchTypeOfPlu );
}

//===========================================================================
//  FUNCTION :  CParaPlu::getDeptNo()
//===========================================================================

//inline UCHAR CParaPlu::getDeptNo( const USHORT usIndex ) const   /* Saratoga */
inline USHORT CParaPlu::getDeptNo( const USHORT usIndex ) const
{
    ASSERT( usIndex < CLI_PLU_MAX_NO );

//    UCHAR uchDeptNo;                                            /* Saratoga */
    USHORT usDeptNo;

//    uchDeptNo = m_paraPlu[ usIndex ].ParaPlu.ContPlu.uchDept;   /* Saratoga */
//    uchDeptNo &= PLU_DEPTNO_MASK;
    usDeptNo = m_paraPlu[ usIndex ].ParaPlu.ContPlu.usDept;
    usDeptNo &= PLU_DEPTNO_MASK;

//    return ( uchDeptNo );                                       /* Saratoga */
    return ( usDeptNo );
}

//---------------------------------------------------------------------------
//
//  CParaDept
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CParaDept::GetLastError()
//===========================================================================

inline SHORT CParaDept::GetLastError() const
{
    return ( m_sLastError );
}

//===========================================================================
//  FUNCTION :  CParaDept::getDeptNo()
//===========================================================================

//inline UCHAR CParaDept::getDeptNo() const /* Saratoga */
inline USHORT CParaDept::getDeptNo() const
{
//    return ( m_paraDept.uchDeptNo );      /* Saratoga */
    return ( m_paraDept.usDeptNo );
}

//===========================================================================
//  FUNCTION :  CParaDept::getDeptType()
//===========================================================================

inline UCHAR CParaDept::getDeptType() const
{
    UCHAR uchType;

    if ( m_paraDept.ParaDept.auchControlCode[ 2 ] & PLU_HASH )
    {
        uchType = pcpara::TYPE_HASH;
    }
    else if ( m_paraDept.ParaDept.auchControlCode[ 0 ] & PLU_MINUS )
    {
        uchType = pcpara::TYPE_MINUS;
    }
    else
    {
        uchType = pcpara::TYPE_NORMAL;
    }
    return ( uchType );
}

//===========================================================================
//  FUNCTION :  CParaDept::getMajorDeptNo()
//===========================================================================

inline UCHAR CParaDept::getMajorDeptNo() const
{
    return ( m_paraDept.uchMajorDeptNo );
}

//---------------------------------------------------------------------------
//
//  CParaCoupon
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CParaCoupon::GetLastError()
//===========================================================================

inline SHORT CParaCoupon::GetLastError() const
{
    return ( m_sLastError );
}

//===========================================================================
//  FUNCTION :  CParaCoupon::getCouponNo()
//===========================================================================

inline TCHAR CParaCoupon::getCouponNo() const
{
    return ( m_paraCoupon.uchCpnNo );
}

//---------------------------------------------------------------------------
//
//  CParaCashier
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CParaCashier::GetLastError()
//===========================================================================

inline SHORT CParaCashier::GetLastError() const
{
    return ( m_sLastError );
}

//===========================================================================
//  FUNCTION :  CParaCashier::getCashierNo()
//===========================================================================

inline ULONG CParaCashier::getCashierNo() const
{
    return ( m_paraCashier.ulCashierNo );
}

//===========================================================================
//  FUNCTION :  CParaCashier::getSecretNo()
//===========================================================================

inline ULONG CParaCashier::getSercretNo() const
{
    return ( m_paraCashier.ulCashierSecret );
}

//---------------------------------------------------------------------------
//
//  CParaETK
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CParaETK::GetLastError()
//===========================================================================

inline SHORT CParaETK::GetLastError() const
{
    return ( m_sLastError );
}

//===========================================================================
//
//  FUNCTION :  CParaPlu::convertPLU()
//
//  PURPOSE :   Constructor of Plu Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================



//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AFX_PARAMETER__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

////////////////////// END OF FILE ( Parameter.h ) //////////////////////////