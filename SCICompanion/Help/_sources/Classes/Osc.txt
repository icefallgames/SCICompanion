.. Osc

.. default - domain::js

.. include:: /includes/standard.rst

=======================
Osc (of :class:`Cycle`)
=======================

.. class:: Osc

	Defined in Osc.sc.

	
	A cycler that oscillates back and forth between cels in a loop.  See the init method for information on parameters.
	
	Sample usage::
	
		// Make the star cycle its animation back and forth.
		(star:setCycle(Osc -1))


Properties
==========

Inherited from :class:`Cycle`:

========= ===========
Property  Description
========= ===========
client               
caller               
cycleDir             
cycleCnt             
completed            
========= ===========

Defined in Osc:

============= ===========
Property      Description
============= ===========
howManyCycles            
============= ===========


Methods
==========

.. function:: init(theClient theHowManyCycles theCaller)
	:noindex:

	
	:param heapPtr theClient: The :class:`Prop` to which this is attached.
	:param number theHowManyCycles: The number of times to cycle, or -1 to cycle indefinitely.
	:param heapPtr theCaller: Optional object to be cue()'d when the cycle is complete.
	


.. function:: doit()
	:noindex:



.. function:: cycleDone()
	:noindex:



