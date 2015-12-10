.. Parse

.. default - domain::js

.. include:: /includes/standard.rst

==============
Parse (Kernel)
==============

.. function:: Parse(text event)

	
	Parses the specified text and creates a parse event, returning it in the event object specified. It returns TRUE if the text is successfully parsed, otherwise, FALSE.
	
	.. IMPORTANT:: This is SCI0 only.
	
	:param string text: The text to parse (generally input by the player).
	:param heapPtr event: An :class:`Event` object.
	:returns: TRUE if the text is successfully parsed.
	
	Example::
	
		(= hEvent (Event new:))
		(if (Parse "open door" hEvent)
			(if (Said 'open/door')
				(Display "The door is now open")
			)
		)


