/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Client/Server STUB, Electoric Journal Source File                        
* Category    : Client/Server STUB, INT'L Hospitality Model
* Program Name: CSSTBEJ.C                                            
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /Alfw /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:  
*                                   
*               SerEJClear()        Reset EJ File
*               SerEJReadFile()     Read file
*               SerEJNoOfTrans()
*               SerEJReadResetTransOver()
*               SerEJReadResetTransStart()
*               SerEJReadResetTrans()
*               SerEJReadResetTransEnd()
*               SerEJNoOfTrans()
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Jun-07-93:00.00.01:H.yamaguchi: Adds function(Same as HPINT'L)
* Sep-07-98:        :O.Nakada   : Add V3.3
*          :        :           :                         
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
#if defined(POSSIBLE_DEAD_CODE)
		// This code is a candidate for removal with the next release of source code
#include    <pc2170.h>
#endif
#include    <r20_pc2172.h>

#include    <r20_pif.h>
#include    <memory.h>
#include    "csstubin.h"
#include "mypifdefs.h"

/*
*==========================================================================
**    Synopsis:    SHORT SerEJClear(VOID)
*
*       Input:    Nothing
*      Output:    Nothing
*       InOut:    Nothing
*
**  Return    :
*            Normal End:  EJ_PERFORM
*          Abnormal End:  EJ_FILE_NOT_EXIST
*                      :  EJ_NO_MAKE_FILE
*
**  Description:
*               Reset Electronic Journal File.     
*                
*==========================================================================
*/
SHORT STUBENTRY SerEJClear(VOID)
{
    CLIREQEJ   ReqMsgH;
    CLIRESEJ   ResMsgH;
	SHORT	   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQEJ));
    memset(&ResMsgH, 0, sizeof(CLIRESEJ));

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCEJCLEAR;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQEJ);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESEJ);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
*   Synopsis:  SHORT  STUBENTRY   SerEJReadFile(ULONG  ulOffset, 
*                                               VOID  FAR  *pData,
*                                               USHORT usDataLen,
*                                               USHORT FAR *pusRetLen)
*
*       Input:    ULONG      ulOffset - Read Pointer of EJ File
*            :    USHORT     usDataLen - Read Buffer Length
*       InOut:    USHORT     *usRetLen - Pointer of Readed Data Length 
*            :    VOID       *pData - Pointer of Buffer 
*
**  Return    :
*            Normal End: EJ_PERFORM
*                      : EJ_EOF
*          Abnormal End: EJ_FILE_NOT_EXIST       
*                      : EJ_DATA_TOO_LARGE
*
**  Description:
*               Read E.J. File Data from the EJ File.
*==========================================================================
*/
SHORT STUBENTRY   SerEJReadFile(ULONG  ulOffset,  VOID FAR  *pData,
                                ULONG usDataLen, ULONG FAR *pusRetLen)
{

    CLIREQEJ   ReqMsgH;
    CLIRESEJ   ResMsgH;
	SHORT	   sRetCode;

    if ( usDataLen < CLI_MAX_EJDATA ) {
        return (EJ_TOO_SMALL_BUFF);
    }

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQEJ));
    memset(&ResMsgH, 0, sizeof(CLIRESEJ));

	ReqMsgH.ulOffset = ulOffset;   

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCEJREADFILE;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQEJ);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESEJ);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if ( (EJ_PERFORM == CliMsg.sRetCode ) ||  
         (EJ_EOF     == CliMsg.sRetCode  )   ) {
        memcpy(pData, ResMsgH.auchEJData, ResMsgH.usDataLen);
        *pusRetLen = ResMsgH.usDataLen;
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;    
}

/*
*==========================================================================
*   Synopsis:   SHORT   SerEJNoOfTrans(VOID)
*
*       Input:    none
*       InOut:    none
**  Return      : status
*
**  Description: Get # of Transaction in E/J File.
*==========================================================================
*/
SHORT   STUBENTRY   SerEJNoOfTrans(VOID)
{

    CLIREQEJ   ReqMsgH;
    CLIRESEJ   ResMsgH;
	SHORT	   sRetCode;


    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQEJ));
    memset(&ResMsgH, 0, sizeof(CLIRESEJ));


    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCEJNOOFTRANS;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQEJ);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESEJ);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);

    return sRetCode;

}

/*
*==========================================================================
*   Synopsis:   SHORT   STUBENTRY   SerEJReadResetTransOver(ULONG FAR *pulOffset,
*                           VOID FAR *pData, USHORT usDataLen, USHORT FAR *pusRetLen)
*
*       Input:    ulOffset - Read Pointer of EJ File
*                 usDataLen - Read Buffer Length
*       InOut:    *usRetLen - Pointer of Readed Data Length
*                 *pData - Pointer of Buffer
**  Return      :
*   Normal End  :  # of Transaction
*               :  EJ_END
*
*   Abnormal End: EJ_FILE_NOT_EXIST
*               : EJ_TOO_SMALL_BUFF
*               : EJ_PROHIBIT
*
**  Description: Read/Reset Trans. Data in E/J File(Override Type).
*==========================================================================
*/
SHORT   STUBENTRY   SerEJReadResetTransOver(ULONG FAR *pulOffset,
            VOID FAR *pData, ULONG usDataLen, ULONG FAR *pusRetLen)
{

    CLIREQEJ   ReqMsgH;
    CLIRESEJ   ResMsgH;
	SHORT	   sRetCode;

    if ( usDataLen < CLI_MAX_EJDATA ) {
        return (EJ_TOO_SMALL_BUFF);
    }

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQEJ));
    memset(&ResMsgH, 0, sizeof(CLIRESEJ));

    ReqMsgH.ulOffset = *pulOffset;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCEJREADRESET_OVER;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQEJ);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESEJ);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if ( (EJ_END == CliMsg.sRetCode ) ||
         (0      <=  CliMsg.sRetCode  )   ) {
        memcpy(pData, ResMsgH.auchEJData, ResMsgH.usDataLen);
        *pusRetLen = ResMsgH.usDataLen;
        *pulOffset = ResMsgH.ulOffset;
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;


}

/*
*==========================================================================
*   Synopsis:   SHORT   STUBENTRY   SerEJReadResetTransStart(ULONG FAR *pulOffset,
*                           VOID FAR *pData, USHORT usDataLen, USHORT FAR *pusRetLen,
*                           USHORT FAR *pusNumTrans)
*
*       Input:    ulOffset     - Read Pointer of EJ File
*                 usDataLen    - Read Buffer Length
*       InOut:    *usRetLen    - Pointer of Readed Data Length
*                 *pData       - Pointer of Buffer
*       Output:   *pusNumTrans - No of Read Transaction
**  Return      : return status
*   Normal End  : EJ_CONTINUE
*               : EJ_END
*               : EJ_NOTHING_DATA
*   Abnormal End: EJ_FILE_NOT_EXIST
*               : EJ_TOO_SMALL_BUFF
*               : EJ_PROHIBIT
*
**  Description: Read/Reset Trans. Data in E/J File(Not Override).
*==========================================================================
*/
SHORT   STUBENTRY   SerEJReadResetTransStart(ULONG FAR *pulOffset,
            VOID FAR *pData, ULONG usDataLen, ULONG FAR *pusRetLen,
            USHORT FAR *pusNumTrans)
{
    CLIREQEJ   ReqMsgH;
    CLIRESEJ   ResMsgH;
	SHORT	   sRetCode;

    if ( usDataLen < CLI_MAX_EJDATA ) {
        return (EJ_TOO_SMALL_BUFF);
    }

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQEJ));
    memset(&ResMsgH, 0, sizeof(CLIRESEJ));

    ReqMsgH.ulOffset = *pulOffset;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCEJREADRESET_START;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQEJ);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESEJ);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if ( (EJ_CONTINUE == CliMsg.sRetCode ) ||
         (EJ_END     == CliMsg.sRetCode  )   ) {
        memcpy(pData, ResMsgH.auchEJData, ResMsgH.usDataLen);
        *pusRetLen = ResMsgH.usDataLen;
        *pulOffset = ResMsgH.ulOffset;
        *pusNumTrans = ResMsgH.usNumTrans;
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;

}

/*
*==========================================================================
*   Synopsis:   SHORT   STUBENTRY   SerEJReadResetTrans(ULONG FAR *pulOffset, VOID FAR *pData,
*                           USHORT usDataLen, USHORT FAR *pusRetLen, USHORT FAR *pusNumTrans)
*
*       Input:    ulOffset     - Read Pointer of EJ File
*                 usDataLen    - Read Buffer Length
*       InOut:    *usRetLen    - Pointer of Readed Data Length
*                 *pData       - Pointer of Buffer
*                 *pusNumTrans - In :No of Delete Transaction
*                                Out:No of Read Transaction
**  Return      : return status
*   Normal End  : EJ_CONTINUE
*               : EJ_END
*   Abnormal End: EJ_FILE_NOT_EXIST
*               : EJ_TOO_SMALL_BUFF
*               : EJ_PROHIBIT
*
**  Description: Read/Reset Trans. Data in E/J File(Not Override).
*==========================================================================
*/
SHORT   STUBENTRY   SerEJReadResetTrans(ULONG FAR *pulOffset, VOID FAR *pData,
            ULONG usDataLen, ULONG FAR *pusRetLen, USHORT FAR *pusNumTrans)
{

    CLIREQEJ   ReqMsgH;
    CLIRESEJ   ResMsgH;
	SHORT	   sRetCode;

    if ( usDataLen < CLI_MAX_EJDATA ) {
        return (EJ_TOO_SMALL_BUFF);
    }

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQEJ));
    memset(&ResMsgH, 0, sizeof(CLIRESEJ));

    ReqMsgH.ulOffset    = *pulOffset;
    ReqMsgH.usNumTrans  = *pusNumTrans;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCEJREADRESET;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQEJ);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESEJ);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    if ( (EJ_CONTINUE == CliMsg.sRetCode ) ||
         (EJ_END     == CliMsg.sRetCode  )   ) {
        memcpy(pData, ResMsgH.auchEJData, ResMsgH.usDataLen);
        *pusRetLen   = ResMsgH.usDataLen;
        *pulOffset   = ResMsgH.ulOffset;
        *pusNumTrans = ResMsgH.usNumTrans;
    }
    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/*
*==========================================================================
*   Synopsis:   SHORT   STUBENTRY   SerEJReadResetTransEnd(USHORT usNumTrans)
*
*       Input:    usNumTrans   - No of Delete Transaction
*       InOut:    none
**  Return      : status
*
**  Description: Delete Previous Trans. Data in E/J File(Not Override).
*==========================================================================
*/
SHORT   STUBENTRY   SerEJReadResetTransEnd(USHORT usNumTrans)
{

    CLIREQEJ   ReqMsgH;
    CLIRESEJ   ResMsgH;
	SHORT	   sRetCode;

    PifRequestSem(husCliExeNet);
    memset(&ReqMsgH, 0, sizeof(CLIREQEJ));
    memset(&ResMsgH, 0, sizeof(CLIRESEJ));

    ReqMsgH.usNumTrans  = usNumTrans;

    memset(&CliMsg, 0, sizeof(CLICOMIF));
    CliMsg.usFunCode    = CLI_FCEJREADRESET_END;
    CliMsg.pReqMsgH     = (CLIREQCOM *)&ReqMsgH;
    CliMsg.usReqMsgHLen = sizeof(CLIREQEJ);
    CliMsg.pResMsgH     = (CLIRESCOM *)&ResMsgH;
    CliMsg.usResMsgHLen = sizeof(CLIRESEJ);

    CstSendMaster();
	sRetCode = CliMsg.sRetCode;

    PifReleaseSem(husCliExeNet);
    return sRetCode;
}

/******** End of Source *************************/
