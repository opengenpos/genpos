#if defined(POSSIBLE_DEAD_CODE)

// converted to POSSIBLE_DEAD_CODE by moving define CONSOLIMAXSIZE to ecr.h
// define ITEMMAXSIZE is not used anywhere anymore.
// as part of slimming down and eliminating as much source as possible.
//  Richard Chambers, Apr-21-2015

/*=======================================================================*\
||  Erectronic  Cash  Register  -  90                                    ||
||                                                                       ||
||        *=*=*=*=*=*=*=*=*                                              ||
||        *  217X         *             NCR Corporation, E&M OISO        ||
||        * << ECR-90 >>  *                                              ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992           ||
||    <|\/~               ~\/|>                                          ||
||   _/^\_                 _/^\_                Release 1.0              ||
||                                                                       ||
\*=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title              : Ecr-90 Header Definition                         :
:   Category           :                                                  :
:   Program Name       : STORAGE.H                                            :
:  ---------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments             :
:                      :          :              :                        :
:                      :          :              :                        :
:  ---------------------------------------------------------------------  :
:   Compile            : MS-C Ver. 6.00 A by Microsoft Corp.              :
:                                                                         :
:   Memory Model       : Midium Model                                     :
\*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

                                
#define		ITEMMAXSIZE			0x0600
#define		CONSOLIMAXSIZE		0x1000


#endif