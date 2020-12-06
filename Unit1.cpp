//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "cmd.h"  //C command processor 

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

 #define QR_TEXTSIZE 14  //scale factor for QRcode
  #define IMG_W 376  //width of ticket image in pixels
  #define IMG_H 520  //height of ticket image in pixels


TStringList* List = new TStringList();

AnsiString dbpath=""; //path to database file
char Is_DB=0; //flag of database opened

int Voter=0; //current Voter desplayed in main input tab
int isqrcode=0;  //QRcode ID (Voter)
unsigned char dat_qrcode[32];

AnsiString OnionPath=""; //path to onion hostnsme file
AnsiString OnionStr="";  //server's onion address in b32
unsigned char onion[10]; //binary server's onion address
char isonion=0; //flag we have server's onion address



 //output data of user loaded from database to main input panel
void __fastcall TForm1::SetForm(void* p)
{
 reg_data reg;
 if(p)
 {
  memcpy(&reg, p, sizeof(reg_data));
  if(reg.Idn) LabelId->Caption="Voter: "+IntToStr(reg.Idn);
  else LabelId->Caption="Voter: NEXT";
  EditName->Text=AnsiString(reg.Fio);
  EditINN->Text=AnsiString(reg.Inn);
  EditPsw->Text=AnsiString(reg.Psw);
  LabelSec->Caption="Sec: "+AnsiString(reg.Sec);
  LabelSig->Caption="Sig: "+AnsiString(reg.Sig);
 }
}

//output search results to list
void __fastcall TForm1::SetList(void* p)
{

 AnsiString S="";
 reg_data reg;
 if(p)
 {
  memcpy(&reg, p, sizeof(reg_data));
  S=IntToStr(reg.n)+": "+AnsiString(reg.Fio)+" ( #"+IntToStr(reg.Idn)+" )";
  S=S+" INN:"+AnsiString(reg.Inn)+" (";
  if(reg.Psw[0]) S=S+"PSW ";
  if(reg.Sec[0]) S=S+"SEC ";
  if(reg.Sig[0]) S=S+"SIG ";
  S=S+")";
  ListBoxSearch->Items->Add(S);
 }
}

extern "C"
{

 //callback for set data in form
 void setform(void* p)
 {Form1->SetForm(p);}

 //callback for set data in list
 void setlist(void* p)
 {Form1->SetList(p);}


} //extern "C"


//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

//set path to database file
void __fastcall TForm1::ButtonDBPathClick(TObject *Sender)
{
 //get app directory path
 AnsiString exeFile=Application->ExeName;
 AnsiString exePath=ExtractFilePath(exeFile);

 //set this path as start for open dialog
 if(EditDBPath->Text != "")
 {
  OpenDialog1->FileName=EditDBPath->Text;
 }

 //sslect database file in dialog
 if(OpenDialog1->Execute())
 {
  exePath = exePath + "dbpath.ini";
  EditDBPath->Text=OpenDialog1->FileName;
  List->Clear();
  List->Add(EditDBPath->Text);

  List->SaveToFile(exePath);   //save databsa file name to ini file
 }


}
//---------------------------------------------------------------------------


//initialize app
void __fastcall TForm1::FormCreate(TObject *Sender)
{

  short i;
 //=================get path to work directory================
  //get app directory path
  AnsiString exeFile=Application->ExeName;
  AnsiString exePath=ExtractFilePath(exeFile);
  AnsiString S;

  //===============Load list of candidates==================

  List->Clear();
  try{
      List->LoadFromFile(exePath+"cdpath.ini");  //load hostname
     }
  catch(...)
  {
   LabelCndInfo->Caption = "Путь к файлу со списком кандидатов не выбран!";
  }

  if(List->Count)  S=List->Strings[0]; else S="";
  LabelCndPath->Caption=S;

  MemoCnd->Lines->Clear();
  if(S!="") //load list of candidates
  try{
      MemoCnd->Lines->LoadFromFile(S);
      LabelCndInfo->Caption = "Загружено "+ IntToStr(MemoCnd->Lines->Count) + " кандидатов";
     }
  catch(...)
  {
   LabelCndInfo->Caption = "Файл со списком кандидатов не найден!";
  }


//=================Load server's onoion address=======================

  //load path to Tor hostname file
  List->Clear();
  try{
      List->LoadFromFile(exePath+"onpath.ini");  //load hostname
     }
  catch(...)
  {
   LabelOnion->Caption = "Путь к файлу tor/hidden_sevice/hostname не выбран!";
  }
  if(List->Count)  S=List->Strings[0]; else S="";

//try load onion address
  List->Clear();
  if(S!="") //load onion address
  try{
      List->LoadFromFile(S);
     }
  catch(...)
  {
   LabelOnion->Caption = "Укажите путь к файлу с адресом сервера: tor/hidden_sevice/hostname!";
  }


  //if hostname loaded check onion address
  if(List->Count)
  {
   char str[32];
   unsigned char adr[16]={0,};

   LabelOnion->Caption=List->Strings[0]; //display loaded  address
   strncpy(str, List->Strings[0].c_str(), sizeof(str));
   i=base32_decode(adr, str); //decode base32 Tor format to binary (compress)
   if(i) //invalid base32
   {
    ShowMessage("Неверный onion-адресс!");
   }
   else  //valid base32
   {
    memcpy(onion, adr, 10); //store compressed onion adress for qr-codes
    isonion=1; //set flag we have server's onion address
    LabelOnion->Caption=LabelOnion->Caption+" (seems valid)";
    OnionStr=List->Strings[0]; //store onion addres
    OnionPath = S; //store path to hostname file
   }

  }
  //==============Load path to database=========================

  //set name of ini file with last path to databas
  List->Clear();
  try{
      List->LoadFromFile(exePath + "dbpath.ini"); //load databsae path
     }
  catch(...)
  {


  }
  
  if(List->Count)
  {
   EditDBPath->Text=List->Strings[0]; //set database path will be used on opening
  }


//================Load content of About page==================
//load logo for About panel
  try{
      RichEdit1->Lines->LoadFromFile(exePath + "logo.rtf");
     }
  catch(...)
  {


  }

 //================Set main form component's sizes============
  //set initial geometry of panels on main form

   GroupBoxCnd->Top=100;
   GroupBoxCnd->Height=Form1->Height-140;
   GroupBoxCnd->Left=20;
   GroupBoxCnd->Width=Form1->Width-40;


   GroupBoxAbout->Top=100;
   GroupBoxAbout->Height=Form1->Height-140;
   GroupBoxAbout->Left=20;
   GroupBoxAbout->Width=Form1->Width-40;

   Image2->Top=16;
   Image2->Left=20;
   Image2->Width=GroupBoxSearch->Width-40;


   RichEdit1->Top=112;
   RichEdit1->Height = GroupBoxSearch->Height-132;
   RichEdit1->Left=20;
   RichEdit1->Width=GroupBoxSearch->Width-40;

   GroupBoxQR->Top=100;
   GroupBoxQR->Height=Form1->Height-120;
   GroupBoxQR->Left=20;
   GroupBoxQR->Width=Form1->Width-40;

   GroupBoxEdit->Top=100;
   GroupBoxEdit->Height=Form1->Height-140;
   GroupBoxEdit->Left=20;
   GroupBoxEdit->Width=Form1->Width-40;

   GroupBoxSearch->Top=100;
   GroupBoxSearch->Height=Form1->Height-140;
   GroupBoxSearch->Left=20;
   GroupBoxSearch->Width=Form1->Width-40;

   ListBoxSearch->Top=120;
   ListBoxSearch->Height = GroupBoxSearch->Height-130;
   ListBoxSearch->Left=10;
   ListBoxSearch->Width=GroupBoxSearch->Width-20;

   //=====================init random number generator==========
   //initialize random number generator with entropy collected with HAVAGE algo
   i=initrnd();
   if(i<2) ShowMessage("Random generator not initialized!");

}
//---------------------------------------------------------------------------

//Switch to maint input panel
void __fastcall TForm1::RadioButtonEditClick(TObject *Sender)
{
 GroupBoxEdit->Visible=true;
 GroupBoxSearch->Visible=false;
 GroupBoxQR->Visible=false;
 GroupBoxAbout->Visible=false;
 GroupBoxCnd->Visible=false;
}
//---------------------------------------------------------------------------

//Switch to QRcode panel
void __fastcall TForm1::RadioButtonQRClick(TObject *Sender)
{
 GroupBoxEdit->Visible=false;
 GroupBoxSearch->Visible=false;
 GroupBoxQR->Visible=true;
 GroupBoxAbout->Visible=false;
 GroupBoxCnd->Visible=false;
}
//---------------------------------------------------------------------------
 //Switch to search panel
void __fastcall TForm1::RadioButtonSearchClick(TObject *Sender)
{
 GroupBoxEdit->Visible=false;
 GroupBoxSearch->Visible=true;
 GroupBoxQR->Visible=false;
 GroupBoxAbout->Visible=false;
 GroupBoxCnd->Visible=false;
}

//---------------------------------------------------------------------------
//switch to About panel
void __fastcall TForm1::RadioButtonAboutClick(TObject *Sender)
{
  GroupBoxEdit->Visible=false;
 GroupBoxSearch->Visible=false;
 GroupBoxQR->Visible=false;
 GroupBoxAbout->Visible=true;
 GroupBoxCnd->Visible=false;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RadioButtonCndClick(TObject *Sender)
{
  GroupBoxEdit->Visible=false;
 GroupBoxSearch->Visible=false;
 GroupBoxQR->Visible=false;
 GroupBoxAbout->Visible=false;
 GroupBoxCnd->Visible=true;
}
//---------------------------------------------------------------------------

//ellastic resize of app form
void __fastcall TForm1::FormResize(TObject *Sender)
{
  GroupBoxQR->Top=100;
   GroupBoxQR->Height=Form1->Height-140;
   GroupBoxQR->Left=20;
   GroupBoxQR->Width=Form1->Width-40;

   GroupBoxCnd->Top=100;
   GroupBoxCnd->Height=Form1->Height-140;
   GroupBoxCnd->Left=20;
   GroupBoxCnd->Width=Form1->Width-40;

   GroupBoxAbout->Top=100;
   GroupBoxAbout->Height=Form1->Height-140;
   GroupBoxAbout->Left=20;
   GroupBoxAbout->Width=Form1->Width-40;




   RichEdit1->Top=112;
   RichEdit1->Height = GroupBoxSearch->Height-132;
   RichEdit1->Left=20;
   RichEdit1->Width=GroupBoxSearch->Width-40;

   Image2->Top=16;
   Image2->Left=20;
   Image2->Width=GroupBoxSearch->Width-40;

   GroupBoxEdit->Top=100;
   GroupBoxEdit->Height=Form1->Height-140;
   GroupBoxEdit->Left=20;
   GroupBoxEdit->Width=Form1->Width-40;

   GroupBoxSearch->Top=100;
   GroupBoxSearch->Height=Form1->Height-140;
   GroupBoxSearch->Left=20;
   GroupBoxSearch->Width=Form1->Width-40;

   ListBoxSearch->Top=120;
   ListBoxSearch->Height = GroupBoxSearch->Height-130;
   ListBoxSearch->Left=10;
   ListBoxSearch->Width=GroupBoxSearch->Width-20;


}
//---------------------------------------------------------------------------

//generate random password for current user
void __fastcall TForm1::ButtonPswClick(TObject *Sender)
{
 //generate random password
 char str[32]={0};
 
 getpas(str, 15); //get random binary data
 EditPsw->Text=AnsiString(str); //convert to base64 charset for readable password
}
//---------------------------------------------------------------------------

//start searching users in database by specified data
void __fastcall TForm1::ButtonSearchClick(TObject *Sender)
{
 char mode;
 char fld;
 char str[128];
 int i;

 ListBoxSearch->Clear(); //clear output

 //check serch field: FIO or INN
 if(RadioButtonINN->Checked)
 {
  strncpy(str, EditSearchINN->Text.c_str(), sizeof(str));
  fld=FIELD_INN;
 }
 else
 {
  strncpy(str, EditSearchFIO->Text.c_str(), sizeof(str));
  fld=FIELD_FIO;
 }

 //check serch mode: starts, containg or match
 if(RadioButtonStart->Checked) mode=SEARCH_START;
 else if(RadioButtonContain->Checked) mode=SEARCH_CONTAIN;
 else mode=SEARCH_MATCH;

 //serch specified string in database
 i=db_search(fld, mode, str);

 //check DB error or output nu,ber of found results
 if(i<0) LabelInfo->Caption=AnsiString(db_err());
 else LabelInfo->Caption=IntToStr(i)+" voters found";

}
//---------------------------------------------------------------------------


//open/close database
void __fastcall TForm1::ButtonDBOpenClick(TObject *Sender)
{
  int i;
  if(!Is_DB) //DB closed: try open
  {
   if(EditDBPath->Text=="") return; //path must be specified
   i=db_open(EditDBPath->Text.c_str()); //open
   if(!i) //if opened success
   {
    EditDBPath->Color=clLime; //set Edit's color
    Is_DB=1;   //set flag
    ButtonDBOpen->Caption="Close DB"; //change Button's caption
    LabelInfo->Caption="DataBase was opened";
   }
   else LabelInfo->Caption="Error opening Database";
  }
  else   //close DB
  {
   i=db_open(0);  //close
   if(!i) //if cosed success
   {
    EditDBPath->Color=clWhite; //reset Edit's color
    Is_DB=0; //clear flag
    ButtonDBOpen->Caption="Open DB"; //restore Button's caption
    LabelInfo->Caption="DataBase was closed";
   } else LabelInfo->Caption="Error closing Database";
  }



}
//---------------------------------------------------------------------------

//Select user in search output list, load user's dat and set it as current user
void __fastcall TForm1::ListBoxSearchDblClick(TObject *Sender)
{
  int i,j;
  AnsiString S="";

  //get list index of output results was doubleclicked
  i=ListBoxSearch->ItemIndex;
  if(i<0)
  {
   LabelInfo->Caption="Iteam not selected in list";
   return;
  }

  //get output string was doubleclicked
  S=ListBoxSearch->Items->Strings[i];
  if(S=="")
  {
   LabelInfo->Caption="Selected iteam is empty";
   return;
  }

  //serch start of user ID in this string
  i=S.Pos("#");
  if(!i)
  {
   LabelInfo->Caption="Iteam's index not found in string";
   return;
  }

  //extract substring of user ID decimal number
  S=S.SubString(i+1, 15);
  i=atoi(S.c_str()); //convert to number
  if(i<=0)
  {
   LabelInfo->Caption="Iteam's index is not valid number in string";
   return;
  }

  //read user's data by this ID from database, output to main input panel over callback
  j=db_read(i);
  if(j<0)
  {
   LabelInfo->Caption=AnsiString(db_err());
   return;
  }

  if(j==0)
  {
   LabelInfo->Caption="Iteam not found in DataBase";
   return;
  }

  //set current Voter ID
  Voter=i;
  EditVoter->Text=IntToStr(i);
  LabelInfo->Caption="Voter "+IntToStr(i)+ " is readed";

  //switch to maint input panel
  RadioButtonEdit->Checked=true;
  GroupBoxEdit->Visible=true;
  GroupBoxSearch->Visible=false;
  GroupBoxQR->Visible=false;
  GroupBoxAbout->Visible=false;

}
//---------------------------------------------------------------------------

//load user specified by ID from database 
void __fastcall TForm1::ButtonLoadClick(TObject *Sender)
{

 int i,j;

 //get user ID specified in field
 i=StrToIntDef(EditVoter->Text, 0);
 if(!i)
 {
  LabelInfo->Caption="Voter's index is not valid number";
  return;
 }

 //read data of this user from db (will be output to main input panel over callback)
 j=db_read(i);
  if(j<0)
  {
   LabelInfo->Caption=AnsiString(db_err());
   return;
  }

  if(j==0)
  {
   LabelInfo->Caption="Voter not found in DataBase";
   return;
  }

  //set current Voter
  Voter=i;
  EditVoter->Text=IntToStr(i);
  LabelInfo->Caption="Voter "+IntToStr(i)+ " is readed";

}
//---------------------------------------------------------------------------


//clear user's input form for next user will be add to database
void __fastcall TForm1::ButtonNextClick(TObject *Sender)
{

 //clear all field on main input panet for add new user
 Voter=0;
 EditVoter->Text="NEW";
 EditName->Text="";
 EditINN->Text="";
 EditPsw->Text="";
 LabelSec->Caption="Sec=";
 LabelSig->Caption="Sig=";
}
//---------------------------------------------------------------------------

//add / change current user to database
void __fastcall TForm1::ButtonSaveClick(TObject *Sender)
{

  reg_data reg;
  int i,j;

  //get current user ID or 0 for add new user
  i=StrToIntDef(EditVoter->Text, 0);

  //add data from user input panel to structere
  reg.Idn=i;
  strncpy(reg.Fio, EditName->Text.c_str(), 126);
  strncpy(reg.Inn, EditINN->Text.c_str(), 15);
  strncpy(reg.Psw, EditPsw->Text.c_str(), 15);

  //add/change user to database
  j=db_add(&reg);

  if(j<=0)
  {
   LabelInfo->Caption=AnsiString(db_err());
   return;
  }


  //inform for user was changed
  if(i)
  {
   LabelInfo->Caption="Voter "+IntToStr(i)+" was saved";
   return;
  }

  //or inform for ID of user was add to database
  LabelInfo->Caption="New Voter "+ IntToStr(j)+ " added to database";
  EditVoter->Text=IntToStr(j);
  Voter=j;

}
//---------------------------------------------------------------------------

//Delete specified User ID from database
void __fastcall TForm1::ButtonDeleteClick(TObject *Sender)
{
 int i, j;
 AnsiString S="нет";
 AnsiString S1;

 //get user ID will be deleted from databse
 i=StrToIntDef(EditVoter->Text, 0);
 if(!i)
 {
  LabelInfo->Caption="Voter's index is not valid number";
  return;
 }

 //Show messagebox for apply
 S1="Удаление участника голосования #"+IntToStr(i);

 if (!InputQuery (S1, "Подтвердите: да/yes", S)) return;
 j=0;
 if(S!="да") j=1;
 if(S!="Да") j=1;
 if(S!="Да") j=1;
 if(S!="yes") j=1;
 if(S!="Yes") j=1;
 if(S!="YES") j=1;

 if(!j)
 {
  LabelInfo->Caption="Delete was canceled";
  return;
 }

 //delete specified user from databse
 j=db_delete(i);
 if(j<0)
 {
   LabelInfo->Caption=AnsiString(db_err());
   return;
 }
 else
 {
  //on deleting sucess clear fields ready for add new user
  Voter=0;
  EditVoter->Text="NEW";
  EditName->Text="";
  EditINN->Text="";
  EditPsw->Text="";
  LabelSec->Caption="Sec=";
  LabelSig->Caption="Sig=";

  LabelInfo->Caption="Voter "+IntToStr(i)+" was deleted";
 }


}
//---------------------------------------------------------------------------



//draw Tickets image with QR-code and text info using current Voter's info
void __fastcall TForm1::ButtonQRClick(TObject *Sender)
{
 AnsiString S;
 unsigned char* p;
 unsigned short w;

 unsigned char qrdata[36];
 char qrstr[48];

 //QRCode qrcode;
 unsigned char qrcodeBytes[106]; //for version 3	
 unsigned short x, y, xx, yy;
 unsigned char c;
 int len, i;

 uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
 uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
 bool ok;

 //---------recognize test------------

     #define REC_W 240
     #define REC_H 320


     struct quirc_code code;
	struct quirc *q=0;
	struct quirc_data qd;
	uint8_t *image=0;
	int id_count=-1;
        unsigned char qrrec[36];
        char recpsw[16]={0,};
        char recpsw1[16]={0,};
        unsigned char recadr[10]={0,};
        unsigned char recadr1[10]={0,};
        int recvoter=0;
        int recvoter1=0;

        q = quirc_new();
	if (!q)
        {
		printf("can't create quirc object\r\n");
	}
	
	
	if(q)
	{
	       id_count=quirc_resize(q, REC_W, REC_H);
	}
	
	if (id_count<0)
	{
     	        printf("quirc_resize err\r\n");
		if(q) quirc_destroy(q);
                q=0;
	}
	
	if(q)
	{
	 image = quirc_begin(q, NULL, NULL);
	}
	
	
	if(!image)
	{
	    printf("quirc_image err\r\n");
            if(q) quirc_destroy(q);
            q=0;
	
	}

	i=sizeof(*image);
        memset(image, 0xFF, REC_W*REC_H);

//------------------------------------





  isqrcode=0; //ckaer qrcode ready flag
  memset(dat_qrcode, 0, sizeof(dat_qrcode));



   //set width and height of output bitmap image
    Image1->Width=IMG_W;
  Image1->Height=IMG_H;

 //check onion server was loaded
 if(!isonion)
 {
  LabelInfo->Caption="Server's onion address not loaded";
  return;
 }

 //check voter was loaded
 if(!Voter)
 {
  LabelInfo->Caption="Voter not loaded/saved";
  return;
 }

 //check all required user's data are specified
 if(EditName->Text=="")
 {
  LabelInfo->Caption="Voter's name not specified";
  return;
 }

 if(EditINN->Text=="")
 {
  LabelInfo->Caption="Voter's INN not specified";
  return;
 }

 if(EditPsw->Text=="")
 {
  LabelInfo->Caption="Voter's password not specified";
  return;
 }

  //switch to QR panel
  RadioButtonQR->Checked=true;
  GroupBoxEdit->Visible=false;
  GroupBoxSearch->Visible=false;
  GroupBoxQR->Visible=true;
  GroupBoxAbout->Visible=false;

  //clear QR image
  Image1->Canvas->Brush->Color=clWhite;
  Image1->Canvas->FillRect(Rect(0,0,Image1->Width, Image1->Height));

  //Output voter's data as text info on image
  Image1->Canvas->Font->Style << fsBold;
  Image1->Canvas->Font->Color=clBlack;
  Image1->Canvas->Font->Size=14;

  Image1->Canvas->TextOutA(10, Image1->Width, EditName->Text);
  S="ID: "+IntToStr(Voter)+"  INN: "+EditINN->Text;
  Image1->Canvas->TextOutA(10, Image1->Width+2*QR_TEXTSIZE, S);
  S="Пароль: "+EditPsw->Text;
  Image1->Canvas->TextOutA(10, Image1->Width+4*QR_TEXTSIZE, S);
  S="Сервер: "+OnionStr;
  Image1->Canvas->TextOutA(10, Image1->Width+6*QR_TEXTSIZE, S);

  //set 32 bytes data for QR-code
  memset(qrdata, 0, sizeof(qrdata));   //clear
  strncpy(qrdata, EditPsw->Text.c_str(), 16); //copy password string up to 15 chars
  strncpy(recpsw1, EditPsw->Text.c_str(), 16); 
  itom(qrdata+16, (unsigned int)Voter); //copy 4 bytes of binary Voter's id
  recvoter1=Voter;
  memcpy(qrdata+20, onion, 10);       //copy 10 bytes of binary server's onion adress
  memcpy(recadr1, onion, 10);
  //add crc16 to qrcode data
  w=telcrc16(qrdata, 30);
  qrdata[30]=w&0xFF;
  qrdata[31]=w>>8;

  //copy 32-bytes data to temp buffer
  if(qrcodegen_BUFFER_LEN_MAX < 32)
  {
   LabelInfo->Caption="Qr code buffer error";
   return;
  }
  memcpy(tempBuffer, qrdata, 32);


  //generate qr-code version 3
  ok = qrcodegen_encodeBinary(tempBuffer, 32, qrcode,
	qrcodegen_Ecc_QUARTILE, 3, 3, qrcodegen_Mask_AUTO, true);
  if(!ok)
  {
   LabelInfo->Caption="Qr code creating error";
   return;
  }





  //draw QR code on image by pixels
  for(y=0;y<QRMODULE_N;y++) //draw lines of modules
  {
   for(yy=0;yy<QRMODULE_S;yy++) //draw lines of pixels for module height
   {
    for(x=0;x<QRMODULE_N;x++) //draw module in line
    {
     //if(qrcode_getModule(&qrcode, x, y))//get module color

     if(qrcodegen_getModule(qrcode, x, y))//get module color
     {
      for(xx=0;xx<QRMODULE_S;xx++)  //draw pixels for module width
      Image1->Canvas->Pixels[x*QRMODULE_S+xx+4*QRMODULE_S][y*QRMODULE_S+yy+5*QRMODULE_S]=clBlack;
     } //end of qr-module is black
    } //end of draw module
   } //end of draw lines block
  } //end of draw line



//------------------qr recognition test------------------


        //draw QR code on image by pixels
  for(y=0;y<29;y++) //draw lines of modules
  {
   for(yy=0;yy<6;yy++) //draw lines of pixels for module height
   {
    for(x=0;x<29;x++) //draw module in line
    {
     //if(qrcode_getModule(&qrcode, x, y))//get module color

     if(qrcodegen_getModule(qrcode, x, y))//get module color
     {
      for(xx=0;xx<6;xx++)  //draw pixels for module width
      if(q && image)
      {

       //i=y*6+yy+4*6; //line number
       //i*=REC_W;
       //i+=(x*6+xx+4*6); //pixel number

       i=x*6+xx+4*6; //line number
       i=REC_H-i;
       i*=REC_W;
       i+=(y*6+yy+4*6); //pixel number



       image[i]=0;
      } //end of compose test image for recognition
     } //end of qr-module is black
    } //end of draw module
   } //end of draw lines block
  } //end of draw line





        id_count=0;

	if(q)
	{
	 quirc_end(q);
	 printf("quirc_end\r\n");
    	 id_count = quirc_count(q);
        }

	if (id_count == 0)
        {
		fprintf(stderr, "Error: not a valid qrcode\n");
		if(q) quirc_destroy(q);
		q=0;
	}

	if(q)
	{



	 quirc_extract(q, 0, &code);
	 quirc_decode(&code, &qd);
	 quirc_destroy(q);

         S="Ver: "+IntToStr(qd.version);
         S=S+" Type: "+IntToStr(qd.data_type);
         S= S+" Len: "+IntToStr(qd.payload_len);
         i=qd.payload_len;
         if(i>sizeof(qrrec)) i=sizeof(qrrec);
         memcpy(qrrec, qd.payload, i);

         //add crc16 to qrcode data
         w=telcrc16(qrrec, 30);
         qrrec[30]^=(w&0xFF);
         qrrec[31]^=(w>>8);

         //CRC16
         w=qrrec[30] | qrrec[31];

         recvoter=mtoi(qrrec+16);
         memcpy(recadr, qrrec+20, 10);
         qrrec[16]=0;
         strncpy(recpsw, (char*)qrrec, sizeof(recpsw));

       


        }

//----------------------------------------------



 LabelInfo->Caption="Qr code for Voter #"+IntToStr(Voter)+ " is composed";
 memcpy(dat_qrcode, qrdata, sizeof(dat_qrcode)); //store qrcode
 isqrcode=Voter; //set current qrcode Id

}
//---------------------------------------------------------------------------

//save composed voting ticket to JPEG image file with number as Voter's ID
void __fastcall TForm1::ButtonSaveQRClick(TObject *Sender)
{
  int w=IMG_W;
  int h=IMG_H;
  jpec_enc_t *e=0;
  const uint8_t *jpeg=0;
  int len;
  int i;
  int x;
  int y;
  
  FILE *file;

  uint8_t img[IMG_W*IMG_H];

  char path[512];
  int pathlen;
  char str[128];

  //get path of app
  AnsiString exeFile=Application->ExeName;
  AnsiString exePath=ExtractFilePath(exeFile);

  //get path of folder for output images
  exePath = exePath + "qr_codes";
  if(!DirectoryExists(exePath)) CreateDir(exePath); //create if not exist yet

  //check total path too long
  strncpy(path, exePath.c_str(), sizeof(path));
  pathlen=strlen(path);
  if(pathlen >(sizeof(path)-17))
  {
   LabelInfo->Caption="Path too long";
   return;
  }

  //check we have qrcode and compose output filename with Voter's ID
  if(!isqrcode)
  {
   LabelInfo->Caption="No QR-code for saving";
   return;
  }

  //save binary dat_qrcode as alternative for jpeg QR-code
  sprintf(path+pathlen, "\\d_%d.dat", isqrcode); //compose file name by client number
  file = fopen(path, "wt");
  if(file)
  {
    bin2str(dat_qrcode, str, sizeof(dat_qrcode)); 
    //i=fwrite(dat_qrcode, sizeof(uint8_t), sizeof(dat_qrcode), file);
    fprintf(file, "%s\r\n", str);
    fclose(file);
  }
  file=0;


  //Compose Jpeg file name by client number
  sprintf(path+pathlen, "\\q_%d.jpg", isqrcode);

  //scan drawed image bitmap to 8bits grayscale array
  for(y=0;y<IMG_H;y++)
  {
   for(x=0;x<IMG_W;x++)
   {
    i=Image1->Canvas->Pixels[x][y];
    if(clBlack==Image1->Canvas->Pixels[x][y])
    {
     img[y*IMG_W + x]=0x00;
    }
    else img[y*IMG_W + x]=0xFF;
   }
  }

  //create jpeg encoder
  e = jpec_enc_new(img, w, h);
  if(!e)
  {
   LabelInfo->Caption="JPEG comressor error";
   return;
  }

   //encode array to jpeg data
  jpeg = jpec_enc_run(e, &len);

  //save jpeg data to file
  i=-1;
  file = fopen(path, "wb");
  if(jpeg && (len>0) && file)
  {
    i=fwrite(jpeg, sizeof(uint8_t), len, file);
  }

  if(file) fclose(file);

  jpec_enc_del(e);

  if(i!=len)
  {
   LabelInfo->Caption="JPEG saving error";
   return;
  }


   LabelInfo->Caption="Ticket "+IntToStr(isqrcode)+" saved success";

}
//---------------------------------------------------------------------------


//Browse path to Tor hidden_service/hostname file and load onion adress
//(server's onion-adress needs for Tickets)
void __fastcall TForm1::ButtonSrvLoadClick(TObject *Sender)
{
 int i;

 //get app folder path
 AnsiString exeFile=Application->ExeName;
 AnsiString exePath=ExtractFilePath(exeFile);

 //set previous path or app path as start for dialog
 //if(OnionPath != "")
 //{
 // OpenDialog2->FileName=OnionPath;
 //} else if(EditDBPath->Text!="") OpenDialog2->FileName = EditDBPath->Text;

 //select Tor hostname file in open dialog
 if(OpenDialog2->Execute())
 {

  OnionPath=OpenDialog2->FileName;  //set path to this file
  List->Clear();
  try{
      List->LoadFromFile(OnionPath); //read hostname
     }
  catch(...)
  {

  }
 }


   //load onion adress from selected hostname file
  if(List->Count)
  {
   char str[32];
   unsigned char adr[16]={0,};

   LabelOnion->Caption=List->Strings[0]; //get base32 string
   strncpy(str, List->Strings[0].c_str(), sizeof(str));
   i=base32_decode(adr, str); //decode to binary 9compress
   if(i)  //check base32 is valid
   {
    ShowMessage("Неверный onion-аддресс!");
   }
   else  //onion address is valid
   {
    memcpy(onion, adr, 10); //store compressed onion address
    isonion=1;  //set flag we gave onion address of server
    LabelOnion->Caption=LabelOnion->Caption+" (seems valid)"; //show it

    //save path to hidden service file
    OnionStr=List->Strings[0];
    List->Clear();
    List->Add(OnionPath);
    List->SaveToFile(exePath+"onpath.ini"); //save server's onion address to ini file will be loaded on next app start
   }

  }

}
//---------------------------------------------------------------------------




void __fastcall TForm1::Button3Click(TObject *Sender)
{
 AnsiString exeFile=Application->ExeName;
 AnsiString exePath=ExtractFilePath(exeFile);
 AnsiString S;

 //set this path as start for open dialog
 if(OpenDialogCnd->FileName == "")
 {
  //OpenDialogCnd->FileName=exePath;
 }

 //sslect database file in dialog
 if(!OpenDialogCnd->Execute()) return;
 if(OpenDialogCnd->FileName == "") return;

 S = OpenDialogCnd->FileName;
 LabelCndPath->Caption = S;

 List->Clear();
 List->Add(S);
 List->SaveToFile(exePath+"cdpath.ini");

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
  AnsiString S;

 if(LabelCndPath->Caption == "")
 {
  LabelCndInfo->Caption = "Файл списка кандидатов не выбран!";
  return;
 }

 S= LabelCndPath->Caption;
 MemoCnd->Lines->Clear();
 try{
      MemoCnd->Lines->LoadFromFile(S);
      LabelCndInfo->Caption = "Загружено "+ IntToStr(MemoCnd->Lines->Count) + " кандидатов";
     }
  catch(...)
  {
   LabelCndInfo->Caption = "Файл со списком кандидатов не найден!";
   S="";
  }

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
 AnsiString S;

 if(LabelCndPath->Caption == "")
 {
  LabelCndInfo->Caption = "Файл списка кандидатов не выбран!";
  return;
 }

 S= LabelCndPath->Caption;
 try{
      MemoCnd->Lines->SaveToFile(S);
      LabelCndInfo->Caption = "Сохранено "+ IntToStr(MemoCnd->Lines->Count) + " кандидатов";
     }
  catch(...)
  {
   LabelCndInfo->Caption = "Файл со списком кандидатов не найден!";
   S="";
  }

}
//---------------------------------------------------------------------------

