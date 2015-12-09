.. GradualCycler

.. default - domain::js

.. include:: /includes/standard.rst

=================================
GradualCycler (of :class:`Cycle`)
=================================

.. class:: GradualCycler

	Defined in Grooper.sc.

	
	This class is used internally by :class:`GradualLooper`.


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
		GradualCycler [color=greenyellow]
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

Defined in GradualCycler:

========== ===========
Property   Description
========== ===========
loopIndex             
numOfLoops            
========== ===========


Methods
==========

.. function:: init(param1 theCaller theLoopIndex)
	:noindex:



.. function:: doit()
	:noindex:



.. function:: nextCel()
	:noindex:



.. function:: cycleDone()
	:noindex:



.. function:: loopIsCorrect()
	:noindex:



