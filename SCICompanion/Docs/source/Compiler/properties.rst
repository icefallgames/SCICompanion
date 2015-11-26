.. Properties

.. include:: /includes/standard.rst

================
 Properties
================

Properties are member variables of classes and instances. They are 16-bit values declared in the properties block.

Properties are not limited to just integers. They can also be declared with strings, saids, local variable addresses, and constant arithmetic expressions. In latter cases, the 16-bit
address is used.

Example::

	(instance {Glowing Gem} of InventoryItem 
		(properties 
			said '~ gem [ AND glowing ] ~'
			description "You are still carrying the piece of orium"
			view 242
			someVarAddr @SomeVar
			signal (| fixPriOn ignAct) ; fixPriOn and ignAct are constant defines.
		)
	)
