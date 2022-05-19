USHORT NetCreateSem(USHORT usCount);
VOID   NetDeleteSem(USHORT usSem);
VOID   NetRequestSem(USHORT usSem);
VOID   NetReleaseSem(USHORT usSem);
VOID NetDeleteSemAll(VOID);
VOID NetSemInitialize(VOID);
