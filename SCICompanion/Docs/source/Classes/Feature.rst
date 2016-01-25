.. Feature

.. default - domain::js

.. include:: /includes/standard.rst

============================
Feature (of :class:`Object`)
============================

.. class:: Feature

	Defined in Feature.sc.

	
	Feature forms the base class for interactable items in the game. Actor, Prop and View all extend from this class.
	It can also be used on its own, generally for attaching interactions to static portions of the background.
	
	Feature has support for displaying messages when the player performs actions (verbs) by clicking on it. It will
	also optionally direct the ego to approach the feature.
	
	Example usage::
	
		; Define a red plant that displays messages associated with the noun N_PLANT
		; when the use clicks on the white control color on the pic background.
		; The (x, y) coordinates are used to identify the "center of interest" of the feature,
		; which determines the direction in which the ego should face when interacting with it.
		(instance redPlant of Feature
			(properties
				x 217
				y 121
				noun N_PLANT
				onMeCheck ctlWHITE
			)
		)
	
	There are two main ways to identify the bounds of a feature: control colors, and polygons. Even if onMeCheck
	has been set in the Feature declaration, this needs to be done with the setOnMeCheck() method.
	
	You can also indicate for which verbs the ego should approach the feature (as opposed to
	just facing it). This is done with the approachVerbs() method.
	
	Example initialization::
	
		(redPlanet
			approachVerbs: V_DO V_LOOK
			setOnMeCheck: omcCOLORS ctlWHITE
			init:
		)


Subclasses: :class:`View`.

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
		Feature [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Object`:

======== ===========
Property Description
======== ===========
name                
======== ===========

Defined in Feature:

============== =============================================================
Property       Description                                                  
============== =============================================================
x              "center of interest" for the Feature.                        
y                                                                           
z                                                                           
heading                                                                     
noun           Message noun associated with the Feature.                    
case           Message case associated with the Feature.                    
modNum                                                                      
nsTop                                                                       
nsLeft                                                                      
nsBottom                                                                    
nsRight                                                                     
sightAngle                                                                  
actions                                                                     
onMeCheck      omcDISABLE, omcCOLORS or omcPOLYGON. See setOnMeCheck().     
state                                                                       
approachX      The approach spot x.                                         
approachY      The approach spot y.                                         
approachDist   The approach distance.                                       
_approachVerbs Bitmask for verbs that cause the ego to approach the Feature.
============== =============================================================


Methods
==========


.. function:: init([initCode])
	:noindex:

	Initializes the Feature. This must be done prior to it being active in the scene.

	:param heapPtr initCode: Optional initialization :class:`Code` for the feature. Generally, this is left unspecified.



.. function:: dispose()
	:noindex:



.. function:: initialize(param1)
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:



.. function:: doVerb(theVerb)
	:noindex:

	
	This method can be overridden by Feature instances to provide custom logic when the
	player interacts with the feature. By default, it displays any message resource associated
	with the given verb for this Feature's noun.
	


.. function:: notFacing()
	:noindex:




.. function:: facingMe([theObj])
	:noindex:

	:param heapPtr theObj: Optional object that the Feature might be facing. If not specified, the player's ego is used.

	Returns TRUE if this Feature is facing the specified object (or ego).



.. function:: isNotHidden()
	:noindex:




.. function:: onMe(theObj)
	:noindex:

.. function:: onMe(x y)
	:noindex:

	:param heapPtr theObj: The object that might be on the Feature.
	:param number x: The x coordinate.
	:param number y: The y coordinate.
	:returns: TRUE if the (x, y) or object is on the Feature.




.. function:: approachVerbs([verbs ...])
	:noindex:

	Indicates which verbs should cause the ego to approach the feature.

	:param number verbs: An optional series of verbs.

	Example usage::

		(theToy approachVerbs: V_DO V_EAT)



.. function:: setName(theName)
	:noindex:




.. function:: setOnMeCheck(omcDISABLE)
	:noindex:

.. function:: setOnMeCheck(omcPOLYGON polygon)
	:noindex:

.. function:: setOnMeCheck(omcCOLORS [colors ...])
	:noindex:

	Controls how the Feature is identified (colors or polygon), or disables the onMe check.

	:param heapPtr polygon: A polygon defining the boundaries of the Feature
	:param number colors: Control color(s) defining the Feature.

	Example usage::

		(theTable setOnMeCheck: omcCOLORS ctlWHITE ctlGREY)

		(theRock setOnMeCheck: omcPOLYGON (CreateNewPolygon @P_TheRock))



