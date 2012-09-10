//---------------------------------------------------------------------------
//
// Name:        mtgui.h
// Author:      Paulo A. S. Nuin
// Created:     05/08/2004 1:20:04 PM
// Copyright:
//
//---------------------------------------------------------------------------

/*! \mainpage MrMtGUI
 * \section Introduction
 * MrMtGUI is a cross-platform interface for the widely used software
 * ModelTest by David Posada
*/


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
#ifdef __WXMSW__
	#include <wx/msw/helpchm.h>
#endif

using namespace std; 
#ifndef __MODELTEST_HPP_
#define __MODELTEST_HPP_

#define ID_WXSTATICTEXT1 1001
#define THIS_DIALOG_STYLE 0

//! Main App Class
/*! Declaration of the application class
*/
class modelApp : public wxApp
{
public:
    bool OnInit();
};

//! Main Frame Class
/*! Declaration of the main frame class
*/
class modeltest : public wxFrame
{
public:
    modeltest( wxWindow *parent, wxWindowID id = 1, const wxString &title = _T("ModelTest GUI"),
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize,
    long style = THIS_DIALOG_STYLE);
    virtual ~modeltest();
	
	wxButton *help;				//!< button that starts help
	wxButton *runModel;			//!< button that runs ModelTest
	wxButton *runPAUP;			//!< button that runs PAUP
	wxButton *inNexus;			//!< button that inserts resulting block in a nexus file
	wxTextCtrl *outputText;			//!< main textcontrol that receives ModelTest main output
	wxButton *selectFile;			//!< button that allows selection of ModelTest scores file
	wxButton *parameters;			//!< calls the dialog to set parameters
	wxButton *modelpath;			//!< button that opens the dialog to set MT's path
	wxButton *pauppath;			//!< button that opens the dialog to set PAUP's path
	wxStaticText *selectNexusStatic;	
	wxButton* saveScores;			//!< saves the final score file
	wxButton *runBayes;			//!< deprecated
	wxButton *runMr;			//!< button that runs MrModelTest 
	wxButton *mrpath;			//!< button that sets the path to MrModeltest
	wxButton *selectFileMr;			//!< button that selects the MrModeltest scores file
	wxButton *inMrNexus;			//!< button that inserts resulting block from MrModeltest into nexus file
	wxButton *saveScoresMr;			//!< button that saves the scores file from MrModeltest
	wxCheckBox *checkBIC;			//<!

	wxStaticText *MT;
	wxStaticText *MrMT;

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
	void runP2(wxCommandEvent& event);

	wxString checkBlock(wxString block);
	void OnMenuAbout(wxCommandEvent& event);

	void scoresSave(wxCommandEvent& event);
	void scoresSaveMr(wxCommandEvent& event);

	void saveResult(wxCommandEvent& event);
	
	void fileSelectMr(wxCommandEvent& event);
	void setPathMr(wxCommandEvent& event);
	void runTestMr(wxCommandEvent& event);

#ifdef __WXMSW__
	wxCHMHelpController  m_msHtmlHelp;
	wxCHMHelpController& GetMSHtmlHelpController() { return m_msHtmlHelp; }
#endif

private:
	wxHelpController m_help;
	DECLARE_EVENT_TABLE()
};

//! Parameters Dialog Class
/*! Declaration of the parameters dialog class 
*/
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
	saveScBt,
	bayesBt,
	fileMrBt,
	mrBt,
	mrpathBt,
	mrNexus,
	saveScBtMr

};
#endif

