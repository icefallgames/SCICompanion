.. Strings

.. include:: /includes/standard.rst

================
 Strings
================

Strings are an array of characters ending in a NULL ('\0') character. They are used for things such as printing text, message boxes, menus and titlebars.

Strings can be enclosed in curly-braces {}, or double-quotes. They are interchangeable unless a (text#) clause is present in the file. See `Auto-generation of text resources`_.

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


Auto-generation of text resources
====================================

Strings take up memory in the interpreter's heap. For that reason, it can be beneficial to use text resources instead: :doc:`/texts`. Text resources
are stored in the interpreter's hunk space, which is larger than the heap.

Text resources are also useful if you are translating your game into another language, as the translator can just work on the text resources and not touch the scripts.

In script, text resources are represented by a text resource/number pair. For instance, to print the 10th entry in text resource 700, in SCI0
you could do::

    (Print 700 10)

Several template game library procedures support text resource/number tuples: Print and FormatPrint (in SCI0), and Prints, and Printf (in SCI1.1).
They use the kernel call :func:`GetFarText` to retrieve the text for a given tuple.

It can be tedious to keep track of which numbers correspond to which text string, so |scicomp| gives you the option to have them automatically
generated when you compile a script. To do so, you need to use double-quoted strings, and specify a text# clause near the top of your script::

    ; auto-generate text resources for this script
    (text# scriptNumber)

You can specify a specific text resource number to generate, or - more commonly - just use 'scriptNumber' to generate one with the same number as this script.
Any double-quoted strings will be converted to a tuple. So for instance, in script number 70, if this is the 10th string encountered::

    (Print "Hello")

will be compiled as if this were written::

    (Print 70 10)

and text resource 70 will contain "Hello" as entry number 10.

Note that the script is not actually changed - only the compiled byte code. Also note that this transformation is only applied to strings passed
as procedure or method parameters. Also note that this will overwrite any existing text resource of the number without warning.