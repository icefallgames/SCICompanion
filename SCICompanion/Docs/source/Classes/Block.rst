.. Block

.. default - domain::js

.. include:: /includes/standard.rst

==========================
Block (of :class:`Object`)
==========================

.. class:: Block

	Defined in Block.sc.

	
	This class defines a rectangular area that an Actor can't enter. In SCI1.1, this has mostly been
	superceded by Polygons.
	
	Example usage::
	
		(theFrog observeBlocks: ((Blk new:)
			left: 150
			top: 140
			right: 170
			bottom: 160
			yourself: )
		)


Subclasses: :class:`Cage`.

Properties
==========

Inherited from :class:`Object`:

======== ===========
Property Description
======== ===========
name                
======== ===========

Defined in Block:

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



