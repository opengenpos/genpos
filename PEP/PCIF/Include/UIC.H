/*
****************************************************************************
**                                                                        **
**        *=*=*=*=*=*=*=*=*                                               **
**        *  NCR 2170     *             NCR Corporation, E&M OISO         **
**     @  *=*=*=*=*=*=*=*=*  0             (C) Copyright, 1992            **
**    <|\/~               ~\/|>                                           **
**   _/^\_                 _/^\_                                          **
**                                                                        **
****************************************************************************
*===========================================================================
* Title       : User Interface Control Module Header File                       
* Category    : User Interface Control, NCR 2170 US Hospitality Model         
* Program Name: UIC.H                                                      
* --------------------------------------------------------------------------
* Compiler    : MS-C Ver. 6.00A by Microsoft Corp.                         
* Memory Model: Midium Model                                               
* Options     : /c /AM /W4 /G1s /Os /Za /Zp                                 
* --------------------------------------------------------------------------
* Abstruct:        
*
* --------------------------------------------------------------------------
* Update Histories                                                         
*    Data  :Ver.Rev.:   Name    : Description
* May-26-92:00.00.01:H.Yamaguchi: initial                                   
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
*---------------------------------------------------------------------------
*   Define Return Code 
*---------------------------------------------------------------------------
*/
/*  External Error Code for UicCheckAndSet() / UicResetFlag() / UicStart() */


#define UIC_SUCCESS         0               /* Success */
#define UIC_NOT_IDLE       -1               /* Not idle  */


/*  Internal Error Code */

#define UIC_EXEC_JOB      -20               /* Reqest Backup copy or Parameter */
#define UIC_OTHER_USE     -21               /* Now execute other user */

/*
*===========================================================================
*   Structure Type Define Declarations
*===========================================================================
*/
/*
*===========================================================================
*   Function Prototype Declarations
*===========================================================================
*/
VOID   UicInit(VOID);                  /* Creates UIC Task                 */
SHORT  UicCheckAndSet(VOID);           /* Checks the Terminal control flag */
                                       /* if Idle then sets Busy state.    */
SHORT  UicResetFlag(USHORT husHandle); /* Changes from Busy to Idle state  */        
SHORT  UicStart(VOID);                 /* Starts UIC Task                  */        

/*========== END OF DEFINITION ============*/

