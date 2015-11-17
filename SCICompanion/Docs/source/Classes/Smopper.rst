.. Smopper

.. default - domain::js

.. include:: /includes/standard.rst

===========================
Smopper (of :class:`Cycle`)
===========================

.. class:: Smopper

	Defined in Smopper.sc.

	
	Smopper (or "smooth stopper") is a complex cycler similar to :class:`StopWalk`, but allowing the use
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
		Cycle -> Smopper
		Cycle -> MouthSync
		Cycle -> FlickerCycler
		Cycle -> Fwd
		Cycle -> Rev
		Cycle -> Blink
		Cycle -> CT
		Cycle -> ROsc
		Cycle -> Osc
		Cycle -> RandCycle
		Cycle -> MCyc
		Cycle -> Grycler
		CT -> End
		CT -> Beg
		Fwd -> ForwardCounter
		Fwd -> StopWalk
		Fwd -> Walk
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



