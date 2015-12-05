.. Synonyms

.. include:: /includes/standard.rst

================
 Synonyms
================

.. IMPORTANT:: SCI0 only.

Synonyms are used to assign word groups to other word groups::

	(synonyms
		'grab' = 'get'
	) 
	...
	(if(Said('get')
	   Print("Hello")
	) 
	// so now if they type get or grab it will print "Hello"


The synonyms are activated when the :func:`SetSynonyms` kernel is called. The template game automatically calls it, so you don't need to worry about it.

