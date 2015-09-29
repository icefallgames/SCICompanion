/* 
sapi_lipsync.h

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
@file sapi_lipsync.h
@brief declares classes and structures for automatic lipsync using SAPI 5.1

This file declares 4 classes:
- alignment_result. a data class holding the labels and timings returned by sapi
- sapi_lipsync. base class containing common processing.
- sapi_textbased_lipsync. aligns an audio file with a text transcript
- sapi_textless_lipsync. pulls phonemes and word time from an audio file 

@author
Mark Zartler - mzartler@annosoft.com

@date
- created august 28 2005
- added phoneme conversion oct 21 2005

*/

#ifndef _H_SAPI_LIPSYNC
#define _H_SAPI_LIPSYNC

// forwards 
class phoneme_estimator;

/**
    @brief This data class contains phoneme alignment/lipsync results

     sapi_lipsync and it's subclasses generate a list of alignment_result
	 objects.
	
	 The 'raw' form will contain the start and end time for each recognized
	 word, along with the orthography (the text of the word) and the
	 list of phonemes (alignment_result::m_phonemes)
	
	 alignment_result::m_phonemeEndTimes are not produced by SAPI.
	 The actual phoneme times are estimated by this application. (see
	 sapi_lipsync::finalize_phoneme_alignment for details).

	 Applications can either use the raw word results or use the finalized
	 data.
	@see ::run_sapi_textbased_lipsync for example code
	@see ::run_sapi_textless_lipsync for example code
  */
class alignment_result
{
public:
    /// start time in milliseconds of this alignment result
    long        m_msStart;
    /// end time in milliseconds of this alignment result
    long        m_msEnd;
    /// the text representing this result
    std::wstring m_orthography;
    /** @brief the phonemes representing this result <P>
		These are pulled from sapi. Each phoneme is a separate
		index in the phonemes array (for easier parsing) */
    std::vector<std::wstring> m_phonemes;
	/**@brief the end time for each phoneme in milliseconds.<P>
		SAPI 5.1 does not generate this information, instead sapi_lipsync
		uses the phoneme_estimator to do this. applications can also
		build their own */
	std::vector<long> m_phonemeEndTimes;
};

/** @brief base class for lipsync

    This class handles all the SAPI objects and handles
    the management of the audio stream.

    Subclasses will override the callback method and also
    handle grammar initialization of SAPI. These methods
    are different depending on whether text based, or textless
    lipsync is used. 

	The lipsync results are stored in sapi_lipsync.m_results.
	The raw results (before sapi_lipsync::finalize_phoneme_alignment is called)
	contain the timings and the recognized words, and phonemes, but don't 
	include timing for each phoneme with the word. 
	
	sapi_lipsync::finalize_phoneme_alignment estimates the values and
	also inserts silence markers (empty orthography with a phoneme label
	'x'). 

    Applications can either use the raw results and generate their own
	timings, or use the default phoneme_estimator, to use the out of the
	box system. 
	@see sapi_textbased_lipsync, sapi_textless_lipsync, alignment_result,
	phoneme_estimator.	
	@see ::run_sapi_textbased_lipsync for example code
	@see ::run_sapi_textless_lipsync for example code
**/
class sapi_lipsync
{
public:
	/// default constructor
	sapi_lipsync();

     /// constructor with estimator object
    sapi_lipsync(phoneme_estimator* pEstimator);
    
    // destructor
    virtual ~sapi_lipsync();

    /// destroy objects
    virtual void close();

    /// this method performs common initialization of SAPI objects
    bool initializeObjects();

    /// this method loads the audio file into the ISPStream 
    bool loadAudio(const std::wstring& audioFile);

    /// retrieve the error string
    const std::wstring& getErrorString();

    /// returns true if subclass thinks we are finished with async lipsync
    virtual bool isDone() { return (m_bDone); }

	/// this method returns the current best phoneme alignment
	std::vector<alignment_result>& get_phoneme_alignment();

	/// pretties up the phoneme alignment
	virtual void finalize_phoneme_alignment();

	/// prints the current best results to the specified stream
    virtual void print_results(std::ostream& os);

    /** @brief pure virtual function implemented by subclasses<P>
        This will be called when sapi events occur. The processing
        will depend on the type of lipsync being performed. 
	**/
    virtual void callback() = 0;

	/**@brief converts a SAPI timestamp into a millisecond time
		@param ts - [in] microsecond time
		@return millisecond time 
	**/
    long sapi_time_to_milli(ULONGLONG ts)
    {
        return (long(ts / 10000));
    }
	/** @brief converts audio bytes into milliseconds using the sample rate of the audio stream <P>
		@param dwBytes - [in] byte position in the audio stream
		@return millisecond timing for the audio at the specified byte
	 **/
    long bytes_to_milli(DWORD dwBytes)
    {
        return (UINT)((dwBytes * 1000 )/ m_pWaveFmt->nAvgBytesPerSec); 
    }

protected:
    // SAPI COM objects
    /// the recognizer COM object.
    CComPtr<ISpRecognizer>		m_recog;
    /// the recognizer context COM object
    CComPtr<ISpRecoContext>		m_recogCntxt;
    /// the grammar COM object
    CComPtr<ISpRecoGrammar>		m_grammar;
    /// the phone converter object. Converts PHONEID into strings
    CComPtr<ISpPhoneConverter>	m_phnCvt;
    /// the audio source object
    CComPtr<ISpStream>			m_audioStream;


    /// wave format of the audio we are processing
    WAVEFORMATEX				*m_pWaveFmt;

	/**@brief the phoneme estimator used to heuristically 
	   spread phoneme timings across the word. Can be NULL. <P>
	   The estimator is set by the constructor, where a pointer is passed in.
	   m_pPhnEstimator is only used in sapi_lipsync::finalize_phoneme_results.
	 **/
	phoneme_estimator			*m_pPhnEstimator;			

    /// error description. 
    std::wstring m_err;

	/// audio file path. Needed for printing .anno file
	std::wstring m_strAudioFile;

    /** @brief static method used to receive notifications from SAPI
     */
    static void _stdcall sapi_callback(WPARAM wParam, LPARAM lParam);


    /// results container
    std::vector<alignment_result> m_results;


    /// subclasses will set this when the lipsync is done.
    bool                          m_bDone;

};


/** 
@brief This class uses SAPI to align a text buffer with an audio file.

    It uses some tricks here and the results are not always complete.

    We create a SAPI grammar rule representing the input string. The idea
    is to use (abuse?) the Command & Control Speech engine of SAPI to match the input string.
	
	This is very different from the textless lipsync which uses the continuous
	speech recognizer. (Hence the two subclasses of sapi_lipsync)

    One problem, unless it's the audio file is very short, it always fails to fully "recognize"
    the text. To work around this problem, we listen for hypotheses from the
    SAPI object and store the one which records the most text. So basically,
    we accept hyphothesis and/or final answers depending on which one 
    generates the most information. 

	@see ::run_sapi_textbased_lipsync for example code
	@todo We noticed that the Command and Control system often "stops short"
	It might be interesting to segment the rest of the text with the remaining
	audio, do it until we are finished. Might be an interesting upgrade.	
*/
class sapi_textbased_lipsync : public sapi_lipsync
{
public:
	/// constructor
	sapi_textbased_lipsync();

	/// constructor with estimator object
    sapi_textbased_lipsync(phoneme_estimator* pEstimator);

	/// destructor
	virtual ~sapi_textbased_lipsync();
    
    /// start the asyncronous lipsync process given a text file and an audio file
    virtual bool lipsync(const std::wstring& strAudioFile, const std::wstring& strText);


    /** @brief notifications from the sapi engine. */
    virtual void callback();

	/// override of sapi_lipsync::print_results
    virtual void print_results(std::ostream& os);

    // the method cleans the transription text for use in text based lipsync.
    static std::wstring preprocess_text(const std::wstring& in);
    
	/// the method filters a characters that cause performance degradation of
	/// text based processes
    static bool is_dirty_char(wchar_t in);

protected:
	/**@brief the raw text string results.
		This is used to decide whether or not to accept a hyphothesis. 
		If the result generates a longer string than m_strResults, we
		accept the hypothesis and generate phonemes, if not. we skip it */
    std::wstring                  m_strResults;

	/// input text. needed for printing anno file
	std::wstring				  m_strInputText;

};

/** @brief this class implements lipsync without a transcription

  This uses the Dictation grammar, and simply points the audio to it
  and waits for the results. 

  @see ::run_sapi_textless_lipsync for example code
  */
class sapi_textless_lipsync : public sapi_lipsync
{
public:
	/// constructor
	sapi_textless_lipsync();

	/// constructor with estimator object
    sapi_textless_lipsync(phoneme_estimator* pEstimator);

	/// destructor
	virtual ~sapi_textless_lipsync();
    
    /// start the asyncronous lipsync process given a text file and an audio file
    virtual bool lipsync(const std::wstring& strAudioFile);
    
    /** @brief notifications from the sapi engine. */
    virtual void callback();

};


#endif



