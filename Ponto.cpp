#include "Ponto.hpp"

Ponto::Ponto( double x, double y ) : x( x ), y( y ) {}
Ponto Ponto::operator + ( const Ponto &p ) const {
    return Ponto{ x + p.x, y + p.y };
}
Ponto Ponto::operator - ( const Ponto &p ) const {
    return Ponto{ x - p.x, y - p.y };
}
double Ponto::cross( const Ponto &p ) const {
    return x * p.y - y * p.x; // double * double resulta em double
}