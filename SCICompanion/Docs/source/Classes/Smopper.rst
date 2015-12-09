.. Smopper

.. default - domain::js

.. include:: /includes/standard.rst

===========================
Smopper (of :class:`Cycle`)
===========================

.. class:: Smopper

	Defined in Smopper.sc.

	
	Smopper is a complex cycler similar to :class:`StopWalk`, but allowing the use
	of separate starting and stopping views. This can be used to allow for more realistic
	movement when starting and stopping.
	
	Example usage::
	
		// Cycle the boy with view 805 for stopped, 814 for slowing and 815 for starting. Cycle speed is 12.
		(boy:setCycle(Smopper 805 814 815 12))


Subclasses: :class:`FiddleStopWalk`.

.. blockdiag::
	:alt: class diagram
	:width: 600

	diagram {
		default_fontsize = 16
		Cycle -> MouthSync
		Cycle -> Smopper
		Cycle -> MoveCycle
		Cycle -> FlickerCycler
		Cycle -> Forward
		Cycle -> Blink
		Cycle -> RangeOscillate
		Cycle -> Oscillate
		Cycle -> RandCycle
		Cycle -> Reverse
		Cycle -> GradualCycler
		Cycle -> CycleTo
		CycleTo -> EndLoop
		CycleTo -> BegLoop
		Forward -> ForwardCounter
		Forward -> StopWalk
		Forward -> Walk
		Smopper -> FiddleStopWalk
		Smopper [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Cycle`:

========= ===========
Property  Description
========= ===========
client               
caller               
cycleDir             
cycleCnt             
completed            
name                 
========= ===========

Defined in Smopper:

========= ===========
Property  Description
========= ===========
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


Methods
==========


.. function:: init(theClient [theVStopped theVSlow theVStart theCSpeed theCaller])
	:noindex:

	Initializes the Smopper.

	:param heapPtr theClient: The :class:`Actor` to which this applies.
	:param number theVStopped: The view number for the stopped state.
	:param number theVSlow: The view number for the slowing down state.
	:param number theVStart: The view number for the starting state.
	:param number theCSpeed: The cycle speed used for starting, stopped and stopped.
	:param heapPtr theCaller: Optional object that gets cue(0)'d when stopped, and cue(1)'d when starting motion again.



.. function:: doit()
	:noindex:



.. function:: dispose(param1)
	:noindex:



