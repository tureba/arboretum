//---------------------------------------------------------------------------
// app.h - Implementation of the application.
//
// Author: Fabio Jun Takada Chino (chino@icmc.sc.usp.br)
//
// Copyright (c) 2002-2004 GBDI-ICMC-USP
//---------------------------------------------------------------------------
#ifndef appH
#define appH

// Metric Tree includes
#include <arboretum/stBasicObjects.h>
#include <arboretum/stBasicMetricEvaluators.h>
#include <arboretum/stMetricTree.h>
#include <arboretum/stMemoryPageManager.h>
#include <arboretum/stDiskPageManager.h>
#include <arboretum/stSlimTree.h>

#include <iostream>
using namespace std;

// Number of dimensions
#define DIM 5
// Number of Random objects
#define N 300

#define PAGESIZE (1024 * 4)

//---------------------------------------------------------------------------
// class myObject
//---------------------------------------------------------------------------
/**
* This class implements an array object.
*/
typedef stBasicArrayObject <double, int> myObject;

/**
* This is the type used by the result.
*/
typedef stResult < myObject > myResult;

/**
* Print operator.
*/
ostream & operator << (ostream & out, myObject & obj);

//---------------------------------------------------------------------------
// class TApp
//---------------------------------------------------------------------------
class TApp{
   public:
   	/**
   	* Evaluator type.
   	*/
   	typedef stBasicEuclideanMetricEvaluator <myObject> myEvaluator;

      /**
      * This is the type used by the result.
      */
      typedef stResult < myObject > myResult;

      /**
      * This is the type of the Slim-Tree defined by TCity and
      * TCityDistanceEvaluator.
      */
      typedef stSlimTree < myObject, myEvaluator > mySlimTree;

      /**
      * Creates a new instance of this class.
      */
      TApp(){
         PageManagerSlim = NULL;
         SlimTree = NULL;
      }//end TApp

      /**
      * Initializes the application.
      */
      void Init();

      /**
      * Runs the application.
      */
      void Run();

      /**
      * Deinitialize the application.
      */
      void Done();
    private :

      /**
      * The Page Manager.
      */
      stPageManager * PageManagerSlim;

      /**
      * The Slim Tree.
      */
      mySlimTree * SlimTree;

      /**
      * Creates a memory page manager. It must be called before CreateTree().
      */
      void CreateMemoryPageManager();

      /**
      * Creates a disk page manager. It must be called before CreateTree().
      */
      void CreateDiskPageManager();

      /**
      * Creates a tree using the current Page Manager. Call
      * CreateMemoryPageManager() or CreateDiskPageManager() before it to create
      * a PageManager.
      */
      void CreateTree();

      /**
      * Generates a ramdom object.
      *
      * @retval obj The destination object.
      */
      void RandomObject(myObject * obj);

      /**
      * Loads the tree with a set of cities.
      */
      void LoadTree();

      /**
      * Performs a query and outputs its results.
      */
      void PerformQuery();

      /**
      * Prints the result set to the standard output.
      *
      * @param result The result to be printed.
      */
      void PrintResults(myResult * result);
};//end TApp

#endif //end appH
