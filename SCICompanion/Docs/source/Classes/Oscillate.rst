.. Oscillate

.. default - domain::js

.. include:: /includes/standard.rst

=============================
Oscillate (of :class:`Cycle`)
=============================

.. class:: Oscillate

	Defined in Oscillate.sc.

	
	A cycler that oscillates back and forth between cels in a loop.  See the init method for information on parameters.
	
	Sample usage::
	
		// Make the star cycle its animation back and forth.
		(star setCycle: Oscillate -1)


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
		Oscillate [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Cycle`:

========= ===========
Property  Description
========= ===========
name                 
client               
caller               
cycleDir             
cycleCnt             
completed            
========= ===========

Defined in Oscillate:

============= ===========
Property      Description
============= ===========
howManyCycles            
============= ===========


Methods
==========

.. function:: init(theClient theHowManyCycles theCaller)
	:noindex:

	
	:param heapPtr theClient: The :class:`Prop` to which this is attached.
	:param number theHowManyCycles: The number of times to cycle, or -1 to cycle indefinitely.
	:param heapPtr theCaller: Optional object to be cue()'d when the cycle is complete.
	


.. function:: doit()
	:noindex:



.. function:: cycleDone()
	:noindex:



