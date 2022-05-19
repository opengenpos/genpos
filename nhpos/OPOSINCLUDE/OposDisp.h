/////////////////////////////////////////////////////////////////////
//
// OposDisp.h
//
//   Line Display header file for OPOS Applications.
//
// Modification history
// ------------------------------------------------------------------
// 95-12-08 OPOS Release 1.0                                     CRM
// 96-03-18 OPOS Release 1.01                                    CRM
//   Add DISP_MT_INIT constant and MarqueeFormat constants.
// 96-04-22 OPOS Release 1.1                                     CRM
//   Add CapCharacterSet values for Kana and Kanji.
//
/////////////////////////////////////////////////////////////////////

#if !defined(OPOSDISP_H)
#define      OPOSDISP_H


#include "Opos.h"


/////////////////////////////////////////////////////////////////////
// "CapBlink" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG DISP_CB_NOBLINK      = 0;
const LONG DISP_CB_BLINKALL     = 1;
const LONG DISP_CB_BLINKEACH    = 2;


/////////////////////////////////////////////////////////////////////
// "CapCharacterSet" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG DISP_CCS_NUMERIC     =   0;
const LONG DISP_CCS_ALPHA       =   1;
const LONG DISP_CCS_ASCII       = 998;
const LONG DISP_CCS_KANA        =  10;
const LONG DISP_CCS_KANJI       =  11;


/////////////////////////////////////////////////////////////////////
// "CharacterSet" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG DISP_CS_ASCII        = 998;
const LONG DISP_CS_UNICODE      = 997;
const LONG DISP_CS_ANSI         = 999;
const LONG DISP_CS_WINDOWS      = 999;



/////////////////////////////////////////////////////////////////////
// "MarqueeType" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG DISP_MT_NONE         = 0;
const LONG DISP_MT_UP           = 1;
const LONG DISP_MT_DOWN         = 2;
const LONG DISP_MT_LEFT         = 3;
const LONG DISP_MT_RIGHT        = 4;
const LONG DISP_MT_INIT         = 5;


/////////////////////////////////////////////////////////////////////
// "MarqueeFormat" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG DISP_MF_WALK         = 0;
const LONG DISP_MF_PLACE        = 1;


/////////////////////////////////////////////////////////////////////
// "DisplayText" Method: "Attribute" Property Constants
// "DisplayTextAt" Method: "Attribute" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG DISP_DT_NORMAL       = 0;
const LONG DISP_DT_BLINK        = 1;


/////////////////////////////////////////////////////////////////////
// "ScrollText" Method: "Direction" Parameter Constants
/////////////////////////////////////////////////////////////////////

const LONG DISP_ST_UP           = 1;
const LONG DISP_ST_DOWN         = 2;
const LONG DISP_ST_LEFT         = 3;
const LONG DISP_ST_RIGHT        = 4;


/////////////////////////////////////////////////////////////////////
// "SetDescriptor" Method: "Attribute" Parameter Constants
/////////////////////////////////////////////////////////////////////

const LONG DISP_SD_OFF          = 0;
const LONG DISP_SD_ON           = 1;
const LONG DISP_SD_BLINK        = 2;


#endif                  // !defined(OPOSDISP_H)
