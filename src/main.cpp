#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include "Terrain.hpp"

int main() {
    Terrain terrain;

    while (terrain.running())
    {
        terrain.update();

        terrain.render();
    }

    return 0;
}