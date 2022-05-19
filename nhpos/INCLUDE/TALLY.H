/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/W>                                           **
**   _/!\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : Terminal Tallies, Header File
* Category    : NCR 2170
* Program Name: TALLY.H                                                  
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Medium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Zap                                 
* --------------------------------------------------------------------------
* Abstract:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* Oct-28-92:00.00.01:N.Suzuki   : Initial
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
*   Print Mnager Tallies
*===========================================================================
*/
typedef	struct {
	USHORT		usMark;					/* 0xFFFF							*/
	USHORT		usMaxCount;				/* number of tallies				*/
	ULONG		ulRctLine;				/* receipt print lines				*/
	ULONG		ulJnlLine;				/* journal print lines				*/
	ULONG		ulSlipLine;				/* slip print lines					*/
	USHORT		usFeedLine;				/* feed lines						*/
	USHORT		usCut;					/* cut								*/
	USHORT		usStamp;				/* stamp							*/
	USHORT		usJamError;				/* jam error						*/
	USHORT		usSeqError;				/* sequence error					*/
	USHORT		usCommError;			/* communication error				*/
	USHORT		usTimeOut;				/* time out error					*/
} PRTTALLY;

#define	PMG_TALLIES		10				/* number of tallies				*/

/*****************************************/
/**	net tallies			**/
/*****************************************/
typedef	struct nettally {
	USHORT		usMark;             /* 0xFFFF                       */
	USHORT		usMaxCount;         /* number of tallies		    */
	ULONG		ulRcvPacket;        /* no. of receive packets	    */
	ULONG		ulNetDrop;          /* no. of dropped Packets	    */
	ULONG		ulNetOver;	        /* no. of overflowed Packets	*/
	ULONG		ulSndPacket;	    /* no. of send packets	        */
	ULONG		ulUnReached;	    /* no. of unreachable Packets	*/
	ULONG		ulReconfig;	        /* no. of reconfig  	        */
} NETTALLY;

#define	NET_TALLIES     6		    /* number of tallies		    */


