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
/**
* @file
*
* This file defines the classes stResult and stResults.
*
* @version 1.0
* $Revision: 1.29 $
* $Date: 2005/03/08 19:43:09 $
* $Author: marcos $
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
*/
// Copyright (c) 2002/2003 GBDI-ICMC-USP
#ifndef __STRESULT_H
#define __STRESULT_H

#include <vector>
#include <arboretum/stTypes.h>

#ifdef __BORLANDC__
   using namespace std;
#endif //__BORLANDC__

//----------------------------------------------------------------------------
// Class template stResultPair
//----------------------------------------------------------------------------
/**
* This class defines the pair Object/Distance returned as the result of a query.
* It is used by class template stResult to store pairs of results and their
* associated distance from the center of the query. Do not create instances of
* this class template directly. 
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @version 1.0
* @ingroup struct
* @see stResult
*/
template < class ObjectType >
class stResultPair{
   public:
      /**
      * Type of the object.
      */
      typedef ObjectType tObject;

      /**
      * Creates a new pair Object/Distance.
      *
      * <P>This instance will claim the ownership of the object. In other words,
      * it will dispose the object when it is no loger necessary.
      *
      * @param obj The result object.
      * @param distance The distance from the sample.
      */
      stResultPair(tObject * obj, stDistance distance){

         this->Object = obj;
         this->JoinedObject = NULL;
         this->Distance = distance;
      }//end stResultPair

      /**
      * Creates a new pair Object/Distance with a JoinedObject.
      *
      * <P>This instance will claim the ownership of the object. In other words,
      * it will dispose the object when it is no loger necessary.
      *
      * @param obj The result object.
      * @param joined obj The result joined object.
      * @param distance The distance from the sample.
      */
      stResultPair(tObject * obj, tObject * joinedobj, stDistance distance){

         this->Object = obj;
         this->JoinedObject = joinedobj;
         this->Distance = distance;
      }//end stResultPair

      /**
      * Disposes this instance and release all associated resources including
      * the result object.
      */
      ~stResultPair(){

         if (Object != NULL){
            delete Object;
         }//end if
         if (JoinedObject != NULL){
            delete JoinedObject;
         }
      }//end ~stResultPair

      /**
      * This method returns the object.
      */
      const tObject * GetObject(){
         return Object;
      }//end GetObject

      /**
      * This method returns the object.
      */
      const tObject * GetJoinedObject(){
         return JoinedObject;
      }//end GetObject

      /**
      * This method gets the distance.
      */
      stDistance GetDistance(){
         return Distance;
      }//end GetDistance
   private:
      /**
      * The object.
      */
      tObject * Object;

      /**
      * The joined object.
      */
      tObject * JoinedObject;

      /**
      * The distance from the query object.
      */
      stDistance Distance;
};//end stResultPair

//----------------------------------------------------------------------------
// Class template stResult
//----------------------------------------------------------------------------
/**
* This class implements a query result. It will hold a set of pairs
* Object/Distance ordered by distance which constitutes the answer
* to a query. All query methods of all metric trees implemented by
* this library will return instances of this class.
*
* <P>In nearest neigbour queries and k-range queries, the result set may contain
* more than k results. It means that the greatest distance from sample to result
* has more than 1 object. In such cases, all objects whose distance from sample
* is equal to GetMaximumDistance() constitute the draw list.
*
* <P>As an extra capability, it can store information about the query but they
* will only be available if the query method supports this feature (this is an
* optional capability). See SetQueryInfo() for more details.
*
* <P> It also performs basic operations that allows the construction of
* the result set by the query procedures.
*
* @author Fabio Jun Takada Chino (chino@icmc.usp.br)
* @author Marcos Rodrigues Vieira (mrvieira@icmc.usp.br)
* @author Adriano Siqueira Arantes (arantes@icmc.usp.br)
* @version 1.1
* @ingroup struct
*/
template < class ObjectType >
class stResult{
   public:
      /**
      * Define constants to query type.
      */
      enum tQueryType{
         /**
         * Indicates that there is no information about the query.
         */
         UNKNOWN = 0,

         /**
         * This is a result of a range query.
         */
         RANGEQUERY = 1,

         /**
         * This is a result of a k nearest neighbour query.
         */
         KNEARESTQUERY = 2,

         /**
         * This is a result of a estimated k nearest neighbour query.
         */
         ESTIMATEKNEARESTQUERY = 3,

         /**
         * This is a result of a ring query.
         */
         RINGQUERY = 4,

         /**
         * This is a result of a k ring query.
         */
         KRINGQUERY = 5,

         /**
         * This is a result of a k AND range query.
         */
         KANDRANGEQUERY = 6,

         /**
         * This is a result of a k OR range query.
         */
         KORRANGEQUERY = 7,

         /**
         * This is a result of a Point query.
         */
         POINTQUERY = 8,

         /**
         * This is a result of a Reverse of Range query.
         */
         REVERSEDRANGEQUERY = 9,

         /**
         * This is a result of a K-Farthest query.
         */
         KFARTHESTQUERY = 10
      };//end tQueryType

      /**
      * Type of the object.
      */
      typedef ObjectType tObject;

      /**
      * This type defines the stResult Pair used by this class.
      */
      typedef stResultPair< ObjectType > tPair;

      /**
      * This method will create a new instance of this class. The parameter hint
      * is used to prepare this instance to hold at least <i>hint</i> results
      * (it is not a upper bound limit).
      *
      * @param hint The projected number of results (default = 1).
      */
      stResult(unsigned int hint = 1){
         // Reserve results
         Pairs.reserve(hint);
         // No info
         SetQueryInfo();
      }//end stResult

      /**
      * This method disposes this instance and releases all allocated resources.
      */
      virtual ~stResult();

      /**
      * This operator allow the access to a pair.
      */
      tPair & operator [] (unsigned int idx){
         return (*Pairs[idx]);
      }//end operator []

      /**
      * This method returns the number of entries in this result.
      */
      unsigned int GetNumOfEntries(){
         return Pairs.size();
      }//end GetNumOfEntries

      /**
      * This method adds a pair Object/Distance to this result list.
      *
      * @param obj The object.
      * @param distance The distance from the sample object.
      * @warning There is no duplicate pair checking. All pairs will be added.
      */
      void AddPair(tObject * obj, stDistance distance){
         unsigned int pos;

         pos = this->Find(distance);
         Pairs.insert(Pairs.begin() + pos, new tPair(obj, distance));
      }//end AddPair

      /**
      * This method adds a joined pair Object/JoinedObject/distance
      * to this result list.
      *
      * @param obj The object.
      * @param joinedObj The joined object.
      * @param distance The distance from the sample object.
      * @warning There is no duplicate pair checking. All pairs will be added.
      */
      void AddJoinedPair(tObject * obj, tObject * joinedObj, stDistance distance){
         unsigned int pos;

         pos = this->Find(distance);
         Pairs.insert(Pairs.begin() + pos, new tPair(obj, joinedObj, distance));
      }//end AddJoinedPair

      /**
      * This method will remove the last object from this result list.
      */
      void RemoveLast(){

         if (Pairs.size() > 0){
            if (Pairs[Pairs.size() - 1] != NULL){
               delete Pairs[Pairs.size() - 1];
            }//end if
            Pairs.pop_back();
         }//end if
      }//end RemoveLast

      /**
      * This method will remove the first object from this result list.
      */
      void RemoveFirst(){

         if (Pairs.size() > 0){
            if (Pairs[0] != NULL){
               delete Pairs[0];
            }//end if
            Pairs.erase(Pairs.begin());
         }//end if
      }//end RemoveFirst

      /**
      * This method returns the minimum distance of the objects in this result
      * list. If this result is empty, it will return a negative value.
      */
      stDistance GetMinimumDistance(){

         if (Pairs.size() > 0){
            return Pairs[0]->GetDistance();
         }else{
            return -1;
         }//end if
      }//end GetMinimumDistance

      /**
      * This method returns the maximum distance of the objects in this result
      * list. If this result is empty, it will return a negative value.
      */
      stDistance GetMaximumDistance(){

         if (Pairs.size() > 0){
            return Pairs[Pairs.size() - 1]->GetDistance();
         }else{
            return -1;
         }//end if
      }//end GetMaximumDistance

      /**
      * This method will cut out undesired objects. It may be used
      * by k-Nearest Neighbour queries to control the number of results.
      *
      * <P>This implementation also treat...
      *
      * @param limit The desired number of results.
      * @todo Review of this explanation.
      */
      void Cut(unsigned int limit);

      /**
      * This method will cut out undesired objects. It may be used
      * by k-Farthest Neighbour queries to control the number of results.
      *
      * <P>This implementation also treat...
      *
      * @param limit The desired number of results.
      * @todo Review of this explanation.
      */
      void CutFirst(unsigned int limit);

      /**
      * Adds information about the query. It is used by Query methods to add
      * information about the query. Since it is optional, not all results will
      * provide meaningful information about it.
      *
      * @param sample The sample object (a copy of it).
      * @param querytype The query type (UNKNOWN, RANGEQUERY, NEARESTQUERY,
      * KANDRANGEQUERY, KORRANGEQUERY, CROWNQUERY, KANDRANGEQUERYESTIMATE or
      * KORRANGEQUERYESTIMATE)
      * @param k The value of k (if it makes sence).
      * @param radius The value of radius (if it makes sence).
      * @param innerRadius The value of inner radius (if it makes sence).
      * @param tie The tie list. Default false;
      *
      * @warning It do not changes the behavior of this result.
      * @see GetQueryType()
      * @see GetK()
      * @see GetRadius()
      * @see GetSample()
      * @see GetTie()
      */
      void SetQueryInfo(tObject * sample = NULL, int querytype = UNKNOWN,
                        stCount k = 0, stDistance radius = 0.0,
                        stDistance innerRadius = 0.0, bool tie = false){
         QueryType = querytype;
         K = k;
         Radius = radius;
         InnerRadius = innerRadius;
         Sample = sample;
         Tie = tie;
      }//end SetQueryInfo

      /**
      * Adds information about the query. It is used by Query methods to add
      * information about the query. Since it is optional, not all results will
      * provide meaningful information about it.
      *
      * @param sample The sample object (a copy of it).
      * @param querytype The query type (UNKNOWN, RANGEQUERY, KRANGEQUERY or
      * KNEARESTQUERY)
      * @param k The value of k (if it makes sence).
      * @param radius The value of radius (if it makes sence).
      * @param tie The tie list. Default false;
      *
      * @warning It do not changes the behavior of this result.
      * @see GetQueryType()
      * @see GetK()
      * @see GetRadius()
      * @see GetSample()
      * @see GetTie()
      */
      void SetQueryInfo(tObject * sample, int querytype,
                        stCount k, stDistance radius, bool tie){
         this->SetQueryInfo(sample, querytype, k, radius, 0.0, tie);
      }//end SetQueryInfo

      /**
      * Gets the information about query type. I may assume the values
      * UNKNOWN, RANGEQUERY, KANDRANGEQUERY, KORRANGEQUERY or KNEARESTQUERY.
      */
      int GetQueryType(){
         return QueryType;
      }//end GetQueryType

      /**
      * Gets information about k. It makes sense only for KANDRANGEQUERY, KORRANGEQUERY
      * and KNEARESTQUERY.
      */
      unsigned int GetK(){
         return K;
      }//end GetK

      /**
      * Gets information about radius. It makes sense only for RANGEQUERY and
      * KRANGEQUERY.
      */
      stDistance GetRadius(){
         return Radius;
      }//end GetRadius

      /**
      * Gets information about inner radius. It makes sense only for CROWNQUERY.
      */
      stDistance GetInnerRadius(){
         return InnerRadius;
      }//end GetRadius

      /**
      * Gets the sample object if it is available. Since it is an optional
      * information it may not be available.
      *
      * @return The sample object or NULL if it is not available.
      */
      tObject * GetSample(){
         return Sample;
      }//end GetSample

      /**
      * Gets the tie list to the query. It is an optional
      * information.
      *
      * @return True if there is tie list or False otherwise.
      */
      bool GetTie(){
         return Tie;
      }//end GetTie

      /**
      * This method tests if two results are equal.
      *
      * @param r1 The second result to be test.
      * @return True if is equal, False otherwise.
      */
      bool IsEqual(const stResult * r1);

      /**
      * This method tests the similarity between two results .
      *
      * @param r1 The second result to be test.
      * @return the percent-similarity of the two results.
      */
      double Precision(const stResult * r1);


      /**
      * This method implements the intersection operator between two results.
      * @param r1 The first result set.
      * @param r2 The second result set.
      */
      void Intersection(stResult * result1, stResult * result2);

      /**
      * This method implements the union operator between two results.
      * @param r1 The first result set.
      * @param r2 The second result set.
      */
      void Union(stResult * result1, stResult * result2);

   private:
      /**
      * The vector of pairs.
      */
      vector < tPair * > Pairs;

      /**
      * Information about QueryType (Optional).
      */
      int QueryType;

      /**
      * Information about k for KANDRANGEQUERY, KORRANGEQUERY and
      * KNEARESTQUERY (Optional).
      */
      unsigned int K;

      /**
      * Information about tie list for KANDRANGEQUERY, KORRANGEQUERY
      * and KNEARESTQUERY .
      */
      bool Tie;

      /**
      * Information about radius for KANDRANGEQUERY, KORRANGEQUERY and
      * RANGEQUERY (Optional).
      */
      stDistance Radius;

      /**
      * Information about inner radius for CROWNQUERY (Optional).
      */
      stDistance InnerRadius;

      /**
      * Sample object (Optional).
      */
      tObject * Sample;

      /**
      * This method locates the insertion position of an object.
      *
      * @param distance The desired distance.
      * @todo This code needs optimizations. I suggest a binary search
      * implementation.
      */
      unsigned int Find(stDistance distance);
      
};//end stResult

#include <arboretum/stResult.cc>

#endif //__STRESULT_H
