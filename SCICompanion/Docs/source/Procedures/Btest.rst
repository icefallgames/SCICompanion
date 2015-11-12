.. Btest

.. default - domain::js

.. include:: /includes/standard.rst

===============
Btest (Main.sc)
===============

.. function:: Btest(flag)

	
	Tests a boolean game flag.
	
	:param number flag: The number of the flag to test.
	:returns: TRUE if the flag is set, otherwise FALSE.
	
	Example usage::
	
		(if (not Btest(FLAG_OpenedSewer))
			TextPrint("You can't enter, the sewer is closed.")
		)


