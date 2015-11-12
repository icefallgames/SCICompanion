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



