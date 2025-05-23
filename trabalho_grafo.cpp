#include <iostream>  // Para entrada/saída padrão
#include <vector>    // Para uso de vetores dinâmicos
#include <algorithm> // Para funções como sort()
#include <limits>    // Para valores de infinito
#include <queue>     // Para uso em BFS e outros algoritmos

using namespace std;

// Classe para elementos da lista de adjacência - cada nó representa uma aresta no grafo
class ElemLista
{
public:
    int vertice;     // Vértice de destino da aresta
    int peso;        // Peso/custo da aresta
    ElemLista *prox; // Ponteiro para o próximo elemento na lista
};

// Estrutura para representar uma aresta - usada principalmente para Kruskal e ordenação
struct Aresta
{
    int origem;  // Vértice de origem da aresta
    int destino; // Vértice de destino da aresta
    int peso;    // Peso/custo da aresta

    // Sobrecarga do operador < para ordenação de arestas por peso (crescente)
    bool operator<(const Aresta &outra) const
    {
        return peso < outra.peso;
    }
};

// Classe Union-Find (Conjuntos Disjuntos) para o algoritmo de Kruskal
// Permite verificar e unir conjuntos eficientemente para detectar ciclos
class DisjointSet
{
private:
    vector<int> pai;  // Vetor que guarda o pai de cada elemento
    vector<int> rank; // Vetor que guarda a altura (rank) de cada árvore

public:
    // Construtor - inicializa cada elemento como seu próprio pai
    DisjointSet(int n)
    {
        pai.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; i++)
        {
            pai[i] = i; // Cada elemento começa como representante de si mesmo
        }
    }

    // Encontra o representante do conjunto que contém x (com compressão de caminho)
    // A compressão de caminho reduz a altura da árvore, tornando buscas futuras mais rápidas
    int find(int x)
    {
        if (pai[x] != x)
        {
            pai[x] = find(pai[x]); // Atualiza o pai diretamente para a raiz
        }
        return pai[x]; // Retorna o representante do conjunto
    }

    // Une dois conjuntos por rank (union by rank)
    // Esta técnica ajuda a manter as árvores mais baixas possíveis
    void unir(int x, int y)
    {
        // Encontra as raízes dos conjuntos
        int raizX = find(x);
        int raizY = find(y);

        // Se já estão no mesmo conjunto, não faz nada
        if (raizX == raizY)
            return;

        // Anexa a árvore de menor rank à raiz da árvore de maior rank
        if (rank[raizX] < rank[raizY])
        {
            pai[raizX] = raizY; // raizX se torna filho de raizY
        }
        else
        {
            pai[raizY] = raizX; // raizY se torna filho de raizX

            // Se os ranks são iguais, a altura da árvore resultante aumenta
            if (rank[raizX] == rank[raizY])
            {
                rank[raizX]++;
            }
        }
    }
};

// Classe principal que implementa um grafo usando lista de adjacência
class Grafo
{
public:
    int numVertices;  // Número de vértices do grafo
    int numArestas;   // Número de arestas do grafo
    bool direcionado; // Define se o grafo é direcionado
    bool ponderado;   // Define se o grafo tem pesos nas arestas
    ElemLista **A;    // Array de listas de adjacência

    // Construtor do grafo
    Grafo(int vertices, bool direcionado = false, bool ponderado = true)
    {
        numVertices = vertices;
        numArestas = 0;
        this->direcionado = direcionado;
        this->ponderado = ponderado;

        // Inicializa a lista de adjacência com NULL
        A = new ElemLista *[vertices];
        for (int i = 0; i < vertices; i++)
        {
            A[i] = nullptr;
        }
    }

    // Método auxiliar para inserir aresta na lista de adjacência
    // Mantém a lista ordenada por vértice de destino
    bool insereArestaAux(int v1, int v2, int peso)
    {
        ElemLista *ant = nullptr;
        ElemLista *atual = A[v1];

        // Busca posição correta para inserir (mantendo ordem crescente de vértices)
        while (atual != nullptr && atual->vertice < v2)
        {
            ant = atual;
            atual = atual->prox;
        }

        // Verifica se a aresta já existe
        if (atual != nullptr && atual->vertice == v2)
        {
            return false; // Aresta já existe, não insere
        }

        // Cria novo elemento para a lista
        ElemLista *novo = new ElemLista();
        novo->vertice = v2;
        novo->peso = peso;
        novo->prox = atual;

        // Insere o novo elemento na lista
        if (ant != nullptr)
        {
            ant->prox = novo; // Insere no meio ou final da lista
        }
        else
        {
            A[v1] = novo; // Insere no início da lista
        }

        return true; // Aresta inserida com sucesso
    }

    // Adiciona aresta não direcionada (ou direcionada se o grafo for direcionado)
    void adicionaAresta(int v1, int v2, int peso)
    {
        // Verifica se os vértices são válidos
        if (v1 < 0 || v2 < 0 || v1 >= numVertices || v2 >= numVertices || v1 == v2)
        {
            cout << "Entrada invalida" << endl;
            return;
        }

        // Insere com peso fornecido ou peso 1 (se não for ponderado)
        if (ponderado)
        {
            if (insereArestaAux(v1, v2, peso))
            {
                if (!direcionado)
                {
                    insereArestaAux(v2, v1, peso); // Para grafo não direcionado, insere em ambas direções
                }
                numArestas++;
            }
        }
        else
        {
            if (insereArestaAux(v1, v2, 1)) // Peso 1 para grafo não ponderado
            {
                if (!direcionado)
                {
                    insereArestaAux(v2, v1, 1);
                }
                numArestas++;
            }
        }
    }

    // Adiciona aresta direcionada (ignorando configuração direcionado)
    void adicionaArestaDirecionada(int v1, int v2, int peso)
    {
        // Verifica se os vértices são válidos
        if (v1 < 0 || v2 < 0 || v1 >= numVertices || v2 >= numVertices || v1 == v2)
        {
            cout << "Entrada invalida" << endl;
            return;
        }

        // Insere a aresta apenas na direção especificada
        if (ponderado)
        {
            if (insereArestaAux(v1, v2, peso))
            {
                numArestas++;
            }
        }
        else
        {
            if (insereArestaAux(v1, v2, 1)) // Peso 1 para grafo não ponderado
            {
                numArestas++;
            }
        }
    }

    // Implementação do algoritmo BFS (Busca em Largura)
    // Calcula distâncias mínimas em grafo não ponderado
    void algoritmoBFS(int v)
    {
        const int INF = 999999;                 // Valor que representa "infinito"
        int *dist = new int[numVertices];       // Array de distâncias
        bool *visitado = new bool[numVertices]; // Marca vértices visitados

        // Inicializa distâncias como infinito e vértices como não visitados
        for (int i = 0; i < numVertices; i++)
        {
            dist[i] = INF;
            visitado[i] = false;
        }

        queue<int> fila; // Fila para BFS
        dist[v] = 0;     // Distância do vértice inicial para ele mesmo é 0
        visitado[v] = true;
        fila.push(v);

        // Processa a BFS
        while (!fila.empty())
        {
            int u = fila.front();
            fila.pop();

            // Explora todos os vizinhos
            ElemLista *vizinho = A[u];
            while (vizinho != nullptr)
            {
                int w = vizinho->vertice;

                // Se o vizinho não foi visitado
                if (!visitado[w])
                {
                    visitado[w] = true;
                    dist[w] = dist[u] + 1; // Distância é a distância do pai + 1
                    fila.push(w);
                }

                vizinho = vizinho->prox;
            }
        }

        // Exibe as distâncias calculadas
        cout << "\nDistancias a partir do vertice " << v << " (usando BFS):\n";
        for (int i = 0; i < numVertices; i++)
        {
            if (dist[i] == INF)
                cout << "Vertice " << i << ": INFINITO\n";
            else
                cout << "Vertice " << i << ": " << dist[i] << "\n";
        }

        // Libera memória alocada
        delete[] dist;
        delete[] visitado;
    }

    // Implementação do algoritmo de Dijkstra para caminhos mínimos em grafos ponderados
    void algoritmoDijkstra(int v)
    {
        const int INF = 999999;                 // Valor que representa "infinito"
        int *dist = new int[numVertices];       // Array de distâncias
        bool *visitado = new bool[numVertices]; // Marca vértices processados

        // Inicializa distâncias como infinito e vértices como não visitados
        for (int i = 0; i < numVertices; i++)
        {
            dist[i] = INF;
            visitado[i] = false;
        }

        dist[v] = 0; // Distância do vértice inicial para ele mesmo é 0

        // Algoritmo principal de Dijkstra
        for (int j = 0; j < numVertices - 1; j++)
        {
            // Encontra vértice não visitado com menor distância atual
            int u = -1;
            int menorDist = INF;

            for (int i = 0; i < numVertices; i++)
            {
                if (!visitado[i] && dist[i] < menorDist)
                {
                    menorDist = dist[i];
                    u = i;
                }
            }

            // Se não encontrou vértice alcançável, encerra
            if (u == -1)
            {
                break;
            }

            visitado[u] = true; // Marca como visitado

            // Relaxa todas as arestas saindo do vértice u
            ElemLista *vizinho = A[u];
            while (vizinho != nullptr)
            {
                int v = vizinho->vertice;
                int peso = vizinho->peso;

                // Verifica se encontrou caminho melhor
                if (!visitado[v] && dist[u] + peso < dist[v])
                {
                    dist[v] = dist[u] + peso; // Atualiza distância
                }

                vizinho = vizinho->prox;
            }
        }

        // Exibe as distâncias calculadas
        cout << "\nDistancias a partir do vertice " << v << " (usando Dijkstra):\n";
        for (int i = 0; i < numVertices; i++)
        {
            if (dist[i] == INF)
                cout << "Vertice " << i << ": INFINITO\n";
            else
                cout << "Vertice " << i << ": " << dist[i] << "\n";
        }

        // Libera memória alocada
        delete[] dist;
        delete[] visitado;
    }

    // Implementação do algoritmo de Floyd-Warshall para todos os pares de caminhos mínimos
    void algoritmoFloyd()
    {
        const int INF = 999999; // Valor que representa "infinito"
        vector<vector<int>> dist(numVertices, vector<int>(numVertices, INF));

        // Inicializa matriz de distâncias
        for (int i = 0; i < numVertices; i++)
        {
            dist[i][i] = 0; // Distância de um vértice para ele mesmo é 0

            // Inicializa com os pesos das arestas diretamente conectadas
            ElemLista *atual = A[i];
            while (atual != nullptr)
            {
                dist[i][atual->vertice] = atual->peso;
                atual = atual->prox;
            }
        }

        // Algoritmo principal de Floyd-Warshall - O(V³)
        for (int k = 0; k < numVertices; k++) // Para cada vértice intermediário
        {
            for (int i = 0; i < numVertices; i++) // Para cada origem
            {
                for (int j = 0; j < numVertices; j++) // Para cada destino
                {
                    // Verifica se existe caminho melhor passando por k
                    if (dist[i][k] != INF && dist[k][j] != INF &&
                        dist[i][k] + dist[k][j] < dist[i][j])
                    {
                        dist[i][j] = dist[i][k] + dist[k][j];
                    }
                }
            }
        }

        // Exibe a matriz de distâncias
        cout << "\nMatriz de distancias minimas (Floyd-Warshall):\n";
        for (int i = 0; i < numVertices; i++)
        {
            for (int j = 0; j < numVertices; j++)
            {
                if (dist[i][j] == INF)
                    cout << "INF\t";
                else
                    cout << dist[i][j] << "\t";
            }
            cout << endl;
        }
    }

    // Implementação do algoritmo de Kruskal para Árvore Geradora Mínima (MST)
    void algoritmoKruskal()
    {
        vector<Aresta> arestas;

        // Coleta todas as arestas do grafo (não direcionado)
        // Considerando apenas uma direção para evitar duplicação
        for (int i = 0; i < numVertices; i++)
        {
            ElemLista *atual = A[i];
            while (atual != nullptr)
            {
                // Armazena apenas arestas onde origem < destino para evitar duplicação
                if (i < atual->vertice)
                {
                    Aresta e;
                    e.origem = i;
                    e.destino = atual->vertice;
                    e.peso = atual->peso;
                    arestas.push_back(e);
                }
                atual = atual->prox;
            }
        }

        // Ordena arestas por peso (crescente)
        sort(arestas.begin(), arestas.end());

        DisjointSet ds(numVertices); // Inicializa Union-Find
        vector<Aresta> mst;          // Armazena arestas da MST
        int custoTotal = 0;          // Custo total da MST

        // Algoritmo principal de Kruskal
        for (auto &aresta : arestas)
        {
            // Verifica se adicionar a aresta não forma ciclo
            if (ds.find(aresta.origem) != ds.find(aresta.destino))
            {
                mst.push_back(aresta);                  // Adiciona à MST
                ds.unir(aresta.origem, aresta.destino); // Une os conjuntos
                custoTotal += aresta.peso;              // Atualiza custo total
            }
        }

        // Exibe a MST encontrada
        cout << "\nArvore Geradora Minima (Kruskal):\n";
        cout << "Arestas na MST:\n";
        for (auto &aresta : mst)
        {
            cout << aresta.origem << " -- " << aresta.destino << " (peso: " << aresta.peso << ")\n";
        }
        cout << "Custo total da MST: " << custoTotal << endl;
    }

    // Implementação do algoritmo de Prim para Árvore Geradora Mínima (MST)
    void algoritmoPrim()
    {
        const int INF = 999999;                    // Valor que representa "infinito"
        vector<int> chave(numVertices, INF);       // Menor peso para conectar à MST
        vector<bool> naArvore(numVertices, false); // Vértices já na MST
        vector<int> pai(numVertices, -1);          // Pai de cada vértice na MST

        chave[0] = 0; // Começa do vértice 0

        // Algoritmo principal de Prim
        for (int i = 0; i < numVertices - 1; i++)
        {
            // Encontra vértice com menor chave (peso) que ainda não está na MST
            int u = -1;
            int menorChave = INF;
            for (int v = 0; v < numVertices; v++)
            {
                if (!naArvore[v] && chave[v] < menorChave)
                {
                    menorChave = chave[v];
                    u = v;
                }
            }

            // Se não encontrou vértice alcançável, encerra
            if (u == -1)
                break;

            naArvore[u] = true; // Adiciona à MST

            // Atualiza chaves dos vértices adjacentes
            ElemLista *adj = A[u];
            while (adj != nullptr)
            {
                int v = adj->vertice;
                int peso = adj->peso;

                // Se encontrou aresta de menor peso para alcançar v
                if (!naArvore[v] && peso < chave[v])
                {
                    pai[v] = u;      // Atualiza o pai
                    chave[v] = peso; // Atualiza a chave
                }

                adj = adj->prox;
            }
        }

        // Exibe a MST encontrada
        int custoTotal = 0;
        cout << "\nArvore Geradora Minima (Prim):\n";
        cout << "Arestas na MST:\n";
        for (int i = 1; i < numVertices; i++)
        {
            if (pai[i] != -1)
            {
                cout << pai[i] << " -- " << i;

                // Localiza a aresta para obter o peso
                ElemLista *adj = A[pai[i]];
                while (adj != nullptr && adj->vertice != i)
                {
                    adj = adj->prox;
                }

                if (adj != nullptr)
                {
                    cout << " (peso: " << adj->peso << ")\n";
                    custoTotal += adj->peso;
                }
                else
                {
                    cout << " (peso desconhecido)\n";
                }
            }
        }
        cout << "Custo total da MST: " << custoTotal << endl;
    }

    // Verifica se o grafo está conectado usando DFS
    bool estaConectadoDFS()
    {
        vector<bool> visitado(numVertices, false);

        // Inicia DFS do vértice 0
        dfs(0, visitado);

        // Verifica se todos os vértices foram visitados
        for (bool v : visitado)
        {
            if (!v)
                return false; // Se algum vértice não foi visitado, grafo não está conectado
        }
        return true; // Todos os vértices alcançáveis, grafo conectado
    }

    // Função auxiliar para DFS recursivo
    void dfs(int v, vector<bool> &visitado)
    {
        visitado[v] = true; // Marca vértice como visitado

        // Visita todos os vizinhos não visitados
        ElemLista *atual = A[v];
        while (atual != nullptr)
        {
            if (!visitado[atual->vertice])
            {
                dfs(atual->vertice, visitado);
            }
            atual = atual->prox;
        }
    }

    // Algoritmo de ApagaReverso para MST
    // Remove arestas em ordem decrescente de peso sem desconectar o grafo
    void algoritmoApagaReverso()
    {
        vector<Aresta> arestas;

        // Coleta todas as arestas do grafo
        for (int i = 0; i < numVertices; i++)
        {
            ElemLista *atual = A[i];
            while (atual != nullptr)
            {
                if (i < atual->vertice) // Evita duplicação em grafo não direcionado
                {
                    arestas.push_back({i, atual->vertice, atual->peso});
                }
                atual = atual->prox;
            }
        }

        // Ordena arestas por peso (decrescente)
        sort(arestas.begin(), arestas.end(), [](Aresta a, Aresta b)
             { return a.peso > b.peso; });

        // Para cada aresta (da mais pesada para a mais leve)
        for (auto &aresta : arestas)
        {
            // Remove temporariamente a aresta
            removeAresta(aresta.origem, aresta.destino);

            // Verifica se o grafo continua conectado
            if (!estaConectadoDFS())
            {
                // Se ficou desconectado, adiciona a aresta de volta (é essencial)
                adicionaAresta(aresta.origem, aresta.destino, aresta.peso);
            }
            // Se continua conectado, a aresta pode ser removida permanentemente
        }

        // Exibe o resultado final (MST)
        cout << "\nResultado do algoritmo Apaga Reverso:\n";
        exibeGrafo();
    }

    // Ordena e exibe as arestas por peso
    void ordenarArestas(bool crescente)
    {
        vector<Aresta> arestas;

        // Coleta todas as arestas do grafo
        for (int i = 0; i < numVertices; i++)
        {
            ElemLista *atual = A[i];
            while (atual != nullptr)
            {
                if (i < atual->vertice) // Evita duplicação em grafo não direcionado
                {
                    Aresta e;
                    e.origem = i;
                    e.destino = atual->vertice;
                    e.peso = atual->peso;
                    arestas.push_back(e);
                }
                atual = atual->prox;
            }
        }

        // Ordena arestas conforme especificado
        if (crescente)
        {
            sort(arestas.begin(), arestas.end()); // Ordem crescente
            cout << "\nArestas ordenadas por peso (crescente):\n";
        }
        else
        {
            sort(arestas.begin(), arestas.end(), [](const Aresta &a, const Aresta &b)
                 { return a.peso > b.peso; }); // Ordem decrescente
            cout << "\nArestas ordenadas por peso (decrescente):\n";
        }

        // Exibe as arestas ordenadas
        for (auto &aresta : arestas)
        {
            cout << aresta.origem << " -- " << aresta.destino << " (peso: " << aresta.peso << ")\n";
        }
    }

    // Método auxiliar para remover aresta da lista de adjacência
    bool removeArestaAux(int v1, int v2)
    {
        ElemLista *ant = nullptr;
        ElemLista *atual = A[v1];

        // Busca a aresta na lista
        while (atual != nullptr && atual->vertice < v2)
        {
            ant = atual;
            atual = atual->prox;
        }

        // Se encontrou a aresta, remove-a
        if (atual != nullptr && atual->vertice == v2)
        {
            if (ant != nullptr)
            {
                ant->prox = atual->prox; // Remove do meio ou final da lista
            }
            else
            {
                A[v1] = atual->prox; // Remove do início da lista
            }

            delete atual; // Libera memória
            return true;  // Aresta removida com sucesso
        }

        return false; // Aresta não encontrada
    }

    // Remove aresta do grafo (em ambas direções para grafo não direcionado)
    bool removeAresta(int v1, int v2)
    {
        // Verifica se os vértices são válidos
        if (v1 < 0 || v2 < 0 || v1 >= numVertices || v2 >= numVertices || v1 == v2)
        {
            cout << "Entrada invalida\n";
            return false;
        }

        // Remove a aresta
        if (removeArestaAux(v1, v2))
        {
            removeArestaAux(v2, v1); // Remove em ambas direções para grafo não direcionado
            numArestas--;
            return true;
        }

        return false; // Aresta não encontrada
    }

    // Exibe o grafo (lista de adjacência)
    void exibeGrafo()
    {
        for (int i = 0; i < numVertices; i++)
        {
            ElemLista *temp = A[i];
            cout << "Vertice " << i << " -> ";

            // Para cada vértice, exibe seus vizinhos e pesos
            while (temp != nullptr)
            {
                cout << "(" << temp->vertice << ", peso: " << temp->peso << ")";
                if (temp->prox != nullptr)
                    cout << " -> ";
                temp = temp->prox;
            }
            cout << endl;
        }
    }

    // Verifica se uma aresta existe no grafo
    bool arestaExiste(int v1, int v2)
    {
        // Verifica se os vértices são válidos
        if (v1 < 0 || v2 < 0 || v1 >= numVertices || v2 >= numVertices || v1 == v2)
        {
            cout << "Entrada invalida\n";
            return false;
        }

        // Busca a aresta na lista de adjacência
        ElemLista *atual = A[v1];
        while (atual != nullptr && atual->vertice < v2)
        {
            atual = atual->prox;
        }

        // Verifica se encontrou
        if (atual != nullptr && atual->vertice == v2)
        {
            return true; // Aresta existe
        }
        else
        {
            return false; // Aresta não existe
        }
    }

    // Destrutor - libera toda a memória alocada
    ~Grafo()
    {
        // Para cada vértice, libera sua lista de adjacência
        for (int i = 0; i < numVertices; i++)
        {
            ElemLista *temp = A[i];
            while (temp != nullptr)
            {
                ElemLista *toDelete = temp;
                temp = temp->prox;
                delete toDelete;
            }
        }

        // Libera o array de listas
        delete[] A;
    }
};

// Função para criar um grafo de exemplo para testes
void criarGrafoExemplo(Grafo &g, bool grafoDirecionado, bool grafoPonderado)
{

    // Adiciona diferentes arestas dependendo das configurações do grafo

    if (grafoDirecionado)
    {
        // Grafo direcionado
        if (grafoPonderado)
        {
            // Grafo direcionado e ponderado
            g.adicionaArestaDirecionada(0, 1, 5);
            g.adicionaArestaDirecionada(0, 2, 3);
            g.adicionaArestaDirecionada(1, 3, 7);
            g.adicionaArestaDirecionada(2, 3, 2);
            g.adicionaArestaDirecionada(3, 4, 1);
        }
        else
        {
            // Grafo direcionado não ponderado (peso 1)
            g.adicionaArestaDirecionada(0, 1, 1);
            g.adicionaArestaDirecionada(0, 2, 1);
            g.adicionaArestaDirecionada(1, 3, 1);
            g.adicionaArestaDirecionada(2, 3, 1);
            g.adicionaArestaDirecionada(3, 4, 1);
        }
    }
    else
    {
        // Grafo não direcionado
        if (grafoPonderado)
        {
            // Grafo não direcionado e ponderado
            g.adicionaAresta(0, 1, 5);
            g.adicionaAresta(0, 2, 3);
            g.adicionaAresta(1, 3, 7);
            g.adicionaAresta(2, 3, 2);
            g.adicionaAresta(3, 4, 1);
        }
        else
        {
            // Grafo não direcionado não ponderado (peso 1)
            g.adicionaAresta(0, 1, 1);
            g.adicionaAresta(0, 2, 1);
            g.adicionaAresta(1, 3, 1);
            g.adicionaAresta(2, 3, 1);
            g.adicionaAresta(3, 4, 1);
        }
    }
}

// Função principal
int main()
{
    bool grafoDirecionado, grafoPonderado;

    // Solicita configurações do grafo ao usuário
    cout << "O grafo sera direcionado? (s/n): ";
    char direcionado;
    cin >> direcionado;
    grafoDirecionado = (direcionado == 's' || direcionado == 'S');

    cout << "O grafo sera ponderado? (s/n): ";
    char ponderado;
    cin >> ponderado;
    grafoPonderado = (ponderado == 's' || ponderado == 'S');

    // Cria grafo com 5 vértices
    int numVertices = 5;
    Grafo g(numVertices);

    // Popula grafo com exemplo
    criarGrafoExemplo(g, grafoDirecionado, grafoPonderado);

    // Variáveis para o menu
    int opcao, v1, v2, peso, verticeInicial;
    bool sair = false;

    // Menu principal do programa
    while (!sair)
    {
        cout << "\n=== MENU DE OPERACOES DO GRAFO ===\n";
        cout << "01. Lista de adjacencia de grafos nao direcionados\n";
        cout << "02. Lista de adjacencia de grafos direcionados\n";
        cout << "03. Menor caminho de grafos direcionados\n";
        cout << "04. Menor caminho de grafos direcionados ponderados\n";
        cout << "05. Menor caminho de grafos nao direcionados\n";
        cout << "06. Menor caminho de grafos nao direcionados ponderados\n";
        cout << "07. Arvore geradora minima - Kruskal\n";
        cout << "08. Arvore geradora minima - Prim\n";
        cout << "09. Arvore geradora minima - ApagaReservo\n";
        cout << "10. Ordenacao de arestas por peso (crescente/decrescente)\n";
        cout << "11. Sair\n";
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        // Processa a opção escolhida
        switch (opcao)
        {
        case 1: // Exibe lista de adjacência de grafo não direcionado
            if (!grafoDirecionado)
            {
                cout << "\n=== LISTA DE ADJACENCIA DE GRAFOS NÃO DIRECIONADOS ===\n";
                g.exibeGrafo();
            }
            else
            {
                cout << "\nEsta opcao so pode ser utilizada em grafos nao direcionados!\n";
            }
            break;

        case 2: // Exibe lista de adjacência de grafo direcionado
            if (grafoDirecionado)
            {
                cout << "\n=== LISTA DE ADJACENCIA DE GRAFOS DIRECIONADOS ===\n";
                g.exibeGrafo();
            }
            else
            {
                cout << "Esta opcao so pode ser utilizada em grafos direcionados!\n";
            }
            break;

        case 3: // BFS para menor caminho em grafo direcionado não ponderado
            if (grafoDirecionado && !grafoPonderado)
            {
                cout << "\n=== MENOR CAMINHO DE GRAFOS DIRECIONADOS NAO PONDERADOS (utilizando BFS) ===\n";
                cout << "Digite o vertice inicial: ";
                cin >> verticeInicial;
                g.algoritmoBFS(verticeInicial);
            }
            else
            {
                cout << "\nEsta opcao so pode ser utilizada em grafos direcionados nao ponderados!\n";
            }
            break;

        case 4: // Dijkstra e Floyd para menor caminho em grafo direcionado ponderado
            if (grafoDirecionado && grafoPonderado)
            {
                cout << "\n=== MENOR CAMINHO DE GRAFOS DIRECIONADOS PONDERADOS ===\n";
                cout << "Utilizando Dijkstra: \n";
                cout << "Digite o vertice inicial: ";
                cin >> verticeInicial;
                g.algoritmoDijkstra(verticeInicial);

                g.algoritmoFloyd();
            }
            else
            {
                cout << "\nEsta opcao so pode ser utilizada em grafos direcionados ponderados!\n";
            }
            break;

        case 5: // BFS para menor caminho em grafo não direcionado não ponderado
            if (!grafoDirecionado && !grafoPonderado)
            {
                cout << "\n=== MENOR CAMINHO DE GRAFOS NAO DIRECIONADOS NAO PONDERADOS (utilizando BFS)===\n";
                cout << "Digite o vertice inicial: ";
                cin >> verticeInicial;
                g.algoritmoBFS(verticeInicial);
            }
            else
            {
                cout << "\nEsta opcao so pode ser utilizada em grafos nao direcionados!\n";
            }
            break;

        case 6: // Dijkstra e Floyd para menor caminho em grafo não direcionado ponderado
            if (!grafoDirecionado && grafoPonderado)
            {
                cout << "\n=== MENOR CAMINHO DE GRAFOS NAO DIRECIONADOS PONDERADOS ===\n";
                cout << "Utilizando Dijkstra: \n";
                cout << "Digite o vertice inicial: ";
                cin >> verticeInicial;
                g.algoritmoDijkstra(verticeInicial);

                g.algoritmoFloyd();
            }
            else
            {
                cout << "\nEsta opcao so pode ser utilizada em grafos nao direcionados ponderados!\n";
            }
            break;

        case 7: // Kruskal para árvore geradora mínima
            if (!grafoDirecionado && grafoPonderado)
            {
                cout << "\n=== ARVORE GERADORA MINIMA - KRUSKAL ===\n";
                g.algoritmoKruskal();
                break;
            }
            else
            {
                cout << "\nEsta opcao so pode ser utilizada em grafos nao direcionados ponderados!\n";
            }
            break;

        case 8: // Prim para árvore geradora mínima
            if (!grafoDirecionado && grafoPonderado)
            {
                cout << "\n=== ARVORE GERADORA MINIMA - PRIM ===\n";
                g.algoritmoPrim();
                break;
            }
            else
            {
                cout << "\nEsta opcao so pode ser utilizada em grafos nao direcionados ponderados!\n";
            }
            break;

        case 9: // ApagaReverso para árvore geradora mínima
            if (!grafoDirecionado && grafoPonderado)
            {
                cout << "\n=== ARVORE GERADORA MINIMA - APAGARESERVO ===\n";
                g.algoritmoApagaReverso();
                break;
            }
            else
            {
                cout << "\nEsta opcao so pode ser utilizada em grafos nao direcionados ponderados!\n";
            }
            break;

        case 10: // Ordenação de arestas por peso
            char ordem;
            cout << "Ordenar em ordem (c)rescente ou (d)ecrescente? ";
            cin >> ordem;
            g.ordenarArestas(ordem == 'c' || ordem == 'C');
            break;

        case 11: // Sair do programa
            cout << "Encerrando programa...\n";
            sair = true;
            break;

        default: // Opção inválida
            cout << "Opcao invalida! Tente novamente.\n";
            break;
        }
    }

    return 0;
}