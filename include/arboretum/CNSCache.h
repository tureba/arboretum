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
 
//==============================================================================
//  Project: CAMF - Complex objects Access Method Framework
//  Arquivo: CNSCache.h
//  Autor: Roberto Figueira Santos Filho
//  Orientador: Caetano Traina Junior
//  Data Inicial: 10/06/1997
//  Versao Atual: 0.0
//
//  Descricao:
//   Este arquivo classe CNSCache - Natural Selection Cache Class.
//    A documentacao precisa ser refeita.
//
//    Importante -> se o objeto da classe continente criar um novo registro,
//  o metodo GMC_NovoNRgs(unsigned long reg, void **) deve ser chamado para que
//  sejam atualizados os controles do cache e tornar a pagina deste registro
//  ativa. Este metodo so deve ser chamado apos a criacao do registro fisico na
//  base, mesmo que em branco, e terem sido feitas as alteracoes na mesma para
//  refletir sua existencia. Fica por conta do usuario qualquer erro de
//  concistencia.
//
// Modificacoes:
//    Author: Roberto Figueira Santos Filho
//    Date: 06/01/2001
//    Description:
//      - Translate the name of public methods to English;
//      - The old version forces user to allocate the cache object at execution
//      time, which means the needing of a reference to the cache. I changed the
//      constructor to solve this. Because of that, prior to used a cache, the
//      method MountCache must be exectuted.
//      - Put all the class definition inside the header file. I don't like this
//      approach, but it seems a requirement for class templates.
//
//==============================================================================

#ifndef CNSCACHEH
#define CNSCACHEH

#include <arboretum/stCommon.h>
#include <stdio.h>
#include <stdlib.h>

//#define CNSCACHE_CHECK

//---------------- DEFINICAO DA CLASSE CNSCache -----------------

template <class TpObjCntin>
class CNSCache {
   public:
      //---- Prototipos ----
      CNSCache();
      ~CNSCache();
      void MountCache(TpObjCntin *father, unsigned long NRegs, unsigned char FRot,
             unsigned int NPgs, unsigned long TamBuff);
      void ReadPage(unsigned long, void **);
      void WritePage() {
      ListaPgs.Pagina[ListaPgs.PgEmUso].Escrita = 1;
      }// fim do metodo writePage
      void NewPage(unsigned long, void **);
      void FlushCache();

   private:
      #define TxCrescBlocos 51
      #define CNSCACHE_NIL 0xFFFFU
      //---- Tipos ----
      //-- estrutura para mapeamento de enderecos de registros
      //   para paginas em memoria
      typedef struct {
         unsigned int *pPagina; // vetor com RGS_POR_BLOCO posicoes
         unsigned char RgsUsados;
      } tBloco;

      typedef struct { // tabela de mapeamento de enderecos
         unsigned long TotBlocos;
         unsigned int *pBloco; // vetor com TotBlocos posicoes
         tBloco *Bloco; // vetor com N_PAGINAS posicoes
         unsigned int LBlocoDispo;
      } tTME;

      //-- estrutura para manter a lista de paginas do cache
      typedef struct {
         unsigned int proxDaFila;
         unsigned int antDaFila;
         unsigned int IdxTotAcessos;
         unsigned long IdRegFisico;
         unsigned char  Escrita;
         unsigned char *Page;
      } tPagina;

      typedef struct {
         tPagina *Pagina; // vetor com N_PAGINAS posicoes
         unsigned int LPgDispo;
         unsigned int PgEmUso;
      } tListaPgs;

      //-- estrutura para manter o Gerenciador de Acessos aas paginas do Cache
      typedef struct {
         unsigned int TotAcessos; // total de acessos
         unsigned int prox; // proximo no da lista de totais de acessos
         unsigned int ant; // no anterior da lista de totais de acessos
         unsigned int frontFilaPg; // aponta para a pg da fila de pgs referenciada
                     // ha mais tempo. As paginas sao retiradas daqui.
         unsigned int rearFilaPg; // aponta para a pg da fila de pgs referenciada
                    // ha menos tempo. Novas paginas sao inseridas aqui.
      } tLTotAcessos;

      typedef struct {
         tLTotAcessos *LTotAcessos; // vetor com os totais de acessos, possui
                              // N_PAGINAS posicoes
         unsigned int Cabeca; // aponta para o de menor total - primeiro da lista
         unsigned int LDispo;
      } tGerAcessos;

      //---- Variaveis ----
      unsigned char F_ROTACAO;
      unsigned char RGS_POR_BLOCO;
      unsigned int N_PAGINAS;

      tTME TME;
      tListaPgs ListaPgs;
      tGerAcessos GerAcessos;
      TpObjCntin *ObjContinente; // objeto que contem o cache
      //---- Prototipos ----
      void IncPagina(); // incrementa o numero de acessos da pagina corrente
      unsigned int ObtemPagina();
      void TiraPgFila(unsigned int, unsigned int);
      void ColocaPgFila(unsigned int, unsigned int);
      void NovoTotal(unsigned int, unsigned int &);
      void UnMountCache();
      #ifdef __CACHECHECK
      long checkCounter; // deve ser usado para identificar o momento em que o
                         // erro ocorreu. Ao inves de ficar percorrendo o cache
                         // ate o erro (e isso pode implicar em MUITOS acessos),
                         // este contador eh incrementado e impresso quando um
                         // um erro na estrutura eh encontrado. Dai, basta o
                         // programador gerar um break quando o contador chegar
                         // no valor do erro novamente.
      void checkStructure(); // checa a estrutura do cache em busca de erros
                             // util para debugar
      #endif
};//end class CNSCache

//---------------------------------------------------------------------------
// Declaracao dos metodos da classe CNSCache
//---------------------------------------------------------------------------

//==============================================================================
// Metodo: CNSCache() - construtor. Sem efeito.
//==============================================================================
template <class TpObjCntin>
CNSCache<TpObjCntin>::CNSCache() {
   ObjContinente = NULL; // indica que nenhum cache foi criado ainda
}//end CNSCache<TpObjCntin>::CNSCache

//==============================================================================
// Metodo: ~CNSCache()
//==============================================================================
template <class TpObjCntin>
CNSCache<TpObjCntin>::~CNSCache() {
   UnMountCache();
}//end CNSCache<TpObjCntin>::~CNSCache

//==============================================================================
// Metodo: MountCache(tFileMngr *father, unsigned long NRegs, unsigned char
//           FRot, unsigned int NPgs, unsigned long TamBuff)
//==============================================================================
template <class TpObjCntin>
void CNSCache<TpObjCntin>::MountCache(TpObjCntin *father, unsigned long NRegs,
           unsigned char FRot, unsigned int NPgs, unsigned long TamBuff) {
   unsigned long i, j;
   int k;

   if (ObjContinente) {
      UnMountCache();
   }

   // inicializando parametros de configuracao do cache - InicPar()
   F_ROTACAO = FRot;
   RGS_POR_BLOCO = (unsigned char)(1 << F_ROTACAO);
   N_PAGINAS = NPgs;

   // inicializando TME
   TME.TotBlocos = (NRegs >> F_ROTACAO) + TxCrescBlocos;
   TME.pBloco = new unsigned int[TME.TotBlocos];
   for (i = 0; i < TME.TotBlocos; TME.pBloco[i++] = CNSCACHE_NIL);
   TME.Bloco = new tBloco[N_PAGINAS];
   for (i = 0; i < N_PAGINAS; i++){
      TME.Bloco[i].pPagina = new unsigned int[RGS_POR_BLOCO];
      for (k = 1; k < RGS_POR_BLOCO; TME.Bloco[i].pPagina[k++] = CNSCACHE_NIL);
      TME.Bloco[i].RgsUsados = 0;
      TME.Bloco[i].pPagina[0] = i + 1; // se bloco nao usado a posicao 0 aponta
                                // para o proximo disponivel - economiza memoria
   } // fim do for
   TME.Bloco[N_PAGINAS - 1].pPagina[0] = CNSCACHE_NIL;
   TME.LBlocoDispo = 0;

   // inicializando ListaPgs
   ListaPgs.Pagina = new tPagina[N_PAGINAS];
   for (i = 0; i < N_PAGINAS; i++){
      unsigned char *paux;

      ListaPgs.Pagina[i].proxDaFila = i + 1;
      ListaPgs.Pagina[i].antDaFila = CNSCACHE_NIL;
      ListaPgs.Pagina[i].IdxTotAcessos = CNSCACHE_NIL;
      ListaPgs.Pagina[i].IdRegFisico = 0xFFFFFFFF;
      ListaPgs.Pagina[i].Escrita = 0;
      ListaPgs.Pagina[i].Page = new unsigned char  [TamBuff];
      paux = ListaPgs.Pagina[i].Page;
      for (j = 0; j < TamBuff; *paux++ = '\0', j++);
   } // fim do for
   ListaPgs.Pagina[N_PAGINAS - 1].proxDaFila = CNSCACHE_NIL;
   ListaPgs.LPgDispo = 0;
   ListaPgs.PgEmUso = CNSCACHE_NIL;

   // inicializando GerAcessos
   GerAcessos.LTotAcessos = new tLTotAcessos[N_PAGINAS];
   for (i = 0; i < N_PAGINAS; i++){
      GerAcessos.LTotAcessos[i].TotAcessos = 0;
      GerAcessos.LTotAcessos[i].prox = i + 1;
      GerAcessos.LTotAcessos[i].ant = CNSCACHE_NIL;
      GerAcessos.LTotAcessos[i].frontFilaPg =
            GerAcessos.LTotAcessos[i].rearFilaPg = CNSCACHE_NIL;
   } // fim do for
   GerAcessos.LTotAcessos[N_PAGINAS - 1].prox = CNSCACHE_NIL;
   GerAcessos.Cabeca = CNSCACHE_NIL;
   GerAcessos.LDispo = 0;

   // inicializando parametros restantes
   ObjContinente = father;
}//end CNSCache<TpObjCntin>::MountCache

//==============================================================================
// Metodo: void ReadPage(pal4, void **)
//==============================================================================
template <class TpObjCntin>
void CNSCache<TpObjCntin>::ReadPage(unsigned long reg, void **page) {
   unsigned long bloco = (reg >> F_ROTACAO);
   unsigned int pBloco = TME.pBloco[bloco];
   unsigned int bit = (unsigned int)(reg - (bloco << F_ROTACAO));
   unsigned int pPg = ((pBloco != CNSCACHE_NIL) ?
                               (TME.Bloco[pBloco].pPagina[bit]) : CNSCACHE_NIL);

   if (pPg != CNSCACHE_NIL){ // a pagina ja esta na memoria
      ListaPgs.PgEmUso = pPg;
      IncPagina(); // incrementa a pagina em uso
      *page = (void *)ListaPgs.Pagina[pPg].Page;
      return;
   } // fim do then
   else { // o registro nao esta na memoria
      pPg = ObtemPagina();
      // obtem registro da base
      ObjContinente->DiskReadPage(reg, ListaPgs.Pagina[pPg].Page);
      ListaPgs.PgEmUso = pPg;
      ListaPgs.Pagina[pPg].IdRegFisico = reg;

      //    IncPagina(); // incrementa a pagina em uso mesmo que seja nova, pois evita
             // que sempre as mesmas paginas sejam liberadas
      *page = (void *)ListaPgs.Pagina[pPg].Page;

      // atualiza TME
      // IMPORTANTE: o teste abaixo nao pode ser "if (pBloco == CNSCACHE_NIL)"!
      // Observe que se TME.pBloco[bloco] possuir no minimo um registro, pBloco
      // e diferente de CNSCACHE_NIL. Porem, se a pagina obtida coincidir com o
      // registro de TME.pBloco[bloco] (que so contem este registro),
      // TME.pBloco[bloco] passa a ser CNSCACHE_NILL (ver // IMPORTANTE REF em
      // ObtemPagina()), gerando erro com relacao a pBloco que antes era diferente
      // de CNSCACHE_NIL, visto que existia um registro neste bloco.
      if (TME.pBloco[bloco] == CNSCACHE_NIL){ // nao existe nenhum registro deste
                                            // bloco na memoria
         // obter um bloco livre e incluir no ponteiro de blocos
         pBloco = TME.LBlocoDispo;
         // o teste abaixo pode ser retirado, se o cache estiver funcionando
         // corretamentepara aumentar desempenho
         if (pBloco == CNSCACHE_NIL) { //nunca deve acontecer - erro de logica
            printf("CNSCache::GMC_le - Erro ao tentar obter um bloco");
            printf(" livre!\n");
            exit(1);
         } // fim do if
         TME.LBlocoDispo = TME.Bloco[pBloco].pPagina[0];
         TME.Bloco[pBloco].pPagina[0] = CNSCACHE_NIL;
         TME.Bloco[pBloco].RgsUsados = 0;
         TME.pBloco[bloco] = pBloco;
      } // fim do if
      TME.Bloco[pBloco].pPagina[bit] = pPg;
      TME.Bloco[pBloco].RgsUsados++;
   } // fim do if
}//end CNSCache<TpObjCntin>::ReadPage

//==============================================================================
// Metodo: void NewPage(unsigned long reg, void **page)
//==============================================================================
template <class TpObjCntin>
void CNSCache<TpObjCntin>::NewPage(unsigned long reg, void **page) {
   if (reg >=  (TME.TotBlocos * RGS_POR_BLOCO)) { // estouro do numero de blocos
      unsigned long novoTotal = TME.TotBlocos + TxCrescBlocos;
      unsigned long i;

      TME.pBloco = (unsigned int *)realloc(TME.pBloco,
                                              sizeof(unsigned int) * novoTotal);
      for (i = TME.TotBlocos; i < novoTotal; TME.pBloco[i++] = CNSCACHE_NIL);
      TME.TotBlocos = novoTotal;
   } // fim do if
   ReadPage(reg, page);
}//end CNSCache<TpObjCntin>::NewPage

//==============================================================================
// Metodo: void FlushCache()
// Descarrega as paginas alteradas em disco
//==============================================================================
template <class TpObjCntin>
void CNSCache<TpObjCntin>::FlushCache() {
   unsigned int i, j;

   i = GerAcessos.Cabeca;
   while (i != CNSCACHE_NIL) {
      j = GerAcessos.LTotAcessos[i].rearFilaPg;
      while (j != CNSCACHE_NIL) {
         if (ListaPgs.Pagina[j].Escrita) {
            ObjContinente->DiskWritePage(ListaPgs.Pagina[j].IdRegFisico,
                                         ListaPgs.Pagina[j].Page);
            ListaPgs.Pagina[j].Escrita = 0;
         }
         // fim do if
         j = ListaPgs.Pagina[j].proxDaFila;
      } // fim do while
      i = GerAcessos.LTotAcessos[i].prox;
   } // fim do while
}//end CNSCache<TpObjCntin>::FlushCache

//==============================================================================
// Metodo: void IncPagina()
// incrementa o numero de acessos da pagina corrente
//==============================================================================
template <class TpObjCntin>
void CNSCache<TpObjCntin>::IncPagina() {
   unsigned int pPgEmUso = ListaPgs.PgEmUso;
   unsigned int pTotAces = ListaPgs.Pagina[pPgEmUso].IdxTotAcessos;
   unsigned int pProxTotAces = GerAcessos.LTotAcessos[pTotAces].prox;

   if (pProxTotAces != CNSCACHE_NIL) { // este nao e o ultimo TotAcessos
      if ((GerAcessos.LTotAcessos[pTotAces].TotAcessos + 1) ==
           GerAcessos.LTotAcessos[pProxTotAces].TotAcessos) {
            // ProxTotAcessos e um a mais que o atual
         TiraPgFila(pPgEmUso, pTotAces);
         ColocaPgFila(pPgEmUso, pProxTotAces);
      } // fim do then
      else { // ProxTotAcessos e mais do que um a mais que o atual
         if (GerAcessos.LTotAcessos[pTotAces].frontFilaPg ==
             GerAcessos.LTotAcessos[pTotAces].rearFilaPg) {
            // esta e a unica pagina neste TotAcessos e o total seguinte e
            // maior do que este incrementado, basta incrementar o total
            GerAcessos.LTotAcessos[pTotAces].TotAcessos++;
         }else { // existem outras paginas na fila deste TotAcessos e o total
            // seguinte e maior do que este incrementado. Um novo total deve ser
            // criado e esta pagina deve ser transferida para ele.
            TiraPgFila(pPgEmUso, pTotAces);
            NovoTotal(pPgEmUso, pTotAces); // pTotAces e alterado retornando o novo
         } // fim do if
         if (GerAcessos.LTotAcessos[pTotAces].TotAcessos > 0xFFEF) {
            // esta proximo do valor maximo, todos os totais sao reajustados
            // a comparacao foi feita com 0xFFEF e nao com 0xFFFF por seguranca
            unsigned int pAux = GerAcessos.Cabeca;
            unsigned int novoTot = 1;

            while (pAux != CNSCACHE_NIL) {
               GerAcessos.LTotAcessos[pAux].TotAcessos = novoTot++;
               pAux = GerAcessos.LTotAcessos[pAux].prox;
            } // fim do while
         } //fim do if
      } // fim do if
   }else{ // este e o ultimo TotAcessos
      if (GerAcessos.LTotAcessos[pTotAces].frontFilaPg ==
          GerAcessos.LTotAcessos[pTotAces].rearFilaPg) {
          // esta e a unica pagina neste TotAcessos e o total seguinte e
          // maior do que este incrementado, basta incrementar o total
         GerAcessos.LTotAcessos[pTotAces].TotAcessos++;
      }else { // existem outras paginas na fila deste TotAcessos e o total
         // seguinte e maior do que este incrementado. Um novo total deve ser
         // criado e esta pagina deve ser transferida para ele.
         TiraPgFila(pPgEmUso, pTotAces);
         NovoTotal(pPgEmUso, pTotAces); // pTotAces e alterado retornando o novo
      } // fim do if
      if (GerAcessos.LTotAcessos[pTotAces].TotAcessos > 0xFFEF) {
         // esta proximo do valor maximo, todos os totais sao reajustados
         // a comparacao foi feita com 0xFFEF e nao com 0xFFFF por seguranca
         unsigned int pAux = GerAcessos.Cabeca;
         unsigned int novoTot = 1;

         while (pAux != CNSCACHE_NIL) {
            GerAcessos.LTotAcessos[pAux].TotAcessos = novoTot++;
            pAux = GerAcessos.LTotAcessos[pAux].prox;
         } // fim do while
      } //fim do if
   } // fim do if
}//end CNSCache<TpObjCntin>::IncPagina

//==============================================================================
// Metodo: unsigned int ObtemPagina()
//==============================================================================
template <class TpObjCntin>
unsigned int CNSCache<TpObjCntin>::ObtemPagina() {
   unsigned int pPgLivre = ListaPgs.LPgDispo;
   unsigned int pTotAces;

   if (pPgLivre != CNSCACHE_NIL){ // ainda existe uma pagina livre no cache
      ListaPgs.LPgDispo = ListaPgs.Pagina[pPgLivre].proxDaFila;
      ListaPgs.Pagina[pPgLivre].proxDaFila = CNSCACHE_NIL;
      if (GerAcessos.Cabeca == CNSCACHE_NIL) { // este e o primeiro total a ser incluido
         pTotAces = GerAcessos.LDispo;
         GerAcessos.LDispo = GerAcessos.LTotAcessos[pTotAces].prox;
         GerAcessos.LTotAcessos[pTotAces].prox = CNSCACHE_NIL;
         GerAcessos.Cabeca = pTotAces;
         GerAcessos.LTotAcessos[pTotAces].frontFilaPg =
                 GerAcessos.LTotAcessos[pTotAces].rearFilaPg = pPgLivre;
         // GerAcessos.LTotAcessos[pTotAces].TotAcessos ja vale 0,
         // nao perder tempo inicializando aqui
         ListaPgs.Pagina[pPgLivre].IdxTotAcessos = pTotAces;
      }else { // ja existe pelo menos um TotAcessos, a pPgLivre sera incluida
         // na fila de paginas do menor TotAcessos (GerAcessos.Cabeca) e nao
         // em um TotAcessos com 0 para que nao haja uma distancia muito
         // grande entre o atual e o proximo. A inclusao na fila e sempre
         // no GerAcessos.LTotAcessos[pTotAces].rearFilaPg
         ColocaPgFila(pPgLivre, GerAcessos.Cabeca);
      }// fim do if
      return (pPgLivre);
   }else { // nao existe pagina livre no cache. Retira pagina acessada a mais
           // tempo da fila de paginas que possui menor frequencia de acessos
      unsigned long reg;
      unsigned long bloco;
      unsigned int pBloco;
      unsigned int bit;
      unsigned int pPgAnt, pPgRear, pPgFrontProxTot;
      unsigned int pProxTotAces;

      pTotAces = GerAcessos.Cabeca;
      pPgLivre = GerAcessos.LTotAcessos[pTotAces].frontFilaPg;
      if (ListaPgs.Pagina[pPgLivre].Escrita) {
         ObjContinente->DiskWritePage(ListaPgs.Pagina[pPgLivre].IdRegFisico,
                                      ListaPgs.Pagina[pPgLivre].Page);
         ListaPgs.Pagina[pPgLivre].Escrita = 0;
      } // fim do if
      reg = ListaPgs.Pagina[pPgLivre].IdRegFisico;
      bloco = (reg >> F_ROTACAO);
      pBloco = TME.pBloco[bloco];
      bit = (unsigned int)(reg - (bloco << F_ROTACAO));
      TME.Bloco[pBloco].pPagina[bit] = CNSCACHE_NIL;
      TME.Bloco[pBloco].RgsUsados--;
      // IMPORTANTE REF : referencia feita por GCM_le sobre tornar
      // TME.pBloco[bloco] = CNSCACHE_NIL quando o bloco da pagina que foi retirada
      // coincide com o bloco da pagina a ser inserida (ver GCM_le).
      if (!TME.Bloco[pBloco].RgsUsados) { // TME.Bloco[pBloco].RgsUsados = 0,
                  // o bloco pBloco nao possui mais nenhum registro em
                        // memoria. Deve ser liberado entao.
         TME.Bloco[pBloco].pPagina[0] = TME.LBlocoDispo;//inclui na lista de dispo
         TME.LBlocoDispo = pBloco;
         TME.pBloco[bloco] = CNSCACHE_NIL;
      } // fim do if
      // tira o no da frente da fila e coloca no fim, no mesmo total.
      // primeiro verifica se so existe uma pg na fila: se so for 1 pg, nao faz
      // nada; se forem mais de 1 pgs, tira a da frente e reinsere no fim da fila.
      if (GerAcessos.LTotAcessos[pTotAces].frontFilaPg !=
          GerAcessos.LTotAcessos[pTotAces].rearFilaPg) {//existe + de 1 pg na fila
         pPgAnt = ListaPgs.Pagina[pPgLivre].antDaFila;
         GerAcessos.LTotAcessos[pTotAces].frontFilaPg = pPgAnt;
         ListaPgs.Pagina[pPgAnt].proxDaFila =
               ListaPgs.Pagina[pPgLivre].antDaFila = CNSCACHE_NIL;
         ListaPgs.Pagina[pPgLivre].proxDaFila = pPgRear =
               GerAcessos.LTotAcessos[pTotAces].rearFilaPg;
         GerAcessos.LTotAcessos[pTotAces].rearFilaPg =
               ListaPgs.Pagina[pPgRear].antDaFila = pPgLivre;
      }
      // incrementa o total desta fila para nao obter uma retirada viciosa.
      // Se o total seguinte for o atual + 1, a fila atual sera incluida
      // na frente da outra, mantendo a ordem em que as paginas seriam retiradas,
      // mas acrescentando as outras paginas para nao obter uma retirada viciosa.
      // Caso contrario, o total atual e incrementado de um.
      pProxTotAces = GerAcessos.LTotAcessos[pTotAces].prox;
      if (pProxTotAces != CNSCACHE_NIL) { // este nao e o ultimo TotAcessos
         if ((GerAcessos.LTotAcessos[pTotAces].TotAcessos + 1) ==
              GerAcessos.LTotAcessos[pProxTotAces].TotAcessos) {
            // ProxTotAcessos e um a mais que o atual, a fila atual e incluida
            // no final da proxima fila, e o total atual sera disponibilizado
            unsigned int pPgAux = pPgLivre;

            while (pPgAux != CNSCACHE_NIL) { // atualiza o indice de totais das
                        // paginas da fila atual para apontar para o novo indice
               ListaPgs.Pagina[pPgAux].IdxTotAcessos = pProxTotAces;
               pPgAux = ListaPgs.Pagina[pPgAux].proxDaFila;
            }// fim do while
            pPgFrontProxTot = GerAcessos.LTotAcessos[pProxTotAces].frontFilaPg;
            ListaPgs.Pagina[pPgLivre].antDaFila = pPgFrontProxTot;
            ListaPgs.Pagina[pPgFrontProxTot].proxDaFila = pPgLivre;
            GerAcessos.LTotAcessos[pProxTotAces].frontFilaPg =
                       GerAcessos.LTotAcessos[pTotAces].frontFilaPg;
            GerAcessos.LTotAcessos[pTotAces].rearFilaPg = CNSCACHE_NIL;
            GerAcessos.LTotAcessos[pTotAces].frontFilaPg = CNSCACHE_NIL;
            GerAcessos.Cabeca = pProxTotAces;
            GerAcessos.LTotAcessos[pProxTotAces].ant = CNSCACHE_NIL;
            GerAcessos.LTotAcessos[pTotAces].TotAcessos = 0;
            GerAcessos.LTotAcessos[pTotAces].prox = GerAcessos.LDispo;
            GerAcessos.LDispo = pTotAces;
         }else { // ProxTotAcessos e mais do que um a mais que o atual
                 // basta incrementar o total
            GerAcessos.LTotAcessos[pTotAces].TotAcessos++;
            if (GerAcessos.LTotAcessos[pTotAces].TotAcessos > 0xFFEF) {
            // esta proximo do valor maximo, todos os totais sao reajustados
            // a comparacao foi feita com 0xFFEF e nao com 0xFFFF por seguranca
               unsigned int pAux = GerAcessos.Cabeca;
               unsigned int novoTot = 1;

               while (pAux != CNSCACHE_NIL) {
                  GerAcessos.LTotAcessos[pAux].TotAcessos = novoTot++;
                  pAux = GerAcessos.LTotAcessos[pAux].prox;
               } // fim do while
            } //fim do if
         } // fim do if
      }else{ // este e o ultimo TotAcessos, basta incrementar o total
         GerAcessos.LTotAcessos[pTotAces].TotAcessos++;
         if (GerAcessos.LTotAcessos[pTotAces].TotAcessos > 0xFFEF) {
            // esta proximo do valor maximo, todos os totais sao reajustados
            // a comparacao foi feita com 0xFFEF e nao com 0xFFFF por seguranca
            unsigned int pAux = GerAcessos.Cabeca;
            unsigned int novoTot = 1;

            while (pAux != CNSCACHE_NIL) {
               GerAcessos.LTotAcessos[pAux].TotAcessos = novoTot++;
               pAux = GerAcessos.LTotAcessos[pAux].prox;
            }// fim do while
         } //fim do if
      } // fim do if
      return (pPgLivre); // o total de acessos a essa pg sera incrementado depois
   }// fim do if
}//end int CNSCache<TpObjCntin>::ObtemPagina

//==============================================================================
// Metodo: void TiraPgFila(unsigned int pPg, unsigned int pTotAces)
//==============================================================================
template <class TpObjCntin>
void CNSCache<TpObjCntin>:: TiraPgFila(unsigned int pPg,
                                       unsigned int pTotAces) {
   unsigned int pPgAux;
   unsigned int pTotAcesAux;

   if (GerAcessos.LTotAcessos[pTotAces].frontFilaPg ==
       GerAcessos.LTotAcessos[pTotAces].rearFilaPg) {//so existe essa pg na fila
      pTotAcesAux = GerAcessos.LTotAcessos[pTotAces].ant;
      if (pTotAcesAux != CNSCACHE_NIL) // existe total anterior
         GerAcessos.LTotAcessos[pTotAcesAux].prox =
                                  GerAcessos.LTotAcessos[pTotAces].prox;
      else // nao existe total anterior, este e o primeiro
         GerAcessos.Cabeca = GerAcessos.LTotAcessos[pTotAces].prox;

      pTotAcesAux = GerAcessos.LTotAcessos[pTotAces].prox;
      if (pTotAcesAux != CNSCACHE_NIL) // existe prox total
         GerAcessos.LTotAcessos[pTotAcesAux].ant =
                                    GerAcessos.LTotAcessos[pTotAces].ant;

      // insere total atual na lista de dispo
      GerAcessos.LTotAcessos[pTotAces].TotAcessos = 0;
      GerAcessos.LTotAcessos[pTotAces].ant = CNSCACHE_NIL;
      GerAcessos.LTotAcessos[pTotAces].frontFilaPg =
                     GerAcessos.LTotAcessos[pTotAces].rearFilaPg = CNSCACHE_NIL;
      GerAcessos.LTotAcessos[pTotAces].prox = GerAcessos.LDispo;
      GerAcessos.LDispo = pTotAces;
   }else { // existe mais de uma pg na fila
      pPgAux = ListaPgs.Pagina[pPg].antDaFila;

      // testa se existe pagina anterior na fila - nao e a primeira
      if (pPgAux != CNSCACHE_NIL)
         ListaPgs.Pagina[pPgAux].proxDaFila = ListaPgs.Pagina[pPg].proxDaFila;
      else  // nao existe pagina anterior na fila - e a primeira
         GerAcessos.LTotAcessos[pTotAces].rearFilaPg =
                                           ListaPgs.Pagina[pPg].proxDaFila;

      pPgAux = ListaPgs.Pagina[pPg].proxDaFila;
      if (pPgAux != CNSCACHE_NIL) // existe prox pagina - nao e a ultima
         ListaPgs.Pagina[pPgAux].antDaFila = ListaPgs.Pagina[pPg].antDaFila;
      else // nao existe prox pagina - e a ultima
         GerAcessos.LTotAcessos[pTotAces].frontFilaPg =
                                          ListaPgs.Pagina[pPg].antDaFila;

   }// fim do if
   ListaPgs.Pagina[pPg].proxDaFila =
                           ListaPgs.Pagina[pPg].antDaFila = CNSCACHE_NIL;
   ListaPgs.Pagina[pPg].IdxTotAcessos = CNSCACHE_NIL;
}//end CNSCache<TpObjCntin>:: TiraPgFila

//==============================================================================
// Metodo: void ColocaPgFila(unsigned int pPg, unsigned int pTotAces)
//==============================================================================
template <class TpObjCntin>
void CNSCache<TpObjCntin>::ColocaPgFila(unsigned int pPg,
                                        unsigned int pTotAces) {
   unsigned int pPgAux;

   pPgAux = GerAcessos.LTotAcessos[pTotAces].rearFilaPg;
   ListaPgs.Pagina[pPgAux].antDaFila = pPg;
   ListaPgs.Pagina[pPg].proxDaFila = pPgAux;
   GerAcessos.LTotAcessos[pTotAces].rearFilaPg = pPg;
   ListaPgs.Pagina[pPg].antDaFila = CNSCACHE_NIL;
   ListaPgs.Pagina[pPg].IdxTotAcessos = pTotAces;
}//end CNSCache<TpObjCntin>::ColocaPgFila

//==============================================================================
// Metodo: void NovoTotal(unsigned int pPg, unsigned int pTotAces)
//==============================================================================
template <class TpObjCntin>
void CNSCache<TpObjCntin>::NovoTotal(unsigned int pPg, unsigned int &pTotAces) {
   unsigned int pTotAcesNovo;
   unsigned int pTotAcesProx;

   // obtem um TotAcess dispo
   pTotAcesNovo = GerAcessos.LDispo;
   GerAcessos.LDispo = GerAcessos.LTotAcessos[pTotAcesNovo].prox;

   // insere novo TotAcess na lista
   GerAcessos.LTotAcessos[pTotAcesNovo].TotAcessos = 1 +
          GerAcessos.LTotAcessos[pTotAces].TotAcessos;
   GerAcessos.LTotAcessos[pTotAcesNovo].ant  = pTotAces;
   pTotAcesProx = GerAcessos.LTotAcessos[pTotAcesNovo].prox =
             GerAcessos.LTotAcessos[pTotAces].prox;
   GerAcessos.LTotAcessos[pTotAces].prox = pTotAcesNovo;
   if (pTotAcesProx != CNSCACHE_NIL) // o total atual nao era o ultimo
      GerAcessos.LTotAcessos[pTotAcesProx].ant = pTotAcesNovo;

   // insere pPg no novo total
   GerAcessos.LTotAcessos[pTotAcesNovo].frontFilaPg =
          GerAcessos.LTotAcessos[pTotAcesNovo].rearFilaPg = pPg;
   ListaPgs.Pagina[pPg].proxDaFila =
         ListaPgs.Pagina[pPg].antDaFila = CNSCACHE_NIL;
   ListaPgs.Pagina[pPg].IdxTotAcessos = pTotAcesNovo;

   pTotAces = pTotAcesNovo; // retorna o novo pTotAces
}//end CNSCache<TpObjCntin>::NovoTotal

//==============================================================================
// Metodo: ~CNSCache()
//==============================================================================
template <class TpObjCntin>
void CNSCache<TpObjCntin>::UnMountCache() {
   unsigned int i;

   FlushCache();

   // liberando TME
   for (i = 0; i < N_PAGINAS; delete[] TME.Bloco[i++].pPagina);

   delete[] TME.Bloco;
   delete[] TME.pBloco;

   // liberando ListaPgs
   for (i = 0; i < N_PAGINAS; delete[] ListaPgs.Pagina[i++].Page);

   delete[] ListaPgs.Pagina;

   // liberando GerAcessos
   delete[] GerAcessos.LTotAcessos;

   ObjContinente = NULL;
}//end CNSCache<TpObjCntin>::UnMountCache

#ifdef CNSCACHE_CHECK
//==============================================================================
// Metodo:
//==============================================================================
template <class TpObjCntin>
CNSCache<TpObjCntin>::checkStructure() {
   int pBlocoE, // entrada no vetor pBloco
      blocoE, // entrada no vetor TME.Bloco[]
      paginasChecadas = 0, // contador de paginas checadas
      blocosChecados = 0; // contador de blocos checados

   checkCounter++;

   // ===========================================================================
   // confere TME
   // ===========================================================================
   // Verificar se o bloco que esta ativado (diferente de CNSCACHE_NIL) esta
   // realmente sendo usado e se a pagina referente a cada entrada nesse bloco
   // esta realmente apontando para ele. Alem disso, cada pagina checada sera
   // contada, se o numero de paginas checadas for diferente de N_PAGINAS, erro.
   // Em seguida, a lista de blocos disponiveis sera checada. O numero de blocos
   // na lista de disponiveis somado ao numero de blocos ativos tem que ser igual
   // a N_PAGINAS. Nao ha a necessidade de contar o numero de entradas inativas
   // no vetor pBlocos, pois elas nao estao listadas e, se ela estiver ativada
   // e apontando para um bloco invalido, a checagem da pagina ira detectar isso.

   for (pBlocoE = 0; pBlocoE < TME.TotBlocos; pBlocoE++) {
      if (TME.pBloco[pBlocoE] != CNSCACHE_NIL) {
         int pPagina, pgChecadaLocal = 0;

         blocoE = TME.pBloco[pBlocoE]; // entrada no vetor TME.Bloco
         blocosChecados++;
         if (!TME.Bloco[blocoE].RgsUsados) { // esta ativo mas nao aponta nenhuma
                                             //  pagina
            printf("Erro em TME.Bloco[blocoE].RgsUsados."
                   " checkcounter = %ld\n", checkCounter);
            exit(1);
         }//end if

         for (pPagina = 0; pPagina < RGS_POR_BLOCO; pPagina++) {
            int pg = TME.Bloco[blocoE].pPagina[pPagina];
            if (pg != CNSCACHE_NIL) {
               pgChecadaLocal++;
               paginasChecadas++;

               if((pblocoE != (ListaPgs.Pagina[pg].IdRegFisico >> F_ROTACAO)) {
                  printf("Erro em ListaPgs.Pagina[pg].IdRegFisico >> F_ROTACAO."
                         " checkcounter = %ld\n", checkCounter);
                  exit(1);
               }//end if
               if((blocoE != (ListaPgs.Pagina[pg].IdRegFisico -
                  (pblocoE << F_ROTACAO)) {
                  printf("Erro em ListaPgs.Pagina[pg].IdRegFisico - (pblocoE << "
                         "F_ROTACAO. checkcounter = %ld\n", checkCounter);
                  exit(1);
               }//end if
            }//end if
         }//end for
         // garantido que todas as paginas apontam para os blocos que apontam
         // para elas.
         if (TME.Bloco[blocoE].RgsUsados != pgChecadaLocal) {
            printf("Erro em TME.Bloco[blocoE].RgsUsados != pgChecadaLocal."
                   " checkcounter = %ld\n", checkCounter);
            exit(1);
         }//end if
      } // garantido que todas as paginas apontadas por um bloco sao validas,
        // que elas apontam para eles mesmos, e que o numero de paginas apontas
        // por eles eh o numero indicado em RgsUsados
   } // garantido que os blocos ativos apontam alguma(s) pagina(s), que
     // esta(s) pagina(s) aponta(m) para o mesmo bloco que a(s) aponta(m), e
     // que o suposto numero de paginas apontadas (RgsUsados) esta correto.
   // checando TME.LBlocoDispo
   blocoE = TME.LBlocoDispo;
   while (blocoE != CNSCACHE_NIL) {
      blocosChecados++;
      blocoE = TME.Bloco[blocoE].pPagina[0];
   }//end while
   if (blocosChecados != N_PAGINAS) {
      printf("Erro em blocosChecados != N_PAGINAS."
             " checkcounter = %ld\n", checkCounter);
      exit(1);
   }//end if
   // garantido que todos os blocos que nao estao ativos estao na lista de
   // disponiveis. Por consequencia, a lista de disponiveis esta corretamente
   // encadeada

}//end CNSCache<TpObjCntin>::checkStructure

#endif // CNSCACHE_CHECK

#endif // CNSCACHEH
