.. FOpen

.. default - domain::js

.. include:: /includes/standard.rst

==============
FOpen (Kernel)
==============

.. function:: FOpen(filename)

.. function:: FOpen(filename mode)
	:noindex:

	
	Opens the file with the specified mode. Upon success, it returns a handle to the file, otherwise, it returns NULL.
	
	.. IMPORTANT::
		This kernel is SCI0 only. For SCI1.1, see :func:`FileIO`.
	
	:param string name: The filename.
	:param number mode: The open mode.
	
	The modes are as follows:
	
	============= =======================================================================
	Mode          Description
	============= =======================================================================
	fOPENCREATE   open or fail: Try to open file, abort if not possible
	fOPENFAIL     open or create: Try to open file, create it if it doesn't exist
	fCREATE       create: Create the file, destroying any content it might have had
	============= =======================================================================
	
	.. NOTE::
		You may note that the mode flags don't match the descriptions. SCI Studio had the flags defined incorrectly, and this reflects that error. This is not the case for :func:`FileIO`.
	
	Example::
	
		(var hFile, buffer[41])
		// Open the file and write to it
		(if( <> NULL (= hFile FOpen("somefile.txt" fCREATE)) )
			FPuts(hFile "Hello World!")
			FClose(hFile)
		)
	
		// Open the file, read it's first string, and print it to the screen
		(if( <> NULL (= hFile FOpen("somefile.txt" fOPENFAIL)) )
			FGets(buffer 40 hFile)
			FClose(hFile)
	
			Print(buffer)
		)


