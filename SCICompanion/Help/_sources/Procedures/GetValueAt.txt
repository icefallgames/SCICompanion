.. GetValueAt

.. default - domain::js

.. include:: /includes/standard.rst

===================
GetValueAt (Obj.sc)
===================

.. function:: GetValueAt(buffer offset)

	
	Retrieves a value at a specified offset in a buffer. This might be used for
	retrieving the a point in a buffer containing a sequence of points, for instance.
	
	:param heapPtr buffer: A block of memory containing 16-bit values. 
	:param number offset: An index into that buffer.
	
	:returns: The item at an offset in a buffer.


