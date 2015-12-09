.. MouthSync

.. default - domain::js

.. include:: /includes/standard.rst

=============================
MouthSync (of :class:`Cycle`)
=============================

.. class:: MouthSync

	Defined in Sync.sc.

	
	This is a cycling class used by :class:`Talker`. It is driven off lip-sync information, and chooses
	the correct mouth cel for the Talker.


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
		MouthSync [color=greenyellow]
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


Methods
==========

.. function:: init(theClient modNum noun verb cond seq)
	:noindex:



.. function:: doit()
	:noindex:



.. function:: dispose()
	:noindex:



.. function:: cue()
	:noindex:



