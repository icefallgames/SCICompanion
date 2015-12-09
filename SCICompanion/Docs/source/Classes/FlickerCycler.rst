.. FlickerCycler

.. default - domain::js

.. include:: /includes/standard.rst

=================================
FlickerCycler (of :class:`Cycle`)
=================================

.. class:: FlickerCycler

	Defined in FlickerCycler.sc.

	A cycler that causes a :class:`Prop` to flicker on and off randomly.


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
		FlickerCycler [color=greenyellow]
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

Defined in FlickerCycler:

========== ===========
Property   Description
========== ===========
cycleSpeed            
========== ===========


Methods
==========

.. function:: doit()
	:noindex:



.. function:: nextCel()
	:noindex:



