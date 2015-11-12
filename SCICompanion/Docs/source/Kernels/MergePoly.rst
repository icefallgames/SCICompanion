.. MergePoly

.. default - domain::js

.. include:: /includes/standard.rst

==================
MergePoly (Kernel)
==================


.. function:: MergePoly(points polygons polyCount)

	.. IMPORTANT:: SCI1.1 only.

	This takes a polygon, and extends it to also cover any polygons from the
	input list with which it intersects. Any of those polygons so covered
	from the input list are marked by adding 0x10 to their type field.

	:param heapPtr points: Buffer of polygon points, with a ($7777, $7777) sentinel point at the end.
	:param k_list polygons: A list of polygons.
	:param number polyCount: The number of polygons in the list.

	Example::

		(var newPoints)
		(= newPoints MergePoly(points (send obstacles:elements) (send obstacles:size)))
		(if (newPoints)
			// Do something with the new points, etc...
			Memory(memFREE newPoints)
		)


