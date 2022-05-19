#ifndef	__CSKPINEX_H
#define	__CSKPINEX_H	1

#ifdef	_KPS_MAIN
	#define	_KPS_EX_EXTERN	
#else
	#define	_KPS_EX_EXTERN		extern
#endif

#ifdef	___DEL___


#define	KPS_CHK_PORTINDEX(x)	((0 <= x) && (x < KPS_NUM_COM))
#define	KPS_CHK_KPNUMBER(x)		((1 <= x) && (x <= KPS_NUMBER_OF_PRINTER))

/* ### CAUTION! 16bit only  */
#define	KPS_MASK16_ALT_MAN		(0x8000)		/* MANUAL ALT			*/
#define	KPS_MASK16_ALT_AUTO		(0x4000)		/* AUTO ALT				*/


#ifndef	KPS_NUMBER_OF_PRINTER
	#define	KPS_NUMBER_OF_PRINTER	(8)
#endif


/* new kitchen printer module */
typedef struct{
	SHORT	nKPNo;			/* KP no (1 - 8) 		*/
	BOOL	bEnable;		/* TRUE or FALSE		*/
	USHORT	usKPType;		/* 						*/
	USHORT	usKPStatus;		/* status (BIT MAP)		*/
} KPS_EX_COM_INFO;

#if _KPS_EX_EXTERN == ""
	static	_KPS_EX_COM_INFO	s_KpsComInfo[KPS_NUM_COM];	/* serial port info */
#endif

#endif	/* ___DEL___ */


#endif	/* __CSKPINEX_H */

