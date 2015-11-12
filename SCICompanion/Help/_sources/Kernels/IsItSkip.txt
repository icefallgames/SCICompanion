.. IsItSkip

.. default - domain::js

.. include:: /includes/standard.rst

=================
IsItSkip (Kernel)
=================

.. function:: IsItSkip(view loop cel x y)

	
	Checks whether a pixel in a cel is transparent. This can be used to enhance hit testing on game objects,
	especially if they have big holes in them.
	
	:param number view: View number.
	:param number loop: Loop number.
	:param number cel: Cel number.
	:param number x: The x coordinate in the view, relative to the top left.
	:param number y: The y coordinate in the view, relative to the top left.
	:returns: TRUE if the pixel is transparent.


