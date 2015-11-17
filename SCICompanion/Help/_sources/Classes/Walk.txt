.. Walk

.. default - domain::js

.. include:: /includes/standard.rst

======================
Walk (of :class:`Fwd`)
======================

.. class:: Walk

	Defined in Cycle.sc.

	
	Walk is a cycler used on Actors. It stops the cel cycling when
	the Actor is not moving.


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
		Walk [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Fwd`:

========= ======================================================
Property  Description                                           
========= ======================================================
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



