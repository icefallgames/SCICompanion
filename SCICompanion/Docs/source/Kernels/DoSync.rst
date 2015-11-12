.. DoSync

.. default - domain::js

.. include:: /includes/standard.rst

===============
DoSync (Kernel)
===============


	DoSync handles lip-sync functions.

	.. IMPORTANT:: SCI1.1 only.

.. function:: DoSync(syncSTART syncObject moduleNumber noun verb condition sequence)

.. function:: DoSync(syncSTART syncObject resourceNumber)
	:noindex:

	Associates an object with a lip-sync resource.

	:param heapPtr syncObject: An instance of :class:`Sync` that will receive lip-sync information.
	:param number resourceNumber: The resource number.
	:param number moduleNumber: The module number (room number).
	:param number noun: The message noun.
	:param number verb: The message verb.
	:param number condition: The message condition.
	:param number sequence: The message sequence number.

.. function:: DoSync(syncNEXT syncObject)
	:noindex:

	Updates the :class:`Sync` object with current information from the lip-sync resource.

	:param heapPtr syncObject: An instance of :class:`Sync` that will receive lip-sync information.

.. function:: DoSync(syncSTOP)
	:noindex:

	Stops the lip-sync operation.



