// DefaultData.cpp: implementation of the DefaultData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DefaultData.h"

#define DO_ULTESTSIGNATURE_FOR_MEMORY_OVERWRITE

#ifdef DO_ULTESTSIGNATURE_FOR_MEMORY_OVERWRITE
#pragma message("DO_ULTESTSIGNATURE_FOR_MEMORY_OVERWRITE is defined.")
#endif

IMPLEMENT_SERIAL( DefaultData, CObject, VERSIONABLE_SCHEMA | 1 )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DefaultData::DefaultData()
{
	//initialization of class data
	defaultAttributes.signatureStart = 0xABCDEF87;		
	defaultAttributes.WidthDefault = 3;				//3 * 3 button
	defaultAttributes.HeightDefault = 3;
	defaultAttributes.ButtonDefault = 0x00c0c0c0;	//grey
	defaultAttributes.WindowDefault = 0x00c0c0c0;   //grey
	defaultAttributes.TextWinDefault = 0x00ffffff;  //white
	defaultAttributes.ShapeDefault = CWindowButton::ButtonShapeRect;
	defaultAttributes.PatternDefault = CWindowButton::ButtonPatternNone;
	memset(&defaultAttributes.FontDefault, 0, sizeof(LOGFONT)); 
	defaultAttributes.FontColorDefault = NULL;

	ulTestSignature = 0xABCDEF01;
}

DefaultData::~DefaultData()
{
#ifdef DO_ULTESTSIGNATURE_FOR_MEMORY_OVERWRITE
	if (ulTestSignature != 0xABCDEF01) {
		__debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
	}
#endif
}

void DefaultData::Serialize( CArchive& ar )
 {
	UCHAR  *pBuff = (UCHAR*) &defaultAttributes;
	CObject::Serialize( ar );

	if( ar.IsStoring() ){
		ASSERT(defaultAttributes.signatureStart = 0xABCDEF87);
		ar<<sizeof(_tagDefDataAttributes);
		for(int i = 0; i < sizeof(_tagDefDataAttributes); i++){
			ar<<pBuff[i];
		}
	}
	else{
		ULONG mySize;
		memset(&defaultAttributes,0,sizeof(_tagDefDataAttributes));
		ar>>mySize;

		// Because the file that is being loaded in may be from a
		// different version of Layout Manager, we must be careful
		// about just reading in the information from the file.
		// If the object stored is larger than the object in the
		// version of Layout Manager being used then we will
		// overwrite memory and cause application failure.
		ULONG  xMySize = mySize;
		UCHAR  ucTemp;

		if (xMySize > sizeof (defaultAttributes)) {
			xMySize = sizeof (defaultAttributes);
		}
		UINT y = 0;
		for(y = 0; y < xMySize;y++){
			ar>>pBuff[y];
		}
		for(; y < mySize;y++){
			ar>>ucTemp;
		}

		ASSERT(defaultAttributes.signatureStart = 0xABCDEF87);

	 }

#ifdef DO_ULTESTSIGNATURE_FOR_MEMORY_OVERWRITE
	if (ulTestSignature != 0xABCDEF01) {
		__debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
	}
#endif
 }


//function assigns attributes from one DefaultData object to another
void DefaultData::AssignTo ( DefaultData &dd)
{
	dd.defaultAttributes.ButtonDefault = defaultAttributes.ButtonDefault;
	dd.defaultAttributes.FontColorDefault = defaultAttributes.FontColorDefault;
	dd.defaultAttributes.FontDefault = defaultAttributes.FontDefault;
	dd.defaultAttributes.HeightDefault = defaultAttributes.HeightDefault;
	dd.defaultAttributes.PatternDefault = defaultAttributes.PatternDefault;
	dd.defaultAttributes.ShapeDefault = defaultAttributes.ShapeDefault;
	dd.defaultAttributes.TextWinDefault = defaultAttributes.TextWinDefault;
	dd.defaultAttributes.WidthDefault = defaultAttributes.WidthDefault;
	dd.defaultAttributes.WindowDefault = defaultAttributes.WindowDefault;

#ifdef DO_ULTESTSIGNATURE_FOR_MEMORY_OVERWRITE
	if (ulTestSignature != 0xABCDEF01) {
		__debugbreak();   // replaced _asm {  int 3   }; to allow ARM compiles for Windows on Arm
	}
#endif
}