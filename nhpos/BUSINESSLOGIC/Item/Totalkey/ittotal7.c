/*

		NeighborhoodPOS Software Product Development Project
		Department of Information Systems
		College of Information Technology
		Georgia Southern University
		Statesboro, Georgia

		Copyright Georgia Southern University Foundation

=======================================================================*/
/*-----------------------------------------------------------------------*\
:   Title          : TOTAL KEY MODULE
:   Category       : TOTAL KEY MODULE, NCR 2170 US Hospitality Application
:   Program Name   : ITTOTAL7.C (charge posting function)
:  ---------------------------------------------------------------------
:  Abstract:
:   ItemTotalCP()           ; pre-authorization main routine
:   ItemTotalCPTotal()      ; pre-display total amount
:   ItemTotalCPCanDisp()    ; re-display after canceled pre-authorization
:   ItemTotalEditRspMsg()   ; edit response msg. from CP
:  ---------------------------------------------------------------------
:  Update Histories
:   Date   : Ver.Rev. :   Name      : Description
: Dec-13-93: 00.00.01 : K.You       : initial
: Mar-28-95: 03.00.00 : hkato       : R3.0
: Dec-19-95: 03.01.00 : hkato       : R3.1
: Aug-13-98: 03.03.00 : hrkato      : V3.3 (VAT/Service)
-------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*\
:   PVCS ENTRY
:    $Revision$
:    $Date$
:    $Author$
:    $Log$
\*-----------------------------------------------------------------------*/

#include	<tchar.h>
#include    <string.h>
#include	<stdlib.h>

#include    "ecr.h"
#include    "regstrct.h"
#include    "uie.h"
#include    "uireg.h"
#include    "pif.h"
#include    "rfl.h"
#include    "plu.h"
#include    "transact.h"
#include    "paraequ.h"
#include    "para.h"
#include    "csstbpar.h"
#include    "display.h"
#include    "mld.h"
#include    "item.h"
#include    "eept.h"
#include    "eeptl.h"
#include    "appllog.h"
#include    "csstbfcc.h"
#include    "cpm.h"
#include    "csstbcpm.h"
#include    "itmlocal.h"
#include	"fsc.h"
#include	"csstbstb.h"
#include	"csstbept.h"
#include	"pifmain.h"


/****** end of source ******/
