.. PChase

.. default - domain::js

.. include:: /includes/standard.rst

=============================
PChase (of :class:`PolyPath`)
=============================

.. class:: PChase

	Defined in PChase.sc.

	
	A motion class that is used to make an Actor get within a certain distance of another (possibly moving) object, while avoiding polygon obstacles.
	
	PChase is generally used as an argument for setMotion::
	
		// Make the peasant approach the merchant, within 20 pixels.
		(peasant:setMotion(PChase theMerchant 20))


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

Defined in PChase:

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


.. function:: init(theClient theWho [theDistance theCaller theObstacles])
	:noindex:

	Initializes the PFollow instance.

	:param heapPtr theClient: The :class:`Actor` to which this is attached.
	:param heapPtr theWho: The target to follow.
	:param number theDistance: How close the client needs to get from the target.
	:param heapPtr theCaller: The object on which cue() will be called when the target is reached.
	:param heapPtr theObstacles: Optional list of polygon obstacles. If not provided, the room's obstacles are used.



.. function:: doit()
	:noindex:



.. function:: moveDone()
	:noindex:



