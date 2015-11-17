.. AddPolygonsToRoom

.. default - domain::js

.. include:: /includes/standard.rst

===========================
AddPolygonsToRoom (Main.sc)
===========================

.. function:: AddPolygonsToRoom(polyBuffer)

	
	Creates :class:`Polygon` objects based on the point lists in polyBuffer and adds
	them to the room's obstacles.
	
	:param heapPtr polyBuffer: An array with polygon points.
	
	Example usage::
	
		AddPolygonsToRoom(@P_ThePolygons)
	
	The array begins with a number indicating how many polygons there are. This is followed
	by the following information for each polygon:
	
		- A number expressing the type of the polygon (e.g. PBarredAccess).
		- A number indicating how many points are in the polygon.
		- (x y) pairs of numbers for each point.
	
	Example::
	
		P_ThePolygons[19] = (2 PContainedAccess 4 319 189 319 50 0 50 0 189 PBarredAccess 3 319 189 319 50 0 50)
	
	See also: :doc:`/polygons`.		


