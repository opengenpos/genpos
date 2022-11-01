______________________________________________

NCR NeighborhoodPOS
PC Interface for Win32 (G370-1796-0000)
Version 1.4.00
(C) Copyright 2000 - 2003. Georgia Southern University
All rights reserved.
______________________________________________


===== Table of Contents =====

  1. Product Overview
  2. System Requirements
  3. Install this software
  4. Uninstall this software
  5. Customize
  6. Directory Information

=============================


1. Product Overview
_____________________

 The purpose of PC Interface for Win32 is:

    + Provide the Win32 base dynamic link library to access to the NCR NeighborhoodPOS terminal.
    + Provide the header file for C/C++ and Basic language
    + Provide the sample programs and source files of Visual C++ and 
      Visual Basic.

This software allows you to develop a Win32 based application to access an NCR
NeighborhoodPOS terminal.

 The sample programs and source files included in this software are provided 
"AS IS" without warranty of any kind.  NCR corporation allows for ISV and 
developer to modify the source files of this product in your own 
responsibility.


2. System Requirements
_______________________

 This software is available on the following operating system:

    + Microsoft Windows NT 4.0 (US)
    + Microsoft Windows 95 (US)
    + Microsoft Windows 98 (US)

 The target POS terminal of this software is available on the following 
system and terminal application.

    + NCR 7448 Platform Software                    (LPIN D570-0725-0000)
    + NCR NeighborhoodPOS Application Software      (LPIN G370-1800-0000)

 To connect between the target POS terminal and the personal computer which 
is installed this software, the following parts are required:

    + PC Interface cable               (9 pin RS232C Cross)
                                       ... To connect direct with RS232C
          or
    + Modem (Hayes AT Command Set Compatible) ... With modem communication
    + Modem Interface cable            (9 pin RS232C strait) 


3. Install this software
_________________________

 The instruction of software installation as follows:

    1. Start up Windows NT 4.0/95/98 and insert the CD-ROM labeled as 
       "NCR NeighborhoodPOS PC Interface for Win32" into the CD-ROM drive.
    2. Select "Run..." from Start menu in the Windows NT/95/98 taskbar.
    3. Type "Setup.exe" in the command line and then click OK button.
    4. The installation program begins. Follow the on-screen instructions.


4. Uninstall this software
_________________________

 The instruction of software uninstallation as follows:

    1. Select "Control Panel" from Start menu in the Windows NT taskbar.
    2. Select "Add/Remove Programs" from control panel.
    3. Select "PC Interface for Win32" in the program list and then click 
       Add/Remove button.
    4. The uninstallation program begins. Follow the on-screen instructions.


5. Customize
_____________

 The following software tool is required to build the sample programs with 
the sample source codes:

    + Microsoft Visual C++   6.0 (US)
    + Microsoft Visual Basic 6.0 (US)

 To execute the sample programs included in this software, no software tool 
is required.


6. Directory Information
_________________________

 The product has the following directory structure:

    NeighborhoodPOS\PCIF  --- Base directory of this product
          |                         (This name is selectable at installation)
          |
          +-- INCLUDE           --- Include files for C/C++ language
          |
          +-- INCLUDE_VB        --- Module files for Visual Basic
          |
          +-- LIB               --- PC Interface Library files for C/C++ language
          |
          +-- SAMPLE.CPP        --- Sample program of Visual C++
          |    |
          |    +-- RES          --- Resource files
          |
          +-- SAMPLE.VB         --- Base directory of Visual Basic sample programs
               |
               +-- CASHIER      --- Cashier function sample
               |
               +-- EJ           --- Electronic Journal/Charge Posting Manager 
               |                    function sample
               |
               +-- ETK          --- Employee Time Keeping function sample
               |
               +-- GUEST        --- Guest Check function sample
               |
               +-- ISP          --- Special(ISP) function sample
               |
               +-- OPEPARA      --- Operational Parameter function sample
               |
               +-- TOTAL        --- Total function sample

_____________________________________________

    End of Read Me File (G570-0707-000)
_____________________________________________
