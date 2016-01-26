This folder contains the "source code" of the documentation, and it meant to be built with sphinx.
http://sphinx-doc.org

Make sure to install the blockdiag and sphinxcontrib-blockdiag extensions:

pip install blockdialog
pip install sphinxcontrib-blockdiag

The following command executed from a cmd prompt in this directory:
make html

...should run the python script that generates the html documentation.

When SCICompanion.exe is built, the built help files will then be copied to the Help folder next to SCICompanion.exe.




Automatically generating the SCI1.1 class hierarchy and kernels rst files from code comments:

- Open a debug version of SCI Companion, create a new template game (use the SCI syntax, as the Sierra syntax doesn't quite yet support proper comment parsing)
- press CTRL-SHIFT-D to bring up the documentation dialog.
- Set the output directory to be the Docs subfolder of the SCI Companion project, with no trailing backslash
- Select all files on the left, and click Generate. The scripts/classes/procedures rst files will be created and compiled.
- (The Kernels button should no longer be used)

To binplace to generated help files, rebuild SCI Companion after this.

