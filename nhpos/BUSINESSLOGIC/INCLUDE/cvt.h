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
* Title       : Conversion Table Header File
* Category    : NCR 2170 US Hospitality Application         
* Program Name: CVT.H                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /Gs /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Date  :Ver.Rev.:   Name    : Description
* May-14-92:00.00.01:M.Suzuki   :                                    
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


/*                                                                                
*===========================================================================
*   Include File Declarations
*===========================================================================
*/

/*
*===========================================================================
*   Define Declarations 
*===========================================================================
*/


/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/

// As new keyboards are implemented, programmers will need to create a
// keyboard translation table for the new keyboard type.
// In the files which use keyboard type to select a new keyboard translation
// table (see files uifpgmod.c and uifacmod.c) the following typedefs are used
// to create keyboard selection tables which are then used by a set of functions
// to call UieSetCvt() with the appropriate translation table.

// The following keyboard table struct is used to set the keyboard translation
// to use the appropriate Alphanumeric or Numeric keyboard translation table.
// This table is used by static function Set_CVT_FromTable() which is embedded
// in any file using this functionality.  See files uifpgmod.c, uifacmod.c,
// uifrgdef.c, and uidcp.c all of which contain table lookup functionality.
//    Set_CVT_FromTable (KeyboardTypeLookupTable *pTable,int kbtype);
typedef struct {
	int      iKbType;
	CVTTBL  *pCvTble;
}  KeyboardTypeLookupTable;


// The following keyboard table struct is used to set the keyboard translation
// to use the appropriate Alphanumeric keyboard translation table.  This struct
// is used to indicate which keyboard translation table to use depending on
// whether a keyboard shift operation is in force or not.
// This table is used by static function Set_CVT_FromTablePgControl() which is
// embedded in any file using this functionality.  See files uifpgmod.c and
// uifacmod.c
//    Set_CVT_FromTablePgControl (KeyboardTypeLookupTablePgControl *pTable, int kbtype);
typedef struct {
	int      iKbType;
	CVTTBL  *pCvTbleOff;          // UIF_SHIFT_PAGE of usUifPGControl is NOT turned on
	CVTTBL  *pCvTbleOn;           // UIF_SHIFT_PAGE of usUifPGControl is turned on
}  KeyboardTypeLookupTablePgControl;


#if 0
// Typical code used with the above table structs.
// This is sample code only and should remain uncompiled hence the #if 0

// The following keyboard table is used to set the keyboard translation
// to use the appropriate Numeric keyboard translation table.
static KeyboardTypeLookupTable KeyboardTypesNumeric [] = {
	{KEYBOARD_CONVENTION, CvtProgC},         // 7448 Conventional Type
	{KEYBOARD_MICRO,      CvtProgM},         // 7448 Micromotion keyboard
	{KEYBOARD_TOUCH,      CvtProgTouch},     // Touchscreen terminal uses Micromotion tables
	{KEYBOARD_WEDGE_68,   CvtProg5932_68},   // 68 key 5932 Wedge keyboard
	{0, NULL}
};
// The following functions are provided to illustrate the code to traverse the
// KeyboardTypeLookupTable table looking for a particular keyboard type
static SHORT Set_CVT_FromTable (KeyboardTypeLookupTable *pTable, int kbtype)
{
	SHORT  sStatusResult = -1;

	if (pTable) {
		for (; pTable->pCvTble; pTable++)
		{
			if (pTable->iKbType == kbtype)
			{
				UieSetCvt(pTable->pCvTble);
				sStatusResult = 0;
				break;
			}
		}

		NHPOS_ASSERT_TEXT(pTable->pCvTble, "Keyboard type not found.");
	}

	return sStatusResult;
}


// The following keyboard table is used to set the keyboard translation
// to use the appropriate Alphanumeric keyboard translation table when
// doing letter case changes as if using the shift key to change letter case.
// The difference between this table and KeyboardTypesAlpha above is this
// table is used by function Set_CVT_FromTablePgControl() which also
// uses a globabl variable to keep page control information.  For an
// example see function UifPGShift().
static KeyboardTypeLookupTablePgControl KeyboardTypesNumeric2 [] = {
	{KEYBOARD_CONVENTION, CvtAlphC2,        CvtAlphC1},         // 7448 Conventional Type
	{KEYBOARD_WEDGE_68,   CvtAlph5932_68_2, CvtAlph5932_68_1},  // 68 key 5932 Wedge keyboard
	{0, NULL, NULL}
};

// The following functions are provided to illustrate the code to traverse the
// KeyboardTypeLookupTablePgControl table looking for a particular keyboard type

#define SETCVT_USUIFPGCONTROL  usUifPGControl

static SHORT Set_CVT_FromTablePgControl (KeyboardTypeLookupTablePgControl *pTable, int kbtype)
{
	SHORT  sStatusResult = -1;

	if (pTable) {
		for (; pTable->pCvTbleOn; pTable++)
		{
			if (pTable->iKbType == kbtype)
			{
				if (SETCVT_USUIFPGCONTROL & UIF_SHIFT_PAGE) {
					UieSetCvt(pTable->pCvTbleOn);                // use standard keyboard table
					SETCVT_USUIFPGCONTROL &= ~UIF_SHIFT_PAGE;    // RESET or turn OFF Shift Page Bit
				}
				else {
					UieSetCvt(pTable->pCvTbleOff);               // use shift keyboard table
					SETCVT_USUIFPGCONTROL |= UIF_SHIFT_PAGE;     // SET or turn ON Shift Page Bit */
				}
				sStatusResult = 0;
				break;
			}
		}

		NHPOS_ASSERT_TEXT(pTable->pCvTbleOn, "Keyboard type not found.");
	}

	return sStatusResult;
}

#endif

/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/


/*
*===========================================================================
*   External Work Area Declarations
*===========================================================================
*/
extern CVTTBL FARCONST CvtRegM[];    // NCR 7448 POS terminal with MicroMotion keyboard
extern CVTTBL FARCONST CvtRegC[];    // NCR 7448 POS terminal with Conventional keyboard
extern CVTTBL FARCONST CvtRegTouch[];	//Touchscreen conversion table
extern CVTTBL FARCONST CvtReg5932_68[];   // NCR 5932 Wedge 64 key keyboard
extern CVTTBL FARCONST CvtReg5932_78[];   // NCR 5932 Wedge 78 key keyboard
extern CVTTBL FARCONST CvtProgM[];    // NCR 7448 POS terminal with MicroMotion keyboard
extern CVTTBL FARCONST CvtProgC[];    // NCR 7448 POS terminal with Conventional keyboard
extern CVTTBL FARCONST CvtProgTouch[];	//Touchscreen conversion table
extern CVTTBL FARCONST CvtProg5932_68[];	//NCR 5932 Wedge 64 key keyboard
extern CVTTBL FARCONST CvtProg5932_78[];	//NCR 5932 Wedge 78 key keyboard
extern CVTTBL FARCONST CvtAlphM[];    // NCR 7448 POS terminal with MicroMotion keyboard
extern CVTTBL FARCONST CvtAlphC1[];    // NCR 7448 POS terminal with Conventional keyboard
extern CVTTBL FARCONST CvtAlphC2[];    // NCR 7448 POS terminal with Conventional keyboard
extern CVTTBL FARCONST CvtAlph5932_68_1[];	//NCR 5932 Wedge 64 key keyboard
extern CVTTBL FARCONST CvtAlph5932_68_2[];	//NCR 5932 Wedge 64 key keyboard
extern CVTTBL FARCONST CvtAlph5932_78_1[];	//NCR 5932 Wedge 78 key keyboard
extern CVTTBL FARCONST CvtAlph5932_78_2[];	//NCR 5932 Wedge 78 key keyboard
extern CVTTBL FARCONST CvtRegTicket[];	//Touchscreen conversion table
