.. Palette

.. default - domain::js

.. include:: /includes/standard.rst

================
Palette (Kernel)
================


	Palette lets you perform palette cycling animations, set the intensity of color
	ranges, and more. The various subfunctions will be described separately.

	.. IMPORTANT:: SCI1.1 only.

.. function:: Palette(palSET_INTENSITY startIndex endIndex intensity [setPalette])

	Sets the intensity of the range of colors.

	:param number startIndex: The inclusive starting index.
	:param number endIndex: The exclusive ending index.
	:param number intensity: Value from 0 to 100 representing the intensity.
	:param boolean setPalette: If TRUE, actually changes the palette value.

.. function:: Palette(palANIMATE startIndex endIndex speed)
	:noindex:

	"Animates" the palette colors in a particular direction. This allows for palette cycling effects.

	:param number startIndex: The inclusive starting index.
	:param number endIndex: The exclusive ending index.
	:param number speed: A speed (lower numbers are faster) and direction (negative or positive).

	A single call to Palette(palANIMATE ...) will only shift the palette by one. It needs to be called
	repeatedly in a Script's doit() method to keep cycling the colors.

	Example::

		(method (doit)
			(super:doit())
			Palette(palANIMATE 192 198 -9)
		)

.. function:: Palette(palSET_FROM_RESOURCE resourceNumber [force])
	:noindex:

	Sets the current palette from a palette resource.

	:param number resourceNumber: The palette resource number.
	:param nunber force: If force is 2, the palette set is forced.

	Example::

		Palette(palSET_FROM_RESOURCE 999 2)

.. function:: Palette(palSET_FLAG startIndex endIndex flag)
	:noindex:

.. function:: Palette(palUNSET_FLAG startIndex endIndex flag)
	:noindex:

	Sets or unsets a flag for a palette index.

	:param number startIndex: The inclusive starting index.
	:param number endIndex: The exclusive ending index.
	:param number flag: A flag. 1 indicates this palette color is *used*. 0 indicates it is not.

.. function:: Palette(palFIND_COLOR red green blue)
	:noindex:

	Finds the closest palette index to a particular color.

	:param number red: The red component (0-255)
	:param number green: The green component (0-255)
	:param number blue: The blue component (0-255)
	:returns: The index of the color most closely matching the given RGB color.

	Example::

		= gLowlightColor Palette(palFIND_COLOR 159 159 159)

.. function:: Palette(palSAVE)
	:noindex:

	Saves the current palette to be restored later.

	:returns: A cookie to use for restoring.

.. function:: Palette(palRESTORE cookie)
	:noindex:

	Restores a palette.

	:param number cookie: A cookie returned from Palette(palSAVE).



