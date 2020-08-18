// SwiftBlocks v0.6
// Micha³ Jab³onowski

#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include<SFML/System.hpp>

#include<cmath>
#include<iostream>
#include<string>
#include<vector>
#include<math.h>
#include<cstdlib>
#include<stdlib.h>

using namespace std;
using namespace sf;



// FIZYKA -------------------------------------------------------

class Data
{
public:
    float force = 125.0;
    float gravity_acceleration = 10.0;
};

class Physic
{
public:
    float mass = 0.0;
    float velocity = 0.0;
    float acceleration = 0.0;
};


// --------------------------------------------------------------


// POSTAC -------------------------------------------------------

class Player :public Physic
{
public:
    RectangleShape sprite_player;
    Vector2f GetPosition() { return sprite_player.getPosition(); };
    int health = 3;
    const float MAX_LEFT = 0.0;
    const float MAX_RIGHT = 4980.0;

    struct mass_center
    {
        float x;
        float y;
    };

    Player()
    {
        sprite_player.setSize(Vector2f(20, 20));
        sprite_player.setFillColor(Color::Green);
        sprite_player.setPosition(0, 380);
    }

    mass_center getMassCenter()
    {
        mass_center mc;
        mc.x = sprite_player.getPosition().x + sprite_player.getPosition().x / 2;
        mc.y = sprite_player.getPosition().y + sprite_player.getPosition().y / 2;
        return mc;
    }

    void jump(float deltaTime)
    {
        Data data;
        velocity -= data.gravity_acceleration * deltaTime;
        sprite_player.move(0, -velocity);
    }

    void player_move(float deltaTime)
    {
        if (Keyboard::isKeyPressed(Keyboard::Right)) {
            sprite_player.move(400 * deltaTime, 0);
            if (sprite_player.getPosition().x > 4980) {
                sprite_player.move(-400 * deltaTime, 0);
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            sprite_player.move(-400 * deltaTime, 0);
            if (sprite_player.getPosition().x < 0) {
                sprite_player.move(400 * deltaTime, 0);
            }
        }
    }
};

// --------------------------------------------------------------------


// PLATFORMY ----------------------------------------------------------


class Platform : public Drawable, public Transformable
{
public:
    RectangleShape sprite_platform;
    Vector2f GetPosition() { return sprite_platform.getPosition(); };

    Platform()
    {
        sprite_platform.setSize(Vector2f(125, 50));
        sprite_platform.setFillColor(Color::Red);
    }

private:
    virtual void draw(RenderTarget& target, RenderStates states) const
    {
        states.transform *= getTransform();
        target.draw(m_vertices, states);
    }

    VertexArray m_vertices;

};


class Platform_Destroy : public Drawable, public Transformable
{
public:
    RectangleShape sprite_destroy;
    Vector2f GetPosition() { return sprite_destroy.getPosition(); };

    Platform_Destroy()
    {
        sprite_destroy.setSize(Vector2f(125, 50));
        sprite_destroy.setFillColor(Color::Yellow);
    }

private:
    virtual void draw(RenderTarget& target, RenderStates states) const
    {
        states.transform *= getTransform();
        target.draw(m_vertices, states);
    }

    VertexArray m_vertices;

};


// --------------------------------------------------------------------


// POCISKO-RAKIETO-PRZECIWNICY ----------------------------------------


class Enemy : public Drawable, public Transformable
{
public:
    CircleShape sprite_enemy;
    Vector2f GetPosition() { return sprite_enemy.getPosition(); };

    float velocity_x = 0.1;
    float velocity_y = 0.1;
    float acceleration = 0;
    float acceleration_x = 0;
    float acceleration_y = 0;
    float a = 0;
    float b = 0;

    Enemy()
    {
        sprite_enemy.setRadius(20);
        sprite_enemy.setOrigin(20, 20);
        sprite_enemy.setFillColor(Color::Blue);
        sprite_enemy.setPosition(Vector2f(2000, 0));
    }

    float velX(float player_x, float player_y, float player_mass, float velocity_x)
    {
        float a = player_x + 10.f - sprite_enemy.getPosition().x;
        float b = player_y + 10.f - sprite_enemy.getPosition().y;
        acceleration = (player_mass * 100) / (a * a + b * b);
        acceleration_x = (acceleration * a) / (sqrt(a * a + b * b));
        acceleration_y = (acceleration * b) / (sqrt(a * a + b * b));
        velocity_x += acceleration_x;
        return velocity_x;
    }

    float velY(float player_x, float player_y, float player_mass, float velocity_y)
    {
        float a = player_x + 10.f - sprite_enemy.getPosition().x;
        float b = player_y + 10.f - sprite_enemy.getPosition().y;
        acceleration = (player_mass * 100) / (a * a + b * b);
        acceleration_x = (acceleration * a) / (sqrt(a * a + b * b));
        acceleration_y = (acceleration * b) / (sqrt(a * a + b * b));
        velocity_y += acceleration_y;
        return velocity_y;
    }

    void reset_data()
    {
        velocity_x = 0.1;
        velocity_y = 0.1;
        acceleration = 0;
        acceleration_x = 0;
        acceleration_y = 0;
    }

private:
    virtual void draw(RenderTarget& target, RenderStates states) const
    {
        states.transform *= getTransform();
        target.draw(m_vertices, states);
    }

    VertexArray m_vertices;

};

// --------------------------------------------------------------------




int main()
{
    // Budzetowa wersja state machine
    bool menu = true;
    bool game = false;
    bool victory = false;

    // S T A N   M E N U
    while (menu)
    {
        RenderWindow window(VideoMode(800, 600), "Swift Blocks v0.6 MENU");
        Texture menu_win;
        if (!menu_win.loadFromFile("menu.png"))
        {
            cout << "texture not found!" << endl;
        }
        menu_win.loadFromFile("menu.png");
        Sprite menu_sprite(menu_win);

        while (window.isOpen())
        {
            Event event;

            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    window.close();
                }
                menu = false;
            }

            if (Keyboard::isKeyPressed(Keyboard::S))
            {
                game = true;
                menu = false;
                window.close();
            }
            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                menu = false;
                window.close();
            }
            window.draw(menu_sprite);
            window.display();
        }
    }


    // S T A N   G R Y
    while (game)
    {
        bool repeat = false;

        Player player;
        player.mass = 20;
        Clock delta;
        float deltaTime;
        bool whileJump = false;
        Data data;

        RenderWindow window(VideoMode(800, 600), "Swift Blocks v0.6 GAME");
        View view(Vector2f(player.sprite_player.getPosition().x, player.sprite_player.getPosition().y), Vector2f(800, 600));
        window.setFramerateLimit(60);
        window.setVerticalSyncEnabled(true);

        // Mapa - tablica (0 - pusta przestrzen, 1 - sciana bez mozliwosci zburzenia, 2 - sciana do zburzenia)
        // 1 element w tablicy = 125x50 pikseli
        int level[320] =
        {
            0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,2,0,1,1,2,2,0,0,0,0,1,1,1,1,0,0,2,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,2,0,0,0,2,2,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,2,0,0,0,2,2,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,2,1,0,1,1,0,0,0,0,0,0,0,2,0,0,0,0,0,
            0,0,0,0,0,0,1,1,0,0,1,0,0,1,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,
            0,0,1,0,1,0,1,1,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
        };

        // Przygotowanie tekstur
        Texture background;
        background.loadFromFile("background_1.png");

        Texture texture_platform;
        texture_platform.loadFromFile("platform.png");

        Sprite sprite_background(background);
        Sprite sprite;
        RenderTexture t;

        t.create(5000, 400);
        t.setSmooth(true);
        sprite.setTexture(t.getTexture());

        t.clear();
        t.draw(sprite_background);

        // Wektory przechowujace platformy
        vector<Sprite> all_platforms;
        vector<Sprite> destructable;
        vector<int> destructable_index;

        // Przeciwnik
        CircleShape test_enemy(20);
        test_enemy.setOrigin(20, 20);
        test_enemy.setFillColor(Color::Blue);
        test_enemy.setPosition(Vector2f(2000, 0));
        float velocity_x = 0.1;
        float velocity_y = 0.1;
        VertexArray torque;
        torque.setPrimitiveType(PrimitiveType::LinesStrip);

        Clock clock;

        while (window.isOpen())
        {
            // Jezeli gracz dotrze do lewego kranca mapy -> wygrana
            if (player.sprite_player.getPosition().x > 4900)
            {
                victory = true;
                game = false;
                window.close();
            }

            // Jezeli bool "repeat" zmieni sie na "true" -> zamkniecie okna + ponowne rozpoczecie gry
            if (repeat)
            {
                window.close();
            }

            float a = player.sprite_player.getPosition().x + 10.f - test_enemy.getPosition().x;
            float b = player.sprite_player.getPosition().y + 10.f - test_enemy.getPosition().y;
            float acceleration = (player.mass * 100) / (a * a + b * b);
            float acceleration_x = (acceleration * a) / (sqrt(a * a + b * b));
            float acceleration_y = (acceleration * b) / (sqrt(a * a + b * b));
            torque.append(Vertex(Vector2f(test_enemy.getPosition().x, test_enemy.getPosition().y)));
            velocity_x += acceleration_x;
            velocity_y += acceleration_y;

            if (fabs(a - b) > 600)
            {
                test_enemy.setPosition(Vector2f(2000, 0));
                velocity_x = 0.2;
                velocity_y = 0.2;
                acceleration = 0;
                acceleration_x = 0;
                acceleration_y = 0;
                torque.clear();
            }

            test_enemy.move(velocity_x, velocity_y);

            deltaTime = delta.restart().asSeconds();

            Event event;

            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    game = false;
                    window.close();
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                game = false;
                window.close();
            }

            if (Keyboard::isKeyPressed(Keyboard::Space) && whileJump == false)
            {
                player.velocity = data.force / player.mass;
                whileJump = true;
            }

            if (Keyboard::isKeyPressed(Keyboard::Num1)) {
                data.force = 125;
            }
            if (Keyboard::isKeyPressed(Keyboard::Num2)) {
                data.force = 175;
            }
            if (Keyboard::isKeyPressed(Keyboard::Num3)) {
                data.force = 225;
            }
            if (Keyboard::isKeyPressed(Keyboard::Q)) {
                player.mass = 20;
            }
            if (Keyboard::isKeyPressed(Keyboard::W)) {
                player.mass = 30;
            }
            if (Keyboard::isKeyPressed(Keyboard::E)) {
                player.mass = 40;
            }

            if (whileJump)
            {
                player.jump(deltaTime);
                if (player.sprite_player.getPosition().y > 380)
                {
                    player.sprite_player.setPosition(player.sprite_player.getPosition().x, 380);
                    whileJump = false;
                    player.acceleration = 0;
                }
                if (player.sprite_player.getPosition().y < -10)
                {
                    player.health -= 3;
                    player.sprite_player.setFillColor(Color::Red);
                    sleep(milliseconds(1000));
                    if (player.health == 0)
                    {
                        repeat = true;
                    }
                }
                for (int i = 0; i < all_platforms.size(); i++)
                {
                    if (player.sprite_player.getGlobalBounds().intersects(all_platforms[i].getGlobalBounds()))
                    {
                        player.health -= 1;
                        switch (player.health)
                        {
                        case (2):
                            player.sprite_player.setFillColor(Color::Yellow);
                            break;
                        case (1):
                            player.sprite_player.setFillColor(Color::Yellow);
                            break;
                        case (0):
                            player.sprite_player.setFillColor(Color::Red);
                            sleep(milliseconds(1000));
                            break;
                        }

                        if (player.health == 0)
                        {
                            repeat = true;
                        }
                    }
                }
            }

            // Aktualizacja pozycji gracza + pozycji kamery
            player.player_move(deltaTime);
            view.setCenter(player.GetPosition());

            // Rysowanie
            window.clear();
            window.setView(view);
            window.draw(sprite_background);

            // Clear wektorow przechowujacych platformy (co by nie rosly w nieskonczonosc)
            all_platforms.clear();
            destructable.clear();
            destructable_index.clear();

            // Rysowanie platform w zaleznosci od elementu w liscie
            for (int i = 0; i < 320; i++)
            {
                if (level[i] == 1)
                {
                    Sprite platform(texture_platform);
                    platform.setPosition((i % 40) * 125.f, (int(i / 40) * 50.f));
                    platform.setColor(Color::Red);
                    all_platforms.push_back(platform);
                    window.draw(platform);
                }
                else if (level[i] == 2)
                {
                    Sprite platform(texture_platform);
                    platform.setPosition((i % 40) * 125.f, (int(i / 40) * 50.f));
                    platform.setColor(Color::Yellow);
                    all_platforms.push_back(platform);
                    destructable.push_back(platform);
                    destructable_index.push_back(i);
                    window.draw(platform);
                }
            }

            // Burzenie platform oznaczonych jako "2" w liscie

            for (int i = 0; i < destructable.size(); i++)
            {
                if (test_enemy.getGlobalBounds().intersects(destructable[i].getGlobalBounds()))
                {
                    int iteration = destructable_index[i];
                    level[iteration] = 0;
                }
            }

            // Atak przeciwnika -> reset mapy -> ponowne uruchomienie mapy

            if (test_enemy.getGlobalBounds().intersects(player.sprite_player.getGlobalBounds()))
            {
                player.health -= 1;
                player.sprite_player.setFillColor(Color::Red);
                if (player.health == 0)
                {
                    sleep(milliseconds(1000));
                    repeat = true;
                }
            }

            window.draw(player.sprite_player);
            window.draw(torque);
            window.draw(test_enemy);

            window.display();
        }
    }

    // S T A N   W Y G R A N E J
    while (victory)
    {
        RenderWindow window(VideoMode(800, 600), "Swift Blocks v0.6 VICTORY");
        Texture victory_win;
        if (!victory_win.loadFromFile("victory.png"))
        {
            cout << "texture not found!" << endl;
        }
        victory_win.loadFromFile("victory.png");
        Sprite victory_sprite(victory_win);

        while (window.isOpen())
        {
            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                victory = false;
                window.close();
            }
            window.draw(victory_sprite);
            window.display();
        }
    }

    return 0;
}
