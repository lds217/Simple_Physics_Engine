#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <random> 
#include <chrono>
#include <iomanip>
#include <sstream>
#include<windows.h>
#define             ll        long long
#define FOR(i, a, b) for(long long i=a, _b=b; i<=_b; i++)
const int w = 800;
const int h = 800;
const float FPS = 60;
using namespace std;

mt19937_64 rd(chrono::steady_clock::now().time_since_epoch().count());

sf::Color Colors[] = { sf::Color::Green, sf::Color::Red, sf::Color::Blue, sf::Color::Yellow, sf::Color::Magenta };
sf::RenderWindow window(sf::VideoMode(800, 800), "Gravity Circle");
ll Rand(ll l, ll r) {
    return l + rd() % (r - l + 1);
}

//simple Point
class sP { 
    public:
        float x, y, R;
        sP(float x1, float y1, float R1)
        {
            x = x1;
            y = y1;
            R = R1;
        }
        sP() = default;
        void show() {
            cout << x << ' ' << y  << endl;
        }
        void draww(sf::Color h)
        {
            sf::CircleShape circle(R);
            circle.setFillColor(h);
            sf::Vector2f draw_pos = { (x - R) ,(y - R) };
            circle.setPosition(draw_pos);
            window.draw(circle);
            sf::CircleShape circle1(1);
            circle1.setFillColor(sf::Color::White);


            //circle1.setPosition( - sf::Vector2f{ 1, 1 });
            //window.draw(circle1);
        }
};

//Rectangle
class Rec {
    public:
        float x, y, w, h;
        Rec(float a, float b, float c, float d)
        {
            x = a;
            y = b;
            w = c;
            h = d;
        }
        Rec() = default;
        void show() {
            cout << x << ' ' << y << ' ' << w << ' ' <<  h << endl;
        }

        bool contains(sP point) {
            return (point.x >= this->x - this->w &&
                point.x < this->x + this->w &&
                point.y >= this->y - this->h &&
                point.y < this->y + this->h);
        }

        bool intersects(Rec range) {
            return !(range.x - range.w > this->x + this->w ||
                range.x + range.w < this->x - this->w ||
                range.y - range.h > this->y + this->h ||
                range.y + range.h < this->y - this->h);
        }
};


class Quadtree {
    public:
        Rec boundary;
        int capacity;
        vector <sP> points;
        bool divided = false;
        Quadtree* northeast, * northwest, * southeast, * southwest;

        Quadtree(Rec a,int cap){
            this->boundary = a;
            this->capacity = cap;
            this->divided = false;
        }

        bool insert(sP x) {
            if (!this->boundary.contains(x)) return false;
            if (this->points.size() < this->capacity) {
                this->points.push_back(x);
                return true;
            }
            else
                if (!this->divided)
                    this->subdivide();
                else {
                    if (this->northeast->insert(x))
                        return true;
                    if (this->northwest->insert(x))
                        return true;
                    if (this->southeast->insert(x))
                        return true;
                    if (this->southwest->insert(x))
                        return true;
                }  
            return false;
        }

        void subdivide() {
            float x = this->boundary.x;
            float y = this->boundary.y;
            float w = this->boundary.w;
            float h = this->boundary.h;
            Rec ne(x + w / 2, y - h / 2, w / 2, h / 2);
            this->northeast = new Quadtree(ne, this->capacity);
            Rec nw(x - w / 2, y - h / 2, w / 2, h / 2);
            this->northwest = new Quadtree(nw, this->capacity);
            Rec se(x + w / 2, y + h / 2, w / 2, h / 2);
            this->southeast = new Quadtree(se, this->capacity);
            Rec sw(x - w / 2, y + h / 2, w / 2, h / 2);
            this->southwest = new Quadtree(sw, this->capacity);
            this->divided = true;
        }

        void query(Rec range, vector <sP> &found) {
            if (found.size()==0) {
                found.clear();
            }
            if (!this->boundary.intersects(range)) {
                return;
            }
            else {
                for (sP p : this->points) {
                    if (range.contains(p)) {
                        found.push_back(p);
                    }
                }
                if (this->divided) {
                    this->northwest->query(range, found);
                    this->northeast->query(range, found);
                    this->southwest->query(range, found);
                    this->southeast->query(range, found);
                }
            }
            return;
        }

        void show() {
            sf::RectangleShape rectangle;
            rectangle.setSize(sf::Vector2f(this->boundary.w * 2, this->boundary.h * 2));
            rectangle.setFillColor(sf::Color::Transparent);
            rectangle.setOutlineColor(sf::Color::White);
            rectangle.setOutlineThickness(1);
            rectangle.setPosition(this->boundary.x- this->boundary.w, this->boundary.y - this->boundary.h);
            window.draw(rectangle);
            if (this->divided)
            {
                this->northeast->show();
                this->northwest->show();
                this->southeast->show();
                this->southwest->show();
            }
        }
};

void draw_rec(Rec a)
{
    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(a.w * 2, a.h * 2));
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineColor(sf::Color::Green);
    rectangle.setOutlineThickness(2);
    rectangle.setPosition(a.x - a.w, a.y - a.h);
    window.draw(rectangle);
}

void draw_circle(sP a)
{
    sf::CircleShape circle(1);
    circle.setFillColor(sf::Color::Green);
    circle.setPosition({ a.x, a.y });

    window.draw(circle);
}

//Rec a(100, 100, 100, 100);
//Quadtree(a)

struct Point {
    string id;
    sf::Vector2f position, velocity;
    float R, mass;
    sf::Color color;
    //sf::Vector2f prev_position;
    Point(int x,int y,int z)
    {
        id = to_string(z);
        R = Rand(3,10);
        mass = R*1.f / 100;
        color = Colors[0];
        position = { x *1.f  , y * 1.f };
        velocity = { ((Rand(1,243)%2)?-1:1) *Rand(30,50) * 1.f , ((Rand(1,243) % 2) ? -1 : 1) * Rand(30,50) * 1.f };
        //velocity = { 0.f,0.f };
    }
    void draww(sf::Color x)
    {
        sf::CircleShape circle(R);
        circle.setFillColor(x);
        sf::Vector2f draw_pos = { (position.x - R) * 1.f ,(position.y - R) * 1.f };
        circle.setPosition(draw_pos);
        window.draw(circle);
        sf::CircleShape circle1(1);
        circle1.setFillColor(sf::Color::White);
        circle1.setPosition(position - sf::Vector2f{1, 1});
        window.draw(circle1);
    }
};

float distance(Point a, Point b)
{
    return sqrt((b.position.x - a.position.x) * (b.position.x - a.position.x) + (b.position.y - a.position.y) * (b.position.y - a.position.y));
}


void update(Point &a) {
    sf::Vector2f& velocity = a.velocity;
    sf::Vector2f& position = a.position;
    
    //velocity.x = velocity.x * (1.0f / 10);
    //velocity.y = velocity.y * (1.0f / 10);

    position.x += velocity.x * (1.0f / FPS);
    position.y += velocity.y * (1.0f / FPS);

    if (position.x > w - a.R)
        velocity.x *= -1.f, position.x = w - a.R;
    if(position.x < a.R)
        velocity.x *= -1.f, position.x = a.R;
    if (position.y > h - a.R )
        velocity.y *= -1.f, position.y = h - a.R;
    if ( position.y < a.R)
        velocity.y *= -1.f, position.y =  a.R;
}

void conllision(Point& a, Point& b) /* Read the equations here: https://www.vobarian.com/collisions/2dcollisions2.pdf */
{
    float m1 = a.mass;
    float m2 = b.mass;
    
    sf::Vector2f v1 = a.velocity;
    sf::Vector2f v2 = b.velocity;
    // Interpolate
    sf::Vector2f n = { b.position.x - a.position.x, b.position.y - a.position.y };
    float dis = distance(a, b);
    float overlap = (a.R + b.R - dis )/ -2.f;
    a.position += (overlap * (n/ dis));
    b.position -= (overlap * (n/ dis));
    
    //Elastic Collision
    sf::Vector2f un = { n.x / sqrt(n.x * n.x + n.y * n.y), n.y / sqrt(n.x * n.x + n.y * n.y) };
    sf::Vector2f ut = { -un.y, un.x };
    float v1n = un * v1;
    float v1t = ut * v1;
    float v2n = un * v2;
    float v2t = ut * v2;
    sf::Vector2f v1nnv = (v1n * (m1 - m2) + 2 * m2 * v2n) / (m1 + m2) * un;
    sf::Vector2f v2nnv = (v2n * (m2 - m1) + 2 * m1 * v1n) / (m1 + m2) * un;
    sf::Vector2f v1ttv = v1t * ut;
    sf::Vector2f v2ttv = v2t * ut;
    a.velocity = v1nnv + v1ttv;
    b.velocity = v2nnv + v2ttv;
    return;
}


int main()
{
    Rec a(400 * 1.f, 400 * 1.f, 400 * 1.f, 400 * 1.f);
    
    int n;
    cin >> n;
    Rec q(Rand(150,600) * 1.f, Rand(150, 600) * 1.f, 150.f, 150.f);
    vector <Point> v;
    FOR(i, 1, n)
    {
        v.push_back(Point(Rand(1+200, w-200), Rand(1+200, h-200),i));
    }

    window.setFramerateLimit(FPS);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        cout << "cannot load font";
        return 0;
    }
    
    //cout << v[0].velocity.x;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        
        FOR(i, 0, v.size() - 1)
            FOR(j,i+1,v.size()-1)
                if(distance(v[i], v[j]) <= v[i].R + v[j].R)
                    conllision(v[j], v[i]);
        window.clear(sf::Color::Black);
       
        
        for (Point& i : v) {  
            update(i);
            i.draww(sf::Color::Green);
        }
        Quadtree qt(a, 4);
        for (Point i : v)
            qt.insert({ i.position.x,i.position.y, i.R });
        vector <sP> res;
        qt.query(q, res);
        draw_rec(q);
        for (sP i : res)
            i.draww(sf::Color::Red);
        qt.show();
        window.display();;
        /*sf::Text text;
        text.setFont(font);
        text.setString(to_string(fps);*/
    }

    return 0;
}