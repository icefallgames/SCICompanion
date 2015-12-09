.. Prop

.. default - domain::js

.. include:: /includes/standard.rst

=======================
Prop (of :class:`View`)
=======================

.. class:: Prop

	Defined in Actor.sc.

	
	Prop extends :class:`View` by providing the following additional abilities:
	
		- cycling through animation frames
		- attach Scripts
		- attach a Scaler
	
	Example definition::
	
		(instance monitor of Prop
			(properties
				x 13
				y 161
				noun N_MONITOR
				view 1142
				loop 1
				priority 15
				signal fixPriOn
				cycleSpeed 14
			)
		)
	
	Example initialization::
	
		(monitor:
			init:
			setCycle: Forward
			setScript: sPlayMC
		)


Subclasses: :class:`Narrator`, :class:`Actor`, :class:`Door`.

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
		Prop [color=greenyellow]
	}

Properties
==========

Inherited from :class:`View`:

============== =========================================================
Property       Description                                              
============== =========================================================
x              x position. See posn().                                  
y              y position. See posn().                                  
z              z position. See posn().                                  
heading        The angle direction the Prop faces.                      
noun           The noun for the Prop (for messages).                    
modNum         Module number (for messages)                             
nsTop          "Now seen" rect. The visual bounds of the Prop.          
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
yStep                                                                   
view           The view for Prop.                                       
loop           Loop for the Prop.                                       
cel            Current cel of the Prop.                                 
priority       Priority screen value of the Prop.                       
underBits                                                               
signal                                                                  
lsTop          The "last seen" rect...                                  
lsLeft                                                                  
lsBottom                                                                
lsRight                                                                 
brTop          The "base rect", used for collison detection.            
brLeft                                                                  
brBottom                                                                
brRight                                                                 
scaleSignal                                                             
scaleX         Current x scale.                                         
scaleY         Current y scale.                                         
maxScale       Max scale.                                               
name                                                                    
============== =========================================================

Defined in Prop:

=========== ============================================
Property    Description                                 
=========== ============================================
cycleSpeed  How quickly the Prop animation cycles.      
script      Arbitrary :class:`Script` object.           
cycler      :class:`Cycle` attached to the Prop.        
timer                                                   
detailLevel                                             
scaler      :class:`Scaler` object attached to the Prop.
=========== ============================================


Methods
==========

.. function:: doit()
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:

	Lets the Prop's script have a chance at handling the event.


.. function:: delete()
	:noindex:



.. function:: motionCue()
	:noindex:



.. function:: checkDetail(param1)
	:noindex:




.. function:: setScale(class params)
	:noindex:

.. function:: setScale(obj params)
	:noindex:

.. function:: setScale(-1 otherObj)
	:noindex:

.. function:: setScale(scale)
	:noindex:

	Provides various ways to control the scaling of a Prop. See :class:`Scaler` and :class:`ScaleTo`.

	Example usage for attaching a dynamic scaler::

		(gEgo setScale: Scaler frontSize backSize frontY backY)

	Example usage for setting an explicit scale::

		(gEgo setScale: 50) // 50 percent size.

	:param class class: A scaler class, such as Scaler.
	:param heapPtr obj: An instance of a scaler class.
	:param params: Initialization parameters for the scaler.
	:param heapPtr otherObj: Another object from which to copy scaling information.
	:param number scale: A percentage scale.



.. function:: setCycle(theCycler sendParams)
	:noindex:

	
	Sets a cycler object on the Prop, optionally supplying initialization parameters for the cycler.
	
	Example usage for telling a Prop to cycle forward::
	
		(theFrog setCycle: Forward)
	
	Example usage for telling a Prop to cycle to the end, then cue its caller::
	
		(theFrog setCycle: EndLoop self)
	
	:param class theCycler: A class derived from :class:`Cycle`, or NULL to remove the current cycler.
	
	


.. function:: setScript(theScript sendParams)
	:noindex:

	
	Attaches an arbitrary script to the Prop, optionally providing initialization parameters.
	
	Example usage::
	
		(self setScript: theRoomScript)
	
	


.. function:: cue()
	:noindex:



