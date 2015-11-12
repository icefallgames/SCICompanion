.. Platform

.. default - domain::js

.. include:: /includes/standard.rst

=================
Platform (Kernel)
=================


.. function:: Platform(subFunction)

	.. IMPORTANT:: SCI1.1 only.

	Retrieves platform information.

	:param number subFunction: One of the sub functions listed below.
	:returns: A value depending on the sub function.

	============= ============== ================================================
	SubFunction   Description    Returns
	============= ============== ================================================
	1             Get CD speed   Returns the CD speed
	3             CD check       Unknown
	4             Get platform   1 for DOS, 2 for Windows.
	5             Is not hi-res? TRUE if not on a hi-res platform.
	6             Is hi-res?     TRUE if on a hi-res platform, otherwise FALSE
	7             Is windows?    TRUE if on Windows, otherwise FALSE.
	============= ============== ================================================


