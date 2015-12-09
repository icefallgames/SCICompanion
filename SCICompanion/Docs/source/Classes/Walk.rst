.. Walk

.. default - domain::js

.. include:: /includes/standard.rst

==========================
Walk (of :class:`Forward`)
==========================

.. class:: Walk

	Defined in Cycle.sc.

	
	Walk is a cycler used on Actors. It stops the cel cycling when
	the Actor is not moving.


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
		Walk [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Forward`:

========= ======================================================
Property  Description                                           
========= ======================================================
client    The object to which this is attached.                 
caller    The object that is cue()'d when the cycle is complete.
cycleDir  cdFORWARD or cdBACKWARD.                              
cycleCnt                                                        
completed                                                       
name                                                            
========= ======================================================


Methods
==========

.. function:: doit()
	:noindex:



