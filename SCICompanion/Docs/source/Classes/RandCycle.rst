.. RandCycle

.. default - domain::js

.. include:: /includes/standard.rst

=============================
RandCycle (of :class:`Cycle`)
=============================

.. class:: RandCycle

	Defined in RandCycle.sc.

	
	A cycler that oscillates back and forth between cels in a loop. See the init method for information on parameters.
	
	Sample usage::
	
		// Make the star cycle between random cels 5 times.
		(star:setCycle(RandCycle 5))


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
		RandCycle [color=greenyellow]
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

Defined in RandCycle:

======== ===========
Property Description
======== ===========
count               
reset               
======== ===========


Methods
==========


.. function:: init(theClient [theCount theCaller shouldReset])
	:noindex:

	:param heapPtr theClient: The object to which the cycler is attached.
	:param number theCount: Optional number of times to cycle. If not specified, or -1, cycling continues indefinitely.
	:param heapPtr theCaller: Optional caller object that will be cue'd when the cycling is complete.
	:param boolean shouldReset: Should the object go back to cel 0 after cycling is complete.



.. function:: doit()
	:noindex:



.. function:: nextCel()
	:noindex:



.. function:: cycleDone()
	:noindex:



