.. GetInput

.. default - domain::js

.. include:: /includes/standard.rst

===================
GetInput (Print.sc)
===================


.. function:: GetInput(buffer maxLength [title font])

	Displays an edit box so the player can enter text.
	
	:param string buffer: A buffer in which to place the text that was entered.
	:param number maxLength: The size of the text buffer.
	:param string title: An optional title for the dialog.
	:param number font: An optional font number.
	:returns: The length of the text that was input.
	
	Example usage::
		
		(method (SomeMethod &tmp [buffer 10])
			(= buffer NULL)	// Set the first character to NULL to create an empty string
			(GetInput buffer 10 "Enter your name")
		)
	
.. IMPORTANT::
	The buffer is also used to initialize the edit field, so it should contain an empty string if you don't want any text there to start with.


