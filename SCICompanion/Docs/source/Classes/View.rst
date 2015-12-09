.. View

.. default - domain::js

.. include:: /includes/standard.rst

==========================
View (of :class:`Feature`)
==========================

.. class:: View

	Defined in Actor.sc.

	
	The View class is an essential part of SCI games. It is the base class for :class:`Prop`, :class:`Actor` and the like.
	It extends :class:`Feature` by providing the ability to be dynamically positioned at different places, and by automatically setting
	its bounds based on its view, loop and cel. It is often (though not exclusively) used to add static views to the background, via
	its addToPic method.
	
	Example definition::
	
		(instance ship of View
			(properties
				x 6
				y 92
				noun N_SHIP
				view 113
				loop 2
				cel 1
				signal ignAct	// Don't interact with Actors
			)
		)
	
	Example initialization::
	
		(ship init:)
	


Subclasses: :class:`Prop`.

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
		View [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Feature`:

============== =========================================================
Property       Description                                              
============== =========================================================
x              x position. See posn().                                  
y              y position. See posn().                                  
z              z position. See posn().                                  
heading        The angle direction the View faces.                      
noun           The noun for the View (for messages).                    
modNum         Module number (for messages)                             
nsTop          "Now seen" rect. The visual bounds of the View.          
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
name                                                                    
============== =========================================================

Defined in View:

=========== =========================
Property    Description              
=========== =========================
yStep                                
view        The view number for View.
loop        The loop of the View.    
cel         The cel of the View.     
priority    The priority of the View.
underBits                            
signal                               
lsTop       The "last seen" rect.    
lsLeft                               
lsBottom                             
lsRight                              
brTop       The "base rect".         
brLeft                               
brBottom                             
brRight                              
scaleSignal                          
scaleX      Current x scale.         
scaleY      Current y scale.         
maxScale    Max scale.               
=========== =========================


Methods
==========

.. function:: init()
	:noindex:



.. function:: dispose()
	:noindex:



.. function:: showSelf()
	:noindex:

	Prints a description of the View on the screen.


.. function:: isNotHidden()
	:noindex:




.. function:: onMe(x y)
	:noindex:

.. function:: onMe(obj)
	:noindex:

	Determines if an object or an (x, y) coordinate lies within the View.

	:param heapPtr obj: An object with x and y properties.
	:returns: TRUE if it's on the View, otherwise FALSE.




.. function:: posn(theX theY [theZ])
	:noindex:

	Sets the position of the View and updates its base rectangle.



.. function:: stopUpd()
	:noindex:



.. function:: forceUpd()
	:noindex:



.. function:: startUpd()
	:noindex:



.. function:: setPri(thePriority)
	:noindex:

	
	:param number thePriority: The new priority of the View. If -1 is specified, the View updates its priority based on its y coordinate.
	


.. function:: setLoop(theLoop)
	:noindex:

	Sets the loop of the View.


.. function:: setCel(theCel)
	:noindex:

	Sets the cel of the View.


.. function:: ignoreActors(shouldIgnore)
	:noindex:

	
	Tells the View if it should or should not interact with (bump into) other objects.
	
	:param boolean shouldIgnore: If TRUE (or not specified), the View should ignore other objects.
	


.. function:: hide()
	:noindex:

	Hides the View.


.. function:: show()
	:noindex:

	Shows the View.


.. function:: delete()
	:noindex:



.. function:: addToPic()
	:noindex:

	Draws the View permanently on the background.


.. function:: lastCel()
	:noindex:

	Returns the last valid cel of the View.


.. function:: motionCue()
	:noindex:



.. function:: checkDetail()
	:noindex:



.. function:: setScale(theScale)
	:noindex:

	Sets the scale of the View.


