.. Door

.. default - domain::js

.. include:: /includes/standard.rst

=======================
Door (of :class:`Prop`)
=======================

.. class:: Door

	Defined in Door.sc.

	
	Door offers support for automatically managing the open and close state of the door, and optionally
	offering access to another room.
	
	Example definition::
	
		(instance frontDoor of Door
			(properties
				x 39
				y 167
				noun N_FRONTDOOR
				approachX 36
				approachY 175
				view 210
				entranceTo 230
				moveToX 22
				moveToY 169
				enterType 0
				exitType 0
			)
		)


.. blockdiag::
	:alt: class diagram
	:width: 600

	diagram {
		default_fontsize = 16
		orientation = portrait;
		Feature -> View
		View -> Prop
		Prop -> Narrator
		Prop -> Actor
		Prop -> Door
		Actor -> Ego
		Ego -> GameEgo
		Narrator -> Talker
		Narrator -> ChoiceNarrator
		Talker -> PriorityTalker
		Talker -> ChoiceTalker
		Door [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Prop`:

============== ===========
Property       Description
============== ===========
x                         
y                         
z                         
heading                   
noun                      
modNum                    
nsTop                     
nsLeft                    
nsBottom                  
nsRight                   
sightAngle                
actions                   
onMeCheck                 
state                     
approachX                 
approachY                 
approachDist              
_approachVerbs            
yStep                     
view                      
loop                      
cel                       
priority                  
underBits                 
signal                    
lsTop                     
lsLeft                    
lsBottom                  
lsRight                   
brTop                     
brLeft                    
brBottom                  
brRight                   
scaleSignal               
scaleX                    
scaleY                    
maxScale                  
cycleSpeed                
script                    
cycler                    
timer                     
detailLevel               
scaler                    
_case                     
name                      
============== ===========

Defined in Door:

=========== ================================================================
Property    Description                                                     
=========== ================================================================
entranceTo  Entrance to which room?                                         
locked      Is the door locked?                                             
lockedCase  The message condition to use when displaying a "locked" message.
openSnd     Open sound number.                                              
closeSnd    Close sound number.                                             
openVerb    The verb to use for opening and closing the door.               
listenVerb  The verb to use for listening.                                  
doubleDoor                                                                  
forceOpen                                                                   
forceClose                                                                  
caller                                                                      
moveToX                                                                     
moveToY                                                                     
enterType                                                                   
exitType                                                                    
closeScript                                                                 
openScript                                                                  
doorPoly                                                                    
polyAdjust  Amount to inflate base rect when creating a polygon.            
=========== ================================================================


Methods
==========

.. function:: init()
	:noindex:



.. function:: dispose()
	:noindex:



.. function:: doVerb(theVerb)
	:noindex:



.. function:: cue()
	:noindex:



.. function:: open()
	:noindex:

	Opens the door if it's not locked.


.. function:: close()
	:noindex:

	Closes the door.


.. function:: listen()
	:noindex:

	Override this method in your door instance to provide logic for listening.



.. function:: createPoly([polyPoints])
	:noindex:

	Adds a :class:`Polygon` for the door to the gAltPolyList.

	:param number polyPoints: A list of points for the polygon. If none are provided, the Door's base rect is used.



