#include "ThemeElements.h"
#include "FileHandler.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
#include <windows.h>
using namespace std;


//Functions                                                         
void outputBoard(vector<string> square);                                                 //Displays Game Board
int  playerChoice(vector<int>& remaining);                                               //Grabs Player choice, Validates
int  computerChoice(vector<int>& remaining);		                                     //Grabs Computer Choice (srand & pass-by-ref)
void convertToXO(vector<string>& square, string letter, int value, FileHandler File);    //Converts square vector values to x or o's
bool winCheck(vector<string> square, string letter, string place, FileHandler File);     //Flag that checks for Wins
void secondPlace(vector<int>& remaining, vector<string>& square, FileHandler File);      //Computer can get second place when player wins
void tieEvent(FileHandler File);                                                         //In the event of a tie, this event triggers

int main() {
	//initialize classes
	Themes DefaultTheme;
	FileHandler File;

	//Print header
	DefaultTheme.header();
	//Change theme
	DefaultTheme.theme();

	//Variable to determine 2+ games
	string anyKey{ "" };

	//Running the game
	cout << "\n\n\tWELCOME TO TIC TAC TOE!\n";

	//game tracking
	int playerWins{ 0 }, computerWins{ 0 }, ties{ 0 }, gameCount{ 0 };

	do {
		//add to gameCount
		gameCount++;

		//write new game to gamesplays.txt
		if (gameCount > 1) {
			File.fileOutcome("\n\n\n\tPlayer chose to play a new game.");
			File.fileOutcome("--------------------------------------------");
		}

		//variables that will need resetting when 2+ games played
		vector<string> square{ "1", "2", "3", "4", "5", "6", "7", "8", "9" };			//Data that is displayed (supports X & O)															
		vector<int> remaining{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };                             //Data used for choice validation
		bool playerWin{ false }, compWin{ false };                                      //Flags that mark wins
		int turns = square.size();                                                      //Sets turns to number of elements

		for (int i = 0; i < turns; i++) {
			if (i % 2 == 0) {
				outputBoard(square);
				convertToXO(square, "X", playerChoice(remaining), File);
			}
			else {
				outputBoard(square);
				convertToXO(square, "O", computerChoice(remaining), File);
			}

			//check and change win bool flags
			playerWin = winCheck(square, "X", "first", File);
			compWin = winCheck(square, "O", "first", File);

			if (playerWin || compWin) {
				break;
			}
		}
		//recording wins and ties
		if (!playerWin && !compWin) {
			tieEvent(File);
			ties++;
		}
		else if (playerWin && !compWin) {
			playerWins++;
			secondPlace(remaining, square, File);
		}
		else {
			/*comp win*/
			computerWins++;
		}

		cout << "\n\n\tEnter any key to play again or CRTL + Z to quit: ";
	} while (cin >> anyKey);

	//Update Records
	File.setRecord("Games", gameCount);
	File.setRecord("Player", playerWins);
	File.setRecord("Computer", computerWins);
	File.setRecord("Ties", ties);

	//write records to file
	File.fileSummary();

	//Reads file to console
	File.getSummary();

	//Prints summary PART OF WINDOWS.H LIBRARY source - https://docs.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutea 
	ShellExecute(0, 0, L"TTTWins.txt", 0, 0, SW_SHOW);
	ShellExecute(0, 0, L"gamePlays.txt", 0, 0, SW_SHOW);

	return EXIT_SUCCESS;
}

void outputBoard(vector<string> square) {

	cout << "\n\t ----- ----- -----";
	cout << "\n\t|  " << square[0] << "  |  " << square[1] << "  |  " << square[2] << "  |  ";
	cout << "\n\t ----- ----- -----";
	cout << "\n\t|  " << square[3] << "  |  " << square[4] << "  |  " << square[5] << "  |  ";
	cout << "\n\t ----- ----- -----";
	cout << "\n\t|  " << square[6] << "  |  " << square[7] << "  |  " << square[8] << "  |  ";
	cout << "\n\t ----- ----- -----\n";
}

int playerChoice(vector<int>& remaining) {
	//variable to save choice and index
	int value{ }, index{ };
	//query choice 
	cout << "\n\n\tPlease select a square by entering its' number: ";
	cin >> value;

	//flag for do while
	bool valid{ false };
	do {
		//Validate choice (test type, range)
		while (cin.fail() || value < 0 || value > 9) {
			//if wrong type
			if (cin.fail()) {
				//clear error flag
				cin.clear();
				//reset input buffer
				cin.ignore(1000, '\n');
			}
			else {
				//re-ask for valid choice
				cout << "\n\tInvalid choice, please try again: ";
				cin >> value;
			}
		}
		//search remaining choices for chosen value
		vector<int>::iterator itr = find(begin(remaining), end(remaining), value);

		if (itr != end(remaining)) {
			index = itr - remaining.begin();
			valid = true;
		}
		else {
			cout << "\n\nPlease select a square by entering its' number: ";
			cin >> value;
		}


	} while (!valid);

	//erase index from remaining data
	remaining.erase(remaining.begin() + index);

	return value;
}

int computerChoice(vector<int>& remaining) {
	//create random number enviroment
	int vectorSize{ }, index{ }, value{ };
	vectorSize = remaining.size();

	//create engine
	srand(static_cast <unsigned int> (time(0)));

	//use engine to find random index
	index = rand() % vectorSize;
	value = remaining[index];
	//erase from future choices
	remaining.erase(remaining.begin() + index);

	cout << "\n\tComputer chose.... ";
	Sleep(1000);
	cout << value << "!\n";

	return value;
}

void convertToXO(vector<string>& square, string letter, int value, FileHandler File) {
	//convert to string
	string strValue = to_string(value);
	int squareIndex{};

	//find index of value in square
	vector<string>::iterator itr = find(begin(square), end(square), strValue);
	squareIndex = itr - square.begin();
	//replace with "o" or "x"
	square[squareIndex] = letter;

	//pass play into plays file writer
	File.fileMoves(letter, strValue, square);
}

bool winCheck(vector<string> square, string letter, string place, FileHandler File) {
	//Create var to store player
	string winOutput{ "\n\t" };
	//bool flag (to not repeat code per each win scenario)
	bool win{};
	//create outcome string for win
	if (letter == "X") {
		winOutput += "Player";
	}
	else {
		winOutput += "Computer";
	}
	//HORIZONTAL WINS
	if (square[0] == letter && square[1] == letter && square[2] == letter) {
		//add to winOutput
		winOutput += " won " + place + " place on the top row of squares!";
		win = true;
	}
	else if (square[3] == letter && square[4] == letter && square[5] == letter) {
		winOutput += " won " + place + " place on the middle row of squares!";
		win = true;
	}
	else if (square[6] == letter && square[7] == letter && square[8] == letter) {
		winOutput += " won " + place + " place on the last row of squares!";
		win = true;
	}
	//VERTICAL WINS
	else if (square[0] == letter && square[3] == letter && square[6] == letter) {
		winOutput += " won " + place + " place on the first column of squares!";
		win = true;
	}
	else if (square[1] == letter && square[4] == letter && square[7] == letter) {
		winOutput += " won " + place + " place on the second column of squares!";
		win = true;
	}
	else if (square[2] == letter && square[5] == letter && square[8] == letter) {
		winOutput += " won " + place + " place on the third column of squares!";
		win = true;
	}
	//DIAGONAL WINS 
	else if (square[0] == letter && square[4] == letter && square[8] == letter) {
		winOutput += " won " + place + " place on the decreasing diagonal!";
		win = true;
	}
	else if (square[2] == letter && square[4] == letter && square[6] == letter) {
		winOutput += " won " + place + " place on the increasing diagonal!";
		win = true;
	}
	else {
		win = false;
	}

	if (win) {
		outputBoard(square);
		cout << "\n\t" << winOutput << "\n";
		File.fileOutcome(winOutput);
		return true;
	}
	else {
		return false;
	}
}

void secondPlace(vector<int>& remaining, vector<string>& square, FileHandler File) {
	//vars
	bool win{ false };

	if (!remaining.empty()) {
		cout << "\n\tComputer trying for second place...\n";
		File.fileOutcome("\n\n\tComputer trying for second place...\n");
	}

	//checks if vector is empty source - https://en.cppreference.com/w/cpp/container/vector/empty
	while (!remaining.empty()) {

		//make choice
		convertToXO(square, "O", computerChoice(remaining), File);
		outputBoard(square);

		//check wins
		win = winCheck(square, "O", "second", File);

		if (win) {
			break;
		}
	}
	if (!win) {
		cout << "\n\tPlayer is the only winner!\n";
		File.fileOutcome("\n\tPlayer is the only winner!\n");
	}
}

void tieEvent(FileHandler File) {
	cout << "\n\n\tGame was a tie!\n\n";
	File.fileOutcome("\n\n\tGame was a tie!\n\n");
}
