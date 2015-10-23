.. Views

.. include:: /includes/standard.rst

================
 View Editor
================

Views are used for all the graphical elements in the game: Actors, Props, icons, window borders and more.
Because they are so important, there is a significant amount of functionality devoted to them in |scicomp|.

A View consists of one or more *loops*, each with one or more *cels*.

A loop is commonly used for an animation sequence, such as the following walking animation:

.. image:: /images/ViewLoop.png

.. image:: /images/ViewLoopAnimated.gif

In the case of a character, different loops within the same view are often used for different directions the character is facing.

The View Editor layout
======================

.. image:: /images/ViewEditor.png

In the center is the currently selected cel on which you draw.

On the bottom are the cels that are in the currently selected loop.
To the right is a list of loops that are in the view. Clicking on any of these will set the currently selected loop or cel (you can also use the arrow keys to do this).
You can rearrange loops and cels within the view by clicking and dragging them to a new position.

The Toolbox pane shows the palette and other controls used to manage the view.
Finally, on the top are the drawing controls common to views, fonts and cursors.


Drawing on the main view pane
-----------------------------

Please read the section on :doc:`drawing tools <rastertools>` for a full description of what each tool does.

Selection and resizing
----------------------

Please read the section on :doc:`selection and resizing <selectionandresize>`.

Palettes
--------

The current foreground and background drawing colors are shown in the Toolbox pane. Left-clicking or Right-clicking on a color in
the palette will set the foreground or background colors respectively. The currently selected colors are highlighted in the palette,
along with a 'T' marking this cel's transparent color.


.. figure:: /images/ViewPalettes.png

    A VGA palette on the left, and the EGA palette on the right.


EGA views use a fixed 16-color palette.

VGA views use a custom 256-color palette. When you create a new VGA view, there is no
palette associated with it by default - it will just use palette 999 (the global palette). If you want a view to have a specific
palette associated with it, you can do so by clicking on the edit palette button |palettebutton|. You can remove an embedded palette
using *View->Remove Embedded Palette*.

Views most commonly just use the "fixed" colors of a palette. In the SCI1.1 template game, these are the lower 64 colors of palette 999.
This lets several views be on screen at once without any palette conflicts.

By default, the View Editor draws a view with its embedded palette or palette 999. You can use any palette in the game to render the view by
choosing one from the dropdown list in the Toolbox pane.

If you want a view to use a different palette, but attempt to keep its current colors as much as possible, you can use *View->Remap Palette*.

Color shifting
--------------

Often you'll want to make differently colored versions of a view (for example, to change the clothing colors of an character) using the same palette.
|scicomp| makes this easy by allowing you to shift colors in the current cel, loop or entire view.

*View->Shift Colors* will bring up the Color Shifter dialog. Dots are shown on the palette indices that are used by the current cel, loop or view.

You can select a range of colors in the palette and move them around as a group with the arrow keys.

.. figure:: /images/ColorShift.png

    Selecting the reddish pixels and "moving" them so they are in the yellow part of the palette.


Adding and removing cels and loops
----------------------------------

The Toolbox pane has buttons for adding and removing cels and loops to/from the current view. A newly-created cel will be a copy of the
currently selected cel (you can change this so that it is filled with the transparent color by default in *Tools->Preferences*). You can also
delete the current cel by pressing the DELETE key.

If you remove all cels in a loop, that loop will be deleted. You can not remove all loops - a view must have at least one loop with one cel.

Affect all cels
---------------

At the bottom of the Toolbox pane is a checkbox for *Affect all cels in loop*. This lets you manipulate draw on all cels in the current loop
at once (even though only the current cel is shown in the canvas). This makes it easy to flood fill an area common to all cels, resize
all cels at once, or perform a mass replace of one color with another.

To map the current pixel location to the pixel location in another cel, |scicomp| uses the offset (or placement) values for the cel. These form
the origin.

Offset
------

By default, the offset of a cel is (0, 0). Since views are commonly used to draw in-game characters, and the location of a character is most
closely associated with his or her feet, an offset of (0, 0) corresponds to the bottom center of the cel. You can change a cel's manually in
the Toolbox pane, or using the placement |placement| drawing tool.

Mirrors
-------

Often the loops for different directions a character faces are just mirror images of one another. In the Toolbox pane you can use the *Mirror of* combo box to indicate that the
current loop is the mirror image of another. Any manipulation or editing of one loop will take immediate effect on the other loop.

Transparency
------------

Each cel can have one palette index marked as its transparent color. When it is drawn in-game, this will let the background show through. The transparent color
can be set with the transparent eyedropped tool |transparenteyedropper|, or by choosing a color explicitly in the Toolbox pane.

Scaleable views
---------------

.. IMPORTANT::
    This section applies to SCI 1.1 only

The Toolbox lets you specify whether a view is scaleable or not. A scaleable views can be reduced in size in-game to give the effect of a character
walking off into the distance.



.. |palettebutton| image:: /images/PaletteButton.png
.. |placement| image:: /images/DrawingPlacement.png
.. |transparenteyedropper| image:: /images/DrawingTransparentEyedropper.png


