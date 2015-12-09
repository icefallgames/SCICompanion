.. Printf

.. default - domain::js

.. include:: /includes/standard.rst

=================
Printf (Print.sc)
=================


.. function:: Printf(theText [params ...])

	Prints text on the screen using. The text may contain the following formatting
	characters:
	
	%d
		Formats a number in decimal.
		
	%x
		Formats a number in hexadecimal.

	%s
		Formats a string.

	:param string theText: A string of text containing formatting characters.
	
	Example usage::
	
		(Printf "Your name is %s, and you have %d hit points" name hitPoints)
	


