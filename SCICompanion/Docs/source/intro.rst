.. This describes how to get started with SCICompanion

================
 Getting Started
================

Opening an existing game
========================

To open an existing game, choose *File->Open* and open the game's **resource.map** file. The game should then open in
**SCI**\ Companion and you should see the different resources in the game. SCI
games come in many different versions with different features. **SCI**\ Companion should be able to automatically
detect the various features of the game.

Broadly, there are these versions of the SCI interpreter:

- **SCI0** was used for games like King's Quest 4 and Space Quest 3. It supports 320x200 EGA graphics, MIDI sound/music, and the player enters text commands to control the game.
- SCI1 was used for games such as King's Quest 5 and Space Quest 4. It supports 320x200 VGA graphics and a point-and-click interface, as well as digital sound effects.
- **SCI1.1** was used for games such as Leisure Suit Larry 6, King's Quest 6 and Space Quest 5. It has some improvements over SCI1, including support for actor speech.
- SCI2 was used for games such as Gabriel Knight and King's Quest 7. It is a 32-bit engine that supports 640x480. **SCI**\ Companion does not support this version of SCI.

The versions marked in bold above are those for which **SCI**\ Companion supports creating a new game.

In reality, the differences between SCI0, SCI1 and SCI1.1 are a bit muddied - some games are a mix. The differences are mostly with the technical resource specifications though.

Resource management
========================

The first thing you see when you open a game will be the *Explorer* tab. It will show a series of buttons corresponding
to the different types of resources in the game. Click on them to see those resources. From each resource tab, you can
select individual resources and open them by double-clicking. Previews are available for some resources.

Via the right-click menu, you can also delete, export or give resources names.

New resources can be created using the buttons on the main toolbar, or in the *File->New* menu. You can also import resources by dragging them into the resource view, or using the *File->Import Resource* menu item.

SCI games have the following types of resources:

- *Views* - these are raster graphics that correspond to the actors, props and other animated (or non-animated) objects in the game.
- *Pics* - these are the backgrounds. Generally, EGA (16-color) games use vector graphics, and VGA (256-color) games use raster graphics.
- *Scripts* - these contain the logic for the game. The actual script data is compiled and non-human-readable, but **SCI**\ Companion has a fully-featured script compiler. It also has a decompiler that can attempt to recover (as much as is possible) the original scripts of a game. Opening a script resource will open the source code file if it exists.
- *Texts* - these contain the in-game text. Text can also be part of scripts, but having it as a separate resource helps with translating a game into other languages, and also saves a bit of in-game memory.
- :doc:`Messages <messages>` - in SCI1.1, message resources largely replaced text resources. These are basically the things the narrator or in-game characters say (or anything else in the game's UI). They integrate with the Narrator/Talker system in the game and can optionally be associated with spoken text.
- *Sounds* - these are MIDI-based music and sound effects.
- *Vocabs* - these are used for various purposes. The main one you will be concered with is vocab.000 in SCI0 games. This comprises the words that the in-game text parser understands.
- *Fonts* - bitmapped fonts used to display text in the game.
- *Cursors* - mouse cursor resources. These are only used for SCI0 (2-color) and SCI1 (3-color). In SCI1.1, views are used for mouse cursors.
- *Patches* - these are needed for the various MIDI drivers. **SCI**\ Companion does not support editing them (nor should you need to).
- *Palettes* - for VGA games (SCI1 and above), these are the color palettes.
- *Audio* - in SCI1.1, these are digital sound effects (SCI1 supports digital audio also, but it is embedded in the sound resources, and **SCI**\ Companion does not support editing those). Speech is also digital audio, but it is accessed through the message resources.
- *Heaps* - in SCI1.1, scripts were split into logic and data. Heap resources contain the data. **SCI**\ Companion takes care of creating these automatically when you compile.

There are a few other types, but these are not shown in **SCI**\ Companion's user interface. In particular, map, audio36 (digital audio) and sync36 (lip syncing) resources exist in SCI1.1. These are tied to messages though, and are managed through the message editor.



Running the game
================

To run the game, Press the green or red play button in the main toolbar, use the Game menu, or press F5. This will run the executable that is specified in the *Game->Game properties* dialog.
By default, the template game sets this up to run DOSBox_ with the Sierra executable as a target. The versions of SCI used in the template games are 16-bit executables,
which means they won't run on modern 64-bit operating systems. DOSBox_ can be used to run them in a DOS emulator, or you can run them inside ScummVM_.

.. _DOSBox: http://www.dosbox.com/
.. _ScummVM: http://scummvm.org/

Creating a new game
========================

To create a new game, choose *File->Create new game*. From there, you can choose one of two templates:

- *SCI0*: This will start you with a classic 16-color EGA game with a text parser interface. EGA views can be easier to draw than VGA views, but SCI0 pic backgrounds are vector graphics. Drawing good backgrounds can take some practice.
- *SCI1.1*: This will start you with a 256-color VGA game with a point-and-click interface. In-game text is provided by message resources, backgrounds are bitmapped instead of vector, and digital audio and speech are supported.

.. TIP::
    Though the scripting is very similar, the two template games are not compatible. They use different resource formats, so porting from one to the other would be tedious.

Once you create the game, you are ready to run it, compile it, or start adding content!