.. Assignment operators

.. include:: /includes/standard.rst

=======================
 Assignment operators
=======================

Use the assignment operators assign values to variables or properties.

The simple assignment operator is =. It assigns the result of code block to variable::

	(= variable code block)

The complex assignment operators are +=, -=, \*=, /=, mod=, &=, \|=, ^=, >>= and <<=. These operators.
These operators use the result of explression and apply the variable to it using the specified operator::

	(operator variable code block)

Example::

    (= SomeVar 10)          ; SomeVar is now 10
    (+= SomeVar 40)         ; SomeVar is now 50
    (/= SomeVar (+ 2 2))    ; SomeVar is now 10 (40 / 4)

The value of an assignment expression is the value of the resulting variable. So these can be used in conditional expressions too::

	; Assign the client property of theObj to the temp variable theClient
	(if (= theClient (theObj client?))
		; and if it's not NULL, execute the code inside the if
	)