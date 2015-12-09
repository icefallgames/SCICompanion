.. MoveCycle

.. default - domain::js

.. include:: /includes/standard.rst

=============================
MoveCycle (of :class:`Cycle`)
=============================

.. class:: MoveCycle

	Defined in MoveCycle.sc.

	
	A cycler that progresses through an explicit set of cels. See the init() method for more information about parameters.
	
	Example usage::
	
		// Defined at the top of the script:
		(local [celList 5] = [3 6 2 1 $8000]) // $8000 indicates the end of the cel list.
	
		// Then in code:
		(aStar setCycle: MoveCycle celList) // cycle through 3, 6, 2, 1 then stop.
	


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
		MoveCycle [color=greenyellow]
	}

Properties
==========

Inherited from :class:`Cycle`:

========= ===========
Property  Description
========= ===========
name                 
client               
caller               
cycleDir             
cycleCnt             
completed            
========= ===========

Defined in MoveCycle:

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



