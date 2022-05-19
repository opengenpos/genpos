/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1993            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Print module Include file   (define printer column )                
* Category    : Print, NCR 2170 US Hospitarity Application
* Program Name: PrRColm_.H                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Jun-14-93 : 00.00.01 :  R.Itoh    :                                    
*           :          :            :                                    
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
============================================================================
+                       D E F I N I T I O N S
============================================================================
*/
#define     PRT_THCOLUMN        42      /* thermal column */
#define     PRT_EJCOLUMN        24      /* journal column */
#define     PRT_SPCOLUMN        44      /* slip column */
#define     PRT_KPCOLUMN        21      /* kp column */
#define     PRT_VLCOLUMN        55      /* validation column */

#define		PRT_MNEMONIC_CLIP	18		// PLU and other mnemoinc clipping point for RJ Column length