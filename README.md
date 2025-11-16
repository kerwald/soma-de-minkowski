# 🤖 Soma de Minkowski e Análise de Desempenho de Polígonos Convexos

Este projeto implementa a **Soma de Minkowski**, um conceito central no **Planejamento de Movimento Robótico** para o cálculo do Espaço de Configuração (C-Space).

O trabalho inclui a visualização interativa do C-Obstacle e um módulo robusto de coleta de dados de desempenho e distância mínima, com a posterior análise no Google Colab.

***

## ⚙️ Tecnologias Utilizadas

* **Implementação Principal:** C++
* **Biblioteca Gráfica:** `p8g` (para visualização interativa)
* **Análise de Dados:** Python (Jupyter Notebook/Google Colab) com **Pandas** e **Matplotlib**
* **Conceitos:** Geometria Computacional, Soma de Minkowski, Espaço de Configuração.

***

## ✨ Funcionalidades Principais

1.  **Soma de Minkowski:** Implementação eficiente para calcular a Soma de Minkowski e obter o **C-Obstacle**.
2.  **Cálculo de Distância Mínima:** Determinação da menor distância Euclideana entre o robô e um obstáculo usando a distância da origem ao C-Obstacle.
3.  **Visualização Interativa:** Interface gráfica que permite ao usuário desenhar o robô, os obstáculos e ver os C-Obstacles resultantes.
4.  **Coleta de Dados Automática:** Módulo que gera **500 amostras** de polígonos aleatórios para medir o tempo de execução e a distância mínima, salvando os resultados em CSV.

***

## 🚀 Como Executar

### Pré-requisitos
É necessário um ambiente C++ moderno, a biblioteca gráfica **p8g** (assumindo que está na pasta `p8g/`) e o utilitário **make** instalado.

### Compilação e Execução (Usando Makefile)

O projeto inclui um `Makefile` que automatiza a compilação e execução.

**1. Para compilar o projeto:**
Execute o comando `make` na raiz do diretório. Isso irá compilar os fontes e criar o executável (`main.exe`).

make

**2. Para compilar e executar diretamente: Use o comando make run. O Makefile irá compilar o projeto (se necessário) e iniciá-lo automaticamente.**

make run

**3. Para limpar os arquivos gerados: Para remover os arquivos objeto (.o), de dependência (.d) e o executável, use:**

make clean

(Nota: O comando clean no Makefile fornecido usa del, otimizado para Windows. Para Linux/macOS, talvez seja necessário alterá-lo para rm -f).

## 🕹️ Modos de Operação (Teclas de Comando)

A aplicação inicia em modo interativo na janela gráfica.

* **Clique do Mouse:** Adiciona um ponto (vértice) na posição do mouse.
* **Tecla P (Processar Polígono):**
    * O **primeiro** polígono desenhado é definido como o **Robô**.
    * Os polígonos seguintes são tratados como **Obstáculos**.
    * Ao pressionar 'P', o programa calcula o C-Obstacle (Soma de Minkowski) e salva os dados de desempenho.
* **Tecla G (Gerar Dados):** Inicia a **coleta automática de 500 amostras** aleatórias, salvando os resultados nos arquivos CSV.

***

## 📊 Análise de Dados e Resultados

Os dados de desempenho e distância são salvos automaticamente em `minkowski_performance.csv` e `minkowski_distances.csv`.

Para analisar os resultados, abra o notebook **`graficos_soma_de_minkowski.ipynb`** (Jupyter/Google Colab):

1.  Carregue os arquivos CSV gerados pelo C++.
2.  O notebook irá gerar visualizações e tabelas, incluindo:
    * Tempo de execução vs. Total de vértices (para validar a complexidade $O(n+m)$).
    * Gráfico de Vértices Resultantes vs. Vértices de Entrada.
    * Análise da distribuição das distâncias mínimas.

## 🧠 Sobre a Soma de Minkowski

A Soma de Minkowski de dois conjuntos A e B é definida matematicamente como:

    A⊕B={a+b∣a∈A,b∈B}

No contexto de planejamento de movimento robótico, esta operação é usada para calcular o Espaço de Configuração (C-Space). Ao "expandir" os obstáculos (Obstáculo ⊕ -Robô), podemos tratar o robô como um simples ponto, simplificando drasticamente o problema de detecção de colisão e planejamento de rotas.

## Demonstração
Link para o video demonstrativo abaixo:   
https://drive.google.com/file/d/1iJFxtxslqMMG196eWWF1uOL8iSv5yZLg/view?usp=sharing  

## 📚 Referências

  Conceitos de Geometria Computacional para Planejamento de Movimento.

  Documentação da biblioteca gráfica p8g++.

  Resultados e gráficos gerados pelo notebook de análise.

## 👤 Autor

Projeto desenvolvido por **Felipe Leonardo Kerwald Santana**.

*Estudante de Ciência da Computação - projeto da disciplina de Programação Avançada - UFPel*

Conecte-se comigo:

* **GitHub:** [@kerwald](https://github.com/kerwald)
* **LinkedIn:** [Felipe Kerwald](https://www.linkedin.com/in/felipekerwald/)
