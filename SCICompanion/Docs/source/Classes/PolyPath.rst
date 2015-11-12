.. PolyPath

.. default - domain::js

.. include:: /includes/standard.rst

=============================
PolyPath (of :class:`Motion`)
=============================

.. class:: PolyPath

	Defined in PolyPath.sc.

	
	A Motion class that is used to move an Actor to a destination while following a path that avoids polygon obstacles. This is usually what is used
	to move the ego when the player clicks on a destination. See the init method for more details on parameters.
	
	Example usage::
	
		// Make the thief move to (255, 146), then cue the current object when done.
		(theThief:setMotion(PolyPath 255 146 self))


Subclasses: :class:`PChase`, :class:`PFollow`, :class:`MoveFwd`.

Properties
==========

Inherited from :class:`Motion`:

========= =============================================================
Property  Description                                                  
========= =============================================================
client    The :class:`Actor` to which this is attached.                
caller    The object that will get cue()'d when the motion is complete.
x                                                                      
y                                                                      
dx                                                                     
dy                                                                     
b-moveCnt                                                              
b-i1                                                                   
b-i2                                                                   
b-di                                                                   
b-xAxis                                                                
b-incr                                                                 
completed                                                              
xLast                                                                  
yLast                                                                  
========= =============================================================

Defined in PolyPath:

========= ===========
Property  Description
========= ===========
value                
points               
finalX               
finalY               
obstacles            
========= ===========


Methods
==========


.. function:: init(theClient theFinalX theFinalY [theCaller optimizationLevel theObstacles])
	:noindex:

	Initializes the PolyPath instance.

	:param heapPtr theClient: The :class:`Actor` to which this is attached.
	:param number theFinalX: The target x coordinate.
	:param number theFinalY: The target y coordinate.
	:param heapPtr theCaller: The object on which cue() will be called when the target is reached.
	:param number optimizationLevel: This appears to be unused.
	:param heapPtr theObstacles: Optional list of polygon obstacles. If not provided, the room's obstacles are used.



.. function:: dispose()
	:noindex:



.. function:: moveDone()
	:noindex:



.. function:: setTarget()
	:noindex:



