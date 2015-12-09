.. Forward

.. default - domain::js

.. include:: /includes/standard.rst

===========================
Forward (of :class:`Cycle`)
===========================

.. class:: Forward

	Defined in Cycle.sc.

	
	Forward is a cycler that cycles through cels forward.
	
	Example usage::
	
		(bird setCycle: Forward)
	
	See also: :class:`Reverse`.


Subclasses: :class:`ForwardCounter`, :class:`StopWalk`, :class:`Walk`.

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
		Forward [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Cycle`:

========= ======================================================
Property  Description                                           
========= ======================================================
name                                                            
client    The object to which this is attached.                 
caller    The object that is cue()'d when the cycle is complete.
cycleDir  cdFORWARD or cdBACKWARD.                              
cycleCnt                                                        
completed                                                       
========= ======================================================


Methods
==========

.. function:: doit()
	:noindex:



.. function:: cycleDone()
	:noindex:



