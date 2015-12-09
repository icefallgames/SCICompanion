.. RangeOscillate

.. default - domain::js

.. include:: /includes/standard.rst

==================================
RangeOscillate (of :class:`Cycle`)
==================================

.. class:: RangeOscillate

	Defined in RangeOscillate.sc.

	
	This cycler is similar to :class:`Oscillate`, but lets you specify specific first and last cels between which to oscillate (range oscillate).
	
	Sample usage::
	
		// Make the star cycle 5 times between cel 2 and 7.
		(star setCycle: RangeOscillate 5 2 7)


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
		RangeOscillate [color=greenyellow]
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

Defined in RangeOscillate:

======== ===========
Property Description
======== ===========
cycles              
firstC              
lastC               
======== ===========


Methods
==========

.. function:: init(theClient theCycles theFirstC theLastC theCaller)
	:noindex:

	
	:param heapPtr theClient: The :class:`Prop` to which this is attached.
	:param number theCycles: The number of times to cycle, or -1 to cycle indefinitely.
	:param number theFirstC: The cel at which to start.
	:param number theLastC: The cel at which to end.
	:param heapPtr theCaller: Optional object to be cue()'d when the cycle is complete.
	


.. function:: doit()
	:noindex:



.. function:: cycleDone()
	:noindex:



