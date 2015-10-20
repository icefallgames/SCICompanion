.. Decompiler

.. include:: /includes/standard.rst

================
 Decompiler
================

With |scicomp| you can decompile Sierra games to see how they work. This is a vast
improvement over just disassembling scripts, as it reconstructs something close to the original source
code - including conditional structures like if statements, while loops and more.

Some information can't be recovered of course. The names of variables aren't in the script resources, but
we can make some guesses based on how they are used, and what values are assigned to them.

Here's an example of a decompiled **nextCel** method from Space Quest 4's **Cycle** class:

::

 (method (nextCel)
        ++cycleCnt
        return 
            (if (<= cycleCnt (send client:cycleSpeed))
                (send client:cel)
            )(else
                = cycleCnt 0
                (if (& (send client:signal) $1000)
                    (send client:cel)
                )(else
                    + (send client:cel) cycleDir
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

Script filenames
----------------

The first thing you should do when decompiling a game is to generate good filenames for the original script files.
This can be done with the *Set Filenames* button, which will analyze the script resources and try to generate
a filename based on the contents of the script.

You can rename a script by clicking on it in the script list and pressing F2 (you can also do this from the
scripts tab in the game explorer).

Decompiling a script
--------------------

You can select one or more scripts on the left (using shift-click), and then press the *Decompile* button. Decompiling a script can be a fairly
lengthy process, so if you're only curious about a few scripts, then just select those. As the decompilation takes place, status updates
are given in the pane on the right.

If you want to decompile the entire game of course, then just select all scripts, click *Decompile* and sit back. Decompiling a typical
entire Sierra game should take about a minute or so.

When a script is decompiled, we also generate the .sco file for it. This contains the "public" information in the script (such as the names of
procedures that it exports) which is used by other scripts.

Public procedure names and variable names
-----------------------------------------

|scicomp| will try to guess good variable names. For temp and parameter names, you have no control over the the names that are used.

However, for script-wide variables and procedure names, we will place these "guessed" names in the .sco file. You can edit these names in the **SCO** pane to
make them more meaningful, and then any script that references that script will pick up those new names when it is decompiled again.

When |scicomp| encounters other scripts using a global variable (from script 0, main.sc) that still has the default name (e.g. global30), it will
try to give that variable a name based on its usage. That means that as you decompile more scripts, the names of the global variables in main.sc may change (i.e. improve).

So to get a complete decompile, it is recommended to run the decompilation steps a second time on all files so that they pick up any new
names for global variables (and any names for procedures that you have entered).

Accuracy
=======

Decompilation is not a perfect science, and there will be situations where |scicomp| is not able to reconstruct the high level language conditional structures.
In that case, the code for a method or procedure will fall back to disassembly (in an *asm* block). In general, this should only happen for about 2% to at most 5% of the
total code in a game.

There may still be some bugs in the resultant decompiled code, so if you recompile a script and suddenly the game starts behaving strangely, that is a likely suspect. If
you encounter this, please send a bug report. If you understand SCI byte code, you can also compare a method's code with its disassembled version (which is much easier to get 100% correct) to see if anything
looks wrong. If you check the *Disassembly only* box in the Decompile Dialog, this will force |scicomp| to fallback to assembly for every method or procedure.

