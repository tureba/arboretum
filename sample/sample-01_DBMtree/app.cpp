//---------------------------------------------------------------------------
// app.cpp - Implementation of the application.
//
// To change the behavior of this application, comment and uncomment lines at
// TApp::Init() and TApp::Query().
//
// Authors: Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
// Copyright (c) 2003 GBDI-ICMC-USP
//---------------------------------------------------------------------------
#include <iostream>
#pragma hdrstop
#include "app.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// Class TApp
//------------------------------------------------------------------------------
void TApp::CreateTree(){
   // create for DBM-Tree
   DBMTree = new myDBMTree(PageManager);
}//end TApp::CreateTree

//------------------------------------------------------------------------------
void TApp::CreateDiskPageManager(){
   //for DBMTree
   PageManager = new stPlainDiskPageManager("DBMTree.dat", 1024);
}//end TApp::CreateDiskPageManager

//------------------------------------------------------------------------------
void TApp::Run(){
   // Lets load the tree with a lot values from the file.
   cout << "\n\nAdding objects in the DBMTree";
   LoadTree(CITYFILE);

   cout << "\n\nLoading the query file";
   LoadVectorFromFile(QUERYCITYFILE);

   if (queryObjects.size() > 0){
      // Do 500 queries.
      PerformQueries();
   }//end if
   // Hold the screen.
   cout << "\n\nFinished the whole test!";
}//end TApp::Run

//------------------------------------------------------------------------------
void TApp::Done(){

   if (this->DBMTree != NULL){
      delete this->DBMTree;
   }//end if
   if (this->PageManager != NULL){
      delete this->PageManager;
   }//end if

   // delete the vetor of queries.
   for (unsigned int i = 0; i < queryObjects.size(); i++){
      delete (queryObjects.at(i));
   }//end for
}//end TApp::Done

//------------------------------------------------------------------------------
void TApp::LoadTree(char * fileName){
   ifstream in(fileName);
   char cityName[200];
   double dLat, dLong;
   long w = 0;
   TCity * city;

   if (DBMTree!=NULL){
      if (in.is_open()){
         cout << "\nLoading objects ";
         while(in.getline(cityName, 200, '\t')){
            in >> dLat;
            in >> dLong;
            in.ignore();
            city = new TCity(cityName, dLat, dLong);
            DBMTree->Add(city);
            delete city;
            w++;
            if (w % 10 == 0){
               cout << '.';
            }//end if
         }//end while
         cout << " Added " << DBMTree->GetNumberOfObjects() << " objects ";
         in.close();
      }else{
         cout << "\nProblem to open the file.";
      }//end if
   }else{
      cout << "\n Zero object added!!";
   }//end if
}//end TApp::LoadTree

//------------------------------------------------------------------------------
void TApp::LoadVectorFromFile(char * fileName){
   ifstream in(fileName);
   char cityName[200];
   int cont;
   double dLat, dLong;

   // clear before using.
   queryObjects.clear();

   if (in.is_open()){
      cout << "\nLoading query objects ";
      cont = 0;
      while(in.getline(cityName, 200, '\t')){
         in >> dLat;
         in >> dLong;
         in.ignore();
         this->queryObjects.insert(queryObjects.end(), new TCity(cityName, dLat, dLong));
         cont++;
      }//end while
      cout << " Added " << queryObjects.size() << " query objects ";
      in.close();
   }else{
      cout << "\nProblem to open the query file.";
      cout << "\n Zero object added!!\n";
   }//end if
}//end TApp::LoadVectorFromFile

//------------------------------------------------------------------------------
void TApp::PerformQueries(){
   if (DBMTree){
      cout << "\nStarting Statistics for Range Query with DBMTree.... ";
      PerformRangeQuery();
      cout << " Ok\n";

      cout << "\nStarting Statistics for Nearest Query with DBMTree.... ";
      PerformNearestQuery();
      cout << " Ok\n";
   }//end if
}//end TApp::PerformQuery

//------------------------------------------------------------------------------
void TApp::PerformRangeQuery(){

   myResult * result;
   stDistance radius;
   clock_t start, end;
   unsigned int size;
   unsigned int i;

   if (DBMTree){
      size = queryObjects.size();
      // reset the statistics
      PageManager->ResetStatistics();
      DBMTree->GetMetricEvaluator()->ResetStatistics();
      start = clock();
      for (i = 0; i < size; i++){
         result = DBMTree->RangeQuery(queryObjects[i], 0.2);
         delete result;
      }//end for
      end = clock();
      cout << "\nTotal Time: " << ((double )end-(double )start) / 1000.0 << "(s)";
      // is divided for queryObjects to get the everage
      cout << "\nAvg Disk Accesses: " << (double )PageManager->GetReadCount() / (double )size;
      // is divided for queryObjects to get the everage
      cout << "\nAvg Distance Calculations: " <<
         (double )DBMTree->GetMetricEvaluator()->GetDistanceCount() / (double )size;
   }//end if

}//end TApp::PerformRangeQuery

//------------------------------------------------------------------------------
void TApp::PerformNearestQuery(){

   myResult * result;
   clock_t start, end;
   unsigned int size;
   unsigned int i;

   if (DBMTree){
      size = queryObjects.size();
      PageManager->ResetStatistics();
      DBMTree->GetMetricEvaluator()->ResetStatistics();
      start = clock();
      for (i = 0; i < size; i++){
         result = DBMTree->NearestQuery(queryObjects[i], 15);
         delete result;
      }//end for
      end = clock();
      cout << "\nTotal Time: " << ((double )end-(double )start) / 1000.0 << "(s)";
      // is divided for queryObjects to get the everage
      cout << "\nAvg Disk Accesses: " << (double )PageManager->GetReadCount() / (double )size;
      // is divided for queryObjects to get the everage
      cout << "\nAvg Distance Calculations: " <<
         (double )DBMTree->GetMetricEvaluator()->GetDistanceCount() / (double )size;
   }//end if
}//end TApp::PerformNearestQuery
