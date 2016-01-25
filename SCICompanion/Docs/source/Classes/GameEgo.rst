.. GameEgo

.. default - domain::js

.. include:: /includes/standard.rst

=========================
GameEgo (of :class:`Ego`)
=========================

.. class:: GameEgo

	Defined in GameEgo.sc.

	
	GameEgo is a game-specific subclass of :class:`Ego`. Here, you can put default answers for
	looking, talking and performing actions on yourself.


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
		GameEgo [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Ego`:

============== ===========
Property       Description
============== ===========
x                         
y                         
z                         
heading                   
noun                      
case                     
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
edgeHit                   
name                      
============== ===========


Methods
==========

.. function:: handleEvent(pEvent param2)
	:noindex:



.. function:: doVerb(theVerb params)
	:noindex:



.. function:: setSpeed(newSpeed)
	:noindex:



