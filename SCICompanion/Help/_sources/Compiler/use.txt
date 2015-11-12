.. Use

.. include:: /includes/standard.rst

================
 Use
================

The use keyword tells the compiler that the script being compiled "uses" the specified script's classes, procedures, and/or variables (if script.000).

When a script is compiled, an object file is created. Object files contain information about the script, such as public procedure name and public instance names.

If other scripts need to use the classes, procedures, and/or variables of it, they use the "use" keyword, and the object file is read in to validate against the code the user has entered.

For example, if you compiled a file myscript.sc, myscript.sco (the ojbect file) would be created.
You could then use that script in other scripts. Below is an example::

	(use "myscript")

