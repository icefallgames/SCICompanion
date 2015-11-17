.. Grycler

.. default - domain::js

.. include:: /includes/standard.rst

===========================
Grycler (of :class:`Cycle`)
===========================

.. class:: Grycler

	Defined in Grooper.sc.

	
	This class is used internally by :class:`Grooper`.


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
		Grycler [color=greenyellow]
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

Defined in Grycler:

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



