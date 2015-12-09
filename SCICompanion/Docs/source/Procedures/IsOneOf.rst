.. IsOneOf

.. default - domain::js

.. include:: /includes/standard.rst

===================
IsOneOf (System.sc)
===================


.. function:: IsOneOf(thing [...])

::

	(= roomNumber 100)
	(= thisWillBeTRUE (IsOneOf roomNumber 50 100 150 200))

:returns: TRUE if thing is equal to one of the parameters that follows it.


