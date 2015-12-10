.. StrLen

.. default - domain::js

.. include:: /includes/standard.rst

===============
StrLen (Kernel)
===============

.. function:: StrLen(text)

	
	Returns the length of the specified string.
	
	:param string text: The string.
	:returns: The length of the string.
	
	Example::
	
		(procedure (SomeFunc &tmp [aString 40] strPtr)
			(StrCpy aString "Hello World")
			(if (< (StrLen aString) 100)
				(Print "The string is less than 100 characters")
			)
		)

