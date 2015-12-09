.. Motion

.. default - domain::js

.. include:: /includes/standard.rst

===========================
Motion (of :class:`Object`)
===========================

.. class:: Motion

	Defined in Cycle.sc.

	
	Motion is the base class for movers: objects that are responsible for moving Actors around the screen.


Subclasses: :class:`Wander`, :class:`DPath`, :class:`MoveTo`, :class:`Approach`, :class:`Orbit`, :class:`Follow`, :class:`PolyPath`, :class:`Track`, :class:`Chase`, :class:`Jump`.

.. blockdiag::
	:alt: class diagram
	:width: 600

	diagram {
		default_fontsize = 16
		Motion -> Wander
		Motion -> DPath
		Motion -> MoveTo
		Motion -> Approach
		Motion -> Orbit
		Motion -> Follow
		Motion -> PolyPath
		Motion -> Track
		Motion -> Chase
		Motion -> Jump
		Jump -> JumpTo
		PolyPath -> PChase
		PolyPath -> PFollow
		PolyPath -> MoveFwd
		MoveTo -> RegionPath
		DPath -> RelDPath
		Motion [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Object`:

======== ===========
Property Description
======== ===========
name                
======== ===========

Defined in Motion:

========= =============================================================
Property  Description                                                  
========= =============================================================
client    The :class:`Actor` to which this is attached.                
caller    The object that will get cue()'d when the motion is complete.
x                                                                      
y                                                                      
dx        Step size.                                                   
dy                                                                     
b-moveCnt                                                              
b-i1                                                                   
b-i2                                                                   
b-di                                                                   
b-xAxis   Which axis is the motion along?                              
b-incr    Bresenham adjustment.                                        
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



