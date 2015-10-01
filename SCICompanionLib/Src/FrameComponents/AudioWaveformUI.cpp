#include "stdafx.h"
#include "AudioWaveformUI.h"
#include "Audio.h"
#include "ResourceEntity.h"
#include <sphelper.h>
#include "sapi_lipsync.h"
#include <locale>
#include <codecvt>

using namespace std;

IMPLEMENT_DYNAMIC(AudioWaveformUI, CStatic)

// AudioWaveformUI control

AudioWaveformUI::~AudioWaveformUI() {}

AudioWaveformUI::AudioWaveformUI()
    : CStatic(), _streamPosition(0)
{
}

BEGIN_MESSAGE_MAP(AudioWaveformUI, CStatic)
    ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()

COLORREF g_phonemeColors[] =
{
    RGB(196, 196, 255),
    RGB(196, 255, 196),
    RGB(196, 255, 255),
    RGB(255, 196, 255),
    RGB(255, 255, 196),
    RGB(255, 255, 255),
};

void AudioWaveformUI::SetResource(const ResourceEntity *audio)
{
    _audioResource = audio;
    Invalidate(FALSE);
}

void AudioWaveformUI::SetStreamPosition(uint32_t streamPosition)
{
    _streamPosition = streamPosition;
    Invalidate(FALSE);
}

void AudioWaveformUI::SetRawLipSyncData(std::vector<alignment_result> &rawLipSyncData)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    _pbitmapDoubleBuf = nullptr;    // Trigger redraw.
    _rawLipSyncData.clear();
    for (auto &result : rawLipSyncData)
    {
        long prevTime = result.m_msStart;
        for (size_t i = 0; i < result.m_phonemes.size(); i++)
        {

            UILipSyncData uiData = { prevTime, result.m_phonemeEndTimes[i], converter.to_bytes(result.m_phonemes[i]) };
            _rawLipSyncData.push_back(uiData);
            prevTime = result.m_phonemeEndTimes[i];
        }

    }
    Invalidate(FALSE);
}

void AudioWaveformUI::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    LPRECT prc = &lpDrawItemStruct->rcItem;
    CDC dc;
    dc.Attach(lpDrawItemStruct->hDC);

    _GenerateDoubleBuffer(&dc, prc);

    CDC dcMem;
    if (dcMem.CreateCompatibleDC(&dc))
    {
        if (_pbitmapDoubleBuf)
        {
            HGDIOBJ hOldBmp = dcMem.SelectObject(_pbitmapDoubleBuf.get());

            // Copy into the final buffer:
            dc.StretchBlt(0, 0, RECTWIDTH(*prc), RECTHEIGHT(*prc), &dcMem, 0, 0, RECTWIDTH(*prc), RECTHEIGHT(*prc), SRCCOPY);

            dcMem.SelectObject(hOldBmp);
        }

        if (_audioResource)
        {
            const AudioComponent &audio = _audioResource->GetComponent<AudioComponent>();
            if (audio.GetLength() > 0)
            {
                uint32_t bar = _streamPosition * RECTWIDTH(*prc) / audio.GetLength();

                CPen pen(PS_SOLID, 1, RGB(0, 128, 0));
                HGDIOBJ hOld = dc.SelectObject(pen);
                dc.MoveTo(bar, prc->top);
                dc.LineTo(bar, prc->bottom);
                dc.SelectObject(hOld);
            }
        }
    }

    dc.Detach();
}

void AudioWaveformUI::_DrawWaveform(CDC *pDC, LPRECT prc)
{
    const AudioComponent &audio = _audioResource->GetComponent<AudioComponent>();
    CDC dcMem;
    if (dcMem.CreateCompatibleDC(pDC))
    {
        HGDIOBJ hOldBmp = dcMem.SelectObject(_pbitmapDoubleBuf.get());

        dcMem.FillSolidRect(prc, RGB(196, 196, 196));

        int blockAlign = IsFlagSet(audio.Flags, AudioFlags::SixteenBit) ? 2 : 1;
        int sampleCount = audio.GetLength() / blockAlign;
        std::vector<CPoint> points;
        std::vector<DWORD> pointCounts;
        points.reserve(200);
        pointCounts.reserve(100);
        int yMiddle = (prc->top + prc->bottom) / 2;
        int height = RECTHEIGHT(*prc);
        int width = RECTWIDTH(*prc);
        std::unique_ptr<int[]> waveFormMax = std::make_unique<int[]>(width);
        std::unique_ptr<int[]> waveFormMin = std::make_unique<int[]>(width);
        int binSize = (sampleCount + (width - 1)) / width;  // "round up"
        for (int i = 0; i < sampleCount; i++)
        {
            int value = (int)audio.DigitalSamplePCM[i] - 128; // Normalize
            waveFormMax[i / binSize] = max(waveFormMax[i / binSize], value);
            waveFormMin[i / binSize] = min(waveFormMin[i / binSize], value);
        }
        // TODO: support for 16 bits

        for (int x = 0; x < width; x++)
        {
            // Add a line from y to -y the middle.
            int yTop = waveFormMax[x] * height / 128;
            int yBottom = waveFormMin[x] * height / 128;
            points.emplace_back(x, yMiddle + yTop);
            points.emplace_back(x, yMiddle + yBottom);
            pointCounts.push_back(2);
        }

        // Finally a horizontal line
        points.emplace_back(0, yMiddle);
        points.emplace_back(width, yMiddle);
        pointCounts.push_back(2);

        if (!points.empty())
        {
            CPen pen(PS_SOLID, 1, RGB(0, 0, 128));
            HGDIOBJ hOld = dcMem.SelectObject(pen);
            dcMem.PolyPolyline(&points[0], &pointCounts[0], (int)pointCounts.size());
            dcMem.SelectObject(hOld);
        }

        // If we have lipsync data, draw that.
        long ms = audio.GetLength() * 1000 / audio.Frequency;
        int colorIndex = 0;
        dcMem.SetTextColor(RGB(0, 0, 0));
        dcMem.SetBkMode(OPAQUE);
        CRect rcMeasure = {};
        dcMem.DrawText("M", &rcMeasure, DT_SINGLELINE | DT_CALCRECT);
        for (auto &phoneme : _rawLipSyncData)
        {
            // To start with, just draw them.
            int xStart = phoneme.start * width / ms;
            int xEnd = phoneme.stop * width / ms;
            dcMem.SetBkColor(g_phonemeColors[colorIndex]);
            colorIndex++;
            colorIndex %= ARRAYSIZE(g_phonemeColors);

            CRect rcText = { xStart, 0, xEnd, RECTHEIGHT(rcMeasure) };
            dcMem.DrawText(phoneme.phoneme.c_str(), &rcText, DT_SINGLELINE);
        }

        dcMem.SelectObject(hOldBmp);
    }
}

void AudioWaveformUI::_GenerateDoubleBuffer(CDC *pDC, LPRECT prc)
{
    CSize sizeCurrent = CSize(RECTWIDTH(*prc), RECTHEIGHT(*prc));
    if (!_pbitmapDoubleBuf || (_sizeDoubleBuf != sizeCurrent))
    {
        if (_audioResource)
        {
            _sizeDoubleBuf = sizeCurrent;
            _pbitmapDoubleBuf = make_unique<CBitmap>();
            // Hope this succeededs...
            if (_pbitmapDoubleBuf->CreateCompatibleBitmap(pDC, _sizeDoubleBuf.cx, _sizeDoubleBuf.cy))
            {
                _DrawWaveform(pDC, prc);
            }
        }
    }
}

