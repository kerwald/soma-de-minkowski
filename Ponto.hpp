
struct Ponto{

        long long x, y;
        Ponto( double x, double y );
        Ponto operator + ( const Ponto &p ) const;
        Ponto operator - ( const Ponto &p ) const;
        long long cross( const Ponto &p ) const;
        
};