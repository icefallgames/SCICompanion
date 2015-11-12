.. Said

.. default - domain::js

.. include:: /includes/standard.rst

=============
Said (Kernel)
=============

.. function:: Said(saidString)

	
	Checks to see if the specified string matches the last line parsed.
	
	
	:param saidString saidString: The string to match.
	:returns: TRUE if there was a match, otherwise FALSE.
	
	The following operators can be used in a said string:
	
	,
		"OR", Used to specify alternatives to words, such as "take,get".
	
	/
		Sentence part separator.  Only two of these tokens may be used, since sentences are split into a maximum of three parts.
	
	(
		Used together with ')' for grouping.
	
	)
		See '('.
	
	[
		Used together with ']' for optional grouping. "[a]" means "either a or nothing".
	
	]
		See '['.
	
	<
		Semantic reference operator (as in 'get<up')
	
	>
		Instructs Said() not to claim the event passed to the previous Parse() call on a match.  Used for successive matching.
	
	If a match is made, the words are "used up" until a fresh parse event happens. For example::
	
		if (Said('look'))
		{
		}
		if (Said('look')) // This will return FALSE, even if the previous line returned TRUE.
		{
			// We can never get here.
		}
	
	Other examples::
	
		Said('look/car')         // Matches "look car", "look in car"
		Said('look<in/car')      // Matches "look in car" only
		Said('give/food/dog')    // Matches "give food to dog" and "give dog food"


