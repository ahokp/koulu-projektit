/* Game15 (or puzzle 15) : Template code
 *
 * Desc:
 *  This program generates a 15 puzzle. The board is SIZE x SIZE,
 * (original was 4x4) and every round the player chooses a direction and
 * a number tile, making the given tile to be moved to the given direction.
 *  Game will end when the numbers 1-15 (1-16) are in the ascending order,
 * starting from top left and proceeding row by row.
 * Empty space is represented by the number 16 in the code (constant EMPTY)
 * and by dot when printing the game board.
 *
 * Name: Paul Ahokas
 * Student number: 283302
 * UserID: 1693
 * E-Mail: paul.ahokas@tuni.fi
 *
 * Notes about the program and it's implementation:
 * This game has been implemented on a base that was given.
 * */

#include "iostream"
#include "board.hh"
#include <cstdlib>
#include <string>
#include <random>
#include <algorithm>

using namespace std;
// Bad global vector variable with all values. What's a better way to do this?
vector<unsigned int> numbers = {1, 2, 3, 4, 5, 6, 7, 8
                                , 9, 10, 11, 12, 13, 14
                                , 15, 16};
// Global variable for checking correct direction commands
string const MOVES = "wasd";

// More functions

bool make_vector(string submitted_nums){
    // Transforms given numbers into a vector. Return true if missing number(s).


    vector<unsigned int> submitted_vector;
    auto start = 0U;
    auto end = submitted_nums.find(" ");
    while(end != string::npos){
        //submitted_vector.push_back(submitted_nums.substr(start, end-start));

        if(submitted_nums.substr(start, end-start).size()!=0){
            submitted_vector.push_back(stoi(submitted_nums.substr(start, end-start)));
        }
        start = end + 1;
        end = submitted_nums.find(" ", start);
    }
    //gets last int
    submitted_vector.push_back(stoi(submitted_nums.substr(start, end-start)));

    // check for missing numbers
    for(int i : numbers){
        if (find(submitted_vector.begin(), submitted_vector.end(), i) == submitted_vector.end()){
            cout << "Number " << i << " is missing"<< endl;
            return true;
        }
    }

    numbers = submitted_vector;
    return false;
}

int initGame(Board& board, string rndm_board){
    //Initializes the game depending on users choice. return 1 for breaking loop,
    //2 for EXIT_FAILURE, 3 for unknown choice.

    //gives tiles random numbersector <
        if(rndm_board=="y"){
            cout << "Give a seed value or an empty line: ";
            string seed_value = "";
            getline(cin, seed_value);

            default_random_engine randomEng;
            int seed;
            if(seed_value == ""){
                // If the user did not give a seed value, use computer time as the seed value.
                seed = time(NULL);
            }
            else {
                // If the user gave a seed value, use it.
                seed = stoi(seed_value);
            }
                board.my_shuffle(numbers, seed);
                board.vector_to_grid(numbers);
                board.grid_to_vector();
                return 1;


        }
        // Takes user submitted numbers
        else if(rndm_board=="n"){

            cout << "Enter the numbers 1-16 in a desired order (16 means empty):" <<endl;
            string submitted_nums = "";
            getline(cin, submitted_nums);

            if(make_vector(submitted_nums)){
                return 2;
            }
            board.vector_to_grid(numbers);
            board.grid_to_vector();
            return 1;
        }
        else{
            cout << "Unknown choice: "<< rndm_board <<endl;
        }
        return 3;
}
bool has_player_won(Board &Board)
//checks if player has won. Returns true if player has won.
{
    vector <unsigned int> sorted_vec = numbers;
    sort(sorted_vec.begin(), sorted_vec.end());

    if (Board.has_player_won(sorted_vec)){
        cout << "You won!" <<endl;
        return true;
    }
    return false;
}

void movement(Board &Board){
// UI for giving direction command and tile number.

    while(true){
        cout << "Dir (command, number): ";
        string input;
        getline(cin, input);

        auto start = 0U;
        auto end = input.find(" ");
        string command = input.substr(start, end);

        if(command == "q"){
            break;
        }

        start = end + 1;
        end = input.find(" ", start);
        int number = stoi(input.substr(start, end-start));

        if(MOVES.find(command) == string::npos){
            cout << "Unknown command: "<< command <<endl;
        }
        else if(number > 15){
            cout << "Invalid number: "<< number <<endl;
        }
        else{
            Board.possible_move(command, number);
        }
        Board.print();

        if (has_player_won(Board)){
            break;
        }
    }
}


int main()
{

    Board board;

    string rndm_board = "";
    //player chooses how he wants to set the board, i==2 if a number is missing
    while(true){
        cout << "Random initialization (y/n): ";
        getline(cin, rndm_board);

        int i = initGame(board, rndm_board);
        if(i == 1){
            break;
        }
        else if (i == 2){
            return EXIT_FAILURE;
        }
        else{
            ;
        }
    }

    if (board.isSolvable()){
        cout << "Game is solvable: Go ahead!" << endl;

        board.print();
        movement(board);
    }
    else{
        cout << "Game is not solvable. What a pity." << endl;
    }


    return EXIT_SUCCESS;
}
