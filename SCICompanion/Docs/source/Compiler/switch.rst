.. switch statement

.. include:: /includes/standard.rst

==================
 switch statement
==================

Use the switch statement to pass control to a case which matches the result of the switch_expression. If no case satisfies the condition, the default case is executed.

syntax::

	(switch switch_expression
		(case1
			code_block_1
			code_block_2
			...
		)
		(case2
			code_block_1
			code_block_2
			...
		)
		. 
		. 
		. 
		(else
			code_block_1
			code_block_2
			...
		)
	)

Example using the switch statement - prints "01.....7....C........" to the screen::

	(for ((= i 0)) (< i 20) ((++ i))
		(switch i
			(0 
				(Display "0" dsCOLOR clLIME) 
			) 
			(1 
				(Display "1" dsCOLOR clBLUE) 
			) 
			(7 
				(Display "7" dsCOLOR clYELLOW) 
			) 
			(12 
				(Display "C" dsCOLOR clCYAN) 
			) 
			(else
				(Display "." dsCOLOR clWHITE) 
			) 
		) 
	)

Switch statements can also be evaluated as expressions::

	; Set direction based on the key pressed.
	(= direction
		(switch (pEvent message?)
			(KEY_W
				UP
			)
			(KEY_S
				DOWN
			)
			(KEY_D
				RIGHT
			)
			(KEY_A
				LEFT
			)
			(else
				CENTER
			)
		)
	)

See also: :doc:`if`, :doc:`switchto`, :doc:`cond`.