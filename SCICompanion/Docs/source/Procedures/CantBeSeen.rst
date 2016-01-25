.. CantBeSeen

.. default - domain::js

.. include:: /includes/standard.rst

=====================
CantBeSeen (Sight.sc)
=====================


.. function:: CantBeSeen(theSight [optSeer fieldAngle fieldDistance])

	Determines if an object can't be seen by another.
	
	:param heapPtr theSight: The object in question.
	:param heapPtr optSeer: The object we are asking, or the ego if not specified.
	:param number fieldAngle: The angle the looker object is looking, or lookerObj:sightAngle if not supplied.
	:param number fieldDistance: An optional distance limit.
	:returns: TRUE if the seer cannot see the sight, FALSE otherwise. FALSE if the sight is the seer.


