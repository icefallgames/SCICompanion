.. MemoryInfo

.. default - domain::js

.. include:: /includes/standard.rst

===================
MemoryInfo (Kernel)
===================

.. function:: MemoryInfo(type)

	
	Retreives and returns the specified type of memory information.
	
	:param number type: miFREEHEAP, miLARGESTPTR, miFREEHUNK or miLARGESTHUNK.
	:returns: The information requested.
	
	The type can be one of the following:
	
	==================== =======================================================================
	type                 Description
	==================== =======================================================================
	miFREEHEAP           The amount of free memory in the heap.
	miLARGESTPTR         The size of the largest chunk of heap memory.
	miFREEHUNK           The size of the largest available hunk memory block.
	miLARGESTHUNK        The total amount of free hunk memory, shifted to the right by 4 bits.
	==================== =======================================================================
	
	Example::
	
		FormatPrint(
			"Free Heap: %u Bytes\nLargest ptr: %u Bytes\nFreeHunk: %u KBytes\nLargest hunk: %u Bytes"
			MemoryInfo(miFREEHEAP)
			MemoryInfo(miLARGESTPTR)
			(>> MemoryInfo(miFREEHUNK) 6)
			MemoryInfo(miLARGESTHUNK)
		)


