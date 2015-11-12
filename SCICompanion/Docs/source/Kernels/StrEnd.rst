.. StrEnd

.. default - domain::js

.. include:: /includes/standard.rst

===============
StrEnd (Kernel)
===============

.. function:: StrEnd(text)

	
	Returns a pointer to the end of the string.
	
	:param string text: The string.
	:returns: A pointer to the end of text.
	
	Example::
	
		(var aString[20], strPtr)
		StrCpy(aString "Hello World")
		= strPtr StrEnd(@aString)


