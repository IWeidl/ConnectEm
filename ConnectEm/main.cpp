#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <array>
#define RAYGUI_IMPLEMENTATION
#include <extras/raygui.h>

// The state of each 'slot' can be either empty, Player1 or Player2
enum class SlotState {
	EMPTY,
	P1,
	P2
};

enum class Scene
{
	MENU,
	GAME
};

// Logic Functions
bool CheckBoard(std::array<std::array<SlotState, 6>, 7>& boardState, SlotState playerTurn, Vector2 lastMove);
Vector2 UpdateBoard(std::array<std::array<SlotState, 6>, 7>& boardState, SlotState playerTurn, Vector2 lastMove);

// Drawing Functions
void DrawBoard(std::array<std::array<SlotState, 6>, 7> boardState);
void DrawMouseSelection(Vector2 mousePos, SlotState playerTurn);
void DrawSlot(Vector2 position, int thickness, Color color);
void DrawSlotFilled(Vector2 position, Color color);

// Utility Functions
Vector2 ToVec2(int x, int y);
Vector2 CoordsToGrid(Vector2 mousePosCoords);
bool CompareTo3(SlotState leftSide, SlotState rightSide1, SlotState rightSide2, SlotState rightSide3);
bool Find4(std::vector<SlotState> line, SlotState playerTurn);

const int SCREENWIDTH = 600;
const int SCREENHEIGHT = 450;
const int TILESIZE = 64;
// const int OFFSET = (SCREENWIDTH - (TILESIZE * 7)) / 2;

int main()
{
	InitWindow(SCREENWIDTH, SCREENHEIGHT, "Connect4");
	SetTargetFPS(60);

	Scene currentScene = Scene::MENU;

	Vector2 mousePos;
	Vector2 mousePosCoord;
	Vector2 lastMove = {};
	std::array<std::array<SlotState, 6>, 7> boardState = { {} };
	SlotState playerTurn = SlotState::P1;
	std::string labelMouseCoords;
	bool validClick;
	std::string menuText = "Connect Em!";

	while (!WindowShouldClose())
	{
		mousePos = GetMousePosition();

		if (currentScene == Scene::MENU)
		{
			// UPDATE


			// DRAW
			BeginDrawing();
			ClearBackground(BLACK);
			GuiSetStyle(DEFAULT, TEXT_SIZE, 50);
			GuiDrawText(menuText.c_str(), Rectangle{ (SCREENWIDTH / 2), (SCREENHEIGHT / 2) - 200 }, GUI_TEXT_ALIGN_CENTER, BLUE);
			if (GuiButton(Rectangle{ (SCREENWIDTH / 2) - 100, (SCREENHEIGHT / 2) - 50, 200, 100 }, "PLAY"))
				currentScene = Scene::GAME;

			EndDrawing();

		}
		else if (currentScene == Scene::GAME)
		{
			// UPDATE		

			// We need to convert the 'real' coordinates to our 'tile' coordinates. 
			mousePosCoord = Vector2Divide(mousePos, Vector2{ TILESIZE, TILESIZE });
			mousePosCoord = Vector2{ floor(mousePosCoord.x), floor(mousePosCoord.y) };
			labelMouseCoords = std::to_string(mousePosCoord.x) + " " + std::to_string(mousePosCoord.y);

			validClick = CheckCollisionPointRec(mousePosCoord, Rectangle{ 0, 0, 6, 0 });

			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && validClick)
			{

				lastMove = UpdateBoard(boardState, playerTurn, mousePosCoord);


				if (CheckBoard(boardState, playerTurn, lastMove))
				{
					boardState = { {} };
					menuText = "Player " + std::to_string((int)playerTurn) + " Wins!!!";
					currentScene = Scene::MENU;
				}
				playerTurn = playerTurn == SlotState::P1 ? SlotState::P2 : SlotState::P1;
			}

			// DRAW
			BeginDrawing();
			ClearBackground(BLACK);

			DrawBoard(boardState);

			if (validClick)
				DrawMouseSelection(mousePosCoord, playerTurn);


			DrawText(labelMouseCoords.c_str(), 10, GetScreenHeight() - 30, 20, BLACK);

			EndDrawing();
		}
	}
}

bool CheckBoard(std::array<std::array<SlotState, 6>, 7>& boardState, SlotState playerTurn, Vector2 LastMove)
{
	int lmX = (int)(LastMove.x);
	int lmY = (int)(LastMove.y);

	// Vertical Check
	// We don't need to do too much checking here, as we know that pieces stack on top of each other, so can only be added to the top
	if (lmY < 3 && CompareTo3(boardState[lmX][lmY], boardState[lmX][lmY + 1], boardState[lmX][lmY + 2], boardState[lmX][lmY + 3]))
		return true;

	// Horizontal Check
	std::vector<SlotState> rowCheck;
	for (int i = 0; i < 7; i++)
		rowCheck.push_back(boardState[i][lmY]);
	if (Find4(rowCheck, playerTurn))
		return true;

	// Diagonal Check
	// Right (TopLeft to BottomRight) Diagonal
	std::vector<SlotState> rightDiag;
	SlotState rightDiagElement;
	for (int i = -3; i <= 6; i++)
	{
		try {
			rightDiagElement = boardState.at(lmX + i).at(lmY + i);
		}
		catch (std::exception e) {
			// if index out of bounds, we just set element to EMPTY
			rightDiagElement = SlotState::EMPTY;
		}
		rightDiag.push_back(rightDiagElement);
	}
	if (Find4(rightDiag, playerTurn))
		return true;


	// Left (TopRight to BottomLeft) Diagonal
	std::vector<SlotState> leftDiag;
	SlotState leftDiagElement;
	for (int i = -3; i <= 6; i++)
	{
		try {
			leftDiagElement = boardState.at(lmX + i).at(lmY - i);
		}
		catch (std::exception e) {
			// if index out of bounds, we just set element to EMPTY
			leftDiagElement = SlotState::EMPTY;
		}
		leftDiag.push_back(leftDiagElement);
	}
	if (Find4(leftDiag, playerTurn))
		return true;

	return false;
}

Vector2 UpdateBoard(std::array<std::array<SlotState, 6>, 7>& boardState, SlotState playerTurn, Vector2 lastMove)
{
	std::array<SlotState, 6>& column = boardState[lastMove.x];
	for (int i = column.size() - 1; i >= 0; i--)
	{
		if (column[i] == SlotState::EMPTY)
		{
			column[i] = playerTurn;
			return Vector2{ lastMove.x, (float)i };
		}

	}
}

bool CompareTo3(SlotState leftSide, SlotState rightSide1, SlotState rightSide2, SlotState rightSide3)
{
	return (leftSide == rightSide1 && leftSide == rightSide2 && leftSide == rightSide3);
}

bool Find4(std::vector<SlotState> line, SlotState playerTurn)
{
	int adjCount = 0;
	for (SlotState slot : line)
	{
		if (slot == playerTurn)
		{
			adjCount++;
			if (adjCount == 4)
				break;
		}
		else
			adjCount = 0;
	}
	if (adjCount >= 4)
		return true;
	return false;
}

void DrawBoard(std::array<std::array<SlotState, 6>, 7> boardState)
{
	DrawRectangle(0, 0, TILESIZE * 7, TILESIZE * 6, BLUE);
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			if (boardState[i][j] == SlotState::P1)
				DrawSlotFilled(ToVec2(i, j), RED);
			else if (boardState[i][j] == SlotState::P2)
				DrawSlotFilled(ToVec2(i, j), YELLOW);
			else
				DrawSlotFilled(ToVec2(i, j), WHITE);
		}
	}
}

void DrawMouseSelection(Vector2 mousePosCoords, SlotState playerTurn)
{
	Vector2 mousePosGrid = CoordsToGrid(mousePosCoords);
	Color playerColor = playerTurn == SlotState::P1 ? Color{ 230, 40, 55, 150 } : Color{ 255, 255, 0, 150 };
	DrawSlotFilled(mousePosCoords, playerColor);
}

void DrawSlot(Vector2 gridPosition, int thickness, Color color)
{
	DrawRectangleLinesEx(Rectangle{ (float)(gridPosition.x * TILESIZE), (float)(gridPosition.y * TILESIZE), (float)(TILESIZE), (float)(TILESIZE) },
		thickness,
		color);
}

void DrawSlotFilled(Vector2 gridPosition, Color color)
{
	float radius = (float)(TILESIZE / 2);
	DrawCircle((float)(gridPosition.x * TILESIZE) + radius, (float)(gridPosition.y * TILESIZE) + radius, radius - 1, color);
}

Vector2 ToVec2(int x, int y)
{
	return Vector2{ (float)(x), (float)(y) };
}

Vector2 CoordsToGrid(Vector2 mousePosCoords)
{
	return Vector2Multiply(mousePosCoords, Vector2{ (float)TILESIZE, (float)TILESIZE });
}