#include "MainFrame.h"
#include <wx/wx.h>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <cctype>

using namespace std;

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
	CreateControls();
	SetupSizers();
}

void MainFrame::CreateControls()
{
	panel = new wxPanel(this);
	panel->SetBackgroundColour(wxT("LIGHTPINK"));

    wxFont displayFont(wxFontInfo(wxSize(0,30)));
	display = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, 50), wxTE_RIGHT | wxTE_READONLY);
    display->SetBackgroundColour(wxT("LIGHTBLUE"));
	display->SetFont(displayFont);

	wxString label[19] = { "*", "/", "-", "+", "9", "8", "7", "6", "5", "4", "3", "2", "1", "0", ".", "=", "+/-", "clr", "<-"};

	wxColor buttonColor(wxT("SKYBLUE"));

	for (int i = 0; i < 19; i++) {
		int id = wxNewId();
		labelIdMap[label[i]] = id;

		wxButton* button = new wxButton(panel, id, label[i], wxDefaultPosition, wxSize(50, 50));
		buttons.push_back(button);

		button->SetBackgroundColour(buttonColor);
		button->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);
	}
}

double evaluateExpression(const string& expr) {
    istringstream in(expr);
    vector<string> output;
    stack<char> ops;
    map<char, int> precedence = { {'+', 1}, {'-', 1}, {'*', 2}, {'/', 2} };
    string token;

    // Tokenize and convert to postfix
    while (in >> ws) {
        // Check for unary minus (negative number)
        if ((in.peek() == '-' && (output.empty() || output.back() == "+" || output.back() == "-" || output.back() == "*" || output.back() == "/"))
            || isdigit(in.peek()) || in.peek() == '.') {
            token.clear();
            if (in.peek() == '-') {
                token += in.get(); // consume '-'
            }
            while (isdigit(in.peek()) || in.peek() == '.') {
                token += in.get();
            }
            output.push_back(token);
        }
        else if (precedence.count(in.peek())) {
            char op = in.get();
            while (!ops.empty() && precedence[ops.top()] >= precedence[op]) {
                output.push_back(string(1, ops.top()));
                ops.pop();
            }
            ops.push(op);
        }
        else {
            in.get(); // skip unknown
        }
    }

    // <-- THIS BLOCK MUST BE BEFORE THE EVALUATION LOOP
    while (!ops.empty()) {
        output.push_back(string(1, ops.top()));
        ops.pop();
    }

    // Evaluate postfix
    stack<double> vals;
    for (const auto& t : output) {
        if (t == "+" || t == "-" || t == "*" || t == "/") {
            double b = vals.top(); vals.pop();
            double a = vals.top(); vals.pop();
            if (t == "+") vals.push(a + b);
            else if (t == "-") vals.push(a - b);
            else if (t == "*") vals.push(a * b);
            else if (t == "/") vals.push(a / b);
        }
        else {
            vals.push(stod(t));
        }
    }
    return vals.top();
}

void MainFrame::OnButtonClicked(wxCommandEvent& event)
{
    int buttonId = event.GetId();

    wxString pressedLabel;
    for (const auto& pair : labelIdMap) {
        if (pair.second == buttonId) {
            pressedLabel = pair.first;
            break;
        }
    }

    if (pressedLabel == "+/-") {
        int lastOp = -1;
        for (int i = expression.Length() - 1; i >= 0; --i) {
            if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/') {
                lastOp = i;
                break;
            }
        }
        // Extract the last number
        wxString lastNum = expression.Mid(lastOp + 1);
        wxString before = expression.Left(lastOp + 1);

        // Toggle sign
        if (lastNum.StartsWith("-")) {
            lastNum = lastNum.Mid(1); // Remove negative sign
        }
        else {
            lastNum = "-" + lastNum; // Add negative sign
        }
        expression = before + lastNum;
        display->SetValue(expression);
        return;
    }

    if (pressedLabel == "clr") {
        display->Clear();
        expression.clear();
        return;
    }

    if (pressedLabel == "<-") {
        if (!expression.IsEmpty()) {
            expression.RemoveLast();
            display->SetValue(expression);
        }
        return;
    }

    if (pressedLabel == "=") {
        double result = evaluateExpression(expression.ToStdString());
        display->SetValue(wxString::Format("%g", result));
        expression = wxString::Format("%g", result); // allow chaining
        return;
    }

    // For numbers, operators, dot
    expression += pressedLabel;
    display->SetValue(expression);
}

void MainFrame::SetupSizers()
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* displaySizer = new wxBoxSizer(wxHORIZONTAL);
	displaySizer->Add(display, wxSizerFlags().Proportion(1));
	
    wxGridSizer* gridSizer = new wxGridSizer(0, 4, 0, 0);
	wxSizerFlags flags = wxSizerFlags().Expand().Proportion(1).CenterHorizontal().Border(wxALL, 5);
	
    for (wxButton* button : buttons) {
		gridSizer->Add(button, flags);
	}

	mainSizer->Add(displaySizer, 0, wxEXPAND | wxALL, 10);
	mainSizer->Add(gridSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 10);
	
    panel->SetSizer(mainSizer);
	mainSizer->SetSizeHints(this);
}

	//Stack Data Structure
	//pop - removes the top element from the stack
	//push - adds an element to the top of the stack