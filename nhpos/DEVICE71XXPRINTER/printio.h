// #define     HI_SPEED

// #define     DEVICE_7158     1 
// #define     DEVICE_7194     2 
// #define     DEVICE_7140     3 
// #define     DEVICE_2208     4 
// #define     DEVICE_7196
// #define     DEVICE_7161 

#if defined (DEVICE_7158)

#define     DEVICE_PRINTER      DEVICE_7158;
#define     SEND_THREAD_NAME    "SEND7158"
#define     RCV_THREAD_NAME     "RCV7158"
#define     STATION_SLIP        
#define     STATION_RECEIPT     
#define     COLMUN_RECEIPT_A    42
#define     COLMUN_RECEIPT_B    56
// #define     COLMUN_SLIP         44       Nov/22/2000
#define     COLMUN_SLIP         44+21       // Add Space    Nov/22/2000
#define     NUMBER_OF_STATION   (defined (STATION_SLIP) + \
                                 defined (STATION_RECEIPT) + \
                                 defined (STATION_JOURNAL) + \
                                 defined (STATION_VALIDATION))
#define     PROVIDED_FONT_TYPE_SELECTION
#define     FEED_COUNT_TO_PAPER_CUT   5

#endif
#if defined (DEVICE_7194)

#define     DEVICE_PRINTER      DEVICE_7194;
#define     SEND_THREAD_NAME    "SEND7194"
#define     RCV_THREAD_NAME     "RCV7194"
#define     STATION_RECEIPT     
#define     COLMUN_RECEIPT_A    42
#define     COLMUN_RECEIPT_B    56
#define     NUMBER_OF_STATION   (defined (STATION_SLIP) + \
                                 defined (STATION_RECEIPT) + \
                                 defined (STATION_JOURNAL) + \
                                 defined (STATION_VALIDATION))
#define     PROVIDED_FONT_TYPE_SELECTION
#define     FEED_COUNT_TO_PAPER_CUT   5

#endif
#if defined (DEVICE_7161)

#define     DEVICE_PRINTER      DEVICE_7161;
#define     SEND_THREAD_NAME    "SEND7161"
#define     RCV_THREAD_NAME     "RCV7161"
#define     STATION_RECEIPT     
#define     COLMUN_RECEIPT_A    35
#define     COLMUN_RECEIPT_B    42
#define     NUMBER_OF_STATION   (defined (STATION_SLIP) + \
                                 defined (STATION_RECEIPT) + \
                                 defined (STATION_JOURNAL) + \
                                 defined (STATION_VALIDATION))
#define     PROVIDED_FONT_TYPE_SELECTION
#define     FEED_COUNT_TO_PAPER_CUT   5

#endif
#if defined (DEVICE_7140)

#define     DEVICE_PRINTER      DEVICE_7140;
#define     SEND_THREAD_NAME    "SEND7140"
#define     RCV_THREAD_NAME     "RCV7140"
#define     STATION_RECEIPT     
#define     STATION_JOURNAL     
#define     STATION_VALIDATION  
#define     COLMUN_RECEIPT_A    24
#define     COLMUN_RECEIPT_B    24
#define     COLMUN_JOURNAL      24
#define     COLMUN_VALIDATION   55
#define     LINE_COUNT_VALIDATION   6
#define     NUMBER_OF_STATION   (defined (STATION_SLIP) + \
                                 defined (STATION_RECEIPT) + \
                                 defined (STATION_JOURNAL) + \
                                 defined (STATION_VALIDATION))
#define     PROVIDED_SAME_PRINT
#define     FEED_COUNT_TO_PAPER_CUT   8

#endif
#if defined (DEVICE_2208)

#define     DEVICE_PRINTER      DEVICE_2208;
#define     SEND_THREAD_NAME    "SEND2208"
#define     RCV_THREAD_NAME     "RCV2208"
#define     STATION_SLIP        
// #define     COLMUN_SLIP         44      Nov/22/2000
#define     COLMUN_SLIP         44+21   // Add Space    Nov/22/2000
#define     NUMBER_OF_STATION   (defined (STATION_SLIP) + \
                                 defined (STATION_RECEIPT) + \
                                 defined (STATION_JOURNAL) + \
                                 defined (STATION_VALIDATION))

#endif
#if defined (DEVICE_7196)

#define     DEVICE_PRINTER      DEVICE_7196;
#define     SEND_THREAD_NAME    "SEND7196"
#define     RCV_THREAD_NAME     "RCV7196"
#define     STATION_RECEIPT     
#define     COLMUN_RECEIPT_A    34
#define     COLMUN_RECEIPT_B    41
#define     NUMBER_OF_STATION   (defined (STATION_SLIP) + \
                                 defined (STATION_RECEIPT) + \
                                 defined (STATION_JOURNAL) + \
                                 defined (STATION_VALIDATION))
#define     PROVIDED_FONT_TYPE_SELECTION
#define     FEED_COUNT_TO_PAPER_CUT   5

#endif
