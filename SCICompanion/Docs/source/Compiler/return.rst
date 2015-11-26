.. Return

.. include:: /includes/standard.rst

================
 Return
================

Return exits from a method or procedure.

Syntax::

	(return)

or::

	(return [optionalExpression])

Example::

	(procedure (CheckMouse)
		(return (HaveMouse))
	)

	(procedure (AnotherProc) 
		(if (CheckMouse)
			(Display "There's a mouse!")
			(return) ; return immediately from procedure here
		) 
		(Display "No mouse, gonna do other stuff")
		; do other stuff...
	)
