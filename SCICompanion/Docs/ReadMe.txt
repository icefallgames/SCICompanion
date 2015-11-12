This folder contains the "source code" of the documentation, and it meant to be built with sphinx.
http://sphinx-doc.org

The following command executed from a cmd prompt in this directory:
make html

...should run the python script that generates the html documentation.

The result (in the build folder) should then be copied into the Help folder, so that it is copied to the same location when SCICompanion.exe is built.




Automatically generating the SCI1.1 class hierarchy and kernels rst files from code comments:

- Open SCI Companion, create a new template game, and press CTRL-SHIFT-D to bring up the documentation dialog.
- Set the output directory to be the Docs\source subfolder of the SCI Companion project
- Select all files on the left, and click Generate. The scripts/classes/procedures rst files will be created and compiled.
- click the Kernels button. The kernels rest files will be created and compiled.

