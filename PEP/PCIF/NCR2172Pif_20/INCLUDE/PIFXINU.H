/*hhhh
*****************************************************************************
**    
**  Title:      Header File of Platform Interface
**
**  File Name:  PIFXINU.H
**
**  Categoly:   Poratable Platform Interface
**
**  Abstruct:   This header file provides function prototype declarations,
**              data types, symbolic constants used with Platform Interface.
**              
**              A software sturcture of Portable Platform Interface is as
**              follows:
**
**                  |                                     |
**                  |        Applications and             |
**                  |        System Applications          |
**                  |                                     |
**                  +-------------------------------------+
**                  | Portable Platform Interface         |
**                  |  +-------------------------------+  |
**                  |  |                               |  |
**                  |  |       Recovery Manager        |  |
**                  |  |                               |  |
**                 ----++-----------------------------++---------------------
**                  |  ||   Function Pointers Table   ||  | or | Function  |
**                  |  ++-----------------------------++  |    | Pointers  |
**                  |  |                               |  |    | Table     |
**                  |  |       Platform Interface      |  |    +-----------+
**                  |  |                               |  |    |           |
**                  |  +-------------------------------+  |    | Simulator |
**                  |                                     |    |           |
**                  +-------------------------------------+    |           |
**                  |                                     |    |           |
**                  |          OS(XINU) or BIOS           |    |           |
**                  |                                     |    |           |
**
**              Portable Platform Interface consists of Recovery Manager,
**              Function Pointers Table and Platform Interface. The upper
**              Portable Platform Interface can also provide the interface
**              with Simulator.
**
**              The Platform Interface consits of the following modules.
**
**                  PIFXINU.C   All Static Variables and All Functions
**
**              Concerning the summary of Recovery Manager, see "PIFRM.H".
**              Concerning the summary of Function Pointers Table, see
**              "PIFTABLE.C".
**
*****************************************************************************
*
*****************************************************************************
**
**  Modification History:
**
**  Ver.      Date       Name           Description        
**  1.00      MAY-01-92  I.Shimizu      Initial release
**  00.00.01  AUG-17-92  I.Shimizu      Correspond to rev. G document
**            SEP-04-92  I.Shimizu      Add PifOpenMsr(), PifReadMsr() and
**                                      PifCloseMsr()
**            SEP-21-92  I.Shimizu      Add Lowest Priority Thread
**            SEP-29-92  I.Shimizu      Correct MSR fuction codes
**            OCT-01-92  I.Shimizu      Modify ID_TRACKJ
**
*****************************************************************************
hhhh*/


/*******************************
 *  Define Symbolic Constants  *
 *******************************/
/* in-line assembler keyword */
#define ASM _asm

/* used with PifAbort() */
#define LEN_DSP     20
#define LEN_PRT     21

/* used with PifBeginThread() */
#define STACKSIZE   100     /* stack size of common process */
#define ARGSIZE1    4       /* size of argument for common process */
#define ARGSIZE2    6

/* used with Common Process */
#define CPU_FLAG    0x0202  /* CPU flag for initialization */

/* used with Open Communication */
#define KITCHEN_PORT_FLAG   0x40
#define SLIP_PORT_FLAG      0x80

/* used with Time Functions */
#define INT_DATE_TIME   0x7C    /* interrupt no. */
#define FUNC_GET_TIME   0x02    /* function code */
#define FUNC_SET_TIME   0x03
#define FUNC_GET_DATE   0x04
#define FUNC_SET_DATE   0x05

/* used with Keyboard and Waiter Functions */
#define INT_KEYBOARD        0x7B
#define FUNC_ENABLE_KEY     0x2200
#define FUNC_DISABLE_KEY    0x2201
#define FUNC_GET_KEYLOCK    0x20
#define FUNC_GET_WAITER     0x30

/* used with Display Functions */
#define INT_DISPLAY         0x78
#define FUNC_DISP_OPER      0x1300  /* Write Character String on Operator */
                                    /* Display */
#define FUNC_DISP_CUST      0x9300  /* Write Character String on Customer */
                                    /* Display */
#define FUNC_DISP_ATTR_OPER 0x1302  /* Write Character String and Attributes */
                                    /* on Operator Display */
#define FUNC_DISP_ATTR_CUST 0x9302  /* Write Character String and Attributes */
                                    /* on Customer Display */
#define FUNC_DESC_OPER  0x26    /* Write Descriptor for Operator Dispaly */
#define FUNC_DESC_CUST  0xA6    /* Write Descriptor for Customer Display */

/* used with Drawer Functions */
#define INT_DRAWER              0x7A
#define FUNC_OPEN_DRAWER        0x7A11
#define FUNC_DRAWER_STATUS      0x7A10
#define DRAWER1                 1
#define DRAWER2                 2
#define STATUS_DRAWER1_OPENED   0x0001
#define STATUS_DRAWER2_OPENED   0x0002
#define STATUS_DRAWER1_OPENING  0x0004
#define STATUS_DRAWER2_OPENING  0x0008

/* used with Tone Functions */
#define INT_TONE            0x7A
#define FUNC_PLAY_NOTE      0x7A00
#define FUNC_PLAY_STANZA    0x7A01
#define FUNC_TONE_OFF       0x7A03
#define FUNC_TONE_VOLUME    0x7A04
#define NOTE_FREQUENCY      0x0030  /* used with Play Note */
#define MAX_VOLUME          7

/* used with Get System Configuration */
#define INT_SYSCONFIG       0x88

/* used with Log Exception */
#define INT_LOG             0x8A

/* used with Integrated MSR */
#define INT_MSR             0x7A
#define FUNC_OPEN_MSR       0x7905
#define FUNC_READ_MSR       0x7906
#define FUNC_CLOSE_MSR      0x7907
#define ID_TRACK1           0x61
#define ID_TRACK2           0x62
#define ID_TRACKJ           0x64
#define ID_TRACK_END        0x6F
#define LEN_MSR_WORK_BUFFER 118

/* used with Restart */
#define INT_SW_RESET        0x8B

/* used with Turn Off Power */
#define INT_TURN_OFF_POWER  0x8C

/* define printer status bit */
#define PRTOK           0x04    /* no error */

/* define status ID for debugging */
#define PRT_RES_ID      -900
#define PRT_ERROR_ID    -800

/* define printer control characters */
#define LF      '\x0A'
#define ESC     '\x1B'


/******************
 *  Define Macro  *
 ******************/
/* manipulation for segment and offset */
#define FP_SEG(fp) (*((USHORT *)&(fp) + 1))
#define FP_OFF(fp) (*((USHORT *)&(fp)))

/* Error Handling for Recovery Manager */
#define XINUERROR(sXinuStatus, usFaultModule) \
        if (sXinuStatus<0) sXinuStatus = PifSubXinuError(sXinuStatus, \
                                                         usFaultModule)


/************************
 *  Declare Structures  *
 ************************/
/* XINU Process Control Table */
#define PNMLEN 9
struct	pentry	{
	CHAR	pname[PNMLEN+1];	    /* process name			*/
	UCHAR	pstate;			        /* process state: PRCURR, etc.	*/
	UCHAR	pprio;			        /* process priority		*/
	UCHAR	psem;			        /* semaphore if process waiting	*/
	UCHAR	phasmsg;		        /* nonzero iff pmsg is valid	*/
	SHORT	pmsg;			        /* message sent to this process	*/
	VOID	*pregs;			        /* saved environment		*/
	SHORT	(*paddr)();		        /* initial code address		*/
	USHORT	pargs;			        /* initial number of arguments	*/
	VOID	*pbase;			        /* base of run time stack	*/
	USHORT	plen;			        /* stack length	in bytes	*/
    USHORT  usReserved;             /* Allign to 32 bytes */
};

/* used with PifOepnCom() */
typedef struct {        /* argument list for PIP driver */
    UCHAR   uchPortId;
    UCHAR   uchPipAddr;
} ARG_PIP;
   
typedef struct {
    UCHAR   uchPortId;
    UCHAR   uchComPara[LEN_COM_PARA];
} ARG_COM;

typedef struct {
    USHORT  usPortId;
    UCHAR   uchComPara[LEN_COM_PARA];
} ARG_YAM;

/* used with PifSubPrint() */
typedef struct {    /* printer text for PIP */
    UCHAR   auchIDENT[3];
    UCHAR   uchSEQ;
    UCHAR   auchPrtMsg[LEN_PRT+1];
} PRTTEXT;

typedef struct {    /* printer response for PIP */
    UCHAR   auchIDENT[3];
    UCHAR   uchSEQ;
    UCHAR   uchStatus;
    UCHAR   uchErrorCode;
    UCHAR   uchAddr;
} PRTRESPONSE;


/*********************************
 *  Declare Function Prototypes  *
 *********************************/
/* common process (use Xinu's Stack) */
static
VOID PifXinuProcess1(VOID (THREADENTRY *pStart)(ULONG ulArg),
                     ULONG ulArg);
 
/* common process (use User's Stack)*/
static
VOID PifXinuProcess2(VOID (THREADENTRY *pStart)(ULONG ulArg),
                     VOID FAR *pStack, ULONG ulArg);

/* lowest priority thread for RAM machine */
VOID LowMain(VOID);

/* sub-functions */
SHORT  FAR PifSubXinuError(SHORT sXinuStatus, USHORT usFaultModule);
USHORT PifSubPtoB(UCHAR uchPPacked);
VOID   PifSubBtoP(USHORT usBinary, UCHAR *pauchPPacked);
VOID   PifSubPtoA(UCHAR *pauchPPacked, UCHAR *pauchMsg, USHORT usSizePacked);
VOID   PifSubFtoA(PIFDEF pRetAddr, UCHAR *pauchMsg);
VOID   PifSubInitMsg(UCHAR *pauchMsg, UCHAR uchData, USHORT usLength);
VOID   PifSubInitPrinter(VOID);
VOID   PifSubPrint(UCHAR *pauchMsg);
VOID   PifSubPrintEnd(VOID);
