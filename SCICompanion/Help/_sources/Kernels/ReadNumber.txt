.. ReadNumber

.. default - domain::js

.. include:: /includes/standard.rst

===================
ReadNumber (Kernel)
===================

.. function:: ReadNumber(text)

	
	Reads the given string and converts it to a number. It can convert hex strings as long as they begin with $. It returns the number generated.
	
	:param string text: The text with a number in it.
	:returns: The number extracted from the text.
	
	Example::
	
		(var aNumber)
		= aNumber ReadNumber("500")
		= aNumber ReadNumber("$12AB")


