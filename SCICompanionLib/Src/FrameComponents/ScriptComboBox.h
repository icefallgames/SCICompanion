#pragma once

#include "NonViewClient.h"
#include "CompileInterfaces.h"

// Fwd declarations
class CScriptDocument;
namespace sci
{
    class Script;
    class SyntaxNode;
};

// CScriptComboBox

class CScriptComboBox : public CExtComboBox, public INonViewClient
{
public:
	CScriptComboBox();
	virtual ~CScriptComboBox();

    void SetDocument(CDocument *pDoc);

    // INonViewClient
    virtual void UpdateNonView(CObject *pObject) override;

protected:
	DECLARE_MESSAGE_MAP()
    void OnTimer(UINT_PTR nIDEvent);
    sci::Script *_CreateScript(CPoint &pt);
    int _AddItem(const std::string &str, sci::SyntaxNode *pPos);
    void DrawItem(LPDRAWITEMSTRUCT pDIStruct);
    void MeasureItem(LPMEASUREITEMSTRUCT lpMIStruct);
    void _ParseAndUpdate(bool fForce = false);

    // Override
    virtual void _OnUpdateFromScript(const sci::Script *pScript, CPoint pt) = 0;
    virtual void OnSelChange() = 0;
    virtual void OnCloseUp() = 0;
    virtual void OnDropDown() = 0;

    CScriptDocument *_pDoc;
    bool _fDroppedDown;
    bool _fIgnorePosChanged;

    // Note: this object must be kept in sync with the itemdata in the combobox.
    std::unique_ptr<sci::Script> _script;

    CImageList _imageList;
};

class CClassComboBox : public CScriptComboBox
{
protected:
    virtual void _OnUpdateFromScript(const sci::Script *pScript, CPoint pt);
    virtual void OnSelChange();
    virtual void OnCloseUp();
    virtual void OnDropDown();
};

/*
class CMethodComboBox : public CScriptComboBox
{
protected:
    virtual void _OnUpdateFromScript(const sci::Script *pScript, CPoint pt);
    virtual void OnSelChange(NMHDR *pnmhdr, LRESULT *plResult);
};
*/
