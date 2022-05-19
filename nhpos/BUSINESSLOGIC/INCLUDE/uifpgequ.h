/*========================================================================*\
||                                                                        ||
||        *=*=*=*=*=*=*=*=*                                               ||
||        *  NCR 2170     *             NCR Corporation, E&M OISO         ||
||     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            ||
||    <|\/~               ~\/|>                                           ||
||   _/^\_                 _/^\_                Release 1.0               ||
||                                                                        ||
\*========================================================================*/
/*------------------------------------------------------------------------*\
:   Title              : NCR 2170 Header Definition                        :
:   Category           : SUPER & PROGRAM MODE,ECR 2170                     :
:   Program Name       : uifpguni.h                                         :
:  ----------------------------------------------------------------------  :
:                      : YY-MM-DD   Ver.Rev          Comments              :
:   Program Written    : 92-05-07 : 00.00.00     :                         :
:   Update Histories   :          :              :                         :
:                      :          :              :                         :
:  ----------------------------------------------------------------------  :
:   Compile            : MS-C Ver. 6.00A by Microsoft Corp.                :
:   Memory Model       : Midium Model                                      :
:   Condition          :                                                   :
\*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*------------------------------------------------------------------------*/
/**
==============================================================================
;                      I N C L U D E     F I L E s                         
;=============================================================================
**/

/*------------------------------------------------------------------------*\

            E U A T I O N     D E F I N I T I O N 

\*------------------------------------------------------------------------*/

/* Define Digit */
                
#define UIF_DIGIT1              1               /* 1 Digit */ 
#define UIF_DIGIT2              2               /* 2 Digit */ 
#define UIF_DIGIT3              3               /* 3 Digit */ 
#define UIF_DIGIT4              4               /* 4 Digit */ 
#define UIF_DIGIT5              5               /* 5 Digit */ 
#define UIF_DIGIT6              6               /* 6 Digit */ 
#define UIF_DIGIT7              7               /* 7 Digit */ 
#define UIF_DIGIT8              8               /* 8 Digit */ 
#define UIF_DIGIT9              9               /* 9 Digit */ 
#define UIF_DIGIT10             10              /* 10 Digit */ 
#define UIF_DIGIT12             12              /* 12 Digit : KDS IP address entry, 4 three digit numbers */
#define UIF_DIGIT13             13              /* 13 Digit : for PLU# entry (KEY). max 14 digit bar code for RSS, UPC, etc., check digit calculated, LABEL_RSS14 */
#define UIF_DIGIT14             14              /* 14 Digit : for PLU# entry (SCANNER). max 14 digit bar code for RSS, UPC, etc., with check digit, LABEL_RSS14 */
#define UIF_DIGIT24				24				// 24 Digit # entry
#define UIF_DIGIT74				74				// RSS14 Expaned up to 74 Numbers
#define UIF_DIGIT77				77				// RSS14 Expaned + 3 character prefix 

/* Define KB Type */

#define UIF_ALPHA_KB            0x0001          /* ALPHA KB Type */
#define UIF_NEUME_KB            0x0002          /* NEUMERIC KB Type */
