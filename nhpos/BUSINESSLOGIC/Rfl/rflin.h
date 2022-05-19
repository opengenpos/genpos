/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS Corporation, E&M OISO    **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Reentrant Functions Library Headder File           
* Category    : Reentrant Functions Library, NCR 2170  Application
* Program Name: RflIn.h                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* May-08-92 : 00.00.01 : K.Nakajima :                                    
* Apr-14-95 : 03.00.00 : T.Nakahata : Add Coupon Feature
*===========================================================================
*===========================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*===========================================================================
*/

/**
;========================================================================
;+               L O C A L    D E F I N I T I O N s 
;========================================================================
**/
#define     RFL_BYTELEN                   8           

#define     RFL_BILLION         1000000000L        /* 1 billion */

#define     RFL_MODULUS                  97        /* Modulus for CD */
#define     RFL_MAXDIGIT                 10        /* MaxDigit  */
#define     RFL_WEIGHTDIGIT               5        /* Weight value start digit */

#define     RFL_TAXTBLHEAD            (2*3)        /* tax table header length (2byte x 3tables) */
#define     RFL_WDCLEN                    5        /* wdc, wdc on tax and rate area length */
#define     RFL_WDC                       0        /* Posit. of Whole dollar constant */
#define     RFL_TAXONWDC                  1        /* Posit. of TaxONWDC */
#define     RFL_PLUS                      0        
#define     RFL_MINUS                     1
/**
;========================================================================
;+               M A C R O    D E F I N I T I O N s 
;========================================================================
**/
#define    RFLATOH(a)                ((a) - 0x30)  /* convert ascii to hexa */

/**
;========================================================================
;+                  T Y P E   D E F I N I T I O N s
;========================================================================
**/
/**
;========================================================================
;+          P R O T O T Y P E    D E C L A R A T I O N s
;========================================================================
**/
#if defined(POSSIBLE_DEAD_CODE)
static VOID  _RflAdjustTtl(D13DIGITS *p13Dest);
/* SHORT  RflRoundHuge(LONG *plModAmount, HUGEINT *phuAmount, UCHAR uchType); *//* move to rfl.h, V3.4 */
VOID  RflAdjustD24Ttl(D24DIGITS *p24Dest);
#endif

/* ===== End of File ( RflIn.h ) ===== */
