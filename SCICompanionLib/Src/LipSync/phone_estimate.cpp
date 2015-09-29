/* 
phone_estimate.cpp

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
@file phone_estimate.cpp
@brief implementation of the phoneme timing estimator to improve results from SAPI 5.1

This file has the following data and constants
- SapiEnglish51. The default phoneme mapper data
- engine_phoneme_spec class implementation
- phoneme_estimator class implementation

@author mark zartler

@date   created oct 22 2005

**/
#include "stdafx.h"
#include <sphelper.h>
#include <comdef.h>
#include <string>
#include "sapi_lipsync.h"
#include "phone_estimate.h"
#include "sapi_util.h"


/** @brief This global is the default phoneme map
	
	It is used by phoneme_estimator if no other spec is provided.
	these were taken literally from the SAPI 5.1 documentation 
	Conversion to anno40 was trivial. 

	Other developers using different speech engines may need to
	create their own list like the SapiEnglish51 list and construct
	a phoneme_estimator based on their data.
*/
engine_phoneme_spec SapiEnglish51[] =
{
engine_phoneme_spec(L"-", L"x",  L"syllable", engine_phoneme_spec::silence),
engine_phoneme_spec(L"!", L"x", L"Sentence" , engine_phoneme_spec::silence),
engine_phoneme_spec(L"aa", L"AH", L"father", engine_phoneme_spec::voiced), 
engine_phoneme_spec(L"ae", L"AE", L"cat", engine_phoneme_spec::voiced), 
engine_phoneme_spec(L"ah", L"AH", L"cut", engine_phoneme_spec::voiced),
engine_phoneme_spec(L"ao", L"AO", L"dog", engine_phoneme_spec::dipthong),
engine_phoneme_spec(L"aw", L"AW", L"foul", engine_phoneme_spec::dipthong),
engine_phoneme_spec(L"ax", L"AH", L"ago", engine_phoneme_spec::voiced),
engine_phoneme_spec(L"ay", L"AY", L"bite", engine_phoneme_spec::voiced),
engine_phoneme_spec(L"b", L"b", L"big", engine_phoneme_spec::voiced),
engine_phoneme_spec(L"ch", L"CH", L"chin", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"d", L"d", L"dig", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"dh", L"DH", L"then", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"eh", L"EH", L"pet", engine_phoneme_spec::voiced),
engine_phoneme_spec(L"er", L"ER", L"fur", engine_phoneme_spec::dipthong),
engine_phoneme_spec(L"ey", L"EY", L"ate", engine_phoneme_spec::dipthong),
engine_phoneme_spec(L"f", L"f", L"fork", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"g", L"g", L"gut", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"h", L"h", L"help", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"ih", L"IH", L"fill", engine_phoneme_spec::silence),
engine_phoneme_spec(L"iy", L"IY", L"feel", engine_phoneme_spec::silence),
engine_phoneme_spec(L"jh", L"j", L"joy", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"k", L"k", L"cut", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"l", L"l", L"lid", engine_phoneme_spec::voiced),
engine_phoneme_spec(L"m", L"m", L"mat", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"n", L"n", L"no", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"ng", L"NG", L"sing", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"ow", L"OW", L"go", engine_phoneme_spec::dipthong),
engine_phoneme_spec(L"oy", L"OY", L"toy", engine_phoneme_spec::dipthong),
engine_phoneme_spec(L"p", L"p", L"put", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"r", L"r", L"red", engine_phoneme_spec::voiced),
engine_phoneme_spec(L"s", L"s", L"sit", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"sh", L"SH", L"she", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"t", L"t", L"talk", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"th", L"TH", L"thin", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"uh", L"UH", L"book", engine_phoneme_spec::dipthong),
engine_phoneme_spec(L"uw", L"UW", L"too", engine_phoneme_spec::dipthong),
engine_phoneme_spec(L"v", L"v", L"vat", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"w", L"w", L"with", engine_phoneme_spec::voiced),
engine_phoneme_spec(L"y", L"y", L"yard", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"z", L"z", L"zap", engine_phoneme_spec::unvoiced),
engine_phoneme_spec(L"zh", L"ZH", L"pleasure", engine_phoneme_spec::unvoiced),
// end marker
engine_phoneme_spec(L"", L"", L"", engine_phoneme_spec::silence)
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// phoneme_estimator implementation
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
	The default constructor uses the SapiEnglish51 as the model for the
	phonemes coming from SAPI. 
**/
phoneme_estimator::phoneme_estimator()
{
	this->m_pSpec = SapiEnglish51;
}

/** 
	This allows application to define their own engine_phoneme_spec list
	and assign it to the phoneme estimator.

	The estimator maintains a live pointer to the incoming spec list
	(not a copy) so the call must be sure the lifecyle of the spec list
	outlasts the lifecycle of this phoneme_estimator
	@param pSpec - [in] phoneme spec to use.
	@note The size of the spec (number of items) is determined by the 
	content of the last spec. if you look at SapiEnglish51 you'll see that
	there is an empty spec as the last item. This is used as a stop condition
	when search the spec, so it needs to be there in order to keep from crashing.
**/
phoneme_estimator::phoneme_estimator(engine_phoneme_spec* pSpec)
{
	m_pSpec = pSpec;
}

/**
The alignment result should have the alignment_results::m_phonemes
instantiated with the correct value, and it should have the start
and end time established for the word.

This method will calculate the alignment_results::m_phonemeStartTimes
given the specification for the phonemes. If there is a conversion process
in the engine_phoneme_spec list, that conversion will also be done.

This method uses no signal processing, the estimation is based on the
type of phoneme being timed out. 
@param align 
		-[in] the phoneme list and start and end time of the ortheme
		-[out] mapped phoneme list and estimated start and end times for the phonemes
**/
void phoneme_estimator::EstimatePhonemeAlignment(alignment_result& align)
{
	float actual_duration = float(align.m_msEnd - align.m_msStart);
	
	// silence, unvoiced = 30
	// voiced = 50
	// dipthong = 60
	// what we do is calculate the "optimal" time given the 30, 50, 60 rule
	// then look at the difference between the actual duration and the optimal
	// duration and scale the numbers by the difference. Very simplistic.
	std::vector<long> optimalEndTimes;
	unsigned long i;
	long lastEndTime = 0;
	for (i = 0; i < align.m_phonemes.size(); i++)
	{
		bool bFound = false;
		engine_phoneme_spec *p = m_pSpec;
		while (p->enginePhoneme.size() && !bFound)
		{
			if (align.m_phonemes[i] == p->enginePhoneme)
			{
				bFound = true;
				if (p->outputPhoneme.size())
				{  // swap the phoneme
					align.m_phonemes[i] = p->outputPhoneme;
				}
				switch (p->m_type)
				{
					
					case engine_phoneme_spec::voiced:
						optimalEndTimes.push_back(lastEndTime + 50);
						lastEndTime += 50;
						break;
					case engine_phoneme_spec::dipthong:
						optimalEndTimes.push_back(lastEndTime + 60);
						lastEndTime += 60;
						break;
					case engine_phoneme_spec::unvoiced:
					case engine_phoneme_spec::silence:						
					default:
						optimalEndTimes.push_back(lastEndTime + 30);
						lastEndTime += 30;
				}
			}
			else
				p++;
		}
		if (!bFound)
		{
			std::wcerr << L"Phoneme label" << align.m_phonemes[i].c_str() << L" not found in phoneme mapper" << std::endl; 
			optimalEndTimes.push_back(lastEndTime + 50);
			lastEndTime += 50;
		}
	}
	if (lastEndTime > 0)
	{
		align.m_phonemeEndTimes.clear(); // clear the results
		float opt_duration = (float)lastEndTime;
		float diff = actual_duration/opt_duration;
		for (i = 0; i < optimalEndTimes.size(); i++)
		{
			float act = optimalEndTimes[i] * diff;
			align.m_phonemeEndTimes.push_back(long(act) + align.m_msStart);
		}
	}
}


/**
This static method evenly spaces the phonemes across the word.

This is used when we don't have a model for the phonemes coming out
of SAPI 5.1, but we'd still like to generate some time alignments for
phonemes. It's actually not too bad.
	
This method will calculate the alignment_results::m_phonemeStartTimes
given the specification for the phonemes. If there is a conversion process
in the engine_phoneme_spec list, that conversion will also be done.

This method uses no signal processing, the estimation is based on the
type of phoneme being timed out. 
@param align 
		-[in] the phoneme list and start and end time of the ortheme
		-[out] mapped phoneme list and estimated start and end times for the phonemes
**/

void phoneme_estimator::TrivialPhonemeAlignment(alignment_result& align)
{
	float duration = float(align.m_msEnd - align.m_msStart);
	if (align.m_phonemes.size())
	{
		float inc = duration/float(align.m_phonemes.size());

		long pos = align.m_msStart;
		align.m_phonemeEndTimes.clear();
		for (unsigned long i = 0; i < align.m_phonemes.size(); i++)
		{
			long endTime = pos + (long)inc;
			align.m_phonemeEndTimes.push_back(pos);
			pos = endTime;
		}
	}
}