.. Anatomy of SCI game

.. include:: /includes/standard.rst

========================
 Anatomy of an SCI game
========================

SCI games consist of the following things:

    - An interpreter
    - A package of game resources, including scripts which compile to byte-code and are run by the interpreter. This is where all the game-specific stuff is.
    - A configuration file
    - Drivers 

The interpreter
================

The Sierra interpreters included in the template games are named **sciv.exe** (for SCI0) and **sierra.exe** (for SCI1.1).
They are both 16-bit DOS programs, so they won't run natively on a modern 64-bit Windows operating system. In this case,
there are two primary options: DOSBox_ and ScummVM_.

DOSBox
-------

DOSBox_ is a DOS emulator that can be used to run SCI games. A version of it comes with SCI Companion, and by default the
template game is set up to use this. You can also point your game to a version of DOSBox that you have already installed.

ScummVM
--------

From the ScummVM_ website:

    ScummVM is a program which allows you to run certain classic graphical point-and-click adventure games, provided you already have their data files. 
    ScummVM just replaces the executables shipped with the games, allowing you to play them on systems for which they were never designed!

When you run the game using ScummVM, Sierra's interpreters aren't even used. ScummVM also has additional features, like a built-in debugger,
the ability to render the dithered palettes from SCI0 games with better fidelity, and the ability to play multiple digital audio files at once.

The game resources
===================

This is where the actual game code, graphics, and sounds are. These files generally consist of a **.map** file that serves as an index, and then
a package file where the actual resources are. Older games that came on multiple low-capacity floppy disks generally have a package file for each
disk, but this isn't needed anymore.

The file structure varies quite a bit over the various versions of SCI, but here is a table of what they are in the template games:

Resource files in the SCI0 template game:

================ =================
         SCI0 template game
----------------------------------
Map file         Package file
================ =================
resource.map     resource.001
================ =================

Resource files in the SCI1.1 template game:

============= ================ ======================
            SCI1.1 template game
-----------------------------------------------------
Map file      Package file     Comments
============= ================ ======================
resource.map  resource.000     Non-audio resources
------------- ---------------- ----------------------
    _         resource.sfx     Digital audio sfx     
------------- ---------------- ----------------------
    _         resource.aud     Digital audio speech
============= ================ ======================


Audio package files have their own map (index) associated with them. In the template game, this is
actually included as a resource in resource.000, but some Sierra games split this out as a separate 65535.map file.

Some Sierra games split out the message resources too, so you'll see message.map (the message resource index)
and resource.msg (the actual message resources). In the SCI1.1 template game, message resource index is included
in resource.map.


Configuration File
===================

In the game's folder is a **resource.cfg** file. This specifies which driver files to use for video, MIDI sound, digital audio,
keyboard, and the mouse (these are used if you're running natively in DOS, or in DOSBox_, but would be ignored if you're running in ScummVM_). Most users won't need to change anything in this
file, but more advanced users might want to change the sound drivers, for instance, if they are using MIDI sound resources that only sound correct with
a certain sound driver.

Drivers
=======

You notice a plethora of **.DRV** files in the game folder. These comprise the DOS drivers used by Sierra's interpreter.
If you want, you can remove the ones that aren't listed in resource.cfg.

Other files
===========

You'll notice a few other stray files in the game folder. In the SCI1.1 template game, **INTERP.ERR** and **VERSION** are essential for the
game to work with Sierra's interpreter, so don't change or remove them.

**INSTALL.EXE** and the other files that begin with INSTALL comprise a DOS
app that lets you configure **resource.cfg** via a series of menus. These can be removed.