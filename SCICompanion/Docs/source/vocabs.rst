.. Vocabs

.. include:: /includes/standard.rst

.. IMPORTANT::
    This section applies to SCI0 only

================
 Vocabs
================

SCI0 uses vocab resources for a few different purposes. The important one (that |scicomp| lets you edit) is the one that contains the words the SCI parser understands.

The words are listed in the view, along with their Group (not significant for editing) and Class (the type of word it is). Synonyms are grouped together on one line.

Adding a word
=============

To add a word to the game's vocabulary, click the *New word* button |newword|, or right click in the view and choose *New word*. Type in your new word and hit enter.

Deleting a word
===============

Click on the word and press DELETE.

Looking up a word
=================

There are two ways to find a word in the vocabulary editor. You can use the *Find* button |wordfind|, or simply just type the letters of the word into the view. It will automatically scroll into view and be selected.


Editing words
=================================

Find the word you want to edit, and right-click *Rename* or press *F2*.

Adding synonyms
===============

To add a word as a synonym of another word, just add it to the end of the first word, separated by a space or a '|' character.

.. image:: /images/Vocab_Example.PNG

Specifying the word class
=========================

The class of a word (e.g. verb, noun, etc...) affects how the parser will interpret it. Consult the SCI tutorials to learn more.

Right-click on the word and check or uncheck one of the nine classes. Mulitple classes can be applied to a word.

.. image:: /images/Vocab_TypeExample.PNG


.. |newword| image:: /images/NewWord.png
.. |wordfind| image:: /images/WordFind.png

.. TIP::
    Words can also be added directly to the vocab resource by using the right-click menu in the script editor.