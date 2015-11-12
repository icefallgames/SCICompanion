.. Return

.. include:: /includes/standard.rst

================
 Return
================

Return exits from a method or procedure.

Syntax::

	return

or::

	return(code block)

Example::

	(procedure (CheckMouse)
		return(HaveMouse())
	)

	(procedure (AnotherProc) 
		(if(CheckMouse())
			Display("There's a mouse!")
		) 
	)
