.. Decompiler

.. include:: /includes/standard.rst

================
 Decompiler
================

With |scicomp| you can decompile Sierra games to see how they work! This is a vast
improvement over just disassembling scripts, as it uses control flow analysis to reconstruct
higher level structures like if statement, switch statements, while loops and more.
The result is something fairly readable.

Sierra's script resources still contain a significant amount of original information. Class and instance
names are preserved, as are property and method names. And of course, we can figure out the kernel function call names.
The names of parameters, script and temp variables aren't available, but
we can make some guesses based on how they are used, and what values are assigned to them.

Decompilation was used as a base to generate the SCI 1.1 template game. It can also just be used to
see how scripts worked, or to find easter eggs, or even to modify the logic to fix bugs in the original Sierra games.

Here's an example of a decompiled **nextCel** method from Space Quest 4's **Cycle** class:

::

 (method (nextCel)
        (++ cycleCnt)
        (return 
            (if (<= cycleCnt (client cycleSpeed?))
                (client cel?)
            else
                (= cycleCnt 0)
                (if (& (client signal?) $1000)
                    (client cel?)
                else
                    (+ (client cel?) cycleDir)
                )
            )
        )
    )

And then the same method simply disassembled:

::

    (method (nextCel)
        (asm
            ipToa   cycleCnt
            pTos    cycleCnt
            pushi   #cycleSpeed
            pushi   0
            pToa    client
            send    4
            le?     
            bnt     code_002e
            pushi   #cel
            pushi   0
            pToa    client
            send    4
            jmp     code_0056
 code_002e: ldi     0
            aTop    cycleCnt
            pushi   #signal
            pushi   0
            pToa    client
            send    4
            push    
            ldi     4096
            and     
            bnt     code_004b
            pushi   #cel
            pushi   0
            pToa    client
            send    4
            jmp     code_0056
 code_004b: pushi   #cel
            pushi   0
            pToa    client
            send    4
            push    
            pToa    cycleDir
            add     
 code_0056: ret     
        )
    )

You can see how much easier the decompiled code is to understand!



The Decompile dialog
====================

All decompilation starts with the Decompile dialog, accessed from *Script->Manage Decompilation*. This
presents you with a list of scripts in the game, and whether or not there are source code (.sc) or .sco
files for each of them.

.. image:: /images/DecompileDialog.jpg

Script filenames
----------------

The first thing that needs to be done before decompiling a game is to generate good filenames for the original script files. These names
will be referenced from other scripts, so it's important that they be correct before decompiling any files. |scicomp| does this by 
analyzing the script resources and trying to generate a filename based on the contents of the script.
|scicomp| will handle this
automatically for you when you start decompiling if it finds no script names in the game.ini file - that is, if you haven't explicitly given any names to scripts
in the Game Explorer. If you want to be sure, you can click the *Reset Filenames* button.

You can rename a script by clicking on it in the script list and pressing F2 (you can also do this from the
scripts tab in the game explorer).

Decompiling a script
--------------------

You can select one or more scripts on the left (using shift-click), and then press the *Decompile* button.
Selecting no scripts will offer the option of decompiling all. So in most cases, you can just open the Decompile dialog and click *Decompile* and be done.

Decompiling a script is complex and can be a fairly
lengthy process depending on the speed of your computer, so if you're only curious about a few scripts, then just select those. As the decompilation takes place, status updates
are given in the pane on the right. Decompiling a typical entire Sierra game should take about a minute or so.

When a script is decompiled, we also generate the .sco file for it. This contains the "public" information in the script (such as the names of
procedures that it exports) which is used by other scripts. You'll need these .sco files if you want to then go in and make changes to the scripts (e.g. for a mod or bug fix to an original game)
and successfully recompile.

Public procedure names and variable names
-----------------------------------------

|scicomp| will try to guess good variable names. For temp and parameter names, you have no control over the the names that are used.

However, for script-wide variables and procedure names, we will place these "guessed" names in the .sco file. You can edit these names in the **SCO** pane to
make them more meaningful, and then any script that references that script will pick up those new names when it is decompiled again.

When |scicomp| encounters other scripts using a global variable (from script 0, main.sc) that still has the default name (e.g. global30), it will
try to give that variable a name based on its usage. That means that as you decompile more scripts, the names of the global variables in main.sc may change (i.e. improve).

So to get a complete decompile, |scicomp| will automatically re-decompile all files if it detects a change to a global variable name. It may do this two or three times.


Accuracy
===========

Decompilation is not a perfect science, and there will be situations where |scicomp| is not able to reconstruct the high level language conditional structures.
In that case, the code for a method or procedure will fall back to disassembly (in an *asm* block). In general, this should only happen for about 2% to at most 5% of the
total code in a game.

Tips
-------------------------

It is suggested that you back up your game before you start to recompile anything, as you will obviously not be able to re-decompile from the original source once that's done.

If you're just trying to change a small part of a game, it is suggested you just recompile the file you're interested in. Although the decompiler is very good, it is *possible*
that some of the generated source code is incorrect, and recompiling the entire game increases the chances that something will break in the game. If you recompile a game and it
stops working properly, please send a bug report.

Errors when you try to recompile the decompiled code can be caused by bugs in Sierra's original code, bugs in the decompiler or bugs in the compiler. If you suspect bugs in the |scicomp| decompiler
or compiler, please send a bug report. If you understand SCI byte code, you can also compare a method's code with its disassembled version (which is much easier to get 100% correct) to see if anything
looks wrong. If you check the *Disassembly only* box in the Decompile Dialog, this will force |scicomp| to fallback to assembly for every method or procedure.

Many times errors will be caused by issues with Sierra's original code and there's little the decompiler can do to produce correct source code. Here are some of the problems you might encounter.

**blah is a property, only one parameter may be supplied.**

The code might look like this:

::

    (something cycles: 0 115 0)

Here, 'cycles' is a property that is being set, and so only one value can be supplied. It turns out that 115 was supposed to be a selector, but there was a typo in Sierra's original source
code that resulted in it being just a number. You can look up the value for the selector in the selector table vocab. In this case, 115 is 'seconds'. So the correct code is:

::

    (something cycles: 0 seconds: 0)

Although it's fine to just remove the '115 0' too, as that would essentially make it behave the same as the original game.


**Undeclared identifier '--UNKNOWN-PROP-NAME--'**

This means the decompiler needed a property name, but was not able to find one. This is caused by bugs in Sierra's source code and compiler, and there's no way to know what the
original intent was. So the best you can do is to just delete the line and move on. Or you might be able to infer was actually meant. One case that appears in many Sierra games is
in the canBeHere method of Actor:

::

    (and
        (== illegalBits 0)
        (& signal --UNKNOWN-PROP-NAME--)
    )

In this case, we know that what was actually meant was ignoreHorizon, which is a define. So the correct code is:

::

    (and
        (== illegalBits 0)
        (& signal ignoreHorizon)
    )


**&rest cannot be used if the send target itself contains nested procedure calls or sends. Assign the result of the procedure call or send to a temporary variable and use that instead**

The code might look like this:

::

    (method (doVerb theVerb)
        ((ScriptID 270 1) doVerb: theVerb &rest)
    )

The problem is that the ScriptID kernel call must be evaluated first (before the doVerb method can be called on the result of the ScriptID call). However, the call
will use up the "rest" of the parameters when they were actually intended for the doVerb call. This actually has no ill effects if no extra parameters are ever passed to
doVerb. And that was probably the case in the actual game, as it would likely crash if any extra parameters were passed. In this case, there are two ways to fix it. The
first is to do what the error message suggests:

::

    (method (doVerb theVerb &tmp theTarget)
        (= theTarget (ScriptID 270 1))
        (theTarget doVerb: theVerb &rest)
    )

The other alternative is to just delete the &rest, although that theoretically changes the behavior of the game.


**Duplicate case values. Already encountered a case for '2'**

This is just a warning, so you can leave them as is. Or, delete the second duplicate case value. These indicate a switch statement
with duplicate case values. Only the first case with that value will ever be hit, so there is unreachable code.

**Ignoring public class for export: blahblah**

This is just a warning, and was caused by Sierra include a class in the exports table. You can just ignore it.

Other notes
------------

You may occasionally notice procedure calls that look like

::

    (kernel_131 blahblahblah)

This indicates that |scicomp| wasn't able to find the name of the kernel function, so it just retained the number (which is how they are invoked in byte code). It will
call the correct kernel, so no worries.

::

    (__proc0_7)

The word 'proc' preceded by two underscores indicates a call to an export that doesn't exist. The double underscore tells |scicomp|'s compiler to just generate 
an export call to the specified export (The 7th export in script 0 in the case above). Normally |scicomp| would try a symbol lookup, but that would fail because
the export doesn't exist. This was commonly used for debug code that was used only during development. The source code still has calls to this code, but
the actual was removed from the final build.
