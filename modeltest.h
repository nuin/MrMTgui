//---------------------------------------------------------------------------
//
// Name:        mtgui.h
// Author:      Paulo A. S. Nuin
// Created:     05/08/2004 1:20:04 PM
// Copyright:
//
//---------------------------------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <string>
#include <fstream>
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/image.h>
#include <wx/splash.h>
#include <wx/mediactrl.h>
#include <wx/help.h>
#include "wx/msw/helpchm.h"

using namespace std; 
#ifndef __MODELTEST_HPP_
#define __MODELTEST_HPP_

#define ID_WXSTATICTEXT1 1001
////GUI Control ID End

////Dialog Style Start
	#define THIS_DIALOG_STYLE 0
////Dialog Style End

class modelApp : public wxApp
{
public:
    bool OnInit();
};

class modeltest : public wxFrame
{
public:
    modeltest( wxWindow *parent, wxWindowID id = 1, const wxString &title = _T("ModelTest GUI"),
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize,
    long style = THIS_DIALOG_STYLE);
    virtual ~modeltest();
	
	wxButton *help;
	wxButton *runModel;
	wxButton *runPAUP;
	wxButton *inNexus;
	wxTextCtrl *outputText;
	wxButton *selectFile;
	wxButton *parameters;
	wxButton *modelpath;
	wxButton *pauppath;
	wxStaticText *selectNexusStatic;
	wxButton* saveScores;

	void modeltestClose(wxCloseEvent& event);
	void CreateGUIControls(void);
	void fileSelect(wxCommandEvent& event);
	void runTest(wxCommandEvent& event);
	void setPath(wxCommandEvent& event);
	void setPaupPath(wxCommandEvent& event);
	void setParameter(wxCommandEvent& event);
	void OnHelp(wxCommandEvent& event);
	void editNexus(wxCommandEvent& event);
	void runP(wxCommandEvent& event);
	wxString checkBlock(wxString block);
	void OnMenuAbout(wxCommandEvent& event);
	void scoresSave(wxCommandEvent& event);
	wxCHMHelpController  m_msHtmlHelp;

	wxCHMHelpController& GetMSHtmlHelpController() { return m_msHtmlHelp; }
private:
	wxHelpController m_help;
    DECLARE_EVENT_TABLE()
};


class modelPar : public wxDialog
{
public:
    modelPar( wxWindow *parent, wxWindowID id = 1, const wxString &title = _T("Set Parameters"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = THIS_DIALOG_STYLE);
public:

	wxButton *okButton;
	wxButton *cancelButton;
	wxTextCtrl *taxaNumber;
	wxStaticText *WxStaticText4;
	wxTextCtrl *sampleS;
	wxStaticText *WxStaticText3;
	wxTextCtrl *alphaLevel;
	wxStaticText *WxStaticText2;
	wxTextCtrl *debugLevel;
	wxStaticText *WxStaticText1;

	
private:
    DECLARE_EVENT_TABLE()

public:
    void modelParClose(wxCommandEvent& event);
    void CreateGUIControls(void);
	void modelParOK(wxCommandEvent& event);

};
enum
{
	mpanel,
	window,
	buttonOK,
	buttonCancel,
	tNumber,
	sizeS,
	aLevel,
	ID_WXSTATICTEXT4,
	ID_WXSTATICTEXT5,
	ID_WXSTATICTEXT3,
	ID_WXSTATICTEXT2,
	dLevel,
	parwindow,
	helpMT,
	nexus,
	modelBt,
	paupBt,
	paramBt,
	pathBt,
	outTxt,
	fileBt,
	mAbout,
	pathPaup,
	saveScBt

};
#endif
 
 
 
 
