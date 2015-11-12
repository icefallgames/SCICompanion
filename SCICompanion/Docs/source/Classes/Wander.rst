.. Wander

.. default - domain::js

.. include:: /includes/standard.rst

===========================
Wander (of :class:`Motion`)
===========================

.. class:: Wander

	Defined in Wander.sc.

	
	The Wander mover makes an Actor wander around aimlessly.
	
	Example usage::
	
		// Make the man wander, up to a distance of 20 pixels from each place he stops.
		(theMan:setMotion(Wander 20))
	
	Note that this class won't intelligently direct an Actor around polygon obstacles.


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

Defined in Wander:

======== ===========
Property Description
======== ===========
distance            
======== ===========


Methods
==========


.. function:: init(theClient theDistance)
	:noindex:

	:param heapPtr theClient: The :class:`Actor` to which this is attached.
	:param number theDistance: The maximum distance to wander each time.



.. function:: doit()
	:noindex:



.. function:: moveDone()
	:noindex:



.. function:: setTarget()
	:noindex:



.. function:: onTarget()
	:noindex:



