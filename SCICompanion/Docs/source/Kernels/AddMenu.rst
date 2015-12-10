.. AddMenu

.. default - domain::js

.. include:: /includes/standard.rst

================
AddMenu (Kernel)
================

.. function:: AddMenu(caption itemCaptions)

	
	Adds a new menu item with the specified caption to the menubar.
	The string itemCaptions specifies the submenu items.
	The itemCaptions string can contain format characters to seperate items and format the text.    
	
	Format Characters:
	
	\`
		Tells the interpreter to right justify the following text
	
	\:
		Specifies the end of current item, the following will be the next item.
	
	--!
		Specifies that the menu item will just be a seperator.
	
	#	
		Specifies the item's function key.
	
	^
		Specifies the item's control+key combination.
	
	Example::
	
		(AddMenu
			" File "
			"Restart Game`#9:Save Game`#5:Restore Game`#7:--! :Quit`^q"
		)


