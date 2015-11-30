/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
#pragma once

#include "CCrystalTextBuffer.h"
#include "NonViewHostDocument.h"

class CompileTables;
class PrecompiledHeaders;
class ICompileLog;
class DependencyTracker;

// CScriptDocument document

enum class ScriptChangeHint
{
    None = 0,
    // A script was saved.  The object is CScriptDocument
    Saved = 0x00000001,
    // The cursor position in the script changed.
    Pos = 0x00000002,
    // The script was converted!
    Converted = 0x00000004,
};

DEFINE_ENUM_FLAGS(ScriptChangeHint, uint32_t)


class CScriptDocument : public CNonViewHostDocument
{
	DECLARE_DYNCREATE(CScriptDocument)

public:
	CScriptDocument();
	virtual ~CScriptDocument();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

    BOOL OnOpenDocument(LPCTSTR lpszPathName, uint16_t scriptNumber);

    CCrystalTextBuffer *GetTextBuffer();
    
    void UpdateModified();

    // For new scripts:
    void SetNameAndContent(ScriptId scriptId, int iResourceNumber, std::string &text);

    ScriptId GetScriptId() { return _scriptId; }
    void SetScriptNumber(WORD wScriptNum) { _scriptId.SetResourceNumber(wScriptNum); }

    void Compile() { OnCompile(); }

    void SaveIfModified();

    void SetDependencyTracker(DependencyTracker &tracker);

protected:
	virtual BOOL OnNewDocument();
	DECLARE_MESSAGE_MAP()

private:

    class CSCITextBuffer : public CCrystalTextBuffer
    {
    private:
        CScriptDocument &_ownerDoc;
    public:
        CSCITextBuffer(CScriptDocument &doc) : _ownerDoc(doc) {}

        virtual void SetModified(BOOL bModified = TRUE)
        {
            __super::SetModified(bModified);
            _ownerDoc.UpdateModified();
        };
    };


    // Msg handlers
    afx_msg void OnCompile();
#ifdef DOCSUPPORT
    afx_msg void OnCompileDocs();
#endif
    afx_msg void OnDisassemble();
    afx_msg void OnDebugRoom();
    afx_msg void OnViewObjectFile();
    afx_msg void OnViewScriptResource();
    afx_msg void OnViewSyntaxTree();
    afx_msg void OnConvertScript();
    afx_msg void OnUpdateIsScript(CCmdUI *pCmdUI);
    afx_msg void OnUpdateConvertScript(CCmdUI *pCmdUI);
    afx_msg void OnUpdateLineCount(CCmdUI *pCmdUI);

    // Other...
    void OnFileSave();
    void OnFileSaveAs();
    void _DoErrorSummary(ICompileLog &log);
    void _ClearErrorCount();
    void _OnUpdateTitle();

    CSCITextBuffer _buffer;
    ScriptId _scriptId;
    DependencyTracker *_dependencyTracker;
};

void DisassembleScript(WORD wScript);
class IDecompilerResults;
class GameFolderHelper;
void DecompileScript(const GameFolderHelper &helper, WORD wScript, IDecompilerResults &results);
