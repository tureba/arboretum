/**********************************************************************
* GBDI Arboretum - Copyright (c) 2002-2004 GBDI-ICMC-USP
*
*                           Homepage: http://gbdi.icmc.usp.br/arboretum
**********************************************************************/
/* ====================================================================
 * The GBDI-ICMC-USP Software License Version 1.0
 *
 * Copyright (c) 2004 Grupo de Bases de Dados e Imagens, Instituto de
 * Ciências Matemáticas e de Computação, University of São Paulo -
 * Brazil (the Databases and Image Group - Intitute of Matematical and 
 * Computer Sciences).  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by Grupo de Bases
 *        de Dados e Imagens, Instituto de Ciências Matemáticas e de
 *        Computação, University of São Paulo - Brazil (the Databases 
 *        and Image Group - Intitute of Matematical and Computer 
 *        Sciences)"
 *
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names of the research group, institute, university, authors
 *    and collaborators must not be used to endorse or promote products
 *    derived from this software without prior written permission.
 *
 * 5. The names of products derived from this software may not contain
 *    the name of research group, institute or university, without prior
 *    written permission of the authors of this software.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OF THIS SOFTWARE OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * ====================================================================
 *                                            http://gbdi.icmc.usp.br/
 */
//Implementation of stDummyTree.h

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stDummyTree<ObjectType, EvaluatorType>::stDummyTree(stPageManager * pageman):
      stMetricTree<ObjectType, EvaluatorType>(pageman){

   // Will I create or read it
   if (this->myPageManager->IsEmpty()){
      // Create it
      this->Create();
      // Default values
      Header->ObjectCount = 0;
      Header->NodeCount = 0;
      Header->MaxOccupation = 0;
   }else{
      // Use it
      this->LoadHeader();
   }//end if
}//end stDummyTree<ObjectType><EvaluatorType>::stDummyTree

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
bool stDummyTree<ObjectType, EvaluatorType>::Add(tObject * obj){
   // The object will be added in the first page.
   // When it is full, it will create a new page and link
   // it in the begining of the list.
   stPage * currPage;
   stDummyNode * currNode;
   bool overflow;
   stPageID next;
   int id;

   // Does it fit ?
   if (obj->GetSerializedSize() > this->myPageManager->GetMinimumPageSize() - 16){
      return false;
   }//end if

   // Adding object
   if (Header->Root == 0){
      overflow = true;
      next = 0;
   }else{
      // Get node
      currPage = this->myPageManager->GetPage(Header->Root);
      currNode = new stDummyNode(currPage);

      // Try to add
      id = currNode->AddEntry(obj->GetSerializedSize(), obj->Serialize());
      if (id >= 0){
         // I was able to add.
         this->myPageManager->WritePage(currPage);
         overflow = false;

         // update the maximum number of entries.
         this->SetMaxOccupation(currNode->GetNumberOfEntries());
      }else{
         // Oops! Overflow!
         overflow = true;
         next = currPage->GetPageID();
      }//end if

      // Clear the mess
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end if

   // Manage overflows
   if (overflow){
      // Oops! New root required !
      currPage = this->myPageManager->GetNewPage();
      currNode = new stDummyNode(currPage, true);
      currNode->SetNextNode(next);
      // Update the number of nodes.
      Header->NodeCount++;

      // I'll add it here
      id = currNode->AddEntry(obj->GetSerializedSize(), obj->Serialize());
      
      // Write the new node
      this->myPageManager->WritePage(currPage);

      // Update "tree" state
      Header->Root = currPage->GetPageID();
      WriteHeader();

      // Clear the mess again
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end overflow
   //Update the number of objects.
   UpdateObjectCounter(1);
   // Write Header!
   WriteHeader();

   return true;
}//end stDummyTree<ObjectType><EvaluatorType>::Add

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDummyTree<ObjectType, EvaluatorType>::RangeQuery(
                              tObject * sample, stDistance range){
   stPage * currPage;
   stDummyNode * currNode;
   tResult * result;
   tObject tmp;
   stDistance distance;
   stCount i;
   stPageID nextNode;

   // Create result
   result = new tResult();
   result->SetQueryInfo(sample->Clone(), tResult::RANGEQUERY, -1, range, false);

   // First node
   nextNode = Header->Root;

   // Let's search
   while (nextNode != 0){
      // Get node
      currPage = this->myPageManager->GetPage(nextNode);
      currNode = new stDummyNode(currPage);

      // Lets check all objects in this node
      for (i = 0; i < currNode->GetNumberOfEntries(); i++){
         // Rebuild the object
         tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));

         // Evaluate distance
         distance = this->myMetricEvaluator->GetDistance(&tmp, sample);

         // Is it qualified ?
         if (distance <= range){
            // Yes! I'm qualified !
            result->AddPair(tmp.Clone(), distance);
         }//end if
      }//end for

      // Next Node...
      nextNode = currNode->GetNextNode();

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end while

   return result;
}//end stDummyTree<ObjectType><EvaluatorType>::RangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDummyTree<ObjectType, EvaluatorType>::ReversedRangeQuery(
                              tObject * sample, stDistance range){
   stPage * currPage;
   stDummyNode * currNode;
   tResult * result;
   tObject tmp;
   stDistance distance;
   stCount i;
   stPageID nextNode;

   // Create result
   result = new tResult();
   result->SetQueryInfo(sample->Clone(), tResult::REVERSEDRANGEQUERY, -1, range, false);

   // First node
   nextNode = Header->Root;

   // Let's search
   while (nextNode != 0){
      // Get node
      currPage = this->myPageManager->GetPage(nextNode);
      currNode = new stDummyNode(currPage);

      // Lets check all objects in this node
      for (i = 0; i < currNode->GetNumberOfEntries(); i++){
         // Rebuild the object
         tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));

         // Evaluate distance
         distance = this->myMetricEvaluator->GetDistance(&tmp, sample);

         // Is it qualified ?
         if (distance >= range){
            // Yes! I'm qualified !
            result->AddPair(tmp.Clone(), distance);
         }//end if
      }//end for

      // Next Node...
      nextNode = currNode->GetNextNode();

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end while

   return result;
}//end stDummyTree<ObjectType><EvaluatorType>::ReversedRangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDummyTree<ObjectType, EvaluatorType>::NearestQuery(
                     tObject * sample, stCount k, bool tie){
   stPage * currPage;
   stDummyNode * currNode;
   tResult * result;
   tObject tmp;
   stDistance distance;
   stCount i;
   stPageID nextNode;

   // Create result
   result = new tResult(k);
   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, -1.0, tie);

   // First node
   nextNode = Header->Root;

   // Let's search
   while (nextNode != 0){
      // Get node
      currPage = this->myPageManager->GetPage(nextNode);
      currNode = new stDummyNode(currPage);

      // Lets check all objects in this node
      for (i = 0; i < currNode->GetNumberOfEntries(); i++){
         // Rebuild the object
         tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));

         // Evaluate distance
         distance = this->myMetricEvaluator->GetDistance(&tmp, sample);

         // Is it qualified ?
         if (result->GetNumOfEntries() < k){
            // Unnecessary to check. Just add.
            result->AddPair(tmp.Clone(), distance);
         }else{
            // Will I add ?
            if (distance <= result->GetMaximumDistance()){
               // Yes! I'll.
               result->AddPair(tmp.Clone(), distance);
               result->Cut(k);
            }//end if
         }//end if
      }//end for

      // Next Node...
      nextNode = currNode->GetNextNode();

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end while

   return result;
}//end NearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDummyTree<ObjectType, EvaluatorType>::FarthestQuery(
                     tObject * sample, stCount k, bool tie){
   stPage * currPage;
   stDummyNode * currNode;
   tResult * result;
   tObject tmp;
   stDistance distance;
   stCount i;
   stPageID nextNode;

   // Create result
   result = new tResult(k);
   result->SetQueryInfo(sample->Clone(), tResult::KFARTHESTQUERY, k, -1.0, tie);

   // First node
   nextNode = Header->Root;

   // Let's search
   while (nextNode != 0){
      // Get node
      currPage = this->myPageManager->GetPage(nextNode);
      currNode = new stDummyNode(currPage);

      // Lets check all objects in this node
      for (i = 0; i < currNode->GetNumberOfEntries(); i++){
         // Rebuild the object
         tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));

         // Evaluate distance
         distance = this->myMetricEvaluator->GetDistance(&tmp, sample);

         // Is it qualified ?
         if (result->GetNumOfEntries() < k){
            // Unnecessary to check. Just add.
            result->AddPair(tmp.Clone(), distance);
         }else{
            // Will I add ?
            if (distance >= result->GetMinimumDistance()){
               // Yes! I'll.
               result->AddPair(tmp.Clone(), distance);
               result->CutFirst(k);
            }//end if
         }//end if
      }//end for

      // Next Node...
      nextNode = currNode->GetNextNode();

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end while

   return result;
}//end FarthestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDummyTree<ObjectType, EvaluatorType>::KAndRangeQuery(
      tObject * sample, stDistance range, stCount k, bool tie){
   stPage * currPage;
   stDummyNode * currNode;
   tResult * result;
   tObject tmp;
   stDistance distance;
   stCount i;
   stPageID nextNode;

   // Create result
   result = new tResult(k);
   result->SetQueryInfo(sample->Clone(), tResult::KANDRANGEQUERY, k, range, tie);

   // First node
   nextNode = Header->Root;

   // Let's search
   while (nextNode != 0){
      // Get node
      currPage = this->myPageManager->GetPage(nextNode);
      currNode = new stDummyNode(currPage);

      // Lets check all objects in this node
      for (i = 0; i < currNode->GetNumberOfEntries(); i++){
         // Rebuild the object
         tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));

         // Evaluate distance
         distance = this->myMetricEvaluator->GetDistance(&tmp, sample);

         // Is it qualified ?
         if (distance <= range){
            // Yes! I'm qualified !
            if (result->GetNumOfEntries() < k){
               // Has less than k.
               result->AddPair(tmp.Clone(), distance);
            }else{
               // May I add ?
               if (distance <= result->GetMaximumDistance()){
                  // Yes! I'll add it and cut the results if necessary
                  result->AddPair(tmp.Clone(), distance);
                  result->Cut(k);
               }//end if
            }//end if
         }//end if
      }//end for

      // Next Node...
      nextNode = currNode->GetNextNode();

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end while

   return result;
}//end KAndRangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDummyTree<ObjectType, EvaluatorType>::KOrRangeQuery(
            tObject * sample, stDistance range, stCount k, bool tie){
   stPage * currPage;
   stDummyNode * currNode;
   tResult * result;
   tObject tmp;
   stDistance distance, dk=MAXDOUBLE;
   stCount i;
   stPageID nextNode;

   // Create result
   result = new tResult();
   result->SetQueryInfo(sample->Clone(), tResult::KORRANGEQUERY, k, range, tie);

   // First node
   nextNode = Header->Root;

   // Let's search
   while (nextNode != 0){
      // Get node
      currPage = this->myPageManager->GetPage(nextNode);
      currNode = new stDummyNode(currPage);

      // Lets check all objects in this node
      for (i = 0; i < currNode->GetNumberOfEntries(); i++){
         // Rebuild the object
         tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));

         // Evaluate distance
         distance = this->myMetricEvaluator->GetDistance(&tmp, sample);

         // KorRange part
         if (distance <= dk){
           result->AddPair(tmp.Clone(), distance);
           // Otimization
           //if (dk == range) then there is nothing to do!!
           // dk will never be smaller than range
           if (dk > range){//Nearest > Range then to cut the result is possible

             if (result->GetNumOfEntries() >= k){ //

               result->Cut(k); // This depends on tie list too

               if (result->GetMaximumDistance() <= range)  //Range > Nearest
                 dk = range; //Query radius is range
               else
                 dk = result->GetMaximumDistance(); //Query radius is the farthest object (last nearest)
             } // end if result->getNumOfEntries...

           } // end if  dk > range

         }// end if distance...

      }//end for

      // Next Node...
      nextNode = currNode->GetNextNode();

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end while

   return result;
}//end KOrRangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDummyTree<ObjectType, EvaluatorType>::RingQuery(
            tObject * sample, stDistance inRange, stDistance outRange){
   stPage * currPage;
   stDummyNode * currNode;
   tResult * result;
   tObject tmp;
   stDistance distance;
   stCount i;
   stPageID nextNode;

   // Create result
   result = new tResult();
   result->SetQueryInfo(sample->Clone(), tResult::RINGQUERY, -1, outRange, inRange);

   // First node
   nextNode = Header->Root;

   // Let's search
   while (nextNode != 0){
      // Get node
      currPage = this->myPageManager->GetPage(nextNode);
      currNode = new stDummyNode(currPage);

      // Lets check all objects in this node
      for (i = 0; i < currNode->GetNumberOfEntries(); i++){
         // Rebuild the object
         tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));

         // Evaluate distance
         distance = this->myMetricEvaluator->GetDistance(&tmp, sample);

         // Is it qualified ?
         if ((distance <= outRange) && (distance > inRange)){
            // Yes! I'm qualified !
            result->AddPair(tmp.Clone(), distance);
         }//end if
      }//end for

      // Next Node...
      nextNode = currNode->GetNextNode();

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end while

   return result;
}//end RingQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDummyTree<ObjectType, EvaluatorType>::SumNearestQuery(
                              tObject ** samplelist, stSize samplesize, stCount k, bool tie){

   stPage * currPage;
   stDummyNode * currNode;
   tObject tmp;
   stDistance distance;
   stCount i;
   stPageID nextNode;

   // Create result
   tResult *result = new tResult();
   result->SetQueryInfo(samplelist[0]->Clone(), tResult::KNEARESTQUERY, k, -1.0, tie);

   // First node
   nextNode = Header->Root;

   // Let's search
   while (nextNode != 0){
      // Get node
      currPage = this->myPageManager->GetPage(nextNode);
      currNode = new stDummyNode(currPage);

      // Lets check all objects in this node
      for (i = 0; i < currNode->GetNumberOfEntries(); i++){
         // Rebuild the object
         tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));

         // For each sample object
         distance = 0;
         for (int samp = 0; samp < samplesize; samp++) {
            // Evaluate distance
            distance += this->myMetricEvaluator->GetDistance(&tmp, samplelist[samp]);
         }//end for

         // Is it qualified ?
         if (result->GetNumOfEntries() < k){
            // Unnecessary to check. Just add.
            result->AddPair(tmp.Clone(), distance);
         }else{
            // Will I add ?
            if (distance <= result->GetMaximumDistance()){
               // Yes! I'll.
               result->AddPair(tmp.Clone(), distance);
               result->Cut(k);
            }//end if
         }//end if
      }//end for

      // Next Node...
      nextNode = currNode->GetNextNode();

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end while

   return result;

}//end SumNearestQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDummyTree<ObjectType, EvaluatorType>::SumRangeQuery(
                              tObject ** samplelist, stSize samplesize, stDistance range){
   stPage * currPage;
   stDummyNode * currNode;
   tObject tmp;
   stDistance distance;
   stCount i;
   stPageID nextNode;

   // Create result
   tResult *result = new tResult();
   result->SetQueryInfo(samplelist[0]->Clone(), tResult::RANGEQUERY, -1, range, false);

   // First node
   nextNode = Header->Root;

   // Let's search
   while (nextNode != 0){
      // Get node
      currPage = this->myPageManager->GetPage(nextNode);
      currNode = new stDummyNode(currPage);

      // Lets check all objects in this node
      for (i = 0; i < currNode->GetNumberOfEntries(); i++){
         // Rebuild the object
         tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));

         // For each sample object
         distance = 0;
         for (int samp = 0; samp < samplesize; samp++) {
            // Evaluate distance
            distance += this->myMetricEvaluator->GetDistance(&tmp, samplelist[samp]);
         }//end for

         // Is it qualified ?
         if (distance <= range){
            // Yes! I'm qualified !
            result->AddPair(tmp.Clone(), distance);
         }//end if
      }//end for

      // Next Node...
      nextNode = currNode->GetNextNode();

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end while

   return result;

}//end SumRangeQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDummyTree<ObjectType, EvaluatorType>::NearestJoinQuery(
                              stDummyTree * joinedtree, stCount k, bool tie){
   stPage * currPage;
   stPage * currPageJoin;
   stDummyNode * currNode;
   stDummyNode * currNodeJoin;
   stPageID nextNode;
   stPageID nextNodeJoin;
   tResult * globalResult;
   tResult ** localResult;
   tObject tmp;
   tObject tmpJoin;
   stDistance distance;
   stCount i, j;

   // Create result
   globalResult = new tResult();
   globalResult->SetQueryInfo(NULL, tResult::KNEARESTQUERY, k, -1.0, tie);

   // First node
   nextNode = Header->Root;

   // Let's search
   while (nextNode != 0){
      // Get node
      currPage = this->myPageManager->GetPage(nextNode);
      currNode = new stDummyNode(currPage);

      nextNodeJoin = joinedtree->Header->Root;

      localResult = new tResult*[currNode->GetNumberOfEntries()];
      for (i = 0; i < currNode->GetNumberOfEntries(); i++){
         localResult[i] = new tResult();
         localResult[i]->SetQueryInfo(NULL, tResult::KNEARESTQUERY, k, -1.0, tie);
      }//end for

      // Let's search the joined tree
      while (nextNodeJoin != 0){
         // Get node
         currPageJoin = joinedtree->myPageManager->GetPage(nextNodeJoin);
         currNodeJoin = new stDummyNode(currPageJoin);

         // Lets check all objects in this node
         for (i = 0; i < currNode->GetNumberOfEntries(); i++){

            // Rebuild the object
            tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));

            for (j = 0; j < currNodeJoin->GetNumberOfEntries(); j++){

               // Rebuild the object
               tmpJoin.Unserialize(currNodeJoin->GetObject(j), currNodeJoin->GetObjectSize(j));

               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmp, &tmpJoin);

               // Is it qualified ?
               if (localResult[i]->GetNumOfEntries() < k){
                  // Unnecessary to check. Just add.
                  localResult[i]->AddJoinedPair(tmp.Clone(), tmpJoin.Clone(), distance);
               }else{
                  // Will I add ?
                  if (distance <= localResult[i]->GetMaximumDistance()){
                     // Yes! I'll.
                     localResult[i]->AddJoinedPair(tmp.Clone(), tmpJoin.Clone(), distance);
                     localResult[i]->Cut(k);
                  }//end if
               }//end if
            }//end for
         }//end for

         // Next Node...
         nextNodeJoin = currNodeJoin->GetNextNode();

         // Free it all
         delete currNodeJoin;
         joinedtree->myPageManager->ReleasePage(currPageJoin);

      }//end while

      // Copy the localResult objects to globalResult
      for (i = 0; i < currNode->GetNumberOfEntries(); i++){
         for (int k = 0; k < localResult[i]->GetNumOfEntries(); k++) {
            globalResult->AddJoinedPair((*localResult[i])[k].GetObject()->Clone(),
                                        (*localResult[i])[k].GetJoinedObject()->Clone(),
                                        (*localResult[i])[k].GetDistance());
         }//end for

         // Cleanning
         delete localResult[i];
      }//end for
      delete[]localResult;

      // Next Node...
      nextNode = currNode->GetNextNode();

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end while

   // Returning the result of range join.
   return globalResult;

}//end NearestJoinQuery

//------------------------------------------------------------------------------
template <class ObjectType, class EvaluatorType>
stResult<ObjectType> * stDummyTree<ObjectType, EvaluatorType>::RangeJoinQuery(
                              stDummyTree * joinedtree, stDistance range){

   stPage * currPage;
   stPage * currPageJoin;
   stDummyNode * currNode;
   stDummyNode * currNodeJoin;
   stPageID nextNode;
   stPageID nextNodeJoin;
   tResult * globalResult, * localResult;
   tObject tmp;
   tObject tmpJoin;
   stDistance distance;
   stCount i, j;

   // Create result
   globalResult = new tResult();
   globalResult->SetQueryInfo(NULL, tResult::RANGEQUERY, -1, range, false);

   // First node
   nextNode = Header->Root;

   // Let's search
   while (nextNode != 0){
      // Get node
      currPage = this->myPageManager->GetPage(nextNode);
      currNode = new stDummyNode(currPage);

      nextNodeJoin = joinedtree->Header->Root;

      // Let's search the joined tree
      while (nextNodeJoin != 0){
         // Get node
         currPageJoin = joinedtree->myPageManager->GetPage(nextNodeJoin);
         currNodeJoin = new stDummyNode(currPageJoin);

         // Lets check all objects in this node
         for (i = 0; i < currNode->GetNumberOfEntries(); i++){

            localResult = new tResult();
            localResult->SetQueryInfo(NULL, tResult::RANGEQUERY, -1, range, false);

            // Rebuild the object
            tmp.Unserialize(currNode->GetObject(i), currNode->GetObjectSize(i));

            for (j = 0; j < currNodeJoin->GetNumberOfEntries(); j++){

               // Rebuild the object
               tmpJoin.Unserialize(currNodeJoin->GetObject(j), currNodeJoin->GetObjectSize(j));

               // Evaluate distance
               distance = this->myMetricEvaluator->GetDistance(&tmp, &tmpJoin);

               // Is it qualified ?
               if (distance <= range){
                  // Yes! I'm qualified !
                  localResult->AddJoinedPair(tmp.Clone(), tmpJoin.Clone(), distance);
               }//end if
            }//end for

            // Copy the localResult objects to globalResult
            for (int k = 0; k < localResult->GetNumOfEntries(); k++) {
               globalResult->AddJoinedPair((*localResult)[k].GetObject()->Clone(),
                                           (*localResult)[k].GetJoinedObject()->Clone(),
                                           (*localResult)[k].GetDistance());
            }//end for

            // Cleanning.
            delete localResult;
         }//end for

         // Next Node...
         nextNodeJoin = currNodeJoin->GetNextNode();

         // Free it all
         delete currNodeJoin;
         joinedtree->myPageManager->ReleasePage(currPageJoin);
      }//end while

      // Next Node...
      nextNode = currNode->GetNextNode();

      // Free it all
      delete currNode;
      this->myPageManager->ReleasePage(currPage);
   }//end while

   // Returning the result of range join.
   return globalResult;

}//end RangeJoinQuery

