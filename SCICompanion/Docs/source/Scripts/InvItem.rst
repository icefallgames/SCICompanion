.. InvItem.sc

.. default - domain::js

.. include:: /includes/standard.rst

==========
InvItem.sc
==========


This is the script that you modify to add additional inventory item instances that inherit from :class:`InvItem`.

An example might be::

	(instance Hammer of InvItem
		(properties
			view 900
			loop 1
			cursor 900			// Optional cursor when using this item.
			message V_HAMMER	// Optional verb associated with the item.
			signal $0002
			noun N_HAMMER		// noun from message resource 0
		)
	)

Then in templateInventory::init(), add the inventory item to the add() call.



Classes
==========

:class:`InvItem` of InvI

