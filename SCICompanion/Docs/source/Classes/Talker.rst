.. Talker

.. default - domain::js

.. include:: /includes/standard.rst

=============================
Talker (of :class:`Narrator`)
=============================

.. class:: Talker

	Defined in Talker.sc.

	
	Talker is similar to :class:`Narrator`, but includes support for showing a bust, eyes and mouth
	while the messages are shown or spoken.
	The mouth can also be driven by lip-sync data, if present.
	
	If you add a new Talker instance, you must assign it some talker number, and then add code in the
	findTalker method of testMessage in Main.sc to direct that talker number to the right script and export.
	
	See :doc:`/talkers` for more information Talkers.


Subclasses: :class:`PriorityTalker`, :class:`ChoiceTalker`.

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
		Talker [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Narrator`:

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
name                       
=============== ===========

Defined in Talker:

=========== ===========
Property    Description
=========== ===========
bust                   
eyes                   
mouth                  
viewInPrint            
textX                  
textY                  
useFrame               
blinkSpeed             
=========== ===========


Methods
==========


.. function:: init([theBust theEyes theMouth])
	:noindex:

	Override this method in your Talker instance, and call (super:init(bust eyes mouth)) with
	the appropriate Props for theBust, theEyes and theMouth. You will need to
	position them at the correct spot.

	:param heapPtr theBust: A :class:`Prop` instance for the overall face (bust) of the talker.
	:param heapPtr theEyes: A :class:`Prop` instance for the eyes.
	:param heapPtr theMouth: A :class:`Prop` instance for the talker's mouth.



.. function:: doit()
	:noindex:



.. function:: dispose(param1)
	:noindex:



.. function:: hide()
	:noindex:



.. function:: show()
	:noindex:



.. function:: say(buffer)
	:noindex:



.. function:: startText(buffer)
	:noindex:



.. function:: display(theText)
	:noindex:



.. function:: startAudio(buffer)
	:noindex:



.. function:: cycle(theProp)
	:noindex:



.. function:: setSize()
	:noindex:



