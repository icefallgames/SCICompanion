.. FiddleStopWalk

.. default - domain::js

.. include:: /includes/standard.rst

====================================
FiddleStopWalk (of :class:`Smopper`)
====================================

.. class:: FiddleStopWalk

	Defined in SQEgo.sc.

	
	FiddleStopWalk is a game-specific subclass of :class:`Smopper` (which allows separate views
	for starting, stopping and stopped animations). It extends Smopper by providing some random
	idle animations. The version of FiddleStopWalk in the SCI1.1 template
	game uses SQ5-specific views (so it won't work with the template game), but is included as an example of what you
	might want to implement in your game.


Properties
==========

Inherited from :class:`Smopper`:

========= ===========
Property  Description
========= ===========
client               
caller               
cycleDir             
cycleCnt             
completed            
vInMotion            
vStopped             
vSlow                
vStart               
stopState            
useSlow              
cSpeed               
oldSpeed             
newCel               
tempMover            
========= ===========

Defined in FiddleStopWalk:

=========== ===========
Property    Description
=========== ===========
lCel                   
ticks                  
curTicks               
lastTicks              
oldControl             
oldCycSpeed            
=========== ===========


Methods
==========

.. function:: doit(sendParams)
	:noindex:



.. function:: cue()
	:noindex:



