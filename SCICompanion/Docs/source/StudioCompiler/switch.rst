.. Switch statement

.. include:: /includes/standard.rst

==================
 Switch statement
==================

Use the switch statement to pass control to a case which matches the result of the switch_expression. If no case satisfies the condition, the default case is executed.

syntax::

	(switch (switch_expression)
		(case (number)
			code_block_2
		)
		. 
		. 
		. 
		(default
			code_block_3
		)
	)

Example using the switch statement - prints "01.....7....C........" to the screen::

	(for (= i 0) (< i 20) (++i) 
		(switch (i)
			(case 0 
				Display("0" dsCOLOR clLIME) 
			) 
			(case 1 
				Display("1" dsCOLOR clBLUE) 
			) 
			(case 7 
				Display("7" dsCOLOR clYELLOW) 
			) 
			(case 12 
				Display("C" dsCOLOR clCYAN) 
			) 
			(default 
				Display("." dsCOLOR clWHITE) 
			) 
		) 
	)

Switch statements can also be evaluated as expressions::

	// Set direction based on the key pressed.
	(= direction
		(switch (pEvent:message))
			(case KEY_W
				UP
			)
			(case KEY_S
				DOWN
			)
			(case KEY_D
				RIGHT
			)
			(case KEY_A
				LEFT
			)
			(default
				CENTER
			)
		)
	)