.. MoveTo

.. default - domain::js

.. include:: /includes/standard.rst

===========================
MoveTo (of :class:`Motion`)
===========================

.. class:: MoveTo

	Defined in Cycle.sc.

	
	MoveTo is the simplest of movers. It moves an Actor to a destination, then stops.
	If the Actor needs to maneuver around room obstacles, :class:`PolyPath` should be used instead.
	
	Example usage::
	
		// Move a bird to (200, 37), then call the cue() method on us.	
		(bird:setMotion(MoveTo 200 37 self))


Subclasses: :class:`RegionPath`.

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


Methods
==========

.. function:: onTarget()
	:noindex:



