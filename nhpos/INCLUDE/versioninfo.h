
// version information to be updated for builds. centralizing this info
// to make the updating of build version information easier to manage.

/*
* --------------------------------------------------------------------------
*    Georgia Southern University Research Services Foundation
*    donated by NCR to the research foundation in 2002 and maintained here
*    since. Created open source version using MIT license without proprietary
*    information and created OpenGenPOS GitHub repository, https://github.com/opengenpos/genpos
*       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
*       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
*       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
*       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
*       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
*       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
*       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
*       2016  -> GenPOS Rel 2.2.2 (Windows 7, maintenance releases using Visual Studio 2005 in parallel with Rel 2.3.0 using Visual Studio 2013)
*       2015  -> GenPOS Rel 2.3.0 (Windows 7, EMV card support, Out Of Scope control, compiler change to Visual Studio 2013 from Visual Studio 2005)
*       2019  -> GenPOS Rel 2.3.1 (Windows 10, compiler change to Visual Studio 2015 from Visual Studio 2013)
*       2020  -> GenPOS Rel 2.3.2 (Unreleased. Windows 10, 64 bit totals)
*       2021  -> GenPOS Rel 2.4.0 (Conversion to OpenGenPOS)
*
*    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0
*/

//---------------------------------------------------------------------------------------------------
// Following define indicates target Windows version. Defined here so as to provide a
// single place to change. versioninfo.h should be included into each StdAfx.h include file
// in order to provide this setting to affect Windows.h and other Windows include file behavior.
//
// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
// See Using the Windows Headers  https://msdn.microsoft.com/library/windows/desktop/aa383745
// See Modifying WINVER and _WIN32_WINNT https://msdn.microsoft.com/en-us/library/6sehtctf.aspx
//    Windows XP       0x0501
//    Windows XP SP2   0x0502
//    Windows 7        0x0601
//    Windows 8        0x0602
//    Windows 8.1      0x0603
//    Windows 10       0x0A00

#define WINVER_FOR_GENPOS  0x0502          // GenPOS targets Windows XP SP2 and Windows POS Ready 2009 and later.

//--------------------------------------------------------------------------------------------------


// these defines are used in the resource file to specify the VERSIONINFO block.

//    VS_VERSION_INFO VERSIONINFO
//        FILEVERSION ALL_N_VERSIONINFO
//        PRODUCTVERSION ALL_N_VERSIONINFO
//        FILEFLAGSMASK 0x3fL
//    #ifdef _DEBUG
//        FILEFLAGS 0x1L
//    #else
//        FILEFLAGS 0x0L
//    #endif
//        FILEOS 0x4L
//        FILETYPE 0x1L
//        FILESUBTYPE 0x0L
//        BEGIN
//            BLOCK "StringFileInfo"
//            BEGIN
//                BLOCK "040904B0"
//                BEGIN
//                    VALUE "FileDescription", "DeviceConfig Application to configure NCR General POS peripherials."
//                    VALUE "FileVersion", ALL_M_VERSIONINFO
//                    VALUE "ProductVersion", ALL_M_VERSIONINFO
//                    VALUE "InternalName", "DeviceConfig"
//                    VALUE "LegalCopyright", "Copyright (C) 2004-2019 Georgia Southern University"
//                    VALUE "OriginalFilename", "DeviceConfig.EXE"
//                    VALUE "ProductName", "DeviceConfig Application"
//                    VALUE "OriginalFilename", "DeviceConfig.EXE"
//                    VALUE "ProductName", "DeviceConfig Application"
//                END
//            END
//        BLOCK "VarFileInfo"
//        BEGIN
//            VALUE "Translation", 0x409, 1200
//        END
//    END


#define  ALL_M_VERSIONINFO   "2, 4, 0, 004\0"
#define  ALL_N_VERSIONINFO   2,4,0,4

#define ALL_Q_COPYRIGHT     "Copyright (C) 2002-2024, Georgia Southern University\0"
#define ALL_Q_TRADEMARKS    "\0"
#define ALL_Q_COMPANYNAME   "OpenGenPOS\0"
#define ALL_Q_PRODUCTNAME   "OpenGenPOS General POS Application Software\0"

#define ALL_Q_PRIVATEBUILD  "\0"


// following define used in FrameWork.rc for the GenPOS release displayed on the
// initial startup progress dialog which appears when GenPOS first starts.
#define  ALL_O_VERSIONINFO   "2.4.0.004"

// following define intended for PEP resource files for PEP file version number.
#define  ALL_O_VERSIONINFO2  "v2.4.0.004"

// special defines for specific utilities and applications with an About box.

#define  ALL_P_VERSIONINFO  "DeviceConfig Version 2.4.0.004"

#define  ALL_Q_VERSIONINFO  "Version\t2.4.0.004"

