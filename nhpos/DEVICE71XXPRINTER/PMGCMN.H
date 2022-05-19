/**
;========================================================================
;+                                                                      +
;+              L O C A L    D E F I N I T I O N s                      +
;+                                                                      +
;========================================================================
**/
#define PMG_TIM_SPOOL   50              /* timer value of wait          */

typedef struct {
    UCHAR       uchEsc;
    UCHAR       uchCmd;
} ESC2BYTE;

typedef struct {
    UCHAR       uchEsc;
    UCHAR       uchCmd;
    UCHAR       uchData;
} ESC3BYTE;

typedef struct {
    UCHAR       uchEsc;
    UCHAR       uchCmd;
    UCHAR       uchCmd2;
    UCHAR       uchData;
} ESC4BYTE;

