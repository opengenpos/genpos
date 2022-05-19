/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2172     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1999            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Evs Initialize
* Category    : Event Sub System Dynamic Link Library, NCR 2172 for Win32
* Program Name: EvsInit.c
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
** NCR2172 **
* Oct-07-99 : 1.00.00  : K.Iwata    : initial (for 2172)
* Sep-29-15 : 2.02.01  : R.Chambers : added reflection of messages to customer display windows.
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

#include <tchar.h>
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "Evs.h"
#include "EvsL.h"

/*
*===========================================================================
** Synopsis:    VOID __EvsInit(VOID);
*     Input:    nothing
*
** Return:      nothing
*===========================================================================
*/
SHORT sScreenMode;

VOID __EvsInit(VOID){
    int cnt,cnt2;

    /* initialize DataSourceTable */
    for(cnt = 0;cnt < EVS_MAX_DATASRC;cnt++){
        for(cnt2 = 0;cnt2 < EVS_MAX_SRCUSERS;cnt2++){
            DataSourceTable[cnt].ulUsers[cnt2] = EVS_USER_DEFAULT;
        }
        DataSourceTable[cnt].sDataID = EVS_DATAID_DEFAULT;
        DataSourceTable[cnt].sType = EVS_TYPE_UNKNOWN;
        DataSourceTable[cnt].pData = NULL;
    }

    /*** register data ID ***/

    /*** CAUTION!!!
            2172 PROTOTYPE1.0 ONLY!
    ***/

    /* COMMON */
    DataSourceTable[0].sDataID = LCDWIN_ID_COMMON001; // CWindowText::TypeLeadThru
    DataSourceTable[0].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[1].sDataID = LCDWIN_ID_COMMON002; // CWindowText::TypeLeadThruDescriptor
    DataSourceTable[1].sType = EVS_TYPE_DISPDATA;

    /* REG1 */
    DataSourceTable[2].sDataID = LCDWIN_ID_REG100;    // CWindowText::TypeSnglReceiptMain
    DataSourceTable[2].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[3].sDataID = LCDWIN_ID_REG101;    // CWindowText::TypeSnglReceiptSub
    DataSourceTable[3].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[4].sDataID = LCDWIN_ID_REG102;
    DataSourceTable[4].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[5].sDataID = LCDWIN_ID_REG103;
    DataSourceTable[5].sType = EVS_TYPE_DISPDATA;

    /* REG2 */
    DataSourceTable[6].sDataID = LCDWIN_ID_REG200;
    DataSourceTable[6].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[7].sDataID = LCDWIN_ID_REG201;
    DataSourceTable[7].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[8].sDataID = LCDWIN_ID_REG202;
    DataSourceTable[8].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[9].sDataID = LCDWIN_ID_REG203;
    DataSourceTable[9].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[10].sDataID = LCDWIN_ID_REG204;
    DataSourceTable[10].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[11].sDataID = LCDWIN_ID_REG205;
    DataSourceTable[11].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[12].sDataID = LCDWIN_ID_REG206;
    DataSourceTable[12].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[13].sDataID = LCDWIN_ID_REG207;
    DataSourceTable[13].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[14].sDataID = LCDWIN_ID_REG208;
    DataSourceTable[14].sType = EVS_TYPE_DISPDATA;
    DataSourceTable[15].sDataID = LCDWIN_ID_REG209;
    DataSourceTable[15].sType = EVS_TYPE_DISPDATA;

    /* SUPER VISER */
    DataSourceTable[16].sDataID = LCDWIN_ID_SUPER300;
    DataSourceTable[16].sType = EVS_TYPE_DISPDATA;

    /* PROGRAM */
    DataSourceTable[17].sDataID = LCDWIN_ID_PROG400;
    DataSourceTable[17].sType = EVS_TYPE_DISPDATA;

    /* SCREEN MODE */
    DataSourceTable[18].sDataID = SCRMODE_ID;
    DataSourceTable[18].sType = EVS_TYPE_SCREENMODE;
    sScreenMode = EVS_MODE_UNKNOWN;
    DataSourceTable[18].pData = &sScreenMode;

    /* Security Reminder Screen added by cwunn for GSU SR 15 */
    DataSourceTable[19].sDataID = LCDWIN_ID_SUPER301;
    DataSourceTable[19].sType = EVS_TYPE_DISPDATA;

	DataSourceTable[20].sDataID = LCDWIN_ID_REG110;    // CWindowText::TypeSnglReceiptMain
    DataSourceTable[20].sType = EVS_TYPE_DISPDATA;

    DataSourceTable[21].sDataID = LCDWIN_ID_COMMON004; // CWindowText::TypeLeadThruCustDisplay
    DataSourceTable[21].sType = EVS_TYPE_DISPDATA;

    DataSourceTable[22].sDataID = LCDWIN_ID_REG104;    // CWindowText::TypeOEPCustDisplay
    DataSourceTable[22].sType = EVS_TYPE_DISPIMAGE;

	// WARNING: When adding additional DataSourceTable elements make sure that the number
	//          of elemements is less than the defined constant EVS_MAX_DATASRC, the maximum
	//          number of elements in the table.
	{
		int iTemp = EVS_MAX_DATASRC;  // make it easy to find this defined constant in Visual Studio IDE.
	}
}


VOID __EvsSetScreenMode(SHORT modeTblIdx){
    sScreenMode = ScreenModeTable[modeTblIdx].sMode;
}

SHORT __EvsGetScreenMode(VOID){
	return sScreenMode;
}


/*** SCREEN MODE TABLE ***/
const SCREENMODE_TABLE ScreenModeTable[] =
{
    /* create window */
    {EVS_CREATE,LCDWIN_ID_REG101,  EVS_MODE_REG1}, /* REG 1 */
    {EVS_CREATE,LCDWIN_ID_REG208,  EVS_MODE_REG2}, /* REG 2 */
    {EVS_CREATE,LCDWIN_ID_SUPER300,EVS_MODE_SPV}, /* SUPER VISER */
    {EVS_CREATE,LCDWIN_ID_PROG400, EVS_MODE_PRG}, /* PROGRAM */

    /* delete window */
    {EVS_DELETE,LCDWIN_ID_REG101,  EVS_MODE_LOCK}, /* LOCK */
    {EVS_DELETE,LCDWIN_ID_REG208,  EVS_MODE_LOCK}, /* LOCK */
    {EVS_DELETE,LCDWIN_ID_SUPER300,EVS_MODE_LOCK}, /* LOCK */
    {EVS_DELETE,LCDWIN_ID_PROG400, EVS_MODE_LOCK}, /* LOCK */

    {EVS_NULL,EVS_NULL,EVS_MODE_UNKNOWN}
};


/* ========================================================================== */

