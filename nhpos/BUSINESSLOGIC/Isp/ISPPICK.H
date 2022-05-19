/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1994-9          **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : ISP Server Pickup/Loan Header                        
* Category    : ISP Server , NCR 2170 US General Purpose Model         
* Program Name: ISPPICK.H                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Feb-15-93:00.00.01:H.Yamaguchi: initial                                   
* Feb-18-94:01.01.00:M.Ozawa    : Add class for R1.1
* Mar-29-94:        :hkato      : Add class for R1.1
* May-17-94:01.01.03:M.Ozawa    : Add IspUpdateFCFormat()
* Oct-04-96:02.00.00:Y.Shoji    : Add intl for R2.0
* Dec-07-99:01.00.00:hrkato     : Saratoga
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

/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
typedef struct {
    USHORT  usFunc;                     /* Function        */
    UCHAR   uchMedia;                   /* Media Class     */
    UCHAR   uchOpeMode;                 /* Operation Mode  */
    ULONG   ulTotal;                    /* Total           */
    USHORT  usCount;                    /* Counter         */
} ISPPICKUP;

/*--------------------------------------------------------------
    USHORT  usFunc; Define function either Pickup or Loan.
--------------------------------------------------------------*/
#define ISP_FUNC_LOAN               1   /* Execute loan   function */
#define ISP_FUNC_PICKUP             2   /* Execute pickup function */

/*--------------------------------------------------------------
    UCHAR   uchOpeMode; Define operation mode for Pickup/Loan
--------------------------------------------------------------*/
#define ISP_PICKUP_NOMAL_OP         1   /* Execute Normal operation       */
#define ISP_PICKUP_VOID_OP          2   /* Execute E/C or Void operation  */

/*------------------------------------------------------------------------
    ISPTTLSB.C  ( 0x0403 Total Functions)
------------------------------------------------------------------------*/

/*===== END OF DEFINITION =====*/