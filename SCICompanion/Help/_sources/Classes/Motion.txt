.. Motion

.. default - domain::js

.. include:: /includes/standard.rst

========================
Motion (of :class:`Obj`)
========================

.. class:: Motion

	Defined in Cycle.sc.

	
	Motion is the base class for movers: objects that are responsible for moving Actors around the screen.


Subclasses: :class:`Wander`, :class:`DPath`, :class:`MoveTo`, :class:`Approach`, :class:`Orbit`, :class:`Follow`, :class:`PolyPath`, :class:`Track`, :class:`Chase`, :class:`Jump`.

Properties
==========

Defined in Motion:

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


.. function::init(theClient theX theY [theCaller])
	:noindex:

	:param heapPtr theClient: The :class:`Actor` to which this is attached.
	:param number theX: The x destination.
	:param number theY: The y destination.
	:param heapPtr theCaller: Optional object on which we call cue() when the move is finished.



.. function:: doit()
	:noindex:



.. function:: moveDone()
	:noindex:



.. function:: setTarget(theX theY)
	:noindex:



.. function:: onTarget()
	:noindex:



.. function:: motionCue()
	:noindex:



