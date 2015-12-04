(version 2)
(include "sci.sh")
(use "System")
(script 993)


/*
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
			
*/
(class File of Object
    (properties
        handle 0
    )

    (method (dispose)
        (self:close())
        (super:dispose())
    )


    (method (showStr param1)
        Format(param1 "File: %s" name)
    )

	/*
		Opens the file in one of the following modes:
		
		fOPENCREATE
			Opens the file. If the file doesn't exist, it is created.
			
		fOPENFAIL
			Opens the file and fails if it doesn't exist.
		
		fCREATE
			Opens the file, deleting any previous content it had.
			
		:param number openMode: fOPENCREATE, fOPENFAIL or fCREATE. If not specified, fOPENCREATE is used.
		:returns: The file object if successful, otherwise NULL.
	*/
    (method (open openMode)
        = handle 
            (switch (paramTotal)
                (case 0
                    FileIO(fiOPEN name fOPENCREATE)
                )
                (case 1
                    FileIO(fiOPEN name openMode)
                )
                (default 
                    0
                )
            )
        (if (== handle -1)
            = handle 0
        )
        return 
            (if (handle)
                self
            )(else
                0
            )
    )

	/*
		Reads a text string from the file.
		
		:param string buffer: The buffer into which the text is placed.
		:param number maxSize: The buffer size.
		
		:returns: The number of characters read.
	*/
    (method (readString buffer maxSize)
        (if (<> paramTotal 2)
            return 0
        )
        (if (not handle)
            (self:open(fOPENFAIL))
        )
        return 
            (if (handle)
                FileIO(fiREAD_STRING buffer maxSize handle)
            )(else
                0
            )
    )

	/*
	.. function:: writeString(buffer [...])
	
		Writes a text string (or multiple text strings) to the file.
		
		:param string buffer: Buffer containing the string. Multiple strings may be provided.
	*/
    (method (writeString buffer)
        (var temp0)
        (if (not handle)
            (self:open())
        )
        (if (handle)
            = temp0 0
            (while (< temp0 paramTotal)
                (if (not FileIO(fiWRITE_STRING handle buffer[temp0]))
                    return 0
                )
                ++temp0
            )
        )
        return 1
    )

	/*
		Writes data to the file.
		
		:param heapPtr buffer: Buffer of data to be written.
		:param number count: The number of bytes in the buffer.
		:returns: The number of bytes written, or NULL if it failed.
	*/
    (method (write buffer count)
        (var temp0)
        (if (not handle)
            (self:open())
        )
        return 
            (if (handle)
                FileIO(fiWRITE handle buffer count)
            )(else
                0
            )
    )

	/*
		Reads data into a buffer.
		
		:param heapPtr buffer: Buffer into which the data will be read.
		:param number size: The size in bytes in the buffer.
		:returns: The number of bytes read, or NULL if it failed.
	*/
    (method (read buffer size)
        (if (<> paramTotal 2)
            return 0
        )
        (if (not handle)
            (self:open(fOPENFAIL))
        )
        return 
            (if (handle)
                FileIO(fiREAD handle buffer size)
            )(else
                0
            )
    )

	/*
		Seeks to a position in the file.
		
		:param number offset: Positive or negative offset.
		:param number whence: 0 to seek from the beginning, 1 to seek from the current position, or 2 to seek from the end.
	*/
    (method (seek offset whence)
        (var temp0)
        = temp0 
            (if (>= paramTotal 2)
                whence
            )(else
                0
            )
        (if (not handle)
            (self:open(fOPENFAIL))
        )
        return 
            (if (handle)
                FileIO(fiSEEK handle offset temp0)
            )(else
                0
            )
    )

	// Closes the file.
    (method (close)
        (if (handle)
            FileIO(fiCLOSE handle)
            = handle 0
        )
    )

	// Deletes the file.
    (method (delete)
        (if (handle)
            (self:close())
        )
        FileIO(fiUNLINK name)
    )
)
