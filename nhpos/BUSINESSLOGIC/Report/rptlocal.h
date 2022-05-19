/*
****************************************************************************
**							 2004, GSU Group                              **
**	
****************************************************************************
*===========================================================================
* Title       : Report Include file, for use with AC 888.                        
* Category    : Report For Supervisor, NCR 2170 US Hospitality Application         
* Program Name: RPTLOCAL.H
*/
/*

typedef struct  {
	TCHAR Name[260];
	TCHAR Version[15];
}DLL_EXEINFO;


typedef struct  {
	TCHAR		OperatingSystem[24];
	TCHAR		ServicePack[36];
	ULONG		Version;
	DLL_EXEINFO	Information[19];    
}PARANHPOSSYSTEM;
*/

BOOL GetNHPOSSystemInformation(TCHAR *pInfoBuffer);