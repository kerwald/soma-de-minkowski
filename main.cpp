#include "p8g/p8g.hpp"

#define WIDTH  960
#define LENGHT 540

void p8g::draw() {}
void p8g::keyPressed() {}
void p8g::keyReleased() {}
void p8g::mouseMoved() {}
void p8g::mousePressed() {}
void p8g::mouseReleased() {}
void p8g::mouseWheel(float delta) {}

int main() {

    {
        using namespace p8g;
        run( WIDTH, LENGHT, "Soma de Minkowski", true );
    }

}
