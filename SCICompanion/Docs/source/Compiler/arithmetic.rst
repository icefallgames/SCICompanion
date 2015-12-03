.. Arithmetic Operators

.. include:: /includes/standard.rst

=======================
 Arithmetic Operators
=======================


Syntax::

	(operator operand)							; Unary operators
	(operator operand1 operand2)				; Binary operators
	(operator operand1 operand2 ... operandn)	; N-ary operators

Addition +
============

Adds two or more values together::

	; Result will be 40.
	(= Result (+ 30 10))
	; Result will be 60
	(= Result (+ 30 10 20))


Subtraction -
===============

Subtracts two values from each other::

	; Result will be 20
	(= Result (- 30 10))


Multiplication \*
====================

Multiplies two or more values together::

	; Result will be 300
	(= Result (* 30 10))
	; Result will be 6000
	(= Result (* 30 10 20))

Division \
==============

Divides one number by another::

	; Result will be 3
	(= Result (/ 30 10))

Note that floating point numbers are not supported in SCI, so this is always an integer division.

Modulus  mod
============

Divides two values and returns the remainder::

	; Result will be 5
	(= Result (mod 35 10))


Increment ++
===============

Increments a variable or property::

	(= Result 10)
	(++ Result)	; Result is now 11

Decrement --
=============

Decrements a variable or property::

	(= Result 10)
	(-- Result)	; Result is now 9

