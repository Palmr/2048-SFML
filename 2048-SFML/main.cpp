#include <SFML/Graphics.hpp>
#include <map>
#include <utility>
#include <sstream>
#include <string>
#include <stdlib.h>

using namespace std;

unsigned short score = 0;
unsigned short highScore = 0;

// TODO - NP - Try animating? Have another grid of vectors for tile speed
//             The tiles moves that far every frame unless there's a tile in its
//             way, in which case it either combines and hides or stops moving.
//
//             Also need to add game-over testing and message

unsigned short grid [4][4];
struct cellCoord {
	unsigned char x;
	unsigned char y;
};

sf::Font font;
sf::Color windowBGColour(250, 248, 239, 255);
sf::Color gridBGColour(187, 173, 160, 255);
sf::Color textColour(67, 53, 40, 255);
map<int, sf::Color> numberColours;

float padding = 10.f;
float gridSize = (400.f - (padding * 2.f));
float tileSize = (gridSize - (5 * padding)) / 4;

sf::RenderWindow window(sf::VideoMode(400, 500), "2048 - Clonetastic", sf::Style::Close|sf::Style::Titlebar);
sf::RectangleShape gridBackgroundRect(sf::Vector2f(gridSize, gridSize));
sf::RectangleShape gridTile(sf::Vector2f(tileSize, tileSize));
sf::Text titleText("2048!", font, 48);
sf::Text scoreText(to_string(score), font, 48);
sf::Text scoreDecoratorText("", font, 15);
sf::Text tileText;

void renderScreen() {
	window.clear(windowBGColour);

	window.draw(gridBackgroundRect);

	// render the grid
	for (int ix = 0; ix < 4; ix++) {
		for (int iy = 0; iy < 4; iy++) {
			gridTile.setPosition(sf::Vector2f(20 + (ix * (tileSize + padding)), 120 + (iy * (tileSize + padding))));
			gridTile.setFillColor(numberColours.find(grid[ix][iy])->second);
			window.draw(gridTile);

			if (grid[ix][iy] > 0) {
				tileText.setString(to_string(grid[ix][iy]));
				sf::Vector2f textPosition = (gridTile.getPosition() + (gridTile.getSize() / 2.f));
				textPosition.x -= tileText.getGlobalBounds().width / 2.f;
				textPosition.y -= (tileText.getGlobalBounds().height + padding) / 2.f;
				tileText.setPosition(textPosition);
				window.draw(tileText);
			}
		} 
	}

	// render the score info
	scoreText.setString(to_string(score));
	scoreText.setPosition(sf::Vector2f(380 - scoreText.getGlobalBounds().width, 25));
	window.draw(scoreText);
	scoreDecoratorText.setString("score:");
	scoreDecoratorText.setPosition(sf::Vector2f(scoreText.getPosition().x - scoreDecoratorText.getGlobalBounds().width - 10, 57));
	window.draw(scoreDecoratorText);
	scoreDecoratorText.setString("highscore: " + to_string(highScore));
	scoreDecoratorText.setPosition(sf::Vector2f(scoreDecoratorText.getPosition().x, 72));
	window.draw(scoreDecoratorText);

	window.draw(titleText);
	window.display();
}

// Return a list of empty cell coordinates
vector<sf::Vector2i> getEmptyCells() {
	vector<sf::Vector2i> empties;
	for (unsigned char ix = 0; ix < 4; ix++) {
		for (unsigned char iy = 0; iy < 4; iy++) {
			if (grid[ix][iy] == 0) {
				empties.push_back(sf::Vector2i(ix, iy));
			}
		}
	}
	return empties;
}

// Add a random tile in an empty space
void addNumber() {
	int newCellOptions[2] = {2, 4};
	vector<sf::Vector2i> empties = getEmptyCells();
	if (empties.size() == 0) {
		printf("No empties\r\n");
		return; // No empties
	}
	else {
		int randEmpty = rand() % empties.size();
		grid[empties.at(randEmpty).x][empties.at(randEmpty).y] = newCellOptions[rand() % 2];
	}
}

// Reset the game
void reset() {
	// Reset score and log if it was a new highscore
	if (score > highScore) {
		highScore = score;
	}
	score = 0;

	// Reset grid
	for (int ix = 0; ix < 4; ix++) {
		for (int iy = 0; iy < 4; iy++) {
			grid[ix][iy] = 0;
		}
	}
	
	// Add two starting numbers
	addNumber();
	addNumber();
}


// TODO - NP - Change vector<int> to a deque<int> so I can push/pop from the front

// Take in a set of cells for a row/column and collapse them down
void combine(vector<int>& cellList) {
	if (cellList.size() <= 1) {
		return;
	}
	unsigned char index = 0;
	while (index < cellList.size() - 1) {
		int cell1 = cellList.at(index);
		int cell2 = cellList.at(index+1);

		if (cell1 == cell2) {
			cellList.at(index) = cell1 * 2;
			score += cell1;
			cellList.erase(cellList.begin() + index + 1);
		}
		index++;
	}
}

// Move tiles down and return how many tiles actually moved
unsigned char moveDown() {
	unsigned char tilesMoved = 0;

	for (int ix = 0; ix < 4; ix++) {
		vector<int> cellList;
		for (int iy = 3; iy >= 0; iy--) {
			if (grid[ix][iy] != 0) {
				cellList.push_back(grid[ix][iy]);
			}
		}
		combine(cellList);

		unsigned short tileBefore;
		for (int iy = 3; iy >= 0; iy--) {
			tileBefore = grid[ix][iy];

			if (cellList.size() > 0) {
				grid[ix][iy] = cellList.at(0);
				cellList.erase(cellList.begin());
			}
			else {
				grid[ix][iy] = 0;
			}

			if (grid[ix][iy] != tileBefore) {
				tilesMoved++;
			}
		}
	}
	return tilesMoved;
}

unsigned char moveUp() {
	unsigned char tilesMoved = 0;

	for (int ix = 0; ix < 4; ix++) {
		vector<int> cellList;
		for (int iy = 0; iy < 4; iy++) {
			if (grid[ix][iy] != 0) {
				cellList.push_back(grid[ix][iy]);
			}
		}
		combine(cellList);
		
		unsigned short tileBefore;
		for (int iy = 0; iy < 4; iy++) {
			tileBefore = grid[ix][iy];

			if (cellList.size() > 0) {
				grid[ix][iy] = cellList.at(0);
				cellList.erase(cellList.begin());
			}
			else {
				grid[ix][iy] = 0;
			}

			if (grid[ix][iy] != tileBefore) {
				tilesMoved++;
			}
		}
	}
	return tilesMoved;
}


unsigned char moveLeft() {
	unsigned char tilesMoved = 0;

	for (int iy = 0; iy < 4; iy++) {
		vector<int> cellList;
		for (int ix = 0; ix < 4; ix++) {
			if (grid[ix][iy] != 0) {
				cellList.push_back(grid[ix][iy]);
			}
		}
		combine(cellList);
		
		unsigned short tileBefore;
		for (int ix = 0; ix < 4; ix++) {
			tileBefore = grid[ix][iy];

			if (cellList.size() > 0) {
				grid[ix][iy] = cellList.at(0);
				cellList.erase(cellList.begin());
			}
			else {
				grid[ix][iy] = 0;
			}

			if (grid[ix][iy] != tileBefore) {
				tilesMoved++;
			}
		}
	}
	return tilesMoved;
}

unsigned char moveRight() {
	unsigned char tilesMoved = 0;

	for (int iy = 0; iy < 4; iy++) {
		vector<int> cellList;
		for (int ix = 3; ix >= 0; ix--) {
			if (grid[ix][iy] != 0) {
				cellList.push_back(grid[ix][iy]);
			}
		}
		combine(cellList);
		
		unsigned short tileBefore;
		for (int ix = 3; ix >= 0; ix--) {
			tileBefore = grid[ix][iy];

			if (cellList.size() > 0) {
				grid[ix][iy] = cellList.at(0);
				cellList.erase(cellList.begin());
			}
			else {
				grid[ix][iy] = 0;
			}

			if (grid[ix][iy] != tileBefore) {
				tilesMoved++;
			}
		}
	}
	return tilesMoved;
}

int main() {
	// Seed the RNG
	srand(time(NULL));

	// Try and load the font
	if (!font.loadFromFile("ClearSans-Bold.ttf")) {
		printf("Font fail\r\n");
		return 1;
	}

	// Define colours for the cells based on their value
	numberColours[0] = sf::Color(204, 192, 179, 255);
	numberColours[2] = sf::Color(238, 228, 218, 255);
	numberColours[4] = sf::Color(237, 224, 200, 255);
	numberColours[8] = sf::Color(242, 177, 121, 255);
	numberColours[16] = sf::Color(245, 149, 99, 255);
	numberColours[32] = sf::Color(246, 124, 95, 255);
	numberColours[64] = sf::Color(246, 94, 59, 255);
	numberColours[128] = sf::Color(237, 207, 114, 255);
	numberColours[256] = sf::Color(237, 204, 97, 255);
	numberColours[512] = sf::Color(237, 200, 80, 255);
	numberColours[1024] = sf::Color(237, 197, 63, 255);
	numberColours[2048] = sf::Color(237, 194, 46, 255);

	// Set up the grid background rect
	gridBackgroundRect.setFillColor(gridBGColour);
	gridBackgroundRect.setPosition(sf::Vector2f(10, 110));

	// Set up the title text
	titleText.setStyle(sf::Text::Bold);
	titleText.setColor(textColour);
	titleText.setPosition(sf::Vector2f(10, 25));

	// Set up the score text objects
	scoreText.setStyle(sf::Text::Bold);
	scoreText.setColor(textColour);
	scoreDecoratorText.setColor(textColour);

	// Set up the tile text
	tileText.setFont(font);
	tileText.setCharacterSize(32);
	tileText.setStyle(sf::Text::Bold);
	tileText.setColor(textColour);

	// Reset to start fresh
	reset();

	// Some stuff for making the random adds to hapen after a slight delay
	bool moveDone = false;
	bool scheduledNumberAdd = false;
	sf::Clock clock;
	clock.restart();
	sf::Time t2 = sf::milliseconds(100);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
					window.close();
			}
			if (event.type == sf::Event::KeyPressed && !moveDone && !scheduledNumberAdd) {
				unsigned char tilesMoved = 0;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
					tilesMoved = moveUp();
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
					tilesMoved = moveRight();
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
					tilesMoved = moveDown();
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
					tilesMoved = moveLeft();
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
					reset();
					continue;
				}
				moveDone = true;
        if (tilesMoved > 0) {
					scheduledNumberAdd = true;
					clock.restart();
				}
			}
			if (event.type == sf::Event::KeyReleased && moveDone) {
				moveDone = false;
			}
		}
		
		if (scheduledNumberAdd && clock.getElapsedTime() > t2) {
			addNumber();
			scheduledNumberAdd = false;
		}

		renderScreen();
	}

	return 0;
}