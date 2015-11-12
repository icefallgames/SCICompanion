.. Game version

.. include:: /includes/standard.rst

==========================
 Game/Interpreter Versions
==========================

When you open a game, |scicomp| inspects the resources and tries to determine exactly which
version of SCI this is. SCI versions are not easy to categorize however, and there are a whole
bunch of different capabilities each game might have.

The Version Detector dialog (*Game->Version detection*) lets you see what |scicomp| detected, and possibly make changes to try
to reload the game if something isn't working correctly.

These are some of the things detected:

Resource map format
    This is the structure of the main resource map file, which tells the game where the resources are.

Resource package format
    This is the structure of the package files that contain the resources.

Compression format
    Sierra's game resources are usually compressed. Some games use different compression algorithms.

Sound format
    The sound resource format changed between SCI0 and SCI1.

Messages
    Is a message map present? Are there audio files to go along with those messages? Are the messages in message.map or resource.map?

View format
    There are several view formats.

Pic format
    And just as many pic formats.

Audio volume
    This may be resource.aud or resource.sfx, or both.

Audio map
    This has different formats in different games, and can either be resource 65535 (most commonly) or resource 0.

Script format
    The bytecode used in the script resources changed slightly over the different versions. Starting in SCI 1.1, script resources were split into script and heap resources.



You can make changes to the settings and try to re-open the game with these settings by clicking **Reload game**.
Please report a bug if you find a game that |scicomp|  doesn't recognize correctly.