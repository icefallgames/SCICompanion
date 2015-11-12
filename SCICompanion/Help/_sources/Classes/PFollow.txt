.. PFollow

.. default - domain::js

.. include:: /includes/standard.rst

==============================
PFollow (of :class:`PolyPath`)
==============================

.. class:: PFollow

	Defined in PFollow.sc.

	
	A motion class that is used to make an Actor stay within a certain distance of another (possibly moving) object, while avoiding polygon obstacles.
	This is similar to :class:`PChase`, except that the Actor doesn't stop moving when it reaches the distance target.
	
	PFollow is generally used as an argument for setMotion::
	
		// Make the dog follow the ego at a distance of 20
		(aDog:setMotion(PFollow gEgo 20))


Properties
==========

Inherited from :class:`PolyPath`:

========= ===========
Property  Description
========= ===========
client               
caller               
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
value                
points               
finalX               
finalY               
obstacles            
========= ===========

Defined in PFollow:

======== ===========
Property Description
======== ===========
who                 
distance            
targetX             
targetY             
======== ===========


Methods
==========


.. function:: init(theClient theWho [theDistance theObstacles])	
	:noindex:

	Initializes the PFollow instance.

	:param heapPtr theClient: The :class:`Actor` to which this is attached.
	:param heapPtr theWho: The target to follow.
	:param number theDistance: How close the client needs to get from the target.
	:param heapPtr theObstacles: Optional list of polygon obstacles. If not provided, the room's obstacles are used.



.. function:: doit()
	:noindex:



.. function:: moveDone()
	:noindex:



