.. End

.. default - domain::js

.. include:: /includes/standard.rst

====================
End (of :class:`CT`)
====================

.. class:: End

	Defined in Cycle.sc.

	
	End cycles to the last cel in a loop and stops.
	
	Example usage::
	
		// Tell the door to cycle to its last cel and notify us when done.
		(aDoor:setCycle(End self))


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
		End [color=greenyellow]
	}

Properties
==========

Inherited from :class:`CT`:

========= ======================================================
Property  Description                                           
========= ======================================================
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



