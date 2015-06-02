#pragma once

#include "resource.h"
#include "NonViewClient.h"
#include "PaletteEditorCommon.h"

class CResourceDocument;
struct PaletteComponent;
struct Cel;

class PaletteEditorDialog : public PaletteEditorCommon<CExtResizableDialog>, public INonViewClient
{
public:
    PaletteEditorDialog(PaletteComponent &palette, const std::vector<const Cel*> &cels);  // standard constructor
    PaletteEditorDialog();

    // Dialog Data
    enum { IDD = IDD_PALETTE_EDITOR };

    // INonViewClient
    void UpdateNonView(CObject *pObject) override;
    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedButtonSaverange();
};
