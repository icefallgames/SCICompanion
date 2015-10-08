#pragma once

// AudioWaveformUI, A control that shows an audio waveform
class alignment_result;
struct SyncComponent;

struct AudioComponent;

struct UILipSyncData
{
    uint32_t start;
    uint32_t stop;
    std::string phoneme;
};


class AudioWaveformUI : public CStatic
{
    DECLARE_DYNAMIC(AudioWaveformUI)

public:
    AudioWaveformUI(const std::string &label);
    AudioWaveformUI();
    virtual ~AudioWaveformUI();

    void SetResource(const AudioComponent *audio);
    void SetStreamPosition(uint32_t streamPosition);
    void SetRawLipSyncData(std::vector<alignment_result> &rawLipSyncData);
    void SetRawLipSyncData(const SyncComponent &sync);

private:
    afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;

    void _OnDraw(CDC *pDC, LPRECT prc);
    void _GenerateDoubleBuffer(CDC *pDC, LPRECT prc);
    void _DrawWaveform(CDC *pDC, LPRECT prc);

    DECLARE_MESSAGE_MAP()

    const AudioComponent *_audioComponent;

    uint32_t _streamPosition;
    std::unique_ptr<CBitmap> _pbitmapDoubleBuf;
    std::vector<UILipSyncData> _rawLipSyncData;
    CSize _sizeDoubleBuf;
    std::string _label;
};
