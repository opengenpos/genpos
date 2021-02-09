# genpos
source code for the GenPOS point of sale application owned by Georgia Southern University and now open source.

This is the updated and extended version of the original NeighborhoodPOS point of sale software that ran on
the NCR 2170 point of sale terminal and was then upgraded for the NCR 7448 point of sale terminal.

## Recent history of NHPOS and GenPOS

In 2002 NCR donated version 1.4 of the NeighborhoodPOS product and source code which ran on the NCR 7448 terminal
to Georgia Southern University along with equipment to continue development. Georgia Southern, supported by grants from the
University System’s Intellectual Capital Partnership Program (ICAPP)
and funding provided by the Georgia state legislature, created the NCR Project lab which employed student
employees to assist in further development.
https://www.usg.edu/news/release/governor_announces_ncr_corp._gifts_to_create_software_enterprises_at_two_ru

In 2003 NCR inked an exclusive license agreement with Georgia Southern University. The NCR Project team at Georgia
Southern Univeristy worked with NCR to provide new versions of the software with new features to NCR customers and
NCR point of sale dealers.

Georgia Southern ported the NeighborhoodPOS software from the NCR 7448 terminal, a Windows CE device, to
Windows NT and Windows XP in 2003 supporting the general Intel x86 architecture. Changes made for this new
release of NeighborhoodPOS, Version 2.0, included support for touchscreen terminals as well as support for
Datacap electronic payment software components and internationalization support by changing mnemonics storage
from ANSI 8 bit text characters to UNICODE 16 bit text characters.

During the NCR relationship, NCR decided to change the marketing and branding from
NeighborhoodPOS (NHPOS) to GeneralPOS (GenPOS).

Georgia Southern University continue to support the product through the years, working with NCR to provide
changes for NCR customers. Beginning around 2010, NCR made changes in their strategic direction regarding point of
sale due to a number of changes in the point of sale industry as electronic payment processors began to bundle
a point of sale with their services for small merchants. The result was that NCR begain deemphasizing much of
the dealer network of small or low sales dealers and small merchants 

In 2020, Georgia Southern Univesity made the decision to shutdown support for the GenPOS product as NCR transitioned its
final customers away from the product. As there were a few remaining dealers interested in GenPOS due to its features
and flexibility and their knowledge of the product, Georgia Southern University decided to open source the application.

There was some propietary information in the source code in the areas of electronic payment interfaces and biometrics
sign-in. As part of making the source code open source, those portions of the source code were eliminated in cooperation
with those vendors.

## Base source code version of GenPOS

This version of the source code is based on Release 2.3.2 of GenPOS. This was the final version of the source code however
this version was never released into the field. Release 2.3.2 was based on Rel 2.3.1 which was released into the field
however it does contain one large change 64 bit totals. This change means that Third Party applications for Release 2.3.1
will probably not work correctly.

Relase 2.0.0 of NHPOS, since rebranded as GenPOS, contained the following major changes from Release 1.4 of NHPOS.

  - use of OPOS controls for peripherials such as receipt printers
  - support for touchscreen terminals as well as keyboards
  - Layout Manager screen design utility for customized screen design
  - support for Asian languages as well as increases in number of characters in mnemonics
  - auto coupons to automatically present possible coupons based on PLUs in transaction

Release 2.3.1 includes support for:

  - LAN kitchen printers
  - USB devices such as scales
  - 500 Thousand PLUs
  - changes and improvements to the PEP utility and PCSample utility
  - Out of Scope Datacap electronic payment to eliminate need for PCI-DSS assessments
  - support for Out of Scope EMV card processing using the Datacap electronic payment control
 
 Release 2.3.2 added support for 64 bit totals along with some improvements in the PEP and PCSample utilities.
 
 The source code base in this GitHub repository contains additional changes from the code base of Release 2.3.2
 in order to eliminate proprietary information as well as to eliminate no longer needed source code. Over the years
 some functionality was added which made some existing functionality obsolete however the source code remained. With
 the initial version of the source code uploaded to this repository, the source for that functionality is being
 removed in order to make the source code smaller and easier to understand.
 
  - using Datacap Out of Scope software components
  - storage of account holder information for Preauth as well as Store and Forward
  - Journal printing replaced by Electronic Journal
  
  Due to the age of some of the functionality that has gone unused and not been reviewed and updated, some features
  may not be compatible with more recent devices.
  
   - beverage dispensers for bars
   - scales
   - kitchen display systems
 
 ## Removal of proprietary information

As part of reaching agreement for making GenPOS open source, some proprietary information was removed from the
source code before putting the source up into this GitHub repository at the request of the owners. To use
these software components, a development agency must arrange a licensing agreement with the suppliers in
order to access the necessary components and documentation.
  - Datacap electronic payment interfaces
  - DigitalPersona biometrics interfaces
  
