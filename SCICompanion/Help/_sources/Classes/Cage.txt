.. Cage

.. default - domain::js

.. include:: /includes/standard.rst

======================
Cage (of :class:`Blk`)
======================

.. class:: Cage

	Defined in Blk.sc.

	
	This class defines a rectangular area to which an Actor is constrained. In SCI1.1, this has mostly been
	superceded by Polygons.
	
	Example usage::
	
		(theFrog:observeBlocks((send ((Blk:new())):
			left(150)
			top(140)
			right(170)
			bottom(160)
			yourself()
		)


Properties
==========

Inherited from :class:`Blk`:

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



