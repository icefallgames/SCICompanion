.. Said

.. include:: /includes/standard.rst

================
 Said
================

.. IMPORTANT:: SCI0 only.

Said strings are used to parse the player's input in the game. They begin and end with single quote characters ('). They consist of words, and a few special characters.

=========== =======================================================================================================================
Operator    Meaning
=========== =======================================================================================================================
,           "OR". Used to specify alternatives to words, such as "take , get".
&           Unknown. Probably used for debugging.
/           Sentence part separator. Only two of these tokens may be used, since sentences are split into a maximum of three parts.
(           Used together with ')' for grouping
)           See '('
[           Used together with '[' for optional grouping. "[ a ]" means "either a or nothing"
]           See '['.
#           Unknown. Assumed to have been used exclusively for debugging, if at all.
<           Semantic reference operator (as in 'get < up').
>           Instructs Said() not to claim the event passed to the previous Parse() call on a match. Used for successive matching.
=========== =======================================================================================================================

The said words must be defined in vocab.000. Use the :doc:`vocabulary editor </vocabs>` in |scicomp| to add and modify words.

An example::

	(method (handleEvent pEvent) 
		(if (Said('open<door'))
			// you open the door
		)
	)


Some more examples
====================

Often, you might want to enclose all your "look" handlers in one if statement, but still support just "look" all by itself::

	(if (Said('look>'))
		(if (Said('/wall'))
			Print("The wall is covered in paint.")
		)
		(if (Said('/floor'))
			Print("The floor is covered in dust.")
		)
		(if (Said('[/!*]'))
			// this will handle just "look" by itself
			Print("You are in a room with a floor and a wall.")
		)
		(if (Said('/*'))
			// this will handle "look anyword"
			Print("You don't see anything interesting about it.")
		)
	)

If you wanted to handle "look around" just like look, you could do::

	(if (Said('look>'))
		(if (Said('/wall'))
			Print("The wall is covered in paint.")
		)
		(if (Said('/floor'))
			Print("The floor is covered in dust.")
		)
		(if (Said('[/around]'))
			// this will handle just "look" by itself, and also "look around"
			Print("You are in a room with a floor and a wall.")
		)
		(if (Said('/*'))
			Print("You don't see anything interesting about it.")
		)
	)

One thing to note is that the order of words in the Said string doesn't necessarily match the order that the words are typed in by the user. 
There is more meaning to the Said string than that.  The below clause will respond positively to the user typing "Give food to the dog", or even "Give dog the food".
However, surprisingly (and fortunately), it will not match "Give food the dog".  Rather than the particular word order in a sentence, the three parts of the follwing clause correspond to sentence parts::

	(if (Said('give/food/dog'))
		Print("You give the food to the dog.")
	)

Similarly::

	(if (Said('point<up/flashlight'))
		Print("You point the flashlight up, and see something curious.")
	)

The above will respond to "point flashlight up", but not "point up flashlight".