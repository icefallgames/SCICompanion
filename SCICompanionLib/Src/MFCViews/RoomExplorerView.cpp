// FontPreviewView.cpp : implementation of the CRoomExplorerView class
//
#include "stdafx.h"
#include "RoomExplorerDoc.h"
#include "AppState.h"
#include "PicDrawManager.h"
#include "ResourceEntity.h"
#include "View.h"
#include "ScriptOM.h"
#include "CompiledScript.h"
#include "Pic.h"
#include "PaletteOperations.h"
#include <unordered_set>

using namespace sci;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "RoomExplorerView.h"

#define COLOR_BG RGB(144, 144, 80)
#define COLOR_SHADOW RGB(100, 100, 60)
#define PEN_COLOR RGB(40, 40, 40)

#define HORZ_SPACE 420
#define VERT_SPACE 280

#define UWM_ROOMBMPREADY      (WM_APP + 0)
#define ROOM_MARGINS        CSize(2, 2)

#define ARROW_SEP           5
#define ARROW_LENGTH 7
#define ARROW_WIDTH 4

#define MOUSEMOVE_TIMER 4567

enum RoomDirection : uint16_t
{
    Unknown = 0,
    North = 1,
    East = 2,
    South = 3,
    West = 4,
};

typedef std::pair<RoomDirection, uint16_t> DirRoomPair;

struct RoomInfo
{
    std::vector<DirRoomPair> directionAndRoomPairs;
    // TODO: put more stuff in here once I convert.
};



// Contains an isolated set of rooms.
struct SubGrid
{
    SubGrid() : left(0), top(0), right(0), bottom(0), xFinalOffset(0), yFinalOffset(0) {}

    int left, top, bottom, right;

    int xFinalOffset, yFinalOffset;

    int width() { return right - left; }
    int height() { return bottom - top; }

    void _Unpack(uint32_t index, int &x, int &y)
    {
        x = (int16_t)(index & 0xffff);
        y = (((int32_t)index) >> 16);   // Make signed before shifting down.
    }
    uint32_t _Pack(int x, int y)
    {
        return (x & 0xffff) | (y << 16);
    }

    void _MakeRoomAt(int xIn, int yIn)
    {
        // For now, just push things to the right
        // Probably the best thing to do would be to make room towards the edge
        // closest to x, y
        for (auto &pair : roomToLocation)
        {
            int x, y;
            _Unpack(pair.second, x ,y);
            if ((y == yIn) && (x >= xIn))
            {
                // Remove it from locationToRoom
                size_t numRemoved = locationToRoom.erase(pair.second);
                assert(numRemoved == 1);

                // Push it over and update roomToLocation
                x++;
                pair.second = _Pack(x, y);

                // Now put it back in locationToRoom
                locationToRoom[pair.second] = pair.first;

                // Ensure that we expand our bounds
                right = max(right, x + 1);
            }
        }
    }


    void AddRoomAt(uint16_t room, int x, int y)
    {
        left = min(x, left);
        top = min(y, top);
        right = max(right, x + 1);
        bottom = max(bottom, y + 1);

        uint32_t index = _Pack(x, y);

        // If something's already there, shift stuff over.
        auto it = locationToRoom.find(index);
        if (it != locationToRoom.end())
        {
            _MakeRoomAt(x, y);
            assert(locationToRoom.find(index) == locationToRoom.end());
        }

        assert(roomToLocation.find(room) == roomToLocation.end());
        locationToRoom[index] = room;
        roomToLocation[room] = index;
    }

    void GetRoomLocation(uint16_t room, int &x, int &y)
    {
        uint32_t location = roomToLocation[room];
        _Unpack(location, x, y);
    }

    std::unordered_map<uint32_t, uint16_t> locationToRoom;
    std::unordered_map<uint16_t, uint32_t> roomToLocation;
};

// This returns a mapping of room to location, given:
//  - A list of rooms
//  - A suggested width (not in pixels, in "room units")
//  - A map of room to roominfos, which contain directional info
void OrganizeGrid(std::vector<uint16_t> &unprocessedRooms, int suggestedWidth, const std::map<uint16_t, RoomInfo> &roomToInfo, std::map<uint16_t, CPoint> &roomToFinalLocation)
{
    assert(roomToFinalLocation.empty());
    std::vector<SubGrid> subGrids;

    while (!unprocessedRooms.empty())
    {
        // These are the connected rooms for this subgrid:
        std::vector<uint16_t> connectedRooms;
        connectedRooms.push_back(unprocessedRooms.back());
        unprocessedRooms.pop_back();

        subGrids.emplace_back();
        SubGrid &subGrid = subGrids.back();

        // Add our seed room to the grid at a particular location.
        subGrid.AddRoomAt(connectedRooms[0], 0, 0);

        while (!connectedRooms.empty())
        {
            uint16_t currentRoom = connectedRooms.back();
            connectedRooms.pop_back();
            int x, y;
            subGrid.GetRoomLocation(currentRoom, x, y);

            const RoomInfo &roomInfo = roomToInfo.at(currentRoom);
            for (const auto &connection : roomInfo.directionAndRoomPairs)
            {
                // Ignore any rooms that have already been processed.
                auto it = find(unprocessedRooms.begin(), unprocessedRooms.end(), connection.second);
                if (it != unprocessedRooms.end())
                {
                    // We found it. Remove it from the unprcessed rooms list
                    unprocessedRooms.erase(it);
                    int newX = x;
                    int newY = y;
                    switch (connection.first)
                    {
                    case RoomDirection::East:
                        newX++;
                        break;
                    case RoomDirection::West:
                        newX--;
                        break;
                    case RoomDirection::North:
                        newY--;
                        break;
                    case RoomDirection::South:
                        newY++;
                        break;
                    default:
                        assert(false);
                        break;
                    }

                    // Add it to the subgrid at the new location.
                    subGrid.AddRoomAt(connection.second, newX, newY);
                    // Add this to the list of ones that needs to be processed for this subgrid.
                    connectedRooms.push_back(connection.second);
                }
            }

        }
    }

    // Now that we have subgrids, we need to finalize the position for each of the rooms.
    // This is like a rectangle packing problem.
    // Once we've put the grids where we need to, we'll then create a mapping from room number
    // to final x/y location.
    // Once that's done, we'll do the same for the "connections" (or perhaps that can be done on the fly
    // when rendering?).

    // Packing. This doesn't need to be terribly complex.
    int currentRow = 0;
    int currentHeight = 0;
    int currentColumn = 0;
    for (SubGrid &subGrid : subGrids)
    {
        // Should we put it in this row? If not, move to next row.
        if ((currentColumn != 0) && ((currentColumn + subGrid.width()) > suggestedWidth))
        {
            currentColumn = 0;
            currentRow += currentHeight;
            currentHeight = 0;
        }

        // We want left, top of the grid to be at (currentColumn, currentRow)
        subGrid.xFinalOffset = currentColumn - subGrid.left;
        subGrid.yFinalOffset = currentRow - subGrid.top;

        currentHeight = max(currentHeight, subGrid.height());
        currentColumn += subGrid.width();
    }

    // Whew, that was simple.
    for (SubGrid &subGrid : subGrids)
    {
        for (auto pair : subGrid.roomToLocation)
        {
            int x, y;
            subGrid.GetRoomLocation(pair.first, x, y);
            roomToFinalLocation[pair.first] = CPoint(x + subGrid.xFinalOffset, y + subGrid.yFinalOffset);
        }
    }
}


CRoomExplorerWorkResult *CRoomExplorerWorkResult::CreateFromWorkItem(CRoomExplorerWorkItem *pWorkItem)
{
    CRoomExplorerWorkResult *pResult = NULL;
    BOOL fOk = FALSE;
    if (pWorkItem->blob.GetType() == ResourceType::Pic)
    {
        unique_ptr<ResourceEntity> picEntity = CreateResourceFromResourceData(pWorkItem->blob);
        PicComponent &pic = picEntity->GetComponent<PicComponent>();
        PaletteComponent *palette = picEntity->TryGetComponent<PaletteComponent>();
        PicDrawManager pdm(&pic, palette);
		std::unique_ptr<BYTE[]> dataDisplay = std::make_unique<BYTE[]>(BMPSIZE);
		std::unique_ptr<BYTE[]> dataAux = std::make_unique<BYTE[]>(BMPSIZE);
        fOk = TRUE;
        pdm.CopyBitmap(PicScreen::Visual, PicPosition::Final, dataDisplay.get(), dataAux.get(), nullptr);

        for (auto &pRoomView : pWorkItem->_views)
        {
            std::unique_ptr<ResourceEntity> view(CreateViewResource(appState->GetVersion()));
            if (SUCCEEDED(view->InitFromResource(&pRoomView->blob)))
            {
                DrawViewWithPriority(dataDisplay.get(), pdm.GetPicBits(PicScreen::Priority, PicPosition::Final), PriorityFromY(pRoomView->wy, *pdm.GetViewPort(PicPosition::Final)),
                    pRoomView->wx, pRoomView->wy,
                    view.get(), pRoomView->wLoop, pRoomView->wCel);
            }
        }

        // Now make a smushed down bitmap for all this.
        pResult = new CRoomExplorerWorkResult();
        if (pResult)
        {
            pResult->wScript = pWorkItem->wScript;
            // Result takes ownership:
            pResult->pBitmapData.swap(dataDisplay);
            // Make a copy of the palette so we can draw it correctly:
            if (palette)
            {
                pResult->optionalPalette.reset(new PaletteComponent(*palette));
            }
        }
    }
    return pResult;
}

CRoomView::CRoomView()
{
    wx = 0;
    wy = 0;
    wLoop = 0;
    wCel = 0;
    hbmp = NULL;
    wView = 0;
}

CRoomView::~CRoomView()
{
    if (hbmp)
    {
        DeleteObject(hbmp);
    }
}

int g_count = 0;

CRoomExplorerNode::CRoomExplorerNode()
{
    North = 0;
    East = 0;
    South = 0;
    West = 0;
    Picture = 0;

    Considered = FALSE;
    BitmapScaled = NULL;
    _fAskedForBitmap = FALSE;
    g_count++;
    _sizeScaled.SetSize(0, 0);
}

CRoomExplorerNode::~CRoomExplorerNode()
{
    g_count--;
    if (BitmapScaled)
    {
        DeleteObject(BitmapScaled);
    }
}

void CRoomExplorerNode::_ResizeBitmap(CSize size)
{
    if (BitmapScaled)
    {
        DeleteObject(BitmapScaled);
        BitmapScaled = NULL;
    }

    if (pBitmapData)
    {
        // Temporarily make a gdi+ wrapper for it.
        Gdiplus::Bitmap *pimg = NULL;
        RGBQUAD *palette = g_egaColors;
        int paletteCout = ARRAYSIZE(g_egaColors);
        if (optionalPalette)
        {
            palette = optionalPalette->Colors;
            paletteCout = ARRAYSIZE(optionalPalette->Colors);
        }
        SCIBitmapInfo bmi(sPIC_WIDTH, sPIC_HEIGHT, palette, paletteCout);
        pimg = Gdiplus::Bitmap::FromBITMAPINFO(&bmi, pBitmapData.get());
        if (pimg)
        {
            if ((size.cx == sPIC_WIDTH) && (size.cy == sPIC_HEIGHT))
            {
                // Exact size.
                pimg->GetHBITMAP(Color::Black, &BitmapScaled);
            }
            else
            {
                // Make a different sized version
                // Note: this cast to (Bitmap*) is undocumented.
                Gdiplus::Bitmap *pimage = (Gdiplus::Bitmap*)pimg->GetThumbnailImage(size.cx, size.cy, NULL, NULL);
                if (pimage)
                {
                    pimage->GetHBITMAP(Color::Black, &BitmapScaled);
                    delete pimage;
                }
            }
            delete pimg;
        }
    }
    _sizeScaled = size;
}

void CRoomExplorerNode::OnDraw(CDC *pDC, CRect *prc, QueueItems<CRoomExplorerWorkItem, CRoomExplorerWorkResult> *pQueue)
{
    CRect rcClip;
    pDC->GetClipBox(&rcClip);
    CRect rcInner;
    if (rcInner.IntersectRect(&rcClip, prc))
    {
        CDC dcMem;
        if (dcMem.CreateCompatibleDC(pDC))
        {
            if (pBitmapData)
            {
                CSize sizeToPaint(prc->Width(), prc->Height());
                if (sizeToPaint != _sizeScaled)
                {
                    // Make a scaled version of the bitmap
                    _ResizeBitmap(sizeToPaint);
                }
                if (BitmapScaled)
                {
                    HGDIOBJ hOld = dcMem.SelectObject(BitmapScaled);
                    pDC->BitBlt(prc->left, prc->top, prc->Width(), prc->Height(), &dcMem, 0, 0, SRCCOPY);
                    dcMem.SelectObject(hOld);
                }
            }
            else
            {
                if (!_fAskedForBitmap)
                {
                    _fAskedForBitmap = TRUE;

                    if (_pWorkItem)
                    {
                        pQueue->GiveWorkItem(move(_pWorkItem));
                    }
                }
                CBrush brush(RGB(128, 128, 128));
                dcMem.FillRect(prc, &brush);
            }
            // Draw number on top.
            TCHAR szNumber[10];
            StringCchPrintf(szNumber, ARRAYSIZE(szNumber), TEXT("%d"), this->ScriptNum);
            COLORREF crOld = pDC->SetTextColor(RGB(0, 0, 0));
            CRect rcShadow = prc;
            rcShadow.OffsetRect(1, 1);
            pDC->DrawText(szNumber, -1, &rcShadow, 0);
            pDC->SetTextColor(RGB(255, 255, 255));
            pDC->DrawText(szNumber, -1, prc, 0);
            pDC->SetTextColor(crOld);
        }
    }
}

void CRoomExplorerNode::Init(const CRoomExplorerView::CRoomExplorerGrid *pGrid, GlobalCompiledScriptLookups &globalLookups, CompiledScript &compiledScript, CompiledObjectBase &classDefinition, const ImportantSelectors &importantSelectors, uint16_t viewSpeciesIndex)
{
    _pGrid = pGrid;
    assert(_pWorkItem.get() == nullptr);
    _pWorkItem = make_unique<CRoomExplorerWorkItem>(compiledScript.GetScriptNumber());
    ScriptNum = compiledScript.GetScriptNumber();
    const vector<CompiledVarValue> &propValues = classDefinition.GetPropertyValues();
    vector<uint16_t> props = classDefinition.GetProperties();
    globalLookups.LookupSpeciesPropertyList(classDefinition.GetSuperClass(), props);
    for (size_t i = 0; i < props.size(); i++)
    {
        if (props[i] == importantSelectors.North)
        {
            North = propValues[i].value;
        }
        else if (props[i] == importantSelectors.East)
        {
            East = propValues[i].value;
        }
        else if (props[i] == importantSelectors.South)
        {
            South = propValues[i].value;
        }
        else if (props[i] == importantSelectors.West)
        {
            West = propValues[i].value;
        }
        else if (props[i] == importantSelectors.Picture)
        {
            Picture = propValues[i].value;
            // Get the ResourceBlob for it.
            auto it = _pGrid->GetPics().find(Picture);
            if (it != _pGrid->GetPics().end())
            {
                _pWorkItem->blob = *it->second;
            }
        }
    }

    // Look for any subclasses of View
    for (auto &classDef : compiledScript.GetObjects())
    {
        // TODO: For accuracy, follow the chain of super classes.
        if (classDef->IsInstance() && (classDef->GetSuperClass() == viewSpeciesIndex))
        {
            _AddView(*classDef, globalLookups, importantSelectors);
        }
    }
}

void CRoomExplorerNode::_AddView(CompiledObjectBase &classDefinition, GlobalCompiledScriptLookups &globalLookups, const ImportantSelectors &importantSelectors)
{
    unique_ptr<CRoomView> pView = make_unique<CRoomView>();

    // I'm often getting an additional thing here.
    const vector<CompiledVarValue> &propValues = classDefinition.GetPropertyValues();
    vector<uint16_t> props;
    globalLookups.LookupSpeciesPropertyList(classDefinition.GetSpecies(), props);
    // Some instances have more property values than their species does. Curious.
    // An example is the buttonPushed View in script 10 of SQ1 VGA.
    assert(props.size() <= propValues.size());
    for (size_t i = 0; i < props.size(); i++)
    {
        if (props[i] == importantSelectors.View)
        {
            pView->wView = propValues[i].value;
        }
        else if (props[i] == importantSelectors.X)
        {
            pView->wx = propValues[i].value;
        }
        else if (props[i] == importantSelectors.Y)
        {
            pView->wy = propValues[i].value;
        }
        else if (props[i] == importantSelectors.Loop)
        {
            pView->wLoop = propValues[i].value;
        }
        else if (props[i] == importantSelectors.Cel)
        {
            pView->wCel = propValues[i].value;
        }
    }

    // Get the ResourceBlob for it.
    auto it = _pGrid->GetViews().find(pView->wView);
    if (it != _pGrid->GetViews().end())
    {
        pView->blob = *it->second;
        // Add it to the views list of the work item.
        assert(_pWorkItem);
        _pWorkItem->_views.push_back(move(pView));
    }
}

CRoomExplorerView::CRoomExplorerGrid::CRoomExplorerGrid()
{
    _fIsComplete = FALSE;
    _pExplorer = NULL;
    _wHoverScript = 0;
    _wSelectedScript = 0;
}

void CRoomExplorerView::CRoomExplorerGrid::AddRoom(GlobalCompiledScriptLookups &globalLookups, CompiledScript &compiledScript, CompiledObjectBase &classDefinition, const ImportantSelectors &importantSelectors, uint16_t viewSpeciesIndex)
{
    // It's a room class!
    unique_ptr<CRoomExplorerNode> pNode = make_unique<CRoomExplorerNode>();
    pNode->Init(this, globalLookups, compiledScript, classDefinition, importantSelectors, viewSpeciesIndex);
    _nodes[compiledScript.GetScriptNumber()] = move(pNode);
}

void CRoomExplorerView::CRoomExplorerGrid::LoadResources()
{
    auto resourceContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::Pic | ResourceTypeFlags::View, ResourceEnumFlags::MostRecentOnly);
    for (auto &pBlob : *resourceContainer)
    {
        if (pBlob->GetType() == ResourceType::Pic)
        {
            _pics[pBlob->GetNumber()] = move(pBlob);
        }
        else
        {
            _views[pBlob->GetNumber()] = move(pBlob);
        }
    }
}

void CRoomExplorerView::CRoomExplorerGrid::OrganizeRooms()
{
    _fIsComplete = TRUE;

    std::vector<uint16_t> unprocessedRooms;
    std::map<uint16_t, RoomInfo> roomToInfo;

    for (auto &node : _nodes)
    {
        uint16_t room = node.first;
        CRoomExplorerNode *pNode = node.second.get();

        unprocessedRooms.push_back(room);

        // We'll consider 0 has meaning there is no connection (as that is the default
        // value for these properties)
        RoomInfo roomInfo;
        if (pNode->East)
        {
            roomInfo.directionAndRoomPairs.push_back(DirRoomPair(RoomDirection::East, pNode->East));
        }
        if (pNode->West)
        {
            roomInfo.directionAndRoomPairs.push_back(DirRoomPair(RoomDirection::West, pNode->West));
        }
        if (pNode->North)
        {
            roomInfo.directionAndRoomPairs.push_back(DirRoomPair(RoomDirection::North, pNode->North));
        }
        if (pNode->South)
        {
            roomInfo.directionAndRoomPairs.push_back(DirRoomPair(RoomDirection::South, pNode->South));
        }

        roomToInfo[room] = roomInfo;
    }

    std::map<uint16_t, CPoint> roomToFinalLocation;
    OrganizeGrid(unprocessedRooms, 8, roomToInfo, roomToFinalLocation);

    _rcBounds.SetRectEmpty();
    for (auto &pair : roomToFinalLocation)
    {
        _rcBounds.left = min(_rcBounds.left, pair.second.x);
        _rcBounds.right = max(_rcBounds.right, pair.second.x);
        _rcBounds.top = min(_rcBounds.top, pair.second.y);
        _rcBounds.bottom = max(_rcBounds.bottom, pair.second.y);

        _nodes[pair.first]->Position = pair.second;
    }
}

void CRoomExplorerView::CRoomExplorerGrid::_GetRoomRect(CRoomExplorerNode *pNode, CRect &rect)
{
    CPoint ptDraw = pNode->Position - GetBounds()->TopLeft();
    int iZoom = _pExplorer->GetZoom();
    ptDraw.x *= (HORZ_SPACE / iZoom);
    ptDraw.y *= (VERT_SPACE / iZoom);

    CPoint ptBR = ptDraw + CPoint(sPIC_WIDTH / iZoom, sPIC_HEIGHT / iZoom);
    rect.left = ptDraw.x;
    rect.right = ptDraw.x + sPIC_WIDTH / iZoom;
    rect.top = ptDraw.y;
    rect.bottom = ptDraw.y + sPIC_HEIGHT / iZoom;
    rect.InflateRect(ROOM_MARGINS);

    CPoint pt = _pExplorer->GetOrigin();
    rect.OffsetRect(-pt.x, -pt.y);
}

#define SWAP(x, y) { int temp = x; x = y; y = temp; }

CPoint GetBresen(CPoint ptFrom, CPoint ptTo, int howMuch)
{
    int x0 = ptTo.x;
    int y0 = ptTo.y;
    int x1 = ptFrom.x;
    int y1 = ptFrom.y;
    BOOL fSteep = abs(y1 - y0) > abs(x1 - x0);
    if (fSteep)
    {
        SWAP(x0, y0);
        SWAP(x1, y1);
    }
    if (x0 > x1)
    {
        SWAP(x0, x1);
        SWAP(y0, y1);
        howMuch = (x1 - x0) - howMuch;
    }
    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = 0;
    int y = y0;
    int ystep = (y0 < y1) ? 1 : -1;
    CPoint ptReturn = ptTo;
    for (int x = x0; x < x1; x++)
    {
        if (howMuch)
        {
            error = error + deltay;
            if ((2 * error) >= deltax)
            {
                y = y + ystep;
                error = error - deltax;
            }
        }
        else
        {
            ptReturn = fSteep ? CPoint(y, x) : CPoint(x, y);
        }
        howMuch--;
    }
    return ptReturn;
}

void _DrawArrowLine(CDC *pDC, CPoint ptFrom, CPoint ptTo)
{
    // Draw the line first.
    pDC->MoveTo(ptFrom);
    pDC->LineTo(ptTo);

    // Calc the base of the arrow
    CPoint ptArrowBase = GetBresen(ptFrom, ptTo, ARROW_LENGTH);
    CPoint ptArrowTipsDelta = GetBresen(ptFrom, ptTo, ARROW_WIDTH) - ptArrowBase;

    // Now rotate right 90 degrees to find the right potin
    CPoint ptRight = ptArrowBase + CPoint(ptArrowTipsDelta.y, -ptArrowTipsDelta.x);
    // And left 90 degrees
    CPoint ptLeft = ptArrowBase + CPoint(-ptArrowTipsDelta.y, ptArrowTipsDelta.x);

    // Now draw shape with these 3 points.
    CPoint rgpt[] = { ptRight, ptLeft, ptTo };
    pDC->Polygon(rgpt, ARRAYSIZE(rgpt));
}

void CRoomExplorerView::CRoomExplorerGrid::_DrawLineTo(CDC *pDC, CPoint ptFrom, uint16_t wRoom)
{
    if (wRoom)
    {
        numToNode_t::iterator nodeIt = _nodes.find(wRoom);
        if (nodeIt != _nodes.end())
        {
            CRoomExplorerNode *pToNode = nodeIt->second.get();
            // Figure out where it is in relation to us.
            CRect rectTo;
            _GetRoomRect(pToNode, rectTo);
            rectTo.DeflateRect(ROOM_MARGINS);
            CPoint ptTo = rectTo.CenterPoint();
            if (ptFrom.x < rectTo.left)
            {
                ptTo.x = rectTo.left;
                ptTo.y += ARROW_SEP;
            }
            else if (ptFrom.x > rectTo.right)
            {
                ptTo.x = rectTo.right;
                ptTo.y -= ARROW_SEP;
            }
            else if (ptFrom.y < rectTo.top)
            {
                ptTo.y = rectTo.top;
                ptTo.x += ARROW_SEP;
            }
            else if (ptFrom.y > rectTo.bottom)
            {
                ptTo.y = rectTo.bottom;
                ptTo.x -= ARROW_SEP;
            }
            _DrawArrowLine(pDC, ptFrom, ptTo);
        }
    }
}

const CRect *CRoomExplorerView::CRoomExplorerGrid::GetBounds()
{
    // Either calculate and store the bounds, or you know...
    return &_rcBounds;
}

void CRoomExplorerView::CRoomExplorerGrid::DrawRooms(CDC *pDC, BOOL fHitTestOnly, CPoint pt, uint16_t *pwRoom)
{
    CPen pen(PS_SOLID, 2, PEN_COLOR);
    HGDIOBJ penOld = nullptr;
    if (pDC)
    {
        penOld = pDC->SelectObject(&pen);
    }
    if (pwRoom)
    {
        *pwRoom = 0;
    }

    numToNode_t::iterator nodeIt = _nodes.begin();
    while (nodeIt != _nodes.end())
    {
        CRoomExplorerNode *pNode = nodeIt->second.get();
        uint16_t wScript = nodeIt->first;
        CRect rect;
        _GetRoomRect(pNode, rect);
        if (fHitTestOnly)
        {
            if (rect.PtInRect(pt))
            {
                *pwRoom = pNode->ScriptNum;
                break;
            }
        }
        else
        {
            // Draw a little shadow
            CSize sizeMargins = ROOM_MARGINS;
            CRect rcShadow(rect.left + sizeMargins.cx * 2, rect.bottom - sizeMargins.cy, rect.right, rect.bottom);
            pDC->FillSolidRect(&rcShadow, COLOR_SHADOW);
            rcShadow.SetRect(rect.right - sizeMargins.cx, rect.top + sizeMargins.cy * 2, rect.right, rect.bottom);
            pDC->FillSolidRect(&rcShadow, COLOR_SHADOW);

            if (_wHoverScript == wScript)
            {
                // Draw a hover rectangle.
                pDC->Rectangle(&rect);
            }
            rect.DeflateRect(ROOM_MARGINS);
            pNode->OnDraw(pDC, &rect, _pExplorer->GetQueue());

            // Draw some lines.
            _DrawLineTo(pDC, CPoint((rect.left + rect.right) / 2 - ARROW_SEP, rect.top), pNode->North);
            _DrawLineTo(pDC, CPoint(rect.right, (rect.top + rect.bottom) / 2 + ARROW_SEP), pNode->East);
            _DrawLineTo(pDC, CPoint((rect.left + rect.right) / 2 + ARROW_SEP, rect.bottom), pNode->South);
            _DrawLineTo(pDC, CPoint(rect.left, (rect.top + rect.bottom) / 2 - ARROW_SEP), pNode->West);
        }
        ++nodeIt;
    }    
    if (pDC)
    {
        pDC->SelectObject(penOld);
    }
}

CRoomExplorerNode *CRoomExplorerView::CRoomExplorerGrid::GetNode(uint16_t wScript)
{
    numToNode_t::iterator nodeIt = _nodes.find(wScript);
    return (nodeIt != _nodes.end()) ? nodeIt->second.get() : nullptr;
}

void CRoomExplorerView::CRoomExplorerGrid::SetHoveredRoom(uint16_t wRoom)
{
    if (wRoom != _wHoverScript)
    {
        numToNode_t::iterator nodeIt = _nodes.find(_wHoverScript);
        if (nodeIt != _nodes.end())
        {
            CRect rect;
            _GetRoomRect(nodeIt->second.get(), rect);
            _pExplorer->InvalidateGridRect(&rect);
        }
        _wHoverScript = wRoom;
        nodeIt = _nodes.find(wRoom);
        if (nodeIt != _nodes.end())
        {
            CRect rect;
            _GetRoomRect(nodeIt->second.get(), rect);
            _pExplorer->InvalidateGridRect(&rect);
        }
    }
}

void CRoomExplorerView::_OnMouseWheel(UINT nFlags, BOOL fForward, CPoint pt, short nNotches)
{
    int iZoomOld = _iZoom;

    if (nFlags & MK_CONTROL)
    {
        // Ctrl held down.  Do a zoom:
        if (fForward)
        {
            _iZoom--;
            _iZoom = max(_iZoom, 1);
        }
        else
        {
            _iZoom++;
        }

        // Zoom around ptHover.
        int cxHalf = _ptHoverClient.x;
        int cyHalf = _ptHoverClient.y;
        _xOrigin = ((_xOrigin + cxHalf) * iZoomOld / _iZoom) - cxHalf;
        _yOrigin = ((_yOrigin + cyHalf) * iZoomOld / _iZoom) - cyHalf;
        _InvalidateScrollStuff();
        Invalidate(FALSE);
    }

    if (nFlags == 0)
    {
        // TODO: scale the scrolling by the ratio of visible area to client height
        int iAmount = 40 * nNotches;

        // Which scroll bar?  Which one are we closest to?
        RECT rc;
        GetClientRect(&rc);
        _ScrollDelta(SB_VERT, m_hWnd, fForward ? -iAmount : iAmount);
    }
}


CRoomExplorerView::CRoomExplorerGrid::~CRoomExplorerGrid()
{
}



// CRoomExplorerView

IMPLEMENT_DYNCREATE(CRoomExplorerView, CView)


BEGIN_MESSAGE_MAP(CRoomExplorerView, CScrollingThing<CView>)
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_MESSAGE(UWM_ROOMBMPREADY, _OnRoomBitmapReady)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_MESSAGE(WM_MOUSELEAVE, _OnMouseLeave)
    ON_WM_TIMER()
    ON_WM_KILLFOCUS()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

#define MOVE_NONE 0
#define MOVE_UP 1
#define MOVE_DOWN 2
#define MOVE_RIGHT 3
#define MOVE_LEFT 4

CRoomExplorerView::CRoomExplorerView()
{
    // Scrolling:
    _yOrigin = 0;
    _xOrigin = 0;

    _cx = 1000;
    _cy = 1000;
    _iZoom = 3;

    _fDoubleBuf = FALSE;
    _pbitmapDoubleBuf = NULL;
    _pQueue = NULL;
    _fMoveTimer = FALSE;
    _fInCapture = FALSE;
    _grid.SetRoomExplorer(this);

    _iMoveDirection = MOVE_NONE;
    _fMouseButtonDown = FALSE;
}

CRoomExplorerView::~CRoomExplorerView()
{
    if (_fInCapture)
    {
        ReleaseCapture();
        _fInCapture = FALSE;
    }

    _CleanDoubleBuffer();
    if (_pQueue)
    {
        _pQueue->Abort();
        _pQueue->Release();
    }
}

void CRoomExplorerView::_CleanDoubleBuffer()
{
    // Reset our double buffering, so we regenerate it on the next paint cycle.
    _fDoubleBuf = FALSE;
    delete _pbitmapDoubleBuf;
    _pbitmapDoubleBuf = FALSE;
}


void CRoomExplorerView::_GenerateDoubleBuffer(CDC *pDC)
{
    ASSERT(!_pbitmapDoubleBuf);
    _pbitmapDoubleBuf = new CBitmap();
    if (_pbitmapDoubleBuf)
    {
        CRect rc;
        GetClientRect(&rc);
        //_fDoubleBuf = _pbitmapDoubleBuf->CreateCompatibleBitmap(pDC, _cx * _iZoom, _cy * _iZoom);
        _fDoubleBuf = _pbitmapDoubleBuf->CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
    }
}


void CRoomExplorerView::_OnDraw(CDC *pDC, BOOL fHitTestOnly, CPoint point, uint16_t *pwRoom)
{
    int iModePrev = pDC->SetBkMode(TRANSPARENT);
    _grid.DrawRooms(pDC, fHitTestOnly, point, pwRoom);
    pDC->SetBkMode(iModePrev);
}

void CRoomExplorerView::OnDraw(CDC *pDC)
{
    if (_pQueue == nullptr)
    {
        _pQueue = new QueueItems<CRoomExplorerWorkItem, CRoomExplorerWorkResult>(this->GetSafeHwnd(), UWM_ROOMBMPREADY);
        if (_pQueue)
        {
            if (!_pQueue->Init())
            {
                _pQueue->Release();
                _pQueue = nullptr;
            }
        }
    }

    RECT rcClient;
    GetClientRect(&rcClient);
    CDC dcMem;
    if (dcMem.CreateCompatibleDC(pDC))
    {
        if (!_fDoubleBuf)
        {
            _GenerateDoubleBuffer(pDC);
        }

        if (_fDoubleBuf)
        {
            ASSERT(_pbitmapDoubleBuf);
            HGDIOBJ hgdiObj = dcMem.SelectObject(_pbitmapDoubleBuf);

            // Fill background
            dcMem.FillSolidRect(0, 0, _cx, _cy, COLOR_BG);


            // Draw the picture.
            _OnDraw(&dcMem);

            // Now blt back to the real DC.
            pDC->StretchBlt(0, 0, _GetViewWidth(), _GetViewHeight(), &dcMem, 0, 0, _GetViewWidth(), _GetViewHeight(), SRCCOPY); 

            dcMem.SelectObject(hgdiObj);

        }
    }

    CBrush brush(RGB(128, 128, 64));
    int cyBottom = RECTHEIGHT(rcClient) - _GetViewHeight();
    if (cyBottom > 0)
    {
        CRect rc(CPoint(0, _GetViewHeight()), CSize(RECTWIDTH(rcClient), cyBottom));
        pDC->FillRect(&rc, &brush);
    }
    int cxLeft = RECTWIDTH(rcClient) -  _GetViewWidth();
    if (cxLeft > 0)
    {
        CRect rc(CPoint(_GetViewWidth(), 0), CSize(cxLeft, RECTHEIGHT(rcClient)));
        pDC->FillRect(&rc, &brush);
    }
    __super::OnDraw(pDC);
}

void CRoomExplorerView::OnSize(UINT nType, int cx, int cy)
{
    _RecalcHeight();
    __super::OnSize(nType, cx, cy);
    _CleanDoubleBuffer();
}


void CRoomExplorerView::_UpdateCursor()
{
    switch (_iMoveDirection)
    {
    case MOVE_UP:
        SetCursor(LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSORUP)));
        break;
    case MOVE_DOWN:
        SetCursor(LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSORDOWN)));
        break;
    case MOVE_RIGHT:
        SetCursor(LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSORRIGHT)));
        break;
    case MOVE_LEFT:
        SetCursor(LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSORLEFT)));
        break;
    default:
        SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
    }
}

#define SENSITIVITY 40

CPoint CRoomExplorerView::_GetMoveDelta(int *piMoveDirection)
{
    int iMoveDirection = MOVE_NONE;
    CPoint ptMove(0, 0);

    if (_ptHoverClient.x && _ptHoverClient.y) // don't move when at 0, 0
    {
        CRect rcClient;
        GetClientRect(&rcClient);

        int iRightDelta = rcClient.right - _ptHoverClient.x;
        if (iRightDelta < SENSITIVITY)
        {
            iMoveDirection = MOVE_RIGHT;
            ptMove.x += (SENSITIVITY - iRightDelta) / 2;
        }
        int iLeftDelta = _ptHoverClient.x - rcClient.left;
        if (iLeftDelta < SENSITIVITY)
        {
            iMoveDirection = MOVE_LEFT;
            ptMove.x -= (SENSITIVITY - iLeftDelta) / 2;
        }
        int iBottomDelta = rcClient.bottom - _ptHoverClient.y;
        if (iBottomDelta < SENSITIVITY)
        {
            iMoveDirection = MOVE_DOWN;
            ptMove.y += (SENSITIVITY - iBottomDelta) / 2;
        }
        int iTopDelta = _ptHoverClient.y - rcClient.top;
        if (iTopDelta < SENSITIVITY)
        {
            iMoveDirection = MOVE_UP;
            ptMove.y -= (SENSITIVITY - iTopDelta) / 2;
        }
    }
    if (piMoveDirection)
    {
        *piMoveDirection = iMoveDirection;
    }
    return ptMove;
}

void CRoomExplorerView::OnMouseMove(UINT nFlags, CPoint point)
{
    CRect rcClient;
    GetClientRect(&rcClient);
    BOOL fMouseWithin = rcClient.PtInRect(point);

    if (fMouseWithin)
    {
        uint16_t wRoom;
        _ptHoverClient = point;
        CPoint ptView = _MapClientPointToView(point);
        ptView.Offset(CPoint(-_xOrigin, -_yOrigin));
        _grid.DrawRooms(NULL, TRUE, ptView, &wRoom);
        _grid.SetHoveredRoom(wRoom);
    }
    else
    {
        _grid.SetHoveredRoom(0);
    }
    __super::OnMouseMove(nFlags, point);

    int iMoveDirection;
    CPoint ptMove = _GetMoveDelta(&iMoveDirection);
    if (iMoveDirection != _iMoveDirection)
    {
        _iMoveDirection = iMoveDirection;
        _UpdateCursor();
    }
    if (fMouseWithin && (ptMove.x || ptMove.y))
    {
        if (!_fMoveTimer)
        {
            _fMoveTimer = TRUE;
            SetTimer(MOUSEMOVE_TIMER, 30, NULL);
        }
    }
    else
    {
        if (_fMoveTimer)
        {
            _fMoveTimer = FALSE;
            KillTimer(MOUSEMOVE_TIMER);
        }
    }

    if (fMouseWithin)
    {
        if (!_fInCapture)
        {
            // Update the cursor as soon as mouse enters the view
            _UpdateCursor();

            _fInCapture = TRUE;
            SetCapture();

            // Track mouseleaves.
            TRACKMOUSEEVENT eventTrack;
            eventTrack.cbSize = sizeof(eventTrack);
            eventTrack.dwFlags = TME_LEAVE;
            eventTrack.hwndTrack = m_hWnd;
            TrackMouseEvent(&eventTrack);
        }
    }
    else
    {
        if (_fInCapture)
        {
            ReleaseCapture();
            _fInCapture = FALSE;
        }
    }
}

void CRoomExplorerView::OnLButtonDown(UINT nFlags, CPoint point)
{
    _fMouseButtonDown = TRUE;
    __super::OnLButtonDown(nFlags, point);
}

void CRoomExplorerView::OnLButtonUp(UINT nFlags, CPoint point)
{
    _fMouseButtonDown = FALSE;
    __super::OnLButtonUp(nFlags, point);
}




LRESULT CRoomExplorerView::_OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    if (_fInCapture)
    {
        _fMouseButtonDown = FALSE;
        ReleaseCapture();
        _fInCapture = FALSE;
    }
    return 0;
}

void CRoomExplorerView::OnKillFocus(CWnd *pNewWnd)
{
    if (_fInCapture)
    {
        ReleaseCapture();
        _fInCapture = FALSE;
    }
}

void CRoomExplorerView::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == MOUSEMOVE_TIMER)
    {
        if (_fMouseButtonDown)
        {
            CRect rcClient;
            GetClientRect(&rcClient);
            CPoint ptMove = _GetMoveDelta();
            _xOrigin += ptMove.x;
            _xOrigin = min(_xOrigin, max(0, (_GetViewWidth() - rcClient.Width())));
            _xOrigin = max(0, _xOrigin);
            _yOrigin += ptMove.y;
            _yOrigin = min(_yOrigin, max(0, (_GetViewHeight() - rcClient.Height())));
            _yOrigin = max(0, _yOrigin);
            _InvalidateScrollStuff();
            Invalidate(FALSE);
        }
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}

void CRoomExplorerView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    uint16_t wRoom;
    CPoint ptView = _MapClientPointToView(point);
    ptView.Offset(CPoint(-_xOrigin, -_yOrigin));
    _grid.DrawRooms(NULL, TRUE, ptView, &wRoom);
    if (wRoom)
    {
        appState->OpenScript(wRoom);
    }
}

void CRoomExplorerView::InvalidateGridRect(LPRECT prc)
{
    CPoint ptTL = _MapViewPointToClient(CPoint(prc->left, prc->top));
    ptTL.Offset(CPoint(_xOrigin, _yOrigin));
    CPoint ptBR = _MapViewPointToClient(CPoint(prc->right, prc->bottom));
    ptBR.Offset(CPoint(_xOrigin, _yOrigin));
    CRect rect(ptTL, ptBR);
    InvalidateRect(&rect);
}

void CRoomExplorerView::_RecalcHeight()
{
    CSize sizeOld = CSize(_cx, _cy);
    if (!_grid.IsComplete())
    {
        _grid.LoadResources();
        GlobalCompiledScriptLookups globalLookups;
        globalLookups.Load(appState->GetResourceMap().Helper());
        SelectorTable &selectorTable = globalLookups.GetSelectorTable();
        GlobalClassTable &globalClassTable = globalLookups.GetGlobalClassTable();
        uint16_t viewSpeciesIndex, roomSpeciesIndex;
        unordered_set<uint16_t> roomSpecieses;  // There could be many. Same with views, but we don't really care about those as much.
        ImportantSelectors importantSelectors;
        if (selectorTable.ReverseLookup("north", importantSelectors.North) &&
            selectorTable.ReverseLookup("east", importantSelectors.East) &&
            selectorTable.ReverseLookup("south", importantSelectors.South) &&
            selectorTable.ReverseLookup("west", importantSelectors.West) &&
            selectorTable.ReverseLookup("picture", importantSelectors.Picture) &&
            selectorTable.ReverseLookup("view", importantSelectors.View) &&
            selectorTable.ReverseLookup("x", importantSelectors.X) &&
            selectorTable.ReverseLookup("y", importantSelectors.Y) &&
            selectorTable.ReverseLookup("loop", importantSelectors.Loop) &&
            selectorTable.ReverseLookup("cel", importantSelectors.Cel) &&
            globalClassTable.LookupSpecies("Rm", roomSpeciesIndex) &&
            globalClassTable.LookupSpecies("View", viewSpeciesIndex))
        {
            // Add any room subclasses
            roomSpecieses.insert(roomSpeciesIndex);
            for (auto roomsubby : globalClassTable.GetSubclassesOf(roomSpeciesIndex))
            {
                roomSpecieses.insert(roomsubby);
            }

            // We found the pre-requisites, now look for scripts with room instances.
            auto resourceContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::Script, ResourceEnumFlags::MostRecentOnly);
            for (auto &blob : *resourceContainer)
            {
                sci::istream byteStream = blob->GetReadStream();
                CompiledScript compiledScript(blob->GetNumber());
                if (compiledScript.Load(appState->GetResourceMap().Helper(), appState->GetVersion(), blob->GetNumber(), byteStream))
                {
                    for (auto &classDefinition : compiledScript.GetObjects())
                    {
                        if (classDefinition->IsInstance() && (roomSpecieses.find(classDefinition->GetSuperClass()) != roomSpecieses.end()))
                        {
                            _grid.AddRoom(globalLookups, compiledScript, *classDefinition, importantSelectors, viewSpeciesIndex);
                        }
                    }
                }
            }
        }
        _grid.OrganizeRooms();
    }

    const CRect *prc = _grid.GetBounds();
    _cx = (prc->Width() + 1) * HORZ_SPACE;
    _cy = (prc->Height() + 1) * VERT_SPACE;
}

void CRoomExplorerView::OnInitialUpdate()
{
    __super::OnInitialUpdate();

    _RecalcHeight();
}

void CRoomExplorerView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    // We have a new thing.
    __super::OnUpdate(pSender, lHint, pHint);

    // Refresh in all cases.
    if (lHint)
    {
        // Need to redraw the pic
        Invalidate(FALSE);
        _CleanDoubleBuffer(); // size migth have changed.
    }
}

//
// prevent flicker
//
BOOL CRoomExplorerView::OnEraseBkgnd(CDC *pDC)
{
    return TRUE;
}

LRESULT CRoomExplorerView::_OnRoomBitmapReady(WPARAM wParam, LPARAM lParam)
{
    CRoomExplorerWorkResult *pWorkResult;
    while (_pQueue->TakeWorkResult(&pWorkResult))
    {
        CRoomExplorerNode *pNode = _grid.GetNode(pWorkResult->wScript);
        if (pNode)
        {
            // Transfer ownership
            pNode->pBitmapData.swap(pWorkResult->pBitmapData);
            pNode->optionalPalette.swap(pWorkResult->optionalPalette);
            if (pNode->pBitmapData)
            {
                Invalidate(FALSE);
            }
        }
        delete pWorkResult;
    }
    return 0;
}


//
// This one takes into account origins
//
CPoint CRoomExplorerView::_MapClientPointToView(CPoint ptScreen)
{
    return CPoint((ptScreen.x + _xOrigin) * 1,
                  (ptScreen.y + _yOrigin) * 1);
}

//
// From pic coordinates to window client coordinates
// (taking into account origins)
//
CPoint CRoomExplorerView::_MapViewPointToClient(CPoint ptPic)
{
    return CPoint((ptPic.x / 1) - _xOrigin,
                  (ptPic.y / 1) - _yOrigin);
}


// CRoomExplorerView diagnostics

#ifdef _DEBUG
void CRoomExplorerView::AssertValid() const
{
    __super::AssertValid();
}

void CRoomExplorerView::Dump(CDumpContext& dc) const
{
    __super::Dump(dc);
}

CRoomExplorerDoc* CRoomExplorerView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRoomExplorerDoc)));
    return (CRoomExplorerDoc*)m_pDocument;
}
#endif //_DEBUG

