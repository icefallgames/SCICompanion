.. JumpTo

.. default - domain::js

.. include:: /includes/standard.rst

=========================
JumpTo (of :class:`Jump`)
=========================

.. class:: JumpTo

	Defined in Jump.sc.

	
	This makes an :class:`Actor` jump to a certain location.
	
	Example usage::
	
		(theRobot:setMotion(JumpTo 240 150))


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
		JumpTo [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Jump`:

=========== ===========
Property    Description
=========== ===========
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


.. function:: init(theClient theX theY [theCaller])
	:noindex:

	Initializes the JumpTo motion.

	:param heapPtr theClient: The :class:`Actor` to which this applies.
	:param number theX: The x target.
	:param number theY: The y target.
	:param heapPtr theCaller: The optional object that will be cue()'d when the jump is complete.




.. function:: moveDone()
	:noindex:



