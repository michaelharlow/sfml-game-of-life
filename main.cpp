#include "imgui.h"
#include "imgui-SFML.h"
#include "timer.h"

#include <SFML/Graphics.hpp>

void handle_mouse_click(sf::RenderWindow& window, int* cells, int CELL_SIZE, int GRID_SIZE);
bool contains_mouse(sf::RenderWindow& window);

int* update_cells(int* cells, int CELL_SIZE, int GRID_SIZE);

int main()
{
    const int WIDTH = 600;
    const int HEIGHT = 600;
    const int CELL_SIZE = 10;
    const int CELL_GRID = WIDTH / CELL_SIZE;

    int is_drawing = 1;
    int is_updating = 0;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!");
    ImGui::SFML::Init(window);
    ImGuiIO& io = ImGui::GetIO();

    // create a continuous integer array to store 1 or 0 , which corresponds to the state of the cell
    int *cells = new int[(WIDTH / CELL_SIZE) * (HEIGHT / CELL_SIZE)];
    std::vector<sf::Vertex> vertices;
    int cell_pos[2] = {0, 0};

    // initialize the array to all 0s
    for (int i = 0; i < CELL_GRID; i++)
    {
        for (int j = 0; j < CELL_GRID; j++)
        {
            cells[i * CELL_GRID + j] = 0;
        }
    }

    sf::VertexArray triangle(sf::Triangles, 3);

    // define the position of the triangle's points
    triangle[0].position = sf::Vector2f(10.f, 10.f);
    triangle[1].position = sf::Vector2f(100.f, 10.f);
    triangle[2].position = sf::Vector2f(100.f, 100.f);

    // define the color of the triangle's points
    triangle[0].color = sf::Color::Red;
    triangle[1].color = sf::Color::Blue;
    triangle[2].color = sf::Color::Green;

    sf::Clock deltaClock;
    sf::Clock deltaTime;
    float accumlator = 0.0f;
    float dt = 0.1f;
    timer timer;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(view));
            }
        }

        vertices.clear();
        for (int i = 0; i < CELL_GRID; i++)
        {
            for (int j = 0; j < CELL_GRID; j++)
            {
                if (cells[i * CELL_GRID + j] == 1)
                {
                    float x = i * CELL_SIZE;
                    float y = j * CELL_SIZE;
                    vertices.push_back(sf::Vertex(sf::Vector2f(x, y), sf::Color::White));
                    vertices.push_back(sf::Vertex(sf::Vector2f(x + CELL_SIZE, y), sf::Color::White));
                    vertices.push_back(sf::Vertex(sf::Vector2f(x + CELL_SIZE, y + CELL_SIZE), sf::Color::White));
                    vertices.push_back(sf::Vertex(sf::Vector2f(x, y), sf::Color::White));
                    vertices.push_back(sf::Vertex(sf::Vector2f(x + CELL_SIZE, y + CELL_SIZE), sf::Color::White));
                    vertices.push_back(sf::Vertex(sf::Vector2f(x, y + CELL_SIZE), sf::Color::White));
                }
            }
        }


        if (is_updating)
        {
            if (accumlator > dt) {
			    cells = update_cells(cells, CELL_SIZE, CELL_GRID);
                accumlator -= dt;
            }
            accumlator += deltaTime.getElapsedTime().asSeconds();
		}
        deltaTime.restart();

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Hello, world!");
        ImGui::Text(("FPS:" + std::to_string(static_cast<int>(timer.get_fps()))).c_str());
        ImGui::SameLine(100);
        ImGui::PlotLines("FPS", timer.get_frames(), 100);
        ImGui::InputInt2("Input", cell_pos, ImGuiInputTextFlags_CharsDecimal);
        ImGui::Text("Mouse Position: (%d, %d)", sf::Mouse::getPosition(window), sf::Mouse::getPosition(window));
        ImGui::RadioButton("Draw", &is_drawing, 1); ImGui::SameLine();
        ImGui::RadioButton("Erase", &is_drawing, 0);
        ImGui::SliderFloat("Delta Time", &dt, 0.0f, 1.0f, "%.2f");
        if (ImGui::Button("Run"))
        {
            is_updating = 1 - is_updating;
		}
		if (ImGui::Button("Clear"))
		{
            for (int i = 0; i < CELL_GRID; i++)
            {
                for (int j = 0; j < CELL_GRID; j++)
                {
					cells[i * CELL_GRID + j] = 0;
				}
			}
		}
		if (ImGui::Button("Step"))
		{
			cells = update_cells(cells, CELL_SIZE, CELL_GRID);
		}
        ImGui::End();

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && contains_mouse(window) && !io.WantCaptureMouse)
        {
            sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
            int x = mouse_pos.x / CELL_SIZE;
            int y = mouse_pos.y / CELL_SIZE;
            cells[x * CELL_GRID + y] = is_drawing;
		}

        window.clear();

        window.draw(triangle);

        if (vertices.size() > 0)
         window.draw(&vertices[0], vertices.size(), sf::Triangles);

        ImGui::SFML::Render(window);

        window.display();

        timer.update();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

void handle_mouse_click(sf::RenderWindow& window, int* cells, int CELL_SIZE, int GRID_SIZE)
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
    int x = mouse_pos.x / CELL_SIZE;
    int y = mouse_pos.y / CELL_SIZE;
    cells[x * GRID_SIZE + y] = 1;
}

bool contains_mouse(sf::RenderWindow& window)
{
	sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
	sf::Vector2u window_size = window.getSize();
	return mouse_pos.x >= 0 && mouse_pos.x <= window_size.x && mouse_pos.y >= 0 && mouse_pos.y <= window_size.y;
}

int* update_cells(int* cells, int CELL_SIZE, int GRID_SIZE)
{
	int* new_cells = new int[GRID_SIZE * GRID_SIZE];
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
			int num_neighbors = 0;
            for (int k = -1; k < 2; k++)
            {
                for (int l = -1; l < 2; l++)
                {
                    if (k == 0 && l == 0)
                    {
						continue;
					}
                    int x = (i + k + GRID_SIZE) % GRID_SIZE;
                    int y = (j + l + GRID_SIZE) % GRID_SIZE;
     //               if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE)
     //               {
					//	continue;
					//}
					num_neighbors += cells[x * GRID_SIZE + y];
				}
			}
            if (cells[i * GRID_SIZE + j] == 1)
            {
                if (num_neighbors < 2 || num_neighbors > 3)
                {
					new_cells[i * GRID_SIZE + j] = 0;
				}
                else
                {
					new_cells[i * GRID_SIZE + j] = 1;
				}
			}
            else
            {
                if (num_neighbors == 3)
                {
					new_cells[i * GRID_SIZE + j] = 1;
				}
                else
                {
					new_cells[i * GRID_SIZE + j] = 0;
				}
			}
		}
	}
	return new_cells;
}   