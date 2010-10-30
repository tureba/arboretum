//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include "CSPIN.h"
#include <Buttons.hpp>
#include <stdlib.h>
#include <time.h>

// Metric Tree includes
#include <arboretum/stPlainDiskPageManager.h>
#include <arboretum/stSlimTree.h>

// My object
#include "city.h"
#include "AboutBox.h"

#include <string.h>
#include <fstream>

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
   __published:	// IDE-managed Components
      TRadioGroup *RGQueryType;
      TGroupBox *GBQueryCity;
      TEdit *ECityName;
      TEdit *ECityLatitude;
      TEdit *ECityLongitude;
      TLabel *Label1;
      TLabel *Label2;
      TLabel *Label3;
      TMainMenu *MainMenu1;
      TMenuItem *Exit1;
      TMenuItem *About1;
      TGroupBox *GBResult;
      TMemo *MResult;
      TGroupBox *GBQueryParameter;
      TCSpinEdit *CSNearestNumber;
      TEdit *ERadius;
      TLabel *Label4;
      TLabel *Label5;
      TBitBtn *BRun;
      TMenuItem *BuildSlimTree1;
      TMenuItem *SlimTree1;
      TBitBtn *BSample;
      TBitBtn *BClear;
      void __fastcall Exit1Click(TObject *Sender);
      void __fastcall SlimTree1Click(TObject *Sender);
      void __fastcall BRunClick(TObject *Sender);
      void __fastcall BSampleClick(TObject *Sender);
      void __fastcall About1Click(TObject *Sender);
      void __fastcall BClearClick(TObject *Sender);
      void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

   private:	// User declarations

         /**
         * This is the type used by the result.
         */
         typedef stResult < TCity > myResult;

         /**
         * This is the type of the Slim-Tree defined by TCity and
         * TCityDistanceEvaluator.
         */
         typedef stSlimTree < TCity, TCityDistanceEvaluator > mySlimTree;

         /**
         * The Page Manager for Slim.
         */
         stPlainDiskPageManager * PageManagerSlim;

         /**
         * The Slim-Tree.
         */
         mySlimTree * SlimTree;

         /**
         * Vector for holding the query objects.
         */
         vector <TCity *> queryCities;

         /**
         * Loads the tree from file with a set of cities.
         */
         void LoadTree(char * fileName);

         /**
         * Loads the sample vector.
         */
         void LoadSample(char * fileName);

   public:		// User declarations

      __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
