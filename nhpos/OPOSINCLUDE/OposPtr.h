/////////////////////////////////////////////////////////////////////
//
// OposPtr.h
//
//   POS Printer header file for OPOS Applications.
//
// Modification history
// ------------------------------------------------------------------
// 95-12-08 OPOS Release 1.0                                     CRM
// 96-04-22 OPOS Release 1.1                                     CRM
//   Add CapCharacterSet values.
//   Add ErrorLevel values.
//   Add TransactionPrint Control values.
// 97-06-04 OPOS Release 1.2                                     CRM
//   Remove PTR_RP_NORMAL_ASYNC.
//   Add more barcode symbologies.
// 98-03-06 OPOS Release 1.3                                     CRM
//   Add more PrintTwoNormal constants.
// 00-09-24 OPOS Release 1.5                               EPSON/BKS
//   Add CapRecMarkFeed values and MarkFeed constants.
//   Add ChangePrintSide constants.
//   Add StatusUpdateEvent constants.
//   Add ResultCodeExtended values.
//   Add CapXxxCartridgeSensor and XxxCartridgeState values.
//   Add CartridgeNotify values.
//   Add CapCharacterset and CharacterSet values for UNICODE.
//
/////////////////////////////////////////////////////////////////////

#if !defined(OPOSPTR_H)
#define      OPOSPTR_H


#include "Opos.h"


/////////////////////////////////////////////////////////////////////
// Printer Station Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_S_JOURNAL        = 1;
const LONG PTR_S_RECEIPT        = 2;
const LONG PTR_S_SLIP           = 4;

const LONG PTR_S_JOURNAL_RECEIPT    = 0x0003;
const LONG PTR_S_JOURNAL_SLIP       = 0x0005;
const LONG PTR_S_RECEIPT_SLIP       = 0x0006;

const LONG PTR_TWO_RECEIPT_JOURNAL  = 0x8003;
const LONG PTR_TWO_SLIP_JOURNAL     = 0x8005;
const LONG PTR_TWO_SLIP_RECEIPT     = 0x8006;


/////////////////////////////////////////////////////////////////////
// "CapCharacterSet" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_CCS_ALPHA        =   1;
const LONG PTR_CCS_ASCII        = 998;
const LONG PTR_CCS_KANA         =  10;
const LONG PTR_CCS_KANJI        =  11;
const LONG PTR_CCS_UNICODE      = 997;


/////////////////////////////////////////////////////////////////////
// "CharacterSet" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_CS_UNICODE       = 997;
const LONG PTR_CS_ASCII         = 998;
const LONG PTR_CS_WINDOWS       = 999;


/////////////////////////////////////////////////////////////////////
// "ErrorLevel" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_EL_NONE          = 1;
const LONG PTR_EL_RECOVERABLE   = 2;
const LONG PTR_EL_FATAL         = 3;


/////////////////////////////////////////////////////////////////////
// "MapMode" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_MM_DOTS          = 1;
const LONG PTR_MM_TWIPS         = 2;
const LONG PTR_MM_ENGLISH       = 3;
const LONG PTR_MM_METRIC        = 4;


/////////////////////////////////////////////////////////////////////
// "CapXxxColor" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_COLOR_PRIMARY      = 0x00000001;
const LONG PTR_COLOR_CUSTOM1      = 0x00000002;
const LONG PTR_COLOR_CUSTOM2      = 0x00000004;
const LONG PTR_COLOR_CUSTOM3      = 0x00000008;
const LONG PTR_COLOR_CUSTOM4      = 0x00000010;
const LONG PTR_COLOR_CUSTOM5      = 0x00000020;
const LONG PTR_COLOR_CUSTOM6      = 0x00000040;
const LONG PTR_COLOR_CYAN         = 0x00000100;
const LONG PTR_COLOR_MAGENTA      = 0x00000200;
const LONG PTR_COLOR_YELLOW       = 0x00000400;
const LONG PTR_COLOR_FULL         = 0x80000000;


/////////////////////////////////////////////////////////////////////
// "CapXxxCartridgeSensor" and  "XxxCartridgeState" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_CART_UNKNOWN         = 0x10000000;
const LONG PTR_CART_OK              = 0x00000000;
const LONG PTR_CART_REMOVED         = 0x00000001;
const LONG PTR_CART_EMPTY           = 0x00000002;
const LONG PTR_CART_NEAREND         = 0x00000004;
const LONG PTR_CART_CLEANING        = 0x00000008;


/////////////////////////////////////////////////////////////////////
// "CartridgeNotify"  Property Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_CN_DISABLED        = 0x00000000;
const LONG PTR_CN_ENABLED         = 0x00000001;


/////////////////////////////////////////////////////////////////////
// "CutPaper" Method Constant
/////////////////////////////////////////////////////////////////////

const LONG PTR_CP_FULLCUT       = 100;


/////////////////////////////////////////////////////////////////////
// "PrintBarCode" Method Constants:
/////////////////////////////////////////////////////////////////////

//   "Alignment" Parameter
//     Either the distance from the left-most print column to the start
//     of the bar code, or one of the following:

const LONG PTR_BC_LEFT          = -1;
const LONG PTR_BC_CENTER        = -2;
const LONG PTR_BC_RIGHT         = -3;

//   "TextPosition" Parameter

const LONG PTR_BC_TEXT_NONE     = -11;
const LONG PTR_BC_TEXT_ABOVE    = -12;
const LONG PTR_BC_TEXT_BELOW    = -13;

//   "Symbology" Parameter:

//     One dimensional symbologies
const LONG PTR_BCS_UPCA         = 101;  // Digits
const LONG PTR_BCS_UPCE         = 102;  // Digits
const LONG PTR_BCS_JAN8         = 103;  // = EAN 8
const LONG PTR_BCS_EAN8         = 103;  // = JAN 8 (added in 1.2)
const LONG PTR_BCS_JAN13        = 104;  // = EAN 13
const LONG PTR_BCS_EAN13        = 104;  // = JAN 13 (added in 1.2)
const LONG PTR_BCS_TF           = 105;  // (Discrete 2 of 5) Digits
const LONG PTR_BCS_ITF          = 106;  // (Interleaved 2 of 5) Digits
const LONG PTR_BCS_Codabar      = 107;  // Digits, -, $, :, /, ., +;
                                        //   4 start/stop characters
                                        //   (a, b, c, d)
const LONG PTR_BCS_Code39       = 108;  // Alpha, Digits, Space, -, .,
                                        //   $, /, +, %; start/stop (*)
                                        // Also has Full ASCII feature
const LONG PTR_BCS_Code93       = 109;  // Same characters as Code 39
const LONG PTR_BCS_Code128      = 110;  // 128 data characters
//        (The following were added in Release 1.2)
const LONG PTR_BCS_UPCA_S       = 111;  // UPC-A with supplemental
                                        //   barcode
const LONG PTR_BCS_UPCE_S       = 112;  // UPC-E with supplemental
                                        //   barcode
const LONG PTR_BCS_UPCD1        = 113;  // UPC-D1
const LONG PTR_BCS_UPCD2        = 114;  // UPC-D2
const LONG PTR_BCS_UPCD3        = 115;  // UPC-D3
const LONG PTR_BCS_UPCD4        = 116;  // UPC-D4
const LONG PTR_BCS_UPCD5        = 117;  // UPC-D5
const LONG PTR_BCS_EAN8_S       = 118;  // EAN 8 with supplemental
                                        //   barcode
const LONG PTR_BCS_EAN13_S      = 119;  // EAN 13 with supplemental
                                        //   barcode
const LONG PTR_BCS_EAN128       = 120;  // EAN 128
const LONG PTR_BCS_OCRA         = 121;  // OCR "A"
const LONG PTR_BCS_OCRB         = 122;  // OCR "B"
const LONG PTR_BCS_Code128_Parsed=123;

//     Two dimensional symbologies
const LONG PTR_BCS_PDF417       = 201;
const LONG PTR_BCS_MAXICODE     = 202;

//     Start of Printer-Specific bar code symbologies
const LONG PTR_BCS_OTHER        = 501;


/////////////////////////////////////////////////////////////////////
// "PrintBitmap" Method Constants:
/////////////////////////////////////////////////////////////////////

//   "Width" Parameter
//     Either bitmap width or:

const LONG PTR_BM_ASIS          = -11;  // One pixel per printer dot

//   "Alignment" Parameter
//     Either the distance from the left-most print column to the start
//     of the bitmap, or one of the following:

const LONG PTR_BM_LEFT          = -1;
const LONG PTR_BM_CENTER        = -2;
const LONG PTR_BM_RIGHT         = -3;


/////////////////////////////////////////////////////////////////////
// "RotatePrint" Method: "Rotation" Parameter Constants
// "RotateSpecial" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_RP_NORMAL        = 0x0001;
const LONG PTR_RP_RIGHT90       = 0x0101;
const LONG PTR_RP_LEFT90        = 0x0102;
const LONG PTR_RP_ROTATE180     = 0x0103;


/////////////////////////////////////////////////////////////////////
// "SetLogo" Method: "Location" Parameter Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_L_TOP            = 1;
const LONG PTR_L_BOTTOM         = 2;


/////////////////////////////////////////////////////////////////////
// "TransactionPrint" Method: "Control" Parameter Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_TP_TRANSACTION   = 11;
const LONG PTR_TP_NORMAL        = 12;


/////////////////////////////////////////////////////////////////////
// "MarkFeed" Method: "Type" Parameter Constants
// "CapRecMarkFeed" Property Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_MF_TO_TAKEUP     = 1;
const LONG PTR_MF_TO_CUTTER     = 2;
const LONG PTR_MF_TO_CURRENT_TOF= 4;
const LONG PTR_MF_TO_NEXT_TOF   = 8;


/////////////////////////////////////////////////////////////////////
// "ChangePrintSide" Method: "Side" Parameter Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_PS_UNKNOWN       = 0;
const LONG PTR_PS_SIDE1         = 1;
const LONG PTR_PS_SIDE2         = 2;
const LONG PTR_PS_OPPOSITE      = 3;


/////////////////////////////////////////////////////////////////////
// "StatusUpdateEvent" Event: "Data" Parameter Constants
/////////////////////////////////////////////////////////////////////

const LONG PTR_SUE_COVER_OPEN   =   11;
const LONG PTR_SUE_COVER_OK     =   12;

const LONG PTR_SUE_JRN_EMPTY    =   21;
const LONG PTR_SUE_JRN_NEAREMPTY=   22;
const LONG PTR_SUE_JRN_PAPEROK  =   23;

const LONG PTR_SUE_REC_EMPTY    =   24;
const LONG PTR_SUE_REC_NEAREMPTY=   25;
const LONG PTR_SUE_REC_PAPEROK  =   26;

const LONG PTR_SUE_SLP_EMPTY    =   27;
const LONG PTR_SUE_SLP_NEAREMPTY=   28;
const LONG PTR_SUE_SLP_PAPEROK  =   29;

const LONG PTR_SUE_JRN_CARTRIDGE_EMPTY     = 41;
const LONG PTR_SUE_JRN_CARTRIDGE_NEAREMPTY = 42;
const LONG PTR_SUE_JRN_HEAD_CLEANING       = 43;
const LONG PTR_SUE_JRN_CARTRIDGE_OK        = 44;

const LONG PTR_SUE_REC_CARTRIDGE_EMPTY     = 45;
const LONG PTR_SUE_REC_CARTRIDGE_NEAREMPTY = 46;
const LONG PTR_SUE_REC_HEAD_CLEANING       = 47;
const LONG PTR_SUE_REC_CARTRIDGE_OK        = 48;

const LONG PTR_SUE_SLP_CARTRIDGE_EMPTY     = 49;
const LONG PTR_SUE_SLP_CARTRIDGE_NEAREMPTY = 50;
const LONG PTR_SUE_SLP_HEAD_CLEANING       = 51;
const LONG PTR_SUE_SLP_CARTRIDGE_OK        = 52;

const LONG PTR_SUE_IDLE         = 1001;


/////////////////////////////////////////////////////////////////////
// "ResultCodeExtended" Property Constants
// Following are return values from GetResultCodeExtended() for OPOS printer
/////////////////////////////////////////////////////////////////////

const LONG OPOS_EPTR_COVER_OPEN            = 201; // (Several)
const LONG OPOS_EPTR_JRN_EMPTY             = 202; // (Several)
const LONG OPOS_EPTR_REC_EMPTY             = 203; // (Several)
const LONG OPOS_EPTR_SLP_EMPTY             = 204; // (Several)
const LONG OPOS_EPTR_SLP_FORM              = 205; // EndRemoval
const LONG OPOS_EPTR_TOOBIG                = 206; // PrintBitmap
const LONG OPOS_EPTR_BADFORMAT             = 207; // PrintBitmap
const LONG OPOS_EPTR_JRN_CARTRIDGE_REMOVED = 208; // (Several)
const LONG OPOS_EPTR_JRN_CARTRIDGE_EMPTY   = 209; // (Several)
const LONG OPOS_EPTR_JRN_HEAD_CLEANING     = 210; // (Several)
const LONG OPOS_EPTR_REC_CARTRIDGE_REMOVED = 211; // (Several)
const LONG OPOS_EPTR_REC_CARTRIDGE_EMPTY   = 212; // (Several)
const LONG OPOS_EPTR_REC_HEAD_CLEANING     = 213; // (Several)
const LONG OPOS_EPTR_SLP_CARTRIDGE_REMOVED = 214; // (Several)
const LONG OPOS_EPTR_SLP_CARTRIDGE_EMPTY   = 215; // (Several)
const LONG OPOS_EPTR_SLP_HEAD_CLEANING     = 216; // (Several)


#endif                  // !defined(OPOSPTR_H)
