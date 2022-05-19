The NHPOS_filetool is an application to process Electronic Journal files and to export
the data stored there in.

This tool was originally developed when working on a US Customs field issue, CBP-132,
in which the Electronic Journal data file created by the RIS back office polling
application had errors in it. The errors were due to some of the EJ transaction
records having an incorrect length specified in the usCVLI value of the EJ record
header.

This tool was originall developed to process an RIS Electronic Journal file to
check for errors and to export the data there in.

With some additional work it could be used to dump a TOTALEJF file from the
GenPOS Database folder as well.

========================================================================
    WIN32 APPLICATION : NHPOS_filetool Project Overview
========================================================================

AppWizard has created this NHPOS_filetool application for you.  

This file contains a summary of what you will find in each of the files that
make up your NHPOS_filetool application.


NHPOS_filetool.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

NHPOS_filetool.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
AppWizard has created the following resources:

NHPOS_filetool.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
    Visual C++.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

NHPOS_filetool.ico
    This is an icon file, which is used as the application's icon (32x32).
    This icon is included by the main resource file NHPOS_filetool.rc.

small.ico
    This is an icon file, which contains a smaller version (16x16)
    of the application's icon. This icon is included by the main resource
    file NHPOS_filetool.rc.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named NHPOS_filetool.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
