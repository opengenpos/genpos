/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             AT&T GIS, E&M OISO                **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Multiline Display Initialization
* Category    : Multiline Display, NCR 2170 US Hospitarity Application
* Program Name: MLDDUMMY.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract:
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
* Nov-10-95 : 03.01.00 :  M.Ozawa   : Initial
*============================================================================
*============================================================================
* PVCS Entry
* --------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
*============================================================================
*/

/*
=============================================================================
+                        I N C L U D E   F I L E s
=============================================================================
*/
/**------- MS - C ---------**/
#include	<tchar.h>
#include <stdlib.h>
#include <string.h>

/**------- 2170 local------**/
#include <ecr.h>
#include <regstrct.h>
#include <transact.h>
#include <paraequ.h>
#include <para.h>
#include <csstbstb.h>
#include <rfl.h>
#include <uie.h>
#include <vos.h>
#include <mld.h>
#include <mldmenu.h>

/*---- Register mode dummy function ----*/
SHORT   MldSetDescriptor(USHORT usMode)
{
    return (MLD_SUCCESS);

    usMode = usMode;
}

SHORT   MldDispSubTotal(USHORT usScroll, DCURRENCY lTotal)
{
    return (MLD_SUCCESS);

    usScroll = usScroll;
    lTotal = lTotal;
}

SHORT MldDispGCFQualifier(USHORT usScroll, TRANGCFQUAL *pData)
{
    return (MLD_SUCCESS);

    usScroll = usScroll;
    pData = pData;
}

SHORT   MldDispWaitKey(FDTWAITORDER *pData)
{
    return (MLD_SUCCESS);

    pData = pData;
}

SHORT   MldDispSeatNum(USHORT usScroll, MLDSEAT *pData)
{
    return (MLD_SUCCESS);

    usScroll = usScroll;
    pData = pData;
}

SHORT MldGetMldStatus()
{
    return(FALSE);
}

SHORT   MldDispCursor()
{
    return (MLD_SUCCESS);
}

SHORT   MldSetCursor(USHORT usScroll)
{
    return (MLD_SUCCESS);

    usScroll = usScroll;
}

SHORT MldDispGuestNum(MLDGUESTNUM *pGuestNum)
{
    return (MLD_SUCCESS);

    pGuestNum = pGuestNum;
}
/*---- Supervisor/Program mode dummy function ----*/
SHORT MldDispItem(VOID  *pItem, USHORT usStatus)
{
    return (MLD_SUCCESS);

    pItem = pItem;

    usStatus = usStatus;
}

UCHAR UifACPGMLDDispCom(CONST MLDMENU * *pForm)
{
    return 0;

    pForm = pForm;
}

VOID UifACPGMLDClear()
{
}

SHORT MldIRptItemOther(USHORT usScroll)
{
    return(MLD_SUCCESS);

    usScroll = usScroll;
}
#if 0
UCHAR FAR * FAR *   FARCONST    ACEntryMenu[1];
UCHAR FAR * FAR *   FARCONST    ACEntryMenuSatt[1];
UCHAR FAR * FAR *   FARCONST    ACEntryMenuBMast[1];
UCHAR FAR *   FARCONST    DaylyPTDSelect[1];
UCHAR FAR *   FARCONST    ResetTypeSelect[1];
UCHAR FAR *   FARCONST    TypeSelectReport1[1];
UCHAR FAR *   FARCONST    TypeSelectReport2[1];
UCHAR FAR *   FARCONST    TypeSelectReport3[1];
UCHAR FAR *   FARCONST    TypeSelectReport4[1];
UCHAR FAR *   FARCONST    TypeSelectReport5[1];
UCHAR FAR *   FARCONST    TypeSelectReport6[1];
UCHAR FAR *   FARCONST    TypeSelectReport7[1];
UCHAR FAR *   FARCONST    TypeSelectReport8[1];
UCHAR FAR *   FARCONST    TypeSelectReport10[1];
UCHAR FAR * FAR *  FARCONST  PGEntryMenu[1];
UCHAR FAR * FAR *  FARCONST  PGEntryMenuSatt[1];
#endif
SHORT   MldString(USHORT usScroll, SHORT sRow,
            TCHAR *pszString, USHORT usLength)
{
    return (MLD_SUCCESS);

    usScroll = usScroll;
    sRow = sRow;
    pszString = pszString;
    usLength = usLength;
}

/**** End of File ***/
