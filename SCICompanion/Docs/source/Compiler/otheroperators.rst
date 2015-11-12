.. Other operators

.. include:: /includes/standard.rst

================
 Other operators
================

not
=====

Mainly used in boolean expressions, it turns TRUE expressions (<> 0) FALSE, and FALSE expressions (== 0) TRUE.

Examples::

	(= Result not TRUE) // the result will be FALSE

	(= Result not FALSE) // the result will be TRUE

	(= Result not 5) // the result will be 0

	(= Result not 0) // the result will be 1


neg
=====

Negates integers. If the number is positive, it will become negative. If the number is negative, it will become positive.

Examples::

	(= Result neg 10) // the result will be -10

	(= Result neg -45) // the result will be 45

