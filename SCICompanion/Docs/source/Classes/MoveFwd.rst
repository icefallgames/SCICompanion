.. MoveFwd

.. default - domain::js

.. include:: /includes/standard.rst

==============================
MoveFwd (of :class:`PolyPath`)
==============================

.. class:: MoveFwd

	Defined in MoveFwd.sc.

	
	This is a :class:`Motion` class that moves an Actor forward some amount.
	
	Example usage::
	
		// Move the ego forward 50 pixels, and cue the calling script when done.
		(send gEgo:setMotion(MoveFwd 50 self))


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


Methods
==========


.. function:: init(theClient distance [theCaller])
	:noindex:

	Initializes the MoveFwd instance.

	:param heapPtr theClient: The :class:`Actor` to which this is attached.
	:param number distance: The distance to move forward.
	:param heapPtr theCaller: Optional object on which cue() will be called when the target is reached.



