.. Reverse

.. default - domain::js

.. include:: /includes/standard.rst

===========================
Reverse (of :class:`Cycle`)
===========================

.. class:: Reverse

	Defined in Reverse.sc.

	
	Reverse is a cycler that cycles through cels backward.
	
	Example usage::
	
		(bird setCycle: Reverse)
	
	See also: :class:`Forward`.


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
		Reverse [color=greenyellow]
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


Methods
==========

.. function:: doit()
	:noindex:



.. function:: cycleDone()
	:noindex:



