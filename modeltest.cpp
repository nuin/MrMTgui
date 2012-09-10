//---------------------------------------------------------------------------
//
// Name:        modeltest.cpp
// Author:      Paulo A. S. Nuin
// Created:     05/08/2004 1:20:04 PM
// Copyright:
//
//---------------------------------------------------------------------------


/** \file modeltest.cpp
 * \brief Main source file. 
 *
 * This is the main source file and the. Implementation, all the constructors to all wxWidgets objects are detailed here.
 */


/*
 HELP

Modeltest options and Result

JC:    Jukes and Cantor 1969
K80:   Kimura 2-parameters, Kimura 1980 (also known as K2P)
TrNef: Tamura-Nei 1993 with equal base frequencies
K81:   Kimura 3-parameters, Kimura 1981 (also known as K3ST
TIM:   Transitional model with equal base frequencies
TVM:   Transversional model with equal base frequencies
SYM:   Symmetrical model, Zharkikh 1994
F81:   Felsenstein 1981
HKY:   Hasegawa-Kishino-Yano 1985
TrN:   Tamura-Nei 1993
K81uf: Kimura 3-parameters with unequal base frequencies
TIM:   Transitional model
TVM:   Transversional model
GTR:   General time reversible, Rodriguez et al 1990 (also known as REV)

I:   invariable sites     G: gamma distribution

Usage:   -d : debug level (e.g. -d2)
         -a : alpha level (e.g. -a0.01)
         -c : sample size or number of characters (all or just variable). For
 the use of AICc
         -t : number of taxa. Forces to include branch lengths as parameters
         -l : LRT calculator mode
         -i : AIC calculator mode
         -f : input from a file for obtaining AIC values
         -? : help
UNIX/WIN usage: modeltest3.5 [-d -a -c -t -l -i -f -?] < infile > outfile
*/



#ifdef __GNUG__
    #pragma implementation "modeltest.cpp"
#endif

/* for compilers that support precompilation
   includes "wx/wx.h" */

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "modeltest.h"

static const wxChar *FILETYPES = _T("ModelTest Score files (*.scores)|*.scores|" "Text files (*.txt)|*.txt|" "All files (*.*)|*.*");
static const wxChar *FILETYPES2 = _T("ModelTest v3.7 (Modeltest3.7.win.exe)|Modeltest3.7.win.exe|" "ModelTest v3.6 (Modeltest3.6.exe)|Modeltest3.6.exe|" "ModelTest v3.5 (Modeltest3.5.win.exe)|Modeltest3.5.win.exe");
static const wxChar *FILETYPES3 = _T("Nexus files (*.nex)|*.nex|" "Text files (*.txt)|*.txt|" "All files (*.*)|*.*");
static const wxChar *FILETYPES4 = _T("PAUP Windows console version (win-paup4b10-console.exe)|*.exe|" "All files (*.*)|*.*");

double debugL = 0, alphaL = 0, sampleSize = 0, numberTaxa = 0, lrtC, aicC;
wxString filePathSelected;
wxString fileSelected;
wxString fileToSave;
string bl;
wxString modelP, paupP, paupDir;

IMPLEMENT_APP(modelApp)

//----------------------------------------------------------------------------
// modeltest
//----------------------------------------------------------------------------

    ////Event Table Start
BEGIN_EVENT_TABLE(modeltest,wxFrame)
	EVT_BUTTON(fileBt, modeltest::fileSelect)
	EVT_BUTTON(modelBt, modeltest::runTest)
	EVT_BUTTON(pathBt, modeltest::setPath)
	EVT_BUTTON(pathPaup, modeltest::setPaupPath)
	EVT_BUTTON(paramBt, modeltest::setParameter)
	EVT_BUTTON(helpMT, modeltest::OnHelp)
	EVT_BUTTON(nexus, modeltest::editNexus)
	EVT_BUTTON(paupBt, modeltest::runP)
	EVT_BUTTON(saveScBt, modeltest::scoresSave)
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
	while(!block.eof())
	{
		block.getline(line, 500);
		bl += line;
		bl.append("\n");
	}
	block.close();
}

bool modelApp::OnInit()
{
	//wxInitAllImageHandlers();
	
	readBlock();
	wxImage::AddHandler(new wxPNGHandler);
	wxBitmap bitmap;
	if (bitmap.LoadFile("mtguisplash.png", wxBITMAP_TYPE_PNG))
	{
		wxSplashScreen* splash = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT, 2000, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxSTAY_ON_TOP);
	}

     // wxSplashScreen* splash = new wxSplashScreen(bitmap,wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,6000, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxSTAY_ON_TOP);

	modeltest *window = new modeltest( NULL, 1,_T("ModelTest GUI"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE);

	wxMenu *menuAbout = new wxMenu;
	//wxMenu *menuAn = new wxMenu;

	menuAbout->Append(mAbout, "About Mtgui");
	wxMenuBar *menubar = new wxMenuBar;
    menubar->Append(menuAbout, _T("&About"));

	window->SetMenuBar(menubar);

	window->GetMSHtmlHelpController().Initialize(_T("MTguiHelp.chm"));

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

void modeltest::CreateGUIControls(void)
{
	char line[1000];

    ////GUI Items Creation Start
	this->SetSize(8,8,680,450);
	this->SetTitle(wxString("MTgui"));
	this->Center();

	wxIcon icon;
	wxBitmap bitmap(wxT("mtgui.png"), wxBITMAP_TYPE_PNG);
	icon.CopyFromBitmap(bitmap);
	this->SetIcon(icon);
	
	wxPanel *mainPanel = new wxPanel(this, mpanel, wxDefaultPosition, wxDefaultSize);

	runPAUP =  new wxButton(mainPanel, paupBt, _("Run PAUP") ,wxPoint(16, 50) ,wxSize(95,30) );

	selectFile =  new wxButton(mainPanel, fileBt, _("Select file ...") , wxPoint(16,100),wxSize(95,30) );
	parameters = new wxButton(mainPanel, paramBt, _("Select parameters"), wxPoint(16,150), wxSize(95,30));
	
	runModel =  new wxButton(mainPanel, modelBt, _("Modeltest !!!") , wxPoint(16,200),wxSize(95,30) );
	runModel->SetToolTip(wxT("Run modeltest and get your results"));
	
	inNexus =  new wxButton(mainPanel, nexus, _("Edit NEXUS file") ,wxPoint(16, 250) ,wxSize(95,30));
	inNexus->Enable(false);

	modelpath =  new wxButton(mainPanel, pathBt, _("ModelTest path") ,wxPoint(120, 350) ,wxSize(95,30) );
	pauppath = new wxButton(mainPanel, pathPaup, _("PAUP path") ,wxPoint(225, 350) ,wxSize(95,30) );

	saveScores = new wxButton(mainPanel, saveScBt, _("Save scores file") ,wxPoint(16, 300) ,wxSize(95,30) );
	this->saveScores->Enable(false);

	help =  new wxButton(mainPanel, helpMT, _("Help") ,wxPoint(16, 350) ,wxSize(95,30) );

	outputText =  new wxTextCtrl(mainPanel, outTxt, "" , wxPoint(120,50),wxSize(520,280) , wxVSCROLL | wxHSCROLL |wxTE_MULTILINE | wxTE_READONLY );
	outputText->SetToolTip(wxT("Your results will be displayed here"));
	outputText->AppendText(_("Your result will be displayed here"));
	outputText->SetFont(wxFont(8, wxMODERN, wxNORMAL, wxNORMAL, FALSE, _T(""), wxFONTENCODING_SYSTEM));

	selectNexusStatic =  new wxStaticText(mainPanel, ID_WXSTATICTEXT1 ,_(" Select your PAUP result file clicking on the Select button.\ After running ModelTest, you can select your Nexus file to automatically include the resulting PAUP block by clicking on Edit Nexus File.") ,wxPoint(16,10),wxSize(480,30)  );
    ////GUI Items Creation End

	ifstream mpath("modelpath.ini");
	if(!mpath)
	{
		wxMessageBox("The path to ModelText3* executabe is not set.\n You can do it by clicking the button ModelTest path");
		this->runModel->Enable(false);   
	}
	else
	{
		mpath.getline(line, 1000);
		modelP = line;
		
	}
	mpath.close();

	ifstream ppath("pauppath.ini");
	if(!ppath)
	{
		this->outputText->WriteText("\n\n\nThe path to PAUP is not set.\n You can do it by clicking the button PAUP path");
		this->runPAUP->Enable(false);
	}
	else
	{
		ppath.getline(line, 1000);
		paupP = line;
		ppath.getline(line, 1000);
		paupDir = line;
	}
}

void modeltest::modeltestClose(wxCloseEvent& event)
{
    Destroy();
}

void modeltest::runP(wxCommandEvent& event)
{
	string toinclude1 = "\nBEGIN PAUP;\nexecute \'";
	string toinclude2 = "\';\nEnd;";
	string toinclude3 = "\nBEGIN PAUP;\nquit warntsave=no;\nEnd;";
	wxArrayString output, errors;
	wxString directory;

	wxFileDialog* openFileDialog = new wxFileDialog ( this, "Select NEXUS file to test in PAUP", "", "", FILETYPES3, wxOPEN, wxDefaultPosition);
	if (openFileDialog->ShowModal() == wxID_OK)
	{
		ofstream paupfile("paupfile.txt");
		wxString fileP = openFileDialog->GetPath();
		directory = openFileDialog->GetDirectory();
		string to = toinclude1;
		//log file= modelfit.log replace;
		string to2 = "log file=\'";
		//to2 += directory;
		//to2 += "\\";
		to2 += fileP;
		to2 += ".scores\' replace;\n";
		to += + fileP;
		to += toinclude2;
		bl.insert(7, to);
		bl.insert(7+to.length()-4, to2);

		bl.insert(bl.length()-1, toinclude3);

		paupfile << bl;
		paupfile.close();
	
		ofstream pbat("p.bat");
		paupP.Prepend("\"");
		paupP.Append("\"");
		string temp = paupP;

		pbat << paupP << " paupfile.txt";
		this->outputText->Clear();
		this->outputText->WriteText("PAUP is running on a separate process, please be patient.\n");
		this->outputText->WriteText("When it finishes this display will be updated.\n");
	//"C:\wxWidgets\projects\modelGUI\modeltest3.7\Modeltest3.7 folder\bin\Modeltest3.7.win.exe" < "C:\wxWidgets\projects\modelGUI\model.scores"
		pbat.close();
		long exec = wxExecute("p.bat", wxEXEC_SYNC);

		this->outputText->Clear();
		this->outputText->WriteText("PAUP run has finished.\nYou can save the scores file and\/or automatically run ModelTest.");
		wxMessageDialog dialog( NULL, _T("Your scores file is ready.\nDo you want to run ModelTest? Click Yes to start the analysis\nor No to perform other actions (do not forget to save your scores file)."),_T("Run ModelTest??"), wxYES_DEFAULT|wxYES_NO|wxICON_QUESTION);
		if(dialog.ShowModal() == wxID_YES)
		{
			//fileSelected =  paupDir + "model.scores";
			fileSelected = fileP + ".scores";
			modeltest::runTest(event);
		}
		else
		{
			fileToSave = fileP + ".scores";
		}
		this->saveScores->Enable(true);
	}
}

void modeltest::OnHelp(wxCommandEvent& event)
{
	//long exec = wxExecute("iexplore",  wxEXEC_ASYNC);
	wxString where;

	where = wxGetCwd();
	where.Append("\\MTguiHelp.chm");

	//system("Simprot1.0.chm");
	m_help.Initialize(where);
	m_help.DisplayContents();
}

void modeltest::fileSelect(wxCommandEvent& event)
{
	wxFileDialog* openFileDialog = new wxFileDialog ( this, "Select file", "", "", FILETYPES, wxOPEN, wxDefaultPosition);
	if (openFileDialog->ShowModal() == wxID_OK)
	{
		filePathSelected = openFileDialog->GetPath();
		fileSelected = openFileDialog->GetFilename();
		this->outputText->Clear();
		this->outputText->WriteText(fileSelected);
	}
}

void modeltest::scoresSave(wxCommandEvent& event)
{
	wxString scoreFile;
	char line[1000];

	ifstream in(fileToSave.wx_str());



wxFileDialog* pathdialog = new wxFileDialog ( this, "Select file", "", "", FILETYPES, wxSAVE | wxOVERWRITE_PROMPT, wxDefaultPosition);
	
    if(pathdialog->ShowModal() == wxID_OK)
	{
		scoreFile = pathdialog->GetPath();
		if(scoreFile == fileToSave)
		{
			wxMessageBox("MTgui creates security copies of your files in its home directory.\nYou are trying to overwrite this security copy.\nPlease try again and select a different location or filename");
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
			this->outputText->WriteText("Score file saved to " + scoreFile);
		}
	}


	in.close();
}

void modeltest::runTest(wxCommandEvent& event)
{
	wxString cmd, pa;
	int dL, sS, nT;
	if(fileSelected == "")
	{
		wxMessageBox("Please select a file before running. \nClick on Select File button");
	}
	else
	{
		//debugL = 0, alphaL = 0, sampleSize = 0, numberTaxa = 0, lrtC, aicC;
		string cmdstr;
		ofstream batchfile("m.bat");
		wxArrayString output, errors;
		cmd = modelP;
		cmd.Append("\"");
		if(debugL > 0)
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
		}
		cmd+= " < ";
		if(filePathSelected == "")
		{
			filePathSelected = fileSelected;
		}
		filePathSelected.Append("\"");
		filePathSelected.Prepend("\"");
		cmd += filePathSelected;
		cmd.Prepend("\"");
		cmdstr = cmd;
		//long exec = wxExecute(cmd, output, errors);
		batchfile << cmdstr;
		batchfile.close();
		//"\"C:\program files\etc\programtostart.exe"
		
		long exec = wxExecute("m.bat", output, errors);
		//long exec = wxExecute(cmd, output, errors);
		this->outputText->WriteText("\n" + cmd + "\n");
		int count = output.GetCount();
		
		for ( size_t n = 0; n < count; n++ )
		{
	        this->outputText->WriteText(output[n] + "\n");
		}
		count = errors.GetCount();
		for ( size_t n = 0; n < count; n++ )
		{
	        this->outputText->WriteText(errors[n] + "\n");
		}
		this->outputText->WriteText("\n\n MTgui designed by Paulo Nuin. More info email me at nuin@genedrift.org");
		this->inNexus->Enable(true);
	}
	debugL = 0;
	alphaL = 0; 
	sampleSize = 0;
	numberTaxa = 0;
}

void modeltest::setPath(wxCommandEvent& event)
{
	string mp;

	//wxDirDialog dialog(this, _T("Select R executable (Rcmd.exe) path"));
	wxFileDialog* pathdialog = new wxFileDialog ( this, "Select file", "", "", FILETYPES2, wxOPEN, wxDefaultPosition);
	//wxFileDialog* openFileDialog = new wxFileDialog ( this, "Select file", "", "", FILETYPES, wxOPEN, wxDefaultPosition);

    if (pathdialog->ShowModal() == wxID_OK)
    {
        modelP = pathdialog->GetPath();
		ofstream rpa("modelpath.ini");
		//ofstream rbat("r.bat");
		//whereR = rPath;
		mp = modelP;
        rpa << mp;
		rpa.close();
	}
}

void modeltest::setPaupPath(wxCommandEvent& event)
{
	string pp;

	//wxDirDialog dialog(this, _T("Select R executable (Rcmd.exe) path"));
	wxFileDialog* pathdialog = new wxFileDialog ( this, "Select file", "", "", FILETYPES4, wxOPEN, wxDefaultPosition);
	//wxFileDialog* openFileDialog = new wxFileDialog ( this, "Select file", "", "", FILETYPES, wxOPEN, wxDefaultPosition);

    if (pathdialog->ShowModal() == wxID_OK)
    {
        paupP = pathdialog->GetPath();
		wxString pdir = pathdialog->GetDirectory();
		pdir.append("\\");
		ofstream rpa("pauppath.ini");
		//ofstream rbat("r.bat");
		//whereR = rPath;
		pp = paupP;
        rpa << pp << "\n" << pdir;
		rpa.close();
	}

}

void modeltest::setParameter(wxCommandEvent& event)
{
    modelPar *parwindow = new modelPar( NULL, 2, _T("Set Parameters"), wxDefaultPosition, wxDefaultSize, THIS_DIALOG_STYLE);
	parwindow->Show(true);
}

void modeltest::editNexus(wxCommandEvent& event)
{
	wxString paup1, paup2, wxline, nexusFile, nexP, finalB, nexTemp;
	char line[2000];
	string p1, p2, sline; int pBpos, lPos, sel;
	bool pBlock = false, toIns = true;

	const wxString choices[] = { _T("hLRTs"), _T("AIC")} ;

	wxFileDialog* dialog = new wxFileDialog (this, "Select NEXUS file to insert PAUP block", "", "", FILETYPES3, wxOPEN, wxDefaultPosition);

    if (dialog->ShowModal() == wxID_OK)
    {
		nexusFile = dialog->GetPath();
		this->outputText->SaveFile("temp.txt");
		ifstream modelOut ("temp.txt");
	
		wxSingleChoiceDialog dialog(this, _T("Select LSET command to parse into NEXUS files"),_T("Please select one"),2, choices);

		if(dialog.ShowModal() == wxID_OK)
		{
			sel = dialog.GetSelection();
			if(sel == 1)
			{
				toIns = false;
			}
			/* getting PAUP blocks from modeltest result file*/
			while(!modelOut.eof())
			{
				modelOut.getline(line, 2000);
				wxline = line;
				if(wxline.Contains("PAUP* Commands Block"))
				{
					if(paup1 == "")
					{
						modelOut.getline(line, 2000);
						wxline = line;
						do
						{
							paup1 += wxline + "\n";
							modelOut.getline(line, 2000);
							wxline = line;
						}while(!wxline.Contains("END"));
						paup1 += "END;\n";
						p1 = paup1;
					}
					else
					{
						modelOut.getline(line, 2000);
						wxline = line;
						do
						{
							paup2 += wxline + "\n";
							modelOut.getline(line, 2000);
							wxline = line;
						}while(!wxline.Contains("END"));
						paup2 += "END;\n";
						p2 = paup2;
					}		
				}
			}
			modelOut.close();
			/*inserting block on nexus file*/
			fstream nex(nexusFile, ios_base::in | ios_base::out /*ios_base::ate |ios_base::app*/);
			nexTemp = nexusFile + "temp";
			ofstream nexT(nexTemp);
			while(!nex.eof())
			{
				nex.getline(line, 2000);
				wxline = line;
				wxline.UpperCase();
				nexT << line << "\n";
				if(wxline.Contains("BEGIN PAUP"))
				{
					pBpos = nex.tellg();
					int pB = wxMessageBox("PAUP block found on " + nexusFile + ".\nDo you want to include ModelTest result block in your file?", "Paup Block", wxYES | wxNO);
					if(pB == wxYES)
					{
						nex.seekp(pBpos);			
						nex.getline(line, 2000);
						wxline = line;
						do{
							nexP += wxline + "\n";
							nex.getline(line, 2000);
							wxline = line;
							if(wxline.Find("LSET") == 0 || wxline.Find("Lset") == 0  || wxline.Find("lset") == 0)
							{
								int pB = wxMessageBox("LSET command found on your PAUP block.\nDo you want to comment it?", "Paup Block", wxYES | wxNO);
								if(pB == wxYES)
								{
									wxline.Prepend("[");
									wxline.Append("]");
									sline = wxline;
									nexT << sline << "\n";
									nex.getline(line, 2000);

									if(toIns == true)
									{
										paup1.Replace("BEGIN PAUP;", "");
										paup1.Replace("END;", "");
										p1 = paup1;
										nexT << p1 << "\n";
									}
									else
									{
										paup2.Replace("BEGIN PAUP;", "");
										paup2.Replace("END;", "");
										p2 = paup2;
										nexT << p2 << "\n";
									}
								}
							}
							nexT << line << "\n";
						}while(!wxline.IsSameAs("END;", false));
					}
					pBlock = true;
					break;
					//finalB = checkBlock(nexP);
				}
			}
			if(pBlock == false)
			{
				if(toIns == true)
				{
					nexT << p1 << "\n";
				}
				else
				{
					nexT << p2 << "\n";
				}
			}
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

void modeltest::OnMenuAbout(wxCommandEvent& event)
{
	wxImage::AddHandler(new wxPNGHandler);
	wxBitmap bitmap;
	if (bitmap.LoadFile("mtguisplash.png", wxBITMAP_TYPE_PNG))
	{
		wxSplashScreen* splash = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT, 2000, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxSTAY_ON_TOP);
	}


}


void modelPar::CreateGUIControls(void)
{
    ////GUI Items Creation Start

	this->SetSize(358,274,259,275);
	this->SetTitle(wxString("Select Parameters"));
	this->Center();
	this->SetIcon(wxNullIcon);
	
	cancelButton =  new wxButton(this, buttonCancel, _("Cancel") , wxPoint(144,184),wxSize(80,27));
	okButton =  new wxButton(this, buttonOK, _("OK") , wxPoint(24,184),wxSize(80,27));
	taxaNumber =  new wxTextCtrl(this, tNumber, _("") , wxPoint(112,136),wxSize(121,21));
	WxStaticText4 =  new wxStaticText(this, ID_WXSTATICTEXT4 ,_("Number of taxa") ,wxPoint(24,144),wxSize(76,17));
	sampleS =  new wxTextCtrl(this, sizeS, _("") , wxPoint(112,88),wxSize(121,21));
	WxStaticText3 =  new wxStaticText(this, ID_WXSTATICTEXT3 ,_("Sample size or number of characters") ,wxPoint(24,80),wxSize(71,41));
	alphaLevel =  new wxTextCtrl(this, aLevel, _("") , wxPoint(112,48),wxSize(121,21) );
	WxStaticText2 =  new wxStaticText(this, ID_WXSTATICTEXT2 ,_("Alpha Level") ,wxPoint(24,48),wxSize(60,17));
	debugLevel =  new wxTextCtrl(this, dLevel, _("") , wxPoint(112,16),wxSize(121,21));
	WxStaticText1 =  new wxStaticText(this, ID_WXSTATICTEXT5 ,_("Debug Level") ,wxPoint(24,16),wxSize(65,17));
    
	//this->Show(true);
	////GUI Items Creation End
}

modelPar::modelPar( wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style ): wxDialog( parent, id, title, position, size, style)
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

