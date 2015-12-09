.. ChoiceNarrator

.. default - domain::js

.. include:: /includes/standard.rst

=====================================
ChoiceNarrator (of :class:`Narrator`)
=====================================

.. class:: ChoiceNarrator

	Defined in ChoiceTalker.sc.

	
	ChoiceNarrator is a subclass of :class:`Narrator` that display a series of options
	for the player to choose. Given a noun/verb/condition tuple, sequence 1 will be
	the title, and the following sequences will be the options. The number of the option
	chosen will be the sequence number of that choice minus one.
	
	To put a ChoiceNarrator in *choice mode* do and specify the noun/verb/condition, do::
	
		(myChoiceNarrator
			normal: FALSE
			curNoun: noun
			curVerb: verb
			curCase: condition
		)
	
	Then have it say something, going through the gMessager like usual, and passing the
	same noun/verb/condition, and 0 for the sequence::
	
		(gMessager say: noun verb condition 0)
	
	Then you can check the whichSelect property::
	
		(switch (myChoiceNarrator whichSelect?)
			(1
				// Do something
			)
			(2
				// Do something else
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
		ChoiceNarrator [color=greenyellow]
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

Defined in ChoiceNarrator:

=========== ===========
Property    Description
=========== ===========
whichSelect            
normal                 
curNoun                
curVerb                
curCase                
=========== ===========


Methods
==========

.. function:: display(theText param2)
	:noindex:



