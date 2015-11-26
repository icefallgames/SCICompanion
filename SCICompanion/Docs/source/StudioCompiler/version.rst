.. Script version

.. include:: /includes/standard.rst

================
 Script version
================

A number of small language enhancements have been made to |scicomp|'s script compiler compared to the script compiler with SCI Studio.
To ensure backwards compatibility, the script syntax version can be specified at the top of the file. Use version 1 (or don't specify anything
at all) to ensure backwards compatibility with SCI Studio's compiler. Otherwise, use version 2::

	(version 2)

Conditional expressions
========================

SCI Studio did not properly evaluate conditional expressions with parentheses (which are typically used to enforce evaluation order). This is described
in more detail in :doc:`conditionalexpressions`.

Constant expressions
=====================

In |scicomp| syntax version 2, a small subset of constant expressions are allowed to be used in class property declarations. This is useful for
OR'ing together flags for property selectors that are bitmasks::

	(instance public rm110 of Room
		(properties
			picture 110
			// Style will be a combination of these two flags:
			style (| dpANIMATION_BLACKOUT dpOPEN_FADEPALETTE)
			horizon 50
			vanishingX 130
			vanishingY 50
			noun N_ROOM
		)
	)


Exports
===========

Public procedures and instances are exported from a script, meaning they can be used from other scripts. Associated with each is an export index.
In SCI Studio, this index was implicitly defined based on where in the source file the procedure or instance was located. The problem with that
is that the index will change when you move things around in a source file, or remove or add procedures or instances. Other scripts might refer
to "export 4 from script 111". You would then, at the very least, need to go recompile all other scripts. Or at the worst (in the case of using the ScriptID kernel),
change other code so that the new index is used.

To avoid that, |scicomp| syntax version 2 requires you to list the exports explicitly and assign them an index. This also facilitates decompiling Sierra scripts,
as they often had gaps in the export indices::

	(version 2)
	(exports
		0 GetMouseRelease
		3 GetNumber
	)

	(procedure public (GetMouseRelease)
		...
	)

	(procedure public (GetNumber)
		...
	)

The export indices do not need to be sequential.

