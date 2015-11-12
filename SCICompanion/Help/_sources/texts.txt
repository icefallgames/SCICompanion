.. Texts editor

.. include:: /includes/standard.rst

================
 Text Editor
================

.. image:: /images/TextEditor.jpg

The text editor shows a list of all the text resources. It's operation is very basic. Text resources are accessed in code by their
module (room number) and index. They are mainly interesting in SCI0, as most in-game text in SCI1.1 has been replaced by :doc:`Messages <messages>`.

To print text entry number 10 in text resource 1 in the game, you can use the following code

::

    Print(1 10)

Texts offer a few advantages to including text strings directly in source code:

- using them will reduce the size of your compiled scripts. Heap space (where the scripts live) is at a premium in SCI0.
- it would make it easier for someone to translate your game, as they can just modify the text resources and not have to touch the scripts.



Adding a new entry
==================

To add a new piece of text, click on the *Click to add new text* message at the bottom of the list, and type in the text.

Deleting an entry
==================

To delete text, click on the number to the left of the entry, and press DELETE.

Editing an entry
================

To edit text, just click on it and type

Moving entries around
=====================

To move an entry with respect to the others, click on the number to the left of the text, and then click on the *Move up* or *Move down* buttons.

Tabs and carriage returns
===========================

Use \\t and \\n to enter tabs and carriage returns into the text

