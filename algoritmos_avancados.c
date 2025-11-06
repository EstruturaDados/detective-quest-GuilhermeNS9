#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura que representa uma sala (nó da árvore binária)
typedef struct Sala {
    char nome[50];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

/*
 * Função: criarSala
 * -----------------
 * Aloca dinamicamente uma nova sala, define o nome e
 * inicializa os ponteiros esquerdo e direito como NULL.
 */
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memória para a sala.\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

/*
 * Função: explorarSalas
 * ---------------------
 * Permite a navegação interativa pela mansão a partir da sala recebida.
 * O jogador escolhe ir à esquerda (e), à direita (d) ou sair (s).
 * A exploração termina ao chegar em um cômodo sem caminhos (nó-folha)
 * ou se o jogador optar por sair.
 */
void explorarSalas(Sala *atual) {
    char opcao;

    while (1) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Caso a sala atual seja uma folha (sem caminhos)
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Não há mais caminhos a seguir. Fim da exploração!\n");
            break;
        }

        printf("Escolha um caminho:\n");
        if (atual->esquerda != NULL) printf("  [e] Ir para a esquerda (%s)\n", atual->esquerda->nome);
        if (atual->direita != NULL) printf("  [d] Ir para a direita (%s)\n", atual->direita->nome);
        printf("  [s] Sair da mansão\n");
        printf("Opção: ");
        scanf(" %c", &opcao);

        if (opcao == 'e' && atual->esquerda != NULL) {
            atual = atual->esquerda;
        } else if (opcao == 'd' && atual->direita != NULL) {
            atual = atual->direita;
        } else if (opcao == 's') {
            printf("Você decidiu encerrar a exploração.\n");
            break;
        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    }
}

/*
 * Função: main
 * ------------
 * Monta manualmente a árvore binária que representa o mapa da mansão.
 * Depois, chama a função de exploração a partir do Hall de Entrada.
 */
int main() {
    // Criação das salas (nós)
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *jardim = criarSala("Jardim");
    Sala *porao = criarSala("Porão");

    // Montagem da árvore (mapa da mansão)
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->direita = porao;

    // Início da exploração
    printf("=== Detective Quest: Exploração da Mansão ===\n");
    explorarSalas(hall);

    // Libera a memória alocada
    free(biblioteca);
    free(jardim);
    free(salaEstar);
    free(porao);
    free(cozinha);
    free(hall);

    printf("\nObrigado por jogar Detective Quest!\n");
    return 0;
}
