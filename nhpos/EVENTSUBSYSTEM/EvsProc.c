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
* Title       : Event Sub System
* Category    : Event Sub System Dynamic Link Library, NCR 2172 for Win32
* Program Name: EvsProc.c
* --------------------------------------------------------------------------
* Compiler    : VC++ Ver. 6.00  by Microsoft Corp.
* Memory Model: 
* Options     : 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*  Date     : Ver.Rev. :   Name     : Description
** NCR2172 **
* Oct-12-99 : 1.00.00  : K.Iwata    : initial (for 2172)
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

#include <tchar.h>
#include "ecr.h"
#include "pif.h"
#include "log.h"
#include "vosl.h"
#include "rfl.h"

#include "Evs.H"
#include "EvsL.H"

/*
*===========================================================================
** Synopsis:    LONG __EvsSearchDataSrc_ID(SHORT sDataId);
*　
*     Input:    sDataId   - data id
*
** Return:      -1        not found
*               >=0       DataSourceTable[]s INDEX
*===========================================================================
*/
LONG __EvsSearchDataSrc_ID(SHORT sDataId){
    int    cnt;

    for(cnt = 0;cnt < EVS_MAX_DATASRC;cnt++){
        if(DataSourceTable[cnt].sDataID == sDataId){
            return (LONG)cnt;
        }
    }
    return -1;
}


/*
*===========================================================================
** Synopsis:    LONG __EvsSearchDataSrc_ID(SHORT sDataId);
*　
*     Input:    nothing
*
** Return:      -1        not found
*               >=0       DataSourceTable[]s INDEX
*===========================================================================
*/
LONG __EvsSearchDataSrc_Empty(VOID){
    int cnt;

    for(cnt = 0;cnt < EVS_MAX_DATASRC;cnt++){
        if(DataSourceTable[cnt].sDataID == EVS_DATAID_DEFAULT){
            return (LONG)cnt;
        }
    }
    return -1;
}


/*
*===========================================================================
** Synopsis:    LONG __EvsSearchUser(SHORT sIdx,ULONG ulUser);
*　
*     Input:    sDataTblIdx - 
*               ulUser      - user handle (HANDLE hWnd)
*
** Return:      index        - found
*               -1       - not found
*===========================================================================
*/
LONG __EvsSearchUser(SHORT sIdx,ULONG ulUser){
    int cnt;

    for(cnt = 0;cnt < EVS_MAX_SRCUSERS;cnt++){
        if(DataSourceTable[sIdx].ulUsers[cnt] == ulUser)
           return cnt;
    }
    return -1;
}


/*
*===========================================================================
** Synopsis:    LONG __EvsSearchUsr_Empty(LONG lDataSrcIdx);
*　
*     Input:    sDataTblIdx - 
*
** Return:      
*               
*===========================================================================
*/
LONG __EvsSearchUsr_Empty(LONG lDataSrcIdx){
    int cnt;

    if((lDataSrcIdx < 0L) || (EVS_MAX_DATASRC <= lDataSrcIdx)){
        return -2;
    }
    for(cnt = 0;cnt < EVS_MAX_SRCUSERS;cnt++){
        if(DataSourceTable[lDataSrcIdx].ulUsers[cnt] == EVS_USER_DEFAULT){
            return (LONG)cnt;
        }
    }
    return -1;
}


/*
*===========================================================================
** Synopsis:    
*　
*     Input:    lDataTblIdx - 
*
** Return:      
*               
*===========================================================================
*/
SHORT __EvsAddUser(LONG lDataTblIdx,LONG lUsrIdx,ULONG ulhUser){

    /* register user */
    DataSourceTable[lDataTblIdx].ulUsers[lUsrIdx] = ulhUser;

    if(DataSourceTable[lDataTblIdx].pData != NULL){
        /* already created */
        __wEvsPostMessage((ULONG)WU_EVS_CREATE,(LONG)lDataTblIdx,(ULONG)DataSourceTable[lDataTblIdx].sDataID,(ULONG)0);

        /* check the screen mode */
		if(DataSourceTable[lDataTblIdx].sType == EVS_TYPE_DISPDATA){
		    __vEvsUpdateCheck_SCRMODE(EVS_CREATE,DataSourceTable[lDataTblIdx].pData); /* pData --> VOSWINDOW pointer */
		}
        return (SHORT)TRUE;
    }
    else{
        return (SHORT)FALSE;
    }
}

/*
*===========================================================================
** Synopsis:    
*　
*     Input:    lDataTblIdx - 
*
** Return:      
*               
*===========================================================================
*/
VOID __EvsRemoveUser(LONG lDataTblIdx,LONG lUsrIdx){

    /* remove user */
    DataSourceTable[lDataTblIdx].ulUsers[lUsrIdx] = -1;

}

/* ========================================================================= */

