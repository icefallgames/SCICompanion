.. Rev

.. default - domain::js

.. include:: /includes/standard.rst

=======================
Rev (of :class:`Cycle`)
=======================

.. class:: Rev

	Defined in Rev.sc.

	
	Rev is a cycler that cycles through cels backward.
	
	Example usage::
	
		(bird:setCycle(Rev))
	
	See also: :class:`Fwd`.


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
		Rev [color=greenyellow]
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


Methods
==========

.. function:: doit()
	:noindex:



.. function:: cycleDone()
	:noindex:



