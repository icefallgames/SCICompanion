.. CycleTo

.. default - domain::js

.. include:: /includes/standard.rst

===========================
CycleTo (of :class:`Cycle`)
===========================

.. class:: CycleTo

	Defined in Cycle.sc.

	
	CycleTo is a cycler that cycles to a specific cel, then stops.
	
	Example usage::
	
		// Cycle the door forward to cel 4, then stop.
		(door setCycle: CycleTo 4 cdFORWARD)


Subclasses: :class:`EndLoop`, :class:`BegLoop`.

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
		CycleTo [color=greenyellow]
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

Defined in CycleTo:

======== ===========
Property Description
======== ===========
endCel              
======== ===========


Methods
==========


.. function:: init(theClient theEndCel theCycleDir [theCaller])
	:noindex:

	:param heapPtr theClient: The :class:`Prop` to which this is attached.
	:param number theEndCel: The cel number that we are cycling to.
	:param number theCycleDir: cdFORWARD or cdBACKWARD.
	:param heapPtr theCaller: Optional object on which we call cue() when the cycle is finished.



.. function:: doit()
	:noindex:



.. function:: cycleDone()
	:noindex:



