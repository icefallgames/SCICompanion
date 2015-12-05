.. SCI Studio compatible compiler

.. include:: /includes/standard.rst

=================================
 SCI Studio compatible compiler
=================================

The script compiler in |scicomp| can use a syntax that is nearly identical to the compiler in SCI Studio. It is more strict, so
some things that compile in SCI Studio may not compile in |scicomp|, or may give warnings (this is a good thing). It also
generates slightly smaller code than SCI Studio.

SCI Studio script is an object-oriented language with message passing semantics (like Objective-C or SmallTalk). Syntactically, it also looks
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

   StudioCompiler/version
   StudioCompiler/classes
   StudioCompiler/properties
   StudioCompiler/procedures
   StudioCompiler/variables
   StudioCompiler/variabletypes
   StudioCompiler/comment
   StudioCompiler/define
   StudioCompiler/include
   StudioCompiler/use
   StudioCompiler/strings
   StudioCompiler/saids
   StudioCompiler/synonyms
   StudioCompiler/selectors
   StudioCompiler/sendselfsuper
   StudioCompiler/return
   StudioCompiler/codeblock
   StudioCompiler/assignment
   StudioCompiler/arithmetic
   StudioCompiler/bitwise
   StudioCompiler/relationaloperators
   StudioCompiler/otheroperators
   StudioCompiler/conditionalexpressions
   StudioCompiler/if
   StudioCompiler/switch
   StudioCompiler/while
   StudioCompiler/do
   StudioCompiler/for
   StudioCompiler/break
   StudioCompiler/rest
   StudioCompiler/asm
   StudioCompiler/objectfiles
   StudioCompiler/classheader
