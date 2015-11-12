.. StrCmp

.. default - domain::js

.. include:: /includes/standard.rst

===============
StrCmp (Kernel)
===============


.. function:: StrCmp(aString bString [max])

	Compares aString to bString. If max is specified, only max number of characters are compared.

	:param string aString: Text string A.
	:param string bString: Text string B.
	:param number max: The maximum number of chars to compare.
	:returns: STRINGS_EQUAL if the strings are equal. STRING_LESSTHAN if aString is less than bString. STRING_GREATER if aString is greater than bString.

	Example::

		(var aString[40], strPtr)
		StrCpy(aString "Hello World")
		(if( == STRINGS_EQUAL StrCmp(aString "Hello World"))
			Print("The string says Hello World")
		)


