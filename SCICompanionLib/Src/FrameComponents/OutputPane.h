#pragma once

#include "CompileInterfaces.h"

// OutputPane

//
// Pane that contains find results and compile results.
//
class OutputPane : public CListBox
{
	DECLARE_DYNAMIC(OutputPane)

public:
	OutputPane();
	virtual ~OutputPane();

    void ClearResults();
    void AddBatch(std::vector<CompileResult> &compileResults);
    void FinishAdd();

protected:
	DECLARE_MESSAGE_MAP()
    int OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
    afx_msg void OnCopy();
    afx_msg void OnDblClick();

    std::vector<CompileResult> _compileResults;
};


