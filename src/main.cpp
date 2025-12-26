#define FMT_HEADER_ONLY

#include <SFML/Graphics.hpp>
#include <cmath>
#include <string.h>
#include "state.h"
#include "node.h"
#include <vector>
#include <sstream>
#include <print>
#include <fmt/core.h>

constexpr double PI = 3.14159265358979323846;

const double theta1init = -PI;
const double theta2init = PI/2;
const double omega1init = 2;
const double omega2init = -2;

const double dt = 0.01/144.0;

const double m1 = 2;
const double m2 = 2;
const float l1 = 3;
const float l2 = 2;

const float pixpermeter = 30;

#define W_WIDTH 1280u
#define W_HEIGHT 720u
#define M_RADIUS 10.f

const float ROOT_X = W_WIDTH / 2;
const float ROOT_Y = W_HEIGHT / 2;

void drawPendulum(state v, sf::RenderWindow& window, sf::CircleShape c1, sf::CircleShape c2, sf::RectangleShape r1, sf::RectangleShape r2);
void drawPath(std::vector<sf::RectangleShape> lines, sf::RenderWindow& window);
float const distance(float x1, float x2, float y1, float y2);
float const sfml_angle(float x1, float y1, float x2, float y2);

int main()
{
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({W_WIDTH, W_HEIGHT}), "double pendulum");
    window.setFramerateLimit(144);
    sf::Font font;
    if (!font.openFromFile("src/Fixedsys62.ttf"))
    {
        exit(1);
    }

    // My drawing/shape variables
    auto windowColor = sf::Color::White;
    state currstate = state(theta1init, theta2init, omega1init, omega2init);
    sf::CircleShape circ_m1(M_RADIUS);
    circ_m1.setFillColor(sf::Color(255, 255, 255));
    circ_m1.setPointCount(50);
    circ_m1.setOrigin({M_RADIUS, M_RADIUS});

    sf::CircleShape circ_m2(M_RADIUS);
    circ_m2.setFillColor(sf::Color(255, 255, 255));
    circ_m2.setPointCount(50);
    circ_m2.setOrigin({M_RADIUS, M_RADIUS});

    sf::RectangleShape rope1({l1 * pixpermeter, 5.f});
    rope1.setFillColor(sf::Color::White);
    rope1.setOrigin({0, 2.5});

    sf::RectangleShape rope2({l2 * pixpermeter, 5.f});
    rope2.setFillColor(sf::Color::White);
    rope2.setOrigin({0, 2.5});

    std::vector<sf::RectangleShape> lines;

    bool paused = true;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (event->is<sf::Event::MouseButtonPressed>())
            {
                paused = !paused;
            } 
            
        }
        window.clear();

        double ke1 = 0.5 * m1 * l1 * l1 * currstate[2] * currstate[2];
        double pe1 = -m1 * g * l1 * sin(currstate[0]);
        double v2x = -l1 * sin(currstate[0]) * currstate[2] - l2 * sin(currstate[1]) * currstate[3];
        double ke2 = 0.5 * m2 * (l1*l1*currstate[2]*currstate[2] + l2*l2*currstate[3]*currstate[3] + 2*l1*l2*currstate[2]*currstate[3]*cos(currstate[1]-currstate[0]));
        double pe2 = -m2 * g * (l1 * sin(currstate[0]) + l2 * sin(currstate[1]));
        double total_pe = pe1 + pe2;
        double total_ke = ke1 + ke1;
        double total_energy = pe1 + pe2 + ke1 + ke2;

        sf::Text energy_text(font);
        std::stringstream ss;
        ss << "KE1: " << std::fixed << std::setw(10) << std::setprecision(5) << ke1 << " J\nKE2: " << std::setprecision(5) << ke2 << " J\nPE1: " << std::fixed << std::setprecision(5) << pe1 << " J\nPE2: " << std::setprecision(5) << pe2 << " J\nTotal: " << std::setprecision(5) << total_energy << " J";
        std::string energy_string = fmt::format("{:>5s}: {:10.6f} J\n{:>5s}: {:10.6f} J\n{:>5s}: {:10.6f} J\n{:>5s}: {:10.6f} J\n{:>5s}: {:10.6f} J", "KE1", ke1, "KE2", ke2, "PE1", pe1, "PE2", pe2, "TOTAL", total_energy);
        energy_text.setString(energy_string);

        // set the character size
        energy_text.setCharacterSize(18); // in pixels, not points!

        // set the color
        energy_text.setFillColor(sf::Color::White);
        
        // set the text style
        energy_text.setStyle(sf::Text::Italic);
        energy_text.setPosition({60, 550});
        // inside the main loop, between window.clear() and window.display()
        window.draw(energy_text);
        if (!paused)
        {
            float line_width = 5.f;

            float xi = currstate.x2(ROOT_X, l1, l2, pixpermeter);
            float yi = currstate.y2(ROOT_Y, l1, l2, pixpermeter);
            for (int i = 0; i < 100; ++i)
            {
                currstate = rk4(currstate, m1, m2, l1, l2, dt);
            }
            // Add a 
            float xf = currstate.x2(ROOT_X, l1, l2, pixpermeter);
            float yf = currstate.y2(ROOT_Y, l1, l2, pixpermeter);
            float d = distance(xf, yf, xi, yi);
            sf::RectangleShape line({d, line_width});
            line.setFillColor(sf::Color::Blue);
            line.setOrigin({0, line_width/2});
            line.setPosition({xi, yi});
            line.setRotation(sf::radians(sfml_angle(xi, yi, xf, yf)));
            lines.push_back(line);
        }
        drawPath(lines, window);
        drawPendulum(currstate, window, circ_m1, circ_m2, rope1, rope2);
        window.display();
    }
}

void drawPendulum(state v, sf::RenderWindow& window, sf::CircleShape c1, sf::CircleShape c2, sf::RectangleShape r1, sf::RectangleShape r2)
{
    // Get values to use in window rendering
    float r1_angle = v[0];
    float r2_angle = v[1];

    // m1 cords
    float x_1 = v.x1(ROOT_X, l1, pixpermeter);
    float y_1 = v.y1(ROOT_Y, l1, pixpermeter);

    // m2 cords
    float x_2 = v.x2(ROOT_X, l1, l2, pixpermeter);
    float y_2 = v.y2(ROOT_Y, l1, l2, pixpermeter);

    r1.setPosition({ROOT_X, ROOT_Y});
    r1.setRotation(sf::radians(r1_angle));

    c1.setPosition({x_1, y_1});

    r2.setPosition({x_1, y_1});
    r2.setRotation(sf::radians(r2_angle));

    c2.setPosition({x_2, y_2});

    window.draw(c1);
    window.draw(c2);
    window.draw(r1);
    window.draw(r2);
}

void drawPath(std::vector<sf::RectangleShape> lines, sf::RenderWindow& window)
{
    for (sf::RectangleShape line : lines)
    {
        window.draw(line);
    }
}

float const distance(float x1, float y1, float x2, float y2)
{
    float x = (x2 - x1);
    float y = (y2 - y1);
    return sqrt(x*x + y*y);
}

float const sfml_angle(float x1, float y1, float x2, float y2)
{
    float x = (x2 - x1);
    float y = (y2 - y1);
    return -atan2(y, x);
}