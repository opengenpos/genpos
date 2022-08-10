The NHPOS source folder hierarchy contains the source code for the OpenGenPOS point of sale application.
There are folders for each of the major subsystems that make up the source code body. Due to the organic
nature of how the source code has evolved over the years with multiple organizations and people working
with the source code for years, the source code is not well organized.

The OpenGenPOS point of sale application supports three types of point of sale uses:
 - Quick Service Restaurant with optional drive thru
 - Table Service Restaurant
 - Retail Store such as small grocery or hard goods

Provisioning and setup of an OpenGenPOS installation is done using either the PEP utility or a
Third Party utility that uses the Remote Provisioning API. The PEP utility uses the Layout Manager
utility for screen design. The source code for both os those utilities in in the PEP folder hierarchy.

## Operational File Hierarchy

When installed, the OpenGenPOS terminal application uses two file trees in the C: drive.
 - C:/FlashDisk/NCR/Saratoga
 - C:/TempDisk/NCR/Saratoga
 
 There are several subfolders in the C:/FlashDisk/NCR/Saratoga folder which contain various data files
 and resources for the terminal application:
  - C:/FlashDisk/NCR/Saratoga/Database - operational data files with provisioning data and totals
  - C:/FlashDisk/NCR/Saratoga/Program - where the executables and DLLs are installed
  - C:/FlashDisk/NCR/Saratoga/Icons - where the graphic files for the touchscreen display are located
  - C:/FlashDisk/NCR/Saratoga/Web - the folder for the built in, simple web server
  - C:/FlashDisk/NCR/PrintFiles - the folder where files containing printer output are placed
