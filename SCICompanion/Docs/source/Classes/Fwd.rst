.. Fwd

.. default - domain::js

.. include:: /includes/standard.rst

=======================
Fwd (of :class:`Cycle`)
=======================

.. class:: Fwd

	Defined in Cycle.sc.

	
	Fwd is a cycler that cycles through cels forward.
	
	Example usage::
	
		(bird:setCycle(Fwd))
	
	See also: :class:`Rev`.


Subclasses: :class:`ForwardCounter`, :class:`StopWalk`, :class:`Walk`.

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
		Fwd [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Cycle`:

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



.. function:: cycleDone()
	:noindex:



