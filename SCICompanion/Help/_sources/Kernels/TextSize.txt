.. TextSize

.. default - domain::js

.. include:: /includes/standard.rst

=================
TextSize (Kernel)
=================

.. function:: TextSize(sizeRect text font [maxWidth])

	
	Analyzes the given string with the specified font to determine how much space it will take up on the screen when it's printed.
	maxWidth specifies how long each line can be. If it is not specified, then 192 is assumed.
	It returns the bounds of the rectangle in sizeRect.
	
	:param heapPtr sizeRect: The rectangle into which the bounds are returns.
	:param string text: The text.
	:param number font: The font number to use.
	:param number maxWidth: Optional maximum pixel width.
	
	sizeRect should be an array of 4 values. Indices 2 and 3 represent the width and height of the text.
	
	Example::
	
		(var sizeRect[4])
		TextSize(sizeRect "Hello World" 4 100)
		// gets the bounds of "Hello World" with FONT.004 with a maximum line width of 100


