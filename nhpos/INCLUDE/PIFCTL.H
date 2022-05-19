#if defined(POSSIBLE_DEAD_CODE)

/*hhhh
*****************************************************************************
**    
**  Title:      Header File for Portable Platform Interface
**
**  File Name:  PIFCTL.H
**
**  Categoly:   Portable Platform Interface
**
**  Abstruct:   This header file provides function prototype declarations
**              for Portable Platform Interface functions and defines data
**              types and symbolic constants used with the functions.
**              Case of using the function of "PifControl()", please include
**              this header file.
**
**
*****************************************************************************
*
*****************************************************************************
**
**  Modification History:
**
**      Ver.  Date          Name            Description        
**      1.00  Jul/31/1992   Tkojima         Initial release
**                                          define symbol for DIAG IHC TEST
**      1.01  Aug/18/1992   M.Sugiyama      Added function number definitions 
**                                          and data structure definitions 
**                                          of NCR/DLC manager
**      1.02  Sep/17/1992   I.Shimizu       Add initialization of RAM disk
**      1.05  Sep/18/1992   I.Shimizu       Add code bank control
**      1.06  Sep/29/1992   I.Shimizu       Add CRC-16 calculation
**      1.0 7 Oct/26/1992   T.Kojima        Add memory dummp for Remote Diag
**      1.0 7 Feb/12/1993   T.Kawamura      Add ROM Version control
**      G0    Apl/05/1995   O.Nakada        Correspond to hospitality release
**                                          3.0 functions
**      G0    May/10/1995   T.Koyama        Add Program information
**
*****************************************************************************
hhhh*/


/******************
 *  Define Macro  *
 ******************/
/* manipulation for segment and offset */
#define FP_SEG(fp) (*((USHORT *)&(fp) + 1))
#define FP_OFF(fp) (*((USHORT *)&(fp)))
#define MK_FP(seg,ofs)  ((VOID FAR *)(((ULONG)(seg) << 16) | (ofs)))


/*******************************
 *  Define Symbolic Constants  *
 *******************************/
/***** Function number of PifControl() *****/
/* for IHC */
#define PIF_IHC_DIAG        0       /* DIAG IHC TEST (chip test)            */
#define PIF_PINGPONG        1       /* DIAG IHC TEST (pingpong test request)*/
#define PIF_NET_START       2       /* DIAG IHC TEST (start pingpong test   */
                                    /* response)                            */
#define PIF_NET_STOP        3       /* DIAG IHC TEST (stop pingpong test    */
                                    /* response)                            */
/* for NCR DLC Manager */   
#define PIF_DMG_OPEN        4       /* Open the NCR/DLC manager             */
#define PIF_DMG_CLOSE       5       /* Close the NCR/DLC manager            */
#define PIF_DMG_READ        6       /* Read received message                */
#define PIF_DMG_WRITE       7       /* Write message to send                */
#define PIF_DMG_RDTALLY     8       /* Read tally data                      */
#define PIF_DMG_RTTALLY     9       /* Reset tally data                     */
#define PIF_DMG_RDLINK     10       /* Read link status                     */
#define PIF_DMG_LOOP1      11       /* Loop 1 test                          */
#define PIF_DMG_LOOP3      12       /* Loop 3 test                          */
#define PIF_DMG_SDIAG      13       /* Set diag mode                        */
#define PIF_DMG_RDIAG      14       /* Reset diag mode                      */
#define PIF_DMG_RDROM      15       /* Read rom information of NCR/DLC board*/
#define PIF_DMG_SETTIME    16       /* Set wait time for receiving message  */

/* for File Manager */
#define PIF_FILE_INIT      17       /* Initialize RAM disk if need */

/* for code bank control */
#define PIF_BANK_CODE      18       /* Change code page */

/* for CRC-16 calculation */
#define PIF_CRC            19       /* calculate CRC-16 */

/* for memory dump  */
#define PIF_MEMDUMP         20

/* for display code set  */
#define PIF_DISPLAY_CODE_SET    21  /* display character code set */

/* for Rom Version get  */
#define PIF_ROM_VERSION   22        /* System Rom Version get */

/* for RAM disk head pointer get  */
#define PIF_GET_DSK_HEAD  23        /* Get disk head */

/* for Flash ROM */
#define PIF_FROM_WRITE      30      /* write FROM, enable interruput  */
#define PIF_FROM_WRITE2     31      /* write FROM, disable interruput */
#define PIF_FROM_ERASE      32      /* erase FROM, enable interruput  */
#define PIF_FROM_ERASE2     33      /* erase FROM, disable interruput */
#define PIF_FROM_MAPPING    34      /* hardware mapping               */

/* for Keyboard */
#define PIF_KEY_STATUS      35      /* read keyboard status */

/*************************
 *  the NCR/DLC manager  *
 ************************/
/* Function number of I/O control of NCR/DLC manager */
#define DMG_RDTALLY     1       /* Read tally data                      */
#define DMG_RTTALLY     2       /* Reset tally data                     */
#define DMG_RDLINK      3       /* Read link status                     */
#define DMG_LOOP1       4       /* Loop 1 test                          */
#define DMG_LOOP3       5       /* Loop 3 test                          */
#define DMG_SDIAG       6       /* Set diag mode                        */
#define DMG_RDIAG       7       /* Reset diag mode                      */
#define DMG_RDROM       8       /* Read rom information of NCR/DLC board*/
#define DMG_SETTIME     9       /* Set wait time for receiving message  */

/* Argument list definitions */
typedef struct  {    
    SHORT       dmgnum;
    CHAR FAR    *parg1;
    SHORT       sarg2;
} DMG_ARGLIST_TYPE1;


/********************
 * Change Code Page *
 ********************/
#define INT_BANK_CODE   0x8E    /* S/W interrupt */
#define INT_BANK_RAM    0x84    /* S/W interrupt */


/***************************\
*   type for memory dump    *
\***************************/
#define PIF_TYPE_BASE       0   /* base RAM */
#define PIF_TYPE_BANK1      1   /* SRAM     */
#define PIF_TYPE_BANK2      2   /* ROM      */
#define INT_MEMORY_DUMP     0x85    /* S/W interrupt */

/***************************************\
*   physical address for memory dump    *
\***************************************/
#ifdef  MODEL_RAM
#define PIF_ADDR_BANK1      0xc0000    
#define PIF_ADDR_BANK2      0x80000    
#else
#define PIF_ADDR_BANK1      0x40000    
#define PIF_ADDR_BANK2      0x60000
#endif

/********************
 * Calculate CRC-16 *
 ********************/
#define INT_CRC         0x8D    /* S/W interrupt */
typedef struct {
    USHORT  usFirstCRC;
    VOID    FAR *pStart;
    ULONG   ulLength;
    USHORT  FAR *pusResultCRC;
} PIF_CRC_ARG;

/*******************\
*   memory dump     *
\*******************/
typedef struct {
    UCHAR   uchType;
    USHORT  usBank;
    ULONG   ulAddress;
    USHORT  usLen;
    VOID FAR *pBuffer;
} ARG_MEMDUMP;    

/******************************
 * display character code set *
 ******************************/
#ifdef  MODEL_RAM
#define INT_DISPLAY         0x78    /* S/W interrupt */
#define CODE_CHANGE         0x00
typedef struct {
    UCHAR   uchType;
    UCHAR   uchCode;
} ARG_DISPLAY_CODE;
#endif

/***********************\
*   Rom Version Get     *
\***********************/
typedef struct {
    VOID FAR *pBuffer;
    USHORT  usLen;
} ARG_ROM_VERSION;    

/*********************************\
*   RAM Disk Head pointer Get     *
\*********************************/
typedef struct {
    USHORT      usInformation;
    USHORT      usDirOffset;
    USHORT      usDirSegment;
} DISK_INFO;

typedef struct {
    VOID FAR *pDir;
} ARG_DISK_PTR;    

/*****************
 *   Flash ROM   *
 *****************/
#define FROM_INT            0xA0        /* FROM BIOS                      */
#define FROM_FUNC_MAPPING   0x01        /* H/W mapping                    */
#define FROM_FUNC_ERASE     0x02        /* erase FROM, enable interruput  */
#define FROM_FUNC_ERASE2    0x00        /* erase FROM, disable interruput */
#define FROM_FUNC_WRITE     0x03        /* write FROM, enable interruput  */
#define FROM_FUNC_WRITE2    0x05        /* write FROM, disable interruput */
#define FROM_STATUS_OK      0x00
#define FROM_STATUS_ERROR   (-2)

typedef struct {
    USHORT  usPage;                         /* page number         */
    ULONG   ulAddress;                      /* destination address */
    USHORT  usLen;                          /* length              */
    VOID    FAR *pvBuffer;                  /* source address      */
} ARG_FROM_WRITE;

typedef struct {
    USHORT  usPage;                         /* page number         */
    USHORT  usBlock;                        /* block number        */
} ARG_FROM_ERASE;

typedef struct {
    USHORT  usFrom;                         /* FROM number         */
} ARG_FROM_MAPPING;

/*************************\
*   Program Information   *
\*************************/
#define INIT_INT                0x89
#define INIT_FUNC_STARTUP       0x00
#define INIT_FUNC_GET_CODE_SEG  0x01
#define INIT_FUNC_INIT_FAR_DATA 0x02
#define PIF_SEG_REL_SYSTEM      0xE000
#define PIF_SEG_REL_DIAG        0x7000
#define PIF_SEG_REL_APPL1       0x6000  /* none bank */
#define PIF_SEG_REL_APPL2       0x8000  /* bank */
#define PIF_PROGRAM_ID_BIOS     1
#define PIF_PROGRAM_ID_OS       2
#define PIF_PROGRAM_ID_UTILL    3
#define PIF_PROGRAM_ID_APPL     4
#define PIF_PROGRAM_ID_UTILD    5

/****************
 *   Keyboard   *
 ****************/
#define KEYBOARD_INT        0x7b        /* keyboard BIOS        */
#define KEY_READ_STATUS     0x01        /* read keyboard status */


/***********************
 *   End of PIFCTL.H   *
 ***********************/
#endif