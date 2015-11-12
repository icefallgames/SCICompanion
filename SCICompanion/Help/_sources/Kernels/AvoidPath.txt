.. AvoidPath

.. default - domain::js

.. include:: /includes/standard.rst

==================
AvoidPath (Kernel)
==================


.. function:: AvoidPath(x y polygon)

	.. IMPORTANT:: SCI1.1 only.

	Determines if a point is inside the given polygon.

	:param number x: The x coordinate of the point.
	:param number y: The y coordinate of the point.
	:param heapPtr polygon: An instance of :class:`Polygon`.
	:returns: TRUE if the point is inside the polygon, otherwise FALSE.

.. function:: AvoidPath(xStart yStart xEnd yEnd polygons polyCount [options])
	:noindex:

	Calculates a path between two points given the polygon obstacles. Returns a memory
	address pointing to an array of points. The memory address returned must be freed with Memory(memFREE).

	The returned array consists of (x, y) pairs of points, followed by a sentinel ending point ($7777, $7777).

	:param number xStart: The x coordinate of the starting point.
	:param number yStart: The y coordinate of the starting point.
	:param number xEnd: The x coordinate of the destination.
	:param number yEnd: The y coordinate of the destination.
	:param k_list polygons: A list of polygons.
	:param number polyCount: The number of polygons in the list.
	:param number options: Optimization levels. Optimization level 0 is for keyboard support, and changes some of the polygons.
	:returns: A list of points comprising the path, or NULL on failure. The points should be freed by Memory(memFREE).



