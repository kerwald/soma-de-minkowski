#include "p8g/p8g.hpp"
#include "Ponto.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

#define WIDTH  960
#define LENGHT 540

std::vector<std::vector<Ponto>> poligonos;
std::vector<std::vector<Ponto>> areasMinkowski;
std::vector<Ponto> robo;
bool existRobo = false;
std::vector<Ponto> pontos;


std::vector<Ponto> minkowski( std::vector<Ponto> P, std::vector<Ponto> Q );
double verificaSentido( const Ponto& A, const Ponto& B, const Ponto& C );
void ordenaPoligonoPorAngulo( std::vector<Ponto> &P );
Ponto calcularCentroide( const std::vector<Ponto>& P );

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
            strokeWeight( 5 ); stroke( 255, 102, 0 );
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

        strokeWeight( 5 ); stroke( 0, 240, 255 );
        line( pontoA.x, pontoA.y, pontoB.x, pontoB.y );

    }
}

void p8g::keyPressed() {
    switch ( keyCode ) {
    case 80: /* P == 80 POLIGONO */
        if( existRobo ){

            ordenaPoligonoPorAngulo( pontos );
            poligonos.push_back( pontos );

            const Ponto roboRef = calcularCentroide( robo ); 
            
            // Cria o Robô Refletido e Centrado na Origem (0,0)
            // A matemática aqui é: p_local = (0,0) - (p_mundo - centro) -> simplificado para: centro - p_mundo
            std::vector<Ponto> roboCSpace;
            for( const Ponto& p : robo ) {
                roboCSpace.push_back( roboRef - p ); 
            }
            // Ordena para o algoritmo de Minkowski funcionar
            ordenaPoligonoPorAngulo( roboCSpace );
            // Calcula Minkowski
            // A mágica acontece aqui: A função soma (Obstáculo no Mundo) + (Robô no 0,0).
            // O resultado é automaticamente o C-Obstacle nas coordenadas corretas do mundo.
            std::vector<Ponto> C_Obstacle = minkowski( pontos, roboCSpace );
            
            areasMinkowski.push_back( C_Obstacle ); 
            pontos.clear();

        } else {
            ordenaPoligonoPorAngulo( pontos );
            robo = pontos; 
            pontos.clear();
            existRobo = true;
        }
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
    using namespace p8g;
    run( WIDTH, LENGHT, "Soma de Minkowski", true );
}

std::vector<Ponto> minkowski( std::vector<Ponto> P, std::vector<Ponto> Q ){

    std::vector<Ponto> edgesP, edgesQ;
    // Calcula arestas
    for ( size_t k = 0; k < P.size(); ++k ) {
        edgesP.push_back( P[(k + 1) % P.size()] - P[k] );
    }
    for ( size_t k = 0; k < Q.size(); ++k ) {
        edgesQ.push_back( Q[(k + 1) % Q.size()] - Q[k] );
    }
    std::vector<Ponto> result;
    
    // Ponto inicial: Soma dos vértices inferiores esquerdos
    // P[0] é coordenada do mundo. Q[0] é coordenada relativa (negativa).
    // A soma resulta na posição correta "expandida" para a esquerda/baixo.
    Ponto current = P[0] + Q[0];
    result.push_back( current );

    size_t i = 0, j = 0; 
    while( i < edgesP.size() || j < edgesQ.size() ){
        Ponto edge_to_add;
        if (i < edgesP.size() && j < edgesQ.size()) {
            double cross = edgesP[i].cross( edgesQ[j] );
            if (cross >= 0) { 
                edge_to_add = edgesP[i];
                i++;
                if (cross == 0) { 
                    edge_to_add = edge_to_add + edgesQ[j];
                    j++;
                }
            } else { 
                edge_to_add = edgesQ[j];
                j++;
            }
        } else if ( i < edgesP.size() ) {
            edge_to_add = edgesP[i];
            i++;
        } else { 
            edge_to_add = edgesQ[j];
            j++;
        }
        current = current + edge_to_add;
        result.push_back( current );
    }
    if ( result.size() > 1 ) result.pop_back(); 
    return result;

}

double verificaSentido( const Ponto& A, const Ponto& B, const Ponto& C ) {
    return (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
}

void ordenaPoligonoPorAngulo( std::vector<Ponto> &P ) {
    
    if ( P.size() < 2 ) return; 
    size_t p0_index = 0;
    for ( size_t i = 1; i < P.size(); ++i ) {
        if ( P[i].y < P[p0_index].y || ( P[i].y == P[p0_index].y && P[i].x < P[p0_index].x ) ) 
            p0_index = i;
    }
    std::swap( P[0], P[p0_index] );
    const Ponto ponto0 = P[0]; 

    std::sort( P.begin() + 1, P.end(), 
        [ ponto0 ]( const Ponto &a, const Ponto &b ) {
            double sentido = verificaSentido( ponto0, a, b );
            if ( sentido == 0 ) { 
                double distA_sq = ( a.x - ponto0.x ) * ( a.x - ponto0.x ) + ( a.y - ponto0.y ) * ( a.y - ponto0.y );
                double distB_sq = ( b.x - ponto0.x ) * ( b.x - ponto0.x ) + ( b.y - ponto0.y ) * ( b.y - ponto0.y );
                return distA_sq < distB_sq; 
            }
            return sentido > 0;
        }
    );
}

Ponto calcularCentroide( const std::vector<Ponto>& P ) {
    if (P.empty()) return Ponto(0, 0);
    double sumX = 0, sumY = 0;
    for (const auto& p : P) {
        sumX += p.x;
        sumY += p.y;
    }
    return Ponto(sumX / P.size(), sumY / P.size());
}