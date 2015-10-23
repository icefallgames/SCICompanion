.. Palette cycling

.. include:: /includes/standard.rst

.. IMPORTANT::
    This section applies to VGA games only

================
 Palette cycling
================

Palette cycling is an animation effect that was popular during the days of palettized graphics. It involves
rotating the palette colors to give the appearance of movement an otherwise static background. It can be
seen most obviously in SCI games in the title screen with the Sierra logo, but also shows up in other
places.

.. figure:: /images/Waterfall.gif

    A palette cycling effect on a room with a waterfall in Quest for Glory.

Some especially impressive examples of what palette cycling can do can be found on this web page: http://www.effectgames.com/effect/article-Old_School_Color_Cycling_with_HTML5.html .
Those examples use a lot of dithering, however, which looks better in a 640x480 canvas than the relatively low 320x200 resolution of SCI1 games.

|scicomp| has some features available to streamline the workflow of generating backgrounds with shifting colors. This section will show how to
generate a simple animation on a logo.

Example
================================

Palette cycling is all about having smooth gradients of palette color *indices* across a region of your background. This can be accomplished by creating a source image
with a color gradient, and then mapping it to an existing palette in |scicomp| that has that same color gradient across a range of palette indices.

Here's the |scicomp| logo with a red to blue color gradient:

.. image:: /images/LogoGradient.png

Next, create a new blank pic. Then, open the :doc:`Palette Editor <palettes>` for this pic, and select the non-fixed colors (64-254). Open the gradient
dialog and create an A-B gradient that goes from red to blue. Mark those colors as "used", then close the Palette Editor.

.. image:: /images/MakeRedBlueGrad.jpg

The next step is to import the background image. Open the Import Background dialog and browse for the logo image. The key part here is that we'll
map the colors in the logi image to the currently palette for the background. Select the 64-254 color range again, and also black (0). Then click
on **Map to current palette**. Click **Accept*.

.. image:: /images/ImportLogo.jpg

You should end up with something like this:

.. image:: /images/ImportedLogo.jpg

Now we have an image with color indices that go from 64 to 254 gradually across the image - perfect for palette cycling. We can then
assign the colors we want.

