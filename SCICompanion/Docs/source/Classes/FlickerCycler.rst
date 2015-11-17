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



