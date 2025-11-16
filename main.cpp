#include "p8g/p8g.hpp"
#include "Ponto.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <fstream>  
#include <chrono>   
#include <iomanip>  
#include <random>
#include <limits> 

std::vector<std::vector<Ponto>> poligonos;
std::vector<std::vector<Ponto>> areasMinkowski;
std::vector<Ponto> robo;
bool existRobo = false;
std::vector<Ponto> pontos;
std::ofstream performanceFile;
std::ofstream distanceFile;

std::vector<Ponto> minkowski( std::vector<Ponto> P, std::vector<Ponto> Q );
double verificaSentido( const Ponto& A, const Ponto& B, const Ponto& C );
void ordenaPoligonoPorAngulo( std::vector<Ponto> &P );
Ponto calcularCentroide( const std::vector<Ponto>& P );
Ponto support(const std::vector<Ponto>& P, const Ponto& d);
double distanciaMinimaEntrePoligonos(const std::vector<Ponto>& P, const std::vector<Ponto>& Q);
std::vector<Ponto> gerarPoligonoConvexoAleatorio(int numVertices, double minX, double maxX, double minY, double maxY);
void gerarDadosParaGraficos(int numRobots, int minObstacleVertices, int maxObstacleVertices, int numSamples);

void p8g::draw() {

    background( 15, 18, 25 ); 

    for( Ponto ponto : pontos ) {
        strokeWeight( 20 ); 
        stroke( 255, 255, 255 ); 
        point( ponto.x, ponto.y );
        strokeWeight( 15 ); 
        stroke( 255, 220, 0 ); 
        point( ponto.x, ponto.y );
    }

    for ( std::vector<Ponto> vertices : poligonos ) {
        for( size_t i = 0; i < vertices.size(); i++ ) {
            const Ponto pontoA{ vertices[i] };
            const Ponto pontoB{ vertices[ ( i + 1 ) % vertices.size() ] };
            strokeWeight( 5 ); 
            stroke( 255, 102, 0 );
            line( pontoA.x, pontoA.y, pontoB.x, pontoB.y );
        }
    }

    for ( std::vector<Ponto> vertices : areasMinkowski ) {
        for( size_t i = 0; i < vertices.size(); i++ ) {
            const Ponto pontoA{ vertices[i] };
            const Ponto pontoB{ vertices[ ( i + 1 ) % vertices.size() ] };
            strokeWeight( 3 ); 
            stroke( 180, 50, 255 ); 
            line( pontoA.x, pontoA.y, pontoB.x, pontoB.y );
        }
    }

    for( size_t i = 0; i < robo.size(); i++ ) {
        const Ponto pontoA{ robo[i] };
        const Ponto pontoB{ robo[ ( i + 1 ) % robo.size() ] };
        strokeWeight( 5 ); 
        stroke( 0, 240, 255 );
        line( pontoA.x, pontoA.y, pontoB.x, pontoB.y );
    }
}

void p8g::keyPressed() {

    switch ( keyCode ) {
    case 80: /* P == 80 POLIGONO */

        if( pontos.size() < 3 ) {
            std::cout << "Adicione pelo menos 3 pontos para formar um polígono.\n";
            return;
        }

        if( existRobo ) {

            ordenaPoligonoPorAngulo( pontos );
            poligonos.push_back( pontos );

            const Ponto roboRef = calcularCentroide( robo ); 
            std::vector<Ponto> roboCSpace;

            for( const Ponto& p : robo ) {
                roboCSpace.push_back( roboRef - p ); 
            }

            ordenaPoligonoPorAngulo( roboCSpace );
            
            // CAPTURA DE DADOS MANUAL
            auto start = std::chrono::high_resolution_clock::now();
            std::vector<Ponto> cObstacle = minkowski( pontos, roboCSpace );
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            
            int nObstacle = pontos.size(); 
            int mRobot = roboCSpace.size(); 
            int nResult = cObstacle.size(); 
            long long timeUs = duration.count(); 

            // Salvar no arquivo CSV
            if ( performanceFile.is_open() ) {
                performanceFile << nObstacle << ","
                                << mRobot << ","
                                << nResult << ","
                                << timeUs << "\n";
            }
            // FIM DA CAPTURA DE DADOS

            areasMinkowski.push_back( cObstacle ); 
            pontos.clear();

        } else {
            // Define o primeiro polígono como o robô
            ordenaPoligonoPorAngulo( pontos );
            robo = pontos; 
            pontos.clear();
            existRobo = true;
            std::cout << "Robô definido. Adicione mais pontos para formar obstáculos e pressione P para processar.\n";
        }
        break;

    case 71: /* G == 71 "CAPTURA DE DADOS AUTOMÁTICA" */
        std::cout << "Iniciando geração de 500 amostras.\n";
        // Parâmetros: (numRobots, minVérticesObstáculo, maxVérticesObstáculo, numAmostras)
        gerarDadosParaGraficos( 5, 5, 100, 500 ); 
        break;

    default: break;
    }
}

void p8g::keyReleased() {}
void p8g::mouseMoved() {}
void p8g::mousePressed() { pontos.push_back( Ponto( mouseX, mouseY ) ); }
void p8g::mouseReleased() {}
void p8g::mouseWheel(float delta) {}

int main() {

    performanceFile.open( "minkowski_performance.csv" );
    if ( performanceFile.is_open() ) {
        performanceFile << "obstacle_vertices,robot_vertices,result_vertices,time_microseconds\n";
    }

    distanceFile.open( "minkowski_distances.csv" );
    if ( distanceFile.is_open() ) {
        distanceFile << "pair_id,obstacle_vertices,robot_vertices,min_distance\n";
    }

    {
        using namespace p8g;
        run( 1280, 720, "Soma de Minkowski e Coleta de Dados", true );
    }

    if ( performanceFile.is_open() ) {
        performanceFile.close();
    }
    if ( distanceFile.is_open() ) {
        distanceFile.close();
    }
    return 0;
}

std::vector<Ponto> minkowski( std::vector<Ponto> P, std::vector<Ponto> Q ){
    
    std::vector<Ponto> edgesP, edgesQ;
    // Calcula arestas (vetores)
    for ( size_t k = 0; k < P.size(); ++k ) {
        edgesP.push_back( P[(k + 1) % P.size()] - P[k] );
    }
    for ( size_t k = 0; k < Q.size(); ++k ) {
        edgesQ.push_back( Q[(k + 1) % Q.size()] - Q[k] );
    }
    std::vector<Ponto> result;
    
    // Ponto inicial: Soma dos vértices inferiores esquerdos (ponto 0 de cada)
    Ponto current = P[0] + Q[0];
    result.push_back( current );

    size_t i = 0, j = 0; 
    while( i < edgesP.size() || j < edgesQ.size() ){
        Ponto edge_to_add;
        
        // Verifica qual aresta tem o menor ângulo em relação ao eixo X positivo
        if (i < edgesP.size() && j < edgesQ.size()) {
            double cross = edgesP[i].cross( edgesQ[j] ); // Testa a orientação (ângulo)
            if (cross >= 0) { // Aresta de P tem ângulo menor ou igual
                edge_to_add = edgesP[i];
                i++;
                if (cross == 0) { // Arestas paralelas
                    edge_to_add = edge_to_add + edgesQ[j];
                    j++;
                }
            } else { // Aresta de Q tem ângulo menor
                edge_to_add = edgesQ[j];
                j++;
            }
        } else if ( i < edgesP.size() ) { // Sobrou apenas P
            edge_to_add = edgesP[i];
            i++;
        } else { // Sobrou apenas Q
            edge_to_add = edgesQ[j];
            j++;
        }
        current = current + edge_to_add;
        result.push_back( current );
    }
    
    if ( result.size() > 1 ) result.pop_back(); // Remove o último ponto que deve fechar com o primeiro
    return result;
}

// Verifica sentido (CCW > 0, Colinear = 0, CW < 0)
double verificaSentido( const Ponto& A, const Ponto& B, const Ponto& C ) {
    return (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
}

void ordenaPoligonoPorAngulo( std::vector<Ponto> &P ) {
    if ( P.size() < 2 ) return; 
    
    // Encontra o ponto de menor Y (ponto 0)
    size_t p0_index = 0;
    for ( size_t i = 1; i < P.size(); ++i ) {
        if ( P[i].y < P[p0_index].y || ( P[i].y == P[p0_index].y && P[i].x < P[p0_index].x ) ) 
            p0_index = i;
    }
    std::swap( P[0], P[p0_index] );
    const Ponto ponto0 = P[0]; 

    // Ordena os pontos restantes por ângulo em relação a p0 (CCW)
    std::sort( P.begin() + 1, P.end(), 
        [ ponto0 ]( const Ponto &a, const Ponto &b ) {
            double sentido = verificaSentido( ponto0, a, b );
            if ( sentido == 0 ) { // Pontos colineares: o mais próximo de p0 vem primeiro
                double distA_sq = ( a.x - ponto0.x ) * ( a.x - ponto0.x ) + ( a.y - ponto0.y ) * ( a.y - ponto0.y );
                double distB_sq = ( b.x - ponto0.x ) * ( b.x - ponto0.x ) + ( b.y - ponto0.y ) * ( b.y - ponto0.y );
                return distA_sq < distB_sq; 
            }
            return sentido > 0; // CCW (ângulo menor)
        }
    );
}

// Calcula o centroide de um polígono
Ponto calcularCentroide( const std::vector<Ponto>& P ) {
    if (P.empty()) return Ponto(0, 0);
    double sumX = 0, sumY = 0;
    for (const auto& p : P) {
        sumX += p.x;
        sumY += p.y;
    }
    return Ponto(sumX / P.size(), sumY / P.size());
}

// Encontra o ponto de suporte (mais extremo) na direção d
Ponto support(const std::vector<Ponto>& P, const Ponto& d) {
    double maxDot = -std::numeric_limits<double>::infinity();
    Ponto supportPoint;
    for (const auto& p : P) {
        double dot = p.x * d.x + p.y * d.y;
        if (dot > maxDot) {
            maxDot = dot;
            supportPoint = p;
        }
    }
    return supportPoint;
}

// Algoritmo para calcular a distância mínima entre P (obstáculo) e Q (robô)
double distanciaMinimaEntrePoligonos(const std::vector<Ponto>& P, const std::vector<Ponto>& Q) {
    
    // Calcula o C-Obstacle (P ⊕ (-Q))
    std::vector<Ponto> negQ;
    for(const auto& q : Q) {
        negQ.push_back(Ponto(-q.x, -q.y));
    }
    std::vector<Ponto> cObstacle = minkowski(P, negQ);

    double minDistSq = std::numeric_limits<double>::infinity();
    
    // Procura a menor distância da origem (0,0) para os segmentos do C-Obstacle
    for (size_t i = 0; i < cObstacle.size(); ++i) {
        const Ponto& A = cObstacle[i];
        const Ponto& B = cObstacle[(i + 1) % cObstacle.size()];
        Ponto AB = B - A;
        Ponto AO = Ponto(0, 0) - A;

        // Projeção do vetor AO sobre AB
        double denom = AB.lengthSq();
        double t = (denom == 0) ? 0 : (AO.x * AB.x + AO.y * AB.y) / denom;

        Ponto projectedPoint;
        if (t < 0) {
            projectedPoint = A; // Ponto mais próximo é A
        } else if (t > 1) {
            projectedPoint = B; // Ponto mais próximo é B
        } else {
            // Ponto projetado está no interior do segmento
            projectedPoint = A + Ponto(AB.x * t, AB.y * t);
        }

        if (projectedPoint.lengthSq() < minDistSq) {
            minDistSq = projectedPoint.lengthSq();
        }
    }

    return std::sqrt(minDistSq);
}

// Função para gerar um polígono convexo aleatório (base para os dados)
std::vector<Ponto> gerarPoligonoConvexoAleatorio(int numVertices, double minX, double maxX, double minY, double maxY) {
    if (numVertices < 3) return {};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distribX(minX, maxX);
    std::uniform_real_distribution<> distribY(minY, maxY);

    std::vector<Ponto> pontosGerados;
    for (int i = 0; i < numVertices; ++i) {
        pontosGerados.push_back(Ponto(distribX(gen), distribY(gen)));
    }

    ordenaPoligonoPorAngulo(pontosGerados);
    
    return pontosGerados;
}

// Função para gerar dados de desempenho e distância 
void gerarDadosParaGraficos(int numRobots, int minObstacleVertices, int maxObstacleVertices, int numSamples) {
    
    // Configuração e geração de robôs (mín. 3 vértices)
    std::vector<std::vector<Ponto>> robots;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> robotVerticesDist(3, 8);
    for (int r = 0; r < numRobots; ++r) {
        int numVert = robotVerticesDist(gen);
        robots.push_back(gerarPoligonoConvexoAleatorio(numVert, 50, 200, 50, 200)); 
    }
    
    // Pré-calcula o C-Space Negativo (-Q) para cada robô
    std::vector<std::vector<Ponto>> negRobotsCSpace;
    for(const auto& robot : robots) {
        const Ponto roboRef = calcularCentroide( robot ); 
        std::vector<Ponto> roboCSpace;
        for( const Ponto& p : robot ) {
            roboCSpace.push_back( roboRef - p ); 
        }
        ordenaPoligonoPorAngulo( roboCSpace );
        negRobotsCSpace.push_back(roboCSpace);
    }
    
    // Gera e salva os dados (loop de amostragem)
    for (int sample = 0; sample < numSamples; ++sample) {
        std::uniform_int_distribution<> obstacleVerticesDist(minObstacleVertices, maxObstacleVertices);
        int n = obstacleVerticesDist(gen);
        std::uniform_int_distribution<> robotIndexDist(0, numRobots - 1);
        int robotIdx = robotIndexDist(gen);

        // Gera Obstáculo (P)
        std::vector<Ponto> obstacle = gerarPoligonoConvexoAleatorio(n, 300, 1000, 50, 600);
        std::vector<Ponto> currentRobot = robots[robotIdx];
        std::vector<Ponto> currentNegRobotCSpace = negRobotsCSpace[robotIdx];

        // Medir o desempenho (Tempo vs Vértices)
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<Ponto> cObstacle = minkowski(obstacle, currentNegRobotCSpace);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        
        int nObstacle = obstacle.size();
        int mRobot = currentNegRobotCSpace.size();
        int nResult = cObstacle.size();
        long long timeUs = duration.count();

        if ( performanceFile.is_open() ) {
            performanceFile << nObstacle << ","
                            << mRobot << ","
                            << nResult << ","
                            << timeUs << "\n";
        }
        
        // Medir a Distância Mínima (para Tabela)
        double minDist = distanciaMinimaEntrePoligonos(obstacle, currentRobot);

        std::string pairId = std::to_string(sample) + "-" + std::to_string(robotIdx);

        if ( distanceFile.is_open() ) {
            distanceFile << pairId << "," 
                         << nObstacle << ","
                         << mRobot << ","
                         << std::fixed << std::setprecision(6) << minDist << "\n";
        }
    }
    std::cout << "Dados de performance e distancia gerados para " << numSamples << " amostras." << std::endl;
}