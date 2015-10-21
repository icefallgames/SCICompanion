.. Cursors

.. include:: /includes/standard.rst

.. IMPORTANT::
    This section applies to SCI0 and SCI1 only

================
 Cursors
================

Cursor resources are used in SCI0 and SCI1 for the mouse cursor (in SCI1.1 and above, views are used for mouse cursors). In SCI0 they can have white, black and transparent pixels,
while in SCI1 they can have white, black, gray or transparent pixels.



.. figure:: /images/CursorExample.png

    A 2-color cursor from King's Quest 4, and 3-color cursor from King's Quest 5.

The cursor drawing tools are mostly similar to those used in views. Refer to the section on :doc:`drawing tools <rastertools>` for a full description of what each tool does.

Cursors have a fixed 16x16 size. Transparent pixels are represented by an aqua color in |scicomp|.

Hotspot
--------

Use the origin tool |placement| to set the cursor hotspot. (0, 0) and (8, 8) are the only two hotspots allowed in SCI0 games. In SCI1 games the hotspot can be anywhere.

.. |placement| image:: /images/DrawingPlacement.png