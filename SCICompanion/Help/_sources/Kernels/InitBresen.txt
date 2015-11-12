.. InitBresen

.. default - domain::js

.. include:: /includes/standard.rst

===================
InitBresen (Kernel)
===================

.. function:: InitBresen(mover [stepMult])

	
	Initializes the mover object for its movement to its x and y coordinates. It uses the Bresenham line algorithm to figure out how much to move each step resulting in a direct path from its client's coordinates to its x and y.
	
	:param heapPtr mover: An instance of a :class:`Motion` class.
	:param number stepMult: Optional multiplier.
	
	If stepMult is specified, it will move xStep \* stepMult and yStep \* stepMult.
	
	See also: :func:`DoBresen`.


