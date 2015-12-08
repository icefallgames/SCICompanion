.. Continue and ContIf statement

.. include:: /includes/standard.rst

================================
 continue and contIf statements
================================

Similar to the :doc:`break`, except that continue and contif jump to the beginning of the loop instead of the exit.

syntax::

	(continue)

	(contif conditional_expression)

Example using the continue statement::

	; Look for the next active item.
	(repeat
		(= curItem (data next:))
		(if (not curItem)
			(continue) ; jump back to begining of loop
		)

		; do more stuff before looping...
	)				

Example using the contif statement::

	; Look for the next active item.
	(repeat
		(= curItem (data next:))
		(contif (not curItem))	; maybe jump back to begining of loop

		; do more stuff before looping...
	)				


See also: :doc:`for`, :doc:`while`, :doc:`repeat`, :doc:`break`.