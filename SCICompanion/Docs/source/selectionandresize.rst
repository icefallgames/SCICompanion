.. Selection and resize

.. include:: /includes/standard.rst

========================
 Selection and Resizing
========================

Selection
=========

.. figure:: /images/Selection_Example.png

    Manipulating selection in a cel

To select an area of the canvas, choose the selection tool |select|
and left-click and drag on the area you wish to select.
You can copy *(CTRL-C)* or cut *(CTRL-X)* the selection to the clipboard. You can also drag the selected area around to move it.

The entire canvas may be selected with *(CTRL-A)*.

Paste
=====

If you paste *(CTRL-V)* an image from the clipboard into the view,
it will appear in the top left of the current cel, in a rubber band selection rectangle. You can position it as you desire. Double-clicking on the selection will set it permanently.
If the image is a 24-bit image from another application, |scicomp| will attempt to map the colors to the current palette (more control over this can be obtained by using *View->Import image sequence*).

Transparent Paste
=================

Transparent pasting *(CTRL-SHIFT-V)* will paste an image, but not include the pixels that are the same colour as the transparent colour of the cel.


Resizing
========

.. figure:: /images/Resize_Example.png

    Resizing a cel

There are resize handles surrounding the bottom right edges of the canvas. You can drag them with the mouse to resize the image. Using different key combinations while dragging will affect how the image is resized.

    ====== =====================================
    Keys   function
    ====== =====================================
    None   anchor the image on the top left
    ------ -------------------------------------
    CTRL   anchor the image on the bottom right
    ------ -------------------------------------
    SHIFT  stretch or shrink the image
    ====== =====================================


.. |select| image:: /images/DrawingSelect.png
