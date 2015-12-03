.. Strings

.. include:: /includes/standard.rst

================
 Strings
================

Strings are an array of characters ending in a NULL ('\0') character. They are used for things such as printing text, message boxes, menus and titlebars.

Strings can be enclosed in curly-braces {}, or double-quotes. In the original Script used by Sierra, any double-quoted strings passed as parameters to a procedure
would be converted into a resource number/entry number tuple (two parameters), and the string would be inserted into the text resource. This of course requires
the procedure to support tuples as input strings (The SCI0 Print procedure supports this). |scicomp| does not support this feature yet, so currently
brace and double-quote strings can be used interchangeably.

Example of using a string::

	(procedure (TitleProc) 
		(Display {This is enclosed in curly braces})
		(Display "This uses double quotes")
	)


All whitespace between words in a string will be converted into a single space character. This allows strings to easily be split up onto multiple lines::

	(procedure (TitleProc) 
		(Prints
			"This is a very long
			string.\n
			That uses three lines in this source file"
		)
	)

If you need to have multiple spaces between words, you can use the underscore character::

	(procedure (TitleProc) 
		(Prints "I want 5 spaces here:_____Then the rest of the text.") ; Prints "I want 5 spaces here:     Then the rest of the text."
	)

To use underscores, therefore, you need to escape them with '\'::

	(procedure (TitleProc) 
		(Prints "This is an underscore: \_")	; Prints "This is an underscore: _"
	)

You can include carriage returns and tabs in strings by escaping them with '\'::

	(Prints "This is on one line\nAnd this is on the next.")
	(Prints "Here is a tab: \t.")

Any value between 0 and 255 can be placed in a string by using a hex value and escaping it::

	(Prints "This is lowercase z: \7A.")

Finally, you can escape braces or double-quotes::

	(Prints "Double quotes: \" and curly braces: {} ")	; Prints "Double quotes: " and curly braces: {} "
	(Prints {Double quotes: " and curly braces: \{\} })	; Prints "Double quotes: " and curly braces: {} "