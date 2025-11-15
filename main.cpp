#include "p8g/p8g.hpp"
#include "Ponto.hpp"
#include <iostream>
#include <algorithm>
#include <vector>

#define WIDTH  960
#define LENGHT 540

std::vector<std::vector<Ponto>> poligonos;
std::vector<Ponto> robo;
bool existRobo = false;
std::vector<Ponto> pontos;

void p8g::draw() {
    for ( std::vector<Ponto> vertices : poligonos ) {
        for( size_t i = 0; i < vertices.size(); i++ ) {
            Ponto pontoA{ vertices[i] };
            Ponto pontoB{ vertices[ ( i + 1 ) % vertices.size() ] };
        }
    }
}
void p8g::keyPressed() {
    switch ( keyCode )
    {
    case 80: /* P == 80 POLIGONOS */
        poligonos.push_back( pontos );
        pontos.clear();
    case 82: /* R == 80 ROBO */
        if( !existRobo ){
            robo = pontos; /* Cria uma cópia dos dados do vetor de pontos para o vetor robo */
            pontos.clear();
        }
        
    default:
        break;
    }
}
void p8g::keyReleased() {}
void p8g::mouseMoved() {}
void p8g::mousePressed() {

    pontos.push_back( Ponto( mouseX, mouseY ) );

}
void p8g::mouseReleased() {}
void p8g::mouseWheel(float delta) {}

int main() {

    {
        using namespace p8g;
        run( WIDTH, LENGHT, "Soma de Minkowski", true );
    }

}

void reorderPolygon( std::vector<Ponto> &P ){

    size_t pos = 0;

    for( size_t i = 1; i < P.size(); i++ ){
        if( P[i].y < P[pos].y || ( P[i].y == P[pos].y && P[i].x < P[pos].x ) )
            pos = i;
    }

    std::rotate( P.begin(), P.begin() + pos, P.end() );

}

std::vector<Ponto> minkowski( std::vector<Ponto> P, std::vector<Ponto> Q ){

    // the first vertex must be the lowest
    reorderPolygon( P );
    reorderPolygon( Q );
    // we must ensure cyclic indexing
    P.push_back( P[0] );
    P.push_back( P[1] );
    Q.push_back( Q[0] );
    Q.push_back( Q[1] );
    // main part
    std::vector<Ponto> result;
    size_t i = 0, j = 0;
    while( i < P.size() - 2 || j < Q.size() - 2 ){

        result.push_back( P[i] + Q[j] );
        auto cross = ( P[i + 1] - P[i] ).cross( Q[j + 1] - Q[j] );

        if( cross >= 0 && i < P.size() - 2 )
            ++i;
        if( cross <= 0 && j < Q.size() - 2 )
            ++j;

    }

    return result;

}