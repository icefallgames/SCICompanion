.. Ego

.. default - domain::js

.. include:: /includes/standard.rst

=======================
Ego (of :class:`Actor`)
=======================

.. class:: Ego

	Defined in Ego.sc.

	
	The Ego represents the player's main character. It extends :class:`Actor` by handling the following:
	
	- Sends the player to a new room when they walk off screen.
	- Moves the player to a new destination when the user clicks the walk icon somewhere.
	- Handles getting and losing inventory items.


Subclasses: :class:`GameEgo`.

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
		Ego [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Actor`:

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
illegalBits               
xLast                     
yLast                     
xStep                     
origStep                  
moveSpeed                 
blocks                    
baseSetter                
mover                     
looper                    
viewer                    
avoider                   
code                      
name                      
============== ===========

Defined in Ego:

======== ===========
Property Description
======== ===========
edgeHit             
======== ===========


Methods
==========

.. function:: init()
	:noindex:



.. function:: doit()
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:

	Handles moving the ego around.


.. function:: facingMe()
	:noindex:

	Always returns TRUE. You **are** me!



.. function:: get(invNumber [...])
	:noindex:

	Gives the ego an inventory item (or items).

	:param number invNumber: The numerical identifier of the inventory item.

	Example usage::

		// Give the ego the crystal and the lizard tail.
		( gEgo get: INV_CRYSTAL INV_LIZARDTAIL)




.. function:: put(invNumber [newOwner])
	:noindex:

	Removes an inventory item from the ego, and optionally gives it to a new owner.

	:param number invNumber: The numerical identifier of the inventory item.
	:param number newOwner: The optional new owner (typically a room number).

	Example usage::

		// The ego used lizard tail, so remove it from the inventory:
		(gEgo put: INV_LIZARDTAIL)



.. function:: has(invNumber)
	:noindex:

	
	Determines if the ego has an inventory item.
	
	:param number invNumber: The numerical identifier of the inventory item.
	
	Example usage::
	
		(if (gEgo has: INV_LIZARDTAIL)
			// Do something...
		)
	


