.. Object files

.. include:: /includes/standard.rst

================
 Object files
================

In order to link script resources together, object files are used. They do not contain full scripts,
but rather, information on the script's procedures, classes and variables. With them, you are able to call and access other scripts. 
This uses the same format as SCI Studio.

The object file format
========================

Header:

======== =========================================================
Offset   Description
======== =========================================================
CHAR[3]  "SCO" - File Identifier
WORD     Compiler Version (Major)
WORD     Compiler Version (Minor)
WORD     Compiler Version (Build)
BYTE     SCI Version (0x00 - SCI0, 0x01 - SCI01, 0x10 - SCI1)
WORD     Script Number
WORD     Total Publics
WORD     Offset to Publics (0 if none)
WORD     Total Classes
WORD     Offset to Classes (0 if none)
WORD     Total Variables
WORD     Offset to Variables (0 if none)
======== =========================================================


Public entries (for each public):

======== =========================================================
Offset   Description
======== =========================================================
WORD     Offset in script file
WORD     String length including the null
CHAR*    Entry name as null-terminated string
======== =========================================================

Class Entries (for each class):

======== =========================================================
Offset   Description
======== =========================================================
WORD     String length including the null
CHAR*    Entry name as null-terminated string
WORD     Total properties
WORD     Total methods
WORD     Species
WORD     Superclass
======== =========================================================


For Each Property:

======== =========================================================
Offset   Description
======== =========================================================
WORD     Property Name Index (in vocab.997)
WORD     Property Value
======== =========================================================


For each Method:

======== =========================================================
Offset   Description
======== =========================================================
WORD     Method Name Index (in vocab.997)
======== =========================================================

Local entries (for each local):

======== =========================================================
Offset   Description
======== =========================================================
WORD     String length including the null
CHAR*    Entry name as null-terminated string
======== =========================================================


