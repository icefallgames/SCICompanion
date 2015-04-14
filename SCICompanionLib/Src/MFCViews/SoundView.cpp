// TextView.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "SoundView.h"
#include "SoundDoc.h"
#include "MidiPlayer.h"

// CSoundView

using namespace std;

CExtTempoSlider *g_pSoundTempoSlider = nullptr;


void DragEntry::OnDragMove(CPoint pt)
{
    // Invalidate old
    CRect rect1 = GetRect();
    _pView->InvalidateRect(&rect1);
    _ptDragged = pt; // Set it
    // Invalidate new
    CRect rect2 = GetRect();
    _pView->InvalidateRect(&rect2);
    UpdateToolTip(true);
}

void DragEntry::UpdateToolTip(bool fShow)
{
    if (fShow)
    {
        CPoint pt;
        if (::GetCursorPos(&pt))
        {
            pt.Offset(0, -30);
        }
        _pView->ToolTip().Show(pt, TipText());
    }
    else
    {
        _pView->ToolTip().Hide();
    }
}

class LoopDragEntry : public DragEntry
{
public:
    LoopDragEntry(CSoundView *pView, CPoint pt, int iconIndex) : DragEntry(pView, pt, iconIndex) {}
    virtual void OnDragOver()
    {
        DWORD dwTicks = _pView->XToTicks(_ptDragged.x + _ptDeltaOrig.x);
        UpdateToolTip(false);
        // NOTE: This SetLoopPoint will cause us to be destroyed, so we'd better not
        // run any code after this.
        // REVIEW: We could refcount this object to avoid this.
        _pView->GetDocument()->ApplyChanges<SoundComponent>(
            [dwTicks](SoundComponent &sound)
            {
                return WrapHint(sound.SetLoopPoint(dwTicks));
            }
            );
    }
    virtual void OnDragStart(CPoint pt)
    {
        _ptDeltaOrig = _ptOrig - pt;
        _ptDragged = pt;
        _pView->ToolTip().SetColor(RGB(200, 255, 200));
        UpdateToolTip(true);
    }
    virtual CRect GetRect()
    {
        DWORD ticks = (DWORD)_pView->XToTicks(_ptDragged.x + _ptDeltaOrig.x);
        return _pView->GetLoopMarkerRect(ticks);
    }
    virtual std::string TipText()
    {
        char sz[100];
        StringCchPrintf(sz, ARRAYSIZE(sz), "%d", _pView->XToTicks(_ptDragged.x + _ptDeltaOrig.x));
        return sz;
    }
    virtual bool IsEnabled()
    {
        bool enabled = false;
        CSoundDoc *pDoc = _pView->GetDocument();
        if (pDoc)
        {
            SoundComponent *pSound = pDoc->GetSoundComponent();
            if (pSound)
            {
                enabled = (pSound->GetLoopPoint() != SoundComponent::LoopPointNone);
            }
        }
        return enabled;
    }
};

class CueDragEntry : public DragEntry
{
public:
    CueDragEntry(CSoundView *pView, CPoint pt, int iconIndex, int activeIconIndex, size_t cueIndex, CuePoint cue)
        : DragEntry(pView, pt, iconIndex), _cueIndex(cueIndex), _cueCopy(cue), _activeIconIndex(activeIconIndex), _fDrag(false) {}
    virtual void OnDragOver()
    {
        _fDrag = false;
        DWORD dwTicks = _pView->XToTicks(_ptDragged.x + _ptDeltaOrig.x);
        _cueCopy.SetTickPos(dwTicks);
        UpdateToolTip(false);

        _pView->GetDocument()->ApplyChanges<SoundComponent>(
            [&](SoundComponent &sound)
        {
            return WrapHint(sound.SetCue(_cueIndex, _cueCopy));
        }
        );
    }
    virtual void OnDragStart(CPoint pt)
    {
        _ptDeltaOrig = _ptOrig - pt;
        _ptDragged = pt;
        _fDrag = true;
        _pView->ToolTip().SetColor(RGB(255, 200, 200));
        UpdateToolTip(true);
        // Also set the active cue.
        _pView->GetDocument()->SetActiveCue(static_cast<int>(_cueIndex));
    }
    virtual CRect GetRect()
    {
        DWORD ticks = (DWORD)_pView->XToTicks(_ptDragged.x + _ptDeltaOrig.x);
        return _pView->GetCueMarkerRect(ticks);
    }
    virtual std::string TipText()
    {
        char sz[100];
        StringCchPrintf(sz, ARRAYSIZE(sz), "%d: %d", _pView->XToTicks(_ptDragged.x + _ptDeltaOrig.x), _cueCopy.GetValue());
        return sz;
    }
    virtual int GetIconIndex()
    {
        return _fDrag ? _activeIconIndex : __super::GetIconIndex();
    }
private:
    size_t _cueIndex;
    int _activeIconIndex;
    CuePoint _cueCopy;
    bool _fDrag;
};

#define TRACK_MARGIN 40
#define BOTTOM_MARGIN (18 + 18) // Room for loop and cues

IMPLEMENT_DYNCREATE(CSoundView, CView)

CSoundView::CSoundView()
{
    _cursorPos = 0;
    _channelHover = -1;
    _imageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 5);
    CBitmap bitmapStock1;
    if (bitmapStock1.LoadBitmap(IDB_BITMAPLOOP))
    {
        _loopBitmapIndex = _imageList.Add(&bitmapStock1, RGB(255, 0, 255));
    }
    CBitmap bitmapStock2;
    if (bitmapStock2.LoadBitmap(IDB_BITMAPCUE))
    {
        _cueBitmapIndex = _imageList.Add(&bitmapStock2, RGB(255, 0, 255));
    }
    CBitmap bitmapStock3;
    if (bitmapStock3.LoadBitmap(IDB_BITMAPCUEACTIVE))
    {
        _activeCueBitmapIndex = _imageList.Add(&bitmapStock3, RGB(255, 0, 255));
    }
    _currentDragIndex = -1;
}

int CSoundView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    _toolTip.CreateEx(0, _T("STATIC"), NULL, SS_NOTIFY | SS_OWNERDRAW | WS_POPUP | WS_CLIPSIBLINGS | WS_BORDER, 0, 0, 100, 100, GetSafeHwnd(), NULL);
    return 0;
}

void CSoundView::_ClearDrags()
{
    _drags.clear();
    _currentDragIndex = -1;
}

BEGIN_MESSAGE_MAP(CSoundView, CView)
    ON_COMMAND(ID_PLAY, OnPlay)
    ON_COMMAND(ID_PAUSE, OnPause)
    ON_COMMAND(ID_STOP, OnStop)
    ON_UPDATE_COMMAND_UI(ID_PLAY, OnUpdatePlay)
    ON_UPDATE_COMMAND_UI(ID_STOP, OnUpdateStop)
    ON_UPDATE_COMMAND_UI(ID_PAUSE, OnUpdatePause)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_MOUSEMOVE()
    ON_UPDATE_COMMAND_UI(ID_TEMPOSLIDER, OnUpdateTempoSlider)
END_MESSAGE_MAP()

void CSoundView::_OnButtonDown(CPoint point)
{
    ASSERT(_CurrentDrag() == NULL);
    _currentDragIndex = _HitTestDraggableThing(point);
    if (_CurrentDrag())
    {
        _CurrentDrag()->OnDragStart(point);
        _InvalidateCurrentDragCursor();
        SetCapture();
    }
    else
    {
        int channel = _HitTestChannelHeader(point);
        if (channel == -1)
        {
            // Maybe the user is trying to set the cursor pos
            CRect rect;
            GetClientRect(&rect);
            rect.left += TRACK_MARGIN;
            point.x = max(point.x, TRACK_MARGIN);
            g_midiPlayer.CuePosition(point.x - TRACK_MARGIN, rect.Width());
            _SetCursorPos();
        }
        else
        {
            // Toggle this channel
            CSoundDoc *pDoc = GetDocument();
            const SoundComponent *pSound = GetSoundComponent();
            WORD wMask = pSound->GetChannelMask(pDoc->GetDevice());
            wMask ^= (0x1 << channel); // Toggle our channel

            pDoc->ApplyChanges<SoundComponent>(
                [pDoc, wMask](SoundComponent &sound)
                {
                    return WrapHint(sound.SetChannelMask(pDoc->GetDevice(), wMask));
                }
                );

            _InvalidateChannelHeaders();
        }
    }
}

void CSoundView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    _OnButtonDown(point);
    __super::OnLButtonDblClk(nFlags, point);
}

void CSoundView::OnLButtonDown(UINT nFlags, CPoint point)
{
    _OnButtonDown(point);
    __super::OnLButtonDown(nFlags, point);
}

void CSoundView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (_CurrentDrag())
    {
        // The drag is over.
        _InvalidateCurrentDragCursor();
        _CurrentDrag()->OnDragOver();
        ReleaseCapture();
        _currentDragIndex = -1;
    }
    __super::OnLButtonDown(nFlags, point);
}

void CSoundView::_InvalidateChannelHeaders()
{
    CRect rect;
    GetClientRect(&rect);
    rect.right = rect.left + TRACK_MARGIN;
    InvalidateRect(&rect);
}

void CSoundView::_InvalidateCurrentDragCursor()
{
    CRect rectDrag = _CurrentDrag()->GetRect();
    CPoint ptCenter = rectDrag.CenterPoint();
    CRect rectClient;
    GetClientRect(&rectClient);
    CRect rect(ptCenter.x, rectClient.top, ptCenter.x + 1, rectDrag.top);
    InvalidateRect(&rect);
}

void CSoundView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (_CurrentDrag())
    {
        // We're dragging.
        _InvalidateCurrentDragCursor();
        _CurrentDrag()->OnDragMove(point);
        _InvalidateCurrentDragCursor();
    }
    else
    {
        int channel = _HitTestChannelHeader(point);
        if (channel != _channelHover)
        {
            _channelHover = channel;
            _InvalidateChannelHeaders();
        }
    }
    __super::OnMouseMove(nFlags, point);
}

bool CSoundView::_PrepTrackBitmaps(int width, int height)
{
    bool fRet = true;
    CDC *pDC = GetDC();
    for (int i = 0; fRet && i < ARRAYSIZE(_channelBitmaps) ; i++)
    {
        if ((HBITMAP)_channelBitmaps[i])
        {
            DeleteObject(_channelBitmaps[i].Detach());
        }
        fRet = (_channelBitmaps[i].CreateCompatibleBitmap(pDC, width, height) == TRUE);
    }
    ReleaseDC(pDC);
    return fRet;
}

BYTE _GetLoudest(BYTE *pByte)
{
    BYTE b = 0;
    for (int i = 0; i < 128; i++)
    {
        if (pByte[i] > b)
        {
            b = pByte[i];
        }
    }
    return b;
}

CRect CSoundView::_GetBottomMargin()
{
    CRect rect;
    GetClientRect(&rect);
    rect.left += TRACK_MARGIN;
    rect.top = rect.bottom - BOTTOM_MARGIN;
    return rect;
}
CRect CSoundView::_GetLeftMargin()
{
    CRect rect;
    GetClientRect(&rect);
    rect.right = rect.left + TRACK_MARGIN;
    rect.bottom -= BOTTOM_MARGIN;    
    return rect;
}
CRect CSoundView::_GetTrackArea()
{
    CRect rect;
    GetClientRect(&rect);
    rect.left += TRACK_MARGIN;
    rect.bottom -= BOTTOM_MARGIN;
    return rect;
}
int CSoundView::_GetTrackHeight(int track)
{
    return _GetTrackY(track + 1) - _GetTrackY(track);
}
int CSoundView::_GetTrackY(int track)
{
    CRect rect = _GetTrackArea();
    return rect.top + track * rect.Height() / ARRAYSIZE(_channelBitmaps);
}

void CSoundView::_RecalculateTrackBitmaps()
{
    const SoundComponent *pSound = GetSoundComponent();
    if (pSound)
    {
        // Make a complete copy of the resource, since we're going to modify it while generating our bitmaps.
        SoundComponent sound = *pSound;

        WORD wChannelMask = sound.GetChannelMask(GetDocument()->GetDevice());

        // Make it so it has as many ticks as we have pixels.
        CRect rect = _GetTrackArea();
        DWORD width = max(1, rect.Width() - 1);
        sound.NormalizeToTicks(width);
        assert(width >= sound.GetTotalTicks());

        int height = (rect.Height() * 2 / 3) / 15;
        if (_PrepTrackBitmaps(width, height))
        {
            // We need some arrays to determine which notes are on on which track.
            BYTE vNote[15][128]; // 15 tracks, 128 possible notes on each.  This array holds the velocity value.
            ZeroMemory(vNote, sizeof(vNote));
            BYTE vNoteMax[15][128]; // 15 tracks, 128 possible notes on each.  This array holds the velocity value.
            ZeroMemory(vNoteMax, sizeof(vNoteMax));
            DWORD dwPos = 0;    // our current position
            std::vector<BYTE> channelValues[15];  // the data we'll eventually put in the bitmaps
            for (size_t i = 0; i < ARRAYSIZE(channelValues); i++)
            {
                channelValues[i].resize(width, 255); // 255 is a sentinel value indicating undefined
            }

            const std::vector<SoundEvent> &events = sound.GetEvents();
            for (size_t i = 0; i < events.size(); i++)
            {
                const SoundEvent &event = events[i];
                DWORD dwPosOld = dwPos;
                dwPos += event.wTimeDelta;
                if (dwPos > dwPosOld)
                {
                    // We moved a bit... initialize the "max" values with the current.
                    CopyMemory(vNoteMax, vNote, sizeof(vNoteMax));
                }
                BYTE channel = event.GetChannel();
                if ((channel < 15) && ((event.GetCommand() == SoundEvent::NoteOn) || (event.GetCommand() == SoundEvent::NoteOff)))
                {
                    BYTE note = event.bParam1;
                    if (note < 128)
                    {
                        BYTE velocity = 0;
                        if (!event.IsEffectiveNoteOff())
                        {
                            velocity = event.bParam2;
                        }
                        vNote[channel][note] = velocity;
                        if (velocity > vNoteMax[channel][note])
                        {
                            vNoteMax[channel][note] = velocity;
                        }
                        BYTE loudest = _GetLoudest(vNoteMax[channel]);
                        //ASSERT(loudest == 0);

                        if (dwPos < channelValues[channel].size())
                        {
                            // We have a bug in our calcualtions here ^^. Either in GetTotalTicks or whatever.
                            // But dwPos can be equal to GetTotalTicks, which is not good. e.g. SQ3 sound 0.

                            BYTE current = channelValues[channel][dwPos];
                            if ((current == 255) || (loudest > current))
                            {
                                channelValues[channel][dwPos] = loudest;
                            }
                        }
                    }
                }
            }

            // Now we create the bitmaps!
            for (int channel = 0; channel < ARRAYSIZE(_channelBitmaps); channel++)
            {
                CDC *pDC = GetDC();
                CDC dcMem;
                if (dcMem.CreateCompatibleDC(pDC))
                {
                    HGDIOBJ hOld = dcMem.SelectObject(_channelBitmaps[channel]);
                    dcMem.FillSolidRect(0, 0, width, height, ColorTrackBackground);

                    BYTE loudness = 0;
                    for (size_t posStart = 0; posStart < channelValues[channel].size(); )
                    {
                        loudness = channelValues[channel][posStart];
                        if (loudness == 255)
                        {
                            loudness = 0;
                        }
                        size_t posEnd = posStart;
                        while (posEnd < channelValues[channel].size() &&
                               (channelValues[channel][posEnd] == 255 ||
                               channelValues[channel][posEnd] == loudness))
                        {
                            posEnd++;
                        }
                        // If we got here, it is time to draw.
                        int cy = max(1, height * loudness / 128);
                        COLORREF cr = (wChannelMask & (0x1 << channel)) ? ColorActiveTrack : ColorSilentTrack;
                        dcMem.FillSolidRect((int)posStart, height - cy, (int)(posEnd - posStart), cy, cr);
                        posStart = posEnd;
                    }

                    dcMem.SelectObject(hOld);
                }
                ReleaseDC(pDC);
            }
        }
    }
}

void CSoundView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        const SoundComponent *pSound = pDoc->GetSoundComponent();
        if (pSound)
        {
            SoundChangeHint hint = GetHint<SoundChangeHint>(pHint);

            if (IsFlagSet(hint, SoundChangeHint::Changed | SoundChangeHint::DeviceChanged))
            {
                g_midiPlayer.SetDevice(pDoc->GetDevice());
                DWORD dwTicks = g_midiPlayer.QueryPosition();
                g_midiPlayer.SetSound(*pSound, pDoc->GetTempo());
                g_midiPlayer.CueTickPosition(dwTicks);
                _RecalculateTrackBitmaps();
                _RedoDrags();
                ClearFlag(hint, SoundChangeHint::CueChanged); // So we don't redo drags again below.
                _SetCursorPos();
                Invalidate(FALSE);
            }

            if (IsFlagSet(hint, SoundChangeHint::CueChanged))
            {
                // TODO: invalidate a smaller region.
                _RedoDrags();
                Invalidate(FALSE);
            }

            if (IsFlagSet(hint, SoundChangeHint::ActiveCueChanged))
            {
                // TODO: invalidate a smaller region.
                Invalidate(FALSE);
            }
        }
    }
}

//
// Prevent flicker
//
BOOL CSoundView::OnEraseBkgnd(CDC *pDC)
{
    return TRUE;
}

void CSoundView::SetTempo(WORD wTempo)
{
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        if (pDoc->GetSoundComponent()->CanChangeTempo())
        {
            pDoc->ApplyChanges<SoundComponent>(
                [wTempo](SoundComponent &sound)
                {
                    return WrapHint(sound.SetTempo(wTempo));
                }
                );
            g_midiPlayer.SetTempo(wTempo);
        }
        // REVIEW: have some message when you can't change tempo?
    }
}
WORD CSoundView::GetTempo() const
{
    WORD w = 120;
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        w = pDoc->GetTempo();   
    }
    return w;
}


void CSoundView::_OnDrawTrackHeader(CDC *pDC, int channel, bool fChannelOn)
{
    CRect rect = _GetLeftMargin();
    rect.left += 2;
    rect.top = _GetTrackY(channel);
    rect.bottom = rect.top + _GetTrackHeight(channel);

    char sz[10];
    StringCchPrintf(sz, ARRAYSIZE(sz), "%d", channel + 1);

	CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pmid(
		this, // Sketchy
        (channel == _channelHover) ? (fChannelOn ? CExtPaintManager::BOX_MOUSE_HOVER_CHECKED : CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED) : (fChannelOn ? CExtPaintManager::BOX_CHECKED : CExtPaintManager::BOX_UNCHECKED),
		false,
		false,
		rect,
		sz,
		(channel == _channelHover),
		false,
		true,
		false,
		CExtPaintManager::__ALIGN_HORIZ_LEFT | CExtPaintManager::__ALIGN_VERT_CENTER);
    g_PaintManager->PaintCheckButton(*pDC, _pmid);
}

void CSoundView::OnDraw(CDC *pDC)
{
    WORD wMask = 0;
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        
        wMask = pDoc->GetSoundComponent()->GetChannelMask(pDoc->GetDevice());
    }

    CRect rectClient;
    GetClientRect(&rectClient);

    CRect rect = _GetTrackArea();
    int cyTrack = rect.Height() / ARRAYSIZE(_channelBitmaps);
    int yRemainder = cyTrack * ARRAYSIZE(_channelBitmaps);
    for (int i = 0; i < ARRAYSIZE(_channelBitmaps); i++)
    {
        int yTrack = _GetTrackY(i);
        int cyTrack = _GetTrackHeight(i);
        CRect rectTrack(rect.left, yTrack, rect.right, yTrack + cyTrack);
        pDC->FillSolidRect(rectClient.left, rectTrack.top, rectClient.Width(), rectTrack.Height(), ColorTrackBackground);

        _OnDrawTrackHeader(pDC, i, (((0x1 << i) & wMask) != 0));

        CDC dcMem;
        if (dcMem.CreateCompatibleDC(pDC))
        {
            HGDIOBJ hOld = dcMem.SelectObject(_channelBitmaps[i]);
            BOOL fSuccess = BitBlt(*pDC, rectTrack.left, rectTrack.top, rectTrack.Width(), rectTrack.Height(), dcMem, 0, 0, SRCCOPY);
            dcMem.SelectObject(hOld);
        }
    }

    CRect rectBottom = _GetBottomMargin();
    pDC->FillSolidRect(rectClient.left, rectBottom.top, rectClient.Width(), rectBottom.Height(), ColorTrackBackground);

    // TODO: draw some sort of well.

    // Cursor
    pDC->FillSolidRect(rectClient.left + _cursorPos, rect.top, 1, rect.Height(), ColorCursor);

    // Loop and cur markers.
    for (auto &drag : _drags)
    {
        if (drag->IsEnabled())
        {
            CRect rectLoop = drag->GetRect();
            _imageList.Draw(pDC, drag->GetIconIndex(), CPoint(rectLoop.left, rectLoop.top), 0);
        }
    }

    // Cursor for a loop/cue drag
    if (_CurrentDrag() && _CurrentDrag()->IsEnabled())
    {
        CRect rectDrag = _CurrentDrag()->GetRect();
        CPoint ptCenter = rectDrag.CenterPoint();
        CPen pen(PS_DOT, 1, RGB(0, 0, 0));
        int nMode = pDC->SetBkMode(TRANSPARENT);
        HGDIOBJ hOld = pDC->SelectObject(pen);
        pDC->MoveTo(ptCenter.x, rect.top);
        pDC->LineTo(ptCenter.x, rectDrag.top);
        pDC->SelectObject(hOld);
        pDC->SetBkMode(nMode);
    }
}

const SoundComponent* CSoundView::GetSoundComponent()
{
    const SoundComponent *pSound = nullptr;
    CSoundDoc *pDoc = GetDocument();
    if (pDoc)
    {
        const ResourceEntity *pResource = pDoc->GetResource();
        if (pResource)
        {
            pSound = pResource->TryGetComponent<SoundComponent>();
        }
    }
    return pSound;
}

#define SOUND_TIMER 5003

void CSoundView::OnPlay()
{
    g_midiPlayer.Play();
    SetTimer(SOUND_TIMER, 100, NULL);
}
void CSoundView::OnStop()
{
    g_midiPlayer.Stop();
    _SetCursorPos();
    KillTimer(SOUND_TIMER);
}

void CSoundView::OnPause()
{
    g_midiPlayer.Pause();
    KillTimer(SOUND_TIMER);
}

void CSoundView::OnUpdatePlay(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(g_midiPlayer.CanPlay());
}
void CSoundView::OnUpdateStop(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(g_midiPlayer.CanStop());
}
void CSoundView::OnUpdatePause(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(g_midiPlayer.CanPause());
}

void CSoundView::_SetCursorPos()
{
    CRect rect = _GetTrackArea();
    int cursorPos = g_midiPlayer.QueryPosition(rect.Width()) + rect.left;
    if (cursorPos != _cursorPos)
    {
        CRect rectClient;
        GetClientRect(&rectClient);
        CRect rectInvalid(_cursorPos, 0, _cursorPos + 1, rectClient.Height());
        InvalidateRect(&rectInvalid);
        _cursorPos = cursorPos;
        rectInvalid.left = _cursorPos;
        rectInvalid.right = _cursorPos + 1;
        InvalidateRect(&rectInvalid);
    }
}

int CSoundView::_HitTestChannelHeader(CPoint pt)
{
    if (pt.x < (TRACK_MARGIN - 5)) // 5: provide a little "dead space" between margin and track.
    {
        CRect rect;
        GetClientRect(&rect);
        rect.bottom -= BOTTOM_MARGIN;
        int cyTrack = rect.Height() / ARRAYSIZE(_channelBitmaps);
        int channel = pt.y / cyTrack;
        return (channel < ARRAYSIZE(_channelBitmaps)) ? channel : -1;
    }
    return -1;
}

bool CSoundView::_IsInLoopTrack(CPoint pt)
{
    CRect rect;
    GetClientRect(&rect);
    rect.top = rect.bottom - 16;
    return !!rect.PtInRect(pt);
}
bool CSoundView::_IsInCueTrack(CPoint pt)
{
    CRect rect;
    GetClientRect(&rect);
    rect.top = rect.bottom - 16;
    return !!rect.PtInRect(pt);
}

int CSoundView::_CuePointInViewCoordinates(const CuePoint &cue)
{
    int dwTicks = (int)GetSoundComponent()->GetTotalTicks();
    CRect rectBottom = _GetBottomMargin();
    int offset = rectBottom.Width() * ((int)cue.GetTickPos()) / dwTicks;
    return rectBottom.left + offset;
}

bool CSoundView::_LoopPointInViewCoordinates(int &x)
{
    DWORD dwLoop = GetSoundComponent()->GetLoopPoint();
    bool fRet = (dwLoop != SoundComponent::LoopPointNone);
    if (fRet)
    {
        int dwTicks = (int)GetSoundComponent()->GetTotalTicks();
        CRect rectBottom = _GetBottomMargin();
        int offset = rectBottom.Width() * ((int)dwLoop) / dwTicks;
        x = rectBottom.left + offset;
    }
    return fRet;
}


void CSoundView::_RedoDrags()
{
    _ClearDrags();
    const SoundComponent *pSound = GetSoundComponent();
    if (pSound)
    {
        // HACK: do cues first, so the active cue index corresponds to the _drags index for the cue.
        const std::vector<CuePoint> &cues = pSound->GetCuePoints();
        for (size_t i = 0; i < cues.size(); i++)
        {
            unique_ptr<DragEntry> newEntry(new CueDragEntry(this, GetCueMarkerRect(cues[i].GetTickPos()).CenterPoint(), _cueBitmapIndex, _activeCueBitmapIndex, i, cues[i]));
            _drags.push_back(move(newEntry));
        }

        DWORD dwTicks = pSound->GetLoopPoint();
        if (dwTicks == SoundComponent::LoopPointNone)
        {
            dwTicks = 0;
        }
        unique_ptr<DragEntry> newLoopEntry(new LoopDragEntry(this, GetLoopMarkerRect(dwTicks).CenterPoint(), _loopBitmapIndex));
        _drags.push_back(move(newLoopEntry));
    }
}

CRect CSoundView::GetLoopMarkerRect(DWORD dwTicks)
{
    CRect rectBottom = _GetBottomMargin();
    // Loop marker appears right near the bottom.
    rectBottom.top = rectBottom.bottom - 16;
    int x = TicksToX((int)dwTicks);
    rectBottom.left = x - 8;
    rectBottom.right = x + 8;
    return rectBottom;
}

CRect CSoundView::GetCueMarkerRect(DWORD dwTicks)
{
    CRect rectBottom = _GetBottomMargin();
    rectBottom.bottom -= 18; // Cues appear above the loop marker.
    rectBottom.top = rectBottom.bottom - 16;
    int x = TicksToX((int)dwTicks);
    rectBottom.left = x - 8;
    rectBottom.right = x + 8;
    return rectBottom;
}

int CSoundView::XToTicks(int cx)
{
    CRect rectBottom = _GetBottomMargin();
    int dwTicks = (int)GetSoundComponent()->GetTotalTicks();
    // Clip it:
    cx = min(cx, rectBottom.right);
    cx = max(cx, rectBottom.left);
    cx -= rectBottom.left;
    int divisor = rectBottom.Width();
    return ((dwTicks * cx) + (divisor / 2)) / divisor;      // / 2 in dividend in order to round.
}

int CSoundView::TicksToX(int ticks)
{
    CRect rectBottom = _GetBottomMargin();
    int dwTicks = (int)GetSoundComponent()->GetTotalTicks();
    int offset = ticks * rectBottom.Width() / dwTicks;
    return rectBottom.left + offset;
}

int CSoundView::_HitTestDraggableThing(CPoint pt)
{
    int nActiveCue = GetDocument()->GetActiveCue();
    int nBest = -1;
    int nSuperBest = -1;
    for (size_t i = 0; i < _drags.size(); i++)
    {
        if (_drags[i]->IsEnabled() && _drags[i]->GetRect().PtInRect(pt))
        {
            nBest = static_cast<int>(i);
            if (nActiveCue == nBest)
            {
                // Prefer the active one when they overlap.
                nSuperBest = nBest;
            }
        }
    }
    if (nSuperBest != -1)
    {
        nBest = nSuperBest;
    }
    return nBest;
}

CPoint CSoundView::_GetLoopIconLocation()
{
    CPoint pt(0, 0);
    const SoundComponent *pSound = GetSoundComponent();
    if (pSound)
    {
        DWORD dwLoop = pSound->GetLoopPoint();
        if (dwLoop != SoundComponent::LoopPointNone)
        {
            CRect rectBottom = _GetBottomMargin();
            int dwTicks = (int)pSound->GetTotalTicks();
            int offset = rectBottom.Width() * ((int)dwLoop) / dwTicks;
            int x = rectBottom.left + offset;
            pt.x = rectBottom.left + offset;
            pt.y = rectBottom.top;
        }
    }
    return pt;
}

void CSoundView::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == SOUND_TIMER)
    {
        _SetCursorPos();
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}

void CSoundView::OnSize(UINT nType, int cx, int cy)
{
    __super::OnSize(nType, cx, cy);

    // Recalc our bitmaps
    _RecalculateTrackBitmaps();
    _SetCursorPos(); // size changed
}

void CSoundView::OnUpdateTempoSlider(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(GetDocument()->GetSoundComponent()->CanChangeTempo());
    g_pSoundTempoSlider->SetView(this);
}


// CSoundView diagnostics

#ifdef _DEBUG
void CSoundView::AssertValid() const
{
	CView::AssertValid();
}

void CSoundView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
CSoundDoc* CSoundView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSoundDoc)));
    return (CSoundDoc*)m_pDocument;
}

#endif //_DEBUG


// CSoundView message handlers
