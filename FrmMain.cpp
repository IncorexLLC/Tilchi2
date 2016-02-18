//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "FrmMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RzPanel"
#pragma link "TntComCtrls"
#pragma link "TntDialogs"
#pragma link "TntStdCtrls"
#pragma link "VirtualTrees"
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
	: TForm(Owner)
{
	Screen->Cursors[1] = LoadCursor(HInstance, "LINK_CURSOR");
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TreeLangsGetText(TBaseVirtualTree *Sender,
	  PVirtualNode Node, TColumnIndex Column, TVSTTextType TextType,
	  WideString &CellText)
{
	CellText = Vec[Sender == TreeLangs ? 0 : 1]->Strings[Node->Index];
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TreeLangsGetImageIndex(TBaseVirtualTree *Sender,
	  PVirtualNode Node, TVTImageKind Kind, TColumnIndex Column, bool &Ghosted,
	  int &ImageIndex)
{
	ImageIndex = Node->Index;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TreeLangsGetCursor(TBaseVirtualTree *Sender,
	  TCursor &Cursor)
{
	Cursor = (TCursor) 1;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TreeLangsClick(TObject *Sender)
{
	if (Sender == TreeLangs) {
		TreeLangs->Visible = false;
		int index = TreeLangs->FocusedNode->Index;

		if (index != image_cur_lang->Tag) {
			/*if (image_cur_lang == ImageLeftLang)
				if (!active_langs[index][ImageRightLang->Tag]){
					ImageList->GetIcon(index = FindFirstActiveLang(index),
						ImageRightLang->Picture->Icon);
					ImageRightLang->Tag = index;
				}*/

			image_cur_lang->Tag = index;
			SetPicture(image_cur_lang);
			SetLangDirection();
		}
	}
	else {
		int te = fileList->Count*2*sizeof(unsigned);
		indicesStream->Seek((TreeWords->FocusedNode->Index + 4)*te, 0);
		indicesStream->Read(&vec_ixes[0][0], te);
		ComboDics->Clear();

		for (int i = fileList->Count - 1; i > -1; --i)
			if (vec_ixes[i][1] > 0)
				ComboDics->Items->Add(dictionaryList->Strings[i]);

		ComboDics->ItemIndex = 0;
    	translate();
	}
}
//---------------------------------------------------------------------------
void TFormMain::translate()
{
	int ix;
	dictionaryList->Find(ComboDics->Text, ix);

	FILE *fn = fopen((ExtractFilePath(Application->ExeName) + "Dics\\" +
		fileList->Strings[ix] + ".tdu").c_str(), "rb");
	if (!fn)
		return;

	std::vector<char> buf(MAX_CHARS < header_vec[ix][1] ?
		header_vec[ix][1] : MAX_CHARS < vec_ixes[ix][1] ?
		vec_ixes[ix][1] : MAX_CHARS);

	fseek(fn, header_vec[ix][0], 0);
	fread(&buf[0], header_vec[ix][1], 1, fn);
	loadStream->Clear();
	loadStream->WriteBuffer(&buf[0], header_vec[ix][1]);
	loadStream->Seek(0, 0);

	loadStream2->Clear();
	if (!ECLDecompressStream(loadStream, loadStream2,
		"бул ар бир стать€ учун !є;14 сыр сощ", zlibFastest))
		MessageDlg(Vec[0]->Strings[21], mtError, TMsgDlgButtons() << mbOK, 0);
	else{
		fseek(fn, vec_ixes[ix][0], 0);
		fread(&buf[0], vec_ixes[ix][1], 1, fn);
		loadStream->Clear();
		loadStream->WriteBuffer(&buf[0], vec_ixes[ix][1]);
		loadStream->Seek(0, 0);

		loadStream2->Seek(0, 2);
		if (!ECLDecompressStream(loadStream, loadStream2,
			"бул ар бир стать€ учун !є;14 сыр сощ", zlibFastest))
			MessageDlg(Vec[0]->Strings[21], mtError, TMsgDlgButtons() << mbOK, 0);
		else{
			loadStream2->Seek(0, 2);
			loadStream2->WriteBuffer("}", 1);
			loadStream2->Seek(0, 0);
			RichEdit->Clear();
			RichEdit->Lines->LoadFromStream(loadStream2);
		}
	}

	fclose(fn);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetLinkCursor(TControl *Control)
{
	Control->Cursor = (TCursor) 1;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormDestroy(TObject *Sender)
{
	propData->WriteInteger("FormMain", "Left Language", ImageLeftLang->Tag);
	propData->WriteInteger("FormMain", "Right Language", ImageRightLang->Tag);

	if (WindowState == wsMaximized)
		propData->WriteBoolean("FormMain", "WindowStateMaximized", true);
	else {
		propData->WriteBoolean("FormMain", "WindowStateMaximized", false);
		propData->WriteInteger("FormMain", "Left", Left);
		propData->WriteInteger("FormMain", "Top", Top);
		propData->WriteInteger("FormMain", "Width", Width);
		propData->WriteInteger("FormMain", "Height", Height);
	}

	delete Vec[0], Vec[1], indicesStream, loadStream, loadStream2, fileList, propData, dictionaryList;	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormShow(TObject *Sender)
{
	LoadSettings();
	LoadPics();
	LoadLayoutStrings();

	SetLinkCursor(ImageAbout);
	SetLinkCursor(ImageLeftLang);
	SetLinkCursor(ImageRightLang);
	SetLinkCursor(ImageDicsLeftLang);
	SetLinkCursor(ImageDicsRightLang);
	SetLinkCursor(PanelLangMenu);
	SetLinkCursor(PanelDicsMenu);
	SetLinkCursor(PanelServicesMenu);
	SetLinkCursor(PanelHistoryMenu);

	SetLangDirection();
	LoadWords();

	TreeLangs->RootNodeCount = LANG_AMOUNT;	
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::LoadSettings()
{
	Vec.resize(2);
	Vec[0] = new TTntStringList();
	Vec[1] = new TTntStringList();
	
	loadStream = new TMemoryStream();     
	loadStream2 = new TMemoryStream();
	indicesStream = new TMemoryStream();
	fileList = new TStringList();
	dictionaryList = new TTntStringList();
	propData = new TDataFile("Tilchi.dat");
	image_cur_lang = ImageLeftLang;

	ImageLeftLang->Tag = propData->ReadInteger("FormMain", "Left Language", ImageLeftLang->Tag);
	ImageRightLang->Tag = propData->ReadInteger("FormMain", "Right Language", ImageRightLang->Tag);

	if (propData->ReadBoolean("MainFormClass", "WindowStateMaximized", 0))
		WindowState = wsMaximized;
	else{
		Left = propData->ReadInteger("FormMain", "Left", Screen->WorkAreaWidth - Width);
		Top = propData->ReadInteger("FormMain", "Top", Screen->WorkAreaHeight - Height);
		Height = propData->ReadInteger("FormMain", "Height", 0);
		Width = propData->ReadInteger("FormMain", "Width", 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::LoadLayoutStrings()
{
	AnsiString file_name = propData->ReadString("FormMain", "Layout Language", "Russian.lng");

	loadStream->Clear();
	if (!FileExists("Langs\\" + file_name))
		propData->ReadStream("Layout Language", "Russian", loadStream);
	else{
		TDataFile *langData = new TDataFile("Langs\\" + file_name);
		langData->ReadStream("LayoutStrings", "Russian", loadStream);
		delete langData;
	}

	Vec[0]->LoadFromStream(loadStream);
	PanelDicsMenu->Caption = "            " + Vec[0]->Strings[13];
	PanelServicesMenu->Caption = "            " + Vec[0]->Strings[17];
	PanelHistoryMenu->Caption = "            " + Vec[0]->Strings[22];

	LabelCaption->Caption = Vec[0]->Strings[15];
	LabelCaptionDescription->Caption = Vec[0]->Strings[16];
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::LoadWords()
{
	AnsiString source = (ImageLeftLang->Tag / 10 == 0 ? "0" : "") + AnsiString(ImageLeftLang->Tag),
		destination = (ImageRightLang->Tag / 10 == 0 ? "0" : "") + AnsiString(ImageRightLang->Tag);

	loadStream->Clear();
	propData->ReadStream("DicNames", source + destination, loadStream);

	if (loadStream->Size > 0) {
		dictionaryList->LoadFromStream(loadStream);
		propData->ReadStrings("DicFileNames", source + destination, fileList);

		vec_ixes.resize(fileList->Count, std::vector<unsigned>(2));
		header_vec.resize(fileList->Count, std::vector<unsigned>(2));

		TECLFileStream *f_read = new TECLFileStream("Dics\\" + source + destination + ".tst", fmOpenRead,
			"дшыео ывдлоп лдоорыва о лорыв жшшк*(*:6 ц34 лвары", zlibFastest, 0);

		if (!ECLDecompressStream(f_read, loadStream,
			") это было, (что ни говори, а это факт).", zlibFastest))
			MessageDlg(Vec[0]->Strings[21], mtError, TMsgDlgButtons() << mbOK, 0);
		else{
			delete f_read;
			Vec[1]->LoadFromStream(loadStream);

			f_read = new TECLFileStream("Dics\\" + source + destination + ".tnd", fmOpenRead,
				"дшыео ывдлоп лдоорыва о лорыв жшшк*(*:6 ц34 лвары", zlibFastest, 0);

			if (!ECLDecompressStream(f_read, indicesStream,
				") это было, (что ни говори, а это факт).", zlibFastest))
				MessageDlg(Vec[0]->Strings[21], mtError, TMsgDlgButtons() << mbOK, 0);
			else{
				TreeWords->RootNodeCount = Vec[1]->Count;
				int size = fileList->Count*2*sizeof(unsigned);
				indicesStream->Seek(size, 0);
				indicesStream->Read(&header_vec[0][0], size);
			}
		}

		delete f_read;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::LoadPics()
{
	loadStream->Clear();
	propData->ReadStream("Images", "About", loadStream);
	ImageAbout->Picture->Icon->LoadFromStream(loadStream);
                        
	loadStream->Clear();
	propData->ReadStream("Images", "DicsMenu", loadStream);
	ImageDics->Picture->Icon->LoadFromStream(loadStream);

	for (int i = 0; i < 9; ++i) {
		loadStream->Clear();
		propData->ReadStream("Images", "Language" + AnsiString(i), loadStream);
		ImageLeftLang->Picture->Icon->LoadFromStream(loadStream);
		ImageList->AddIcon(ImageLeftLang->Picture->Icon);
	}

	SetPicture(ImageLeftLang);
	SetPicture(ImageRightLang);
}                                                                           
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetPicture(TImage* Image)
{
	ImageList->GetIcon(Image->Tag, Image->Picture->Icon);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::SetLangDirection()
{
	TImage *imageLeft = image_cur_lang == ImageLeftLang ||
		image_cur_lang == ImageRightLang ? ImageLeftLang : ImageDicsLeftLang,
		*imageRight = imageLeft == ImageLeftLang ? ImageRightLang : ImageDicsRightLang;

	TRzPanel *Panel = image_cur_lang == ImageLeftLang ||
		image_cur_lang == ImageRightLang ? PanelLangMenu : PanelDicsTop;

	Panel->Caption = Vec[0]->Strings[imageLeft->Tag] + " - " +
		Vec[0]->Strings[imageRight->Tag];
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ImageLeftLangClick(TObject *Sender)
{
	if (TreeLangs->Visible && image_cur_lang == Sender)
		TreeLangs->Visible = false;
	else{
		image_cur_lang = (TImage*)Sender;

		TreeLangs->BeginUpdate();
		try {
			TVirtualNodeStates Filter;
			TreeLangs->IterateSubtree(NULL, HideLangs, NULL, Filter, false, false);
		}
		__finally{
			TreeLangs->EndUpdate();
		}

		int temp = TreeLangs->VisibleCount*28;
		TreeLangs->Height = temp > 280 ? 280 : temp;

		if (TreeLangs->VisibleCount > 0) {
			if (image_cur_lang == ImageLeftLang || image_cur_lang == ImageRightLang){
				TreeLangs->Top = 90;
				TreeLangs->Left = 2;
				TreeLangs->Width = PanelLeft->Width - 6;
			}
			else{
				TreeLangs->Top = 175;
				TreeLangs->Left = PanelLeft->Width + 7;
				TreeLangs->Width = PanelDicsTop->Width - 10;
			}

			TreeLangs->BiDiMode = image_cur_lang == ImageLeftLang
				|| image_cur_lang == ImageDicsLeftLang ?
				bdLeftToRight : bdRightToLeft;
			TreeLangs->Visible = true;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::HideLangs(TBaseVirtualTree* Sender,
	PVirtualNode Node, void* Data, bool &Abort)
{
	if (image_cur_lang == ImageLeftLang || image_cur_lang == ImageRightLang)
		Sender->IsVisible[Node] = FindFirstActiveLang(Node->Index);
	else
		Sender->IsVisible[Node] = true;
}
//---------------------------------------------------------------------------
int __fastcall TFormMain::FindFirstActiveLang(int index)
{
	bool ok;
	TStringList *tt = new TStringList();
	AnsiString ix = (index / 10 == 0 ? "0" : "") + AnsiString(index);
	propData->GetValueNames("DicsFileNames", tt);

	if (image_cur_lang == ImageLeftLang)
		ok = tt->Count > 0;
	else{
    	// NOT CORRECT - SHOLD BE CHANGED
		/*for (int i = tt->Count - 1; i > -1; --i) {
			if (tt->Strings[i] == ix) {
				ok = true;
				break;
			}
		}*/
	}
	delete tt;

	return ok;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ImageAboutMouseEnter(TObject *Sender)
{                       
	loadStream->Clear();
	propData->ReadStream("Images", "AboutHot", loadStream);
	ImageAbout->Picture->Icon->LoadFromStream(loadStream);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ImageAboutMouseLeave(TObject *Sender)
{                       
	loadStream->Clear();
	propData->ReadStream("Images", "About", loadStream);
	ImageAbout->Picture->Icon->LoadFromStream(loadStream);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PanelDicsMenuClick(TObject *Sender)
{
	LabelCaption->Caption = Vec[0]->Strings[13];
	LabelCaptionDescription->Caption = Vec[0]->Strings[14];
	loadStream->Clear();
	propData->ReadStream("Images", "DicsCaption", loadStream);
	ImageCaption->Picture->Icon->LoadFromStream(loadStream);

	ButtonAddDic->Caption = Vec[0]->Strings[19];

	ImageDicsLeftLang->Tag = ImageLeftLang->Tag;
	ImageDicsRightLang->Tag = ImageRightLang->Tag;

	SetPicture(ImageDicsLeftLang);
	SetPicture(ImageDicsRightLang);
	PanelDics->BringToFront();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ButtonAddDicClick(TObject *Sender)
{
	OpenDialog->Title = Vec[0]->Strings[20];

	if (OpenDialog->Execute()){
		// FOR FILESTREAM PASSWORD
		TECLFileStream *f_read = new TECLFileStream(OpenDialog->FileName, fmOpenRead,
			"погов. то было, (да) это было, дес€ть верблюдов Ёшкары пало (что ни говори, а это факт).",
			zlibFastest, 0);

		loadStream->Clear();	// FOR DECOMPRESSION PASSWORD
		if (!ECLDecompressStream(f_read, loadStream,
			") это было, (что ни говори, а это факт).", zlibFastest))
			MessageDlg(Vec[0]->Strings[21], mtError, TMsgDlgButtons() << mbOK, 0);
		else
            create_dictionary();

		delete f_read;
	}
}
//---------------------------------------------------------------------------
void TFormMain::create_dictionary()
{
	TStringList *db_file = new TStringList();
	db_file->LoadFromStream(loadStream);

	AnsiString sys_file_name = ExtractFilePath(Application->ExeName) +
		"Dics\\" + db_file->Strings[0];

	Shortint mode = FileExists(sys_file_name + ".tst") ? fmOpenReadWrite : fmCreate;

	TECLFileStream *f_read = new TECLFileStream(sys_file_name + ".tst", mode,
		"дшыео ывдлоп лдоорыва о лорыв жшшк*(*:6 ц34 лвары", zlibFastest, 0);

	loadStream2->Clear();
	if (!ECLDecompressStream(f_read, loadStream2,
		") это было, (что ни говори, а это факт).", zlibFastest))
		MessageDlg(Vec[0]->Strings[21], mtError, TMsgDlgButtons() << mbOK, 0);
	else
		index_dictionary(db_file, f_read, sys_file_name, mode);

	delete db_file, f_read;
}
//---------------------------------------------------------------------------
void TFormMain::index_dictionary(TStringList *db_file, TECLFileStream *f_words_stream,
	const AnsiString& sys_file_name, const Shortint& mode)
{
	TECLFileStream *f_read = new TECLFileStream(sys_file_name + ".tnd", mode,
		"дшыео ывдлоп лдоорыва о лорыв жшшк*(*:6 ц34 лвары", zlibFastest, 0);

	loadStream->Clear();
	if (!ECLDecompressStream(f_read, loadStream,
		") это было, (что ни говори, а это факт).", zlibFastest))
		MessageDlg(Vec[0]->Strings[21], mtError, TMsgDlgButtons() << mbOK, 0);
	else
		compile_dictionary(db_file, f_words_stream, f_read);

	delete f_read;
}
//---------------------------------------------------------------------------
void TFormMain::compile_dictionary(TStringList *db_file, TECLFileStream *f_words_stream,
	TECLFileStream *f_indices_stream)
{
	TTntStringList *wordsList = new TTntStringList(),
    	*dicsNamesList = new TTntStringList();
	wordsList->LoadFromStream(loadStream2);

	TStringList *dicsList = new TStringList();
                                                                         
	loadStream2->Clear();
	propData->ReadStrings("DicFileNames", db_file->Strings[0], dicsList);
	propData->ReadStream("DicNames", db_file->Strings[0], loadStream2);
	dicsNamesList->LoadFromStream(loadStream2);

	std::string str = AnsiString(OpenDialog->FileName.c_bstr()).c_str();
	
	int ix = str.rfind('\\', str.length()),	ixu = str.rfind('.', str.length()),
		count = db_file->Count - 1, cur_ix = -1, dicsCount = dicsList->Count,
		fin_min, ixx;

	str = str.substr(ix + 1, ixu < ix ? str.length() : ixu - ix - 1);
	dicsList->Add(str.c_str());

	FILE *fn = fopen((ExtractFilePath(Application->ExeName) + "Dics\\" +
		AnsiString(str.c_str()) + ".tdu").c_str(), "wb");
	if(!fn)
		return;

	unsigned total = 0, vec_size = sizeof(unsigned)*2*dicsCount,
		new_vec_size = sizeof(unsigned)*2*(dicsCount + 1), size;

	std::vector<char> temp_vec(MAX_CHARS);
	std::vector< std::vector<unsigned> > prev_vec(dicsCount,
		std::vector<unsigned>(2)), ix_vec;

	TMemoryStream *indStream = new TMemoryStream();
	TECLMemoryStream *tempWriteStream = new TECLMemoryStream("", zlibFastest, 0);

	AnsiString t(db_file->Strings[1] + db_file->Strings[2] + "}");
	loadStream2->Clear();
	loadStream2->Write(t.c_str(), t.Length());
	loadStream2->Seek(0, 0);

	RichEdit->Clear();
	RichEdit->Lines->LoadFromStream(loadStream2);
	dicsNamesList->Add(Trim(RichEdit->Lines->Text));

	WideString word;
	loadStream->Seek(0, 0);

	for (int i = 0; i < 4; ++i) {
		loadStream2->Clear();
		loadStream2->Write(db_file->Strings[i].c_str(), db_file->Strings[i].Length());
		loadStream2->Seek(0, 0);

		tempWriteStream->Clear();
		ECLCompressStream(loadStream2, tempWriteStream,
			"бул ар бир стать€ учун !є;14 сыр сощ", zlibFastest);

		if ((size = tempWriteStream->Size) > temp_vec.size())
			temp_vec.resize(size);

		tempWriteStream->Seek(0, 0);
		tempWriteStream->Read(&temp_vec[0], size);
		fwrite(&temp_vec[0], size, 1, fn);
		if (vec_size > 0)
			loadStream->Read(&prev_vec[0][0], vec_size);

		ix_vec = prev_vec;
		ix_vec.push_back(std::vector<unsigned>(2, 0));
		ix_vec[dicsCount][0] = total;
		total += (ix_vec[dicsCount][1] = size);
		indStream->Write(&ix_vec[0][0], new_vec_size);
	}
                                         
	AnsiString comma = ",", colon = ":", exclam = "!",
		bracket = "(", I_letter = "I", semicolon = ";";

	for (int i = 4; i < count; ++i) {
		loadStream2->Clear();
		loadStream2->WriteBuffer(db_file->Strings[i].c_str(), db_file->Strings[i].Length());
		loadStream2->Seek(0, 0);

		tempWriteStream->Clear();
		ECLCompressStream(loadStream2, tempWriteStream,
			"бул ар бир стать€ учун !є;14 сыр сощ", zlibFastest);

		ix = db_file->Strings[i].Pos("'") - 1;
		ixu = db_file->Strings[i].Pos("\\u") - 1;
		ix = ix < ixu ? ix : ixu > -1 ? ixu : ix;

		str = db_file->Strings[i].c_str();
		t = db_file->Strings[i].SubString(ix,
			str.find("\\par", ix) - ix + 1).Trim();

		fin_min = t.Length();
		ixu = 500;

		ixx = t.Pos(exclam);
		if (ixx > 0 && ixx < ixu) ixu = ixx;

		ixx = t.Pos(comma);
		if (ixx > 0 && ixx < ixu) ixu = ixx;

		ixx = t.Pos(bracket);
		if (ixx > 0 && ixx < ixu) ixu = ixx;

		ixx = t.Pos(colon);
		if (ixx > 0 && ixx < ixu) ixu = ixx;

		ixx = t.Pos(semicolon);
		if (ixx > 0 && ixx < ixu) ixu = ixx;

		ixx = t.Pos(I_letter);
		if (ixx > 0 && ixx < ixu) ixu = ixx;

		if (ixu <= fin_min) --ixu;
		else ixu = fin_min;

		t = db_file->Strings[1] +
			db_file->Strings[i].SubString(ix, ixu) + "}";

		loadStream2->Clear();
		loadStream2->Write(t.c_str(), t.Length());
		loadStream2->Seek(0, 0);

		RichEdit->Clear();
		RichEdit->Lines->LoadFromStream(loadStream2);

		word = Trim(RichEdit->Lines->Text);
		if ((size = tempWriteStream->Size) > temp_vec.size())
			temp_vec.resize(size);

		tempWriteStream->Seek(0, 0);
		tempWriteStream->Read(&temp_vec[0], size);
		fwrite(&temp_vec[0], size, 1, fn);

		while(++cur_ix < wordsList->Count && wordsList->Strings[cur_ix] < word){
			if (vec_size > 0)
				loadStream->Read(&prev_vec[0][0], vec_size);
			ix_vec = prev_vec;
			ix_vec.push_back(std::vector<unsigned>(2, 0));
			indStream->Write(&ix_vec[0][0], new_vec_size);
		}

		if (vec_size > 0)
			loadStream->Read(&prev_vec[0][0], vec_size);

		ix_vec = prev_vec;
		ix_vec.push_back(std::vector<unsigned>(2, 0));

		if (cur_ix < wordsList->Count && wordsList->Strings[cur_ix] == word) {
			ix_vec[dicsCount][0] = total;
			total += (ix_vec[dicsCount][1] = size);
			indStream->Write(&ix_vec[0][0], new_vec_size);
		}
		else{
			if (vec_size > 0)
				indStream->Write(&ix_vec[0][0], new_vec_size);

			wordsList->Insert(cur_ix, word);
			std::vector< std::vector<unsigned> > new_vec(dicsCount + 1,
				std::vector<unsigned>(2, 0));

			new_vec[dicsCount][0] = total;
			total += (new_vec[dicsCount][1] = size);
			indStream->Write(&new_vec[0][0], new_vec_size);
		}
	}

	propData->WriteStrings("DicFileNames", db_file->Strings[0], dicsList);

	loadStream2->Clear();
	dicsNamesList->SaveToStream(loadStream2);
	propData->WriteStream("DicNames", db_file->Strings[0], loadStream2);

	indStream->Seek(0, 0);
	f_indices_stream->Seek(0, 0);
	ECLCompressStream(indStream, f_indices_stream,
		") это было, (что ни говори, а это факт).", zlibFastest);

	loadStream->Clear();
	wordsList->SaveToStream(loadStream);
	loadStream->Seek(0, 0);
	f_words_stream->Seek(0, 0);
	ECLCompressStream(loadStream, f_words_stream,
		") это было, (что ни говори, а это факт).", zlibFastest);

	fclose(fn);
	delete dicsNamesList, dicsList, tempWriteStream, indStream;
}                                                                       
//---------------------------------------------------------------------------
void __fastcall TFormMain::PanelLangMenuClick(TObject *Sender)
{
	PanelTranslation->BringToFront();
	LabelCaption->Caption = Vec[0]->Strings[15];
	LabelCaptionDescription->Caption = Vec[0]->Strings[16];
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PanelServicesMenuClick(TObject *Sender)
{
	PanelServices->BringToFront();	          
	LabelCaption->Caption = Vec[0]->Strings[17];
	LabelCaptionDescription->Caption = Vec[0]->Strings[18];
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PanelHistoryMenuClick(TObject *Sender)
{
	PanelHistory->BringToFront();
	LabelCaption->Caption = Vec[0]->Strings[22];
	LabelCaptionDescription->Caption = Vec[0]->Strings[23];
}
//---------------------------------------------------------------------------
