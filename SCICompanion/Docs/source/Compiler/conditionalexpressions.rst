.. Conditional expressions

.. include:: /includes/standard.rst

===========================
 Conditional expressions
===========================

Conditional expressions evaluate and return boolean (TRUE and FALSE) expressions for conditional and looping expressions.
They can contain conditional operators, **and** and **or**.

Examples of conditional expressions::

	// The branch is taken if SomeVar is TRUE, *or* both the following things are true:
	// SomeOtherVar is TRUE and AnotherVar is FALSE.
	(if (SomeVar or (SomeOtherVar and not AnotherVar))
		= x 10
	)(else 
		= x 20
	)

Differences between SCI Studio and |scicomp|
=================================================================================

SCI Studio did not handle nested **and** and **or** operators properly. For instance, consider the following code::

	(if (FALSE and (FALSE or TRUE))
		TextPrint("Hi!")
	)

In SCI Studio, this would be interpreted as if the parenthese weren't there. As a result the final "or TRUE" would cause the branch to be taken.

In |scicomp|, when using **(version 2)** at the top of the script file (which is the default) (see :doc:`version`), the conditional expression is evaluated according to the
order specified in the parentheses. That is, first (FALSE or TRUE) is evaluated to TRUE. Then, that we evaluate (FALSE and (TRUE)), which will be FALSE. The
branch will not be taken.
