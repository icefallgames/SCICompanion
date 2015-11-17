.. DPath

.. default - domain::js

.. include:: /includes/standard.rst

==========================
DPath (of :class:`Motion`)
==========================

.. class:: DPath

	Defined in DPath.sc.

	
	DPath (or "directed path") uses a list of locations to direct the :class:`Actor` towards its destination.
	
	Example usage::
	
		// Send the hoverGuys along 3 points to the destination, then cue() the caller.
		(hoverGuys:setMotion(DPath 255 137 154 138 140 137 self))


Subclasses: :class:`RelDPath`.

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
		DPath [color=greenyellow]
	}

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

Defined in DPath:

======== ===========
Property Description
======== ===========
points              
value               
======== ===========


Methods
==========


.. function:: init(theClient [points... theCaller])
	:noindex:

	:param heapPtr theClient: The Actor to which this is attached.
	:param number points: Pairs of (x, y) coordinates.
	:param heapPtr theCaller: Object that will have its cue() method called when the Actor reaches the target.



.. function:: dispose()
	:noindex:



.. function:: moveDone()
	:noindex:



.. function:: setTarget()
	:noindex:



