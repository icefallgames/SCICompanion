.. Include

.. include:: /includes/standard.rst

================
 Include
================

|scicomp| allows the include keyword to be used to include header files (file suffix .sh).
Header files can contain define keywords, and other *include* keywords.

An example header file::

	; Example Header 
	(define rsVIEW    $80)
	(define rsPIC     $81) 
	(define rsSCRIPT  $82) 
	(define rsTEXT    $83) 
	(define rsSOUND   $84) 
	(define rsMEMORY  $85) 
	(define rsVOCAB   $86) 
	(define rsFONT    $87) 
	(define rsCURSOR  $88) 
	(define rsPATCH   $89) 


Including it::

	(include myheader.sh)


Message files and polygon files
==================================

In the SCI1.1 template game, message files (\*.shm) and polygon files (\*.shp) can also be included::

	(include 110.shp)
	(include 110.shm)

In this case, support
for defining local variables in include files was added. This is needed for polygon definitions::

	; 110.shp -- Produced by SCI Companion
	; This file should only be edited with the SCI Companion polygon editor

	(local
		[P_Default110 11] = [1 PContainedAccess 4 319 189 319 50 0 50 0 189]
	)

At compile time, a script that includes *.sh* files that have local variables defined in them will have those local variables added to the script.
