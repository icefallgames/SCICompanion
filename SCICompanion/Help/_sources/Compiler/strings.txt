.. Strings

.. include:: /includes/standard.rst

================
 Strings
================

Strings are an array of characters ending in a NULL ('\0') character. They are used for things such as printing text, message boxes, menus and titlebars.

Strings can either be directly inserted into your code like this::

	(procedure (TitleProc) 
		Display("SCI Quest")
	)

Or you can declare them in the script segment and use their label::

	(string
		TitleString = "SCI Quest"
	)

	(procedure (TitleProc) 
		Display(TitleString)
	)

Strings can be split up onto multiple lines with the '+' operator::

	(procedure (TitleProc) 
		Print(
			"This is a very long "+
			"string.\n"+
			"That uses three lines in this source file"
		)
	)

Because strings are arrays, you can use them as arrays::

	(string
		// A simple string. 
		SomeString = "This is a string" 
		// Numbers can be inserted into the strings. 
		AnotherString = ("H" "ELL" "O" $20 "There!" 0) 
		// This allocates 40 bytes of an empty string. 
		StringBuffer[40] 
		// This allocates 20 bytes of string. The first five bytes are filled with "Hello".
		YetAnotherString[20] = "Hello"
	)

	(procedure (StringProc) 
		// You can make use of the array. 
		// This would display "is a string" 
		Display(SomeString[5]) 
	)

 