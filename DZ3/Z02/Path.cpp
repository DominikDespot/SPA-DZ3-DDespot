#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include "Path.h"
//A* algoritam jer je optimalan za ovu primjenu

//promjenjivo ali mora ostati 16:9
constexpr int CELL_SIZE = 30;
constexpr int GRID_WIDTH = 64;
constexpr int GRID_HEIGHT = 36;

constexpr int WINDOW_WIDTH = GRID_WIDTH * CELL_SIZE;
constexpr int WINDOW_HEIGHT = GRID_HEIGHT * CELL_SIZE;

int cf = 0;

enum class CellType { Empty, Wall, Start, End, Path };

struct Cell {
    CellType type = CellType::Empty;
    sf::RectangleShape shape;
    bool n_type = false;
    bool is_drawn = false;
};

struct Node {
    int x;
    int y;
    float g;
    float h;
    Node* parent;

    Node(int x, int y, float g, float h, Node* parent)
        : x(x), y(y), g(g), h(h), parent(parent) {}

    float getF() const { return g + h; }

    bool operator==(const Node& other) const {
        return x == other.x && y == other.y;
    }
};

struct CompareNodes {
    bool operator()(const Node* a, const Node* b) const {
        return a->getF() > b->getF();
    }
};

class main {
public:
    void run(sf::RenderWindow& window) {
        initialize_grid();

        sf::Clock clock;
        sf::Time elapsed;
        cf = 1;
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (event.type == sf::Event::MouseButtonPressed) {
                    m_event(event.mouseButton);
                }
                else if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Space) {
                        if (!drawPath) {
                            reset_path();
                            find_path();
                            drawPath = true;
                            pathIndex = path.size() - 1;
                        }
                    }
                }
            }

            elapsed = clock.restart();
            render(window, cf);
        }
    }

private:
    std::vector<Cell> grid;
    std::vector<Node*> path;
    int startX = -1;
    int startY = -1;
    int endX = -1;
    int endY = -1;
    bool start_set = false;
    bool end_set = false;
    bool drawPath = false;
    int pathIndex = 0;

    void initialize_grid() {
        grid.resize(GRID_WIDTH * GRID_HEIGHT);
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                Cell& cell = grid[y * GRID_WIDTH + x];
                cell.shape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.shape.setPosition(sf::Vector2f(x * CELL_SIZE, y * CELL_SIZE));
            }
        }
    }

    void m_event(const sf::Event::MouseButtonEvent& mouse) {
        int mouseX = mouse.x;
        int mouseY = mouse.y;
        int cellX = mouseX / CELL_SIZE;
        int cellY = mouseY / CELL_SIZE;

        if (!start_set) {
            set_start(cellX, cellY);
        }
        else if (!end_set) {
            set_end(cellX, cellY);
        }
        else {
            toggle_wall(cellX, cellY);
        }
    }

    void set_start(int x, int y) {
        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
            Cell& cell = grid[y * GRID_WIDTH + x];
            if (cell.type != CellType::End && cell.type != CellType::Wall) {
                if (startX != -1 && startY != -1) {
                    Cell& prevStartCell = grid[startY * GRID_WIDTH + startX];
                    prevStartCell.type = CellType::Empty;
                }
                cell.type = CellType::Start;
                cell.n_type = true;
                startX = x;
                startY = y;
                start_set = true;
            }
        }
    }

    void set_end(int x, int y) {
        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
            Cell& cell = grid[y * GRID_WIDTH + x];
            if (cell.type != CellType::Start && cell.type != CellType::Wall) {
                if (endX != -1 && endY != -1) {
                    Cell& prevEndCell = grid[endY * GRID_WIDTH + endX];
                    prevEndCell.type = CellType::Empty;
                }
                cell.type = CellType::End;
                endX = x;
                endY = y;
                end_set = true;
            }
        }
    }

    void toggle_wall(int x, int y) {
        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
            Cell& cell = grid[y * GRID_WIDTH + x];
            if (cell.type == CellType::Wall) {
                cell.type = CellType::Empty;
            }
            else if (cell.type == CellType::Empty) {
                cell.type = CellType::Wall;
            }
        }
    }

    std::vector<Node*> get_neighbours(const Node* node) {
        std::vector<Node*> neighbors;

        int x = node->x;
        int y = node->y;

        if (x > 0) {
            add_neigbour(x - 1, y, neighbors);
        }
        if (x < GRID_WIDTH - 1) {
            add_neigbour(x + 1, y, neighbors);
        }
        if (y > 0) {
            add_neigbour(x, y - 1, neighbors);
        }
        if (y < GRID_HEIGHT - 1) {
            add_neigbour(x, y + 1, neighbors);
        }

        return neighbors;
    }

    void add_neigbour(int x, int y, std::vector<Node*>& neighbors) {
        Cell& cell = grid[y * GRID_WIDTH + x];
        if (cell.type != CellType::Wall) {
            neighbors.push_back(new Node(x, y, std::numeric_limits<float>::max(), 0.0f, nullptr));
        }
    }

    float heuristic(int x, int y) {
        return std::sqrt(std::pow(endX - x, 2) + std::pow(endY - y, 2));
    }

    void find_path() {
        std::vector<std::vector<bool>> visited(GRID_HEIGHT, std::vector<bool>(GRID_WIDTH, false));
        std::priority_queue<Node*, std::vector<Node*>, CompareNodes> pq;

        Node* startNode = new Node(startX, startY, 0.0f, heuristic(startX, startY), nullptr);
        pq.push(startNode);

        while (!pq.empty()) {
            Node* current = pq.top();
            pq.pop();

            if (visited[current->y][current->x]) {
                continue;
            }
            visited[current->y][current->x] = true;

            if (current->x == endX && current->y == endY) {
                drawPath = false;
                new_path(current);
                break;
            }
            else {
                std::vector<Node*> neighbors = get_neighbours(current);
                for (Node* neighbor : neighbors) {
                    if (visited[neighbor->y][neighbor->x]) {
                        continue;
                    }
                    float tentativeG = current->g + 1.0f;
                    if (tentativeG < neighbor->g) {
                        neighbor->parent = current;
                        neighbor->g = tentativeG;
                        neighbor->h = heuristic(neighbor->x, neighbor->y);
                        pq.push(neighbor);
                    }
                }
            }
        }

        while (!pq.empty()) {
            delete pq.top();
            pq.pop();
        }
    }

    void new_path(const Node* node) {
        while (node) {
            int x = node->x;
            int y = node->y;
            Cell& cell = grid[y * GRID_WIDTH + x];
            if (cell.type != CellType::Start && cell.type != CellType::End) {
                cell.type = CellType::Path;
            }
            node = node->parent;
        }
    }

    void reset_path() {
        for (Cell& cell : grid) {
            if (cell.type == CellType::Path) {
                cell.type = CellType::Empty;
            }
        }
    }

    void render(sf::RenderWindow& window, int& cf) {
        sf::Texture steve_texture;
        try {
            if (!steve_texture.loadFromFile("Resources\\steve.jpg")) {
                throw std::runtime_error("Error loading steve texture! 404, not found!");
            }
        }
        catch (std::runtime_error err) { std::cout << err.what() << std::endl; }
        sf::Texture stone_texture;
        try {
            if (!stone_texture.loadFromFile("Resources\\stone.jpg")) {
                throw std::runtime_error("Error loading stone texture! 404, not found!");
            }
        }
        catch (std::runtime_error err) { std::cout << err.what() << std::endl; }

        sf::Texture bedrock_texture;
        try {
            if (!bedrock_texture.loadFromFile("Resources\\bedrock.jpg")) {
                throw std::runtime_error("Error loading diamond texture! 404, not found!");
            }
        }
        catch (std::runtime_error err) { std::cout << err.what() << std::endl; }

        sf::Texture diamond_texture;
        try {
            if (!diamond_texture.loadFromFile("Resources\\diamond.jpg")) {
                throw std::runtime_error("Error loading diamond texture! 404, not found!");
            }
        }
        catch (std::runtime_error err) { std::cout << err.what() << std::endl; }

        int c = 0;
        bool flag = false;
        window.clear();
        for (Cell& cell : grid) {
            if (cell.type == CellType::Path) {
                flag = true;
                if (c < cf && has_neigbour(cell) || cell.is_drawn) {
                    c++;
                    cell.n_type = true;
                    cell.shape.setTexture(&steve_texture);
                    window.draw(cell.shape);
                    cell.is_drawn = true;
                }
                else {
                    window.draw(cell.shape);
                }
            }
            if (cell.type == CellType::Empty) {
                cell.shape.setTexture(&stone_texture);
                window.draw(cell.shape);
            }
            if (cell.type == CellType::Start) {
                cell.shape.setTexture(&steve_texture);

                window.draw(cell.shape);
            }
            if (cell.type == CellType::End) {
                cell.shape.setTexture(&diamond_texture);
                window.draw(cell.shape);
            }
            if (cell.type == CellType::Wall) {
                cell.shape.setTexture(&bedrock_texture);
                window.draw(cell.shape);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        window.display();
        if (flag)
            cf++;
    }

    bool has_neigbour(const Cell& cell) {
        int x = cell.shape.getPosition().x / CELL_SIZE;
        int y = cell.shape.getPosition().y / CELL_SIZE;

        bool left = x > 0 && grid[y * GRID_WIDTH + (x - 1)].n_type;
        bool right = x < GRID_WIDTH - 1 && grid[y * GRID_WIDTH + (x + 1)].n_type;
        bool up = y > 0 && grid[(y - 1) * GRID_WIDTH + x].n_type;
        bool down = y < GRID_HEIGHT - 1 && grid[(y + 1) * GRID_WIDTH + x].n_type;

        return left || right || up || down;
    }
};

void Path::display(sf::RenderWindow& window)
{
    //klasa u klasi jer sam DZ3 nadogradio na DZ2
    sf::SoundBuffer buffer;
    sf::Sound click;
    try {
        if (!buffer.loadFromFile("Resources\\click.ogg")) {
            throw std::runtime_error("Error loading click sound! 404, not found!");
        }
    }
    catch (std::runtime_error err) { std::cout << err.what(); }
    click.setBuffer(buffer);
    click.play();
    main main;
    main.run(window);
}
