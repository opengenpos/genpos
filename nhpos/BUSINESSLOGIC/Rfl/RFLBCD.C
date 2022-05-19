#if defined(POSSIBLE_DEAD_CODE)

/*
*===========================================================================
*
*   Copyright (c) 1996, NCR Corporation, E&M-OISO. All rights reserved.
*
*===========================================================================
* Title       : Convert Ascii to BCD
* Category    : Reentrant Functions Library, NCR 2170 GP R2.0 Application
* Program Name: RFLA2BCD.C
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.
* Memory Model: Medium Model
* Options     : /c /AM /W4 /G1s /Os /Zap
* --------------------------------------------------------------------------
* Abstract: The provided function names are as follows:
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.     Name          Description
* Apr-02-93  0.00.01  O.Nakada      Initial
* Apr-14-93           T.Koike       Add  "RflPLUAs2BCDRight","RflPLUBCD2AsRight
* Dec-05-96           S.Kubota      Add  "RflNCFAs2BCDRight","RflNCFBCD2AsRight
*                                   for Negative check file
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
*===========================================================================
*   Include File Declarations
*===========================================================================
*/
#include <tchar.h>
#include <string.h>
#include "ecr.h"
#include "rfl.h"
#include "paraequ.h"


/*
*===========================================================================
*   Compile Message Declarations
*===========================================================================
*/


/*
*===========================================================================
*   Work Area Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Public Work Area
*===========================================================================
*/


/*
*===========================================================================
*   Static Work Area
*===========================================================================
*/


/*
*===========================================================================
*   Code Area Declarations
*===========================================================================
*/
/*
*===========================================================================
** Synopsis:    VOID RflPLUAscii2BCD(UCHAR *puchBCD, UCHAR *pszAscii)
*
*    Output:    puchBCD  - pointer of BCD data
*                          +-+-+-+-+-+-+-+   +-+
*                          |0|?|?|?|?|?|?|...|?|
*                          +-+-+-+-+-+-+-+   +-+
*                           ^
*                           fill character
*
*     Input:    pszAscii - pointer of ASCIIZ string
*
** Return:      nothing
*
** Description:
*===========================================================================
*/
VOID RflPLUAscii2BCD(UCHAR *puchBCD, UCHAR *pszAscii)
{
    if (*pszAscii) {
        *puchBCD = 0x00;                                        /* high */

        do {
            *puchBCD++ |= *pszAscii++ - 0x30;                   /* low  */

            if (*pszAscii) {
                *puchBCD = (UCHAR)((*pszAscii++ - 0x30) << 4);  /* high */
            } else {
                break;
            }
        } while (*pszAscii);
    }
}


/*
*===========================================================================
** Synopsis:    VOID RflPLUBCD2Ascii(UCHAR *pszAscii, UCHAR *puchBCD)
*
*    Output:    pszAscii  - pointer of ASCIIZ string
*
*     Input:    puchBCD   - pointer of BCD data
*                          +-+-+-+-+-+-+-+   +-+
*                          |0|?|?|?|?|?|?|...|?|
*                          +-+-+-+-+-+-+-+   +-+
*                           ^
*                           skip character
*
** Return:      nothing
*
** Description:
*===========================================================================
*/
VOID RflPLUBCD2Ascii(UCHAR *pszAscii, UCHAR *puchBCD)
{
    UCHAR   cuchLoop = 6;

    /* ----- first byte ----- */
    *pszAscii++ = (UCHAR)((*puchBCD++ & 0x0F) + 0x30);          /* low  */

    while (cuchLoop--) {
        *pszAscii++ = (UCHAR)(((*puchBCD >> 4) & 0x0F) + 0x30); /* high */
        *pszAscii++ = (UCHAR)((*puchBCD++ & 0x0F)      + 0x30); /* low  */
    }

    *pszAscii = '\0';
}


/*
*===========================================================================
** Synopsis:    SHORT RflPLUAs2BCDRight(UCHAR *puchBCD, UCHAR *pszAscii)
*
*
*    Input:     pszAscii - pointer to Ascii-Z string area
*                             (max 13 characters + '0')
*                         +--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*                         |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
*                         +--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*
*
*    Output:    puchBCD  - pointer to 7 bytes BCD area
*                             (7 bytes fixed)
*                          +-+-+-+-+-+-+-+   +-+
*                          |0|?|?|?|?|?|?|...|?| Filled from right most byte
*                          +-+-+-+-+-+-+-+   +-+
*                           ^
*                           fill character
*
** Return:      RFL_SUCCESS : normal termination
*               RFL_FAIL    : error (input string exceeds 13 digits)
*
** Description:
*   Convert PLU Number (ASCII String) ---> BCD Data
*===========================================================================
*/
SHORT   RflPLUAs2BCDRight(UCHAR *puchBCD, UCHAR *pszAscii)
{
    SHORT   i,j;

    j=strlen(pszAscii);
    if((j>PLU_MAX_DIGIT)||(j==0))   /* over 13 digits or 0 ? */
        return RFL_FAIL;
    memset(puchBCD,0,(PLU_MAX_DIGIT+1)/2);  /* clear return memory */
    i=((PLU_MAX_DIGIT+1)/2)-1;

    for(j=j-1;j>=0;i--) {   /* j : offset to the last byte of input data */
        *(puchBCD+i) = (UCHAR)(*(pszAscii+j--) - 0x30); /* low  */
        if(j>=0) {
            *(puchBCD+i) |= (UCHAR)((*(pszAscii+j--) - 0x30) << 4);  /* high */
        } else {
            break;
        }
    }
    return RFL_SUCCESS;
}


/*
*===========================================================================
** Synopsis:    SHORT RflPLUBCD2AsRight(UCHAR *pszAscii, UCHAR *puchBCD)
*
*
*    Input:     puchBCD  - pointer to 7 bytes BCD area
*                             (7 bytes fixed)
*                          +-+-+-+-+-+-+-+   +-+
*                          |0|?|?|?|?|?|?|...|?| Filled from right most byte
*                          +-+-+-+-+-+-+-+   +-+
*
*
*    Output:    pszAscii - pointer to Ascii-Z string area
*                         (min 14 bytes area : Filled from left most byte)
*                         +--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*                         |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
*                         +--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*
** Return:      RFL_SUCCESS : normal termination
*               RFL_FAIL    : error (input string exceeds 13 digits)
*
** Description:
*   Convert PLU Number (BCD Data) ---> ASCII String
*   PLU label data is analysed "RflConvertPLU" routine.
*   The output area has to have at least 14 bytes.
*===========================================================================
*/
SHORT   RflPLUBCD2AsRight(TCHAR *pszAscii, UCHAR *puchBCD)
{
    USHORT  i,j;
    TCHAR   aszTemp[PLU_MAX_DIGIT+1];

    tcharnset(aszTemp,_T('0'),PLU_MAX_DIGIT); /* all brank */
    //memset(aszTemp,(UCHAR)'0',sizeof(aszTemp)); /* all brank */
    for(i=j=0;j<PLU_MAX_DIGIT+1;i++,j++) {      /* move pointer to non zero digit */
        if(*(puchBCD+i) & 0xf0)
            break;
        else {
            j++;
            if(*(puchBCD+i) & 0x0f)
                break;
        }
    }
    if(j>=PLU_MAX_DIGIT+1)  /* all 0 ? */
        return RFL_FAIL;

    if(j) j--;
    while(j<PLU_MAX_DIGIT) {        /* PLU number exist */
        if(j & 0x01) {              /* even or odd ? */
            aszTemp[j] = (TCHAR)(((*(puchBCD+i)&0xf0) >> 4) + '0');
        }
        else {                      /* even */
            aszTemp[j] = (TCHAR)((*(puchBCD+i)&0x0f) + '0');
            i++;
        }
        j++;
    }
    aszTemp[j--]='\0';
    RflConvertPLU(pszAscii, aszTemp);   /* analyses PLU type and convert */
    return RFL_SUCCESS;                 /* EAN-13/8, Velocity, UPC-A/E */
}
#if 0
/*
*===========================================================================
** Synopsis:    SHORT RflNCFAs2BCDRight(UCHAR *puchBCD, UCHAR *pszAscii)
*
*
*    Input:     pszAscii - pointer to Ascii-Z string area
*                             (max 25 characters + '0')
*                         +--+--+--+--+--+--+--+   +--+--+--+--+--+--+
*                         |  |  |  |  |  |  |  |...|  |  |  |  |  |00|
*                         +--+--+--+--+--+--+--+   +--+--+--+--+--+--+
*
*
*    Output:    puchBCD  - pointer to 13 bytes BCD area
*                             (13 bytes fixed)
*                          +-+-+-+-+-+-+-+   +-+
*                          |0|?|?|?|?|?|?|...|?| Filled from right most byte
*                          +-+-+-+-+-+-+-+   +-+
*                           ^
*                           fill character
*
** Return:      RFL_SUCCESS : normal termination
*               RFL_FAIL    : error (input string exceeds 25 digits)
*
** Description:
*   Convert NCF Number (ASCII String) ---> BCD Data
*===========================================================================
*/
SHORT   RflNCFAs2BCDRight(UCHAR *puchBCD, UCHAR *pszAscii)
{
    SHORT   i,j;

    j=strlen(pszAscii);
    if((j>NCF_MAX_DIGIT)||(j==0))   /* over 13 digits or 0 ? */
        return RFL_FAIL;
    memset(puchBCD,0,(NCF_MAX_DIGIT+1)/2);  /* clear return memory */
    i=((NCF_MAX_DIGIT+1)/2)-1;

    for(j=j-1;j>=0;i--) {   /* j : offset to the last byte of input data */
        *(puchBCD+i) = (UCHAR)(*(pszAscii+j--) - 0x30); /* low  */
        if(j>=0) {
            *(puchBCD+i) |= (UCHAR)((*(pszAscii+j--) - 0x30) << 4);  /* high */
        } else {
            break;
        }
    }
    return RFL_SUCCESS;
}


/*
*===========================================================================
** Synopsis:    SHORT RflNCFBCD2AsRight(UCHAR *pszAscii, UCHAR *puchBCD)
*
*
*    Input:     puchBCD  - pointer to 13 bytes BCD area
*                             (13 bytes fixed)
*                          +-+-+-+-+-+-+-+   +-+
*                          |0|?|?|?|?|?|?|...|?| Filled from right most byte
*                          +-+-+-+-+-+-+-+   +-+
*
*
*    Output:    pszAscii - pointer to Ascii-Z string area
*                         (min 26 bytes area : Filled from left most byte)
*                         +--+--+--+--+--+--+   +--+--+--+--+--+--+--+
*                         |  |  |  |  |  |  |...|  |  |  |  |  |  |  |
*                         +--+--+--+--+--+--+   +--+--+--+--+--+--+--+
*
** Return:      RFL_SUCCESS : normal termination
*               RFL_FAIL    : error (input string exceeds 25 digits)
*
** Description:
*   Convert NCF Number (BCD Data) ---> ASCII String
*===========================================================================
*/
SHORT   RflNCFBCD2AsRight(UCHAR *pszAscii, UCHAR *puchBCD)
{
    USHORT  i,j;
    UCHAR   aszTemp[NCF_MAX_DIGIT+1];



    memset(aszTemp,(UCHAR)'0',sizeof(aszTemp)); /* all brank */
    for(i=j=0;j<NCF_MAX_DIGIT+1;i++,j++) {      /* move pointer to non zero digit */
        if(*(puchBCD+i) & 0xf0)
            break;
        else {
            j++;
            if(*(puchBCD+i) & 0x0f)
                break;
        }
    }
    if(j>=NCF_MAX_DIGIT+1)  /* all 0 ? */
        return RFL_FAIL;

    if(j) j--;
    while(j<NCF_MAX_DIGIT) {        /* NCF number exist */
        if(j & 0x01) {              /* even or odd ? */
            aszTemp[j] = (UCHAR)(((*(puchBCD+i)&0xf0) >> 4) + '0');
        }
        else {                      /* even */
            aszTemp[j] = (UCHAR)((*(puchBCD+i)&0x0f) + '0');
            i++;
        }
        j++;
    }
    aszTemp[j--]='\0';

    memcpy(pszAscii,aszTemp,NCF_MAX_DIGIT+1);
    return RFL_SUCCESS;
}

#endif
/* ---------- End of RFLBCD.C ---------- */

#endif