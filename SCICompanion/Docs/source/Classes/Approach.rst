.. Approach

.. default - domain::js

.. include:: /includes/standard.rst

=============================
Approach (of :class:`Motion`)
=============================

.. class:: Approach

	Defined in Approach.sc.

	
	A motion class that causes the :class:`Actor` to move approach a target until within a certain distance, and then stop.
	This Motion class ignores the room obstacles. In SCI1.1, if path-finding is required, one would generally use the :class:`PolyPath` class instead.


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
		Approach [color=greenyellow]
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
name                                                                   
========= =============================================================

Defined in Approach:

======== ===========
Property Description
======== ===========
who                 
distance            
======== ===========


Methods
==========


.. function:: init(theClient theWho theDistance)
	:noindex:

	Initializes the Approach instance.

	:param heapPtr theClient: The :class:`Actor` to which this is attached.
	:param heapPtr theWho: The target object.
	:param number theDistance: How close the Actor needs to get to theWho before stopping.



.. function:: doit()
	:noindex:



.. function:: setTarget(param1)
	:noindex:



.. function:: onTarget()
	:noindex:



