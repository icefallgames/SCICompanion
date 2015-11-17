.. Track

.. default - domain::js

.. include:: /includes/standard.rst

==========================
Track (of :class:`Motion`)
==========================

.. class:: Track

	Defined in Track.sc.

	
	This Motion class appears to simply make the :class:`Actor` face in the direction of a target.


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
		Track [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Motion`:

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
========= ===========

Defined in Track:

======== ===========
Property Description
======== ===========
who                 
xOffset             
yOffset             
zOffset             
======== ===========


Methods
==========

.. function:: init(theClient theWho theXOffset theYOffset theZOffset theCaller)
	:noindex:



.. function:: doit()
	:noindex:



