.. Debugging

.. include:: /includes/standard.rst

==========
 Debugger
==========

It's important to know how to properly debug your game in order to quickly find the causes of bugs. Debugging support with SCI is somewhat limited,
but |scicomp| does have some functionality to help you.

Debugging with |scicomp|
=========================

.. IMPORTANT::
    This is currently only supported with the SCI1.1 template game

|scicomp| provides some limited debugging functionality when used with the SCI1.1 template game. A DebugPrint procedure is defined
in main.sc that allows you to print debug messages to |scicomp| :doc:`output pane <outputpane>`. It supports basic printf formatting syntax (%d, %s).

::

    DebugPrint("Switching to room %d" newRoomNumber)

This debugging functionality requires that the game be able to write to arbitrary files in the same directory as the game
executable. For this reason, it is not compatible with ScummVM. It does, however, work when the game is run in DOSBox. It should be noted that ScummVM comes with
its own fully-featured in-game debugger.

The debugging functionality is enabled when the game is run with the Debug button |debug|, or the Debug Room button |debugroom|.

See :ref:`debugroom-label` for more information on starting the game in a specific room.


Debugging with Sierra's interpreter
===================================

.. IMPORTANT::
    This section only applies to SCI0

Most (or all) SCI0 games include debugging functionality right in Sierra's interpreter. It is triggered by pressing both **SHIFT** keys and the **numpad minus** key all at the same time.
Sierra's debugger does take some time to understand.

.. image:: /images/SierraDebugger.png

Unfortunately the internal debugger was removed in later SCI versions.


In-game debug functionality for SCI1.1
=======================================

.. IMPORTANT::
    This is currently only supported with the SCI1.1 template game

The SCI1.1 template game comes with some debug functionality written in script (in **debughandler.sc**). It is similar to debug functionality that appears in some of Sierra's SCI1.1 games,
with a few additional features.

It can be invoked by pressing ALT-d. Then, pressing question mark (?) will list all the various options.

.. image:: /images/TemplateGameDebugger1.png

As an example, ALT-v, ALT-p and ALT-c switch between visual, priority and control screens.

.. figure:: /images/TemplateGameDebugger2.png

    ALT-y highlights the obstacles in the current room

The debugger must be reactivated upon entering a new room.


ScummVM
========

If you run your game in the ScummVM_ interpreter, you're in luck! ScummVM provides fairly complete debugging functionality,
including the ability to set breakpoints and step through code (or rather, SCI byte code). You can trigger the ScummVM debugger by pressing CTRL-SHIFT-d while
playing the game.





.. |debugroom| image:: /images/DebugRoom.png
.. |debug| image:: /images/Debug.png
