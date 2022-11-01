#ifndef	__FILEMDEF_H
#define	__FILEMDEF_H

/*
	RECPLU22_XXXXX ---> 2170 GP
	RECPLU_XXXXX   ---> 2172 Saratoga

	[EXAMPLE]
		if( Recplu22.aucStatus[ RECPLU22_IDX_XXX ] & RECPLU22_BIT_XXX ){
				Recplu.aucStatus[ RECPLU_IDX_XXX ] |= RECPLU_BIT_XXX ;
		}
		..........
*/
/*** varidation		***/
#define	RECPLU22_IDX_VALIDATION		0			/* 2170GP	*/
#define	RECPLU22_BIT_VALIDATION		(0x01)
#define	RECPLU_IDX_VALIDATION		0			/* Saratoga	*/
#define	RECPLU_BIT_VALIDATION		(0x20)

/*** scalable PLU	***/
#define	RECPLU22_IDX_SCALABLEPLU	0			/* 2170GP	*/
#define	RECPLU22_BIT_SCALABLEPLU	(0x20)
#define	RECPLU_IDX_SCALABLEPLU		2			/* Saratoga	*/
#define	RECPLU_BIT_SCALABLEPLU		(0x01)

/*** request superv interv	***/
#define	RECPLU22_IDX_REQSPVINTERV	0			/* 2170GP	*/
#define	RECPLU22_BIT_REQSPVINTERV	(0x80)
#define	RECPLU_IDX_REQSPVINTERV		0			/* Saratoga	*/
#define	RECPLU_BIT_REQSPVINTERV		(0x04)

/*** affect taxable itemizer	***/
#define	RECPLU22_IDX_AFFECTTAX1		1			/* 2170GP	*/
#define	RECPLU22_BIT_AFFECTTAX1		(0x01)
#define	RECPLU22_IDX_AFFECTTAX2		1
#define	RECPLU22_BIT_AFFECTTAX2		(0x02)
#define	RECPLU22_IDX_AFFECTTAX3		1
#define	RECPLU22_BIT_AFFECTTAX3		(0x04)
#define	RECPLU_IDX_AFFECTTAX1		1			/* Saratoga	*/
#define	RECPLU_BIT_AFFECTTAX1		(0x01)
#define	RECPLU_IDX_AFFECTTAX2		1
#define	RECPLU_BIT_AFFECTTAX2		(0x02)
#define	RECPLU_IDX_AFFECTTAX3		1
#define	RECPLU_BIT_AFFECTTAX3		(0x04)

/*** affect discountable itemzer	***/
#define	RECPLU22_IDX_AFFECTDISC1	1			/* 2170GP	*/
#define	RECPLU22_BIT_AFFECTDISC1	(0x10)
#define	RECPLU22_IDX_AFFECTDISC2	1
#define	RECPLU22_BIT_AFFECTDISC2	(0x20)
#define	RECPLU_IDX_AFFECTDISC1		1			/* Saratoga	*/
#define	RECPLU_BIT_AFFECTDISC1		(0x40)
#define	RECPLU_IDX_AFFECTDISC2		1
#define	RECPLU_BIT_AFFECTDISC2		(0x80)

/*** minus PLU (coupon)	***/
#define	RECPLU22_IDX_MINUS			1			/* 2170GP	*/
#define	RECPLU22_BIT_MINUS			(0x40)
#define	RECPLU_IDX_MINUS			0			/* Saratoga	*/
#define	RECPLU_BIT_MINUS			(0x01)

/*** use dept control	***/
#define	RECPLU22_IDX_USEDEPTCONT	1			/* 2170GP	*/
#define	RECPLU22_BIT_USEDEPTCONT	(0x80)
#define	RECPLU_IDX_USEDEPTCONT		2			/* Saratoga	*/
#define	RECPLU_BIT_USEDEPTCONT		(0x08)

/*** hash(memo)			***/
#define	RECPLU22_IDX_HASH			2			/* 2170GP	*/
#define	RECPLU22_BIT_HASH			(0x08)
#define	RECPLU_IDX_HASH				2			/* Saratoga	*/
#define	RECPLU_BIT_HASH				(0x04)

/*** birthday			***/
#define	RECPLU22_IDX_BIRTHDAY0		2			/* 2170GP	*/
#define	RECPLU22_BIT_BIRTHDAY0		(0x10)
#define	RECPLU22_IDX_BIRTHDAY1		2
#define	RECPLU22_BIT_BIRTHDAY1		(0x20)
#define	RECPLU_IDX_BIRTHDAY0		5			/* Saratoga	*/
#define	RECPLU_BIT_BIRTHDAY0		(0x10)
#define	RECPLU_IDX_BIRTHDAY1		5
#define	RECPLU_BIT_BIRTHDAY1		(0x20)

/* ### ADD Saratoga (052700) */
/*** Food Stamp 		***/
#define RECPLU22_IDX_FOODSTAMP		1			/* 2170 GP	*/
#define RECPLU22_BIT_FOODSTAMP		(0x08)
#define RECPLU_IDX_FOODSTAMP		1			/* Saratoga	*/
#define RECPLU_BIT_FOODSTAMP		(0x08)

/*** WIC		 		***/
#define RECPLU22_IDX_WIC			2			/* 2170 GP	*/
#define RECPLU22_BIT_WIC			(0x40)
#define RECPLU_IDX_WIC				5			/* Saratoga	*/
#define RECPLU_BIT_WIC				(0x40)


#endif	/* __FILEMDEF_H */
/* ========================================================================= */
