.. DeviceInfo

.. default - domain::js

.. include:: /includes/standard.rst

===================
DeviceInfo (Kernel)
===================


	Retrieves information about the computer's devices.

.. function:: DeviceInfo(diGET_DEVICE thePath outBuffer)

	Obtains the drive letter from a directory string.
	It evaluates the specified path and writes it's drive letter to strBuf.

	:param string thePath: A directory string.
	:param string buffer: A buffer in which to write the drive letter.
	:returns: a pointer to the end of the string written to outBuffer (the index of the NULL terminating character).
		
	Example::

		(procedure (Foo &tmp [buffer 3])
			(DeviceInfo diGET_DEVICE "C:\scistudio" @buffer)
			(Print @strBuf) ; Prints "C:"
		)
	
.. function:: DeviceInfo(diGET_CURRENT_DEVICE thePath outBuffer)
	:noindex:

	Obtains the drive letter the current path and and writes it to strBuf.

	:param string thePath: A directory string.
	:param string buffer: A buffer in which to write the drive letter.
	:returns: a pointer to the end of the string written to outBuffer (the index of the NULL terminating character).
		
	Example::

		(procedure (Foo &tmp [buffer 3])
			(DeviceInfo diGET_CURRENT_DEVICE @buffer)
			(Print @strBuf) ; Prints "C:", "D:", or whatever the current drive is
		)

.. function:: DeviceInfo(diPATHS_EQUAL path1 path2)
	:noindex:

	Compares path1 and path2. If they both point to the same physical location, it returns TRUE, otherwise, FALSE. 

	:param string path1: The first path.
	:param string path2: The second path.
	:returns: TRUE if both paths point to the same physical location.

	Example::

		if (DeviceInfo diPATHS_EQUAL @aPath "C:\scistudio")
		{
			(Print "The paths are equal")
		} 
	
.. function:: DeviceInfo(diIS_FLOPPY thePath)
	:noindex:

	Checks the specified drive letter to see if it's a floppy or not, returning TRUE or FALSE.

	:param string thePath: A path with a drive letter.
	:returns: TRUE if the drive letter is a floppy drive.

	Example::

		DeviceInfo(diIS_FLOPPY "A:") // returns TRUE
		DeviceInfo(diIS_FLOPPY "C:") // returns FALSE

.. function:: DeviceInfo(command)
	:noindex:

	It is unknown what this does.

	Example::

		(DeviceInfo 4)


