/*************************************************************************************************
 * 
 * tbc.h
 *
 *************************************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/* command ID */
enum {TBC_CMDID_INIT, TBC_CMDID_STARTCOPY, TBC_CMDID_GETSTATUS};

/* result code */
enum {
    /* result values for TBC_CMDID_INIT/TBC_CMDID_GETSTATUS */
    TBC_RSLT_COMPLETE,
    TBC_RSLT_ACCEPTED,
    TBC_RSLT_BUSY,
    TBC_RSLT_ERROR,

    /* result values for TBC_CMDID_STARTCOPY */
    TBC_RSLT_ACC_CLEAR,
    TBC_RSLT_ACC_LOWSTOR,
    TBC_RSLT_DNY_BUSY,
    TBC_RSLT_DNY_NOHEAP,
    TBC_RSLT_DNY_NOSTOR,
    TBC_RSLT_DNY_ERROR
};

/* command packet for TbcCommand() */
typedef struct {
    DWORD dwCmdId;
    DWORD dwResult;
    DWORD dwLen;
    VOID  *pvData;
} TBC_CMD;

extern LONG TbcCommand(TBC_CMD *);           /* TBC api entry point */

#if defined(__cplusplus)
}
#endif
/*************************************************************************************************
 * end of file
 *************************************************************************************************/
