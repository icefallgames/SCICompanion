.. MemorySegment

.. default - domain::js

.. include:: /includes/standard.rst

======================
MemorySegment (Kernel)
======================


.. function:: MemorySegment(fRestore address [size])

	.. IMPORTANT:: SCI1.1 only.

	MemorySegment provides access to a 256-byte block of memory that remains intact across restarts and restores.

	:param boolean fRestore: If 1, data is restored. If 0, data is saved.
	:param heapPtr address: Beginning of memory block to be saved or restored.
	:param number size: The size of the memory block to be saved (up to 256 bytes). Only specify this size when saving.


