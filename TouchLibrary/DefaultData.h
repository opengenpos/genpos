// DefaultData.h: interface for the DefaultData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFAULTDATA_H__03B8CF03_0B1D_4012_9DE3_A2C07B5BB2B5__INCLUDED_)
#define AFX_DEFAULTDATA_H__03B8CF03_0B1D_4012_9DE3_A2C07B5BB2B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "WindowButton.h"

/*Structure containing all attributes that are serialized for this class
any new attributes need to be added at the end of the struct so data from 
old layout files does not get corrupted during serialization*/
typedef struct _tagDefDataAttributes{
	unsigned long signatureStart;   //used to check that the data we are serializing is valid
	COLORREF ButtonDefault;			//default button color
	COLORREF WindowDefault;         //default window face color
	COLORREF TextWinDefault;		//default text control face color
	LOGFONT FontDefault;			//LOGFONT structure to hold defualt font attributes for button labels and text controls
	COLORREF FontColorDefault;		//defualt font color for button labels and text controls
	USHORT     WidthDefault;		//defualt button width (# of grid squares)
	USHORT     HeightDefault;       //defualt button height (# of grid squares)
	CWindowButton::ButtonShape    ShapeDefault;		//defualt button shape
	CWindowButton::ButtonPattern  PatternDefault;	//default button face pattern
}DefDataAttributes;


class DefaultData : public CObject
{
public:
	void Serialize( CArchive& ar );
	void AssignTo (DefaultData & dd);

	DefaultData();
	virtual ~DefaultData();

	DefDataAttributes defaultAttributes;
	ULONG ulTestSignature;

	DECLARE_SERIAL(DefaultData)

};

#endif // !defined(AFX_DEFAULTDATA_H__03B8CF03_0B1D_4012_9DE3_A2C07B5BB2B5__INCLUDED_)
