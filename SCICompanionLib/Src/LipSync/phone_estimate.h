/* 
phone_estimate.h

Copyright (C) 2005 Annosoft, LLC. Richardson, Texas. All rights reserved.  
	
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
@file phone_estimate.h
@brief declaration of the phoneme timing estimator to improve results from SAPI 5.1

SAPI can use various recognition engines, some from 3rd parties.
If You use "Microsoft English Recognizer 5.1" then the default set, supplied
in the code will work. Otherwise, you'll need to figure out the phoneme
labels of your speech recognizer and create a new set of spacing rules here.

So that we can view the results in the lipsync tool, we also provide
a way to map phonemes into the anno 40 set.  I like being able to view the results.

Each phoneme mapper object has the engine phoneme, the output phoneme, an
optional descripton, and a phoneme type. 

Since SAPI 5.1 does not provide alignments for phonemes within a word,
we have to do it ourself. The strategy is fairly simple, we spread out
the phonemes according to the classification, where unvoiced phonemes
have a short hold compared to voiced phonemes, dipthongs hold the longest.

**/

#ifndef _H_PHONE_CVT
#define _H_PHONE_CVT

// forward declaration
class engine_phoneme_spec;

/**
@brief the phoneme estimator class is used to estimate the
timing of phonemes in a word.

SAPI 5.1 does not provide phoneme timing details. we must estimate them.

For the english version, we have pulled a list of phonemes from the SAPI SDK
documentation, we assign them values based on their voicing characteristics
and use this information to spread the phonetical results in a way that
should be similar to normal speech patterns.

The phoneme estimator abstracts that process. the software will still work
without a specification for all the phonemes in the engine, but it can't
use heuristics to generate the timing information, instead it will just
evenly spread the phonemes across the word. 

The default phoneme_estimator uses the SAPI English set. 

The phoneme estimator also has the ability to transform the phonemes
based on information in the engine_phoneme_spec. This is kind of a hack.
I want to transform the phonemes into the \ref anno40 so that they can
be better viewed in \ref lipsync_tool. Other implementations may want
to change this.
  
@todo
It would be nice to load the engine_phoneme_spec from an auxillary file
so that we wouldn't need to change the code to support other SAPI speech 
engines
@see ::run_sapi_textbased_lipsync for example code
@see ::run_sapi_textless_lipsync for example code
**/
class phoneme_estimator
{
public:
	/// default constructor. Assumes SAPI 5.1 english phoneme set
	phoneme_estimator();

	/// construct the phoneme estimator with the list of expected phonemes.
	phoneme_estimator(engine_phoneme_spec* pSpec);
	
	/// This method estimates the phoneme durations given an alignment result
	void EstimatePhonemeAlignment(alignment_result& align);

	/// This static method estimates the phoneme durations given an alignment result
	static void TrivialPhonemeAlignment(alignment_result& align);
protected:
	/// the list of phonemes we expect to see in EstimatePhonemeAlignment
	engine_phoneme_spec* m_pSpec;
};


/** 
@brief This data class describes an engine phoneme.

As described above, in order to produce better spreading of phonemes
within the word alignment, we can use heuristics, but in order to
use them, we need some information about the phoneme, whether it
is voiced, unvoiced, a dipthong, etc. These classes typically are
the same duration. By classifying the phoneme, we are able to do
a little better job. It is not required though.

Additionally, the spec provides a mapping. This is somewhat of a hack,
I want to be able to map everything to the \ref anno40 so that they
display better in the the lipsync tool. Again, this can be adjusted to taste
@todo provide a way to read these in from an external file.
*/
class engine_phoneme_spec
{
public:
	/// SAPI engine phoneme
	std::wstring enginePhoneme;
	/// output phoneme
	std::wstring outputPhoneme;
	/// description
	std::wstring desc;

	/** @brief type of phoneme  
		
		When we go to pick durations for phonemes we take the
		type of phoneme into account, generally unvoiced phonemes
		have the shortest hold, followed by voiced phonemes, and
		dipthongs generally hold the longest */
	typedef enum phoneme_type
	{
		silence,	///< phoneme is a silence phoneme
		unvoiced,	///< phoneme is an unvoiced phoneme
		voiced,		///< phoneme is a voiced phoneme
		dipthong	///< phoneme is a dipthong (two voiced sounds)
	};
	/// the type of phoneme one of engine_phoneme_spec::phoneme_type
	phoneme_type m_type;

	/// paramaterized constructor
	engine_phoneme_spec(const wchar_t * _enginePhn, wchar_t * _outputPhn, wchar_t* _desc, phoneme_type _pt)
	{
		enginePhoneme = _enginePhn;
		outputPhoneme = _outputPhn;
		desc = _desc;
		m_type = _pt;		
	}
};

#endif