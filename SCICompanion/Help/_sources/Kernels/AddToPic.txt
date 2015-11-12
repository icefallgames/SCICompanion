.. AddToPic

.. default - domain::js

.. include:: /includes/standard.rst

=================
AddToPic (Kernel)
=================

.. function:: AddToPic(pList)

	
	Draws the views contained in the viewList directly onto the picture
	as if they were originally part of the picture.
	They will stay there until the picture is discarded.
	
	:param k_list list: A list of object that inherit from :class:`View`.
	
	.. NOTE::
		In general, this should not be used. Instead, addToPic() on should be called on the individual :class:`View` objects instead.
		It will allow keeping the proper state across game save and restore.


