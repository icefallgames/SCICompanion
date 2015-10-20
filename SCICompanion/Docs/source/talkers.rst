.. This describes talkers and narrators

.. include:: /includes/standard.rst

.. IMPORTANT::
    This section applies to SCI 1.1 only

================
 Talkers and Narrators
================

Talkers and Narrators are responsible for delivering the in-game text and speech. They describe how
in-game text (and possibly speech) is delivered: the font, text color, dialog shape, and so on.

A Talker offers additional functionality over a Narrator: it shows a bust, blinking eyes and moving mouth
to go along with the text and/or speech.

.. figure:: /images/KQ6Narrator.png

    A Narrator from King's Quest 6. This one customizes things by using gothic lettering for the first letter.

.. figure:: /images/KQ6Talker.png

    The main character's Talker from King's Quest 6. 

The SCI 1.1 template game comes with a default Narrator in the main.sc script. It is talker number 99, or N_NARRATOR.
Talkers and Narrators are numbered from the same 0-99 namespace, defined in Talkers.sh and editable in the 
message editor. Throughout most of this documentation the term Talker is used to refer to both Talkers and Narrators,
as they are both the same from the point of view of the message editor and most game code.

Adding a Talker
===============

To add a new Talker to your game, you'll need to complete the following steps:

1. In the message editor (for any message resource), assign a new talker name to a new number in the *Global Talkers* pane. Once this is done, it will be in **Talkers.sh** and you will be able to create message entries with this Talker name.
2. Choose a script in which to implement your Talker. If the Talker only ever appears in one room, you may as well just define it in that room. Otherwise, you can create a new empty script just for this Talker.
3. In your chosen script, create the Talker (or Narrator) instance and customize as necessary. Then add the instance to the list of exports for that script (using the :doc:`exports <exports>` keyword).
4. In the main.sc script, find the *findTalker* method in the *testMessager* instance, and insert a new case in the switch statement for *talkerNumber*. The case value should be the talker name you used in step 1. In this case statement, call ScriptID kernel with the script number you chose in step 2, and the export number you chose in step 3.
5. Finally, *if* you chose a new script in step 2, you'll need to add this script number to the list of scripts to dispose on a room change in **dispose.sc**.

If this is a Talker and not a Narrator, now you will need to draw :doc:`views <views>` that comprise the bust, mouth and eyes. In the same script
as your Talker instance, you'll need to create *Props* for the bust, mouth and eyes that reference the view you created. Then you'll need to assign them to the Talker in
the Talker's *init* method.

.. TIP::
    You can right-click in the script editor and choose *Insert Object -> Talker*

Views for a Talker
==================

When creating a view for a Talker, it will generally consist of three loops:

1. The bust (the overall Talker image, possibly including a frame).
2. The mouth, which consists of a series of mouth shapes that are overlaid on the bust. This is especially important if your Talker uses speech and lip-syncing.
3. The eyes, which consists of different eye movements overlaid on the bust.

.. figure:: /images/EcoBoy.png

    An example of a view used for a Talker in EcoQuest.

Your Talker instance's *init* method is used to assign the mouth, bust and eye props. You'll need to position these three Props manually in order to align them with each other.
You can do this either in
the Talker's init method, or using the default property values in your mouth/bust/eye Props.