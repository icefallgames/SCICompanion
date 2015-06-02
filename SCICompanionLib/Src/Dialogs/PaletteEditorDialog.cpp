#include "stdafx.h"
#include "PaletteEditorDialog.h"
#include "ExtColorDlg.h"
#include "PaletteOperations.h"
#include "RasterOperations.h"
#include "ImageUtil.h"
#include "ResourceDocument.h"
#include "ResourceEntity.h"

// CChooseColorDialog dialog

PaletteEditorDialog::PaletteEditorDialog(PaletteComponent &palette, const std::vector<const Cel*> &cels)
    : PaletteEditorCommon<CExtResizableDialog>(PaletteEditorDialog::IDD, nullptr)
{
    Init(palette, cels);
}

// INonViewClient
void PaletteEditorDialog::UpdateNonView(CObject *pObject)
{
    UpdateCommon(pObject);
}
BEGIN_MESSAGE_MAP(PaletteEditorDialog, PaletteEditorCommon)
END_MESSAGE_MAP()

