# genpos
source code for the GenPOS point of sale application owned by Georgia Southern University and now open source.

This is the updated and extended version of the original NeighborhoodPOS point of sale software that ran on
the NCR 2170 point of sale terminal and was then upgraded for the NCR 7448 point of sale terminal.

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


**Removal of proprietary information**

This version of the source code is based on Release 2.3.2 of GenPOS which includes support for:
  - 64 bit totals
  - LAN kitchen printers
  - changes and improvements to the PEP utility and PCSample utility
  - Out of Scope Datacap electronic payment to eliminate need for PCI-DSS assessments
  - use of OPOS controls for peripherials such as receipt printers
  - Layout Manager screen design utility
  
As part of reaching agreement for making GenPOS open source, some proprietary information was removed.
  - Datacap electronic payment interfaces
  - DigitalPersona biometrics interfaces
  
