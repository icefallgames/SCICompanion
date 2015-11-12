.. FGets

.. default - domain::js

.. include:: /includes/standard.rst

==============
FGets (Kernel)
==============

.. function:: FGets(buffer max [handle])

	
	Reads a string from the file pointed to by handle and stores it in buffer. If the string's length is greater than max, only max bytes are read.
	
	.. IMPORTANT::
		This kernel is SCI0 only. For SCI1.1, see :func:`FileIO`.
	
	:param string buffer: A buffer where the string is written.
	:param number max: The maximum number of bytes to read.
	:param number handle: A file handle.
	:returns: The number of bytes read.
	
	See also: :func:`FOpen`, :func:`FClose`, :func:`FPuts`.


