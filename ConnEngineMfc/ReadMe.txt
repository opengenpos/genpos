========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : ConnEngineMfc
========================================================================

The Connection Engine is an MFC DLL that is used as a transport layer to
transport XML messages between the GenPOS application and a remote application.

The DLL has two operating modes, a server mode used by GenPOS to create a
connection point on a GenPOS terminal and a client mode used by a remote
application to connect to a GenPOS Connection Engine server on a GenPOS terminal.

The DLL provides a C callable API whose source is in file ConnEngineMfc.cpp
that provides an interface into the C++ and MFC functionality.

When using the DLL in server mode call CONNENGINE_API int fnConnEngineStartEngine()
to initialize the server which will then beging listening for connection requests.

When using the DLL in client mode call either CONNENGINE_API int fnConnEngineConnectTo()
or CONNENGINE_API int fnConnEngineConnectToExt() to issue a connection request.

ConnEngineMfc.dsp
    This file (the project file) contains information at the project level and
    is used to build a single project or subproject. Other users can share the
    project (.dsp) file, but they should export the makefiles locally.

ConnEngineMfc.h
	This is the main header file for the DLL.  It declares the
	CConnEngineMfcApp class.

ConnEngineMfc.cpp
	This is the main DLL source file.  It contains the class CConnEngineMfcApp.

ConnEngineSocket.cpp
        This is the source file containing the lower level MFC socket interface handling
        source code. The class CConnEngineSocket is the base layer of the socket interface
        code handling the sending and receiving of complete XML messages and the class
        CConnEngineListenSocket used for the listening socket specifically. Both classes
        are extensions to the MFC class CAsyncSocket.

ConnEngineMfc.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	Visual C++.

res\ConnEngineMfc.rc2
    This file contains resources that are not edited by Microsoft 
	Visual C++.  You should place all resources not editable by
	the resource editor in this file.

ConnEngineMfc.def
    This file contains information about the DLL that must be
    provided to run with Microsoft Windows.  It defines parameters
    such as the name and description of the DLL.  It also exports
	functions from the DLL.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named ConnEngineMfc.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
