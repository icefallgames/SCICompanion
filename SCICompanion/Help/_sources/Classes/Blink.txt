.. Blink

.. default - domain::js

.. include:: /includes/standard.rst

=========================
Blink (of :class:`Cycle`)
=========================

.. class:: Blink

	Defined in Talker.sc.

	
	Blink is a cycler used with :class:`Talker` to make eyes blink randomly.


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
		Blink [color=greenyellow]
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

Defined in Blink:

========= ===========
Property  Description
========= ===========
waitCount            
lastCount            
waitMin              
waitMax              
========= ===========


Methods
==========

.. function:: init(theClient averageWaitTicks)
	:noindex:



.. function:: doit()
	:noindex:



.. function:: cycleDone()
	:noindex:



