.. Flags

.. default - domain::js

.. include:: /includes/standard.rst

=======================
Flags (of :class:`Obj`)
=======================

.. class:: Flags

	Defined in Flags.sc.

	
	Provides a way to compactly store a bunch of boolean flags, in the same
	manner as :func:`Btest`, :func:`Bset` and :func:`Bclear`.


Properties
==========

Defined in Flags:

======== ===========
Property Description
======== ===========
size                
array               
======== ===========


Methods
==========

.. function:: init()
	:noindex:



.. function:: dispose()
	:noindex:



.. function:: setSize(param1)
	:noindex:

	Sets the maximum number of flags.


.. function:: set(theFlag)
	:noindex:

	Sets a flag (or multiple flags).


.. function:: clear(theFlag)
	:noindex:

	Clears a flag (or multiple flags).


.. function:: test(theFlag)
	:noindex:

	Returns TRUE if the flag is set.


