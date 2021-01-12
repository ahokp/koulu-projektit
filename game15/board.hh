/* Game15 (or puzzle 15) : Template code
 *
 * Class: Board
 *  Describes a two-dimensional grid for puzzle 15 with necessary functions.
 *
 * Name: Paul Ahokas
 * Student number: 283302
 * UserID: 1693
 * E-Mail: paul.ahokas@tuni.fi
 *
 * Notes:
 *
 * */

#ifndef BOARD_HH
#define BOARD_HH
#include <vector>
#include <string>
const int SIZE = 4;

class Board
{
public:
    // Constructor, fills grid and vector with nubmer 1-16 in ascending order
    Board();

    // Prints the game grid
    void print();

    // Shuffles the numbers vector by using seed as a seed value
    void my_shuffle(std::vector<unsigned int>& numbers, int seed);

    // Transforms vector in to 2d vector(grid), used for printing out the grid.
    // parameter is the vector to be transfered.
    void  vector_to_grid(std::vector<unsigned int> vec);

    // Transforms 2d vector(grid) into a vector, used so 2d vector (grid) can
    // be easily modified. Returns transfered grid in vector form.
    std::vector<unsigned int> grid_to_vector();

    // Checks if inputted move is possible. Takes direction command and tile number
    // as parameters.
    void possible_move(std::string command, int number);

    // Checks if player has won. Takes sorted/completed vector as parameter so
    // it can compare it to the game vector. Returns true if player has won.
    bool has_player_won(std::vector<unsigned int> sorted_num);

    // Checks if game is solvable. Returns true of it is solvable.
    bool isSolvable();

private:


    // Game grid for the 15 puzzle
    std::vector<std::vector<unsigned int>> grid_;
    // Grid in vector form
    std::vector<unsigned int> vec_;

};

#endif // BOARD_HH
