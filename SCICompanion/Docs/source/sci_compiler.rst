.. SCI Compiler

.. include:: /includes/standard.rst

==========================
 The SCI Compiler
==========================

The script compiler in |scicomp| uses a syntax that is nearly identical to the compiler in SCI Studio. It is more strict, so
some things that compile in SCI Studio may not compile in |scicomp|, or may give warnings (this is a good thing). It also
generates slightly smaller code than SCI Studio.

SCI is an object-oriented language with message passing semantics (like Objective-C or SmallTalk). Syntactically, it also looks
a lot like LISP (due to the plethora of parentheses).

One difference that will be programmers familiar with C++, C# or java, is that expressions use Prefix notation instead of Infix notation.
For instance, to add two numbers together and assign them to a variable in C#, might look like this::

    // C#
    int i = 5 + 6;

But in SCI script it looks like::

    // SCI
    (var i)
    (= i (+ 5 6))

The topics below describe all the various language features and syntax.


.. toctree::
   :maxdepth: 2

   Compiler/version
   Compiler/classes
   Compiler/properties
   Compiler/procedures
   Compiler/variables
   Compiler/variabletypes
   Compiler/comment
   Compiler/define
   Compiler/include
   Compiler/use
   Compiler/strings
   Compiler/saids
   Compiler/synonyms
   Compiler/selectors
   Compiler/sendselfsuper
   Compiler/return
   Compiler/codeblock
   Compiler/assignment
   Compiler/arithmetic
   Compiler/bitwise
   Compiler/relationaloperators
   Compiler/otheroperators
   Compiler/conditionalexpressions
   Compiler/if
   Compiler/switch
   Compiler/while
   Compiler/do
   Compiler/for
   Compiler/break
   Compiler/rest
   Compiler/asm
   Compiler/objectfiles
   Compiler/classheader
