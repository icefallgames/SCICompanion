.. Load

.. default - domain::js

.. include:: /includes/standard.rst

=============
Load (Kernel)
=============

.. function:: Load(resourceType resourceNumber)

	
	Loads a resource into memory. If the resource type is rsMEMORY, is will allocate resourceNumber bytes of memory. It returns a pointer to the resource loaded or memory allocated.
	
	:param number resourceType: rsVIEW, rsPIC, etc...
	:param number resourceNumber: The resource number, or the number of bytes to allocate in the case of rsMEMORY.
	:returns: A pointer to the memory in the case of rsMEMORY.
	
	See also: :func:`Unload`.
	
	Example::
	
		(var hView, memPtr)
		= hView Load(rsVIEW 100) // loads VIEW.100
		= memPtr Load(rsMEMORY 1024) // allocates 1024 bytes of memory
		UnLoad(rsVIEW 100) // unloads VIEW.000 from memory
		UnLoad(rsMEMORY memPtr) // frees memPtr from


