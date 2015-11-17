.. ROsc

.. default - domain::js

.. include:: /includes/standard.rst

========================
ROsc (of :class:`Cycle`)
========================

.. class:: ROsc

	Defined in ROsc.sc.

	
	This cycler is similar to :class:`Osc`, but lets you specify specific first and last cels between which to oscillate.
	
	Sample usage::
	
		// Make the star cycle 5 times between cel 2 and 7.
		(star:setCycle(Osc 5 2 7))


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
		ROsc [color=greenyellow]
	}

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

Defined in ROsc:

======== ===========
Property Description
======== ===========
cycles              
firstC              
lastC               
======== ===========


Methods
==========

.. function:: init(theClient theCycles theFirstC theLastC theCaller)
	:noindex:

	
	:param heapPtr theClient: The :class:`Prop` to which this is attached.
	:param number theCycles: The number of times to cycle, or -1 to cycle indefinitely.
	:param number theFirstC: The cel at which to start.
	:param number theLastC: The cel at which to end.
	:param heapPtr theCaller: Optional object to be cue()'d when the cycle is complete.
	


.. function:: doit()
	:noindex:



.. function:: cycleDone()
	:noindex:



