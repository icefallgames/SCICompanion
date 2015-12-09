.. ForwardCounter

.. default - domain::js

.. include:: /includes/standard.rst

====================================
ForwardCounter (of :class:`Forward`)
====================================

.. class:: ForwardCounter

	Defined in ForwardCounter.sc.

	
	This extends the functionality of the :class:`Forward` cycler by allowing for cycling forward through the cels in a loop
	a specified number of times and then stopping.
	
	Example usage::
	
		; Cycle through the diamond's cels three times
		(diamond setCycle: ForwardCounter 3)


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
		ForwardCounter [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Forward`:

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

Defined in ForwardCounter:

======== ===========
Property Description
======== ===========
count               
======== ===========


Methods
==========


.. function:: init(theClient theCount [theCaller])
	:noindex:

	:param heapPtr theClient: The :class:`Prop` to which this cycler is attached.
	:param number theCount: The number of times to cycle through the prop's cels.
	:param heapPtr theCaller: Optional caller that will have its cue() method called when the cycling is done.



.. function:: cycleDone()
	:noindex:



