.. Importing images

.. include:: /includes/standard.rst

==============================
 Importing images in SCI views
==============================

Images and image sequences can be imported into both EGA and VGA views (and cursors). Image formats supported include jpg, png, tiff, bmp and gif (including multi-frame gif).

The Import Images dialog offers full control over how these images are converted to the restricted palette of SCI. In particular, this includes control over:

- Which palette indices to use
- Whether to map to the current palette or generate a new palette
- The type of color matching to use
- Optional dithering for limited color palettes or images with an alpha channel
- Modifications to brightness, contrast, saturation and hue of the input image

Import Images
=============

The Import Images dialog is available from the drawing toolbar, or *View->Import Image Sequence*. It first brings up a file dialog that lets you
choose one or more images. Once you've made your selection, the Image Images dialog is presented:

.. image:: /images/ImportImage.jpg

If you've selected multiple images, they will be arranged in alphabetical order. The original image(s) are displayed on the left, and the processed
image(s) on the right.

Palettes
--------

You have three options for palettes:

1) You can have |scicomp| generate a palette automatically, replacing the selected colors of the existing palette of the view
2) You can try to map colors to selected colors of the existing palette (using either RGB or CCIR color matching)
3) You can use the palette included in the image file, if available (for instance, .gif files may include a palette).

In either of these cases, you have control over which indices are affected in the existing palette for this view. For VGA views, by default the "fixed" colors
specified in palette 999 are selected, and option 2 above is the default. This is the most common use case, as most views generally only use the fixed colors in
palette 999.

If you want to customize which indices are used, you can check or uncheck the *Use global palette entries* checkbox to quickly control which palette indices are used, or you can manually
select them in the palette and press the refresh button.

Transparency
------------

By default, the current transparent color is used for transparent parts of the image, but you can change this. For images with 8-bit alpha, you can
set the alpha threshold used to determine if a pixel is opaque or transparent (by default this is set to 128).

Dithering
----------

Due to the limited SCI palettes (especially in EGA), you may want to enable dithering.

.. figure:: /images/ImportImageDitherCompare.png

    A comparison of an image imported into a view using the default VGA palette for the template game.

In addition to color dithering, you can also have the alpha channel dithered. If you choose to enable alpha dithering, you
have a number of options for the dither algorithm. An ordered dither is recommended for moving images, as the dithering matrix will then be stable from frame to frame.

.. figure:: /images/Boom.gif

    An image sequence from a particle effects software, rendered out to an image sequence and imported into a VGA view in |scicomp| using alpha dithering.

.. figure:: /images/Flame.gif

    A flame sequence imported into a 16-color EGA view using color dithering.

Image adjustments
------------------

Adjustments for brightness, contrast, saturation and hue are provided. If the imported image is a tough match for the current palette, adjusting
these parameters might help.