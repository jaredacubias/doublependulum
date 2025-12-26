#define FMT_HEADER_ONLY

#include <SFML/Graphics.hpp>
#include <cmath>
#include <string.h>
#include "state.h"
#include <queue>
#include <fmt/core.h>
#include <random>
#include "linepath.h"

#define W_WIDTH 1280u
#define W_HEIGHT 720u
#define M_RADIUS 10.f
#define FPS 144

const sf::Color PATH_COLOR1 = sf::Color::Red;
const sf::Color PATH_COLOR2 = sf::Color::Cyan;

constexpr double PI = 3.14159265358979323846;

const double theta1init = -PI;
const double theta2init = PI/2;
const double omega1init = 2;
const double omega2init = -2;

const double dt = 0.01/FPS;

const double m1 = 2;
const double m2 = 2;
const float l1 = 3;
const float l2 = 2;

const float pixpermeter = 30;



const float ROOT_X = W_WIDTH / 2;
const float ROOT_Y = W_HEIGHT / 2;

void drawPendulum(State v, sf::RenderWindow& window, sf::CircleShape c1, sf::CircleShape c2, sf::RectangleShape r1, sf::RectangleShape r2);
float const distance(float x1, float x2, float y1, float y2);
float const sfml_angle(float x1, float y1, float x2, float y2);

int main(int argc, char* argv[])
{
    State currState;
    State currState2;
    if (argc != 5)
    {
        std::random_device rd;
        // Standard mersenne_twister_engine seeded with rd()
        std::mt19937 gen(rd());
        // Define the range [0.0, 1.0)
        std::uniform_real_distribution<double> angle(0.0, 2*PI);
        std::uniform_real_distribution<double> omega(-1, 1);
        currState = State(angle(gen), angle(gen), omega(gen), omega(gen));
        currState2 = State(currState[0] + 0.01, currState[1] + 0.01, currState[2], currState[3]);
    }
    else 
    {
        currState = State(theta1init, theta2init, omega1init, omega2init);
        currState2 = State(currState[0] + 0.01, currState[1] + 0.01, currState[2], currState[3]);
    }
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({W_WIDTH, W_HEIGHT}), "double pendulum");
    window.setFramerateLimit(FPS);
    sf::Font font;
    if (!font.openFromFile("src/Fixedsys62.ttf"))
    {
        exit(1);
    }

    // My drawing/shape variables
    auto windowColor = sf::Color::White;
    

    // Pen 1
    sf::CircleShape circ1_m1(M_RADIUS);
    circ1_m1.setFillColor(sf::Color(255, 255, 255));
    circ1_m1.setPointCount(50);
    circ1_m1.setOrigin({M_RADIUS, M_RADIUS});

    sf::CircleShape circ1_m2(M_RADIUS);
    circ1_m2.setFillColor(sf::Color(255, 255, 255));
    circ1_m2.setPointCount(50);
    circ1_m2.setOrigin({M_RADIUS, M_RADIUS});

    sf::RectangleShape rope1_1({l1 * pixpermeter, 5.f});
    rope1_1.setFillColor(sf::Color::White);
    rope1_1.setOrigin({0, 2.5});

    sf::RectangleShape rope1_2({l2 * pixpermeter, 5.f});
    rope1_2.setFillColor(sf::Color::White);
    rope1_2.setOrigin({0, 2.5});

    LinePath path1(PATH_MAX, PATH_COLOR1);
    path1.add(currState.x2(ROOT_X, l1, l2, pixpermeter), currState.y2(ROOT_Y, l1, l2, pixpermeter));

    // Pen 2
    sf::CircleShape circ2_m1(M_RADIUS);
    circ2_m1.setFillColor(sf::Color::Blue);
    circ2_m1.setPointCount(50);
    circ2_m1.setOrigin({M_RADIUS, M_RADIUS});

    sf::CircleShape circ2_m2(M_RADIUS);
    circ2_m2.setFillColor(sf::Color::Blue);
    circ2_m2.setPointCount(50);
    circ2_m2.setOrigin({M_RADIUS, M_RADIUS});

    sf::RectangleShape rope2_1({l1 * pixpermeter, 5.f});
    rope2_1.setFillColor(sf::Color::Blue);
    rope2_1.setOrigin({0, 2.5});

    sf::RectangleShape rope2_2({l2 * pixpermeter, 5.f});
    rope2_2.setFillColor(sf::Color::Blue);
    rope2_2.setOrigin({0, 2.5});

    LinePath path2(PATH_MAX, PATH_COLOR2);
    path1.add(currState2.x2(ROOT_X, l1, l2, pixpermeter), currState2.y2(ROOT_Y, l1, l2, pixpermeter));

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

        double ke1 = 0.5 * m1 * l1 * l1 * currState[2] * currState[2];
        double pe1 = -m1 * g * l1 * sin(currState[0]);
        double v2x = -l1 * sin(currState[0]) * currState[2] - l2 * sin(currState[1]) * currState[3];
        double ke2 = 0.5 * m2 * (l1*l1*currState[2]*currState[2] + l2*l2*currState[3]*currState[3] + 2*l1*l2*currState[2]*currState[3]*cos(currState[1]-currState[0]));
        double pe2 = -m2 * g * (l1 * sin(currState[0]) + l2 * sin(currState[1]));
        double total_energy = pe1 + pe2 + ke1 + ke2;
        
        sf::Text energy_text(font);
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
            path1.add(currState.x2(ROOT_X, l1, l2, pixpermeter), currState.y2(ROOT_Y, l1, l2, pixpermeter));
            path2.add(currState2.x2(ROOT_X, l1, l2, pixpermeter), currState2.y2(ROOT_Y, l1, l2, pixpermeter));
            for (int i = 0; i < 100; ++i)
            {
                currState = rk4(currState, m1, m2, l1, l2, dt);
                currState2 = rk4(currState2, m1, m2, l1, l2, dt);
            }
        }
        window.draw(path1);
        drawPendulum(currState, window, circ1_m1, circ1_m2, rope1_1, rope1_2);

         window.draw(path2);
        drawPendulum(currState2, window, circ2_m1, circ2_m2, rope2_1, rope2_2);
        window.display();
    }
}

void drawPendulum(State v, sf::RenderWindow& window, sf::CircleShape c1, sf::CircleShape c2, sf::RectangleShape r1, sf::RectangleShape r2)
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