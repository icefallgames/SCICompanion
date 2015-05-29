// TextDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "MessageDoc.h"
#include "Text.h"
#include "Message.h"
#include "MessageSource.h"
#include "MessageHeaderFile.h"
#include "format.h"

using namespace std;

// CMessageDoc

IMPLEMENT_DYNCREATE(CMessageDoc, CResourceDocument)

CMessageDoc::CMessageDoc() : _selectedIndex(-1)
{
}

void CMessageDoc::SetSelectedIndex(int index, bool force)
{
    if (force || (index != _selectedIndex))
    {
        _selectedIndex = index;
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(MessageChangeHint::Selection));
    }
}

void CMessageDoc::SetMessageResource(std::unique_ptr<ResourceEntity> pMessage, int id)
{
    _checksum = id;
    AddFirstResource(move(pMessage));
    _UpdateTitle();

    if (GetResource())
    {
        _messageHeaderFile = GetMessageFile(appState->GetResourceMap().Helper().GetMsgFolder(), GetResource()->ResourceNumber);
    }
    else
    {
        _messageHeaderFile = nullptr;
    }

    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(MessageChangeHint::Changed | MessageChangeHint::AllMessageFiles));
}

BEGIN_MESSAGE_MAP(CMessageDoc, TCLASS_2(CUndoResource, CResourceDocument, ResourceEntity))
END_MESSAGE_MAP()

void CMessageDoc::v_OnUndoRedo()
{
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(MessageChangeHint::Changed));
}

MessageSource *GetMessageSourceFromType(CMessageDoc *pDoc, MessageSourceType sourceType, bool reload)
{
    if (pDoc)
    {
        switch (sourceType)
        {
            case MessageSourceType::Conditions:
                return pDoc->GetConditionMessageSource();
            case MessageSourceType::Verbs:
                return appState->GetResourceMap().GetVerbsMessageSource(reload);
            case MessageSourceType::Talkers:
                return appState->GetResourceMap().GetTalkersMessageSource(reload);
            case MessageSourceType::Nouns:
                return pDoc->GetNounMessageSource();
        }
    }
    return nullptr;
}

MessageSource *CMessageDoc::GetNounMessageSource()
{
    if (_messageHeaderFile)
    {
        return _messageHeaderFile->GetMessageSource("NOUNS");
    }
    return nullptr;
}
MessageSource *CMessageDoc::GetConditionMessageSource()
{
    if (_messageHeaderFile)
    {
        return _messageHeaderFile->GetMessageSource("CASES");
    }
    return nullptr;
}

// CMessageDoc diagnostics

#ifdef _DEBUG
void CMessageDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CMessageDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


// CMessageDoc serialization

void CMessageDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }
}


// CMessageDoc commands
