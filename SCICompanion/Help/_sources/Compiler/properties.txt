.. Properties

.. include:: /includes/standard.rst

================
 Properties
================

Properties are member variables of classes and instances. They are 16-bit values declared in the properties block.

Properties are not limited to just integers. They can also be declared with strings, saids, and local variable addresses. In latter cases, the 16-bit
address is used.

Example::

	(instance {Glowing Gem} of InventoryItem 
		(properties 
			said '~ gem [ AND glowing ] ~'
			description "You are still carrying the piece of orium"
			view 242
			someVarAddr @SomeVar
		)
	)

In :doc:`version` 2 of the SCI syntax, constant expressions are also permitted. These are evaluated at compile type, and can consist of 
arithmetic operators and constant values.

Example::

	(instance mrDeath of Actor
		(properties
			// fixPriOn and ignAct are both constants. signal will be equal
			// to the bitwise OR of fixPriOn and ignAct.
			signal (| fixPriOn ignAct)
		)
	)