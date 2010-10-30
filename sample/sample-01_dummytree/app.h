//---------------------------------------------------------------------------
// app.h - Implementation of the application.
//
// Authors: Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
// Copyright (c) 2003 GBDI-ICMC-USP
//---------------------------------------------------------------------------
#ifndef appH
#define appH

// Metric Tree includes
#include <arboretum/stMetricTree.h>
#include <arboretum/stPlainDiskPageManager.h>
#include <arboretum/stDiskPageManager.h>
#include <arboretum/stMemoryPageManager.h>
#include <arboretum/stDummyTree.h>
#include <arboretum/stMetricTree.h>

// My object
#include "city.h"

#include <string.h>
#include <fstream>

#define CITYFILE "BrazilianCities.txt"
#define QUERYCITYFILE "BrazilianCities500.txt"
//---------------------------------------------------------------------------
// class TApp
//---------------------------------------------------------------------------
class TApp{
   public:
      /**
      * This is the type used by the result.
      */
      typedef stResult < TCity > myResult;

      typedef stMetricTree < TCity, TCityDistanceEvaluator > MetricTree;

      /**
      * This is the type of the Slim-Tree defined by TCity and
      * TCityDistanceEvaluator.
      */
      typedef stDummyTree < TCity, TCityDistanceEvaluator > myDummyTree;

      /**
      * Creates a new instance of this class.
      */
      TApp(){
         PageManager = NULL;
         DummyTree = NULL;
      }//end TApp

      /**
      * Initializes the application.
      *
      * @param pageSize
      * @param minOccup
      * @param quantidade
      * @param prefix
      */
      void Init(){
         // To create it in disk
         CreateDiskPageManager();
         // Creates the tree
         CreateTree();
      }//end Init

      /**
      * Runs the application.
      *
      * @param DataPath
      * @param DataQueryPath
      */
      void Run();

      /**
      * Deinitialize the application.
      */
      void Done();

   private:

      /**
      * The Page Manager for Dummy.
      */
      stPlainDiskPageManager * PageManager;

      /**
      * The Dummy.
      */
      myDummyTree * DummyTree;

      /**
      * Vector for holding the query objects.
      */
      vector <TCity *> queryObjects;

      /**
      * Creates a disk page manager. It must be called before CreateTree().
      */
      void CreateDiskPageManager();

      /**
      * Creates a tree using the current PageManager.
      */
      void CreateTree();

      /**
      * Loads the tree from file with a set of cities.
      */
      void LoadTree(char * fileName);

      /**
      * Loads the vector for queries.
      */
      void LoadVectorFromFile(char * fileName);

      /**
      * Performs the queries and outputs its results.
      */
      void PerformQueries();

      void PerformNearestQuery();

      void PerformRangeQuery();

};//end TApp

#endif //end appH
