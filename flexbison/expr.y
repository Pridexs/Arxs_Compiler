%{
#include <stdio.h>
#include <stdlib.h>
#include "comp.h"
#define YYSTYPE struct Atributo
%}

%token TSTRING TVOID TINT TRETURN TPRINT TREAD TIF TWHILE TELSE TMEIGUAL TMAIGUAL TIGUAL TDIFER TATRIB TMAIOR TMENOR TACHAVE TFCHAVE TADD TSUB TMUL TDIV TAPAR TFPAR TFIM TNUM TVIRG TPVIRG TID TLITERAL TNOT TAND TOR TERROR EOP TINC TDEC TINC_N TDEC_N

%%
Inicio
      : Programa { geraArquivoFinal(); }
      ;

Programa
      : ListaFuncoes J BlocoPrincipal
      | J BlocoPrincipal
      ;

J
      : { corrigirIDsMain(); }
      ;

ListaFuncoes
      : ListaFuncoes FuncaoGeral { insereDelimitadorFuncao(); }
      | FuncaoGeral { insereDelimitadorFuncao(); }
      ;

FuncaoGeral
      : Funcao BlocoPrincipal { corrigirStack_e_Local(); }
      ;

Funcao
      : TipoRetorno TID TAPAR DeclParametros TFPAR { insereFuncaoComPar(&($4.listaParametros), $1.tipo, $2.id); }
      | TipoRetorno TID TAPAR TFPAR { insereFuncaoSemPar($1.tipo, $2.id); }
      ;

TipoRetorno
      : Tipo { $$.tipo = $1.tipo; }
      | TVOID { $$.tipo = TIPO_VOID; }
      ;

DeclParametros
      : DeclParametros TVIRG Tipo TID { insereParametro(&($$.listaParametros), $3.tipo, $4.id); }
      | Tipo TID { inicializaListaParametros(&($$.listaParametros)); insereParametro(&($$.listaParametros), $1.tipo, $2.id); }
      ;

BlocoPrincipal
      : TACHAVE Declaracoes ListaCmd TFCHAVE
      | TACHAVE ListaCmd TFCHAVE
      ;

Declaracoes
      : Declaracoes Declaracao
      | Declaracao
      ;

Declaracao
      : Tipo ListaId TPVIRG { insereTabelaSimbolos( (&$2.listaIDs), $1.tipo );}
      ;

Tipo
      : TINT {$$.tipo = TIPO_INT;}
      | TSTRING {$$.tipo = TIPO_STRING; }
      ;

ListaId
      : ListaId TVIRG TID { insereID( &($$.listaIDs), $3.id); }
      | TID { inicializaListaIDs(&($$.listaIDs)); insereID( &($$.listaIDs), $1.id); }
      ;

Bloco
      : TACHAVE ListaCmd TFCHAVE
      ;

ListaCmd
      : ListaCmd Comando
      | Comando
      ;

Comando
      : CmdSe
      | CmdEnquanto
      | CmdAtrib
      | CmdEscrita
      | CmdLeitura
      | ChamadaFuncao
      | CmdIncrementaUnario
      | CmdDecrementaUnario
      | CmdIncrementa
      | CmdDecrementa
      | Retorno
      ;

CmdIncrementaUnario
      : TID TINC TPVIRG { geraIncrementa($1.id, TIPO_INT, OP_IINC); }
      ;

CmdDecrementaUnario
      : TID TDEC TPVIRG { geraDecrementa($1.id, TIPO_INT, OP_IINC); }
      ;

CmdIncrementa
      : CarregaID TINC_N ExpressaoAritmetica TPVIRG { geraIncrementa($1.id, $3.tipo, 0);}
      ;

CmdDecrementa
      : CarregaID TDEC_N ExpressaoAritmetica TPVIRG { geraDecrementa($1.id, $3.tipo, 0); }
      ;

CarregaID
      : TID { strcpy($$.id, $1.id); if(buscaTipo($1.id) == TIPO_INT) empilhaVarInt($1.id); else error(); }
      ;

Retorno
      : TRETURN ExpressaoAritmetica TPVIRG { geraRetorno($2.tipo); }
      ;

CmdSe
      : TIF TAPAR ExpressaoLogica TFPAR NL Bloco { corrigir(&$3.listav, $5.label); corrigir(&$3.listaf, novoLabel()); }
      | TIF TAPAR ExpressaoLogica TFPAR NL Bloco N TELSE NL Bloco
        {
          corrigir(&$3.listav, $5.label);
          corrigir(&$3.listaf, $9.label);
          corrigir(&$7.listav, novoLabel());
        }
      ;

N
      : { inicializaListaVF(&$$.listav, &$$.listaf); gerarGOTO(); }
      ;

CmdEnquanto
      : TWHILE NL TAPAR ExpressaoLogica TFPAR NL Bloco
        {
          corrigir(&$4.listav, $6.label);
          gerarGOTOLabel($2.label);
          corrigir(&$4.listaf, novoLabel());
        }
      ;

CmdAtrib
      : TID TATRIB ExpressaoAritmetica TPVIRG { geraAtribuicao( $1.id, buscaTipo($1.id), $3.tipo); }
      | TID TATRIB TLITERAL TPVIRG { empilhaString($3.literal); geraAtribuicao( $1.id, buscaTipo($1.id), TIPO_STRING); }
      ;

CmdEscrita
      : TPRINT TAPAR M ExpressaoAritmetica TFPAR TPVIRG { if ($4.tipo == TIPO_INT) invokeStatic(TIPO_INT); else invokeStatic(TIPO_STRING); }
      | TPRINT TAPAR M TLITERAL TFPAR TPVIRG { empilhaString($4.literal); invokeStatic(TIPO_STRING); }
      ;

M
      : { getPrint(); }
      ;

CmdLeitura
      : TREAD TAPAR TID TFPAR TPVIRG
        {
          geraRead(buscaTipo($3.id), buscaPos($3.id));
        }
      ;

ChamadaFuncao
      : TID TAPAR ListaParametros TFPAR TPVIRG { geraChamadaFuncaoComPar($1.id, $3.nParametros); }
      | TID TAPAR TFPAR TPVIRG { geraChamadaFuncao($1.id); }
      ;

ChamadaFuncaoSemPVIRG
      : TID TAPAR ListaParametros TFPAR { $$.tipo = buscaTipoFuncao($1.id); geraChamadaFuncaoComPar($1.id, &$3.listaParametros); }
      | TID TAPAR TFPAR { $$.tipo = buscaTipoFuncao($1.id); geraChamadaFuncao($1.id); }
      ;

ListaParametros
      : ListaParametros TVIRG ExpressaoAritmetica { insereParametroSimples(&$$.listaParametros, $3.tipo); }
      | ExpressaoAritmetica { inicializaListaParametros(&$$.listaParametros); insereParametroSimples(&$$.listaParametros, $1.tipo);  }
      ;

ExpressaoAritmetica
      : ExpressaoAritmetica TADD TermoAritmetico { geraOperacao(OP_ADD); }
      | ExpressaoAritmetica TSUB TermoAritmetico { geraOperacao(OP_SUB); }
      | TermoAritmetico { $$.tipo == $1.tipo; }
      ;

TermoAritmetico
      : TermoAritmetico TMUL FatorAritmetico { if ($1.tipo == $3.tipo) geraOperacao(OP_MUL); else error(); }
      | TermoAritmetico TDIV FatorAritmetico { if ($1.tipo == $3.tipo) geraOperacao(OP_DIV); else error(); }
      | FatorAritmetico { $$.tipo = $1.tipo; }
      ;

FatorAritmetico
      : TNUM { empilhaInteiro($1.constante); $$.tipo = TIPO_INT; }
      | TSUB FatorAritmetico { $$.tipo = $2.tipo; }
      | ChamadaFuncaoSemPVIRG { $$.tipo = $1. tipo; }
      | TID 
        { 
          if( buscaTipo($1.id) == TIPO_INT )  { empilhaVarInt($1.id); $$.tipo = TIPO_INT; }
          else if (buscaTipo($1.id) == TIPO_STRING) { empilhaVarString($1.id); $$.tipo = TIPO_STRING; }
        }
      | TAPAR ExpressaoAritmetica TFPAR
      ;


ExpressaoLogica
      : ExpressaoLogica TAND NL FatorLogico 
        {
          corrigir(&$1.listav, $3.label);
          inicializaListaVF(&$$.listav, &$$.listaf);
          merge(&$$.listaf, &$1.listaf, &$4.listaf);
          merge2(&$$.listav, &$4.listav);
        }
      | ExpressaoLogica TOR NL FatorLogico
        {
          corrigir(&$1.listaf, $3.label);
          inicializaListaVF(&$$.listav, &$$.listaf);
          merge(&$$.listav, &$1.listav, &$4.listav);
          merge2(&$$.listaf, &$4.listaf);
        }
      | FatorLogico { transfereListaVF(&$$.listav, &$$.listaf, &$1.listav, &$1.listaf); }
      ;

NL
      : { $$.label = novoLabel(); }
      ;

FatorLogico
      : TNOT FatorLogico { transfereListaVF(&$$.listav, &$$.listaf, &$2.listav, &$2.listaf); }
      | TAPAR ExpressaoLogica TFPAR { transfereListaVF(&$$.listav, &$$.listaf, &$2.listav, &$2.listaf); }
      | ExpressaoRelacional { transfereListaVF(&$$.listav, &$$.listaf, &$1.listav, &$1.listaf); }
      ;

ExpressaoRelacional
      : ExpressaoAritmetica TMAIOR ExpressaoAritmetica { inicializaListaVF(&$$.listav, &$$.listaf); geraComparacao(CMP_GTHAN); gerarGOTO(); }
      | ExpressaoAritmetica TMENOR ExpressaoAritmetica { inicializaListaVF(&$$.listav, &$$.listaf); geraComparacao(CMP_LTHAN); gerarGOTO(); }
      | ExpressaoAritmetica TMAIGUAL ExpressaoAritmetica { inicializaListaVF(&$$.listav, &$$.listaf); geraComparacao(CMP_GEQUAL); gerarGOTO(); }
      | ExpressaoAritmetica TMEIGUAL ExpressaoAritmetica { inicializaListaVF(&$$.listav, &$$.listaf); geraComparacao(CMP_LEQUAL); gerarGOTO(); }
      | ExpressaoAritmetica TIGUAL ExpressaoAritmetica { inicializaListaVF(&$$.listav, &$$.listaf); geraComparacao(CMP_LEQUAL); gerarGOTO(); }
      | ExpressaoAritmetica TDIFER ExpressaoAritmetica { inicializaListaVF(&$$.listav, &$$.listaf); geraComparacao(CMP_NEQUAL); gerarGOTO(); }
      ;

%%
#include "lex.yy.c"

int yyerror (char *str)
{
    printf("%s - antes %s\n", str, yytext);
}

int yywrap()
{
    return 1;
}
