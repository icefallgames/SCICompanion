.. Variable types

.. include:: /includes/standard.rst

================
 Variable types
================

At their core, all variables in Sierra Script are 16-bit values. They are untyped, meaning you can assign a number, an object, or a pointer
to a buffer to the same variable. That said, it is useful to note the kinds of things that can be assigned to variables.

Number
==========

A 16-bit number between -32768 and 32767.

Example::

	// An immediate 
	(Wait 1234)

	// Assigned to a variable
	= someNum 1234
	(Wait someNum)


Boolean
==========

A TRUE or FALSE expression. This is the same as a number, but is evaluated differently. If it's value is 0, it is a FALSE expression, otherwise it is a TRUE expression.

String
==========

A string of characters. A string is a pointer to memory containing null-terminated text.


HeapPtr
==========

A pointer to a block of memory. Blocks of memory can contain anything, from classes, to strings, to variables.

Examples::

	; An immediate string
	(Display "Hello World")

	; A string variable
	(string
		helloStr = "Hello World"
	)
	...
	(Display helloString)

	; A variable (local, global, var, param, property) containing a pointer to a string
	= strPtr "Hello World"
	(Display strPtr)

	; A pointer to a variable (local, global, var, param)
	(local
		[strBuf 40]
	)
	; then in code:
	(StrCpy @strBuf "Hello World")
	(Display @strBuf)


Rect
======

An array of four variables defining a rectangle

Example::

	(procedure (SomeFunc &tmp [rect 4])
		(TextSize @rect)
		(= yMin [rect 0])
		(= xMin [rect 1])
		(= yMax [rect 2])
		(= xMax [rect 3])
	)

Point
=========

An array of two variables defining a point

Example::

	(procedure (SomeFunc &tmp [point 2])
		(= y [point 0])
		(= x [point 1])
	)
