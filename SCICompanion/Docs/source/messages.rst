.. Message editing

.. include:: /includes/standard.rst

.. IMPORTANT::
    This section applies to SCI 1.1 only

================
 Message editor
================

.. toctree::
    :maxdepth: 1
    :hidden:

    messageaudio
    talkers

.. figure:: /images/CharSpeak.jpg
    :align: center

    A character in King's Quest 6 speaking a message resource.

In SCI 1.1, message resources are used for nearly all in-game text. They have special features, however,
to enable them to be easily associated with various actions on different objects. In contrast to SCI0, almost no
code is needed to display text to the player in response to the player doing something with an object.

.. figure:: /images/MessageEditor.png

    The message editor consists of the main list of messages, a *message details* pane to edit properties of a particular message entry, and an optional speech and lip-sync section. In addition, there are
    panes to edit the global verbs and talkers, and the nouns and conditions for this room.

What's in a message
===================

.. image:: /images/MessageEntry.PNG

When you open a message for a module (e.g. a room), you'll see that each message entry consists of the following information:

- The actual text of the message.
- A **noun** indicating the object to which the message applies.
- A **verb** indicating the action that triggers the message.
- An optional **condition** (refered to in code as a case).
- An optional **sequence** number.
- A **talker** number that indicates who is delivering this message. Often it is the game "narrator", but it may also be a character.

A message is uniquely defined by its module number (i.e. room number), and its noun, verb, condition and sequence. That means these combined values must 
be different for each individual message. |scicomp| enforces this in the message editor, so it's hard to go wrong.

Nouns and verbs
---------------

Some verbs are already defined for you when you create a new game. These include V_LOOK, V_TALK, V_WALK, V_DO, V_HELP and V_COMBINE.
Verbs are numbers, but they are associated with a text label in order to make your code easier to understand. These text labels are defined
in the Verbs.sh header file, and are also shown in the message editor. Verbs are global in scope. There is a single Verbs.sh for the entire game,
which contains all the label to number mappings. You don't edit the Verbs.sh header file directly - instead, you can add, edit and remove verbs in
the pane on the lefthand side of the message editor.

.. figure:: /images/Verbs.PNG

    The global verbs list

Nouns, in contrast, are scoped to the particular room you are in. When you create a new room in your game, a header file (nnn.shm, which nnn is the room number) is already set up for
you that will contain one noun: N_ROOM. You can add as many as you which, corresponding to objects in the room. The ego (main character) is a bit special. It is
associated with module 0 (the main script), which by default has a N_EGO noun defined in its message header file (0.shm).

.. figure:: /images/MessageHeaderInclude.PNG

    The message header is included by default in a new room script

.. figure:: /images/Nouns.PNG

    The room's nouns list in the message editor, which corresponds to the values defined in the .shm header

When you create a new room, if you open its corresponding message resource you'll see one message entry already created for you. It's associated with the N_ROOM noun, and the V_LOOK verb.
That means when the player clicks on the room background with the "look" icon, they will see this message in response.

When adding objects to the room, you can add a noun in the message editor, and then set the object's *noun* property to that noun. Then, in the message editor it is a simple matter to add
message entries corresponding to various actions taken on that noun (e.g. V_LOOK, V_TALK, V_DO).

.. figure:: /images/MessageLightSwitchCode.PNG

    Set the noun in the object's declaration in script

.. figure:: /images/MessageLightSwitch.PNG

    Then add message entries for it


Conditions
----------

Sometimes, you'll have objects that are in various different states. For instance, a book or door can be open or closed. You might want
to show different messages depending on the state of the object. While you *could* just change the noun of the object (N_BOOKCLOSED, N_BOOKOPEN), that
could cause other difficulties. 

Conditions can be used for anything, but they are commonly used to distinguish object states. Conditions, like nouns, are scoped to the current room.

Sequences
---------

Often, you'll want a series of messages to be displayed in sequence in response to some in-game action. To do so, you can create a series of message entries with the same noun/verb/condition values,
but with increasing sequence numbers. Sequence numbers start at 1 and continue up to 36.

.. TIP::
    You can select an existing message entry and use the Add Sequence button to add a message entry with an increased sequence number.

Talkers
-------

In addition to the message text itself, a talker value must be associated with each message. By default, this will be talker 99, or N_NARRATOR. This is generally the value you should use for 
text that is not part of a game characters speech. Talkers, like verbs, are global in scope. They are defined in Talkers.sh, which can be edited from the message editor.

:doc:`More information on Talkers and Narrators. <talkers>`


Speech
======

If the game supports it (such as the SCI 1.1 template game), a spoken text (or any digital audio sample) can be associated with each message entry.
|scicomp| can record audio for you, or you can import a .wav file. In addition, lip-syncing can be automatically generated and applied so that the speech matches
a character's lip movement.

:doc:`More information speech. <messageaudio>`


Special encodings
=================

Message text can contain information about which fonts and colors to use. Here are some examples::

    Hello, |c5|this part is in color number five|c|, and this is not.
    Hello, |f8|this part is in font eight|f|, and this is not.
    |f5|This entire message is in font five.

The mapping for the font and color indices is provided by the calls to *TextColors* and *TextFonts* in your game's init method in Main.sc.

In addition, the message text can contain stage directions. These were instructions for Sierra's voice actors, and they instructions do not appear when the
text is printned (or when it's used to generate lip-sync data in |scicomp|). For example::

    Alexander, (ANGRY VOICE) come here right now!

This will print the following on screen: "Alexander, come here right now!". Anything within parentheses that does *not* contain lower case letters or digits will
be counted as stage directions.

In Code
=============

In the simplest case, all that's needed to trigger messages are to have a :class:`Feature`, :class:`View`, :class:`Prop` or :class:`Actor` with a noun associated with it:

.. code-block:: python
    :emphasize-lines: 10

    (instance mug of Prop
        (properties
            view 901
            loop 2
            x 262
            y 165
            approachX 242
            approachY 162
            signal ignAct
            noun N_MUG
        )
    )

Then when the player performs an action (V_DO, V_TALK, etc...) on the item, the associated message (or sequence of messages) will be displayed.

Of course, you may need to trigger messages as part of a series of game events in a changeState or some other method. In that case, you can make a direct call to the messager:

.. code-block:: python
    :emphasize-lines: 13

    (local numberTimesTalked = 0)
        ...

    (instance mug of Prop)
        ...

        (method (doVerb theVerb params)
            (switch (theVerb)
                (case V_TALK
                    (if (< numberTimesTalked 3)
                        // In this case, we'd have 3 messages for N_MUG/V_TALK, with cases 0 1 and 2.
                        // Each time the player tried to talk to a mug, it would show a message with a successive case.
                        (send gTestMessager:say(N_MUG V_TALK numberTimesTalked 1))
                        (++numberTimesTalked)
                    )(else
                        // Until they talked to the mug too many times, they're crazy.
                        Die(DEATH_REASON_CRAZY)
                    )
                )
                (default
                    (super:doVerb(theVerb rest params))
                )
            )
        )
    )

When using gTestMessager:say(noun verb condition sequence), if the sequence number is 0, then all messages with the noun/verb/condition tuple will be shown in sequence.
Otherwise, only the one with the specified sequence number will be shown.

The noun/verb/condition/sequence tuple is used in many other places in the game too. So for instance, to show a button in a dialog, you can do::

    (Print:
        // message N_MYDIALOG/V_LOOK/0/1, at (60, 0), from message resource 101.
        addText(N_MYDIALOG V_LOOK 0 1 60 0 101)
        // Button value 0, message N_MYDIALOG/V_LOOK/0/2, at (70, 35), from message resource 101.
        addButton(0 N_MYDIALOG V_LOOK 0 2 70 35 101)
        init()
    )


