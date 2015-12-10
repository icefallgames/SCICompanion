.. Memory

.. default - domain::js

.. include:: /includes/standard.rst

===============
Memory (Kernel)
===============


	Memory has several subfunctions that allow for inspecting, modifying and allocating memory dynamically.

	.. IMPORTANT::
		SCI1.1 only.

.. function:: Memory(memALLOC_CRIT byteCount)

	Allocates memory, and crashes the game if it fails.

	:param number byteCount: The number of bytes to allocate.
	:returns: A pointer to the memory block.

	Free the memory with Memory(memFREE).

	Example::

		(method (setName theName)
			(= name (Memory memALLOC_CRIT (+ (StrLen theName) 1)))
			(StrCpy name theName)
		)

.. function:: Memory(memALLOC_NONCRIT byteCount)
	:noindex:

	Allocates memory, returns 0 if it fails.

	:param number byteCount: The number of bytes to allocate.
	:returns: A pointer to the memory block.

	Example::

		(if (not (= text (Memory memALLOC_NONCRIT 20)))
			(Prints "Couldn't allocate memory.")
		)

	Free the memory with Memory(memFREE).

.. function:: Memory(memFREE address)
	:noindex:

	Frees memory allocated by memALLOC or memALLOC_NONCRIT.

	:param heapPtr address: A pointer to the memory block.

	Example::

		(Memory memFREE blah)

.. function:: Memory(memCPY dest source byteCount)
	:noindex:

	Copies memory from one location to another.

	:param heapPtr dest: Destination address.
	:param heapPtr source: Source address.
	:param number byteCount: The number of bytes to copy.

.. function:: Memory(memPEEK address)
	:noindex:

	Returns the 16-bit value at the address. It's important to note that addresses point
	to 16-bit values, so the address of two adjacent values in memory will be separated by 2.

	:param heapPtr address: A memory address.
	:returns: The value at the address.

	Example::

		(= point (Memory memPEEK (polyPointArray + (* 2 pointIndex))))

.. function:: Memory(memPOKE address value)
	:noindex:

	Sets the value at a memory location.

	:param heapPtr address: A memory address.
	:param number value: The value to put at this address.

	Example::

		(Memory memPOKE (+ polyPointArray 2) 137)



