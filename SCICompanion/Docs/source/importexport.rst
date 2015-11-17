.. Importing and exporting resources

.. include:: /includes/standard.rst

================================================
 Importing and exporting resources
================================================

In addition to packaging the resources into the large resource volumes, Sierra games could also
load individual files into the game. These were known as patch files, since they were usually used to
correct bugs in the game post-release.

They are a useful transfer mechanism between games though. You can export a resource from one game as a patch file,
and then import it into another. The following table lists what the various patch filenames look like:

==================  ==========================  ===========================
Resource type       SCI0 patch filename format  SCI1+ patch filename format
==================  ==========================  ===========================
View                view.123                    1234.v56
Pic                 pic.123                     1234.p56
Script              script.123                  1234.scr
Text                text.123                    1234.tex
Sound               sound.123                   1234.snd
Vocab               vocab.123                   1234.voc
Font                font.123                    1234.fon
Cursor              cursor.123                  1234.cur
Patch               patch.123                   1234.pat
Palette             \-                          1234.pal
CD Audio            \-                          1234.cda
Audio               \-                          1234.aud
Sync                \-                          1234.syn
Message             \-                          1234.msg
Map                 \-                          1234.map
Heap                \-                          1234.hep
==================  ==========================  ===========================


Exporting and importing resources from different non-SCI file formats (e.g. importing a windows font, a jpg image, MIDI or .wav file) is covered under the 
documentation for the editors of that particular type. This section is concerned simply with patch files.

Exporting
=============

You can click on an item in the game explorer and choose *File->Export as patch file*. Or right-click on it and choose *Export as patch file*.
A file dialog will then appear so you can tell it where to save.

You can also export a resource directly from that resource's editor using *File->Export as patch file*.

Importing
=============

Importing can be done simply by dragging a patch file into the Game Explorer, by right-clicking on the background and choosing *Import patch file*, or
by using *File->Import patch file*.

You can also open a patch file directly using *File->Open patch file* and then save it under some resource number.




