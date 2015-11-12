.. Follow

.. default - domain::js

.. include:: /includes/standard.rst

===========================
Follow (of :class:`Motion`)
===========================

.. class:: Follow

	Defined in Follow.sc.

	
	A motion class that is used to make an Actor move so it stays within a certain distance of another (possibly moving) object.
	
	Example usage::	
	
		// Make the dog follow the ego at a distance of 20
		(aDog:setMotion(Follow gEgo 20))
	
	Note that this class doesn't intelligently move the Actor around polygon obstacles. For that, use :class:`PFollow`.


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

Defined in Follow:

======== ===========
Property Description
======== ===========
who                 
distance            
======== ===========


Methods
==========


.. function:: init(theClient theWho [theDistance])
	:noindex:

	Initializes the Follow instance.

	:param heapPtr theClient: The :class:`Actor` to which this is attached.
	:param heapPtr theWho: The target to chase.
	:param number theDistance: How close the client needs to get from the target.



.. function:: doit()
	:noindex:



.. function:: moveDone()
	:noindex:



.. function:: setTarget(param1)
	:noindex:



.. function:: onTarget()
	:noindex:



