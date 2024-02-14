//Sintese
//Objetivo:Cadastrar,listar,editar,pesquisar e excluir cursos.
//Entrada: Nome do curso,codigo (cic), sigla da escola e modalidade.
//Saida: Lista de cursos cadastrados.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <conio.h>
#include <ctype.h>

#define TAMANHO 100
#define MINCIC 10
#define MAXIMO 5

#define NOME_ARQUIVO "cursos.bin"

typedef struct {
    char nomeCurso[TAMANHO], siglaEscola[MAXIMO];
    char modalidade;
    int cic;
} dadosCurso;

// PROTOTIPOS

//FUNCIONALIDADES
void cadastraCurso();
void editaCursos(char resp);
void listarCursos();
void pesquisaModalidade();
void deletaCursos(FILE **arquivo);
void removeCursoEspecifico(FILE *arquivo, int cic);

//VALIDAÇÕES
char *validaNome(char nome[]);
char *validaSigla(char sigla[]);
int recebeCIC();
char validaModalidade(char modalidade);
char validaResp(char resp);
int validaOrdem(int ordem);

//ORDENAÇÕES E CONVERSÕES
char *modalidadePorExtenso(char modalidade);
int comparaCursos(const void *a, const void *b);
int comparaCursosSigla(const void *a, const void *b);

int main() {
    FILE *arquivo;
    int opcao,ordem;
    char resp;

    setlocale(LC_ALL, "Portuguese");

    // VERIFICA SE JA EXISTE ARQUIVO SE EXISTIR UTILIZA ARQUIVO
    if ((arquivo = fopen(NOME_ARQUIVO, "rb+")) == NULL) {
        // CASO NÃO EXISTA CRIA.
        if ((arquivo = fopen(NOME_ARQUIVO, "wb+")) == NULL) {
            //MENSAGEM DE ERRO CASO NÃO CONSIGA ABRIR O ARQUIVO
            fprintf(stderr, "Erro ao criar o arquivo %s.\n", NOME_ARQUIVO);
            //EXIT FAIULURES É A MESMA COISA DE 1.
            exit(EXIT_FAILURE);
        }
    }

    //FECHA ARQUIVO
    fclose(arquivo);

    //MENU
    do {
        puts("\n----------------------------------------------------------------------");
        puts("                        SISTEMA DE CADASTRO DE CURSOS                  ");
        puts("                               SEJA BEM VINDO !!                       ");
        puts("\n----------------------------------------------------------------------");
        puts("1. Cadastrar novo Curso");
        puts("2. Editar dados de Curso cadastrado");
        puts("3. Listar todos os Cursos cadastrados");
        puts("4. Pesquisar os Cursos cadastrados de uma Única modalidade específica");
        puts("5. Apagar arquivo com todos os cadastros de Curso");
        puts("0. Terminar o programa\n");
        puts("----------------------------------------------------------------------");
        printf("Escolha uma opcao: ");

        scanf("%d", &opcao);
        fflush(stdin);

        system("CLS");

        switch (opcao) {

        case 1:
            //CADASTRO
            puts("\n----------------------------------------------------------------------");
            puts("                           CADASTRO DE CURSOS                      ");
            puts("----------------------------------------------------------------------");
            cadastraCurso();
            break;

        case 2:
            //EDIÇÃO
            puts("\n------------------------------------------------------------------------------");
            puts("                                 EDICAO DE CURSOS                      ");
            puts("------------------------------------------------------------------------------");
            editaCursos(resp);
            break;

        case 3:
            //LISTAGEM
            puts("\n----------------------------------------------------------------------");
            puts("                          CURSO(S) CADASTRADOS             ");
            puts("----------------------------------------------------------------------");
            printf("\n");
            listarCursos();
            break;

        case 4:
            //PESQUISA
            puts("\n----------------------------------------------------------------------");
            puts("                           PESQUISA DE CURSOS                      ");
            puts("----------------------------------------------------------------------");
            pesquisaModalidade();
            break;

        case 5:
            //DELEÇÃO
            puts("Confirma que deseja excluir todos os cursos ? S/N");
            printf("Resposta:");
            resp = toupper(getche());
            resp = validaResp(resp);

            if(resp == 'S') {
                FILE *arquivo = fopen(NOME_ARQUIVO, "rb+");
                deletaCursos(&arquivo);
            } else {
                puts("\nCANCELADA REMOCAO!!");
            }

            break;

        case 0:
            //ENCERRAMENTO
            puts("Programa encerrado.");
            break;

        default:
            puts("Opção inválida. Tente novamente.");
        }
    } while (opcao != 0);

    return 0;
}

// ==================== SUBPROGRAMAS ==================

// Sintese
// Objetivo:Cadastrar cursos.
// Parametros:.
// Retorno: Sem retorno.
void cadastraCurso() {

    FILE *arquivo;
    dadosCurso novoCurso;

    // VERIFICA SE EXISTE O ARQUIVO
    if ((arquivo = fopen(NOME_ARQUIVO, "ab")) == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", NOME_ARQUIVO);
        exit(EXIT_FAILURE);
    }

    fseek(arquivo, SEEK_SET, SEEK_END);

    //RECEBE E VALIDA NOME
    printf("Digite o nome do curso: ");
    fgets(novoCurso.nomeCurso, TAMANHO, stdin);
    novoCurso.nomeCurso[strcspn(novoCurso.nomeCurso, "\n")] = '\0';
    strcpy(novoCurso.nomeCurso,validaNome(novoCurso.nomeCurso));
    printf("\n");
    fflush(stdin);

    //RECEBE E VALIDA SIGLA DA ESCOLA
    printf("Digite a sigla da escola: ");
    fgets(novoCurso.siglaEscola, MAXIMO, stdin);
    novoCurso.siglaEscola[strcspn(novoCurso.siglaEscola, "\n")] = '\0';
    strcpy(novoCurso.siglaEscola,validaSigla(novoCurso.siglaEscola));
    printf("\n");
    fflush(stdin);

    //RECEBE E VALIDA CIC
    novoCurso.cic=recebeCIC();
    printf("\n");
    fflush(stdin);

    //RECEBE MODALIDADE E VALIDA MODALIDADE
    puts("P. Presencial  S. Semipresencial  D. Distancia ");
    printf("Digite a Modalidade (P/S/D): ");
    novoCurso.modalidade = toupper(getche());
    novoCurso.modalidade = validaModalidade(novoCurso.modalidade);
    fflush(stdin);

    //ESCREVA DADOS NO ARQUIVO
    fwrite(&novoCurso, sizeof(dadosCurso), 1, arquivo);
    fclose(arquivo);

    printf("\n\nCurso Cadastrado com Sucesso.\n\n");
}

//Objetivo:Editar dado do curso.
//Parametros: resposta.
//Retorno:Sem retorno.
void editaCursos(char resp) {
    FILE *arquivo;
    dadosCurso curso;
    int cicEdicao, dadoEdicao, cursoEncontrado = 0;

    // VERIFICA SE JA TEM ESSE ARQUIVO PARA ABRILO EM MODO DE LEITURA
    if ((arquivo = fopen(NOME_ARQUIVO, "rb+")) == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", NOME_ARQUIVO);
        exit(EXIT_FAILURE);
    }

    // VERIFICA SE HÁ CURSOS CADASTRADOS
    fseek(arquivo, SEEK_SET, SEEK_END);

    if (ftell(arquivo) == 0) {
        fclose(arquivo);
        printf("Nenhum curso cadastrado para edição.\n");
    } else {
        listarCursos();
        fseek(arquivo, SEEK_SET, SEEK_SET);

        // PERGUNTA CIC A SER EDITADO
        printf("\nDigite o CIC do curso que deseja editar: ");
        scanf("%d", &cicEdicao);

        // BUSCA CIC INFORMADO
        while (fread(&curso, sizeof(dadosCurso), 1, arquivo) == 1 && !cursoEncontrado) {
            cursoEncontrado = (curso.cic == cicEdicao);
        }

        // VERIFICA SE CURSO FOI ENCONTRADO
        if (!cursoEncontrado) {
            printf("\nCurso com CIC %d não encontrado.\n", cicEdicao);
            fclose(arquivo);
        } else {

            // PERGUNTA QUAL DADO USUARIO DESEJA ALTERAR
            printf("\nDigite o número correspondente ao dado que deseja alterar:\n");
            puts("Caso deseje alterar o CIC deve excluir o curso.\n");
            puts("1. Nome");
            puts("2. Sigla");
            puts("3. Modalidade");
            puts("4. Deletar Curso\n");
            printf("Escolha:");
            scanf("%d", &dadoEdicao);
            fflush(stdin);
            system("cls");

            switch (dadoEdicao) {
            case 1:
                // EDITA NOME
                puts("--------------------------------");
                puts("-------- EDIÇÃO DE NOME --------\n");
                puts("--------------------------------\n");
                printf("Nome Antigo: %s\n\n", curso.nomeCurso);
                printf("Digite o novo nome: ");
                fgets(curso.nomeCurso, TAMANHO, stdin);
                curso.nomeCurso[strcspn(curso.nomeCurso, "\n")] = '\0';
                strcpy(curso.nomeCurso, validaNome(curso.nomeCurso));
                fflush(stdin);
                system("cls");
                break;

            case 2:
                // EDITA SIGLA
                puts("---------------------------------");
                puts("-------- EDIÇÃO DE SIGLA --------");
                puts("--------------------------------\n");
                printf("Sigla Antiga: %s\n\n", curso.siglaEscola);
                printf("Digite a nova sigla: ");
                fgets(curso.siglaEscola, MAXIMO, stdin);
                curso.siglaEscola[strcspn(curso.siglaEscola, "\n")] = '\0';
                strcpy(curso.siglaEscola, validaSigla(curso.siglaEscola));
                fflush(stdin);
                system("cls");
                break;

            case 3:
                // EDITA MODALIDADE
                puts("-------------------------------------");
                puts("-------- EDIÇÃO DE MODALIDADE --------");
                puts("--------------------------------\n");
                printf("Modalidade Antiga: %c\n\n", curso.modalidade);
                puts("P. Presencial  S. Semipresencial  D. Distancia ");
                printf("Digite a nova Modalidade (P/S/D): ");
                curso.modalidade = toupper(getche());
                curso.modalidade = validaModalidade(curso.modalidade);
                fflush(stdin);
                system("cls");
                break;
            case 4:
                printf("Confirma que deseja excluir o curso com CIC %d ? S/N \n", cicEdicao);
                printf("Resposta:");
                resp = toupper(getche());
                resp = validaResp(resp);

                removeCursoEspecifico(arquivo, cicEdicao);
                break;

            default:
                puts("\nOpção inválida. Tente novamente.");

            }

            printf("\nAlteração concluída com sucesso.\n\n");

            // VOLTA AO INICIO DO REGISTRO ENCONTRADO
            fseek(arquivo, -sizeof(dadosCurso), SEEK_CUR);

            // ESCREVE DADOS ALTERADOS
            fwrite(&curso, sizeof(dadosCurso), 1, arquivo);
            fclose(arquivo);

        }
    }
}

// Objetivo:Listar cursos.
// Parametros:
// Retorno: sem retorno.
void listarCursos() {

    FILE *arquivo;
    int totalCursos = 0, ordem,cont;

    //VERIFICA SE EXISTE ARQUIVO
    if ((arquivo = fopen(NOME_ARQUIVO, "rb+")) == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", NOME_ARQUIVO);
        exit(EXIT_FAILURE);
    }

    // ARMAZENA OS CURSOS DE FORMA TEMPORARIA PARA LISTAGEM
    dadosCurso cursos[TAMANHO];

    fseek(arquivo, SEEK_SET, SEEK_SET);

    //LE TODOS OS CURSOS CADASTRADOS
    while (fread(&cursos[totalCursos], sizeof(dadosCurso), 1, arquivo) == 1) {
        totalCursos++;
    }

    //CASO NÃO EXISTA NENHUM APRESENTA MENSAGEM
    if (totalCursos == 0) {
        printf("\nNão existem cursos cadastrados.\n\n");
    } else {
        //PERGUNTA QUAL A ORDEM DE LISTAGEM
        printf("1.Crescente 2.Decrescente\n");
        printf("Escolha a ordem de listagem: ");
        scanf("%d", &ordem);
        ordem = validaOrdem(ordem);
        printf("\n");

        //ORDENA OS CURSOS EM ORDEM CRESCENTE
        qsort(cursos, totalCursos, sizeof(dadosCurso), comparaCursos);

        //INVRTE ORDEM DE LISTAGEM CASO SEJA ESCOLHIDO A ORDEM DESCRESCENTE
        if (ordem == 2) {
            for (cont = 0; cont < totalCursos / 2; cont++) {
                dadosCurso temp = cursos[cont];
                cursos[cont] = cursos[totalCursos - cont - 1];
                cursos[totalCursos - cont - 1] = temp;
            }
        }

        //CRIA CAMPOS DA TABELA
        printf("| %-30s | %-10s | %-3s | %-15s |\n", "Nome do Curso", "Sigla", "CIC", "Modalidade");
        printf("|--------------------------------|------------|-----|-----------------|\n");

        //DADOS DOS CURSOS
        for (cont = 0; cont < totalCursos; cont++) {
            printf("| %-30s | %-10s | %-3d | %-15s |\n", cursos[cont].nomeCurso, cursos[cont].siglaEscola, cursos[cont].cic, modalidadePorExtenso(cursos[cont].modalidade));
        }
    }

    fclose(arquivo);
}

//Objetivo: Pesquisar cursos pela modalidade.
//Parametros: dados do arquivo.
//Retorno: sem retorno.
void pesquisaModalidade() {

    FILE *arquivo;
    dadosCurso cursos[TAMANHO];
    dadosCurso curso;
    char modalidadePesquisa;
    int encontrados = 0, cont = 0, aux;

    //VERIFICA SE EXISTE ARQUIVO
    if ((arquivo = fopen(NOME_ARQUIVO, "rb+")) == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", NOME_ARQUIVO);
        exit(EXIT_FAILURE);
    }

    // PERGUNTA MODALIDADE QUE DESEJA PESQUISAR
    puts("P. Presencial S. Semipresencial D. Distancia ");
    printf("Digite a modalidade que deseja pesquisar: ");
    modalidadePesquisa = toupper(getche());
    modalidadePesquisa = validaModalidade(modalidadePesquisa);

    // POSICIONA CURSOR NO INICIO DO ARQUIVO
    fseek(arquivo, SEEK_SET, SEEK_SET);

    // LE CURSOS E ARMAZENA OS QUE ATENDEM A MODALIDADE DIGITADA
    while (fread(&curso, sizeof(dadosCurso), 1, arquivo) == 1) {
        if (toupper(curso.modalidade) == modalidadePesquisa) {
            cursos[cont] = curso;
            cont++;
            encontrados++;
        }
    }

    if (encontrados == 0) {
        puts("\n\nNenhum curso encontrado com a modalidade especificada.\n\n");
    } else {

        printf("\n\n");

        // ORDENA PELA SIGLA DA ESCOLA
        qsort(cursos, encontrados, sizeof(dadosCurso), comparaCursosSigla);

        // CAMPOS DA TABELA
        printf("| %-30s | %-10s | %-3s | %-15s |\n", "Nome do Curso", "Sigla", "CIC", "Modalidade");
        printf("|--------------------------------|------------|-----|-----------------|\n");

        // DADOS DOS CURSOS
        for (aux = 0; aux < encontrados; aux++) {
            printf("| %-30s | %-10s | %-3d | %-15s |\n", cursos[aux].nomeCurso, cursos[aux].siglaEscola, cursos[aux].cic, modalidadePorExtenso(cursos[aux].modalidade));
        }
    }

    printf("\n");

    fclose(arquivo);
}

//Objetivo:Deletar cursos.
//Parametros: Arquivo.
//Retorno: sem retorno.
void deletaCursos(FILE **arquivo) {
    fclose(*arquivo);

    fflush(stdin);
    if (remove(NOME_ARQUIVO) != 0) {
        fprintf(stderr, "\nErro ao remover o arquivo %s.\n", NOME_ARQUIVO);
    } else {
        printf("\nTodos os cursos foram removidos.\n");
    }

        if ((*arquivo = fopen(NOME_ARQUIVO, "wb+")) == NULL) {
            fprintf(stderr, "Erro ao criar o arquivo %s.\n", NOME_ARQUIVO);
            exit(EXIT_FAILURE);
        }

    //FECHA ARQUIVO
    fclose(*arquivo);
}

//Objetivo:Excluir curos especifico.
//Parametros: arquivo e cic.
//Retorno: sem retorno.
void removeCursoEspecifico(FILE *arquivo, int cic) {
    FILE *temp;
    dadosCurso curso;
    int contador = 0;

    // CRIA ARQUIVO TEMPORÁRIO
    temp = fopen("temp.bin", "wb");
    if (temp == NULL) {
        fprintf(stderr, "Erro ao criar arquivo temporário.\n");
        exit(EXIT_FAILURE);
    }

    // POSICIONA CURSOR NO INÍCIO
    fseek(arquivo, SEEK_SET, SEEK_SET);

    // LÊ E GRAVA CURSO NO ARQUIVO TEMPORÁRIO, EXCETO O QUE SERÁ EXCLUÍDO
    while (fread(&curso, sizeof(dadosCurso), 1, arquivo) == 1) {
        if (curso.cic != cic) {
            fwrite(&curso, sizeof(dadosCurso), 1, temp);
            contador++;
        }
    }

    // FECHA O ARQUIVO TEMPORÁRIO
    fclose(temp);
    fclose(arquivo);

    // EXCLUI O ARQUIVO ORIGINAL COM O CURSO A SER EXCLUÍDO
    if (remove(NOME_ARQUIVO) != 0) {
        fprintf(stderr, "\nErro ao remover o arquivo %s.\n", NOME_ARQUIVO);
        exit(EXIT_FAILURE);
    }

    // RENOMEIA TEMP PARA ORIGINAL
    if (rename("temp.bin", NOME_ARQUIVO) != 0) {
        fprintf(stderr, "\nErro ao renomear o arquivo.\n");
        exit(EXIT_FAILURE);
    }

    // REABRE O ARQUIVO PARA CONTINUAR COM A EXECUÇÃO DO PROGRAMA
    arquivo = fopen(NOME_ARQUIVO, "rb+");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao reabrir o arquivo %s.\n", NOME_ARQUIVO);
        exit(EXIT_FAILURE);
    }

    // APRESENTA CURSOS RESTANTES
    puts("\n----------------------------------------------------------------------");
    puts("                          CURSO(S) RESTANTES             ");
    puts("----------------------------------------------------------------------");
    fseek(arquivo, 0, SEEK_SET);
    while (fread(&curso, sizeof(dadosCurso), 1, arquivo) == 1) {
        printf("| %-30s | %-10s | %-3d | %-15s |\n", curso.nomeCurso, curso.siglaEscola, curso.cic, modalidadePorExtenso(curso.modalidade));
    }

    // FECHA O ARQUIVO APÓS A LISTAGEM
    fclose(arquivo);
}


// Objetivo: validar nomes
// Parametros: nome
// Retorno: nome valido
//Retorna um ponteiro para um caracter char
char *validaNome(char nome[]) {
    nome[strcspn(nome, "\n")] = '\0';
    while (strlen(nome) < MAXIMO || strlen(nome) == 0) {
        puts("\nNome Inválido!\n");
        printf("Nome deve ser preenchido e ter no minimo %d caracteres: ", MAXIMO);
        fgets(nome, TAMANHO, stdin);
        nome[strcspn(nome, "\n")] = '\0';
    }
    return nome;
}

// Objetivo: Validar Sigla.
// Parametros: Sigla.
// Retorno: Sigla valida.
char *validaSigla(char sigla[]) {
    sigla[strcspn(sigla, "\n")] = '\0';
    while (strlen(sigla) > MAXIMO || strlen(sigla) == 0) {
        puts("\nSigla Inválida!\n");
        printf("Sigla deve ser preenchido e ter no maximo %d caracteres: ", MAXIMO);
        fgets(sigla, MAXIMO, stdin);
        sigla[strcspn(sigla, "\n")] = '\0';
    }

    return sigla;
}

//Objetivo: Validar CIC.
//Parametros: cic.
//Retorno: cic valida.
int recebeCIC() {
    int cic, cicInvalido;
    FILE *arquivo;
    dadosCurso tempCurso;

    do {
        printf("Digite o CIC do curso (maior que 10 e não repetido): ");

        // GARANTIR QUE SEJA UM NÚMERO INTEIRO
        while (scanf("%d", &cic) != 1 || cic <= MINCIC) {
            fflush(stdin);
            puts("\nCIC inválido. Digite novamente.");
            printf("Digite o CIC do curso (maior que 10 e não repetido): ");
        }

        // ABRIR O ARQUIVO
        arquivo = fopen(NOME_ARQUIVO, "rb");
        if (arquivo == NULL) {
            fprintf(stderr, "Erro ao abrir o arquivo %s.\n", NOME_ARQUIVO);
            exit(EXIT_FAILURE);
        }

        fseek(arquivo, SEEK_SET, SEEK_SET);

        cicInvalido = 0;

        // LER O ARQUIVO E VERIFICAR SE O CIC JÁ EXISTE
        while (fread(&tempCurso, sizeof(dadosCurso), 1, arquivo) == 1 && !cicInvalido) {
            cicInvalido = (tempCurso.cic == cic);
        }

        // FECHAR O ARQUIVO
        fclose(arquivo);

        // APRESENTAR MENSAGEM SE O CIC JÁ EXISTE
        if (cicInvalido) {
            puts("\nCIC já existe. Digite um valor único.");
        }

        // APRESENTAR MENSAGEM SE O CIC É MENOR OU IGUAL A MINCIC
        if (cic <= MINCIC && !cicInvalido) {
            puts("\nCIC menor ou igual a 10. Digite um valor maior que 10.");
        }

    } while (cicInvalido || cic <= MINCIC);

    return cic;
}


// Objetivo:Validar modalidade do curso.
// Parametros:modalidade.
// Retorno: modalidade valida.
char validaModalidade(char modalidade) {
    while (modalidade != 'P' && modalidade != 'S' && modalidade != 'D') {
        printf("Campo Inválido, Digite novamente: ");
        modalidade = toupper(getche());
        fflush(stdin);
    }
    return modalidade;
}

//Objetivo: Validar resposta.
//Parametros: resposta.
//Retorno: resposta valida.
char validaResp(char resp) {
    while(resp != 'S' && resp != 'N') {
        puts("\nResposta Invalida,digite novamente: S/N \n");
        resp = toupper(getche());
    }
    return resp;
}

//Objetivo: Converter o caracter da modalidade para a expressao.
//Parametros: caracter da modalidade.
//Retorno: expressão da modalidade do curso.
char *modalidadePorExtenso(char modalidade) {
    switch (modalidade) {
    case 'P':
        return "Presencial";
    case 'S':
        return "Semipresencial";
    case 'D':
        return "Distância";
    default:
        return "Desconhecida";
    }
}

//Objetivo: Validar ordem de Listagem.
//Parametros: Ordem.
//Retorno: Ordem Valida
int validaOrdem(int ordem) {
    while(ordem != 1 && ordem != 2) {
        printf("\nOrdem de Listagem Invalida,digite novamente: ");
        scanf("%d",&ordem);
    }
    return ordem;
}

//Obetivo: Ordenar cursos em ordem crescente pelo nome do curso.
//Parametros: 2 cusos.
//Retorno: 1(crescente).
int comparaCursos(const void *c1, const void *c2) {
    const dadosCurso *cursoA = (const dadosCurso *)c1;
    const dadosCurso *cursoB = (const dadosCurso *)c2;
    return strcmp(cursoA->nomeCurso, cursoB->nomeCurso);
}

//Obetivo: Ordenar cursos em ordem crescente pela sigla.
//Parametros: 2 cursos.
//Retorno: 1(crescente).
int comparaCursosSigla(const void *a, const void *b) {
    const dadosCurso *cursoA = (const dadosCurso *)a;
    const dadosCurso *cursoB = (const dadosCurso *)b;
    return strcmp(cursoA->siglaEscola, cursoB->siglaEscola);
}
