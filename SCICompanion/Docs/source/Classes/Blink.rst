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
name                 
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



