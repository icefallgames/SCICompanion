.. Scoring and Flags

.. default - domain::js

.. include:: /includes/standard.rst

.. IMPORTANT:: SCI1.1 only.

====================================
Scoring and Flags
====================================

When the player successfully kills the evil zombie guy, they should be rewarded some. So let's
add something to their score.

Adding directly to the score
================================

That can be as simple as using the AddToScore procedure (from Main.sc). So let's add it to the throwBoxScript:

.. code-block:: python
    :emphasize-lines: 4-5

    (
        (evilEgo hide:)
        (gGame handsOn:)
        ; Add 5 points to the player's score:
        (AddToScore 5)
        ; Box hangs in the air for a couple of seconds for comedic effect
        (= seconds 2)
        (theBox posn: (theBox x?) (- (theBox y?) 30) 0)
    )

Now when the ego throws the box and hits the evil ego, the score sound will play (sound resource 1000) and 5 points will be
added to the score in the status bar.

Assigning a value to an inventory item
=========================================

Another way to add something to the player's score is to assign a value to an inventory item. When the player
first picks up the item, the score will be added.

Go into the Inventory script and find the BlueBox. Add a **value** property to it:

.. code-block:: python
    :emphasize-lines: 9

    (instance BlueBox of InventoryItem
        (properties
            view 900
            cursor 900
            loop 1
            signal $0002
            noun N_BOX
            message V_BOX
            value 3
        )
    )

This is useful, because the score won't be added again if the ego picks up the box a second time (try it).


Flags
==============================

To keep track of various important events and states that happen in your game, you can add global variables
to the Main.sc script. If the important event is just a true or false value, however, you can instead define
a flag. Flags are more memory efficient, since 16 of them can be packed into the space of one 16-bit variable.

Suppose we want the fact that you shrunk the box to be remembered even if you left the room and came back. We were using
our *hasBoxShrunk* local variable, but instead we could
define a flag F_ShrunkBox to indicate this, and then it can be tested and set anywhere in the game.
To do so, open game.sh from the Quick links pane of the script editor, scroll
to the end, and add this::

    ; flags
    (enum
        F_ShrunkBox
        ; You can add more here later...
    )

Now go back to rm100 and find the shrinkBoxScript. Modify the third case to set the F_ShrunkBox flag:

.. code-block:: python
    :emphasize-lines: 2

    (
        (Bset F_ShrunkBox)
        (Prints {The box shrunk!})
        (gGame handsOn:)
    )

Also modify the place in theBox's doVerb where we were checking for hasBoxShrunk:

.. code-block:: python
    :emphasize-lines: 4-5

    (method (doVerb theVerb)
        (switch theVerb
            (V_DO
                (if (not (Btest F_ShrunkBox))
                    (self setScript: shrinkBoxScript)
                else
                    (Prints {The box is small now, so you can pick it up!})
                    (gEgo get: INV_BOX)
                    (theBox hide:)
                )
            )
            (else 
                (super doVerb: theVerb &rest)
            )
        )
    )

This will come in useful later when we go to another room, and can remember that the box was shrunk.

Now, what if we wanted to give the player some points for shrinking the box? We can certainly Bset the flag
and then AddToScore. However, we can also take advantage of the **ScoreFlag** procedure to do both those things
in one. ScoreFlag also checks that the flag is not already set - in that case the points won't be added. So this
is a good way to ensure that you only add points *once* for a particular event.

.. code-block:: python
    :emphasize-lines: 2

    (
        (ScoreFlag F_ShrunkBox 3)
        (Prints {The box shrunk!})
        (gGame handsOn:)
    )

.. IMPORTANT::
    The template game allocated 14 16-bit spots for the global variable gFlags, where the flags are stored. That's enough room
    for 224 flags. If you need more, increase the size of the gFlags array.
