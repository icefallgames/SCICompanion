.. Actor

.. default - domain::js

.. include:: /includes/standard.rst

========================
Actor (of :class:`Prop`)
========================

.. class:: Actor

	Defined in Actor.sc.

	
	Actor is the base class for moving objects in your game. It extends :class:`Prop` by providing the following additional capabilities:
	
		- A mover property that is responsible for controlling how the Actor moves. This is assigned with setMotion().
		- An optional Avoider that makes the Actor avoid objects.
		- Optional "blocks" that indicate areas the Actor can or can't be.
	
	Example definition::
	
		(instance wd40 of Actor
			(properties
				x 20
				y 20
				noun N_ROBOT
				view 400
				loop 8
				signal ignAct
			)
		)
	
	Example initialization::
	
		(wd40:
			init()
			setMotion(PolyPath 127 128)
		)
	


Subclasses: :class:`Ego`.

Properties
==========

Inherited from :class:`Prop`:

============== =========================================================
Property       Description                                              
============== =========================================================
x              x position. See posn().                                  
y              y position. See posn().                                  
z              z position. See posn().                                  
heading        The angle direction the Actor faces.                     
noun           The noun for the Actor (for messages).                   
modNum         Module number (for messages)                             
nsTop          "Now seen" rect. The visual bounds of the Actor.         
nsLeft                                                                  
nsBottom                                                                
nsRight                                                                 
sightAngle                                                              
actions                                                                 
onMeCheck      The type of onMe checks that are done.                   
state                                                                   
approachX      The approach spot x.                                     
approachY      The approach spot y.                                     
approachDist   The approach distance.                                   
_approachVerbs Bitmask indicating which verbs cause the ego to approach.
yStep          The number of pixels moved in the y direction each cycle.
view           The view for Actor.                                      
loop                                                                    
cel                                                                     
priority                                                                
underBits                                                               
signal                                                                  
lsTop          The "last seen" rect...                                  
lsLeft                                                                  
lsBottom                                                                
lsRight                                                                 
brTop          The "bounds rect" (near the feet of the Actor).          
brLeft                                                                  
brBottom                                                                
brRight                                                                 
scaleSignal                                                             
scaleX         Current x scale.                                         
scaleY         Current y scale.                                         
maxScale       Max scale.                                               
cycleSpeed     How quickly the Actor animation cycles.                  
script         Arbitrary :class:`Script` object.                        
cycler         :class:`Cycle` attached to the Actor.                    
timer                                                                   
detailLevel                                                             
scaler         :class:`Scaler` object attached to the Actor.            
============== =========================================================

Defined in Actor:

=========== =================================================
Property    Description                                      
=========== =================================================
illegalBits                                                  
xLast                                                        
yLast                                                        
xStep                                                        
origStep                                                     
moveSpeed   How quickly the Actor moves.                     
blocks                                                       
baseSetter                                                   
mover       The :class:`Motion` object attached to the Actor.
looper      Optional looper code.                            
viewer                                                       
avoider                                                      
code                                                         
=========== =================================================


Methods
==========

.. function:: init(params)
	:noindex:



.. function:: doit()
	:noindex:




.. function:: posn(theX theY [theZ])
	:noindex:

	Sets the position of the Actor.




.. function:: setLoop(loop)
	:noindex:

.. function:: setLoop(loopClass [params ...])
	:noindex:

.. function:: setLoop(looper [params ...])
	:noindex:

	Sets a loop on the Actor, or sets an object that controls which loop is used.

	:param number loop: A loop number.
	:param class loopClass: A class that has a doit method that controls its client loop (e.g. see :class:`Grooper`).
	:param heapPtr looper: An instance of a looper class.



.. function:: delete()
	:noindex:



.. function:: motionCue()
	:noindex:



.. function:: checkDetail(param1)
	:noindex:



.. function:: setMotion(theMover sendParams)
	:noindex:

	
	Assigns a mover object to the Actor. The mover is initialized with the Actor and any
	sendParams that have been provided.
	
	:param theMover: A class name, or an instance that inherits from :class:`Motion`.
	:param sendParams: Any params that should be forwarded to the mover's init() method.
	
	Movers control the Actor's motion, whether it be via
	mouse or keyboard input, or some in-game logic.
	
	Example usage for moving a ball to a particular position, and cueing the caller when it's done::
	
		(myBall:setMotion(MoveTo 123 100 self))
	


.. function:: setAvoider(theAvoider sendParams)
	:noindex:



.. function:: ignoreHorizon(param1)
	:noindex:



.. function:: observeControl(bits)
	:noindex:

	
	Specifies the control colors which the Actor's movement. This is not used commonly in SCI1.1. Constraining
	an Actor's motion is generally done with Polygons instead.
	


.. function:: ignoreControl(bits)
	:noindex:

	
	Specifies which control colors should no longer block the Actor's movement.
	


.. function:: observeBlocks(block)
	:noindex:

	
	Adds a block (an instance which inherits from :class:`Blk`) to the Actor's list of blocks.
	These control where an Actor is allowed to go. In SCI1.1, these have generally been replaced by Polygons.
	


.. function:: ignoreBlocks(block)
	:noindex:

	Removes a block from the Actor's list of blocks.


.. function:: isStopped()
	:noindex:

	Returns TRUE if the Actor is not moving, FALSE otherwise.


.. function:: isBlocked()
	:noindex:



.. function:: inRect(left top right bottom)
	:noindex:

	Returns TRUE if the Actor is inside the specified rectangle, FALSE otherwise.



.. function:: onControl([fUsePoint])
	:noindex:

	Provides a bitmask of the control colors on which an Actor is located.

	:param boolean fUsePoint: If TRUE, the Actor's location is used. If FALSE (or not specified), the Actor's base rectangle (near its feet) is used.
	:returns: A bitmask of ctl flags. These should usually be tested with the & operator.

	Example usage::

		(if (& ctlGREEN (send gEgo:onControl()))
			Print("The ego is on Green")
		)



.. function:: distanceTo(obj)
	:noindex:

	
	:param heapPtr obj: An object with x and y properties.
	:returns: the distance between this Actor and the object.
	


.. function:: cantBeHere()
	:noindex:

	Returns TRUE if the Actor can't be in its current location.



.. function:: setStep(newX newY [fDontSetOrigStep])
	:noindex:

	Sets the pixel increments in which the Actor moves. Bigger increments means the Actor will cover
	larger distances in each frame.

	:param number newX: The xStep, or -1 if not provided.
	:param number newY: The yStep, or -1 if not provided.
	:param boolean fDontSetOrigStep: Optional flag telling us not to set origStep.



.. function:: setDirection(newDirection)
	:noindex:

	
	Sets the direction that the Actor faces.
	
	:param number newDirection: One of CENTER, UP, UPRIGHT, RIGHT, DOWNRIGHT, DOWN, DOWNLEFT, LEFT or UPLEFT.
	


.. function:: setHeading(theHeading cueObj cueValues)
	:noindex:

	Sets the angle heading of the Actor.


.. function:: setSpeed(newSpeed)
	:noindex:

	Sets the speed of the Actor. This controls both the move and cycle speed.


