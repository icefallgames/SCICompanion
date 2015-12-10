.. UnLoad

.. default - domain::js

.. include:: /includes/standard.rst

===============
UnLoad (Kernel)
===============

.. function:: UnLoad(resourceType resourceNumber)

	
	Unloads a resource from memory. If the resource is rsMEMORY, it frees the allocated memory specified by its pointer.
	
	:param number resourceType: rsVIEW, rsPIC, etc...
	:param number resourceNumber: The resource number, or the number of bytes to allocate in the case of rsMEMORY.
	
	Example::
	
		(= hView (Load rsVIEW 100))		; loads VIEW.100
		(= memPtr (Load rsMEMORY 1024)	; allocates 1024 bytes of memory
		(UnLoad rsVIEW 100)				; unloads VIEW.000 from memory
		(UnLoad rsMEMORY memPtr)		; frees memPtr from


