.. Jump

.. default - domain::js

.. include:: /includes/standard.rst

=========================
Jump (of :class:`Motion`)
=========================

.. class:: Jump

	Defined in Jump.sc.

	
	This serves as a base class for the :class:`JumpTo` motion.


Subclasses: :class:`JumpTo`.

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
		Jump [color=greenyellow]
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

Defined in Jump:

=========== ===========
Property    Description
=========== ===========
gx                     
gy                     
xStep                  
yStep                  
signal                 
illegalBits            
waitApogeeX            
waitApogeeY            
=========== ===========


Methods
==========

.. function:: init(theClient theCaller)
	:noindex:



.. function:: doit()
	:noindex:



.. function:: moveDone()
	:noindex:



.. function:: motionCue()
	:noindex:



.. function:: setTest()
	:noindex:



