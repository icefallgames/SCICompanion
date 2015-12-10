.. ScriptID

.. default - domain::js

.. include:: /includes/standard.rst

=================
ScriptID (Kernel)
=================

.. function:: ScriptID(scriptNum [index])

	
	Returns the address in memory pointed to by entry Index in the exports segment of SCRIPT.<scriptNum>.
	
	Example::
	
		; gets the address in memory pointed to by entry 4 in the exports segment of SCRIPT.100.
		(= heapPointer (ScriptID 100 4))


