This is the Parameter Entry Program (PEP) source code tree. PEP is used to create and modify parameter databases
containing the various parameters for an OpenGenPOS counter terminal.

In addition to PEP, there are several other utilities:
 - Layout Manager Application NHPOSLM
 - PCSample data polling application to perform actions such as End of Day
 - PC Interface library PCIF for creating data polling applications

## Parameter Entry Program (PEP)

The PEP application is written in C using the Win 32 API directly as it originated as an older Windows 3.1 application
whose source code has been updated in various places. User prompts and other UI related materials are in human language
specific DLLs. Due to the age of the source, most of the UI components are old style and the user interface is a collection
of data type specific dialog boxes that are accessed through the main window menus.

PEP source code contains DLLs that provide UI prompts and strings in several languages. Due to the lack of
language specialists for the provided languages (Chinese, Spanish, French, and German) for the last few years,
the only up to date language DLL is English.

## Layout Manager

The Layout Manager Application is used to create and modify the user interface for an OpenGenPOS terminal. The application
allows you to create multiple windows with various windows, buttons, and labels with various kinds of behaviors. The flexibility
of Layout Manager is sufficient so that two different OpenGenPOS installations can look like two different POS software products.

Layout Manager allows colors, icons, and images to be used to decorate the terminal UI. There are also provisions to allow for
buttons to be automatically populated with text from the provisioning data provided by PEP. For example, the text for a tender or
total key or PLU mnemonic is pulled from the internal PEP database of the terminal. Some windows such as Order Entry Prompting
windows are created on the fly and automatically populated allowing the terminal UI behavior to be modified through PEP data
changes. This capability allows a layout to be created which is able to change the displayed UI based solely on PEP data
changes reducing problems with having to update two different data sources when making a change.

## PCSample

The PCSample data polling application was originally a sample program that used the PCIF library to demonstrate
how to write an application that could connect to an OpenGenPOS terminal and perform various tasks. These tasks
include:
 - reading the totals and other data from the terminal
 - performing management tasks such as End of Day

The PCSample data polling application is a C++ application using the Microsoft MFC framework.

