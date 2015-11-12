.. ResCheck

.. default - domain::js

.. include:: /includes/standard.rst

=================
ResCheck (Kernel)
=================


.. function:: ResCheck(resourceType moduleNumber noun verb condition sequence)

.. function:: ResCheck(resourceType resourceNumber)
	:noindex:

	.. IMPORTANT:: SCI1.1 only.

	Checks if a resource is present.

	:param number resourceType: The type of the resource (rsVIEW, rsPIC, rsAUDIO36, etc...)
	:param number resourceNumber: The resource number.
	:param number moduleNumber: The module number (room number).
	:param number noun: The message noun.
	:param number verb: The message verb.
	:param number condition: The message condition.
	:param number sequence: The message sequence number.
	:returns: TRUE if the resource is present, otherwise FALSE.

	Example::

		// Do we have an audio resource for this message?
		(if (ResCheck(rsAUDIO36 moduleNumber noun verb condition sequence))
			DoAudio(audPLAY moduleNumber noun verb condition sequence)
		)



