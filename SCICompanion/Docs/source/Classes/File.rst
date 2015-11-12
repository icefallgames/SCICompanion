.. File

.. default - domain::js

.. include:: /includes/standard.rst

======================
File (of :class:`Obj`)
======================

.. class:: File

	Defined in File.sc.

	
	This class encapsulates reading and writing to a file.
	
	Example usage::
	
		(var myFile)
	
		= myFile (File:new())
		(if ((send newFile:
			name("myfile.txt")
			open(fOPENFAIL)
			 ))
	
			 // Read stuff from file, etc...
		)
	
		// Close the file.
		(send newFile:
			close()
			dispose()
		)
	


Properties
==========

Defined in File:

======== ===========
Property Description
======== ===========
handle              
======== ===========


Methods
==========

.. function:: dispose()
	:noindex:



.. function:: showStr(param1)
	:noindex:



.. function:: open(openMode)
	:noindex:

	
	Opens the file in one of the following modes:
	
	fOPENCREATE
		Opens the file. If the file doesn't exist, it is created.
	
	fOPENFAIL
		Opens the file and fails if it doesn't exist.
	
	fCREATE
		Opens the file, deleting any previous content it had.
	
	:param number openMode: fOPENCREATE, fOPENFAIL or fCREATE. If not specified, fOPENCREATE is used.
	:returns: The file object if successful, otherwise NULL.
	


.. function:: readString(buffer maxSize)
	:noindex:

	
	Reads a text string from the file.
	
	:param string buffer: The buffer into which the text is placed.
	:param number maxSize: The buffer size.
	
	:returns: The number of characters read.
	



.. function:: writeString(buffer [...])
	:noindex:

	Writes a text string (or multiple text strings) to the file.

	:param string buffer: Buffer containing the string. Multiple strings may be provided.



.. function:: write(buffer count)
	:noindex:

	
	Writes data to the file.
	
	:param heapPtr buffer: Buffer of data to be written.
	:param number count: The number of bytes in the buffer.
	:returns: The number of bytes written, or NULL if it failed.
	


.. function:: read(buffer size)
	:noindex:

	
	Reads data into a buffer.
	
	:param heapPtr buffer: Buffer into which the data will be read.
	:param number size: The size in bytes in the buffer.
	:returns: The number of bytes read, or NULL if it failed.
	


.. function:: seek(offset whence)
	:noindex:

	
	Seeks to a position in the file.
	
	:param number offset: Positive or negative offset.
	:param number whence: 0 to seek from the beginning, 1 to seek from the current position, or 2 to seek from the end.
	


.. function:: close()
	:noindex:

	Closes the file.


.. function:: delete()
	:noindex:

	Deletes the file.


