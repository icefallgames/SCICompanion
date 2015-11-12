.. Format

.. default - domain::js

.. include:: /includes/standard.rst

===============
Format (Kernel)
===============

.. function:: Format(destString formatString)

.. function:: Format(destString resourceNumber textId)
	:noindex:

	
	Formats the fmtString with the given parameters and stores it in destString. It then returns a pointer to the destString.
	
	:param string destString: The buffer to which the string will be written.
	:param string formatString: The formatting string.
	:param number resourceNumber: A text resource number.
	:param number textId: An index in the text resource to use for the formatting string.
	
	There are a number of different formatting options available::
	
		// The following are for numbers:
		Format(aString "Number: %d" 5) // Would print "Number: 5"
		Format(aString "Number: %03d" 5) // Would print "Number: 005"
		Format(aString "Number: %3d" 5) // Would print "Number:   5"
		Format(aString "Number: %-3d" 5) // Would print "Number: 5  "
		// The following are for unsigned numbers:
		Format(aString "Number: %u" 5) // Would print "Number: 5"
		Format(aString "Number: %03u" 5) // Would print "Number: 005"
		Format(aString "Number: %3u" 5) // Would print "Number:   5"
		Format(aString "Number: %-3u" 5) // Would print "Number: 5  "
		// The following are for hex:
		Format(aString "Number: %x" $A5) // Would print "Number: A5"
		Format(aString "Number: %03x" $A5) // Would print "Number: 0A5"
		Format(aString "Number: %3x" $A5) // Would print "Number:  A5"
		Format(aString "Number: %-3x" $A5) // Would print "Number: A5 "
		// The following are for strings:
		Format(aString "String: %s" "Hello") // Would print "String: hello"
		Format(aString "String: %10s" "Hello") // Would print "String:      Hello"
		Format(aString "String: %-10s" "Hello") // Would print "String: Hello    "


