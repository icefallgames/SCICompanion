.. EditControl

.. default - domain::js

.. include:: /includes/standard.rst

====================
EditControl (Kernel)
====================

.. function:: EditControl(control event)

	
	By specifying an edit control object and an event object,
	it will handle everything related to the edit control,
	such as letting the user enter text.
	
	The event object is then updated accordingly.
	
	:param heapPtr control: An object that inherits from :class:`DEdit`.
	:param heapPtr event: An :class:`Event` object.
	
	See Also: :func:`DrawControl`, :func:`HiliteControl`


