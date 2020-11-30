//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <jpeg.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TEdit *EditDBPath;
        TButton *ButtonDBPath;
        TOpenDialog *OpenDialog1;
        TButton *ButtonDBOpen;
        TGroupBox *GroupBoxEdit;
        TRadioButton *RadioButtonSearch;
        TRadioButton *RadioButtonEdit;
        TRadioButton *RadioButtonQR;
        TEdit *EditName;
        TLabel *Label2;
        TLabel *Label4;
        TEdit *EditINN;
        TEdit *EditPsw;
        TLabel *LabelPsw;
        TButton *ButtonPsw;
        TButton *ButtonSave;
        TGroupBox *GroupBoxSearch;
        TRadioButton *RadioButtonMatch;
        TRadioButton *RadioButtonStart;
        TRadioButton *RadioButtonContain;
        TListBox *ListBoxSearch;
        TButton *ButtonQR;
        TPanel *Panel1;
        TEdit *EditSearchFIO;
        TEdit *EditSearchINN;
        TButton *ButtonSearch;
        TRadioButton *RadioButtonFIO;
        TRadioButton *RadioButtonINN;
        TGroupBox *GroupBoxQR;
        TRadioButton *RadioButtonAbout;
        TShape *Shape1;
        TGroupBox *GroupBoxAbout;
        TRichEdit *RichEdit1;
        TLabel *LabelId;
        TLabel *LabelSec;
        TLabel *LabelSig;
        TLabel *LabelInfo;
        TLabel *Label5;
        TEdit *EditVoter;
        TButton *ButtonLoad;
        TButton *ButtonNext;
        TButton *ButtonDelete;
        TLabel *LabelOnion;
        TButton *ButtonSaveQR;
        TImage *Image1;
        TButton *ButtonSrvLoad;
        TOpenDialog *OpenDialog2;
        TImage *Image2;
        TGroupBox *GroupBoxCnd;
        TRadioButton *RadioButtonCnd;
        TLabel *Label3;
        TButton *Button1;
        TButton *Button2;
        TButton *Button3;
        TLabel *LabelCndPath;
        TMemo *MemoCnt;
        TMemo *MemoCnd;
        TLabel *LabelCndInfo;
        TOpenDialog *OpenDialogCnd;
        void __fastcall ButtonDBPathClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall RadioButtonEditClick(TObject *Sender);
        void __fastcall RadioButtonQRClick(TObject *Sender);
        void __fastcall RadioButtonSearchClick(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall RadioButtonAboutClick(TObject *Sender);
        void __fastcall SetForm(void* p);
        void __fastcall SetList(void* p);
        void __fastcall ButtonPswClick(TObject *Sender);
        void __fastcall ButtonSearchClick(TObject *Sender);
        void __fastcall ButtonDBOpenClick(TObject *Sender);
        void __fastcall ListBoxSearchDblClick(TObject *Sender);
        void __fastcall ButtonLoadClick(TObject *Sender);
        void __fastcall ButtonNextClick(TObject *Sender);
        void __fastcall ButtonSaveClick(TObject *Sender);
        void __fastcall ButtonDeleteClick(TObject *Sender);
        void __fastcall ButtonQRClick(TObject *Sender);
        void __fastcall ButtonSaveQRClick(TObject *Sender);
        void __fastcall ButtonSrvLoadClick(TObject *Sender);
        void __fastcall RadioButtonCndClick(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
