.. This describes message audio

.. include:: /includes/standard.rst

.. IMPORTANT::
    This section applies to SCI 1.1 only

================
 Message Speech
================

With the advent of CD-ROMs, Sierra's SCI games began including spoken text. |scicomp|'s SCI 1.1 template game
fully supports this, including the ability to automatically generate lip-sync data.

.. toctree::
   :maxdepth: 2

   recording
   lipsync
   audiofiles


Quick start
===========

Ideally, adding speech for a message entry is as simple as 1-2-3:

1. Select a message entry.
2. Press the record button and speak into your microphone with your best narrator/actor voice, then press stop when done.
3. If this is for a Talker, press the Quick Lip Sync button to automatically generate lip-sync data.

.. TIP::
    The F10, F11 and F12 keys can be used to trigger play, stop, and record functionality.

.. image:: /images/SpeechLipSyncSection.png

You can of course also import a .wav file to use for a message entry.

In reality, there are some important things to be aware of. It is suggested you read the sections on :doc:`audio recording <recording>` and :doc:`lip-syncing <lipsync>`.

Notes
=====

Messsage audio and lip-sync data are not included directly in Message resources. Instead, message audio and lip-sync data are separate resources known as audio36 and sync36, and are
associated with specific message entries based on the module(room)/noun/verb/condition/sequence tuple. This means that of any of those values change, the audio36/sync36 resources need to be
saved under a new name. |scicomp| handles all this automatically for you, but it is something to keep in mind.

The message editor does not support undo/redo functionality because of the necessity of keeping the message resource and its associated audio367/sync36 resources in sync.