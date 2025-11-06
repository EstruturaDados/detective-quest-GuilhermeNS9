#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* -------------------------
   Definições e estruturas
   ------------------------- */

#define MAXNOME 64
#define MAXPISTA 128
#define HASH_SIZE 101  // tamanho simples para a tabela hash

/* Nó da árvore binária que representa uma sala */
typedef struct Sala {
    char nome[MAXNOME];
    char pista[MAXPISTA]; /* pista associada (string vazia se não houver) */
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

/* Nó da BST de pistas coletadas */
typedef struct PistaNode {
    char pista[MAXPISTA];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

/* Entrada para encadeamento na tabela hash */
typedef struct HashEntry {
    char chave[MAXPISTA];      /* a pista */
    char suspeito[MAXNOME];    /* o suspeito associado */
    struct HashEntry *prox;
} HashEntry;

/* Tabela hash: array de ponteiros para HashEntry */
typedef struct HashTable {
    HashEntry *tabela[HASH_SIZE];
} HashTable;

/* -------------------------
   Funções utilitárias
   ------------------------- */

/* djb2 - hash simples para strings */
unsigned long hash_string(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % HASH_SIZE;
}

/* converte string para minúsculas (retorna buffer passado) */
void to_lowercase(char *dst, const char *src) {
    size_t i;
    for (i = 0; src[i] != '\0' && i < MAXNOME - 1; ++i)
        dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}

/* -------------------------
   Funções para Salas
   ------------------------- */

/*
 * criarSala() – cria dinamicamente um cômodo.
 * Parâmetros:
 *   nome  - nome da sala
 *   pista - string da pista (pode ser "" se não houver)
 * Retorno: ponteiro para Sala alocada
 */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        fprintf(stderr, "Erro: malloc falhou em criarSala\n");
        exit(EXIT_FAILURE);
    }
    strncpy(nova->nome, nome, MAXNOME-1); nova->nome[MAXNOME-1] = '\0';
    strncpy(nova->pista, pista, MAXPISTA-1); nova->pista[MAXPISTA-1] = '\0';
    nova->esquerda = nova->direita = NULL;
    return nova;
}

/* libera recursivamente a árvore de salas */
void liberarSalas(Sala *r) {
    if (!r) return;
    liberarSalas(r->esquerda);
    liberarSalas(r->direita);
    free(r);
}

/* -------------------------
   Funções para BST de pistas
   ------------------------- */

/*
 * inserirPista() / adicionarPista() – insere a pista coletada na árvore de pistas.
 * Se a pista já existir, não insere duplicata.
 */
PistaNode* criarNoPista(const char *pista) {
    PistaNode *n = (PistaNode*) malloc(sizeof(PistaNode));
    if (!n) { fprintf(stderr, "Erro: malloc falhou criarNoPista\n"); exit(EXIT_FAILURE); }
    strncpy(n->pista, pista, MAXPISTA-1); n->pista[MAXPISTA-1] = '\0';
    n->esquerda = n->direita = NULL;
    return n;
}

PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) {
        return criarNoPista(pista);
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (cmp > 0)
        raiz->direita = inserirPista(raiz->direita, pista);
    /* se igual: já existe, não inserir duplicata */
    return raiz;
}

/* Percorre em ordem e imprime as pistas */
void exibirPistasEmOrdem(PistaNode *raiz) {
    if (!raiz) return;
    exibirPistasEmOrdem(raiz->esquerda);
    printf(" - %s\n", raiz->pista);
    exibirPistasEmOrdem(raiz->direita);
}

/* liberar árvore de pistas */
void liberarPistas(PistaNode *r) {
    if (!r) return;
    liberarPistas(r->esquerda);
    liberarPistas(r->direita);
    free(r);
}

/* Função auxiliar: conta pistas totais (opcional) */
int contarPistas(PistaNode *r) {
    if (!r) return 0;
    return 1 + contarPistas(r->esquerda) + contarPistas(r->direita);
}

/* -------------------------
   Funções para Hash Table
   ------------------------- */

/*
 * inserirNaHash() – insere associação pista -> suspeito na tabela hash.
 * Se a chave já existir, sobrescreve o suspeito.
 */
void inicializarHash(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; ++i) ht->tabela[i] = NULL;
}

void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito) {
    unsigned long idx = hash_string(pista);
    HashEntry *head = ht->tabela[idx];
    /* procura se já existe chave */
    for (HashEntry *e = head; e != NULL; e = e->prox) {
        if (strcmp(e->chave, pista) == 0) {
            /* atualiza suspeito */
            strncpy(e->suspeito, suspeito, MAXNOME-1);
            e->suspeito[MAXNOME-1] = '\0';
            return;
        }
    }
    /* não existe: cria novo entry e adiciona ao início */
    HashEntry *novo = (HashEntry*) malloc(sizeof(HashEntry));
    if (!novo) { fprintf(stderr, "Erro: malloc falhou inserirNaHash\n"); exit(EXIT_FAILURE); }
    strncpy(novo->chave, pista, MAXPISTA-1); novo->chave[MAXPISTA-1] = '\0';
    strncpy(novo->suspeito, suspeito, MAXNOME-1); novo->suspeito[MAXNOME-1] = '\0';
    novo->prox = head;
    ht->tabela[idx] = novo;
}

/*
 * encontrarSuspeito() – consulta o suspeito correspondente a uma pista.
 * Retorna NULL se não encontrado.
 */
const char* encontrarSuspeito(HashTable *ht, const char *pista) {
    unsigned long idx = hash_string(pista);
    for (HashEntry *e = ht->tabela[idx]; e != NULL; e = e->prox) {
        if (strcmp(e->chave, pista) == 0) return e->suspeito;
    }
    return NULL;
}

/* libera toda a tabela hash */
void liberarHash(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashEntry *e = ht->tabela[i];
        while (e) {
            HashEntry *prox = e->prox;
            free(e);
            e = prox;
        }
        ht->tabela[i] = NULL;
    }
}

/* -------------------------
   Exploração interativa
   ------------------------- */

/*
 * explorarSalas() – navega pela árvore e ativa o sistema de pistas.
 * Parâmetros:
 *   atual - sala inicial
 *   bstPistas - ponteiro para a raiz da BST de pistas coletadas
 *   ht - tabela hash para relacionar pista -> suspeito
 *
 * Comportamento: ao entrar na sala, exibe a pista (se houver),
 * insere na BST e segue interação (e/d/s).
 */
void explorarSalas(Sala *atual, PistaNode **bstPistas, HashTable *ht) {
    char opcao;
    while (1) {
        printf("\nVocê está na sala: %s\n", atual->nome);
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            /* insere na BST (cuidando de duplicatas) */
            *bstPistas = inserirPista(*bstPistas, atual->pista);
            /* mostra suspeito associado (se existir na hash) */
            const char *s = encontrarSuspeito(ht, atual->pista);
            if (s) printf(" -> Essa pista está relacionada ao suspeito: %s\n", s);
            else printf(" -> Nenhum suspeito relacionado a essa pista.\n");
        } else {
            printf("Nenhuma pista nesta sala.\n");
        }

        /* opções de movimento */
        printf("\nEscolha: ");
        if (atual->esquerda) printf("[e] esquerda ");
        if (atual->direita) printf("[d] direita ");
        printf("[s] sair\n");
        printf("Opção: ");
        scanf(" %c", &opcao);

        if (opcao == 'e' && atual->esquerda) {
            atual = atual->esquerda;
        } else if (opcao == 'd' && atual->direita) {
            atual = atual->direita;
        } else if (opcao == 's') {
            printf("Você decidiu encerrar a exploração.\n");
            break;
        } else {
            printf("Opção inválida ou caminho não disponível. Tente novamente.\n");
        }
    }
}

/* -------------------------
   Verificação final (julgamento)
   ------------------------- */

/*
 * verificarSuspeitoFinal() – conduz à fase de julgamento final.
 * Requisitos: o jogador acusa um suspeito (nome). O sistema verifica
 * quantas pistas coletadas apontam para esse suspeito (usando a hash).
 * Se >= 2 -> acusações suficientes.
 */
int contarPistasParaSuspeitoRec(PistaNode *raiz, HashTable *ht, const char *acusado) {
    if (!raiz) return 0;
    int total = 0;
    const char *s = encontrarSuspeito(ht, raiz->pista);
    if (s && strcmp(s, acusado) == 0) total = 1;
    total += contarPistasParaSuspeitoRec(raiz->esquerda, ht, acusado);
    total += contarPistasParaSuspeitoRec(raiz->direita, ht, acusado);
    return total;
}

void verificarSuspeitoFinal(PistaNode *bstPistas, HashTable *ht) {
    if (!bstPistas) {
        printf("\nVocê não coletou pistas. Não há base para acusação.\n");
        return;
    }

    printf("\nPistas coletadas (em ordem):\n");
    exibirPistasEmOrdem(bstPistas);

    /* pede o nome do suspeito acusado */
    char acusado[MAXNOME];
    printf("\nQuem você acusa como culpado? (Digite o nome completo):\n> ");
    /* limpar buffer restante antes de fgets */
    while (getchar() != '\n'); /* descartar */
    if (!fgets(acusado, sizeof(acusado), stdin)) acusado[0] = '\0';
    /* remove newline */
    acusado[strcspn(acusado, "\n")] = '\0';

    if (strlen(acusado) == 0) {
        printf("Nenhum acusado informado.\n");
        return;
    }

    /* contar quantas pistas apontam para o acusado */
    int cont = contarPistasParaSuspeitoRec(bstPistas, ht, acusado);

    printf("\nResultado da acusação contra '%s':\n", acusado);
    if (cont >= 2) {
        printf("✅ Há %d pistas que relacionam o acusado ao crime. A acusação é SUSTENTÁVEL.\n", cont);
    } else if (cont == 1) {
        printf("⚠️ Apenas 1 pista aponta para o acusado. Evidência insuficiente.\n");
    } else {
        printf("❌ Nenhuma pista aponta para o acusado. A acusação não se sustenta.\n");
    }
}

/* -------------------------
   Função main: montagem do mapa, hash e execução
   ------------------------- */

int main(void) {
    /* Montagem manual do mapa da mansão (fixo) */
    Sala *hall = criarSala("Hall de Entrada", "Pedaço rasgado de mapa");
    Sala *salaEstar = criarSala("Sala de Estar", "Copo com impressões digitais");
    Sala *cozinha = criarSala("Cozinha", "Marcas de pegadas com lama");
    Sala *biblioteca = criarSala("Biblioteca", "Livro removido da prateleira");
    Sala *jardim = criarSala("Jardim", "Lenço com iniciais M.R.");
    Sala *quarto = criarSala("Quarto do Caseiro", "Recibo de compra suspeito");
    Sala *escritorio = criarSala("Escritório", "Carta ameaçadora");
    Sala *porao = criarSala("Porão", "Cofre trancado com arranhões");

    /* Montagem da árvore (mapa) */
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->esquerda = escritorio;
    cozinha->direita = porao;

    biblioteca->esquerda = quarto; /* apenas exemplo: cria maior árvore */

    /* Inicializa BST de pistas coletadas e tabela hash */
    PistaNode *bstPistas = NULL;
    HashTable ht;
    inicializarHash(&ht);

    /* Insere associações pista -> suspeito na hash (pré-definidas) */
    inserirNaHash(&ht, "Pedaço rasgado de mapa", "Sr. Black");
    inserirNaHash(&ht, "Copo com impressões digitais", "Sra. White");
    inserirNaHash(&ht, "Marcas de pegadas com lama", "Sr. Green");
    inserirNaHash(&ht, "Livro removido da prateleira", "Sra. White");
    inserirNaHash(&ht, "Lenço com iniciais M.R.", "M.R.");
    inserirNaHash(&ht, "Recibo de compra suspeito", "Sr. Green");
    inserirNaHash(&ht, "Carta ameaçadora", "Sr. Black");
    inserirNaHash(&ht, "Cofre trancado com arranhões", "Sr. Black");
    /* Note: várias pistas apontam para Sr. Black e Sr. Green e Sra. White */

    /* Mensagem inicial e exploração */
    printf("=== Detective Quest: Julgamento Final ===\n");
    printf("Explore a mansão e colete pistas. Ao final, acuse um suspeito.\n");

    explorarSalas(hall, &bstPistas, &ht);

    /* Fase de julgamento */
    verificarSuspeitoFinal(bstPistas, &ht);

    /* Liberar memória */
    liberarPistas(bstPistas);
    liberarHash(&ht);
    liberarSalas(hall); /* libera todas as salas recursivamente */

    printf("\nFim do jogo. Obrigado por jogar!\n");
    return 0;
}