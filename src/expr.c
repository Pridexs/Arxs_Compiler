#include <stdio.h>

extern FILE *yyin;

extern char nomeClasse[200];
extern void inicializa();

void copiarNomeClasse(char *nomeArquivo);

int main(int argc, char **argv)
{
    inicializa();
    
    if (argc < 2)
    {
        printf("Nenhum arquivo especificado.\n");
        return 0;
    }

    copiarNomeClasse(argv[1]);

    FILE *arq = fopen(argv[1], "r");
    if (arq == NULL) {
        printf("Erro ao abrir arquivo\n");
        return -1;
    }

    yyin = arq;
    yyparse();
    fclose(arq);
    return 0;
}

void copiarNomeClasse(char *nomeArquivo)
{
    int i = 0;

    while (nomeArquivo[i] != '.')
    {
        nomeClasse[i] = nomeArquivo[i];
        i++;

        if (i == 199)
            break;
    }
    nomeClasse[i] = '\0';
}
