/*************************************************************************
 *
 * PrinterCaps.h
 *
 * $Workfile:: PrinterCaps.h                                             $
 *
 * $Revision::                                                           $
 *
 * $Archive::                                                            $
 *
 * $Date::                                                               $
 *
 *************************************************************************
 *
 * Copyright (c) NCR Corporation 2000.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: PrinterCaps.h                                              $
 *
 ************************************************************************/

#if !defined(_INC_PRINTER_CAPS)
#define _INC_PRINTER_CAPS

#ifdef __cplusplus
extern "C"{
#endif

/*
/////////////////////////////////////////////////////////////////////////////
//  IoControl Function Interface
/////////////////////////////////////////////////////////////////////////////
*/

typedef struct _DEVICE_CAPS_PRINTER
{
    DWORD   dwSize;         /* Length of this Structure */
    DWORD   dwStation;      /* Supporting Station */
    DWORD   dwColumnR;      /* Column of Receipt Station */
    DWORD   dwColumnJ;      /* Column of Journal Station */
    DWORD   dwColumnS;      /* Column of Slip Station */
    DWORD   dwColumnV;      /* Column of Validation Station */
    DWORD   dwLineV;        /* Line count of Validation Station */
    DWORD   dwCut;          /* Supporting Cut Function */
    DWORD   dwStamp;        /* Supporting Stamp Function */
    DWORD   dwStampSize;    /* Stamp Size Infomation */
    DWORD   dwFeedR;        /* feed count to paper cut ,after print */
} DEVICE_CAPS_PRINTER, *PDEVICE_CAPS_PRINTER;


/* Supporting Station */
#define     PRINTER_CAPS_RECIEPT        0x00000001
#define     PRINTER_CAPS_JOURNAL        0x00000002
#define     PRINTER_CAPS_SLIP           0x00000004
#define     PRINTER_CAPS_VALIDATION     0x00000008

/* Supporting Cut Function */
#define     PRINTER_CAPS_FULL_CAT       0x00000001
#define     PRINTER_CAPS_PARTIAL        0x00000002

/* Supporting Stamp Function */
#define     PRINTER_CAPS_ELEC_STAMP     0x00000001
#define     PRINTER_CAPS_BITMAP_STAMP   0x00000002


#ifdef __cplusplus
}
#endif

#endif /* !defined(_INC_DEVICEIO_PRINTER) */
