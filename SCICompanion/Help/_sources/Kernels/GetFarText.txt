.. GetFarText

.. default - domain::js

.. include:: /includes/standard.rst

===================
GetFarText (Kernel)
===================

.. function:: GetFarText(resourceNum textId buffer)

	
	Reads a string from a text resource and copies it to buffer. It loads text resource resourceNum, and accesses the textId numbered string. It returns a pointer to buffer.
	
	:param number resourceNum: A text resource number.
	:param number textId: An id into the text resource.
	:param string buffer: The buffer that receives the string.
	:returns: A pointer to buffer.


