.. Lip-syncing

.. include:: /includes/standard.rst

.. IMPORTANT::
    This section applies to SCI 1.1 only

===========
Lip-syncing
===========

Overview
========

To generate lip-sync data, you need three things:

1. A message entry with spoke text audio associated with it
2. A view/loop for that message entry's talker, which will correspond to its mouth
3. A mapping of phonemes to cels within the view/loop from (2).

Step 3 is the most time-consuming to set up, but it only needs to be done once for a mouth view/loop.
Once steps 2 and 3 are done, most lip-syncing can be accomplished just by select a message entry and
clicking the "Quick Lip Sync" button.

Lip Sync resources
------------------

Sierra's lip-sync resources (known as sync36) are fairly straightforward. They just consist of sequence of pairs of
values. The first value in each pair is the tick count (60 ticks per sound), and the second value in each pair
is the cel number in the mouth view/loop that should be used at that time.

Lip Sync dialog
---------------

Open the Lip Sync dialog by selecting a message entry with audio and clicking on the "Advanced" button.
You should see something like this:

.. image:: /images/LipSyncDialog.png


Associating a talker number with a view/loop.
--------------------------------------------

In the Lip Sync dialog, you can use the *Set View* button to associate the given talker number with
a view and loop. Once that is done, the mouth loop will appear in the dialog and in the message editor
when a message entry with that talker number is selected. This only needs to be done once per talker.

This information is stored inside *talker_to_view.ini* in your game's *lipsync* directory, and is only used by |scicomp|.

Phoneme mapping
===============

For a given view loop, we need to create a mapping of phonemes to the cels corresponding to the mouth
shapes for that phoneme. To get an idea of which mouth shapes correspond to which phonemes, you can try
an internet search for "lip sync mouth shapes".

From the Lip Sync dialog, click the *Edit* button to edit the phoneme-to-cel mapping. By default, all the phonemes
will be associated with cel 0. You can drag the phonemes alongside the different cels to complete the mapping.

.. image:: /images/PhonemeEditor.png

Once you're done, click *Ok* to accept the changes.

This information will be stored in a file in the *lipsync* folder of your game. It's only used by |scicomp|, not the actual game.
If you prefer to edit the phonemes manually, you can open and reload the phoneme file directly from the Lip Sync dialog.

Generating the Lip Sync Data
============================

Once you have a view/loop and phoeneme mapping set up for a talker, you can generate Lip Sync data directly from the message
editor (*Quick Lip Sync*). You can do it from the Lip Sync dialog, which has more options. The dialog will draw the phonemes
directly on the audio waveform, and show you the words that the Lip Sync extraction functionality guessed.

You can also save the lip sync data to a file so you can edit it manually if necessary.

The more clearly you speak, the better the data generated will be. You can also choose to provide the message text to the
lip sync data generator (the *Use Text* checkbox). Sometimes this will produce a better result. However, it may also make things worse and try to align
the words to the wrong spoken words, generating data that is not aligned with the speech. Try both to see which is better for
a particular recording.

When you are generating lip-sync data from speech you recorded in |scicomp|, the audio negative will be used. The same audio processing
will be run on it, but the lip sync engine is provided with a 16-bit version of the result (even if you specified the final audio resource to be 8-bit). This
generates significantly better results.

.. NOTE::
    Currently |scicomp| only supports US English phoneme detection. As lip-syncing does not need to be 100% accurate, this may suffice
    for other languages and accents too. If there is enough demand however, this could be made extensible to other languages.

Using the sample mouth
======================

If you just want to play around with lip-syncing without the overhead of drawing a mouth view and setting a phoneme mapping for it,
you can use the sample mouth (and its associated phoneme mapping) that comes with |scicomp|. In the Lip Sync dialog, you can
check *Use Sample* to do this.