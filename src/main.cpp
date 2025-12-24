#include <SFML/Graphics.hpp>
#include <cmath>
#include <string.h>
#include "state.h"

constexpr double PI = 3.14159265358979323846;

const double theta1init = PI/2;
const double theta2init = 0;
const double omega1init = 1;
const double omega2init = 0.1;

const double dt = 1.0/60.0;

const double m1 = 3;
const double m2 = 5;
const float l1 = 4;
const float l2 = 5;

const float pixpermeter = 30;

#define W_WIDTH 1280u
#define W_HEIGHT 720u
#define M_RADIUS 10.f

const float ROOT_X = W_WIDTH / 2;
const float ROOT_Y = W_HEIGHT / 2;

void drawPendulum(state v, sf::RenderWindow& window, sf::CircleShape c1, sf::CircleShape c2, sf::RectangleShape r1, sf::RectangleShape r2);

int main()
{
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({W_WIDTH, W_HEIGHT}), "double pendulum");
    window.setFramerateLimit(60);
    sf::Font font;
    if (!font.openFromFile("src/arial.ttf"))
    {
        exit(1);
    }

    // My drawing/shape variables
    auto windowColor = sf::Color::White;
    state currstate = state(theta1init, theta2init, omega1init, omega2init);
    sf::CircleShape circ_m1(M_RADIUS);
    circ_m1.setFillColor(sf::Color(255, 255, 255));
    circ_m1.setPointCount(50);
    circ_m1.setOrigin({M_RADIUS/2, M_RADIUS/2});

    sf::CircleShape circ_m2(M_RADIUS);
    circ_m2.setFillColor(sf::Color(255, 255, 255));
    circ_m2.setPointCount(50);
    circ_m2.setOrigin({M_RADIUS/2, M_RADIUS/2});

    sf::RectangleShape rope1({l1 * pixpermeter, 5.f});
    rope1.setFillColor(sf::Color::White);
    rope1.setOrigin({0, 2.5});

    sf::RectangleShape rope2({l2 * pixpermeter, 5.f});
    rope2.setFillColor(sf::Color::White);
    rope2.setOrigin({0, 2.5});

    int count = 0;
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
                windowColor = sf::Color::Red;
            } 
            
        }
        window.clear();

        // drawPendulum(currstate);
        // updateText(currstate);

        sf::Text text(font); // a font is required to make a text object

        // set the string to display

        count++;
        double ke1 = 0.5 * m1 * l1 * l1 * currstate[2] * currstate[2];
        double pe1 = m1 * g * l1 * cos(currstate[0]);
        double v2x = l1 * cos(currstate[0]) * currstate[2] + l2 * cos(currstate[1]) * currstate[3];
        double v2y = l1 * sin(currstate[0]) * currstate[2] + l2 * sin(currstate[1]) * currstate[3];
        double ke2 = 0.5 * m2 * (v2x*v2x + v2y*v2y);
        double pe2 = m2 * g * (l1 * cos(currstate[0]) + l2 * cos(currstate[1]));
        double total_energy = pe1 + pe2 + ke1 + ke2;
        text.setString(std::to_string(total_energy));

        // set the character size
        text.setCharacterSize(100); // in pixels, not points!

        // set the color
        text.setFillColor(sf::Color::White);

        // set the text style
        text.setStyle(sf::Text::Italic | sf::Text::Underlined);
        text.setPosition({15, 55});
        // inside the main loop, between window.clear() and window.display()
        window.draw(text);
        state next = rk4(currstate, m1, m2, l1, l2, dt);
        drawPendulum(next, window, circ_m1, circ_m2, rope1, rope2);
        currstate = next;
        window.display();
    }
}

void drawPendulum(state v, sf::RenderWindow& window, sf::CircleShape c1, sf::CircleShape c2, sf::RectangleShape r1, sf::RectangleShape r2)
{
    // Get values to use in window rendering
    float r1_angle = v[0] + PI/2.0;
    float r2_angle = v[1] + PI/2.0;

    // m1 cords
    float x_1 = ROOT_X - (l1 * pixpermeter) * sin(v[0]);
    float y_1 = ROOT_Y + (l1 * pixpermeter) * cos(v[0]);

    // m2 cords
    float x_2 = x_1 - (l2 * pixpermeter) * sin(v[1]);
    float y_2 = y_1 + (l2 * pixpermeter) * cos(v[1]);

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