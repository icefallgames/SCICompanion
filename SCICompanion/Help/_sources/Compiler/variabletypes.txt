.. Variable types

.. include:: /includes/standard.rst

================
 Variable types
================

Number
==========

A 16-bit number between -32768 and 32767.

Example::

	// An immediate 
	Wait(1234)

	// Assigned to a variable
	(var someNum)
	= someNum 1234
	Wait(someNum)


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

	// An immediate string
	Display("Hello World")

	// A string variable
	(string
		helloStr = "Hello World"
	)
	...
	Display(helloString)

	// A variable (local, global, var, param, property) containing a pointer to a string
	(var strPtr)
	= strPtr "Hello World"
	Display(strPtr)

	// A pointer to a variable (local, global, var, param)
	(var strBuf[40])
	StrCpy(@strBuf "Hello World")
	Display(@strBuf)


Rect
======

An array of four variables defining a rectangle

Example::

	(var rect[4])
	TextSize(@rect)
	= yMin rect[0]
	= xMin rect[1]
	= yMax rect[2]
	= xMax rect[3]

Point
=========

An array of two variables defining a point

Example::

	(var point[2])
	= y point[0]
	= x point[1]
