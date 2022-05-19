========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : TouchLibrary
========================================================================

TouchLibrary is a base layer of window functions that are used by both the
GenPOS terminal application and the Layout Manager utility within the PEP
terminal provisioning application for the design of the GenPOS GUI. This
library contains the GUI design data for windows and buttons as well as
providing an interface to the MFC window management functionality.

In essence the TouchLibrary extends the MFC window management functionality
acting as a middle layer between the MFC API and the window functionality of
either the GenPOS application, which uses it to implement the User Interface
display, or the Layout Manager utility, which uses it to implement the window
design toolkit for creating a User Interface display file to be used by the
GenPOS application to present a User Interface.

WindowControl.h, WindowControl.cpp
	This is the base class for all window controls.  It declares the
	CWindowControl class which contains the basic user interface
        control that is then extended by specific types of user interface
        controls such as buttons, windows, labels, etc.

WindowDocument.h, WindowDocument.cpp
        This is the document control class, CWindowDocument, which extends CWindowControl
        to provide the container for all of the user interface elements (buttons, windows, etc.)
        for a user interface.

WindowButton.h, WindowButton.cpp
        This is the button control class, CWindowButton, which extends CWindowControl
        to implement the base layer for buttons. This class contains the data describing
        a button (size, color, text, etc.) along with the methods to Serialize the
        button data and to interface between a higher level button API and the Windows
        MFC API for drawing buttons.

WindowItem.h, WindowItem.cpp
        This is the window container control class, CWindowItem, which extends
        CWindowControl to implement the base layer for windows that contain other
        controls. This class contains search functions used to find a particular
        control such as a button based on an assigned identifier or to iterate over
        the list of controls contained in the window.

WindowText.h, WindowText.cpp
        This is the text window control class, CWindowText, which extends CWindowControl
        in order to display special text windows such as the receipt window.

WindowLabel.h, WindowLabel.cpp
        This is the text label control class, CWindowLabel, which extends CWindowControl
        to provide a way to add labels to a user interface. These may be used to specify
        a line of text in a window to explain a procedure or to identify what the window
        contains.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named TouchLibrary.pch and a precompiled types file named StdAfx.obj.


/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
