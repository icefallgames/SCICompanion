.. CreateNewPolygon

.. default - domain::js

.. include:: /includes/standard.rst

==========================
CreateNewPolygon (Main.sc)
==========================


.. function:: CreateNewPolygon(polyBuffer [nextPolyOptional])

	Creates a new polygon object.
	
	:param heapPtr polyBuffer: An array with polygon points.
	:param heapPtr nextPolyOptional: An optional pointer that receives the position of the next polygon in the buffer.
	
	Example usage::
	
		(aRock setOnMeCheck: omcPOLYGON (CreateNewPolygon @P_Rock))

	The array consists of the following:	
	
		- A number expressing the type of the polygon (e.g. PBarredAccess).
		- A number indicating how many points are in the polygon.
		- (x y) pairs of numbers for each point.
		
	Example::
	
		[P_Rock 10] = [PContainedAccess 4 319 189 319 50 0 50 0 189]
		
	See also: :doc:`/polygons`.


