#include "Terrain.hpp"
#include <math.h>
#include <iostream>
#include <random>



float mapRange(float value, float inputMin, float inputMax, float outputMin, float outputMax) {
    return outputMin + ((value - inputMin) / (inputMax - inputMin)) * (outputMax - outputMin);
}

Terrain::Terrain() {
    initVariables();
    initWindow();
    initEnviroment();
}

Terrain::~Terrain() {
}

// Function to project 3D points onto 2D space
sf::Vector2f Terrain::projectPoint(const Point3D& point, float angleX, const sf::Vector2u& windowSize) {
    // Rotation around x-axis
    float cosAngleX = std::cos(angleX);
    float sinAngleX = std::sin(angleX);
    float x = point.x;
    float y = point.y;
    float z = point.z;

    // Rotate X axis
    y = point.y * cosAngleX - point.z * sinAngleX;
    z = point.y * sinAngleX + point.z * cosAngleX;

    // Rotate Y axis
    // x = point.x * cosAngleX + z * sinAngleX;
    // z = - point.x * sinAngleX + z * cosAngleX;

    // // Rotate Z axis
    // x = x * cosAngleX - y * sinAngleX;
    // y = x * sinAngleX + y * cosAngleX;

    // Simple orthographic projection
    float scaleFactor = 500 / (500 + z);
    float y2D = (y - 100) * scaleFactor + windowSize.y / 2;
    float x2D = x * scaleFactor + windowSize.x / 2;

    return sf::Vector2f(x2D, y2D);
}

// Function to draw the cube edges
void Terrain::drawTriangleStrip(sf::RenderWindow& window, const std::vector<Point3D>& vertices, float angleX, int colorVal) {
    sf::Vector2u windowSize = window.getSize();

    sf::Color lineColor;
    bool black = false;
    for (int i = 0; i < vertices.size() - 2; i++) {
        if(!black){
            lineColor = sf::Color(colorVal,0,colorVal,255);
        }
        else {
            lineColor = sf::Color(0,0,0,255);
        }
        sf::Vector2f point1 = projectPoint(vertices[i], angleX, windowSize);
        sf::Vector2f point2 = projectPoint(vertices[i+1], angleX, windowSize);
        sf::Vector2f point3 = projectPoint(vertices[i+2], angleX, windowSize);

        sf::Vertex line[] = {
            sf::Vertex(point1, lineColor),
            sf::Vertex(point2, lineColor),
            sf::Vertex(point2, lineColor),
            sf::Vertex(point3, lineColor),
            sf::Vertex(point3, lineColor),
            sf::Vertex(point1, lineColor)
        };

        // window.draw(line, 3, sf::Triangles);
        window.draw(line, 6, sf::Lines);
        black = !black;
    }
}

void Terrain::initVariables() {
    m_window = nullptr;
    m_isWindowFocused = true;
    m_scale = 20;

    const siv::PerlinNoise::seed_type seed = 123456u;
	m_perlin = siv::PerlinNoise{ seed };
}

void Terrain::initWindow() {
    m_videoMode.height = HEIGHT;
    m_videoMode.width = WIDTH;

    m_window = std::make_shared<sf::RenderWindow>(m_videoMode, "Terrain", sf::Style::Close);
    m_window->setFramerateLimit(75);
}

void Terrain::initEnviroment() {
    // Initialize random number generators
    float width = 3200 / m_scale;
    float height = 1000 / m_scale;

    m_vertices.resize(height);
    for(int y = -height/2; y < height/2; y+=1) {
        for(int x = -width/2; x < width/2; x++) {
            m_vertices[y+height/2].push_back({x * m_scale, y * m_scale, 0});
            m_vertices[y+height/2].push_back({x * m_scale, (y + 1) * m_scale, 0});
        }
    }

    m_fly = 0;
}

void Terrain::update() {
    pollEvents();
    m_fly -= 0.1;
}

void Terrain::render() {

    /*
        - clear old frame
        - render objects
        - display frame in window

        Renders objects
    */

    m_window->clear(sf::Color::Black);

    // Set Z and draw
    for(int y = 0; y < m_vertices.size(); y++) {
        for(int i = 0; i < m_vertices[y].size(); i+=2) {
            int x = i / 2;
            float noise2D = mapRange(m_perlin.noise2D_01((x * 0.1), ((y + m_fly) * 0.1)), 0, 1, -100, 100);
            float noise2D2 = mapRange(m_perlin.noise2D_01((x * 0.1), ((y + 1 + m_fly) * 0.1)), 0, 1, -100, 100);
            m_vertices[y][i].z = noise2D;
            m_vertices[y][i+1].z = noise2D2;
        }

        // Intensity increase
        // double intensity = std::pow((double)y / (double)m_vertices.size(),2); // Cuadratic (Slowest)
        // double intensity = (double) y / m_vertices.size(); // Linear (Mid)
        double intensity = std::sqrt((double)y / (double)m_vertices.size()); // Square root (Fastest)

        drawTriangleStrip(*m_window, m_vertices[y], (-60.f / 180.f) * M_PI, intensity * 255);
    }

    m_window->display();
}

void Terrain::pollEvents() {
    while (m_window->pollEvent(m_ev)) {
        switch (m_ev.type) {
            case sf::Event::Closed:
                m_window->close();
                break;
            // Window focus events
            case sf::Event::GainedFocus:
                m_isWindowFocused = true;
                break;
            case sf::Event::LostFocus:
                m_isWindowFocused = false;
                break;
        }
    }
}

const bool Terrain::running() const {
    return m_window->isOpen();
}
