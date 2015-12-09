.. Differences from SCI Studio syntax

.. include:: /includes/standard.rst

=====================================
 Differences from SCI Studio syntax
=====================================

The script syntax used by SCI Studio (and previous versions of |scicomp|) was an educated guess at what the original
Sierra Script syntax looked like. It's similar in many ways, but also has some important differences.

Sierra Script is more faithful to its roots in LISP, and more rigorous and strict about parentheses. You can think
of each statement as an expression enclosed in parentheses.

Quick guide
================

Here's a easy-reference table of some of the differences:

================================= ========================================== =================================================
 Operation                         SCI Studio                                 Sierra Script
================================= ========================================== =================================================
 Calling a procedure               **Proc**\(param1 param2)                   (**Proc** param1 param2)
 Calling a method                  (send gEgo:posn(4 5))                      (gEgo posn: 4 5)
 Calling a method                  (anInstance: init())                       (anInstance init:)
 Calling multiple methods          (anInstance: posn(4 5) init())             (anInstance posn: 4 5 init:)
 Retrieving a property             (send gEgo: x)                             (gEgo x?)
 Declare an array                  myArray[5]                                 [myArray 5]
 Initialize an array               myArray[3] = (1 10 100)                    [myArray 3] = [1 10 100]
 Reference an array                (= temp0 myArray[2])                       (= temp0 [myArray 2])
 Items not equal                   (if (**<>** one two) Print("not equal") )  (if (**!=** one two) (Print {not equal}) )
 Bitwise not                       (= temp0 (bnot signal))                    (= temp0 (~ signal))
 Conditional expressions           (if (A and B) Print("Both!") )             (if (and A B) Print({Both!}) )
 Temp variables                    (var temp0, temp1)                         (procedure (Proc &tmp temp0 temp1) ... )
================================= ========================================== =================================================


Identifying a script as being in Sierra Script
===============================================

Sierra Script files use the same file extensions as SCI Studio files. To identify a Sierra Script, |scicomp| looks for a Sierra Script token
in the first comment of the file::

	; Sierra Script - don't remove this comment

Both languages can be mixed and matched in the same project (but not the same file), though it is not recommended you do so (but this might be the case if you are
converting an old project to the Sierra Script syntax file-by-file).

Enclosing expressions in parentheses
========================================

Sierra Script is much more strict regarding parentheses. Unlike SCI Studio syntax, you can't arbitrarily enclose expressions
in parentheses, nor can you leave them out::

    (method (foo)
        (var temp0)
        = temp0 6       // This is legal in SCI Studio
    )

    (method (foo &tmp temp0))
        = temp0 6       ; This is a compile error in Sierra Script
        (= temp0 6)     ; This is the correct way to do it.
    )


    (while (TRUE)       // This is legal in SCI Studio
        SomeProc()
    )

    (while (TRUE)       ; This is a compile error in Sierra Script
        (SomeProc)
    )

    (while TRUE         ; This is the correct way to do it in Sierra Script
        (SomeProc)
    )

In summary, **all** expressions must be enclosed in parentheses in Sierra Script, unless it is just a single value (in which case it cannot
be enclosed, or it will be treated as a procedure call).

Public exports
=================

**public** is not longer a keyword that can be applied to procedures and instances. Instead, public instances and procedures are
listed in a separate **public** block at the top of the script file.


The send keyword
==================

The **send** keyword used in SCI Studio is not present in Sierra Script. There is no difference in sending messages to
an explicitly declared instance versus a variable that points to an object.

Suppose you had::

    (instance aMan of Actor)
        (properties
            x 100
            y 100
            view 10
        )
    )

In SCI Studio you would do::

    (aMan: posn(50 100))   // but...
    (send gEgo: posn(50 100))

In Sierra script these would both look the same::

    (aMan posn: 50 100)
    (gEgo posn: 50 100)


and/or operators are now prefix
===================================

In SCI Studio syntax, **and** and **or** behaved differently than all other oeprators in that they were infix instead of prefix.
In Sierra Script, **and** and **or** are prefix. So if statements must be written like so::

    (if (and A B))
        (Print {Both A and B are true})
    )

Note that these can also take multiple arguments::

    (if (and A B C))
        (Print {A, B and C are all true})
    )



Switch and Switchto
========================

There is no more **case** keyword for the **switch** statement. Instead, the case is the first expression in the enclosing parentheses.
SCI Studio::

    (switch (state)
        (case 0
            = seconds 2
        )
        (case 1
            (= egoSpotted TRUE)
            (aMan: setMotion(MoveTo 100 100))
        )
        (case 2
            // done
        )
    )

And in Sierra Script::

    (switch state
        (0
            (= seconds 2)
        )
        (1
            (= egoSpotted TRUE)
            (aMan setMotion: MoveTo 100 100)
        )
        (2
            // done
        )
    )

With the **switchto** statement, you can also leave out the case values altogether if your cases are sequential starting from 0::

    (switchto state
        (
            (= seconds 2)
        )
        (
            (= egoSpotted TRUE)
            (aMan setMotion: MoveTo 100 100)
        )
    )

Loops
=============

The do-loop is gone. In its place you can use the **repeat** loop with a **breakif** statement.

Strings
=============

In SCI Studio, strings are enclosed in double-quotes. In Sierra Script, they can be enclosed in curly braces or double-quotes. In the original Script
used by Sierra, double-quoted strings were automatically placed into a text resource and converted into a resource number/entry tuple, while brace-enclosed strings
were not. |scicomp| does not support this feature yet, so currently curly braces strings and double-quote strings are treated the same::

	Print("Hello there")		// SCI Studio

	(Print {Hello there})		; Sierra Script
	(Print "Hello there")		; Sierra Script

