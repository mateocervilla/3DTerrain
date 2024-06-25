#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

#include "PerlinNoise.hpp"

constexpr int WIDTH = 1800;
constexpr int HEIGHT = 800;

// Define a 3D point
struct Point3D {
    float x, y, z;
};

class Terrain {
private:

    std::shared_ptr<sf::RenderWindow> m_window;
    sf::VideoMode m_videoMode;
    sf::Event m_ev;
    bool m_isWindowFocused;

    void initVariables();
    void initWindow();
    void initEnviroment();
    void drawTriangleStrip(sf::RenderWindow& window, const std::vector<Point3D>& vertices, float angleX, int colorVal = 255);
    sf::Vector2f projectPoint(const Point3D& point, float angleX, const sf::Vector2u& windowSize);

    float m_scale;
    sf::Vector2i m_gridSize;
    std::vector<std::vector<Point3D>> m_vertices;
    float m_fly;
    siv::PerlinNoise m_perlin;

    sf::Clock m_clock;

public:
    Terrain();
    virtual ~Terrain();

    const bool running() const;

    void pollEvents();
    void update();
    void render();
};