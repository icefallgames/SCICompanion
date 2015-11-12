.. IsFacing

.. default - domain::js

.. include:: /includes/standard.rst

=====================
IsFacing (Helpers.sc)
=====================


.. function:: IsFacing(targetObj [lookerObj theAngle theDistance])

	Determines if an object is facing another.
	
	:param heapPtr targetObj: The object in question.
	:param heapPtr lookerObj: The object we are asking, or the ego if not specified.
	:param number theAngle: The angle the looker object is looking, or lookerObj:sightAngle if not supplied.
	:param number theDistance: An optional distance limit.


