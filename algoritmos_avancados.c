#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ==============================
// Estruturas de Dados
// ==============================

// Estrutura da sala da mansão (nó da árvore binária)
typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// Estrutura da árvore BST de pistas
typedef struct PistaNode {
    char pista[100];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// ==============================
// Funções relacionadas às salas
// ==============================

/*
 * Função: criarSala
 * -----------------
 * Cria dinamicamente uma nova sala com um nome e (opcionalmente) uma pista.
 */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memória para a sala.\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// ==============================
// Funções relacionadas às pistas (BST)
// ==============================

/*
 * Função: criarNoPista
 * --------------------
 * Cria um novo nó da árvore de pistas.
 */
PistaNode* criarNoPista(const char *pista) {
    PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
    if (novo == NULL) {
        printf("Erro ao alocar memória para a pista.\n");
        exit(1);
    }
    strcpy(novo->pista, pista);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

/*
 * Função: inserirPista
 * --------------------
 * Insere uma pista na árvore de busca (BST), mantendo a ordem alfabética.
 */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) return criarNoPista(pista);

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);

    return raiz;
}

/*
 * Função: exibirPistas
 * --------------------
 * Exibe as pistas em ordem alfabética (percorrendo a BST em ordem simétrica).
 */
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf("🔎 %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}

// ==============================
// Função de exploração
// ==============================

/*
 * Função: explorarSalasComPistas
 * ------------------------------
 * Permite ao jogador explorar a mansão, coletando automaticamente as pistas
 * encontradas e adicionando-as à árvore BST de pistas.
 */
void explorarSalasComPistas(Sala *atual, PistaNode **pistas) {
    char opcao;

    while (1) {
        printf("\n📍 Você está em: %s\n", atual->nome);

        // Se houver uma pista, coletá-la
        if (strlen(atual->pista) > 0) {
            printf("🧩 Você encontrou uma pista: \"%s\"\n", atual->pista);
            *pistas = inserirPista(*pistas, atual->pista);
        } else {
            printf("Nada interessante aqui...\n");
        }

        printf("\nEscolha o próximo caminho:\n");
        if (atual->esquerda != NULL)
            printf("  [e] Ir para a esquerda (%s)\n", atual->esquerda->nome);
        if (atual->direita != NULL)
            printf("  [d] Ir para a direita (%s)\n", atual->direita->nome);
        printf("  [s] Sair da mansão\n");

        printf("Opção: ");
        scanf(" %c", &opcao);

        if (opcao == 'e' && atual->esquerda != NULL) {
            atual = atual->esquerda;
        } else if (opcao == 'd' && atual->direita != NULL) {
            atual = atual->direita;
        } else if (opcao == 's') {
            printf("\nVocê decidiu encerrar a exploração.\n");
            break;
        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    }
}

// ==============================
// Função principal (main)
// ==============================

int main() {
    // ----- Criação do mapa da mansão -----
    Sala *hall = criarSala("Hall de Entrada", "Um pedaço rasgado de um mapa antigo");
    Sala *salaEstar = criarSala("Sala de Estar", "");
    Sala *cozinha = criarSala("Cozinha", "Marcas de pegadas com lama");
    Sala *biblioteca = criarSala("Biblioteca", "Um livro faltando na estante");
    Sala *jardim = criarSala("Jardim", "Um lenço com iniciais misteriosas");
    Sala *porao = criarSala("Porão", "Um cofre trancado e empoeirado");

    // ----- Montagem da árvore de salas -----
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->direita = porao;

    // ----- Criação da árvore de pistas -----
    PistaNode *pistasColetadas = NULL;

    // ----- Início da exploração -----
    printf("=== Detective Quest: Coleta de Pistas ===\n");
    explorarSalasComPistas(hall, &pistasColetadas);

    // ----- Exibição das pistas -----
    printf("\n===== Pistas Coletadas (em ordem alfabética) =====\n");
    if (pistasColetadas == NULL)
        printf("Nenhuma pista foi coletada!\n");
    else
        exibirPistas(pistasColetadas);

    // ----- Liberação de memória -----
    free(biblioteca);
    free(jardim);
    free(salaEstar);
    free(porao);
    free(cozinha);
    free(hall);
    // (liberar a BST de pistas é opcional para este nível)

    printf("\nObrigado por jogar Detective Quest!\n");
    return 0;
}
