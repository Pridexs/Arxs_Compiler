#include "comp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
*
*   TODO:
        CHECAR SE TODOS OS ATRIBUTOS ESTAO CORRETOS (DE UMA FUNCAO).
*
*/

Lista tabelaSimbolos;
Lista listaInstrucoes;
Lista listaFuncoes;

unsigned contPos = 0;
unsigned contMaiorPos = 1;
unsigned contLabel = 0;
unsigned contInstrucao = 0;
unsigned contFuncao = 0;
// Conta quantos parametros uma funcao tem. Zera apos inserir a funcao
unsigned contParametro = 0;

// Variavel para contar se a funcao ja foi adicionado na lista.
// Se contFuncao == nFuncoes, entao e necessario adicionar e 
// inicializar uma nova struct Funcao na Lista.
unsigned nFuncoes = 0;

unsigned contStackSize = 0;
unsigned maiorStack = 0; 

int inicializado = 0;

char nomeClasse[200];

void inicializa()
{
    inicializa_lista(&tabelaSimbolos, sizeof(struct Simbolo));
    inicializa_lista(&listaInstrucoes, sizeof(struct Instrucao));
    inicializa_lista(&listaFuncoes, sizeof(struct Funcao));
    inicializado = 1;

}

void inicializaListaIDs(Lista *l)
{
    inicializa_lista(l, sizeof(char) * 32);
}

void inicializaListaParametros(Lista *l)
{
    inicializa_lista(l, sizeof(struct Parametro));
}

void inicializaListaVF(Lista *v, Lista *f) 
{
    inicializa_lista(v, sizeof(unsigned));
    inicializa_lista(f, sizeof(unsigned));

    unsigned proxPos = contInstrucao + 1;
    insereNoFim(v, &contInstrucao);
    insereNoFim(f, &proxPos);

}

void transfereListaVF(Lista *v, Lista *f, Lista *v_old, Lista *f_old)
{
    inicializa_lista(v, sizeof(unsigned));
    inicializa_lista(f, sizeof(unsigned));

    unsigned i;

    while (!listaVazia(v_old))
    {
        removeDoFim(v_old, &i);
        insereNoInicio(v, &i);
    }

    while(!listaVazia(f_old))
    {
        removeDoFim(f_old, &i);
        insereNoInicio(f, &i);
    }

}

// Insere uma lista de ID's na tabela de simbolos
void insereTabelaSimbolos(Lista *l, int tipo)
{
    char info[32];

    while(!listaVazia(l))
    {
        removeDoInicio(l, info);
        struct Simbolo s;
        strcpy(s.id, info);
        s.tipo = tipo;

        if (elementoExiste(&tabelaSimbolos, &s, compara_simbolos))
        {
            printf("ID %s ja existe na tabela de simbolos!\n", info);
            exit(-1);
        }
        
        s.pos = contPos++;

        insereNoFim(&tabelaSimbolos, &s);

    }
}

// Insere um ID na Lista de Atributo
void insereID(Lista *l, char id[32])
{
    insereNoFim(l, id);
}

void imprimeTabelaSimbolos()
{
    printf("Tabela de Simbolos:\n");
    printf("%-3s %-6s %s\n", "POS", "TIPO", "ID");
    mostra_lista(tabelaSimbolos, mostra_simbolo);
    printf("\n");
}

void mostra_simbolo(void *info)
{
    struct Simbolo *s = (struct Simbolo*) info;

    printf("%3d ", s->pos);

    if (s->tipo == TIPO_INT)
        printf("%6s", "INT");
    else if (s->tipo == TIPO_STRING)
        printf("%6s", "STRING");
    printf(" %s\n", s->id);
}

int compara_simbolos(void *a, void *b)
{
    struct Simbolo *s1 = (struct Simbolo*) a;
    struct Simbolo *s2 = (struct Simbolo*) b;

    return strcmp(s1->id, s2->id);
}

int compara_funcao(void *a, void *b)
{
    struct Funcao *f1 = (struct Funcao*) a;
    struct Funcao *f2 = (struct Funcao*) b;

    return strcmp(f1->id, f2->id);
}

void insereFuncaoComPar(Lista *lParam, unsigned tipoRetorno, char id[32])
{
    struct Funcao f;
    strcpy(f.id, id);
    f.tipoRetorno = tipoRetorno;
    f.pos = contFuncao;
    f.nParametros = contParametro;

    inicializa_lista(&(f.listaParametros), sizeof(struct Parametro));
    //printf("Funcao nome %s tipo retorno %u pos %u nparame %u\n", f.id, f.tipoRetorno, contPos, contParametro);
    // Mover lista de parametros;
    while(!listaVazia(lParam))
    {
        struct Parametro p;
        removeDoFim(lParam, &p);
        insereNoInicio(&(f.listaParametros), &p);
        //printf("Inseri parametro tipo %u id: %s\n", p.tipo, p.id);
    }

    insereNoFim(&listaFuncoes, &f);
    contParametro = 0;
    contFuncao++;
}

void insereFuncaoSemPar(unsigned tipoRetorno, char id[32])
{
    struct Funcao f;
    strcpy(f.id, id);
    f.tipoRetorno = tipoRetorno;
    f.pos = contFuncao;
    f.nParametros = contParametro;

    insereNoFim(&listaFuncoes, &f);
    contParametro = 0;
    contFuncao++;
}

void insereParametro(Lista *l, unsigned tipo, char id[32])
{
    struct Parametro p;
    p.tipo = tipo;
    p.pos = contParametro;
    strcpy(p.id, id);
    insereNoFim(l, &p);
    contParametro++;

    // Insere Simbolo na Tabela
    //printf("INSERI %s NA POS %u TIPO %u\n", id, tipo, contPos);
    struct Simbolo s;
    strcpy(s.id, id);
    s.tipo = tipo;
    s.pos = contPos;
    insereNoFim(&tabelaSimbolos, &s);
    contPos++;
}

void insereDelimitadorFuncao()
{
    geraInstrucao(NULO, INST_DELIMITADORFUNC, NULO, NULO, NULO_STR);
   
    if (contPos > contMaiorPos)
        contMaiorPos = contPos;
    contPos = 0;
    // contlabel tambem reseta??
    limpa_lista(&tabelaSimbolos);
}

void geraInstrucao(unsigned label, int inst, int p1, int p2, char str[256])
{
    struct Instrucao i;
    i.label = label;
    i.inst = inst;
    i.p1 = p1;
    i.p2 = p2;

    if (str[0] != '\0')
    {
        strcpy(i.literal, str);
    }

    insereNoFim(&listaInstrucoes, &i);

    contInstrucao++;
}

void geraAtribuicao(char var[32], unsigned tipo1, unsigned tipo2)
{
    decrementaStack(1);

    //printf("%s %u %u\n", var, tipo1, tipo2);
    if (tipo1 == tipo2)
    {
        if (tipo1 == TIPO_INT)
            geraInstrucao(NULO, INST_ISTORE, buscaPos(var), NULO, NULO_STR);
        else
            geraInstrucao(NULO, INST_ASTORE, buscaPos(var), NULO, NULO_STR);
    }
    else
    {
        printf("Atribuicao na variavel %s com tipos diferentes!\n", var);
        exit(-1);
    }
}

void geraComparacao(int comp)
{
    decrementaStack(2);

    switch(comp)
    {
        case CMP_EQUAL:
            geraInstrucao(NULO, INST_IF_ICMPEQ, NULO, NULO, NULO_STR); break;
        case CMP_NEQUAL:
            geraInstrucao(NULO, INST_IF_ICMPNE, NULO, NULO, NULO_STR); break;
        case CMP_LTHAN:
            geraInstrucao(NULO, INST_IF_ICMPLT, NULO, NULO, NULO_STR); break;
        case CMP_LEQUAL:
            geraInstrucao(NULO, INST_IF_ICMPLE, NULO, NULO, NULO_STR); break;
        case CMP_GTHAN:
            geraInstrucao(NULO, INST_IF_ICMPGT, NULO, NULO, NULO_STR); break;
        case CMP_GEQUAL:
            geraInstrucao(NULO, INST_IF_ICMPGE, NULO, NULO, NULO_STR); break;
        default:
            printf("Comparacao nao existente!\n"); exit(-1);
    }

}

void gerarGOTO() 
{
    geraInstrucao(NULO, INST_GOTO, NULO, NULO, NULO_STR);
}

void gerarGOTOLabel(unsigned label)
{
    geraInstrucao(label, INST_GOTO, NULO, NULO, NULO_STR);
}

void geraOperacao(int comp)
{
    // Tira 2 do stack mas empilha outro
    decrementaStack(1);

    switch(comp)
    {
        case OP_ADD:
            geraInstrucao(NULO, INST_IADD, NULO, NULO, NULO_STR); break;
        case OP_SUB:
            geraInstrucao(NULO, INST_ISUB, NULO, NULO, NULO_STR); break;
        case OP_DIV:
            geraInstrucao(NULO, INST_IDIV, NULO, NULO, NULO_STR); break;
        case OP_MUL:
            geraInstrucao(NULO, INST_IMUL, NULO, NULO, NULO_STR); break;

    }
}

void geraRead(int tipo, int pos)
{

    // getstatic e invokespecial nao empilham nada ?
    incrementaStack(3);

    geraInstrucao(NULO, INST_NEWSCANNER, NULO, NULO, NULO_STR);
    geraInstrucao(NULO, INST_DUP, NULO, NULO, NULO_STR);
    geraInstrucao(NULO, INST_GETINPUTSTREAM, NULO, NULO, NULO_STR);
    geraInstrucao(NULO, INST_INVOKEINPUTSTR, NULO, NULO, NULO_STR);
    if (tipo == TIPO_INT)
    {
        geraInstrucao(NULO, INST_NEXTINT, NULO, NULO, NULO_STR);
        geraInstrucao(NULO, INST_ISTORE, pos, NULO, NULO_STR);
    }
    else if (tipo == TIPO_STRING)
    {
        geraInstrucao(NULO, INST_NEXTLINE, NULO, NULO, NULO_STR);
        geraInstrucao(NULO, INST_ASTORE, pos, NULO, NULO_STR);
    }

    decrementaStack(3);
}

void geraChamadaFuncao(char id[32])
{
    struct Funcao f;
    strcpy(f.id, id);
    if (buscaElemento(&listaFuncoes, &f, compara_funcao) == ERRO_ELEMENTO_NAO_ENCONTRADO)
    {
        printf("Funcao %s não declarada!\n", id);
        exit(-1);
    }
    geraInstrucao(NULO, INST_INVOKESTATIC, f.pos, NULO, f.id);
}

void geraChamadaFuncaoComPar(char id[32], unsigned nPar)
{
    struct Funcao f;
    strcpy(f.id, id);
    if (buscaElemento(&listaFuncoes, &f, compara_funcao) == ERRO_ELEMENTO_NAO_ENCONTRADO)
    {
        printf("Funcao %s não declarada!\n", id);
        exit(-1);
    }

    if (f.nParametros != nPar)
    {
        printf("Chamada de funcao %s com numero de parametros inconsistentes!\n", id);
        exit(-1);
    }

    geraInstrucao(NULO, INST_INVOKESTATIC, f.pos, NULO, f.id);
}

void geraRetorno(unsigned tipo)
{
    if (tipo == TIPO_INT)
        geraInstrucao(NULO, INST_IRETURN, NULO, NULO, NULO_STR);
    else if (tipo == TIPO_STRING)
        geraInstrucao(NULO, INST_ARETURN, NULO, NULO, NULO_STR);
    else
        geraInstrucao(NULO, INST_RETURN, NULO, NULO, NULO_STR);
}

void empilhaInteiro(int num)
{
    incrementaStack(1);

    int inst, p1;
    p1 = NULO;

    if (num >= 0 && num <= 5)
    {
        switch(num)
        {
            case 0: 
                inst = INST_ICONST0; break;
            case 1:
                inst = INST_ICONST1; break;
            case 2:
                inst = INST_ICONST2; break;
            case 3:
                inst = INST_ICONST3; break;
            case 4:
                inst = INST_ICONST4; break;
            case 5:
                inst = INST_ICONST5; break;
        }
    }
    else if (num >= -127 && num <= 127)
    {
        p1 = num;
        inst = INST_BIPUSH;
    }
    else
    {
        inst = INST_LDC;
        p1 = num;
    }

    geraInstrucao(NULO, inst, p1, NULO, NULO_STR);
}

void empilhaString(char str[256])
{
    incrementaStack(1);

    geraInstrucao(NULO, INST_LDCSTRING, NULO, NULO, str);

}

void corrigir(Lista *l, int label)
{
    unsigned u;

    while (!listaVazia(l))
    {
        removeDoInicio(l, &u);
        struct Instrucao i;
        leNaPosicao(&listaInstrucoes, &i, u);
        i.label = label;
        modificaNaPosicao(&listaInstrucoes, &i, u);
    }
}

void merge(Lista *l_dest, Lista *l1, Lista *l2)
{
    unsigned u;
    while(!listaVazia(l1))
    {
        removeDoInicio(l1, &u);
        insereNoFim(l_dest, &u);
    }


    while(!listaVazia(l2))
    {
        removeDoInicio(l2, &u);
        insereNoFim(l_dest, &u);
    }
}

void merge2(Lista *l_dest, Lista *l1)
{
    unsigned u;
    while(!listaVazia(l1))
    {
        removeDoInicio(l1, &u);
        insereNoFim(l_dest, &u);
    }
}

int novoLabel()
{
    contLabel++;
    geraInstrucao(contLabel, INST_NEWLABEL, NULO, NULO, NULO_STR);
    return contLabel;
}

void empilhaVarInt(char id[32])
{
    incrementaStack(1);

    int pos = buscaPos(id);

    geraInstrucao(NULO, INST_ILOAD, pos, NULO, NULO_STR);
}

void empilhaVarString(char id[32])
{
    incrementaStack(1);

    int pos = buscaPos(id);

    geraInstrucao(NULO, INST_ALOAD, pos, NULO, NULO_STR);
}

void getPrint() 
{
    // getstatic java/lang/System/out Ljava/io/PrintStream;
    // empilha algo ?
    incrementaStack(1);

    geraInstrucao(NULO, INST_GETPRINT, NULO, NULO, NULO_STR);
}

void invokeStatic(int TIPO)
{
    // CONFERIR ESSE VALOR!!!!
    // Desempilha PrintStream e variavel
    decrementaStack(2);

    if (TIPO == TIPO_INT) 
        geraInstrucao(NULO, INST_INVOKEPRINT_INT, NULO, NULO, NULO_STR);
    else if (TIPO == TIPO_STRING)
        geraInstrucao(NULO, INST_INVOKEPRINT_STR, NULO, NULO, NULO_STR);
}


unsigned buscaTipo(char id[32])
{
    struct Simbolo s;
    strcpy(s.id, id);

    if (buscaElemento(&tabelaSimbolos, &s, compara_simbolos) == ERRO_ELEMENTO_NAO_ENCONTRADO)
    {
        printf("Funcao com id %s nao foi declarada!\n", id);
        exit(-1);
    }

    return s.tipo;
}

unsigned buscaPos(char id[32])
{
    struct Simbolo s;
    strcpy(s.id, id);

    if (buscaElemento(&tabelaSimbolos, &s, compara_simbolos) == ERRO_ELEMENTO_NAO_ENCONTRADO)
    {
        printf("Variavel com id %s nao foi declarada!\n", id);
        exit(-1);
    }

    return s.pos;
}

unsigned buscaTipoFuncao(char id[32])
{
    struct Funcao f;
    strcpy(f.id, id);

    if (buscaElemento(&listaFuncoes, &f, compara_simbolos) == ERRO_ELEMENTO_NAO_ENCONTRADO)
    {
        printf("Variavel com id %s nao foi declarada!\n", id);
        exit(-1);
    }

    return f.tipoRetorno; 
}

void geraArquivoFinal()
{
    struct Instrucao i;

    char nomeArquivoFinal[250];
    strcpy(nomeArquivoFinal, nomeClasse);
    strcat(nomeArquivoFinal, ".jout");
    FILE *fp = fopen(nomeArquivoFinal, "w");

    // Variaveis de controle
    int estouMain = 0;
    int sairInstrucoes = 0;
    int contadorFuncao = 0;

    fprintf(fp, ".class public %s\n"
                ".super java/lang/Object\n\n"
                ".method public <init>()V\n"
                "\taload_0\n\n"
                "\tinvokenonvirtual java/lang/Object/<init>()V\n"
                "\treturn\n"
                ".end method\n\n", nomeClasse);

    while(!estouMain)
    {
        struct Funcao f;
        if (contadorFuncao == contFuncao)
        {
            fprintf(fp, ".method public static main([Ljava/lang/String;)V\n");
            estouMain = 1;
        }
        else
        {   
            leNaPosicao(&listaFuncoes, &f, contadorFuncao);
            contadorFuncao++;
            fprintf(fp, ".method public static %s(", f.id);
            struct Parametro p;
            int contAux = 0;
            for (contAux; contAux < f.nParametros; contAux++)
            {
                leNaPosicao(&(f.listaParametros), &p, contAux);
                if (p.tipo == TIPO_INT)
                    fprintf(fp, "I");
                else if (p.tipo == TIPO_STRING)
                    fprintf(fp, "Ljava/lang/String;");
                else if (p.tipo == TIPO_VOID)
                    fprintf(fp, "V");
            }
            fprintf(fp, ")");
            if (f.tipoRetorno == TIPO_INT)
                fprintf(fp, "I");
            else if (f.tipoRetorno == TIPO_STRING)
                fprintf(fp, "Ljava/lang/String;");
            else if (f.tipoRetorno == TIPO_VOID)
                fprintf(fp, "V");
            fprintf(fp, "\n");

        }

        fprintf(fp, "\t.limit stack %d\n", maiorStack);
        fprintf(fp, "\t.limit locals %d\n", contPos);

        while (!listaVazia(&listaInstrucoes))
        {
            removeDoInicio(&listaInstrucoes, &i);

            //printf("%u %d %d %d\n", i.label, i.inst, i.p1, i.p2);
            switch(i.inst)
            {

                case INST_IDIV:
                    fprintf(fp, "\tidiv\n"); break;
                case INST_IMUL:
                    fprintf(fp, "\timul\n"); break;
                case INST_IADD:
                    fprintf(fp, "\tiadd\n"); break;
                case INST_ISUB:
                    fprintf(fp, "\tisub\n"); break;
                case INST_ICONST0:
                    fprintf(fp, "\ticonst_0\n"); break;
                case INST_ICONST1:
                    fprintf(fp, "\ticonst_1\n"); break;
                case INST_ICONST2:
                    fprintf(fp, "\ticonst_2\n"); break;
                case INST_ICONST3:
                    fprintf(fp, "\ticonst_3\n"); break;
                case INST_ICONST4:
                    fprintf(fp, "\ticonst_4\n"); break;
                case INST_ICONST5:
                    fprintf(fp, "\ticonst_5\n"); break;
                case INST_BIPUSH:
                    fprintf(fp, "\tbipush %d\n", i.p1); break;
                case INST_ILOAD:
                    fprintf(fp, "\tiload %d\n", i.p1); break;
                case INST_ISTORE:
                    fprintf(fp, "\tistore %d\n", i.p1); break;
                case INST_GETPRINT:
                    fprintf(fp, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
                    break;
                case INST_INVOKEPRINT_INT:
                    fprintf(fp, "\tinvokevirtual java/io/PrintStream/println(I)V\n");
                    break;
                case INST_INVOKEPRINT_STR:
                    fprintf(fp, "\tinvokevirtual java/io/PrintStream/println(Ljava/lang/String;)V\n");
                    break;
                case INST_IF_ICMPEQ:
                    fprintf(fp, "\tif_icmpeq l%u\n", i.label); break;
                case INST_IF_ICMPNE:
                    fprintf(fp, "\tif_icmpne l%u\n", i.label); break;
                case INST_IF_ICMPLT:
                    fprintf(fp, "\tif_icmplt l%u\n", i.label); break;
                case INST_IF_ICMPLE:
                    fprintf(fp, "\tif_icmple l%u\n", i.label); break;
                case INST_IF_ICMPGT:
                    fprintf(fp, "\tif_icmpgt l%u\n", i.label); break;
                case INST_IF_ICMPGE:
                    fprintf(fp, "\tif_icmpge l%u\n", i.label); break;
                case INST_GOTO:
                    fprintf(fp, "\tgoto l%u\n", i.label); break;
                case INST_NEWLABEL:
                    fprintf(fp, "l%u:\n", i.label); break;
                case INST_LDC:
                    fprintf(fp, "\tldc %d\n", i.p1); break;
                case INST_DUP:
                    fprintf(fp, "\tdup\n"); break;
                case INST_NEWSCANNER:
                    fprintf(fp, "\tnew java/util/Scanner\n"); break;
                case INST_GETINPUTSTREAM:
                    fprintf(fp, "\tgetstatic java/lang/System/in Ljava/io/InputStream;\n"); break;
                case INST_INVOKEINPUTSTR:
                    fprintf(fp, "\tinvokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V\n"); break;
                case INST_NEXTINT:
                    fprintf(fp, "\tinvokevirtual java/util/Scanner/nextInt()I\n"); break;
                case INST_LDCSTRING:
                    fprintf(fp, "\tldc %s\n", i.literal); break;
                case INST_ASTORE:
                    fprintf(fp, "\tastore %d\n", i.p1); break;
                case INST_ALOAD:
                    fprintf(fp, "\taload %d\n", i.p1); break;
                case INST_NEXTLINE:
                    fprintf(fp, "\tinvokevirtual java/util/Scanner/nextLine()Ljava/lang/String;\n"); break;
                case INST_DELIMITADORFUNC:
                    sairInstrucoes = 1; break;
                case INST_INVOKESTATIC:
                    fprintf(fp, "\tinvokestatic %s/%s(", nomeClasse, i.literal);
                    struct Funcao f;
                    leNaPosicao(&listaFuncoes, &f, i.p1);
                    int cont = 0;
                    for (cont = 0; cont < f.nParametros; cont++)
                    {
                        struct Parametro p;
                        leNaPosicao(&(f.listaParametros), &p, cont);
                        if (p.tipo == TIPO_INT)
                            fprintf(fp, "I");
                        else if (p.tipo == TIPO_STRING)
                            fprintf(fp, "V");
                    }
                    fprintf(fp, ")");
                    if (f.tipoRetorno == TIPO_INT)
                        fprintf(fp, "I");
                    else if (f.tipoRetorno == TIPO_STRING)
                        fprintf(fp, "Ljava/lang/String;");
                    else if (f.tipoRetorno == TIPO_VOID)
                        fprintf(fp, "V");
                    fprintf(fp, "\n");
                    break;
                case INST_IRETURN:
                    fprintf(fp, "\tireturn\n"); break;
                case INST_ARETURN:
                    fprintf(fp, "\tareturn\n"); break;
                case INST_RETURN:
                    fprintf(fp, "\treturn\n"); break;

                default:
                    printf("ERRO! INSTRUCAO %d NAO DEFINIDA!\n", i.inst);
                    break;
            }
            if (sairInstrucoes) {
                sairInstrucoes = 0;
                break;
            }
        }

        fprintf(fp, "\treturn\n");
        fprintf(fp, ".end method\n\n");
    }
    fclose(fp);
}

void error()
{
    printf("ERRO!\n");
    exit(-1);
}

void incrementaStack(int i)
{
    //printf("INCREMENTOU %d\n", i);
    contStackSize += i;
    if (contStackSize > maiorStack)
        maiorStack = contStackSize;
}

void decrementaStack(int i)
{
    //printf("DECREMENTOU %d\n", i);
    contStackSize -= i;
    if (contStackSize < 0)
    {
        printf("INCONSISTENCIA NA REDUCAO DO STACK! TENTOU REDUZIR %d\n", i);
        exit(-1);
    }
}