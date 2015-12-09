.. Cycle

.. default - domain::js

.. include:: /includes/standard.rst

==========================
Cycle (of :class:`Object`)
==========================

.. class:: Cycle

	Defined in Cycle.sc.

	
	The base class for all cyclers. Cyclers are responsible for changing the cel
	of a :class:`Prop` or :class:`Actor` over time, giving the illusion of animation.
	
	Example usage::
	
		// Cycle the cels of myProp forward.
		(myProp setCycle: Forward)


Subclasses: :class:`MouthSync`, :class:`Smopper`, :class:`MoveCycle`, :class:`FlickerCycler`, :class:`Forward`, :class:`Blink`, :class:`RangeOscillate`, :class:`Oscillate`, :class:`RandCycle`, :class:`Reverse`, :class:`GradualCycler`, :class:`CycleTo`.

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
		Cycle [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Object`:

======== ===========
Property Description
======== ===========
name                
======== ===========

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



