/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  ParaFlexMem.cpp
//
//  PURPOSE:    Flexible Memory Parameter class definitions and declarations.
//
//  AUTHOR:
//
//      Ver 1.00.00 : 1998-09-24 : Initial Release
//
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//          I N C L U D E    F I L E s
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include "PC2170.h"
#endif
#include "R20_PC2172.h"

#include "Parameter.h"
#include "Global.h"

/////////////////////////////////////////////////////////////////////////////
//
//          D E B U G    D E F I N I T I O N s
//
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
//      C L A S S    F U N C T I O N    I M P L E M E N T A T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//
//  FUNCTION :  CParaFlexMem::CParaFlexMem()
//
//  PURPOSE :   Constructor of Flexible Memory Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaFlexMem::CParaFlexMem()
{
	memset( m_paraFlexMem, 0, sizeof( m_paraFlexMem ));
    //::ZeroMemory( m_paraFlexMem, sizeof( m_paraFlexMem ));
}

//===========================================================================
//
//  FUNCTION :  CParaFlexMem::~CParaFlexMem()
//
//  PURPOSE :   Destructor of Flexible Memory Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaFlexMem::~CParaFlexMem()
{
}

//===========================================================================
//
//  FUNCTION :  CParaFlexMem::Read()
//
//  PURPOSE :   Read the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaFlexMem::Read()
{
    pcsample::TraceFunction( _T("BEGIN >> CParaFlexMem::Read()") );

    BOOL    fSuccess;
    fSuccess = CParameter::ReadAllPara( CLASS_PARAFLEXMEM,
                                        m_paraFlexMem,
                                        sizeof( m_paraFlexMem ));

    pcsample::TraceFunction( _T("END   >> CParaFlexMem::Read() = %d"), fSuccess );

    return ( fSuccess );
}

//===========================================================================
//
//  FUNCTION :  CParaFlexMem::Write()
//
//  PURPOSE :   Write the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaFlexMem::Write()
{
    pcsample::TraceFunction( _T("BEGIN >> CParaFlexMem::Write()") );

    BOOL    fSuccess;
    fSuccess =  CParameter::WriteAllPara( CLASS_PARAFLEXMEM,
                                          m_paraFlexMem,
                                          sizeof( m_paraFlexMem ));

    pcsample::TraceFunction( _T("END   >> CParaFlexMem::Write() = %d"), fSuccess );

    return ( fSuccess );
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------


//===========================================================================
//
//  FUNCTION :  CParaMdcMem::CParaMdcMem()
//
//  PURPOSE :   Constructor of MDC Memory Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaMdcMem::CParaMdcMem()
{
	memset(m_ParaMDC, 0, sizeof(m_ParaMDC));
}

//===========================================================================
//
//  FUNCTION :  CParaMdcMem::~CParaMdcMem()
//
//  PURPOSE :   Destructor of MDC Memory Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaMdcMem::~CParaMdcMem()
{
}

//===========================================================================
//
//  FUNCTION :  CParaMdcMem::Read()
//
//  PURPOSE :   Read the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaMdcMem::Read()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaMdcMem::Read()"));

	BOOL    fSuccess;

	fSuccess = CParameter::ReadAllPara(CLASS_PARAMDC, m_ParaMDC, sizeof(m_ParaMDC));

	pcsample::TraceFunction(_T("END   >> CParaMdcMem::Read() = %d"), fSuccess);

	return (fSuccess);
}

//===========================================================================
//
//  FUNCTION :  CParaMdcMem::Write()
//
//  PURPOSE :   Write the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaMdcMem::Write()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaMdcMem::Write()"));

	BOOL    fSuccess;

	fSuccess = CParameter::WriteAllPara(CLASS_PARAMDC, m_ParaMDC, sizeof(m_ParaMDC));

	pcsample::TraceFunction(_T("END   >> CParaMdcMem::Write() = %d"), fSuccess);

	return (fSuccess);
}

BOOL    CParaMdcMem::CheckMdcBit (USHORT usAddrs, CParaMdcMem::MdcBitFlags bit)  const
{
	BOOL    bRet = FALSE;

	if (usAddrs > 0) {
		USHORT  usIsOdd = usAddrs & 0x01;    // determine if odd or even address.

		/* calculate for fitting program address to ram address */
		usAddrs = ((usAddrs - 1) / 2);

		if (usIsOdd) {
			bRet = (m_ParaMDC[usAddrs] & bit);         // odd address so just use the mask as specified  ODD_MDC_BIT0
		}
		else {
			bRet = ((m_ParaMDC[usAddrs] >> 4) & bit);  // even address so shift then use the mask as specified  EVEN_MDC_BIT0
		}
	}

	return bRet;
}


//===========================================================================
//
//  FUNCTION :  CParaHourlyTime::CParaHourlyTime()
//
//  PURPOSE :   Constructor of MDC Memory Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaHourlyTime::CParaHourlyTime()
{
	memset(m_ParaHourlyTime, 0, sizeof(m_ParaHourlyTime));
}

//===========================================================================
//
//  FUNCTION :  CParaHourlyTime::~CParaHourlyTime()
//
//  PURPOSE :   Destructor of MDC Memory Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaHourlyTime::~CParaHourlyTime()
{
}

//===========================================================================
//
//  FUNCTION :  CParaHourlyTime::Read()
//
//  PURPOSE :   Read the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaHourlyTime::Read()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaHourlyTime::Read()"));

	BOOL    fSuccess;

	fSuccess = CParameter::ReadAllPara(CLASS_PARAHOURLYTIME, m_ParaHourlyTime, sizeof(m_ParaHourlyTime));

	pcsample::TraceFunction(_T("END   >> CParaHourlyTime::Read() = %d"), fSuccess);

	return (fSuccess);
}

//===========================================================================
//
//  FUNCTION :  CParaHourlyTime::Write()
//
//  PURPOSE :   Write the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaHourlyTime::Write()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaHourlyTime::Write()"));

	BOOL    fSuccess;

	fSuccess = CParameter::WriteAllPara(CLASS_PARAHOURLYTIME, m_ParaHourlyTime, sizeof(m_ParaHourlyTime));

	pcsample::TraceFunction(_T("END   >> CParaHourlyTime::Write() = %d"), fSuccess);

	return (fSuccess);
}

BOOL    CParaHourlyTime::GetBlockTime(USHORT usAddrs, CParaHourlyTime::HourlyTime & blocktime)  const
{
	BOOL    bRet = FALSE;

	blocktime.uchHour = 0;
	blocktime.uchMin = 0;
	if (usAddrs > 0) {
		// The first block at offset zero contains the number of activity blocks.
		// We need to start at HOUR_TSEG1_ADR which is the offset to the first block time.
		if (usAddrs < MAX_HOUR_NO) {
			blocktime.uchHour = m_ParaHourlyTime[usAddrs][0];  /* 0 = Major FSC RAM Address */
			blocktime.uchMin = m_ParaHourlyTime[usAddrs][1];   /* 1 = Minor FSC RAM Address */
		}
		else {
			blocktime.uchHour = 23;  /* max hour is 11pm */
			blocktime.uchMin = 59;   /* max minute is 59 */
		}
		bRet = TRUE;
	}

	return bRet;
}


//===========================================================================
//
//  FUNCTION :  CParaRound::CParaRound()
//
//  PURPOSE :   Constructor of Rounding Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaRound::CParaRound()
{
	memset(m_ParaRound, 0, sizeof(m_ParaRound));
}

//===========================================================================
//
//  FUNCTION :  CParaRound::~CParaRound()
//
//  PURPOSE :   Destructor of Rounding Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaRound::~CParaRound()
{
}

//===========================================================================
//
//  FUNCTION :  CParaRound::Read()
//
//  PURPOSE :   Read the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaRound::Read()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaRound::Read()"));

	BOOL    fSuccess;

	fSuccess = CParameter::ReadAllPara(CLASS_PARAROUNDTBL, m_ParaRound, sizeof(m_ParaRound));

	pcsample::TraceFunction(_T("END   >> CParaRound::Read() = %d"), fSuccess);

	return (fSuccess);
}

//===========================================================================
//
//  FUNCTION :  CParaRound::Write()
//
//  PURPOSE :   Write the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaRound::Write()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaRound::Write()"));

	BOOL    fSuccess;

	fSuccess = CParameter::WriteAllPara(CLASS_PARAROUNDTBL, m_ParaRound, sizeof(m_ParaRound));

	pcsample::TraceFunction(_T("END   >> CParaRound::Write() = %d"), fSuccess);

	return (fSuccess);
}

SHORT CParaRound::RflGetRoundParms(USHORT usAddrs, CParaRound::RoundParms &roundParms)
{
	if (usAddrs > 0) {
		usAddrs = ((usAddrs - 1) / 2);
		roundParms.uchRoundDelimit = m_ParaRound[usAddrs][0];
		roundParms.uchRoundModules = m_ParaRound[usAddrs][1];
		roundParms.chRoundPosition = m_ParaRound[usAddrs][2];
	}
	else {
		roundParms.uchRoundDelimit = 0;
		roundParms.uchRoundModules = 0;
		roundParms.chRoundPosition = 0;
	}
	return 0;
}

VOID CParaRound::RflGetRoundParmsTable(RoundParms &roundParms, UCHAR uchType)
{
	if (uchType / 100) {
		roundParms.chRoundPosition = -1;
		switch (uchType) {
			case RND_RND_UP:
				roundParms.uchRoundModules = 10;
				roundParms.uchRoundDelimit = 0;
				break;

			case RND_RND_DOWN:
				roundParms.uchRoundModules = 10;
				roundParms.uchRoundDelimit = 9;
				break;

			case RND_RND_NORMAL:
				roundParms.uchRoundModules = 10;
				roundParms.uchRoundDelimit = 5;
				break;
		}
	}
	else {
		RflGetRoundParms(uchType, roundParms);
	}


	/* -- case of parameter not set -- */
	if (roundParms.uchRoundModules == 0 && roundParms.chRoundPosition == 0
		&& roundParms.uchRoundDelimit == 0) {

		roundParms.uchRoundModules = 10;       /* set normal rounding */
		roundParms.uchRoundDelimit = 5;
		roundParms.chRoundPosition = -1;
	}
}

SHORT  CParaRound::RflRoundDcurrency(DCURRENCY *plModAmount, D13DIGITS d13Amount, CParaRound::RoundParms &roundParms)
{
	LONG         lModulus, lDelimit;

	if (roundParms.uchRoundModules == 0        /* check 0 divide */
		|| roundParms.chRoundPosition == 0     /* round positon effective? */
		|| roundParms.chRoundPosition < -1
		|| 3 < roundParms.chRoundPosition) {

		d13Amount /= 10L;                      /* delete under decimal point */
		*plModAmount = (DCURRENCY)d13Amount;
		return (-1);   // RFL_FAIL
	}

	lModulus = roundParms.uchRoundModules;     /* adjust modules, delimiter */
	lDelimit = roundParms.uchRoundDelimit;

	if (roundParms.chRoundPosition > 0) {
		SHORT        i;

		/* '1' = digits of under decimalpoints */
		for (i = 1; i < roundParms.chRoundPosition + 1; i++) {
			lModulus = 10 * lModulus;
			lDelimit = 10 * lDelimit;
		}
	}

	if (d13Amount < 0) {
		lDelimit = -1L * lDelimit;                  /* if minus value */
	}


	d13Amount += lDelimit;      /* Add delimiter to amount         */
	d13Amount /= lModulus;      /* divide by modulus */
	d13Amount *= lModulus;      /* multiply by modulus */
	d13Amount /= 10L;           /* 10L for 1digits of under decimal point */
	*plModAmount = (DCURRENCY)d13Amount;

	return(0);    // RFL_SUCCESS
}

SHORT  CParaRound::RflRoundDcurrency(DCURRENCY *plModAmount, D13DIGITS d13Amount, USHORT usAddrs)
{
	CParaRound::RoundParms roundParms;

	RflGetRoundParms(usAddrs, roundParms);

	return RflRoundDcurrency(plModAmount, d13Amount, roundParms);
}


//===========================================================================
//
//  FUNCTION :  CParaTotalType::CParaTotalType()
//
//  PURPOSE :   Constructor of Discount Rate Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaTotalType::CParaTotalType()
{
	memset(m_ParaTtlKeyTyp, 0, sizeof(m_ParaTtlKeyTyp));
	memset(m_ParaTtlKeyCtl, 0, sizeof(m_ParaTtlKeyCtl));
}

//===========================================================================
//
//  FUNCTION :  CParaTotalType::~CParaTotalType()
//
//  PURPOSE :   Destructor of Discount Rate Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaTotalType::~CParaTotalType()
{
}

//===========================================================================
//
//  FUNCTION :  CParaTotalType::Read()
//
//  PURPOSE :   Read the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaTotalType::Read()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaTotalType::Read()"));

	BOOL    fSuccess;

	fSuccess = CParameter::ReadAllPara(CLASS_PARATTLKEYTYP, m_ParaTtlKeyTyp, sizeof(m_ParaTtlKeyTyp));
	fSuccess = CParameter::ReadAllPara(CLASS_PARATTLKEYCTL, m_ParaTtlKeyCtl, sizeof(m_ParaTtlKeyCtl));

	pcsample::TraceFunction(_T("END   >> CParaTotalType::Read() = %d"), fSuccess);

	return (fSuccess);
}

//===========================================================================
//
//  FUNCTION :  CParaTotalType::Write()
//
//  PURPOSE :   Write the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaTotalType::Write()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaTotalType::Write()"));

	BOOL    fSuccess;

	fSuccess = CParameter::WriteAllPara(CLASS_PARATTLKEYTYP, m_ParaTtlKeyTyp, sizeof(m_ParaTtlKeyTyp));
	fSuccess = CParameter::WriteAllPara(CLASS_PARATTLKEYCTL, m_ParaTtlKeyCtl, sizeof(m_ParaTtlKeyCtl));

	pcsample::TraceFunction(_T("END   >> CParaTotalType::Write() = %d"), fSuccess);

	return (fSuccess);
}

UCHAR  CParaTotalType::getTtlKeyCtl(USHORT usTtlKeyAddrs, USHORT usTtlKeyField)
{
	UCHAR  uchRet = 0;

	// usTtlKeyAddrs is a value from 1 (total key #1) to 20 (total key #20).
	if (usTtlKeyAddrs > 0 && usTtlKeyField > 0) {
		usTtlKeyAddrs--; usTtlKeyField--;
		uchRet = m_ParaTtlKeyCtl[usTtlKeyAddrs][usTtlKeyField];
	}

	return uchRet;
}

// uchNo is zero based offset from Total #3 (uchNo value of zero means Total #3).
// uchNo value    0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16
// total key      3   4   5   6   7   8  10  11  12  13  14  15  16  17  18  19  20
// uchNo is used to address Total Key #3 through Total Key #8 and then Total Key #10
// through Total Key #20. Total Key #9 is skipped.
UCHAR  CParaTotalType::getTtlKeyType(USHORT usTtlKeyAddrs)
{
	UCHAR  uchRet = 0;

	// usTtlKeyAddrs is a value from 1 (total key #1) to 20 (total key #20).
	// we do not have data for Total Key #1, Total Key #2, or Total Key #9 as these
	// are all special, dedicated total keys whose provisioning does not change.
	if (usTtlKeyAddrs > 2 && usTtlKeyAddrs != 9) {
		usTtlKeyAddrs -= 3;                       // skip over total key #1 and #2.
		if (usTtlKeyAddrs > 5) usTtlKeyAddrs--;   // skip over total key #9.
		uchRet = m_ParaTtlKeyTyp[usTtlKeyAddrs];
	}

	return uchRet;
}

BOOL   CParaTotalType::updateServiceTotal(USHORT usTotalNo, const TOTAL &ttlKeyTotal, TOTAL srvcTotal[3])
{
	// usTotalNo is a value from 0 to 17 which is the total keys from #3 through #20 excluding #9.
	usTotalNo += 2;
	if (usTotalNo > 8) usTotalNo++;

	BOOL  bSrvc = FALSE;
	UCHAR  uchType = getTtlKeyType(usTotalNo);
	if (uchType && ((uchType / 21) == PRT_SERVICE1) || (uchType / 21) == PRT_SERVICE2) {
		UCHAR  uchFunc = getTtlKeyCtl(usTotalNo, 7);
		switch (uchFunc & 0x0006) {
		case 4:
			srvcTotal[2].lAmount += ttlKeyTotal.lAmount;
			srvcTotal[2].sCounter += ttlKeyTotal.sCounter;
			bSrvc = TRUE;
			break;
		case 2:
			srvcTotal[1].lAmount += ttlKeyTotal.lAmount;
			srvcTotal[1].sCounter += ttlKeyTotal.sCounter;
			bSrvc = TRUE;
			break;
		case 0:
			srvcTotal[0].lAmount += ttlKeyTotal.lAmount;
			srvcTotal[0].sCounter += ttlKeyTotal.sCounter;
			bSrvc = TRUE;
			break;
		}
	}

	return bSrvc;
}


//===========================================================================
//
//  FUNCTION :  CParaDiscountRate::CParaDiscountRate()
//
//  PURPOSE :   Constructor of Discount Rate Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaDiscountRate::CParaDiscountRate()
{
	memset(m_ParaDisc, 0, sizeof(m_ParaDisc));
}

//===========================================================================
//
//  FUNCTION :  CParaDiscountRate::~CParaDiscountRate()
//
//  PURPOSE :   Destructor of Discount Rate Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaDiscountRate::~CParaDiscountRate()
{
}

//===========================================================================
//
//  FUNCTION :  CParaDiscountRate::Read()
//
//  PURPOSE :   Read the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaDiscountRate::Read()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaDiscountRate::Read()"));

	BOOL    fSuccess;

	fSuccess = CParameter::ReadAllPara(CLASS_PARADISCTBL, m_ParaDisc, sizeof(m_ParaDisc));

	pcsample::TraceFunction(_T("END   >> CParaDiscountRate::Read() = %d"), fSuccess);

	return (fSuccess);
}

//===========================================================================
//
//  FUNCTION :  CParaDiscountRate::Write()
//
//  PURPOSE :   Write the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaDiscountRate::Write()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaDiscountRate::Write()"));

	BOOL    fSuccess;

	fSuccess = CParameter::WriteAllPara(CLASS_PARADISCTBL, m_ParaDisc, sizeof(m_ParaDisc));

	pcsample::TraceFunction(_T("END   >> CParaDiscountRate::Write() = %d"), fSuccess);

	return (fSuccess);
}

UCHAR    CParaDiscountRate::getRate(USHORT usAddrs)  const
{
	UCHAR    ucRet = 0;

	if (usAddrs > 0) {
		usAddrs--;

		ucRet = m_ParaDisc[usAddrs];
	}

	return ucRet;
}


//===========================================================================
//
//  FUNCTION :  CParaCurrencyRate::CParaCurrencyRate()
//
//  PURPOSE :   Constructor of Currency Conversion Rate Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaCurrencyRate::CParaCurrencyRate()
{
	memset(m_ParaCurrency, 0, sizeof(m_ParaCurrency));
}

//===========================================================================
//
//  FUNCTION :  CParaCurrencyRate::~CParaCurrencyRate()
//
//  PURPOSE :   Destructor of Currency Conversion Rate Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaCurrencyRate::~CParaCurrencyRate()
{
}

//===========================================================================
//
//  FUNCTION :  CParaCurrencyRate::Read()
//
//  PURPOSE :   Read the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaCurrencyRate::Read()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaCurrencyRate::Read()"));

	BOOL    fSuccess;

	fSuccess = CParameter::ReadAllPara(CLASS_PARACURRENCYTBL, m_ParaCurrency, sizeof(m_ParaCurrency));

	pcsample::TraceFunction(_T("END   >> CParaCurrencyRate::Read() = %d"), fSuccess);

	return (fSuccess);
}

//===========================================================================
//
//  FUNCTION :  CParaCurrencyRate::Write()
//
//  PURPOSE :   Write the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaCurrencyRate::Write()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaCurrencyRate::Write()"));

	BOOL    fSuccess;

	fSuccess = CParameter::WriteAllPara(CLASS_PARACURRENCYTBL, m_ParaCurrency, sizeof(m_ParaCurrency));

	pcsample::TraceFunction(_T("END   >> CParaCurrencyRate::Write() = %d"), fSuccess);

	return (fSuccess);
}

ULONG    CParaCurrencyRate::getRate(USHORT usAddrs)  const
{
	ULONG    ulRet = 0;

	if (usAddrs > 0) {
		usAddrs--;

		ulRet = m_ParaCurrency[usAddrs];
	}

	return ulRet;
}


// ===============================================================================

//===========================================================================
//
//  FUNCTION :  CParaMnemonicTransaction::CParaMnemonicTransaction()
//
//  PURPOSE :   Constructor of Transaction Mnemonics Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaMnemonicTransaction::CParaMnemonicTransaction()
{
	memset(m_ParaTransMnemo, 0, sizeof(m_ParaTransMnemo));
}

//===========================================================================
//
//  FUNCTION :  CParaMnemonicTransaction::~CParaMnemonicTransaction()
//
//  PURPOSE :   Destructor of Transaction Mnemonics Parameter class.
//
//  RETURN :    No return value.
//
//===========================================================================

CParaMnemonicTransaction::~CParaMnemonicTransaction()
{
}

//===========================================================================
//
//  FUNCTION :  CParaMnemonicTransaction::Read()
//
//  PURPOSE :   Read the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaMnemonicTransaction::Read()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaMnemonicTransaction::Read()"));

	BOOL    fSuccess;
	fSuccess = CParameter::ReadAllParaByBlocks(CLASS_PARATRANSMNEMO, m_ParaTransMnemo, sizeof(m_ParaTransMnemo));

	pcsample::TraceFunction(_T("END   >> CParaMnemonicTransaction::Read() = %d"), fSuccess);

	return (fSuccess);
}

WCHAR *  CParaMnemonicTransaction::getMnemonic(UINT uiAddress)
{
	if (uiAddress > 0 && uiAddress <= MAX_TRANSM_NO) {
		uiAddress--;   // move range from one based to zero based to prep for array index.
		wcsncpy(m_lastMnemonic, m_ParaTransMnemo[uiAddress], PARA_TRANSMNEMO_LEN);
		m_lastMnemonic[PARA_TRANSMNEMO_LEN] = 0;
	}
	else {
		m_lastMnemonic[0] = 0;
	}

	pcsample::TraceFunction(_T("BEGIN >> CParaMnemonicTransaction::getMnemonic()"));

	return m_lastMnemonic;
}

//===========================================================================
//
//  FUNCTION :  CParaMnemonicTransaction::Write()
//
//  PURPOSE :   Write the specified parameter in 2170.
//
//  RETURN :    TRUE    - Specified parameter is read.
//              FALSE   - Function is failed.
//
//===========================================================================

BOOL CParaMnemonicTransaction::Write()
{
	pcsample::TraceFunction(_T("BEGIN >> CParaMnemonicTransaction::Write()"));

	BOOL    fSuccess;
	fSuccess = CParameter::WriteAllParaByBlocks(CLASS_PARATRANSMNEMO, m_ParaTransMnemo, sizeof(m_ParaTransMnemo));

	pcsample::TraceFunction(_T("END   >> CParaMnemonicTransaction::Write() = %d"), fSuccess);

	return (fSuccess);
}

/////////////////// END OF FILE ( ParaFlexMem.cpp ) /////////////////////////