#include <tchar.h>
#include <windows.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
//#include <time.h>
#include <ecr.h>
#include <pif.h>
#include <appllog.h>
#include <rfl.h>
#include "PluTtlD.h"

//extern UCHAR FARCONST  auchNEW_FILE_WRITE[];
static UCHAR FARCONST	aReadOnly[]  = "ro";				/* open w/ read mode	*/
static UCHAR FARCONST	aReadWrite[] = "rwo";				/* open w/ read & write	*/
static UCHAR FARCONST	aCreateNew[] = "rwn";				/* create newly			*/

PLUTOTAL_EXTERN_C	PLUTOTAL_DECLSPEC VOID 	PLUTTLAPI  PluTotalPutLog_Dbg(LPCTSTR tMsg1,LPCTSTR tMsg2,LPCTSTR tMsg3);
void	__PutLog2(int mode);

VOID	__PluCD(TCHAR *PluNo)
{
    SHORT    i,sEven,sOdd,cdigit;

    for (sEven = sOdd = i = 0 ; i < 12 ; ++i) {
        if (i % 2) {
            sOdd  += (SHORT)(PluNo[i]&0x0f);        /* odd addr, Saratoga */
        } else {
            sEven += (SHORT)(PluNo[i]&0x0f);        /* even addr, Saratoga */
        }
    }
    cdigit = (SHORT)((sEven + sOdd*3) % 10);        /* Saratoga */
    cdigit = (SHORT)((10 - cdigit) % 10);           /* Saratoga */
    PluNo[12] = (UCHAR)(cdigit | 0x30);              /* return CD about plu# */
}


LONG	__GetNum_fromMnemonic(TCHAR * pMnemonic){
	TCHAR	buf[10];
	//char	buf[10];
	_tcsncpy(buf,&pMnemonic[5],5);
	//memcpy(buf,&pMnemonic[5],5);
	buf[5] = (TCHAR)0;
	//buf[5] = (char)0;
	return	(LONG)_ttoi(buf);
}

void	__GenPluNo(LONG lNo,TCHAR * pPluNo){
	TCHAR	tBuf[20];
	wsprintf(tBuf,TEXT("000%09lu"),lNo);
//#ifdef _WIN32_WCE
//	wcstombs(pPluNo,tBuf,12);
//#else
	_tcsncpy(pPluNo,tBuf,12);
	//memcpy(pPluNo,tBuf,12);
//#endif
	__PluCD(pPluNo);
}

BOOL	CopyFilesTest(int mode){
	BOOL	bSts1,bSts2;

	__PutLog2(1);
	switch(mode){
		case	1:
			bSts1 = CopyFile(TEXT("\\TempDisk\\T3MD.bin"),TEXT("\\Flashdisk\\Daily.bin"),FALSE);
			bSts2 = CopyFile(TEXT("\\TempDisk\\T3MP.bin"),TEXT("\\Flashdisk\\PTD.bin"),FALSE);
			break;
		case	2:
			bSts1 = CopyFile(TEXT("\\TempDisk\\T2MD.bin"),TEXT("\\Flashdisk\\Daily.bin"),FALSE);
			bSts2 = CopyFile(TEXT("\\TempDisk\\T2MP.bin"),TEXT("\\Flashdisk\\PTD.bin"),FALSE);
			break;
		case	3:
			bSts1 = CopyFile(TEXT("\\TempDisk\\T1MD.bin"),TEXT("\\Flashdisk\\Daily.bin"),FALSE);
			bSts2 = CopyFile(TEXT("\\TempDisk\\T1MP.bin"),TEXT("\\Flashdisk\\PTD.bin"),FALSE);
			break;
		case	4:
			bSts1 = CopyFile(TEXT("\\TempDisk\\T500KD.bin"),TEXT("\\Flashdisk\\Daily.bin"),FALSE);
			bSts2 = CopyFile(TEXT("\\TempDisk\\T500KP.bin"),TEXT("\\Flashdisk\\PTD.bin"),FALSE);
			break;
		default:
			__PutLog2(4);
			return FALSE;
	}
	if(bSts1 && bSts2){
		__PutLog2(2);
		return	TRUE;
	}
	else{
		__PutLog2(3);
		return	FALSE;
	}
}

void	__PutLog2(int mode){

	switch(mode){
	case 1:
		PluTotalPutLog_Dbg(TEXT("### FileCopy START\n"),TEXT(" "),TEXT(" "));
		break;
	case 2:
		PluTotalPutLog_Dbg(TEXT("### Completed!\n"),TEXT(" "),TEXT(" "));
		break;
	case 3:
		PluTotalPutLog_Dbg(TEXT("### Failed!!!\n"),TEXT(" "),TEXT(" "));
		break;
	case 4:
		PluTotalPutLog_Dbg(TEXT("### PLU mnemonic error.\n"),TEXT(" "),TEXT(" "));
		break;
	}
}

void	__PutLog(int mode,long lCnt,char * szMsg){
	TCHAR	tBuf[255];

	switch(mode){
	case 1:
		PluTotalPutLog_Dbg(TEXT("### TEST2 START\n"),TEXT(" "),TEXT(" "));
		break;
	case 2:
		PluTotalPutLog_Dbg(TEXT("### TEST2 END\n"),TEXT(" "),TEXT(" "));
		break;
	case 3:
		wsprintf(tBuf,TEXT("TEST2 [%ld] \n "),lCnt);
		PluTotalPutLog_Dbg(tBuf,TEXT(" "),TEXT(" "));
		break;
	}

#ifdef	XXX_XXX
	USHORT		hf;
	SYSTEMTIME	stm;
	TCHAR	tBuf[256],tTim[100];
	UCHAR	aBuf[512];
	char	fname[] = {"T2log.txt"};
	GetLocalTime(&stm);
	wsprintf(tTim,TEXT("%02d/%02d   %02d:%02d:%02d.%03d"),stm.wMonth,stm.wDay,stm.wHour,stm.wMinute,stm.wSecond,stm.wMilliseconds);
	switch(mode){
		case	1:
			//if ((SHORT)(hf = PifOpenFile(fname, auchNEW_FILE_WRITE)) < 0)
			//	if((SHORT)(hf = PifOpenFile(fname, auchOLD_FILE_WRITE)) < 0)
			if ((SHORT)(hf = PifOpenFile(fname, aCreateNew)) < 0){
				if((SHORT)(hf = PifOpenFile(fname, aReadWrite)) < 0)
					return;
			}
			wsprintf(tBuf,TEXT("### TEST2 START %s \n") , tTim);
			wcstombs(aBuf,tBuf,sizeof(aBuf));
			PifWriteFile(hf, aBuf, (SHORT)strlen(aBuf));
			break;
		case	2:
			if((SHORT)(hf = PifOpenFile(fname, aReadWrite)) < 0)
				return;
			wsprintf(tBuf,TEXT("### TEST2 END %s \n") , tTim);
			wcstombs(aBuf,tBuf,sizeof(aBuf));
			PifWriteFile(hf, aBuf, (SHORT)strlen(aBuf));
			break;
		case	3:
			if((SHORT)(hf = PifOpenFile(fname, aReadWrite)) < 0)
				return;
			wsprintf(tBuf,TEXT("### TEST2     %s  [%06ld]  \n") , tTim,lCnt);
			wcstombs(aBuf,tBuf,sizeof(aBuf));
			PifWriteFile(hf, aBuf, (SHORT)strlen(aBuf));
			PifCloseFile(hf);
		default:
			return;
	}
#endif
//	wcstombs(aBuf,tBuf,sizeof(aBuf));
//	PifWriteFile(hf, aBuf, (SHORT)strlen(aBuf));
//	PifCloseFile(hf);
}
