.. Sounds

.. include:: /includes/standard.rst

================
 Sound Editor
================

SCI Sound resources are MIDI-based music and sound effects. In SCI0, they are the only form of audio, while
SCI1.1 also supports digital :doc:`audio` resources.

|scicomp| doesn't provide a MIDI editor, but you can preview existing sound resources in the Sound Editor, and
import MIDI files you have created elsewhere.

.. image:: /images/SoundEditor.jpg


Anatomy of an SCI sound resource
================================

Sound resources aren't *just* MIDI data. They also contain a mapping of sound driver to MIDI tracks. 

Around the time SCI games were initially released, there were several different sound cards available. Each sound card had different
capabilities and sounds. Thus, for a particular sound, different MIDI tracks might be needed for each card.

If you open a sound resource in a Sierra game, you can choose different sound drivers in the Toolbox pane on the left, and see
different tracks become enabled and disabled.

The important thing is that the tracks you want are enabled for the device that matches the sound driver used in the resource.cfg for your game.
The most straightforward thing to do would be to import |midiimport| a MIDI file and enable all the tracks for every device, and then make sure you are using a general midi driver in your resource.cfg
(GENMIDI.DRV for the SCI1.1 template game, and GM.DRV for the SCI0 template game).

Tempo
=====

Note that the tempo (Speed) of the sound resource can only be adjusted when you first import a MIDI file. If you save this sound resource and re-open it,
the Speed adjustment will be disabled. The tick counts in the SCI sound resources are too coarse to have speed adjustments work properly.

Tracks
======

Clicking on a track will show the MIDI events for that track in the Toolbox pane. 

To enable or disable a track for the currently selected sound driver, use the checkbox in the track header.

Cue points
==========

From the Toolbox pane, you can add one or more cue points. When a sound is played in-game, this will cause the client of the Sound object to be cued (which, if
it is a Script object, will cause its changeState to be called). This lets you trigger in-game events at keys points while a sound is being played.

In the Sound Editor, you can adjust cue point times by entering a new tick value, or more easily by dragging the cue point |cuepoint| at the bottom of the view.

Cues also have values associated with them. The only really important thing is that these values increment.

Loop point
==========

A loop point |looppoint| can be specified to have the sound loop back to a predetermined point (instead of the beginning) when it ends.

This doesn't make the sound automatically loop when played in the game though. To loop a sound in script, set the Sound
object's loop value to -1. A sound can loop even if there is no loop point specified. Likewise, a sound with a loop point won't loop if the loop value of the Sound object
is anything other than -1. A loop point simply controls where the sound loops back to.

If a sound has a loop point specified, cue points won't work. Instead, the Sound object's client will be cued when the sound loops.


In Code
==========

You might reference a sound resource number when you tell the game to play some music::

    // Set the game's music to sound resource 101:
    (send gMusic1:
        number(101)
        loop(1)
        play()
        hold()
    )

Or to declare a sound effect in your room::

    (instance openDoorSound of Sound
        (properties
            number 103 // Resource number 103
            priority 15
            flags 1
        )
    )

    // Then elsewhere ...

    (openDoorSound:play())



See :class:`Sound` and :func:`DoSound`.

.. |midiimport| image:: /images/MIDIImport.png
.. |cuepoint| image:: /images/CuePoint.png
.. |looppoint| image:: /images/LoopPoint.png


