.. Blk

.. default - domain::js

.. include:: /includes/standard.rst

=====================
Blk (of :class:`Obj`)
=====================

.. class:: Blk

	Defined in Blk.sc.

	
	This class defines a rectangular area that an Actor can't enter. In SCI1.1, this has mostly been
	superceded by Polygons.
	
	Example usage::
	
		(theFrog:observeBlocks((send ((Blk:new())):
			left(150)
			top(140)
			right(170)
			bottom(160)
			yourself()
		)


Subclasses: :class:`Cage`.

Properties
==========

Defined in Blk:

======== ===========
Property Description
======== ===========
top                 
left                
bottom              
right               
======== ===========


Methods
==========

.. function:: doit(param1)
	:noindex:



