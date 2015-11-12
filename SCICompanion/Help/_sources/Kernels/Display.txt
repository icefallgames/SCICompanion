.. Display

.. default - domain::js

.. include:: /includes/standard.rst

================
Display (Kernel)
================


.. function:: Display(text [params...])

	Displays the specified text to the screen.
	
	Following the text, you can specify how the string is displayed or handled with parameters.
	Each parameter has a type ID, specified in sci.sh.
	
	Allowed parameters:

	dsCOORD
		2 parameters: (x, y) coordinates at which to display the text.

	dsALIGN
		1 parameter: alRIGHT, alLEFT or alCENTER.

	dsCOLOUR
		1 parameter: set the colour.

	dsBACKGROUND
		1 parameter: set the background colour. -1 will use a transparent background.

	dsDISABLED
		1 parameter: set the disabled (grey) flag. 1 will draw disabled text.

	dsFONT
		1 parameter: the resource number of the font to use.

	dsWIDTH
		1 parameter: the width to wrap the text.

	dsSAVEPIXELS
		no parameters: set the "save under" flag to save a copy of the pixels before writing the text. The handle to the saved pixels is returned.

	dsRESTOREPIXELS
		1 parameter: handle to the saved pixels. With this command, the text and all the other parameters are ignored.

	Examples::

		Display("Hello World");

		Display(
			"Hello World"
			dsCOORD 60 80
			dsCOLOUR clRED
			dsBACKGROUND clYELLOW
			dsFONT 4
			dsALIGN alCENTER
			dsWIDTH 200
		); 



