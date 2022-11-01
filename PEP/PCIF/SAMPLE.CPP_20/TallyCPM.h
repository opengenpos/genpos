/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1998  NCR Corporation.  All Rights Reserved.
//
//  FILE NAME:  TallyCPM.h
//
//  PURPOSE:    Tally of CPM/EPT class definitions and declarations.
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

#if !defined(AFX_TALLYCPM_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)
#define AFX_TALLYCPM_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_

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
//  CTallyCPM
//---------------------------------------------------------------------------

class CTallyCPM
{
public:
    CTallyCPM();
    ~CTallyCPM();

    BOOL    Read();
    BOOL    ReadAndReset();

    SHORT   GetLastError() const;

    USHORT  getCpmSendOK() const;
    USHORT  getCpmSendError() const;
    USHORT  getCpmResponse() const;
    USHORT  getCpmUnsolicitedData() const;
    USHORT  getCpmDroppedResponse() const;
    USHORT  getCpmTimeOut() const;
    USHORT  getCpmIHCError() const;
    USHORT  getEptSendOK() const;
    USHORT  getEptSendError() const;
    USHORT  getEptResponseOK() const;
    USHORT  getEptResponseError() const;

private:
    SHORT   m_sLastError;
    CPMEPT_TALLY    m_tallyCPM;
};

/////////////////////////////////////////////////////////////////////////////
//
//                  I N L I N E    F U N C T I O N s
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//  MEMBER VARIABLE ACCESS FUNCTIONs
//
//---------------------------------------------------------------------------

//===========================================================================
//  FUNCTION :  CTallyCPM::GetLastError()
//===========================================================================

inline SHORT CTallyCPM::GetLastError() const
{
    return ( m_sLastError );
}

//===========================================================================
//  FUNCTION :  CTallyCPM::getCpmSendOK()
//===========================================================================

inline USHORT CTallyCPM::getCpmSendOK() const
{
    return ( m_tallyCPM.CpmTally.usPMSSendOK );
}

//===========================================================================
//  FUNCTION :  CTallyCPM::getCpmSendError()
//===========================================================================

inline USHORT CTallyCPM::getCpmSendError() const
{
    return ( m_tallyCPM.CpmTally.usPMSSendErr );
}

//===========================================================================
//  FUNCTION :  CTallyCPM::getCpmResponse()
//===========================================================================

inline USHORT CTallyCPM::getCpmResponse() const
{
    return ( m_tallyCPM.CpmTally.usPMSRecResp );
}

//===========================================================================
//  FUNCTION :  CTallyCPM::getCpmUnsolicitedData()
//===========================================================================

inline USHORT CTallyCPM::getCpmUnsolicitedData() const
{
    return ( m_tallyCPM.CpmTally.usPMSRecUnsoli );
}

//===========================================================================
//  FUNCTION :  CTallyCPM::getCpmDroppedResponse()
//===========================================================================

inline USHORT CTallyCPM::getCpmDroppedResponse() const
{
    return ( m_tallyCPM.CpmTally.usPMSThrowResp );
}

//===========================================================================
//  FUNCTION :  CTallyCPM::getCpmTimeOut()
//===========================================================================

inline USHORT CTallyCPM::getCpmTimeOut() const
{
    return ( m_tallyCPM.CpmTally.usTimeout );
}

//===========================================================================
//  FUNCTION :  CTallyCPM::getCpmIHCError()
//===========================================================================

inline USHORT CTallyCPM::getCpmIHCError() const
{
    return ( m_tallyCPM.CpmTally.usIHCSendErr );
}

//===========================================================================
//  FUNCTION :  CTallyCPM::getEptSendOK()
//===========================================================================

inline USHORT CTallyCPM::getEptSendOK() const
{
    return ( m_tallyCPM.EptTally.usEPTSendOK );
}

//===========================================================================
//  FUNCTION :  CTallyCPM::getEptSendError()
//===========================================================================

inline USHORT CTallyCPM::getEptSendError() const
{
    return ( m_tallyCPM.EptTally.usEPTSendErr );
}

//===========================================================================
//  FUNCTION :  CTallyCPM::getEptResponseOK()
//===========================================================================

inline USHORT CTallyCPM::getEptResponseOK() const
{
    return ( m_tallyCPM.EptTally.usEPTRespOK );
}

//===========================================================================
//  FUNCTION :  CTallyCPM::getEptResponseError()
//===========================================================================

inline USHORT CTallyCPM::getEptResponseError() const
{
    return ( m_tallyCPM.EptTally.usEPTRespErr );
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TALLYCPM_H__70072F08_42ED_11D2_A9E2_0000399BE006__INCLUDED_)

//////////////////// END OF FILE ( TallyCPM.h ) /////////////////////////////
