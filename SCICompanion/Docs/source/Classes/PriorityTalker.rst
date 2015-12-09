.. PriorityTalker

.. default - domain::js

.. include:: /includes/standard.rst

===================================
PriorityTalker (of :class:`Talker`)
===================================

.. class:: PriorityTalker

	Defined in PriorityTalker.sc.

	
	A PriorityTalker is similar to a :class:`Talker`, except that when the PriorityTalker is drawn, it obeys
	the priority value it is given, allowing it to appear partially behind other objects in the scene.
	This could be useful, for instance, if the talker was an image on a tv screen in a game.


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
		PriorityTalker [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Talker`:

=============== ===========
Property        Description
=============== ===========
x                          
y                          
z                          
heading                    
noun                       
_case                      
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
caller                     
disposeWhenDone            
ticks                      
talkWidth                  
keepWindow                 
modeless                   
font                       
cueVal                     
initialized                
showTitle                  
color                      
back                       
curVolume                  
saveCursor                 
bust                       
eyes                       
mouth                      
viewInPrint                
textX                      
textY                      
useFrame                   
blinkSpeed                 
name                       
=============== ===========

Defined in PriorityTalker:

======== ===========
Property Description
======== ===========
priBits             
======== ===========


Methods
==========

.. function:: dispose(param1)
	:noindex:



.. function:: hide()
	:noindex:



.. function:: show()
	:noindex:



.. function:: cycle(param1)
	:noindex:



