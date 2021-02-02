# genpos
source code for the GenPOS point of sale application owned by Georgia Southern University and now open source.

This is the updated and extended version of the original NeighborhoodPOS point of sale software that ran on
the NCR 2170 point of sale terminal and was then upgraded for the NCR 7448 point of sale terminal.

NCR donated version 1.4 of the NeighborhoodPOS product and source code to Georgia Southern University in 2002. In 2003 NCR
inked an exclusive license agreement with Georgia Southern University.

Georgia Southern ported the NeighborhoodPOS software from the NCR 7448 terminal, a Windows CE device, to
Windows NT and Windows XP in 2003 supporting the general Intel x86 architecture. Changes made for this new
release of NeighborhoodPOS, Version 2.0, included support for touchscreen terminals as well as support for
Datacap electronic payment software components and internationalization support by changing mnemonics storage
from ANSI 8 bit text characters to UNICODE 16 bit text characters.

During the NCR relationship, NCR decided to change the marketing and branding from
NeighborhoodPOS (NHPOS) to GeneralPOS (GenPOS).

Georgia Southern University continue to support the product through the years, working with NCR to provide
changes for NCR customers until 2020 when the decision to terminate all license agreements with NCR and
other customers was made.

The university decided that it would be appropriate to open source the product which led to this repository.

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
  
