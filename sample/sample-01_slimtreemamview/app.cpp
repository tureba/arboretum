//---------------------------------------------------------------------------
// Implementation of the application.
//
// Author: Fabio Jun Takada Chino (chino@icmc.sc.usp.br)
// 
// Copyright (c) 2002-2004 GBDI-ICMC-USP
//---------------------------------------------------------------------------
#include <stdlib.h>
#include "app.h"

//---------------------------------------------------------------------------
// Class TApp
//---------------------------------------------------------------------------
void TApp::Init(){

   // Uncoment one of the page manager creation procedures. Do not uncoment both
   // of them.
    CreateDiskPageManager(); // To create it in disk
   //CreateMemoryPageManager(); // To create it in memory
   // Creates the tree
   CreateTree();
}//end TApp::Init

//---------------------------------------------------------------------------
void TApp::Run(){

   // Lets load the tree with a few values.
   LoadTree();                                                             
   // Lets do a query
   PerformQuery();
}//end TApp::Run

//---------------------------------------------------------------------------
void TApp::Done(){
   if (this->SlimTree != NULL){
      delete this->SlimTree;
   }//end if
   if (this->PageManagerSlim != NULL){
      delete this->PageManagerSlim;
   }//end if
}//end TApp::Done

//---------------------------------------------------------------------------
void TApp::CreateMemoryPageManager(){
   // 1024 bytes per page
   PageManagerSlim = new stMemoryPageManager(256);
}//end TApp::CreateMemoryPageManager()

//---------------------------------------------------------------------------
void TApp::CreateDiskPageManager(){
   stDiskPageManager * dmanSlim = new stDiskPageManager();
   // Setup Filename is mytree.dat, 1024 bytes per page
   // HeaderSize is 16 bytes
   dmanSlim->Create("mytreeSlim.dat", PAGESIZE, 64);
   PageManagerSlim = dmanSlim;
}//end TApp::CreateDiskPageManager

//---------------------------------------------------------------------------
void TApp::CreateTree(){
   SlimTree = new mySlimTree(PageManagerSlim);
}//end TApp::CreateTree

//---------------------------------------------------------------------------
void TApp::RandomObject(myObject * obj){
	int w;
	
	for (w = 0; w < obj->GetSize(); w++){
		(*obj)[w] = rand();
	}//end for	
}//end TApp::RandomObject

//---------------------------------------------------------------------------
void TApp::LoadTree(){
	myObject obj(DIM);
   myResult * r;
	int w;
	
	for (w = 0; w < N; w++){
		RandomObject(&obj);
		SlimTree->Add(&obj);
	}//end for
	
	#ifdef __stMAMVIEW__
		// Configure MAMView
		SlimTree->MAMViewSetOutputDir(".");
		
		// Initialize MAMView
		SlimTree->MAMViewInit();
		
		// Create a dump for a k-Nearest Neighbor query
		RandomObject(&obj);
      r = SlimTree->NearestQuery(&obj, 15);
      delete r;

		// Create a dump for a Range query
		RandomObject(&obj);
      r = SlimTree->RangeQuery(&obj, 15000);
      delete r;

		// Create a dump
		SlimTree->MAMViewDumpTree();
	#endif //__stMAMVIEW__
	
}//end TApp::LoadTree

//---------------------------------------------------------------------------
void TApp::PerformQuery(){
   myResult * resultSlim;

   // Print query result.
   // PrintResults(resultSlim);

   // Clean home
//   delete resultSlim;
}//end TApp::PerformQuery
//---------------------------------------------------------------------------
