/*************************************************************************
 *
 * security.h	
 *
 * $Workfile:: security.h                                                $
 *
 * $Revision:: Initial                                                   $
 *
 * $Archive::                                                            $
 *
 * $Date:: 2/20/2003                                                     $
 *
 *************************************************************************
 *
 * Copyright (c) Georgia Sothern University 2003.  All rights reserved.
 *
 *************************************************************************
 *
 * $History:: security.h                                                $
 * Date :		Name :			Version :		Comments				$
 * Feb-20-03	Chris Wunn		Initial
 *
 ************************************************************************/
//prevent multiple inclusions	
//#ifndef SECURITY_H
#define SECURITY_H

/************************************************************************
*
*	Function Declarations	
*
************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

int secCompareSerials();  //compares EEPROM and user entered serial numbers
void secCnvrtKey2Serial(); //converts a key code into a serial number (debug use only)
void secCnvrtKey2Unlock(); //conversts key code into an unlock number (debug use only)
void secReadRegistry();	//reads all NHPOS software security values from NHPOS registy key
void secGenKeyCode();		//generates a key code based soley on the user entered serial number
void secCnvrtUnlock2Serial();//converts an unlock number to a serial number
int secUnlock();			//compares the user entered serial number and the S/N derived from unlock number
SHORT secKeyCodeRegWrite(); //write key code to registry for program 95

#ifdef __cplusplus
}
#endif

//#endif