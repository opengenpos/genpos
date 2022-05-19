/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1995            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Function Selection Code, Header File
* Category    : NCR 2170 Application         
* Program Name: FSC.H
* --------------------------------------------------------------------------
* Abstract:
*
* --------------------------------------------------------------------------
* Update Histories
*
* Date       Ver.      Name         Description
* Mar-20-92  00.00.01  O.Nakada     Initial.
* Sep-29-92  00.01.05  O.Nakada     Added FSC_TRACK_SWITCH and
*                                   FSC_TRACK_FUNC.
* Oct-19-92  00.01.08  O.Nakada     Added FSC_ERROR.
* Nov-25-92  01.00.00  hkato        Added FSC_TARE. 
* Jul-20-93  01.00.12  kyou         Added US Enhance feature. 
* Feb-23-95  03.00.00  hkato        R3.0
* Mar-27-95  03.00.00  O.Nakada     Added FSC_PAUSE, FSC_AN, FSC_MODE and
*                                   FSC_KEYBOARD.
* Apr-06-95  03.00.00  M.Suzuki     Added FSC_KEYED_STRING, FSC_STRING,
*                                   FSC_FORWARD, FSC_BACKWARD, FSC_CALL_STRING.
* Jul-19-95  03.00.00  hkato        Add FSC_ALPHA, FSC_LEFT_DISPLAY,
*                                   Drop FSC_FORWARD, FSC_BACKWARD
* Nov-08-95  03.01.00  hkato        R3.1
* Feb-14-96  03.01.00  O.Nakada     Add FSC_BEVERAGE.
* Jun-06-02  03.03.00  M.Ozawa      Add FSC_INTERRUPT
*** NHPOS 1.4.x, NCR 7448  ***
* Aug-01-02  NHPOS 1.4 R.Chambers   Product donated to Georgia Southern University
*** NHPOS 2.0.x Touchscreen ***
* Mar-01-04  NHPOS 2.0 R.Chambers   Conversion from NCR 7448 to Touchscreen terminals
*** GenPOS 2.2.x Touchscreen ***
* Nov-19-14  GenPOS 2.2 R.Chambers  Source cleanup to remove old, before NCR 7448,
*                                   code that applied to NCR 2170 back in the 1990s.
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
*   Define Declarations 
*===========================================================================
*/
/*
*===========================================================================
*   Reserved
*===========================================================================
*/
/*
=== Reserved ===                               	There is must not be Changed.
#define PIF_KEYERROR        0x00                Keyboard Error 

#define PIF_KEYLOCK         0x01                Key Lock Position
#define PIF_KEYLOCK + 1     0x02                Key Lock Position
#define PIF_KEYLOCK + 2     0x03                Key Lock Position
#define PIF_KEYLOCK + 3     0x04                Key Lock Position
#define PIF_KEYLOCK + 4     0x05                Key Lock Position
#define PIF_KEYLOCK + 6     0x06                Key Lock Position
=== Reserved ===
*/


/*
*===========================================================================
*   Fix Key Code
*===========================================================================
*/
#define FSC_BS              0x08                /* Back Space Key               */

#define FSC_TRACK_SWITCH    0x0E                /* Control Code of Key Track    */
#define FSC_TRACK_FUNC      0x0F                /* Control Code of Key Track    */

#define FSC_ERROR           0x10                /* Key Sequence Error           */
#define FSC_DOUBLE_WIDE     0x12                /* Double Wide Key. See also UIE_DOUBLE_WIDE indicator. See as well RFL_DOUBLE  */
#define FSC_PAUSE           0x13                /* Pause Key.  See function UifRegPauseString() */
#define FSC_AN              0x14                /* Alpha/Numeric Key            */
#define FSC_CALL_STRING     0x15                /* Call another control string Key */

//QTY GUARD RPH 2-2-4
#define FSC_QTY_GUARD		0x16				//Quantity Guard for touchscreen
#define FSC_PRICE_GUARD		0x17				//Price Guard for touchscreen
#define FSC_CNTRL_STING_GRD 0x18				//Quard for Control String
#define FSC_CNTRL_STRING_EVENT  0x19            // wait for an event to continue execution of the control string

#define FSC_CLEAR           0x1B                /* Clear key                    */
#define FSC_AUTO_OUT		167					// Auto Sign Out

// following special keys are used with Alphanumeric keyboard entry as well as PROG Mode entry.
#define FSC_P1              0x21                /* P1 Key, put away key - similar behavior as #/Type */
#define FSC_P2              0x22                /* P2 Key                       */
#define FSC_P3              0x23                /* P3 Key                       */
#define FSC_P4              0x24                /* P4 Key                       */
#define FSC_P5              0x25                /* P5 Key                       */


#define FSC_SCANNER         0x28
#define FSC_MSR             0x29
#define FSC_MODE            0x2A
#define FSC_KEYBOARD        0x2B
#define FSC_RESET_LOG       0x2D                /* PC i/F Reset Log,    V3.3 */
#define FSC_BEVERAGE        0x2E                /* Beverage Dispenser           */
#define FSC_WAITER_PEN      0x2F                /* Waiter Pen / Lock            */
#define FSC_POWER_DOWN      0x30                /* Power Down Detected, Saratoga */
#define FSC_VIRTUALKEYEVENT 0x31                // PIF_VIRTUALKEYEVENT
#define FSC_PINPAD          0x32                // PIF_VIRTUALKEYEVENT
#define FSC_SIGNATURECAPTURE    0x33            // PIF_VIRTUALKEYEVENT
#define FSC_OPERATOR_ACTION     0x34            // UIE_OPERATORACTION

//  defined codes for the various types of events indicated by FSC_CNTRL_STRING_EVENT
//  in control string using Pause messages used with function UifSignalStringWaitEvent()
//  to indicate a particular event has triggered.
#define CNTRL_STRING_EVENT_CLEAR_QUEUE       0    // indicates that the wait event queue should be cleared and any waits provided an event
#define CNTRL_STRING_EVENT_VALID_EVENT     100    // event number below this are not valid events.
#define CNTRL_STRING_EVENT_FORWARD_DONE    101    // indicates that a Forwarding operation of the Store and Forward functionality is complete
#define CNTRL_STRING_EVENT_WINDOW_INPUT    102    // indicates that either an AC key or a #/Type button was pressed in a displayed window

/*
=== Reserved ===                                There is must not be Changed.
#define FSC_UNSOLI          0x2C                / Unsoli. Report      /
=== Reserved ===
*/

/*
*===========================================================================
*  This FSC is different from the FSC code which the user input
*  in the program mode.
*  This FSC is a value by which 0x40 was added to actual FSC.
*===========================================================================
*/
/*
*===========================================================================
*   Function Selection Code
*===========================================================================
*/
#define UIE_FSC_BIAS        0x40


/*
*===========================================================================
*   Function Selection Code
*
*   Function Selection Codes (FSCs) are added in the following steps:
*      - create the define in the list below for the new FSC code
*      - add the new FSC to the list in the file Uifreg\CVTREGTOUCH.C
*      - add the new FSC to the list in the file Para\uniniram.c
*
*  For an example do a find on the FSC define FSC_SUPR_INTRVN to see various
*  places that may need to be changed.
*
*  See function UieConvertFsc() for how the device keypress data is translated
*  into an FSC value by doing a lookup of the keypress data.
*===========================================================================
*/
#define FSC_NO_FUNC         UIE_FSC_BIAS + 0    /* No Function                  */
#define FSC_KEYED_PLU       UIE_FSC_BIAS + 1    /* Keyed PLU Key                */
#define FSC_KEYED_DEPT      UIE_FSC_BIAS + 2    /* Keyed Department Key         */
#define FSC_PRINT_MODIF     UIE_FSC_BIAS + 3    /* Print Modifier               */
#define FSC_ADJECTIVE       UIE_FSC_BIAS + 4    /* Adjective Key                */
#define FSC_TENDER          UIE_FSC_BIAS + 5    /* Tender Key                   */
#define FSC_FOREIGN         UIE_FSC_BIAS + 6    /* Foreign Currency Key         */
#define FSC_TOTAL           UIE_FSC_BIAS + 7    /* Total Key                    */
#define FSC_ADD_CHECK       UIE_FSC_BIAS + 8    /* Add Check Key                */
#define FSC_ITEM_DISC       UIE_FSC_BIAS + 9    /* Item Discount Key            */
#define FSC_REGULAR_DISC    UIE_FSC_BIAS + 10   /* Regular Discount Key         */
#define FSC_TAX_MODIF       UIE_FSC_BIAS + 11   /* Tax Modifier Key             */
#define FSC_SIGN_IN         UIE_FSC_BIAS + 12   /* Sign-In / A / Super # Key    */
#define FSC_B               UIE_FSC_BIAS + 13   /* B Key                        */
#define FSC_WAITER          UIE_FSC_BIAS + 14   /* Waiter No.                   */
#define FSC_NEW_CHK         UIE_FSC_BIAS + 15   /* New Check                    */
#define FSC_PREVIOUS        UIE_FSC_BIAS + 16   /* Previous Balance             */
#define FSC_GUEST_CHK_NO    UIE_FSC_BIAS + 17   /* Guest Check                  */
#define FSC_GUEST_CHK_TRN   UIE_FSC_BIAS + 18   /* Guest Check Transfer         */
#define FSC_NUM_PERSON      UIE_FSC_BIAS + 19   /* Number of Person             */
#define FSC_TABLE_NO        UIE_FSC_BIAS + 20   /* Table No.                    */
#define FSC_LINE_NO         UIE_FSC_BIAS + 21   /* Line No.                     */
#define FSC_FEED_REL        UIE_FSC_BIAS + 22   /* Feed / Release               */
#define FSC_RECEIPT         UIE_FSC_BIAS + 23   /* Receipt / No Receipt         */
#define FSC_TIPS_PAID       UIE_FSC_BIAS + 24   /* Tips Paid Out                */
#define FSC_DECLAR_TIPS     UIE_FSC_BIAS + 25   /* Declared Tips                */
#define FSC_NO_SALE         UIE_FSC_BIAS + 26   /* No Sale                      */
#define FSC_PAID_OUT        UIE_FSC_BIAS + 27   /* Paid Out                     */
#define FSC_ROA             UIE_FSC_BIAS + 28   /* Received on Account          */      
#define FSC_COMPUT          UIE_FSC_BIAS + 29   /* Change Computation           */
#define FSC_CANCEL          UIE_FSC_BIAS + 30   /* Cancel / Abort               */
#define FSC_VOID            UIE_FSC_BIAS + 31   /* Void                         */
#define FSC_EC              UIE_FSC_BIAS + 32   /* E/C                          */
#define FSC_PRE_VOID        UIE_FSC_BIAS + 33   /* Preselect Void               */
#define FSC_QTY             UIE_FSC_BIAS + 34   /* Qty                          */
#define FSC_MENU_SHIFT      UIE_FSC_BIAS + 35   /* Menu Shift                   */
#define FSC_NUM_TYPE        UIE_FSC_BIAS + 36   /* #/Type, alphanumeric keyboard uses FSC_P1 */
#define FSC_PRT_DEMAND      UIE_FSC_BIAS + 37   /* Print on Demand              */
#define FSC_RESERVE1        UIE_FSC_BIAS + 38   /* Reserved                     */
#define FSC_CHARGE_TIPS     UIE_FSC_BIAS + 39   /* Charge Tips                  */
#define FSC_PLU             UIE_FSC_BIAS + 40   /* PLU No.                      */
#define FSC_00              UIE_FSC_BIAS + 41   /* 00 Key                       */
#define FSC_AC              UIE_FSC_BIAS + 42   /* Action Code Key              */
#define FSC_RECEIPT_FEED    UIE_FSC_BIAS + 43   /* Receipt Feed                 */
#define FSC_JOURNAL_FEED    UIE_FSC_BIAS + 44   /* Journal Feed                 */
#define FSC_DECIMAL         UIE_FSC_BIAS + 45   /* Decimal Point                */
#define FSC_TARE            UIE_FSC_BIAS + 46   /* Scale/tare key               */
#define FSC_OFFTEND_MODIF   UIE_FSC_BIAS + 47   /* Off Line Tender Modifier Key */
#define FSC_RESERVE2        UIE_FSC_BIAS + 48   /* reserved                     */
#define FSC_MONEY           UIE_FSC_BIAS + 49   /* Money key, 2172              */
#define FSC_RESERVE4        UIE_FSC_BIAS + 50   /* reserved                     */
#define FSC_RESERVE5        UIE_FSC_BIAS + 51   /* reserved                     */
#define FSC_REVERSE_PRINT   UIE_FSC_BIAS + 52   /* Reverse Print Key            */
#define FSC_PRESET_AMT      UIE_FSC_BIAS + 53   /* Preset Amount Key            */
#define FSC_D_MENU_SHIFT    UIE_FSC_BIAS + 54   /* Direct Menu Shift Key        */
#define FSC_TIME_IN         UIE_FSC_BIAS + 55   /* Time In Key                  */
#define FSC_TIME_OUT        UIE_FSC_BIAS + 56   /* Time Out Key                 */
#define FSC_KEYED_COUPON    UIE_FSC_BIAS + 57   /* Keyed Coupon Key             */
#define FSC_COUPON          UIE_FSC_BIAS + 58   /* Coupon No Key                */
#define FSC_SCROLL_UP       UIE_FSC_BIAS + 59   /* Scroll-Up Key                */
#define FSC_SCROLL_DOWN     UIE_FSC_BIAS + 60   /* Scroll-Down Key              */
#define FSC_RIGHT_ARROW     UIE_FSC_BIAS + 61   /* Right Arrow Key              */
#define FSC_LEFT_ARROW      UIE_FSC_BIAS + 62   /* Left Arrow Key               */
#define FSC_WAIT            UIE_FSC_BIAS + 63   /* Wait Key                     */
#define FSC_PAID_RECALL     UIE_FSC_BIAS + 64   /* Paid Order Recall Key        */
#define FSC_KEYED_STRING    UIE_FSC_BIAS + 65   /* Keyed Control String Key     */
#define FSC_STRING          UIE_FSC_BIAS + 66   /* Control String No Key        */
#define FSC_ALPHA           UIE_FSC_BIAS + 67   /* Alpha Entry Key              */
#define FSC_LEFT_DISPLAY    UIE_FSC_BIAS + 68   /* Cursor on Left Display       */
#define FSC_BAR_SIGNIN      UIE_FSC_BIAS + 69   /* Bartender or surrogate Sign-in Key,  R3.1 */
#define FSC_SPLIT           UIE_FSC_BIAS + 70   /* Split Key,              R3.1 */
#define FSC_SEAT            UIE_FSC_BIAS + 71   /* Seat No Key,            R3.1 */
#define FSC_TRANSFER        UIE_FSC_BIAS + 72   /* Transfer Key,           R3.1 */
#define FSC_REPEAT          UIE_FSC_BIAS + 73   /* Repeat Key,             R3.1 */
#define FSC_CURSOR_VOID     UIE_FSC_BIAS + 74   /* Cursor Void Key,        R3.1 */
#define FSC_PRICE_CHECK     UIE_FSC_BIAS + 75   /* Price Check Key,        R3.1 */
#define FSC_ASK             UIE_FSC_BIAS + 76   /* Ask Key, 2172                */
#define FSC_RESERVE7        UIE_FSC_BIAS + 77   /* reserved                     */
#define FSC_CASINT          UIE_FSC_BIAS + 78   /* Cashier Interrupt,      R3.3 */
#define FSC_CASINT_B        UIE_FSC_BIAS + 79   /* Cashier Interrupt B key,R3.3 */
#define FSC_ADJ_SHIFT       UIE_FSC_BIAS + 80   /* Adjective Shift, 2172        */
#define FSC_DEPT            UIE_FSC_BIAS + 81   /* Dept. No. Key, 2172          */
#define FSC_E_VERSION       UIE_FSC_BIAS + 82   /* E Version Modifier Key, 2172 */
#define FSC_PRICE_ENTER     UIE_FSC_BIAS + 83   /* Price Enter Key , 2172       */
#define FSC_FOODSTAMP       UIE_FSC_BIAS + 84   /* Foodstamp Modifier Key       */
#define FSC_WIC_TRN         UIE_FSC_BIAS + 85   /* WIC Transaction Key          */
#define FSC_WIC_MODIF       UIE_FSC_BIAS + 86   /* WIC Modifier Key             */
#define FSC_HALO			UIE_FSC_BIAS + 87	/* Halo override key (SR 143 cwunn) */
#define FSC_FOR			    UIE_FSC_BIAS + 88   /* @/For Key (SR 143 cwunn)     */
#define FSC_VALIDATION      UIE_FSC_BIAS + 89   /* Manual Validation Key, SR 47 */
#define FSC_ALPHANUM        UIE_FSC_BIAS + 90   // Alphanumeric ASCII value as extended FSC
#define FSC_CLEARFSC        UIE_FSC_BIAS + 91   // Clear key assignable FSC 
#define FSC_EDIT_CONDIMENT	UIE_FSC_BIAS + 92	/* Condiment Editing SR 192 JHHJ*/
#define FSC_TENT			UIE_FSC_BIAS + 93   //Receipt ID implementation  ***PDINU
#define FSC_ORDER_DEC		UIE_FSC_BIAS + 94	/* Order Declaration Feature JHHJ*/
#define FSC_ADJ_MOD			UIE_FSC_BIAS + 95	/* Adjective Modification JHHJ */
#define FSC_GIFT_CARD		UIE_FSC_BIAS + 96	// Gift Card Feature  SS
#define FSC_SUPR_INTRVN		UIE_FSC_BIAS + 97	/* Supervisor Intervention */
#define FSC_DOC_LAUNCH		UIE_FSC_BIAS + 98	/* launch a document through shortcut in Database folder */
#define FSC_OPR_PICKUP		UIE_FSC_BIAS + 99	/* money pickup for Register mode like AC11 */
#define FSC_EDIT_COND_TBL   UIE_FSC_BIAS + 100  /* Condiment Table Edit RLZ */
#define FSC_WINDOW_DISPLAY  UIE_FSC_BIAS + 101  // Display a window whose ID label (character digits) follows
#define FSC_WINDOW_DISMISS  UIE_FSC_BIAS + 102  // Dismiss a displayed window whose ID label (character digits) follows
#define FSC_AUTO_SIGN_OUT	UIE_FSC_BIAS + 103  // Auto Sign Out
/*
*===========================================================================
*   Reserved 
*===========================================================================
*/
/*
=== Reserved ===
#define FSC                 0xFF                
=== Reserved ===
*/


/* ================================== */
/* ========== End of FSC.H ========== */
/* ================================== */
