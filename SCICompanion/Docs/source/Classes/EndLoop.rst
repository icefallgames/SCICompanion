.. EndLoop

.. default - domain::js

.. include:: /includes/standard.rst

=============================
EndLoop (of :class:`CycleTo`)
=============================

.. class:: EndLoop

	Defined in Cycle.sc.

	
	End cycles from the first cel to the last cel in a loop and stops.
	
	Example usage::
	
		// Tell the door to cycle to its last cel and notify us when done.
		(aDoor setCycle: EndLoop self)


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
		EndLoop [color=greenyellow]
	}

Properties
==========

Inherited from :class:`CycleTo`:

========= ======================================================
Property  Description                                           
========= ======================================================
name                                                            
client    The object to which this is attached.                 
caller    The object that is cue()'d when the cycle is complete.
cycleDir  cdFORWARD or cdBACKWARD.                              
cycleCnt                                                        
completed                                                       
endCel                                                          
========= ======================================================


Methods
==========


.. function:: init(theClient [theCaller])
	:noindex:

	:param heapPtr theClient: The :class:`Prop` to which this is attached.
	:param heapPtr theCaller: Optional object on which we call cue() when the cycle is finished.



