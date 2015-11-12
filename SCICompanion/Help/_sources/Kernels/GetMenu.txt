.. GetMenu

.. default - domain::js

.. include:: /includes/standard.rst

================
GetMenu (Kernel)
================


.. function:: GetMenu(menuItem subFunction [params...])

	Get a menu item's properties. Following the menuItem parameter, you can specify what property you would like to obtain with a subFunction parameter.

	:param number menuItem: A menu item identifier, e.g. $0302 for the second item in the third menu.
	:param number subFunction: One of smMENU_SAID, smMENU_TEXT, smMENU_SHORTCUTKEY, smMENU_ENABLE.

	Examples::

		(var theSaid, theKey)
		// Retrieve the said string for an item.
		= theSaid GetMenu($202 smMENU_SAID)

		// Get the menu item's text.
		Print(GetMenu($202 smMENU_TEXT))

		// Get the menu item's shortcut key.
		= theKey GetMenu($202 smMENU_SHORTCUTKEY)

		// Tell if a menu item is enabled.
		(if(SetMenu($202 smMENU_ENABLE))
		   Print("The item is enabled!")
		)


