.. SmoothLooper

.. default - domain::js

.. include:: /includes/standard.rst

===============================
SmoothLooper (of :class:`Code`)
===============================

.. class:: SmoothLooper

	Defined in SmoothLooper.sc.

	
	SmoothLooper is a looper (a class that determines the view loop based on Actor direction)
	that lets you specify a different view that is used when changing directions.
	This allows for smooth turns when an :class:`Actor` heads in a new direction.
	
	Example usage::
	
		(instance mySmooper of SmoothLooper
			(properties
				cycleSpeed 6
				vChangeDir 3931
			)
		)
	
		// Then in code:
		(theGuy looper: mySmooper)
	


Properties
==========

Inherited from :class:`Code`:

======== ===========
Property Description
======== ===========
name                
======== ===========

Defined in SmoothLooper:

============= ===========
Property      Description
============= ===========
nextLoop                 
client                   
oldCycler                
oldMover                 
newMover                 
oldCycleSpeed            
cycleSpeed               
inProgress               
vNormal                  
vChangeDir               
============= ===========


Methods
==========

.. function:: doit(theClient theAngle)
	:noindex:



.. function:: dispose()
	:noindex:



.. function:: cue()
	:noindex:



