.. FileIO

.. default - domain::js

.. include:: /includes/standard.rst

===============
FileIO (Kernel)
===============


	FileIO handles all file operations in SCI1.1. The :class:`File` class wraps
	all these calls, and should generally be used instead.

	.. IMPORTANT:: SCI1.1 only.

.. function:: FileIO(fiOPEN filename mode)

	Opens a file.

	:param string filename: The filename.
	:param number mode: An open mode described below.
	:returns: A handle to the file if successful. Otherwise NULL.

	The modes are as follows:

	============= =======================================================================
	Mode          Description
	============= =======================================================================
	fOPENFAIL     open or fail: Try to open file, abort if not possible
	fOPENCREATE   open or create: Try to open file, create it if it doesn't exist
	fCREATE       create: Create the file, destroying any content it might have had
	============= =======================================================================


.. function:: FileIO(fiCLOSE handle)
	:noindex:

	Closes the specified file.

	:param number handle: A handle returned by FileIO(fiOPEN).

.. function:: FileIO(fiREAD handle outBuffer byteCount)
	:noindex:

	Reads a number of bytes from a file.

	:param number handle: A handle returned by FileIO(fiOPEN).
	:param heapPtr outBuffer: A buffer into which the bytes are placed (at least byteCount big).
	:param number byteCount: The number of bytes to read.
	:returns: The number of bytes actually read.

.. function:: FileIO(fiWRITE handle inBuffer byteCount)
	:noindex:

	Writes data to a file.

	:param number handle: A handle returned by FileIO(fiOPEN).
	:param heapPtr inBuffer: A buffer containing the data to be written.
	:param number byteCount: The number of bytes to write.

.. function:: FileIO(fiUNLINK filename)
	:noindex:

	Deletes a file.

	:param string filename: The filename to delete.

.. function:: FileIO(fiREAD_STRING outText byteCount handle)
	:noindex:

	Reads a string from a file.

	:param heapPtr outText: A buffer into which string is placed (at least byteCount big).
	:param number byteCount: The number of bytes to read.
	:param number handle: A handle returned by FileIO(fiOPEN).

.. function:: FileIO(fiWRITE_STRING handle inString byteCount)
	:noindex:

	Writes a string to a file.

	:param handle handle: A handle returned by FileIO(fiOPEN).
	:param heapPtr inText: The string to write.

.. function:: FileIO(fiSEEK handle offset whence)
	:noindex:

	Seeks to a position in the file.

	:param number handle: A handle returned by FileIO(fiOPEN).
	:param number offset: The offset.
	:param number whence: The point to which the offset is relative.

	======== ========================
	Whence values
	---------------------------------
	Values   Position
	======== ========================
	0        Beginning
	1        Current position
	2        End
	======== ========================

.. function:: FileIO(fiFIND_FIRST filenameMask outBuffer fileAttributes)
	:noindex:

	Begins a file enumeration in the current directory. Continue the enumeration
	using FileIO(fiFIND_NEXT).

	:param string filenameMask: A file name filter string, like "\*.txt".
	:param string outBuffer: The buffer that receives the first filename.
	:param number fileAttributes: Attributes of the file. Pass 0 for regular files.
	:returns: TRUE if the first file was found, otherwise FALSE.

.. function:: FileIO(fiFIND_NEXT outBuffer)
	:noindex:

	Returns the next file in an enumeration begun by fiFIND_FIRST.

	:param string outBuffer: The buffer that receives the next filename.
	:returns: TRUE if there was another file, otherwise FALSE.

.. function:: FileIO(fiEXISTS filename)
	:noindex:

	Checks whether a file exists.

	:param string filename: The filename.
	:returns: TRUE if the file exists.

.. function:: FileIO(fiRENAME oldFilename newFilename)
	:noindex:

	Renames a file.

	:param string oldFilename: The current filename.
	:param string newFilename: The new filename.


