/* 
sapi_lipsync.cpp

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
@file sapi_lipsync.cpp
@brief implements classes and structures for automatic lipsync using SAPI 5.1

@b Classes
- alignment_result. a data class holding the labels and timings returned by sapi
- sapi_lipsync. base class containing common processing.
- sapi_textbased_lipsync. aligns an audio file with a text transcript
- sapi_textless_lipsync. pulls phonemes and word time from an audio file 


@author
Mark Zartler - mzartler@annosoft.com

@date
- created august 28 2005
- added better support for printing .anno files Oct 21, 2005

*/


#include "stdafx.h"
#include <sphelper.h>
#include <comdef.h>
#include <string>
#include "sapi_lipsync.h"
#include "phone_estimate.h"
#include "sapi_util.h"
#include "format.h"

///////////////////////////////////////////////////////////////////////////////
/// constants
///////////////////////////////////////////////////////////////////////////////

#define GID_LIPSYNC   0   // grammar identifier. 

/// Interest level for event in SAPI
const ULONGLONG ullInterest = SPFEI(SPEI_SOUND_START) | SPFEI(SPEI_SOUND_END) |
                                      SPFEI(SPEI_PHRASE_START) | SPFEI(SPEI_RECOGNITION) |
                                      SPFEI(SPEI_FALSE_RECOGNITION) | SPFEI(SPEI_HYPOTHESIS) |
                                      SPFEI(SPEI_INTERFERENCE) | SPFEI(SPEI_RECO_OTHER_CONTEXT) |
                                      SPFEI(SPEI_REQUEST_UI) | SPFEI(SPEI_RECO_STATE_CHANGE) |
                                      SPFEI(SPEI_END_SR_STREAM) | 
                                      SPFEI(SPEI_PROPERTY_NUM_CHANGE) | SPFEI(SPEI_PROPERTY_STRING_CHANGE);




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// sapi_lipsync class implementation
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
sapi_lipsync::sapi_lipsync()
{
    m_pPhnEstimator = NULL;
}

/**
	This method constructs the lipsync object with a phoneme_estimator

	This will be used to hueristically spread the phonemes
	across the word time stamp. 
	@param pEstimator - [in] the estimator. Can be NULL
*/
sapi_lipsync::sapi_lipsync(phoneme_estimator* pEstimator)
{
	m_pPhnEstimator = pEstimator;
}


///////////////////////////////////////////////////////////////////////////////
sapi_lipsync::~sapi_lipsync()
{
    close();
}


///////////////////////////////////////////////////////////////////////////////
/**
    This method releases SAPI com components
*/
void sapi_lipsync::close()
{
    this->m_recogCntxt.Release();
    this->m_grammar.Release();
    this->m_recog.Release();
    this->m_phnCvt.Release();
    if (m_pWaveFmt)
    {
        /// allocated by COM object
        CoTaskMemFree(m_pWaveFmt);
    }
}


///////////////////////////////////////////////////////////////////////////////
/**
    This method initializes SAPI and creates the objects used by subclasses,
    it does not start the lipsync process. 
  */
bool sapi_lipsync::initializeObjects()
{
    HRESULT hr = S_OK;
    m_err = L"";
    try
    {
        // create the recognizer (inproc)
        hr = this->m_recog.CoCreateInstance(CLSID_SpInprocRecognizer);
        if (hr != S_OK) 
        {
            m_err = L"Error: Can't create SAPI Speech Recognizer (ISpRecognizer)";
            throw (hr);
        }

        // create the recognition context from the recognizer
        hr = this->m_recog->CreateRecoContext(&this->m_recogCntxt);
        if (hr != S_OK)
        {
            m_err = L"Error: Cannot create SAPI Recognition Context (ISpRecoContext)";
            throw (hr);
        }            

        hr = m_recogCntxt->SetNotifyCallbackFunction(&this->sapi_callback, 0, LPARAM(this));
        if (hr != S_OK)
        {
            m_err = L"Error: Cannot set notify callback function. (SetNofifyCallbackFunction)";
            throw (hr);
        }
        // initialize and disable the grammar
        hr = m_recogCntxt->CreateGrammar(GID_LIPSYNC, &m_grammar);
        if (hr != S_OK)
        {
            m_err = L"Error: Failed to create grammar for lipsync";
            throw (hr);
        }
		// not totally sure here!
		//hr = m_grammar->SetGrammarState(SPGS_DISABLED); Let subclasses handle the grammer state
        if (hr != S_OK)
        {
            m_err = L"Error: Failed to disable the grammar.";
            throw (hr);
        }

		// need a phoneme converter to map SPHONEID into phoneme strings
        hr = SpCreatePhoneConverter(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), 
            NULL, NULL, &this->m_phnCvt);
        if (hr != S_OK)
        {
            m_err = L"Error: Failed create phoneme converter";
            throw (hr);
        }

	    // Set interest level for events, we want all events, just in case.                
        hr = m_recogCntxt->SetInterest(ullInterest, ullInterest);
        if (hr != S_OK)
        {
            m_err = L"Error: Cannot correctly set notifications for the Speech Recognizer";
            throw(hr);
        }
        // turn off recognizer while we initialize things. HMMM
        //m_recog->SetRecoState(SPRST_INACTIVE);
    } 
    catch (HRESULT& _hr)
    {
        hr = _hr;
    }
    return (hr == S_OK);
}


/**
	This method uses the sapi helpers to create an ISpStream object
	given an audio file
	@param audioFile - [in] path to audio file
	@return true if successful, false if not. Sets an appropriate error code
**/
bool sapi_lipsync::loadAudio(const std::wstring& audioFile)
{
    HRESULT hr = S_OK;

	this->m_strAudioFile = audioFile;
    try
    {
        m_bDone = false;
		
        hr = SPBindToFile(audioFile.c_str(), SPFM_OPEN_READONLY, &this->m_audioStream);
        if (hr != S_OK)
        {
            m_err = L"Error: Can't open audio file";
            throw(hr);
        }
        GUID guid; // unused
        hr = this->m_audioStream->GetFormat(&guid, &m_pWaveFmt);
        if (hr != S_OK)
        {
            m_err = L"Error: cannot get audio formatting information";
            throw (hr);
        }
        
        /// set the input stream to our newly created audio file
        hr = this->m_recog->SetInput(this->m_audioStream, TRUE);
        if (hr != S_OK)
        {
            m_err = L"Error: cannot set the input stream for ASR";
            throw (hr);
        }
    }
    catch (HRESULT& _hr)
    {
        hr = _hr;
    }
    return (hr == S_OK);

}

/**
    This method returns the current error string describing
    what went wrong, if anything.
    @retval string - error string 
*/
const std::wstring& 
sapi_lipsync::getErrorString()
{
    return (m_err);
}

/**
    Callback method. This method dispatches the callback to
    the class which installed the method, using unsafe casting. 
**/
void _stdcall 
sapi_lipsync::sapi_callback(WPARAM wParam, LPARAM lParam)
{
    sapi_lipsync* pThis = (sapi_lipsync*)lParam;
    pThis->callback();
}

/**
	The results list can be copied, edited as a reference, etc.

	The exact data in the results depends on whether 
	sapi_lipsync::finalize_phoneme_alignment has been called.
	@return - reference to list of current alignment results 
	@note if the lipsync is still running, we're making threading assumption.
	I think it's ok, since sapi events seem t be getting dispatched from the event loop.

**/
std::vector<alignment_result>& 
sapi_lipsync::get_phoneme_alignment()
{
	return (m_results);
}


/**
	This method tweeks the phoneme alignment. It first adds 'x' to 
	areas not covered by the existing m_results. At the same time,
	it maps the phonemes and estimates timings for them in the words.
	
	SAPI 5.1 does not produce phoneme timing, we use some heuristics
	to guess at the appropriate timings for the phoneme.

	This should be called before print_results.
**/

void sapi_lipsync::finalize_phoneme_alignment()
{
	std::vector<alignment_result>::iterator p, pEnd;
    std::vector<alignment_result> result;
	result.reserve(m_results.size() * 3 + 2);
	p = m_results.begin();
    pEnd = m_results.end();
	long lastEndTime = 0;
	while (p != pEnd)
	{
		if (p->m_msStart > lastEndTime)
		{
			alignment_result r;
			r.m_msEnd = p->m_msStart;
			r.m_msStart = lastEndTime;
			r.m_phonemes.push_back(std::wstring(L"x"));
			r.m_phonemeEndTimes.push_back(p->m_msStart);			
			lastEndTime = p->m_msStart;
			result.push_back(r);
		}
		if (m_pPhnEstimator)
			m_pPhnEstimator->EstimatePhonemeAlignment(*p);
		else
			phoneme_estimator::TrivialPhonemeAlignment(*p);
		result.push_back(*p);
		lastEndTime = p->m_msEnd;
		p++;
	}
	/// assign the new "finalized" results;
	this->m_results = result;
}


/** 
	This method prints the results of the lipsync (word alignment) to
	an output stream of choice.

	It is printed in a format that can be directly loaded in "The Lipsync Tool"
	http://www.annosoft.com/demos.htm. You can download the free version to see
	the results (with annosoft's nice smoothing of the phonemes)

	The format of the results see \ref anno_format.

	For the output, since we don't have an energy value, we just use "75" percent. Other
	readers can ignore this parameter.
	@note sapi_lipsync::finalize_phoneme_alignment should be called before
	calling this method.

    @param os - [in] stream to print the results to
**/
void sapi_lipsync::print_results(std::ostream& os)
{

    std::vector<alignment_result>::iterator p, pEnd;
    p = m_results.begin();
    pEnd = m_results.end();
	// print the audio result
	if (m_strAudioFile.size())
	{
		os << "audio " << wstring_2_string(m_strAudioFile) << std::endl;
	}
    while (p != pEnd)
    {
        // print the word marker
		if (p->m_orthography.size())
		{
			os << "word " << p->m_msStart << ' ' << p->m_msEnd <<
				' ' << wstring_2_string(p->m_orthography) << std::endl;
		}
		// print the phn markers
        long pos = p->m_msStart;
        for (unsigned long j = 0; j < p->m_phonemes.size(); j++)
        {
            long start = pos; 
            long end = p->m_phonemeEndTimes[j];
            os << "phn " << start << ' ' << end << ' ' << 75 <<
				' ' << wstring_2_string(p->m_phonemes[j]) << std::endl;
            pos = end;
        }
        p++;
    }
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// sapi_textbased_lipsync class implementation
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
	constructor. Nothing to do
**/
sapi_textbased_lipsync::sapi_textbased_lipsync()
{

}

/**
	This method constructs the lipsync object with a phoneme_estimator

	This will be used to hueristically spread the phonemes
	across the word time stamp. 
	@param pEstimator - [in] the estimator. Can be NULL
*/
sapi_textbased_lipsync::sapi_textbased_lipsync(phoneme_estimator* pEstimator) :
sapi_lipsync(pEstimator)
{
	
}


/** 
	destructor. automatic
**/
sapi_textbased_lipsync::~sapi_textbased_lipsync()
{

}

/**
	The lipsync process runs asyncronously. Application will need to poll for completion
	or decide when it's time to bail. 

	The program first initializes the SAPI objects, and the loads the audio file.

	We use, abuse the command and control grammar. We create a top level grammatical
	rule, and then add the source text as a lexical transition. I believe that internally
	the command and control grammar creates transitions for each word. At least that's
	what the hypothesis seem to show.

	We disable speech processing until we have loaded the grammar.

    We also preprocess the file removing punctuation and other "dirty" characters
	that seem to negatively impact performance.
	@see ::run_sapi_textbased_lipsync for an example
	
	@param strAudioFile - [in] name of the audio file
	@param strText - [in] text transcript (not a file but the contents)
	@retval true if the lipsync got started successfully
	@retval false - if the lipsync failed. call sapi_lipsync::getErrorString for a detailed
		description of the error.	
*/
bool 
sapi_textbased_lipsync::lipsync(const std::wstring& strAudioFile, const std::wstring& strText)
{
    HRESULT hr;
    try
    {
        m_strInputText = strText;
        if (!this->initializeObjects())
            throw (HRESULT(E_FAIL));
        
        if (!this->loadAudio(strAudioFile))
             throw (HRESULT(E_FAIL));
        
        // initialize the grammar
            
        SPSTATEHANDLE hLipsyncRule;
        /// create a new top level rule. We don't want to use dictation here
        /// so we assign our own grammar.
        hr = this->m_grammar->GetRule(L"TextLipsync", NULL,
							SPRAF_TopLevel | SPRAF_Active, TRUE,
							&hLipsyncRule);

        if (hr != S_OK)
        {
            m_err = L"Failed to create grammar rule for text based lipsync";
            throw (hr);
        }
		
        // prepare text for text based lipsync. Tokenize out formatting, punctuation
        std::wstring strIn = preprocess_text(strText);
        // create the phrase inside the rule
        hr = m_grammar->AddWordTransition(hLipsyncRule, NULL, strIn.c_str(), 
            L" ", SPWT_LEXICAL, 1, NULL);

        if (hr != S_OK)
        {
            m_err = L"Failed to create lipsync rule for specified text transcription";
            throw (hr);
        }
		
		// finalize the grammar
        hr = m_grammar->Commit(0);
        if (hr != S_OK)
        {
            m_err = L"Failed to commit lipsync text rule for specified text transcription.";
            throw (hr);
        }
                
        // turn the grammar on
        hr = m_grammar->SetGrammarState(SPGS_ENABLED);
        if (hr != S_OK)
        {
            m_err = L"Error: Failed to disable the grammar.";
            throw (hr);
        }
        // start up recognition
        m_recog->SetRecoState(SPRST_ACTIVE);
		// enable the rule
        m_grammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);

        // now we should be running!

    }
    catch (HRESULT _hr)
    {
        hr = _hr;
    }
    return (hr == S_OK);
}

/**
	This is called by SAPI 5.1 when it has an event. 
	
	We use the SpEvent class provided by their SDK to simplify the processing.

	Basically, when we get a "RECOGNITION" event or a "SPEI_HYPOTHESIS" event
	we process them the same. Hypothesis are more likely, for all but very
	short files, "SPIE_RECOGNITION" is a rarity. 

	Since the hypothesis will include duplicate data, we have a decision. We
	can save the newest hypothesis or we can save the one which generates the
	most alignments. Imperically, it seems that sticking with the longest
	result works best. But perhaps this is not so.
**/
void sapi_textbased_lipsync::callback()
{
    //USES_CONVERSION;
    CSpEvent event;

	ISpRecoResult *pRecoResult; // recoResult from the event
	SPPHRASE *pSpPhrase;	// phrase from recoResult
	SPRECORESULTTIMES pRecoResultTimes; // result times from RecoResult
	WCHAR phone_buffer[256];			// buffer for the phonemes
	UINT msStart;						// start time of the phrase
	

	// Process the events
	while (event.GetFrom(this->m_recogCntxt) == S_OK)
    {
		if (event.eEventId == SPEI_RECOGNITION || event.eEventId == SPEI_HYPOTHESIS)
		{
			// text based has to accept hypothesis or it mostly fails unless the
			// script is very short

			// pull out the result object
			pRecoResult = event.RecoResult();

			// pull the whole text from the result
			CSpDynamicString pSapiText;
			pRecoResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, FALSE, &pSapiText, NULL);

			// get the start time for the phrase. we use this as an offset for the phrase
			// elements. Not sure if this is correct.
			pRecoResult->GetResultTimes(&pRecoResultTimes);
			msStart = sapi_time_to_milli(pRecoResultTimes.ullStart);

			std::wstring strPrintText = pSapiText;
			std::cerr << "hypothesis: " << wstring_2_string(strPrintText) << std::endl;
			// if the new results are longer than existing results in orthographic form
			// we accept the results and process the phonemes. Otherwise, we skip it
            if ((wcslen(pSapiText) > this->m_strResults.size()))
            {
                m_strResults = pSapiText;
				// clear the old results. This hypothesis trumps it
                this->m_results.clear();
			    
			    // extract the phrase object
			    pRecoResult->GetPhrase(&pSpPhrase);

			    if (pSpPhrase != NULL)
			    {
				    // Process each element of the phrase. These should be our
					// orthorgraphs
					const SPPHRASEELEMENT *p = pSpPhrase->pElements;
					const SPPHRASEELEMENT *pEnd = p + pSpPhrase->Rule.ulCountOfElements;
				    while (p != pEnd)
				    {
						// for each phrase element we create a marker 
					// that contains the time stamps along with the 
					// phonemes. associated with it.
					    alignment_result al;
                        al.m_orthography = p->pszDisplayText;
					    // Get the phonemes
                        ULONG j = 0;
                        SPPHONEID phn[2];
                        phn[1] = 0x00;
                        while (p->pszPronunciation[j] != 0)
                        {
                            // process each phoneme
                            phn[0] = p->pszPronunciation[j];
                            m_phnCvt->IdToPhone(phn, phone_buffer);
                            al.m_phonemes.push_back(phone_buffer);
                            j++;
                        }
                                             
                        // start time of the ortheme
					    al.m_msStart= msStart + bytes_to_milli(p->ulAudioStreamOffset);
					    // end time of the ortheme
					    al.m_msEnd = bytes_to_milli(p->ulAudioSizeBytes);
                        al.m_msEnd += al.m_msStart;
						// add it to the results
                        m_results.push_back(al);
						
						p++;
				    }
			    }
            }
		}
		else if (event.eEventId == SPEI_END_SR_STREAM)
		{
			// This event occurs when the stream has finished processing.
            // we set a flag to indicate that things are done.
			m_bDone = TRUE;        
		}
    }
}

/**
	We call the base class to process most of the results and then
	print out the original text in the .anno format at the end of the file.
*/
void sapi_textbased_lipsync::print_results(std::ostream& os)
{
	sapi_lipsync::print_results(os);
	// use bstr_t to convert to single byte
	bstr_t annoText = m_strInputText.c_str();
	os << "%%-begin-anno-text-%% " << std::endl;
	os << (const char*)annoText << std::endl;
	os << "%%-end-anno-text-%%" << std::endl;

}

/**
	SAPI 5.1 is picky about the data input into the Command and Control system.

	This method preprocesses the transcription text removing characters that
	cause problems with alignment.
	
	If it finds a dirty character or sequence of them it replaces them with
	pure white space. This definitely improves things.

	@param in - transcription
	@retval transcription modified with bad characters/punctuation removed.
**/
std::wstring 
sapi_textbased_lipsync::preprocess_text(const std::wstring& in)
{
    std::wstring::const_iterator p, pEnd;
    std::wstring out;
    p = in.begin();
    pEnd = in.end();
    while (p != pEnd)
    {
        if (is_dirty_char(*p))
        {
            while (p != pEnd && is_dirty_char(*p))
                p++;
            out += L" ";
        }
        if (p != pEnd)
        {
            out += *p;
            p++;
        }
    }
    bstr_t ist = out.c_str();
    std::cerr << (const char*)ist << std::endl;
    return (out);
}

/**
	checks to see if we have a character that sapi is not friendly with.
	Currently we just filter spaces, punctuation and quote markers.

	This routine will probably need tweeking. I've worked it to just
	return false for alpha-numeric and true for everything else, but
	I got a little paranoid.
	@param - [in] character
	@retval - true if its dirty.
**/
bool sapi_textbased_lipsync::is_dirty_char(wchar_t in)
{
    if (iswspace(in) || (iswpunct(in) && in != L'\'')) 
		return (true);
    return (false);
    if (iswalnum(in))
        return (false);
    return (true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// sapi_textless_lipsync class implementation
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/**
constructor. Nothing to do.
**/
sapi_textless_lipsync::sapi_textless_lipsync()
{
}


/**
	This method constructs the lipsync object with a phoneme_estimator

	This will be used to hueristically spread the phonemes
	across the word time stamp. 
	@param pEstimator - [in] the estimator. Can be NULL
*/
sapi_textless_lipsync::sapi_textless_lipsync(phoneme_estimator* pEstimator) :
sapi_lipsync(pEstimator)
{
	
}

/**
	destructor. everything is on auto pilot
 **/
sapi_textless_lipsync::~sapi_textless_lipsync()
{

}

/**
	Given an audio file, this method kicks off the SAPI 5.1 DICTATION grammar
	on it. This is Microsoft's Speaker Independent Speech Recognition.

	@see ::run_sapi_textless_lipsync for an example

	Results are handled in the sapi_textless_lipsync::callback. 
	@param strAudioFile - [in] the source audio file
	@retval true if lipsync got started
	@retval false if it failed to start
**/	
bool sapi_textless_lipsync::lipsync(const std::wstring& strAudioFile)
{
    HRESULT hr;
    try
    {
        
        if (!this->initializeObjects())
            throw (HRESULT(E_FAIL));
        
        if (!this->loadAudio(strAudioFile))
             throw (HRESULT(E_FAIL));
        
        // initialize the grammar
        hr = m_grammar->LoadDictation(NULL, SPLO_STATIC);
        if (hr != S_OK)
        {
            m_err = L"Error: Cannot load SAPI Dictation Grammar";
        }
    
        hr = m_grammar->SetDictationState(SPRS_ACTIVE);

        if (hr != S_OK)
        {
            m_err = L"Cannot activate the SAPI Dictation Grammar";
            throw (hr);
        }
        
        m_recog->SetRecoState(SPRST_ACTIVE);
       

        // now we should be running!

    }
    catch (HRESULT _hr)
    {
        hr = _hr;
    }
    return (hr == S_OK);
}

    

/**
	This is called when SAPI 5.1 has an event.

	In the textless case, we only handle SPIE_RECOGNITION event. We aren't looking
	at SPIE_HYPOTHESIS. This might be an error. We might be more robust by handling
	both.

	We process the event and add the phonemes we get to the result list
**/
void sapi_textless_lipsync::callback()
{
    CSpEvent event;	// the event

	ISpRecoResult *pRecoResult;			// recoResult from the event
	SPPHRASE      *pSpPhrase;			// phrase from recoResult
	SPRECORESULTTIMES pRecoResultTimes; // result times from RecoResult
	WCHAR phone_buffer[256];			// phoneme buffer for conversion
	long msStart;						// time stamp of the result 
	

    while (event.GetFrom(this->m_recogCntxt) == S_OK)
    {
		if (event.eEventId == SPEI_RECOGNITION /*|| event.eEventId == SPEI_HYPOTHESIS */)
		{	
			// for textless we only accept full recognition. This might be an area
			// to watch out for
			
			// pull out the result object
			pRecoResult = event.RecoResult();

			// pull the whole text from the result
			CSpDynamicString pSapiText;
			pRecoResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, FALSE, &pSapiText, NULL);

            // get the start time for the phrase. we use this as an offset for the phrase
			// elements. Not sure if this is correct.
			pRecoResult->GetResultTimes(&pRecoResultTimes);
			msStart = sapi_time_to_milli(pRecoResultTimes.ullStart);

			// extract the phrase object
			pRecoResult->GetPhrase(&pSpPhrase);

			if (pSpPhrase != NULL)
			{
				// Process each element of the phrase. These should be our
				// orthorgraphs
				const SPPHRASEELEMENT *p = pSpPhrase->pElements;
				const SPPHRASEELEMENT *pEnd = p + pSpPhrase->Rule.ulCountOfElements;
				while (p != pEnd)
				{
					// for each phrase element we create a marker 
					// that contains the time stamps along with the 
					// phonemes. associated with it.
					alignment_result al;
                    al.m_orthography = p->pszDisplayText;
					// Get the phonemes
					ULONG j = 0;
                    SPPHONEID phn[2];
                    phn[1] = 0x00;
                    while (p->pszPronunciation[j] != 0)
                    {
						// process each phoneme
                        phn[0] = p->pszPronunciation[j];
                        m_phnCvt->IdToPhone(phn, phone_buffer);
                        al.m_phonemes.push_back(phone_buffer);
                        j++;
                    }
					// start time of the ortheme
                    al.m_msStart= msStart + bytes_to_milli(p->ulAudioStreamOffset);
					// end time of the ortheme
					al.m_msEnd = bytes_to_milli(p->ulAudioSizeBytes);
                    al.m_msEnd += al.m_msStart;
					// add it to the results
                    m_results.push_back(al);

					p++;
				}
			}
		}
		else if (event.eEventId == SPEI_END_SR_STREAM)
		{
			// This event occurs when the stream has finished processing.
            // we set a flag to indicate that things are done.
			m_bDone = TRUE;        
		}
    }
}
