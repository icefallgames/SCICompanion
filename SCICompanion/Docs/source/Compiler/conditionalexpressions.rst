.. Conditional expressions

.. include:: /includes/standard.rst

===========================
 Conditional expressions
===========================

Conditional expressions evaluate and return boolean (TRUE and FALSE) expressions for conditional and looping expressions.
They can contain conditional operators, **and** and **or**.

Examples of conditional expressions::

	; The branch is taken if SomeVar is TRUE, *or* both the following things are true:
	; SomeOtherVar is TRUE and AnotherVar is FALSE.
	(if (or SomeVar (and SomeOtherVar (not AnotherVar)))
		(= x 10)
	else 
		(= x 20)
	)

	; The or and and operators can take multiple arguments. In this case, if either SomeVar,
	; AnotherVar or AThirdVar are TRUE, the branch will be taken.
	(if (or SomeVar AnotherVar AThirdVar)
		(= x 10)
	)

	; Sometimes it can be useful to place the condition terms on more than one line
	; Here the branch is taken if the ego is on green, its x coordinate is less than 100,
	; and it has the radio transmitter in its inventory.
	(if (and
			(& ctlGREEN (gEgo onControl:))
			(< 100 (gEgo x?))
			(gEgo has: INV_TRANSMITTER)
		)
		(Print "The transmitter begins to beep")
	)