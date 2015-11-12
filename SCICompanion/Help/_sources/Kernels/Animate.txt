.. Animate

.. default - domain::js

.. include:: /includes/standard.rst

================
Animate (Kernel)
================

.. function:: Animate(cast cycle)

.. function:: Animate(cast)
	:noindex:

.. function:: Animate()
	:noindex:

	
	Draws the background picture and foreground views.
	
	:param k_list cast: The game cast.
	:param boolean cycle: TRUE if the views be cycled.
	
	If the background picture has not yet been drawn, it executes it's
	opening animation specified in the :func:`DrawPic` kernel call.
	If specified, it draws the views in the cast list on the foreground.
	
	If the cycle property is specified, and TRUE, it cycles the views as well, calling their doit() method.


