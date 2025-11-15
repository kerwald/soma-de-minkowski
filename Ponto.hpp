struct Ponto{

    double x, y; 

    Ponto( double x = 0.0, double y = 0.0 );
    Ponto operator + ( const Ponto &p ) const;
    Ponto operator - ( const Ponto &p ) const;
    double cross( const Ponto &p ) const; 
    
};