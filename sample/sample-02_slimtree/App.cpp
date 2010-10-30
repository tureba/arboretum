//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "App.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
   : TForm(Owner)
{
   // Set initial values.
   PageManagerSlim = NULL;
   SlimTree = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Exit1Click(TObject *Sender)
{
   // If the SlimTree exist.
   if (this->SlimTree != NULL){
      delete this->SlimTree;
   }//end if
   // If the PageManagerSlim exist.
   if (this->PageManagerSlim != NULL){
      delete this->PageManagerSlim;
   }//end if

   // For every entry in queryCities.
   for (int i = 0; i < queryCities.size(); i++){
      delete queryCities[i];
   }//end if

   // Terminate the application.      
   Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SlimTree1Click(TObject *Sender)
{
   // Create a page manager to slim-tree.
   PageManagerSlim = new stPlainDiskPageManager("SlimTree.dat", 1024);
   // Create the slim-tree associate to a pageManager.
   SlimTree = new mySlimTree(PageManagerSlim);
   // Load the slim-tree with objects in this file.
   LoadTree("cities.txt");
   // Load the sample vector with objects in this file.
   LoadSample("cities500.txt");
   // Make enable the query button.
   BRun->Enabled = true;
   // Set the maximum value of NearestQuery Button.
   CSNearestNumber->MaxValue = SlimTree->GetNumberOfObjects();
   // For samples.
   randomize();
}
//---------------------------------------------------------------------------

void TForm1::LoadTree(char * fileName){
   ifstream in(fileName);
   int contObj=0;
   char buffer[200], cityName[200];
   double dLat, dLong;
   char * ptr;
   TCity * city;
   string messageText;

   // If the slim-tree was created.
   if (SlimTree != NULL){
      // Try to open the file.
      if (!in.is_open()){
         // It is not possible to open the file.
         MResult->Lines->Add("Cannot open input file!");
      }else{
         while(in.getline(cityName, 200, '\t')){
            in >> dLat;
            in >> dLong;
            in.ignore();
            // Construct the object.
            city = new TCity(cityName, dLat, dLong);
            // Add it in the Slim-Tree.
            SlimTree->Add(city);
            // Clean.
            delete city;
            // Update the object counter.
            contObj++;
         }//end while
         // Build the message.
         messageText = "Added ";
         messageText += itoa(contObj, cityName, 10);
         messageText += " objects in Slim-Tree";
         // Close the file.
         in.close();
         // Set the query button enable.
         SlimTree1->Enabled = false;
      }//end else
  }else{
      // Build the message.
      messageText = "Zero object added.";
  }//end else
  // Show the message.
  Application->MessageBoxA(messageText.c_str(), "Slim-Tree", MB_OK);

}//end TForm1::LoadTree
//---------------------------------------------------------------------------

void TForm1::LoadSample(char * fileName){
   ifstream in(fileName);
   int contObj=0;
   char buffer[200], cityName[200];
   double dLat, dLong;
   char * ptr;
   TCity * city;
   string messageText;

   if (SlimTree != NULL){
      // Try to open the file.
      if (!in.is_open()){
         // It is not possible to open the file.
         MResult->Lines->Add("Cannot open input file!");
      }else{
         // First clean the vector.
         queryCities.clear();
         // Get the first line.
         while(in.getline(cityName, 200, '\t')){
            in >> dLat;
            in >> dLong;
            in.ignore();
            // Build the query city.
            queryCities.insert(queryCities.end(), new TCity(cityName, dLat, dLong));
         }//end while
         // Close the file
         in.close();
         // Make the sample button enable.
         BSample->Enabled = true;
      }//end else
  }//end else

}//end TForm1::LoadSample
//---------------------------------------------------------------------------

void __fastcall TForm1::BRunClick(TObject *Sender)
{
   myResult * result;   // To hold the result
   TCity * sample;      // For query object.
   TCity * tmp;
   AnsiString text;
   int i;

   // Build the query object.
   sample = new TCity(ECityName->Text.c_str(),
                      StrToFloat(ECityLatitude->Text),
                      StrToFloat(ECityLongitude->Text));

   switch (RGQueryType->ItemIndex){
      case 0:  // Nearest Query
         result = SlimTree->NearestQuery(sample, StrToInt(CSNearestNumber->Value));
         break;
      case 1:  // Range Query
         result = SlimTree->RangeQuery(sample, StrToFloat(ERadius->Text));
         break;
      case 2:  // Point Query
         result = SlimTree->PointQuery(sample);
         break;
   }//end switch

   // Write the sample object in the memo.
   text = "The sample object was: ";
   text += (*(result->GetSample())).GetName().c_str();
   text += "; ";
   text += (*(result->GetSample())).GetLatitude();
   text += "; ";
   text += (*(result->GetSample())).GetLongitude();
   MResult->Lines->Add(text);

   // Is there a entry in the result?
   if (!result->GetNumOfEntries()){
      MResult->Lines->Add("No results!");
   }else{
      // Write the number of entries in memo.
      text = "Number of results: ";
      text += result->GetNumOfEntries();
      MResult->Lines->Add(text);
      // Write the query result.
      for (i = 0; i < result->GetNumOfEntries(); i++){
         tmp = (TCity *)(*result)[i].GetObject();
         text = " Distance = ";
         text += (*result)[i].GetDistance();
         text += " - Object: ";
         text += tmp->GetName().c_str();
         text += "; ";
         text += tmp->GetLatitude();
         text += "; ";
         text += tmp->GetLongitude();
         MResult->Lines->Add(text);
      }//end for
   }//end if
   // Write a blank line.
   MResult->Lines->Add("");

   // Clean the mess.
   delete result;
   delete sample;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BSampleClick(TObject *Sender)
{
   int i;
   // Choose a number between 0 and the sample size.
   i = rand() % queryCities.size();
   // If a number is valid.
   if (i >= 0 && i < queryCities.size()){
      // Set the values in apropriate fields.
      ECityName->Text = (queryCities[i])->GetName().c_str();
      ECityLatitude->Text = (queryCities[i])->GetLatitude();
      ECityLongitude->Text = (queryCities[i])->GetLongitude();
   }//end if
}
//---------------------------------------------------------------------------

void __fastcall TForm1::About1Click(TObject *Sender)
{
/*
    TAboutBox * AboutBox = new TAboutBox(Application);
    AboutBox->ShowModal();
    delete AboutBox;/**/
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BClearClick(TObject *Sender)
{
   MResult->Lines->Clear();   
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
   Exit1Click(Sender);
}
//---------------------------------------------------------------------------
