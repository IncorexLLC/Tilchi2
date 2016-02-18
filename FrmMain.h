//---------------------------------------------------------------------------

#ifndef FrmMainH
#define FrmMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RzPanel.hpp"
#include "TntComCtrls.hpp"
#include "TntDialogs.hpp"
#include "TntStdCtrls.hpp"
#include "VirtualTrees.hpp"
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include "DataFile.hpp"
#include <vector>
#include "EasyCompression.hpp"
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
	TRzSpacer *SpacerTopMain;
	TRzSpacer *SpacerBottomMain;
	TRzSpacer *SpacerLeftMain;
	TRzSpacer *SpacerRightMain;
	TPanel *PanelMain;
	TSplitter *SplitterLeft;
	TRzSpacer *SpacerTop2;
	TRzPanel *PanelTop;
	TLabel *LabelIncorex;
	TLabel *LabelTilchi;
	TImage *ImageAbout;
	TPanel *PanelRight;
	TRzSpacer *SpacerRight1;
	TPanel *PanelDics;
	TRzPanel *PanelDicsTop;
	TImage *ImageDicsLeftLang;
	TImage *ImageDicsRightLang;
	TRzPanel *ButtonAddDic;
	TRzPanel *PanelTranslation;
	TTntRichEdit *RichEdit;
	TRzPanel *PanelCaption;
	TImage *ImageCaption;
	TTntLabel *LabelCaptionDescription;
	TTntLabel *LabelCaption;
	TPanel *PanelServices;
	TPanel *PanelHistory;
	TPanel *PanelLeft;
	TRzSpacer *SpacerLeft2;
	TRzSpacer *SpacerLeftTop;
	TRzSpacer *SpacerLeft3;
	TRzSpacer *SpacerLeft4;
	TRzPanel *PanelDicsMenu;
	TImage *ImageDics;
	TRzPanel *PanelServicesMenu;
	TImage *ImageServices;
	TRzPanel *PanelWords;
	TSplitter *SplitterWords;
	TTntMemo *MemoSearchWord;
	TRzPanel *PanelLangMenu;
	TImage *ImageLeftLang;
	TImage *ImageRightLang;
	TRzPanel *PanelHistoryMenu;
	TImage *ImageHistory;
	TImageList *ImageList;
	TVirtualStringTree *TreeWords;
	TVirtualStringTree *TreeLangs;
	TRzPanel *PanelTranslationTop;
	TImage *Image1;
	TTntComboBox *ComboDics;
	TTntOpenDialog *OpenDialog;
	void __fastcall TreeLangsGetText(TBaseVirtualTree *Sender,
          PVirtualNode Node, TColumnIndex Column, TVSTTextType TextType,
          WideString &CellText);
	void __fastcall TreeLangsGetImageIndex(TBaseVirtualTree *Sender,
          PVirtualNode Node, TVTImageKind Kind, TColumnIndex Column,
          bool &Ghosted, int &ImageIndex);
	void __fastcall TreeLangsClick(TObject *Sender);
	void __fastcall TreeLangsGetCursor(TBaseVirtualTree *Sender,
		  TCursor &Cursor);
	void __fastcall ImageLeftLangClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ImageAboutMouseEnter(TObject *Sender);
	void __fastcall ImageAboutMouseLeave(TObject *Sender);
	void __fastcall PanelDicsMenuClick(TObject *Sender);
	void __fastcall ButtonAddDicClick(TObject *Sender);
	void __fastcall PanelLangMenuClick(TObject *Sender);
	void __fastcall PanelServicesMenuClick(TObject *Sender);
	void __fastcall PanelHistoryMenuClick(TObject *Sender);
private:	// User declarations
	TImage *image_cur_lang;
	TDataFile *propData;
	TStringList *fileList;
	TTntStringList *dictionaryList;
	TMemoryStream *loadStream, *loadStream2, *indicesStream;
	std::vector<TTntStringList*> Vec;	// Vec[0] -> langList
										// Vec[1] -> wordsList
	std::vector< std::vector<unsigned> > vec_ixes, header_vec;										
	int cur_lang_ix;

	enum {ENGLISH, SPAIN, ITALIAN, KAZAKH, KOREAN, KYRGYZ, GERMAN, RUSSIAN,
		TADJYK, TURKISH, TURKMEN, UZBEK, FRENCH, LANG_AMOUNT, MAX_CHARS = 5000};

	int __fastcall FindFirstActiveLang(int);
	void __fastcall LoadSettings();
	void __fastcall LoadLayoutStrings();
	void __fastcall LoadWords();
	void __fastcall LoadPics();                      
	void __fastcall SetLinkCursor(TControl *Control);
	void __fastcall SetPicture(TImage*);
	void __fastcall SetLangDirection();               
	void __fastcall HideLangs(TBaseVirtualTree* Sender,
		PVirtualNode Node, void * Data, bool &Abort);
	void translate();
	void create_dictionary();
	void index_dictionary(TStringList*, TECLFileStream*,
		const AnsiString&, const Shortint&);
	void compile_dictionary(TStringList*, TECLFileStream*,
		TECLFileStream*);
public:		// User declarations
	__fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
