.. Cycle

.. default - domain::js

.. include:: /includes/standard.rst

=======================
Cycle (of :class:`Obj`)
=======================

.. class:: Cycle

	Defined in Cycle.sc.

	
	The base class for all cyclers. Cyclers are responsible for changing the cel
	of a :class:`Prop` or :class:`Actor` over time, giving the illusion of animation.
	
	Example usage::
	
		// Cycle the cels of myProp forward.
		(myProp:setCycle(Fwd))


Subclasses: :class:`Smopper`, :class:`MouthSync`, :class:`FlickerCycler`, :class:`Fwd`, :class:`Rev`, :class:`Blink`, :class:`CT`, :class:`ROsc`, :class:`Osc`, :class:`RandCycle`, :class:`MCyc`, :class:`Grycler`.

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
		Cycle [color=greenyellow]
	}

Properties
==========

Defined in Cycle:

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

.. function:: init(theClient)
	:noindex:



.. function:: nextCel()
	:noindex:



.. function:: cycleDone()
	:noindex:



.. function:: motionCue()
	:noindex:



