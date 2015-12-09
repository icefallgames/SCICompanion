.. DebugPrint

.. default - domain::js

.. include:: /includes/standard.rst

====================
DebugPrint (Main.sc)
====================


.. function:: DebugPrint(theText [params ...])

	Prints a debug message that can be displayed in SCI Companion. The text may contain the following formatting
	characters:
	
	%d
		Formats a number in decimal.
		
	%x
		Formats a number in hexadecimal.

	%s
		Formats a string.

	:param string theText: A string of text containing formatting characters.
	
	Example usage::
	
		(DebugPrint {You are in room %d} gNewRoomNumber)


