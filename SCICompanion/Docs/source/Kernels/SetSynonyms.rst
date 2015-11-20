.. SetSynonyms

.. default - domain::js

.. include:: /includes/standard.rst

====================
SetSynonyms (Kernel)
====================

.. function:: SetSynonyms(scripts)

	
	Examines the scripts pointed to by the objects in the specified set (by their script property).
	If they have any synonyms, it enables them.
	
	.. IMPORTANT::
		SCI0 only.
	
	This is handled automatically by the :func:`Room` and :func:`Region` classes.
	
	Example::
	
		(synonyms
		  'grab' = 'open'
		)
		...
	
		SetSynonyms(gRegions) // the global variable pointing to the list of rooms


