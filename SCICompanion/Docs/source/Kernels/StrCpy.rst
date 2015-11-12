.. StrCpy

.. default - domain::js

.. include:: /includes/standard.rst

===============
StrCpy (Kernel)
===============


.. function:: StrCpy(destString srcString [max])

	Copies destString to srcString. If max is specified, only max number of characters are copied. It returns a pointer to the destString.

	:param string destString: The destination string buffer.
	:param string srcString: The source string.
	:param number max: The maximum number of chars to copy.
	:returns: A pointer to destString.

	Example::

		(var aString[40], strPtr)
		StrCpy(aString "Hello World")


