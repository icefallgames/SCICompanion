.. TextColors

.. default - domain::js

.. include:: /includes/standard.rst

===================
TextColors (Kernel)
===================


.. function:: TextColors([colors ...])

	.. IMPORTANT:: SCI1.1 only.

	Sets the palette color indices used to render message text.

	:param number colors: One or more color indices.

	Example::

		; By default, render messages as color 0.
		; Render messages with the |c1| tag as color 15.
		; Render messages with the |c2| tag as color 52.
		; Render messages with the |c3| tag as color 53.
		(TextColors 0 15 52 53)



