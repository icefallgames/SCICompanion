.. MCyc

.. default - domain::js

.. include:: /includes/standard.rst

========================
MCyc (of :class:`Cycle`)
========================

.. class:: MCyc

	Defined in MCyc.sc.

	
	A cycler that progresses through an explicit set of cels. See the init() method for more information about parameters.
	
	Example usage::
	
		// Defined at the top of the script:
		(local celList[5] = (3 6 2 1 $8000)) // $8000 indicates the end of the cel list.
	
		// Then in code:
		(aStar:setCycle(celList)) // cycle through 3, 6, 2, 1 then stop.
	


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

Defined in MCyc:

======== ===========
Property Description
======== ===========
value               
points              
size                
======== ===========


Methods
==========


.. function:: init(theClient thePoints [theCaller cycleDir])
	:noindex:

	:param heapPtr theClient: The object to which the cycler is attached.
	:param heapPtr thePoints: An array of cels.
	:param heapPtr theCaller: An optional object that will be cued when we're done.
	:param number cycleDir: 1 to go forward, or -1 to go backward.



.. function:: doit()
	:noindex:



.. function:: nextCel()
	:noindex:



.. function:: cycleDone()
	:noindex:



