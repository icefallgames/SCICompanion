.. CT

.. default - domain::js

.. include:: /includes/standard.rst

======================
CT (of :class:`Cycle`)
======================

.. class:: CT

	Defined in Cycle.sc.

	
	CT (or "cycle to") is a cycler that cycles to a specific cel, then stops.
	
	Example usage::
	
		// Cycle the door forward to cel 4, then stop.
		(door:setCycle(CT 4 cdFORWARD))


Subclasses: :class:`End`, :class:`Beg`.

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

Defined in CT:

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



