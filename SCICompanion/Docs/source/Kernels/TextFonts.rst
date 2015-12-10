.. TextFonts

.. default - domain::js

.. include:: /includes/standard.rst

==================
TextFonts (Kernel)
==================


.. function:: TextFonts([fonts ...])

	.. IMPORTANT:: SCI1.1 only.

	Sets the fonts used to render message text.

	:param number fonts: One or more font resource numbers.

	Example::

		; By default, render messages in font 1605.
		; Render messages with the |f1| tag in font 900.
		; Render messages with the |f2| tag in font 901.
		(TextColors 1605 900 901)


