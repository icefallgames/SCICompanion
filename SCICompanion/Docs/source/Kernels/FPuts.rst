.. FPuts

.. default - domain::js

.. include:: /includes/standard.rst

==============
FPuts (Kernel)
==============

.. function:: FPuts(handle buffer)

	
	Writes the string contained in buffer to the file pointed to by handle.
	
	.. IMPORTANT::
		This kernel is SCI0 only. For SCI1.1, see :func:`FileIO`.
	
	:param number handle: The file handle.
	:param string buffer: The string to write.
	
	See also: :func:`FOpen`, :func:`FGets`, :func:`FClose`.


