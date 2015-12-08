.. public exports

.. include:: /includes/standard.rst

================
 Public exports
================

In order to use an instance or procedure from another script file, you can declare it as public by listing
it in the public section of the script:

.. code-block:: python
    :emphasize-lines: 3-6

	(script# MYSPECIALSCRIPT)	; MYSPECIALSCRIPT is a number defined in game.sh, say.

	(public
		SomeProcedure 0
		AngryTalker 1
	)

	(procedure (SomeProcedure param1)
	)

	(instance AngryTalker of Talker
		(properties)
		...
	)


If the above script was called FooBar.sc, then you could "use" FooBar in another script in order to call the public procedures in FooBar.sc:

.. code-block:: python
    :emphasize-lines: 1

	(use FooBar)

	(procedure (SomeProc)
		(SomeProcedure 6)	; Because we declared we were using FooBar, we can call public procedures in FooBar.sc
	)

In the rare cases where you need to use a public instances, it is a little more complicated. For this, we need to use the ScriptID kernel. It will return the object ID for the object
located at a particular index in a particular script number::

	(procedure (SomeProc &tmp theTalker)
		(= theTalker (ScriptID MYSPECIALSCRIPT 1))	; export 1 of script# MYSPECIALSCRIPT
		(Printf {The talker's name is %s} (theTalker name:))
	)