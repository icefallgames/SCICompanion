.. Sort

.. default - domain::js

.. include:: /includes/standard.rst

=============
Sort (Kernel)
=============


.. function:: Sort(source dest orderFunction)

	.. IMPORTANT:: SCI1.1 only.

	Sorts a list and puts the result into another list.

	:param k_list source: The list of items to sort.
	:param k_list dest: The destination list. The list may be the same as the source list.
	:param heapPtr orderFunction: An instance of an object with a doit() method (e.g. :class:`Code`) that returns a numerical value indicating order.

	Example::

		(instance sortByDistanceToEgo of Code
			(properties)
			(method (doit theObj)
				return (send gEgo:distanceTotheObj))
			)
		)

		// Then somewhere in the script:
		Sort(myEventHandler myEventHandler sortByDistanceToEgo)	


