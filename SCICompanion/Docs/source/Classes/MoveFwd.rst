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
		(gEgo setMotion: MoveFwd 50 self)


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
		MoveFwd [color=greenyellow]
	}

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
name                 
========= ===========


Methods
==========


.. function:: init(theClient distance [theCaller])
	:noindex:

	Initializes the MoveFwd instance.

	:param heapPtr theClient: The :class:`Actor` to which this is attached.
	:param number distance: The distance to move forward.
	:param heapPtr theCaller: Optional object on which cue() will be called when the target is reached.



