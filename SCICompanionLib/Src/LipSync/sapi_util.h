/* 
sapi_util.h

Copyright (C) 2005 Annosoft, LLC. Richardson, Texas. 
All rights reserved.  
	
Permission is hereby granted, free of charge, to use and distribute
this software and its documentation without restriction, including   
without limitation the rights to use, copy, modify, merge, publish,  
distribute, sublicense, and/or sell copies of this work, and to      
permit persons to whom this work is furnished to do so, subject to   
the following conditions:                                            
1. The code must retain the above copyright notice, this list of    
	conditions and the following disclaimer.                        
2. Any modifications must be clearly marked as such.                
3. Original authors' names are not deleted.                         
4. The name "Annosoft" and the authors' names can be not used to endorse or 
   promote products derived from this software without specific prior written       
   permission.                                            

ANNOSOFT AND THE CONTRIBUTORS TO THIS WORK DISCLAIM ALL WARRANTIES 
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS, IN NO EVENT ANNOSOFT NOR THE CONTRIBUTORS 
BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY 
DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING 
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 

*/	
/**
@file sapi_util.h
@brief simple utilities that don't belong anywhere else
*/
	
/**
@defgroup utility_functions Utility Functions
@brief a grab bag of utilities designed to keep the code clean

These consist of functions to load files, convert partial paths
to full paths, and various unicode and ansi conversion routines
@{
*/


/** @brief converts a TCHAR (unicode or ansi) into a wide character string

	This method uses the bstr_t object to provide character conversion 
	@param str - [in] input string, either unicode or MBCS
	@return unicode string 
*/
std::wstring TCHAR_2_wstring(const TCHAR *str);

/**@brief converts a unicode string into an MBCS string

	This is used to help print unicode strings to ansi streams
	@param str - [in] unicode string
	@return MBCS string
**/
std::string  wstring_2_string(const std::wstring& str);

/// retrieve the unicode bytes of the specified text file
HRESULT GetTextFile(const TCHAR* pszFileName,WCHAR ** ppwszCoMem,
    ULONG * pcch);

/** @} */