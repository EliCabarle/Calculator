#pragma once
#include <wx/wx.h>
#include <map>

using namespace std;

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);

private:
	void CreateControls();
	void SetupSizers();
	void OnButtonClicked(wxCommandEvent& event);

	enum Operand { LEFT_OPERAND, RIGHT_OPERAND };
	enum Operation { ADD, SUB, MUL, DIV };
	Operand currentOperand = LEFT_OPERAND;
	Operation currentOperation;

	double result = 0;
	wxString leftOp = "", rightOp = "";

	wxPanel* panel;
	wxTextCtrl* display;
	vector<wxButton*> buttons;
	map <wxString, int> labelIdMap;
	wxString currentText;
	wxString expression;
};

