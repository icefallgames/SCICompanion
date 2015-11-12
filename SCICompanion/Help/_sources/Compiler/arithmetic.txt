.. Arithmetic Operators

.. include:: /includes/standard.rst

=======================
 Arithmetic Operators
=======================

Addition +
============

Adds two values together::

	// Result will be 40.
	= Result (+ 30 10)


Subtraction -
===============

Subtracts two values from each other::

	// Result will be 20
	= Result (- 30 10)


Multiplication \*
====================

Multiplies two values together::

	// Result will be 300
	= Result (* 30 10)

Division \
==============

Divides one number by another::

	// Result will be 3
	= Result (/ 30 10)

Note that floating point numbers are not supported in SCI, so this is always an integer division.

Modulus  %
============

Divides two values and returns the remainder::

	// Result will be 5
	= Result (% 35 10)


Increment ++
===============

Increments a variable or property::

	(= Result 10)
	++Result

Decrement --
=============

Decrements a variable or property::

	(= Result 10)
	--Result

