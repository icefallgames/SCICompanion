.. Audio files

.. include:: /includes/standard.rst

.. IMPORTANT::
    This section applies to SCI 1.1 only

=============================
Audio file management.
=============================

|scicomp| handles audio file management mostly transparently so you don't have to think about it. However, an explanation of
how these files are stored and updated may be useful.

Overview
========

Audio files take up a lot of disk space (in Sierra's CD-ROM games, they can typically take up 400-500MB of data). As a result, unlike other
resources, |scicomp| doesn't save them directly to the resource packages (resource.aud and/or resource.sfx) whenever you add
an audio resource.

Instead, they are placed in folders in the *audiocache* folder of your game. If you look there, you might see a folder structure like this:

::

    audiocache\
        0\
            @0000m00.001
            @0000k00.001
        15\
            @00f0b05.001
            @00f0d00.001
            @00f0c03.001
        65535\
            1.aud
            7.aud
        0.map
        15.map
        65535.map
        uptodate.bin

The numbered subfolders contain the audio36 and sync36 resources for that particular module (room number). 65535 is special, as this is
the main audio sound effects from the game (accessed by the Audio tab in |scicomp|). In all cases, these are the actual patch files
that would be used.

As you add, edit or remove audio resources, it is these files and folders that are updated. When you start the game from within |scicomp|,
we check to see which files/folders have been updated, and then repackage them into the resource.aud or resource.sfx which are used by the game.
For this reason, if you have been adding lots of audio there may be a slight delay the next time you run the game.

The first time you edit audio for a module (room number), if the audio file cache folder for that module does not exist, |scicomp| will
pull the necessary resources out of resource.aud (or resource.sfx) and place them into the audio file cache folder.

These folders are also where the audio negative .wav files are stored for recordings you have made from within |scicomp|.

Managing the audio cache
=========================

You can force the audio files to be re-packaged into resource.aud or resource.sfx by using *Tools->Repackage Audio*. You'll need to do this
if you're running the game externally (i.e. not from within |scicomp|) and you want to be using your latest audio changes.

From *Tools->Audio Preferences*, you can also force a repackage of all audio files (instead of just those that |scicomp| thinks are out of date).

You can also clear out the audio file cache completely. This means you will *lose* any new audio files that have not been repackaged. You
will also lose the audio negatives for any recordings you have made in |scicomp|.


