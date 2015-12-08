#include "stdafx.h"
#include "ParseAutoCompleteContext.h"

const ParseACChannels BlockAllChannels =
    ((uint32_t)ParseAutoCompleteContext::Block) |
    ((uint32_t)ParseAutoCompleteContext::Block << 8) |
    ((uint32_t)ParseAutoCompleteContext::Block << 16) | 
    ((uint32_t)ParseAutoCompleteContext::Block << 24);

const ParseACChannels NoChannels = 0;

ParseAutoCompleteContext ExtractChannel(ParseACChannels data, ParseAutoCompleteChannel channel)
{
    assert(channel != ParseAutoCompleteChannel::All);
    return (ParseAutoCompleteContext)((data >> (8 * (uint32_t)channel)) & 0xff);
}

ParseACChannels SetChannel(ParseACChannels existing, ParseAutoCompleteChannel channel, ParseAutoCompleteContext ac)
{
    if (channel == ParseAutoCompleteChannel::All)
    {
        for (int i = 0; i < 4; i++)
        {
            existing |= (((uint32_t)ac) << (8 * i));
        }
    }
    else
    {
        existing |= (((uint32_t)ac) << (8 * (uint32_t)channel));
    }
    return existing;
}