.. Beg

.. default - domain::js

.. include:: /includes/standard.rst

====================
Beg (of :class:`CT`)
====================

.. class:: Beg

	Defined in Cycle.sc.

	
	Beg cycles backward to the first cel in a loop and stops.


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



