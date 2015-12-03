.. Synonyms

.. include:: /includes/standard.rst

================
 Synonyms
================

.. IMPORTANT:: SCI0 only.

Synonyms are used to assign word groups to other word groups. They are listed in a **synonyms** block and take the form of::

	(mainword synonym1 synonym2 ...)

An example::

	(synonyms
		(get grab steal)
		; you can add more entries here
	) 
	...
	(if (Said 'get')
	   (Print "Hello")
	) 
	; so now if they type get, grab or steal, it will print "Hello"


The synonyms are activated when the :func:`SetSynonyms` kernel is called. The SCI0 template game automatically calls it, so you don't need to worry about it.

