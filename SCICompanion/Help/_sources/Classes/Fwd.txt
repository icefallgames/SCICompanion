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



