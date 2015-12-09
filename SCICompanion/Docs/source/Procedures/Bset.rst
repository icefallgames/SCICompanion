.. Bset

.. default - domain::js

.. include:: /includes/standard.rst

==============
Bset (Main.sc)
==============

.. function:: Bset(flag)

	
	Sets a boolean game flag.
	
	:param number flag: The number of the flag to set.
	:returns: The previous value of the flag (TRUE or FALSE).
	
	Example usage::
	
		(V_DO
			(Bset FLAG_OpenedSewer)
			(sewer setCel: 3)
		)


