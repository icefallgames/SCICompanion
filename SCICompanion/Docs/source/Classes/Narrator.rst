.. Narrator

.. default - domain::js

.. include:: /includes/standard.rst

===========================
Narrator (of :class:`Prop`)
===========================

.. class:: Narrator

	Defined in Talker.sc.

	
	Narrator is responsible for displaying messages and message sequences, and controlling the look
	of the text dialogs. A default Narrator
	(templateNarrator in main.sc) is defined as part of the template game, but you can create your
	own Narrator instances to define different text styles or colors for messages.
	
	If you add a new Narrator instance, you must assign it some talker number, and then add code in the
	findTalker method of testMessage in Main.sc to direct that talker number to the right script and export.
	
	See :doc:`/talkers` for more information Talkers.	


Subclasses: :class:`Talker`, :class:`ChoiceNarrator`.

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
		Narrator [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Prop`:

============== ===============
Property       Description    
============== ===============
x              -1 means center
y              -1 means center
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
name                          
============== ===============

Defined in Narrator:

=============== ===========
Property        Description
=============== ===========
_case                      
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
=============== ===========


Methods
==========

.. function:: init()
	:noindex:



.. function:: doit()
	:noindex:



.. function:: dispose(theDisposeWhenDone)
	:noindex:



.. function:: handleEvent(pEvent)
	:noindex:



.. function:: say(buffer theCaller)
	:noindex:



.. function:: startText(buffer)
	:noindex:



.. function:: display(theText)
	:noindex:



.. function:: startAudio(buffer)
	:noindex:



