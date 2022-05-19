
#pragma pack(push, 1)

// following two structs are used with the Electronic Journal file to provide
// the over all file management, EJF_HEADER, and the entry by entry housekeeping, EJT_HEADER.
//
// WARNING:  Be very careful in making any changes to the layout of these structs.
//           The source code that deals with Electronic Journal entries is scattered
//           throughout both the GenPOS terminal source as well as the PCIF DLL source.
//           Most remote applications using PCIF to pull the Electronic Journal have
//           dependencies on the EJT_HEADER struct and its layout so making changes
//           you will risk:
//              - breaking GenPOS in strange and unexpected ways
//              - breaking remote applications such as CWS in strange and unexpected ways

// Electronic Journal file contains a series of variable length records.  To traverse the file
// you need to start with the first entry pointed to by ulBeginPoint and then use the length of
// that entry (member usCVLI of the EJT_HEADER struct) to compute the offset to the next entry.
// The physical size of the file is fixed by provisioning through the Setup however the amount
// of data will vary.  The last entry near the physical end of file is always complete, there is
// no wrapping of an entry so ulEndofPoint indicates the end of the working physical data size.
// So there is the physical max size of the file, ulEJFSize, and the end of the working physical
// data, ulEndofPoint.  ulEndofPoint should always be smaller than ulEJFSize.  If the file is
// empty being just created then ulBeginPoint equals ulPrevPoint equals ulNextPoint equals ulEndofPoint.
// The first entry added will go to ulBeginPoint, ulPrevPoint will point to the this first entry,
// and ulNextPoint and ulEndofPoint will point to ulBeginPoint + usCVLI.
// If there is insufficient space between ulEndofPoint and ulEJFSize, then ulEndofPoint will remain
// showing where the working physical data ends, and ulNextPoint will wrap around to the beginning
// of the Electronic Journal file.
typedef  struct {
    UCHAR    fchEJFlag;      /* Status Flag */
    ULONG    ulEJFSize;      /* Physical file size at Creation */
    USHORT   usTtlLine;      /* Number of Total Lines of EJ entries */
    USHORT   usMonth;        /* Start Month */
    USHORT   usDay;          /* Start Day */
    USHORT   usHour;         /* Start Hour */
    USHORT   usMin;          /* Start minutue */
    ULONG    ulBeginPoint;   /* Start point of first transaction in the list of entries */
    ULONG    ulPrevPoint;    /* Start point of last transaction in the list of entries */
    ULONG    ulNextPoint;    /* Start point of next transaction or next location to put an entry */
    ULONG    ulEndofPoint;   /* Ending point of end of the last transaction physically stored in the file */
}EJF_HEADER;

#define EJT_HEADER_SIGNATURE  0xF234
typedef  struct {
    USHORT  usPVLI;        /* Previous Length */
    USHORT  usCVLI;        /* Current Length */
    USHORT  usTermNo;      /* Terminal number */
    USHORT  usConsecutive; /* Consecutive number */
    USHORT  usSeqNo;
    USHORT  usMode;        /* Mode */
    USHORT  usOpeNo;       /* Operator number */
    USHORT  fsTranFlag;    /* Transaction Flags */
    USHORT  usGCFNo;       /* G.C.F. number, Not Used   R2.0 */
    USHORT  usDate;        /* Start Date */
    USHORT  usTime;        /* Start Time */
    USHORT  usEjSignature; /* Reserve now used for signature */
}EJT_HEADER;


typedef struct {
	EJT_HEADER header;
	wchar_t    text[1024];
}EJ_RECORD;

#pragma pack(pop)
