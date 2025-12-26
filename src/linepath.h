#ifndef LINEPATH_H
#define LINEPATH_H
#include <SFML/Graphics.hpp>
class LinePath : public sf::Drawable, public sf::Transformable
{
    public:
        LinePath(int max, sf::Color color) : max_size(max), head(0), count(0) {
            m_vertices.setPrimitiveType(sf::PrimitiveType::LineStrip);
            m_vertices.resize(max);
            path_color = color;
        }

        void add(float x, float y)
        {
            if (count != max_size) count++;
            sf::Vertex vertex;
            vertex.position = sf::Vector2f(x, y);
            vertex.color = path_color;
            m_vertices[head] = vertex;
            head = (head + 1) % max_size;
            //update_color();
        }

    private: 
        int count;
        int head;
        int max_size;
        sf::Color path_color;
        sf::VertexArray m_vertices;
        sf::Texture m_texture;
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override
        {
            // apply the entity's transform -- combine it with the one that was passed by the caller
            states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

            // apply the texture
            states.texture = &m_texture;

            // you may also override states.shader or states.blendMode if you want
            // create the m_verticies array
            // draw the vertex array
            if (count < max_size)
            {
                target.draw(&m_vertices[0], count, sf::PrimitiveType::LineStrip, states);
            }
            else
            {
                sf::VertexArray gap;
                gap.resize(2);
                gap[0] = m_vertices[head];
                gap[1] = m_vertices[0];
                target.draw(&m_vertices[head], max_size - head, sf::PrimitiveType::LineStrip, states);
                target.draw(&m_vertices[0], head, sf::PrimitiveType::LineStrip, states);
                target.draw(gap, states);
            }
        }

        void update_color()
        {
            for (int i = 0; i < count; i++)
            {
                int age = (i- head) % max_size;
                int brightness = 255.0 / (1.0 + age);
                m_vertices[i].color.a = brightness;
            }
        }

        
};

#endif