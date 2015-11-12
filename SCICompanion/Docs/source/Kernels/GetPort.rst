.. GetPort

.. default - domain::js

.. include:: /includes/standard.rst

================
GetPort (Kernel)
================


.. function:: GetPort()

	Retrieves the currently active port and returns it.

	:returns: The currently active port.

	Example::

		(var hWnd, oldPort)
		= oldPort GetPort()
		// Draw a white window at 50, 20 with a width of 200 100, a title, and a black foreground.
		= hWnd NewWindow(
			50 20 250 120
			"Test Window"
			nwTITLE
			nwON_TOP
			clBLACK
			clWHITE
		)

		SetPort(hWnd)
		Display("Hello in the hWnd window!")
		SetPort(oldPort)
		Display("Hello in the oldPort!")
		DisposeWindow(hWnd)


