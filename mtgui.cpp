//---------------------------------------------------------------------------
//
// Name:        mtgui.cpp
// Author:      Paulo A. S. Nuin
// Created:     05/08/2004 1:20:04 PM
// Copyright:
//
//---------------------------------------------------------------------------
#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "mtgui.h"

/**
 * \defgroup Variables Global variables
 * All global defined variables
 */
/*@{*/ 
/**  file types for open and save file dialogs - score files */
static const wxChar *FILETYPES = _T("Score files (*.scores)|*.scores|" "Text files (*.txt)|*.txt|" "All files (*.*)|*.*");
/**  file types for open and save file dialogs - nexus files */
static const wxChar *FILETYPES3 = _T("Nexus files (*.nex)|*.nex|" "Text files (*.txt)|*.txt|" "All files (*.*)|*.*");
#ifdef __WXMSW__
/**  ModelTest version 3.7 - file dialog */
	static const wxChar *FILETYPES2 = _T("ModelTest v3.7 (Modeltest3.7.win.exe)|Modeltest3.7.win.exe|" "ModelTest v3.6 (Modeltest3.6.exe)|Modeltest3.6.exe|" "ModelTest v3.5 (Modeltest3.5.win.exe)|Modeltest3.5.win.exe");
/**  MrModelTest version 2.2 - file dialog */
	static const wxChar *FILETYPES5 = _T("MrModelTest v2.2 (mrmodeltest2.exe)|mrmodeltest2.exe|" "All files (*.*)|*.*");
/**  PAUP Windows CLI version - file dialog */
	static const wxChar *FILETYPES4 = _T("PAUP Windows console version (win-paup4b10-console.exe)|*.exe|" "All files (*.*)|*.*");
#endif

double debugL = 0, 

alphaL = 0, 

sampleSize = 0, 

numberTaxa = 0, 

lrtC, 

aicC;

wxString filePathSelected, 

filePathSelectedMr,

fileSelected, 

fileSelectedMr,

fileToSave,

modelP, 

paupP, 

paupDir, 

mrModelP, 

bayesP, 

bayesDir;

string bl, 

mrbl;
/**  BIC check box state - selected or not */
bool BIC = false;
/*@}*/

IMPLEMENT_APP(modelApp)

//----------------------------------------------------------------------------
// modeltest
//----------------------------------------------------------------------------

    ////Event Table Start
BEGIN_EVENT_TABLE(modeltest,wxFrame)
	EVT_BUTTON(fileBt, modeltest::fileSelect)
	EVT_BUTTON(fileMrBt, modeltest::fileSelectMr)

	EVT_BUTTON(modelBt, modeltest::runTest)
	EVT_BUTTON(mrBt, modeltest::runTestMr)

	EVT_BUTTON(pathBt, modeltest::setPath)
	EVT_BUTTON(mrpathBt, modeltest::setPathMr)

	EVT_BUTTON(pathPaup, modeltest::setPaupPath)

	EVT_BUTTON(paramBt, modeltest::setParameter)
	EVT_BUTTON(helpMT, modeltest::OnHelp)
	EVT_BUTTON(nexus, modeltest::editNexus)
	
	EVT_BUTTON(paupBt, modeltest::runP)
	EVT_BUTTON(bayesBt, modeltest::runP2)


	EVT_BUTTON(saveScBt, modeltest::scoresSave)
	EVT_BUTTON(saveScBtMr, modeltest::scoresSaveMr)

	EVT_MENU(mAbout, modeltest::OnMenuAbout)
	EVT_CLOSE(modeltest::modeltestClose)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(modelPar,wxDialog)
	EVT_BUTTON(buttonCancel, modelPar::modelParClose)
	EVT_BUTTON(buttonOK, modelPar::modelParOK)
END_EVENT_TABLE()
    ////Event Table End

void readBlock()
{
	char line[500];
	ifstream block("block.dat");
	ifstream mrblock("mrblock.dat");
	while(!block.eof())
	{
		block.getline(line, 500);
		bl += line;
		bl.append("\n");
	}
	while(!mrblock.eof())
	{
		mrblock.getline(line, 500);
		mrbl += line;
		mrbl.append("\n");
	}
	mrblock.close();
	block.close();
}

bool modelApp::OnInit()
{
	readBlock();
	wxImage::AddHandler(new wxPNGHandler);
	wxBitmap bitmap;
	wxString image = "mtguisplash.png";

	if (bitmap.LoadFile(image, wxBITMAP_TYPE_PNG))
	{
		wxSplashScreen* splash = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT, 2000, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxSTAY_ON_TOP);
	}

	modeltest *window = new modeltest( NULL, 1,_T("MrModelTest GUI"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER));

	wxMenu *menuAbout = new wxMenu;
	//wxMenu *menuAn = new wxMenu;

	menuAbout->Append(mAbout, "About MrMTgui");
	wxMenuBar *menubar = new wxMenuBar;
	menubar->Append(menuAbout, _T("&About"));

	window->SetMenuBar(menubar);	
#ifdef __WXMSW__
//	window->GetMSHtmlHelpController().Initialize(_T("MTguiHelp.chm"));
#endif

	window->Show(TRUE);
	return true;
}

modeltest::modeltest( wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style )
: wxFrame( parent, id, title, position, size, style)
{
    CreateGUIControls();
}

modeltest::~modeltest()
{
    //wxMessageBox("Closing");
} 

void modeltest::OnMenuAbout(wxCommandEvent& event)
{
	wxImage::AddHandler(new wxPNGHandler);
	wxBitmap bitmap;
	wxString image = "mtguisplash.png";
	if (bitmap.LoadFile(image, wxBITMAP_TYPE_PNG))
	{
		wxSplashScreen* splash = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT, 2000, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxSTAY_ON_TOP);
	}
}

void modeltest::CreateGUIControls(void)
{
	//char line[1000];
	wxString title = "MrMTgui";

	this->SetSize(8,8,700,450);
	this->SetTitle(wxString(title));
	this->Center();

	wxIcon icon;
	wxBitmap bitmap(wxT("mtgui.png"), wxBITMAP_TYPE_PNG);
	icon.CopyFromBitmap(bitmap);
	this->SetIcon(icon);
	
	wxPanel *mainPanel = new wxPanel(this, mpanel, wxDefaultPosition, wxDefaultSize);

	wxStaticText *MT = new wxStaticText(mainPanel, -1,"ModelTest", wxPoint(16,10),wxDefaultSize);
	wxStaticText *MrMT = new wxStaticText(mainPanel, -1,"MrModelTest", wxPoint(580,10),wxDefaultSize);

	//ModelTest "panel" - left side///////////////////////////////////////////////////////////////////
	runPAUP =  new wxButton(mainPanel, paupBt, _("Run PAUP") ,wxPoint(16, 30) ,wxSize(95,30));
	selectFile =  new wxButton(mainPanel, fileBt, _("Select file ...") , wxPoint(16,70),wxSize(95,30));
	parameters = new wxButton(mainPanel, paramBt, _("Parameters"), wxPoint(16,110), wxSize(95,30));
	runModel =  new wxButton(mainPanel, modelBt, _("Modeltest !!") , wxPoint(16,150),wxSize(95,30));
	runModel->SetToolTip(wxT("Run modeltest and get your results"));
	checkBIC = new wxCheckBox(mainPanel, -1, _T("Use BIC (-b)"), wxPoint(16, 190), wxDefaultSize);

	inNexus =  new wxButton(mainPanel, nexus, _("Edit NEXUS") ,wxPoint(16, 230) ,wxSize(95,30));
	inNexus->Enable(false);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	//MrModelTest "panel"- right side////////////////////////////////////////////////////////////////
	runBayes =  new wxButton(mainPanel, bayesBt, _("Run Paup") ,wxPoint(580, 30) ,wxSize(95,30));
	selectFileMr =  new wxButton(mainPanel, fileMrBt, _("Select file ...") , wxPoint(580,70),wxSize(95,30));
	//parameters = new wxButton(mainPanel, paramBt, _("Parameters"), wxPoint(16,100), wxSize(95,30));
	runMr =  new wxButton(mainPanel, mrBt, _("MrModeltest !!!") , wxPoint(580,150),wxSize(95,30));
	runMr->SetToolTip(wxT("Run MrModeltest and get your results"));
	inMrNexus =  new wxButton(mainPanel, mrNexus, _("Edit NEXUS") ,wxPoint(580, 190) ,wxSize(95,30));
	inMrNexus->Enable(false);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	modelpath =  new wxButton(mainPanel, pathBt, _("MT path") ,wxPoint(120, 350) ,wxSize(95,30) );
	modelpath->Enable(false);
	pauppath = new wxButton(mainPanel, pathPaup, _("PAUP path") ,wxPoint(225, 350), wxSize(95,30));
	pauppath->Enable(false);

	mrpath = new wxButton(mainPanel, mrpathBt, _("MrMT path") ,wxPoint(478, 350) ,wxSize(95,30));
	mrpath ->Enable(false);
#ifdef __WXMSW__
	char line[1000];
	modelpath->Enable(true);
	pauppath->Enable(true);
	//inNexus->Enable(true);
	//bayespath->Enable(true);
	mrpath->Enable(true);
#endif

	saveScores = new wxButton(mainPanel, saveScBt, _("Save scores") ,wxPoint(16, 270) ,wxSize(95,30));
	this->saveScores->Enable(false);
	saveScoresMr = new wxButton(mainPanel, saveScBtMr, _("Save scores") ,wxPoint(580, 230) ,wxSize(95,30));
	this->saveScoresMr->Enable(false);

	help =  new wxButton(mainPanel, helpMT, _("Help") ,wxPoint(16, 350) ,wxSize(95,30) );

	outputText =  new wxTextCtrl(mainPanel, outTxt, "" , wxPoint(120,20),wxSize(450,280) , wxVSCROLL | wxHSCROLL |wxTE_MULTILINE | wxTE_READONLY );
	outputText->SetToolTip(wxT("Your results will be displayed here"));
	outputText->AppendText(_("Your result will be displayed here"));
	outputText->SetFont(wxFont(8, wxMODERN, wxNORMAL, wxNORMAL, FALSE, _T(""), wxFONTENCODING_SYSTEM));
	//selectNexusStatic =  new wxStaticText(mainPanel, ID_WXSTATICTEXT1 ," Select your PAUP result file clicking on the Select button.\ After running ModelTest, you can select your Nexus file to automatically include the resulting PAUP block by clicking on Edit Nexus File." ,wxPoint(16,10),wxSize(480,30));

#ifdef __WXMSW__
	ifstream mpath("modelpath.ini");
	ifstream mrpath("MrModelpath.ini");
	ifstream ppath("pauppath.ini");

	if(!mpath)
	{
		wxMessageBox("The path to ModelTest3* executable is not set.\nYou can do it by clicking the button MT path");
		this->runModel->Enable(false);   
	}
	else
	{
		mpath.getline(line, 1000);
		modelP = line;
	}
	mpath.close();

	if(!mrpath)
	{
		wxMessageBox("The path to MrModelTest2.2 executable is not set.\nYou can do it by clicking the button MrMT path");
		this->runMr->Enable(false); 
	}
	else
	{
		mrpath.getline(line, 1000);
		mrModelP = line;
	}

	if(!ppath)
	{
		this->outputText->WriteText("\n\n\nThe path to PAUP is not set.\n You can do it by clicking the button PAUP path");
		this->runPAUP->Enable(false);
		this->runBayes->Enable(false);
	}
	else
	{
		ppath.getline(line, 1000);
		paupP = line;
		ppath.getline(line, 1000);
		paupDir = line;
	}
	mpath.close();
	mrpath.close();
	ppath.close();
#endif
}

void modeltest::runP(wxCommandEvent& event)
{
	string toinclude1 = "\nBEGIN PAUP;\nexecute \'";
	string toinclude2 = "\';\nEnd;";
	string toinclude3 = "\nBEGIN PAUP;\nquit warntsave=no;\nEnd;";
	wxArrayString output, errors;
	wxString directory;
	string temp;

	wxFileDialog* openFileDialog = new wxFileDialog ( this, "Select NEXUS file to test in PAUP", "", "", FILETYPES3, wxOPEN, wxDefaultPosition);
	if (openFileDialog->ShowModal() == wxID_OK)
	{
		readBlock();
		ofstream paupfile("paupfile.txt");
		wxString fileP = openFileDialog->GetPath();
		directory = openFileDialog->GetDirectory();
		string to = toinclude1;
		//log file= modelfit.log replace;
		string to2 = "log file=\'";
		//to2 += directory;
		//to2 += "\\";
		to2 += fileP;
		to2 += ".log\' replace;\n";
		to += fileP;
		to += toinclude2;
		bl.insert(7, to);
		bl.insert(7+to.length()-4, to2);
		bl.insert(bl.length()-1, toinclude3);

		paupfile << bl;
		paupfile.close();

#ifdef __WXMSW__	
		ofstream pbat("p.bat");
		paupP.Prepend("\"");
		paupP.Append("\"");
		temp = paupP;
		pbat << paupP << " paupfile.txt";
		pbat.close();
#endif
		this->outputText->Clear();
		this->outputText->WriteText("PAUP is running on a separate process, please be patient.\n");
		this->outputText->WriteText("When it finishes this display will be updated.\n");
	//"C:\wxWidgets\projects\modelGUI\modeltest3.7\Modeltest3.7 folder\bin\Modeltest3.7.win.exe" < "C:\wxWidgets\projects\modelGUI\model.scores"
		
#ifdef __WXMSW__
		long exec = wxExecute("p.bat", wxEXEC_SYNC);
#else
		//long exec = wxExecute("ls", wxEXEC_SYNC);
		long exec = wxExecute("paup  paupfile.txt", wxEXEC_SYNC);
#endif
		remove("paupfile.txt");
		bl.clear();
		this->outputText->Clear();
		this->outputText->WriteText("PAUP run has finished.\nYou can save the scores file and automatically run ModelTest.");
		wxMessageDialog dialog( NULL, _T("Your scores file is ready.\nDo you want to run ModelTest? Click Yes to start the analysis\nor No to perform other actions (do not forget to save your scores file)."),_T("Run ModelTest??"), wxYES_DEFAULT|wxYES_NO|wxICON_QUESTION);
		if(dialog.ShowModal() == wxID_YES)
		{
			fileSelected =  paupDir + "model.scores";
			//fileSelected = fileP + "model.scores";
			modeltest::runTest(event);
		}
		else
		{
			fileToSave = fileP + ".scores";
		}
		this->saveScores->Enable(true);
	}
}

void modeltest::modeltestClose(wxCloseEvent& event)
{
    Destroy();
}

void modeltest::OnHelp(wxCommandEvent& event)
{
#ifdef __WXMSW__
	wxString where;

	where = wxGetCwd();
	where.Append("\\MrMTguiHelp.chm");
	m_help.Initialize(where);
	m_help.DisplayContents();
#endif
}

void modeltest::fileSelect(wxCommandEvent& event)
{
	wxFileDialog* openFileDialog = new wxFileDialog ( this, "Select file", "", "", FILETYPES, wxOPEN, wxDefaultPosition);
	if (openFileDialog->ShowModal() == wxID_OK)
	{
		filePathSelected = openFileDialog->GetPath();
		fileSelected = openFileDialog->GetFilename();
		this->outputText->Clear();
		this->outputText->WriteText("You selected:\t");
		this->outputText->WriteText(fileSelected);
	}
}

void modeltest::scoresSave(wxCommandEvent& event)
{
	wxString scoreFile;
	char line[1000];
	string scoref;

	//scoref = fileSelected + "model.scores";
	scoref = "model.scores";

	ifstream in(scoref.c_str());
	wxFileDialog* pathdialog = new wxFileDialog ( this, "Select file", "", "", FILETYPES, wxSAVE | wxOVERWRITE_PROMPT, wxDefaultPosition);
    if(pathdialog->ShowModal() == wxID_OK)
	{
		scoreFile = pathdialog->GetPath();
		if(scoreFile == fileToSave)
		{
			//wxMessageBox("MrMTgui creates security copies of your files in its home directory.\nYou are trying to overwrite this security copy.\nPlease try again and select a different location or filename");
		}
		else
		{
			if(!scoreFile.Contains(".score"))
			{
				scoreFile.Append(".score");
			}
			ofstream out(scoreFile.wx_str());
			while(!in.eof())
			{
				in.getline(line, 1000);
				out << line << "\n";
			}
			out.close();
			this->outputText->WriteText("\n\nScore file saved to ");
			this->outputText->WriteText(scoreFile);
		}
	}
	in.close();
}

void modeltest::runTest(wxCommandEvent& event)
{
	wxString cmd, cmd2, pa, ssize = "-n";
	int dL, sS, nT;
	wxArrayString output, errors;
	string cmdstr, cmdstr2;

	if(fileSelected == "")
	{
		wxMessageBox("Please select a file before running. \nClick on Select File button");
	}
	else
	{
		BIC = this->checkBIC->GetValue();
		if(BIC == true)
		{
			wxTextEntryDialog* samplesize = new wxTextEntryDialog(this, "Please enter the sample size:", "", "", wxOK, wxDefaultPosition);
			if (samplesize->ShowModal() == wxID_OK)
			{
				ssize += samplesize->GetValue();
			}
		}

#ifdef __WXMSW__
		ofstream batchfile("m.bat");
		cmd = modelP;
		cmd2 = modelP;
		cmd.Append("\""); cmd2.Append("\"");
#else
		ofstream batchfile("m.sh");
		cmd = "modeltest3.7 ";
		cmd2 = "modeltest3.7 ";
#endif
		if(debugL > 0)
		{
			cmd += " -d";
			cmd2 += " -d";
			dL = debugL;
			pa.Printf("%d", dL);
			cmd += pa;
			cmd2 += pa;
		}
		if(alphaL > 0)
		{
			cmd += " -a";
			cmd2 += " -a";
			pa.Printf("%lf", alphaL);
			cmd += pa;
			cmd2 += pa;
		}
		if(sampleSize > 0)
		{
			cmd += " -n";
			cmd2 += " -n";
			sS = sampleSize;
			pa.Printf("%d", sS);
			cmd += pa;
			cmd2 += pa;
		}
		if(numberTaxa > 0)
		{
			cmd += " -t";
			cmd2 += " -t";
			nT = numberTaxa;
			pa.Printf("%d", nT);
			cmd += pa;
			cmd2 += pa;
		}
		if(BIC == true)
		{
			cmd2 += " -b " + ssize;
		}
		cmd += " < ";
		cmd2 += " < ";
		if(filePathSelected == "")
		{
			filePathSelected = fileSelected;
		}
		
#ifdef __WXMSW__
		filePathSelected.Append("\"");
		filePathSelected.Prepend("\"");
		cmd.Prepend("\"");
		cmd2.Prepend("\"");
#endif
		cmd += filePathSelected;
		cmd2 += filePathSelected;
		//cmd += "model.scores";
		//cmd2 += "model.scores";
		//wxMessageBox(cmd2);
		cmdstr = cmd;
		cmdstr2 = cmd2;
		batchfile << cmdstr << "\n";
		if(BIC == true)
		{
			batchfile << cmdstr2;
		}
		batchfile.close();
#ifdef __WXMSW__
		long exec = wxExecute("m.bat", output, errors);
#else
		long exec = wxExecute("bash m.sh", output, errors);
#endif
		this->outputText->WriteText("\n" + cmd + "\n");
		int count = output.GetCount();
		for ( size_t n = 0; n < count; n++ )
		{
	        	this->outputText->WriteText(output[n] + "\n");
				this->inNexus->Enable(true);
				this->saveScores->Enable(true);
		}
		count = errors.GetCount();
		for ( size_t n = 0; n < count; n++ )
		{
	        	this->outputText->WriteText(errors[n] + "\n");
		}
		this->outputText->WriteText("\n\nMTgui designed by Paulo Nuin.\nMore info email me at nuin@genedrift.org");
	}
	debugL = 0;
	alphaL = 0; 
	sampleSize = 0;
	numberTaxa = 0;
}

void modeltest::setPath(wxCommandEvent& event)
{
#ifdef __WXMSW__
	string mp;
	wxFileDialog* dialog = new wxFileDialog ( this, "Select file", "", "", FILETYPES2, wxOPEN, wxDefaultPosition);

	if (dialog->ShowModal() == wxID_OK)
	{
        modelP = dialog->GetPath();
		ofstream rpa("modelpath.ini");
		mp = modelP;
        rpa << mp;
		rpa.close();
		this->runModel->Enable(true);
	}
#endif
}

void modeltest::setParameter(wxCommandEvent& event)
{
    modelPar *parwindow = new modelPar( NULL, 2, _T("Set Parameters"), wxDefaultPosition, wxDefaultSize, THIS_DIALOG_STYLE);
	parwindow->Show(true);
}

void modeltest::editNexus(wxCommandEvent& event)
{
	wxString wxline, nexusFile, nexP, finalB, nexTemp, temp, BICblock, AICblock, HLRTblock;
	char line[2000];
	string p1, p2, p3, sline; int pBpos, sel;
	bool pBlock = false, toIns = true;
	wxArrayString block;
	const wxString choices[] = { _T("hLRTs"), _T("AIC"), _T("BIC")};


	wxFileDialog* dialog = new wxFileDialog (this, "Select NEXUS file to insert PAUP block", "", "", FILETYPES3, wxOPEN, wxDefaultPosition);

    if (dialog->ShowModal() == wxID_OK)
    {
		nexusFile = dialog->GetPath();
		this->outputText->SaveFile("temp.txt");
		ifstream modelOut ("temp.txt");
	
		wxSingleChoiceDialog dialog(this, _T("Select LSET command to parse into NEXUS files"),_T("Please select one"), 3, choices);
		if(dialog.ShowModal() == wxID_OK)
		{
			sel = dialog.GetSelection();
			//if(sel == 1)
			//{
			//	toIns = false;
			//}

			/* getting PAUP blocks from modeltest result file*/
			while(!modelOut.eof())
			{
				modelOut.getline(line, 2000);
				wxline = line;
				if(wxline.Contains("PAUP* Commands Block"))
				{
						modelOut.getline(line, 2000);
						wxline = line;
						do
						{
							if(wxline.Length() >= 1)
							{
								block.Add(wxline+"\n");
							}
							temp += wxline + "\n";
							modelOut.getline(line, 2000);
							wxline = line;
						}while(!wxline.Contains("END"));
						block.Add("END;\n");
						temp += "END;\n";
				}
			}
			HLRTblock = block[0]+block[1]+block[2]+block[3]+block[4]+block[5];
			AICblock = block[6]+block[7]+block[8]+block[9]+block[10]+block[11];
			if(BIC == true)
			{
				BICblock = block[6]+block[7]+block[8]+block[9]+block[10]+block[11];
			}
			modelOut.close();
			/*inserting block on nexus file*/
			string nexustr;
			nexustr = nexusFile;
			ifstream nex(nexustr.c_str());
			nexTemp = nexusFile + "temp";
			ofstream nexT(nexTemp);
			while(!nex.eof())
			{
				nex.getline(line, 2000);
				wxline = line;
				nexT << line << "\n";
			}
			if(sel == 0)
			{
				p1 = HLRTblock;
				nexT << p1 << "\n";
			}
			else if(sel == 1)
			{
				p2 = AICblock;
				nexT << p2 << "\n";
			}
			else if(sel == 2 && BIC == true)
			{
				p3 = BICblock;
				nexT << p3 << "\n";
			}
			else if(sel == 2 && BIC == false)
			{
				wxMessageBox("BIC analysis not available.\nPlease rerun ModelTest with this option enabled");
			}
			nexT << line << "\n";
			nex.close();
			nexT.close();
			remove(nexusFile);
			rename(nexTemp, nexusFile);
		}
	}
}

wxString modeltest::checkBlock(wxString block)
{
	int lsetPos, semiPos;
	wxString fblock;

	if(block.Contains("Lset") || block.Contains("lset") || block.Contains("LSET"))
	{
		int pB = wxMessageBox("LSET command found on your PAUP block.\nDo you want to comment it?", "Paup Block", wxYES | wxNO);
		if(pB == wxYES)
		{
			lsetPos = block.Find("LSET");
			block.Replace("LSET", "[LSET", false);
			if(lsetPos == -1)
			{
				lsetPos = block.Find("Lset");
				block.Replace("Lset", "[Lset", false);
			}
			if(lsetPos == -1)
			{
				lsetPos = block.Find("lset");
				block.Replace("lset", "[lset", false);
			}
			if(lsetPos == 0)
			{
				semiPos = block.First(";");
				block.Replace(";", ";]", false);
			}
		}
	}
	return block;
}

void modeltest::saveResult(wxCommandEvent& event)
{
	wxString resFile;
	wxFileDialog* dialog = new wxFileDialog (this, "Select NEXUS file to insert PAUP block", "", "", "Text files (*.txt)|*.txt|" "All files (*.*)|*.*", wxSAVE, wxDefaultPosition);
    if (dialog->ShowModal() == wxID_OK)
    {
		resFile = dialog->GetPath();
		this->outputText->SaveFile(resFile);
	}
}

void modeltest::setPaupPath(wxCommandEvent& event)
{
#ifdef __WXMSW__
	string pp;
	wxFileDialog* pathdialog = new wxFileDialog ( this, "Select file", "", "", FILETYPES4, wxOPEN, wxDefaultPosition);
    if (pathdialog->ShowModal() == wxID_OK)
    {
        paupP = pathdialog->GetPath();
		wxString pdir = pathdialog->GetDirectory();
		pdir.append("\\");
		ofstream rpa("pauppath.ini");
		pp = paupP;
        rpa << pp << "\n" << pdir;
		rpa.close();
		this->runPAUP->Enable(true);
		this->runBayes->Enable(true);
	}

#endif
}

//MrModeltest/////////////////////
//////////////////////////////////////////////////////////////

void modeltest::fileSelectMr(wxCommandEvent& event)
{
	wxFileDialog* openFileDialog = new wxFileDialog ( this, "Select file", "", "", FILETYPES, wxOPEN, wxDefaultPosition);
	if (openFileDialog->ShowModal() == wxID_OK)
	{
		filePathSelectedMr = openFileDialog->GetPath();
		fileSelectedMr = openFileDialog->GetFilename();
		this->outputText->Clear();
		this->outputText->WriteText("You selected\t" + fileSelectedMr);
	}
}

void modeltest::setPathMr(wxCommandEvent& event)
{
#ifdef __WXMSW__
	string mp;
	
	wxFileDialog* dialog = new wxFileDialog(this, "Select file", "", "", FILETYPES5, wxOPEN, wxDefaultPosition);
	if (dialog->ShowModal() == wxID_OK)
	{
        mrModelP = dialog->GetPath();
		ofstream rpa("MrModelpath.ini");
		mp = mrModelP;
        rpa << mp;
		rpa.close();
		this->runMr->Enable(true);
	}
#endif
}
void modeltest::runP2(wxCommandEvent& event)
{
	string toinclude1 = "\nBEGIN PAUP;\nexecute \'";
	string toinclude2 = "\';\nEnd;";
	string toinclude3 = "\nBEGIN PAUP;\nquit warntsave=no;\nEnd;";
	wxArrayString output, errors;
	wxString directory;
	string temp;

	wxFileDialog* openFileDialog = new wxFileDialog ( this, "Select NEXUS file to test in PAUP", "", "", FILETYPES3, wxOPEN, wxDefaultPosition);
	if (openFileDialog->ShowModal() == wxID_OK)
	{
		readBlock();
		ofstream paupfile("paupfile.txt");
		wxString fileP = openFileDialog->GetPath();
		directory = openFileDialog->GetDirectory();
		string to = toinclude1;
		string to2 = "log file=\'";
		to2 += fileP;
		to2 += ".log\' replace;\n";
		to += + fileP;
		to += toinclude2;
		mrbl.insert(7, to);
		mrbl.insert(7+to.length()-4, to2);
		mrbl.insert(mrbl.length()-1, toinclude3);

		paupfile << mrbl;
		paupfile.close();

#ifdef __WXMSW__	
		ofstream pbat("p.bat");
		paupP.Prepend("\"");
		paupP.Append("\"");
		temp = paupP;
		pbat << paupP << " paupfile.txt";
		pbat.close();
#endif
		this->outputText->Clear();
		this->outputText->WriteText("PAUP is running on a separate process, please be patient.\n");
		this->outputText->WriteText("When it finishes this display will be updated.\n");
	//"C:\wxWidgets\projects\modelGUI\modeltest3.7\Modeltest3.7 folder\bin\Modeltest3.7.win.exe" < "C:\wxWidgets\projects\modelGUI\model.scores"
		
#ifdef __WXMSW__
		long exec = wxExecute("p.bat", wxEXEC_SYNC);
#else
		//long exec = wxExecute("ls", wxEXEC_SYNC);
		long exec = wxExecute("paup  paupfile.txt", wxEXEC_SYNC);
#endif
		remove("paupfile.txt");
		mrbl.clear();
		this->outputText->Clear();
		this->outputText->WriteText("PAUP run has finished.\nYou can save the scores file and automatically run MrModelTest.");
		wxMessageDialog dialog( NULL, _T("Your scores file is ready.\nDo you want to run MrModelTest? Click Yes to start the analysis\nor No to perform other actions (do not forget to save your scores file)."),_T("Run MrModelTest??"), wxYES_DEFAULT|wxYES_NO|wxICON_QUESTION);
		if(dialog.ShowModal() == wxID_YES)
		{
			fileSelectedMr =  paupDir + "mrmodel.scores";
			//fileSelected = fileP + "model.scores";
			modeltest::runTestMr(event);
		}
		else
		{
			fileToSave = fileP + ".scores";
		}
		this->saveScoresMr->Enable(true);
	}
}

void modeltest::runTestMr(wxCommandEvent& event)
{
	wxString cmd, pa;
	wxArrayString output, errors;
	string cmdstr;

	if(fileSelectedMr == "")
	{
		wxMessageBox("Please select a file before running. \nClick on Select File button");
	}
	else
	{

#ifdef __WXMSW__
		ofstream batchfile("mr.bat");
		cmd = mrModelP;
		cmd.Append("\"");
#else
		ofstream batchfile("mr.sh");
		cmd = "mrmodeltest2 ";
#endif
		/*if(debugL > 0)
		{
			cmd += " -d";
			dL = debugL;
			pa.Printf("%d", dL);
			cmd += pa;
		}
		if(alphaL > 0)
		{
			cmd += " -a";
			pa.Printf("%lf", alphaL);
			cmd += pa;	
		}
		if(sampleSize > 0)
		{
			cmd += " -c";
			sS = sampleSize;
			pa.Printf("%d", sS);
			cmd += pa;
		}
		if(numberTaxa > 0)
		{
			cmd += " -t";
			nT = numberTaxa;
			pa.Printf("%d", nT);
			cmd += pa;
		}*/
		cmd += " < ";
		if(filePathSelectedMr == "")
		{
			filePathSelectedMr = fileSelectedMr;
		}
		
#ifdef __WXMSW__
		filePathSelectedMr.Append("\"");
		filePathSelectedMr.Prepend("\"");
		cmd.Prepend("\"");
#endif
		cmd += filePathSelectedMr;
		cmdstr = cmd;
		batchfile << cmdstr;
		batchfile.close();
#ifdef __WXMSW__
		long exec = wxExecute("mr.bat", output, errors);
#else
		long exec = wxExecute("bash mr.sh", output, errors);
#endif
		this->outputText->WriteText("\n");
		this->outputText->WriteText(cmd);
		this->outputText->WriteText("\n");
		int count = output.GetCount();
		for ( size_t n = 0; n < count; n++ )
		{
	        	this->outputText->WriteText(output[n]);
			this->outputText->WriteText("\n");
				this->inNexus->Enable(true);
				//this->saveScores->Enable(true);
		}
		count = errors.GetCount();
		for ( size_t n = 0; n < count; n++ )
		{
	        	this->outputText->WriteText(errors[n]);
			this->outputText->WriteText("\n");
		}
		this->outputText->WriteText("\n\nMTgui designed by Paulo Nuin.\nMore info email me at nuin@genedrift.org");
	}
	debugL = 0;
	alphaL = 0; 
	sampleSize = 0;
	numberTaxa = 0;
}

void modeltest::scoresSaveMr(wxCommandEvent& event)
{
	wxString scoreFile; string sc;
	char line[1000];
	string scoref;

	//scoref = fileSelectedMr + "mrmodel.scores";
	scoref = "mrmodel.scores";

	ifstream in(scoref.c_str());
	wxFileDialog* pathdialog = new wxFileDialog ( this, "Select file", "", "", FILETYPES, wxSAVE | wxOVERWRITE_PROMPT, wxDefaultPosition);
    if(pathdialog->ShowModal() == wxID_OK)
	{
		scoreFile = pathdialog->GetPath();
		if(scoreFile == fileToSave)
		{
			//wxMessageBox("MrMTgui creates security copies of your files in its home directory.\nYou are trying to overwrite this security copy.\nPlease try again and select a different location or filename");
		}
		else
		{
			if(!scoreFile.Contains(".score"))
			{
				scoreFile.append(".score");
			}
			sc = scoreFile;
			ofstream out(sc.c_str());
			while(!in.eof())
			{
				in.getline(line, 1000);
				out << line << "\n";
			}
			out.close();
			this->outputText->WriteText("\n\nScore file saved to ");
			this->outputText->WriteText(scoreFile);
		}
	}
	in.close();
}
///////////////////Parameters for modeltest////////////////////////////////
void modelPar::CreateGUIControls(void)
{
	wxString title = "Select Parameters";

	this->SetSize(358,274,250,250);
	this->SetTitle(title);
	this->Center();
	this->SetIcon(wxNullIcon);
	
	cancelButton =  new wxButton(this, buttonCancel, _("Cancel") , wxPoint(144,184),wxSize(80,27));
	okButton =  new wxButton(this, buttonOK, _("OK") , wxPoint(24,184),wxSize(80,27));
	taxaNumber =  new wxTextCtrl(this, tNumber, _("") , wxPoint(112,112),wxSize(121,21));
	WxStaticText4 =  new wxStaticText(this, ID_WXSTATICTEXT4 ,_("Number of taxa") ,wxPoint(24,112),wxSize(76,41));
	sampleS =  new wxTextCtrl(this, sizeS, _("") , wxPoint(112,80),wxSize(121,21));
	WxStaticText3 =  new wxStaticText(this, ID_WXSTATICTEXT3 ,_("Sample size") ,wxPoint(24,75),wxSize(71,41));
	alphaLevel =  new wxTextCtrl(this, aLevel, _("") , wxPoint(112,48),wxSize(121,21) );
	WxStaticText2 =  new wxStaticText(this, ID_WXSTATICTEXT2 ,_("Alpha Level") ,wxPoint(24,48),wxSize(60,17));
	debugLevel =  new wxTextCtrl(this, dLevel, _("") , wxPoint(112,16),wxSize(121,21));
	WxStaticText1 =  new wxStaticText(this, ID_WXSTATICTEXT5 ,_("Debug Level") ,wxPoint(24,16),wxSize(65,17));    
}

modelPar::modelPar( wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style )
    : wxDialog( parent, id, title, position, size, style)
{
    CreateGUIControls();
}

void modelPar::modelParClose(wxCommandEvent& event)
{
	Destroy();
}

void modelPar::modelParOK(wxCommandEvent& event)
{
	wxString dL, aL, sS, nT;

	dL = debugLevel->GetValue();
	aL = alphaLevel->GetValue();
	sS = sampleS->GetValue();
	nT = taxaNumber->GetValue();

	dL.ToDouble(&debugL);
	aL.ToDouble(&alphaL);
	sS.ToDouble(&sampleSize);
	nT.ToDouble(&numberTaxa);

	Destroy();
}

