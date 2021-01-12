/* Game15 (or puzzle 15) : Template code
 *
 * Class: Board
 *
 *
 * Name: Paul Ahokas
 * Student number: 283302
 * UserID: 1693
 * E-Mail: paul.ahokas@tuni.fi
 *
 * Notes:
 *
 * */

#include "board.hh"
#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <algorithm>

const int EMPTY = 16;
const unsigned int PRINT_WIDTH = 5;

Board::Board():grid_(SIZE,std::vector<unsigned int>(SIZE)), vec_(SIZE*SIZE)
// Constructor, fills_ grid and vec_ with numbers 1-16 in ascending order
{
    // "Count" keeps track of index of vector, "i" and "j" keeps track of indexes of the grid.
    int count = 1U;
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            grid_[i][j] = count;
            vec_[count-1] = count;
            count++;
        }
    }
}

bool Board::has_player_won(std::vector<unsigned int> sorted_num)
// Checks if player has won
{
    if (sorted_num == vec_){
        return true;
    }
    return false;
}

void Board::possible_move(std::string command, int number)
// Moves given tile if possible
{
    // Find position of tile and empty tile.
    int pos = std::distance(vec_.begin(), std::find(vec_.begin(), vec_.end(), number));
    int empty_pos = std::distance(vec_.begin(), std::find(vec_.begin(), vec_.end(), EMPTY));
    int x_pos = pos % SIZE;
    int y_pos = pos / SIZE;

    // Checks if move is possible for each direction command, almost copy paste.
    // Better way to do this?
    if(command == "s"){
        if(y_pos+1 > SIZE-1 or pos+SIZE != empty_pos){
            std::cout << "Impossible direction: "<< command <<std::endl;
        }
        else{
            vec_.at(pos) = EMPTY;
            vec_.at(empty_pos) = number;
            vector_to_grid(vec_);
        }
    }
    else if(command == "w"){
        if(y_pos-1 < 0 or pos-SIZE != empty_pos){
            std::cout << "Impossible direction: "<< command <<std::endl;
        }
        else{
            vec_.at(pos) = EMPTY;
            vec_.at(empty_pos) = number;
            vector_to_grid(vec_);
        }
    }
    else if(command == "d"){
        if(x_pos+1 > SIZE-1 or pos+1 != empty_pos){
            std::cout << "Impossible direction: "<< command <<std::endl;
        }
        else{
            vec_.at(pos) = EMPTY;
            vec_.at(empty_pos) = number;
            vector_to_grid(vec_);
        }
    }
    else if(command == "a"){
        if(x_pos-1 < 0 or pos-1 != empty_pos){
            std::cout << "Impossible direction: "<< command <<std::endl;
        }
        else{
            vec_.at(pos) = EMPTY;
            vec_.at(empty_pos) = number;
            vector_to_grid(vec_);
        }
    }
}

void Board::vector_to_grid(std::vector<unsigned int> vec)
{
    // "Count" keeps track of index of vector, "i" and "j" keeps track of indexes of the grid.
    int count = 0U;
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            grid_[i][j] = vec[count];
            count++;
        }
    }
}

std::vector<unsigned int> Board::grid_to_vector()
{
    // "Count" keeps track of index of vector, "i" and "j" keeps track of indexes of the grid.
    int count = 0U;
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            vec_[count] = grid_[i][j];
            count++;
         }
    }
    return vec_;
}


bool Board::isSolvable()
// Checks if game is solvable. Returns true of it is solvable.
{

    int empty_pos = std::distance(vec_.begin(), std::find(vec_.begin(), vec_.end(), EMPTY));
    int y_pos = empty_pos / SIZE;

    // Move empty tile to bottom
    int count = 0U;
    for(; y_pos < SIZE-1; y_pos++){
        vec_.at(empty_pos) = vec_.at(empty_pos + SIZE);
        vec_.at(empty_pos + SIZE) = EMPTY;

        vector_to_grid(vec_);
        count++;
        empty_pos = empty_pos + SIZE;
    }
    // Calculate inversions from 1-15, skips value 16
    int inversions = 0;
    for(int i: vec_){
        if(i==16){
            ;
        }
        else{

            for(int j: vec_){
                if(j==16){
                    ;
                }
                else if(i < j){
                    inversions++;
                }
                else if(i == j){
                    break;
                }
            }
        }
    }
    if (inversions % 2 == 0){
        // Moves empty tile back
        for(; count > 0; count--){
            vec_.at(empty_pos) = vec_.at(empty_pos - SIZE);
            vec_.at(empty_pos - SIZE) = EMPTY;

            vector_to_grid(vec_);
            empty_pos = empty_pos - SIZE;
        }
        return true;
    }
    else{
        return false;
    }

}

void Board::print()
// Prints the board
{
    for(unsigned int x = 0; x < SIZE; ++x)
    {
        std::cout << std::string(PRINT_WIDTH * SIZE + 1, '-') << std::endl;
        for(unsigned int y = 0; y < SIZE; ++y)
        {
            std::cout << "|" << std::setw(PRINT_WIDTH - 1);
            if(grid_.at(x).at(y) != EMPTY)
            {
                std::cout << grid_.at(x).at(y);
            }
            else
            {
                std::cout << ".";
            }
        }
        std::cout << "|" << std::endl;
    }
    std::cout << std::string(PRINT_WIDTH * SIZE + 1, '-') << std::endl;
}

void Board::my_shuffle(std::vector<unsigned int> &numbers, int seed)
// Randomizes numbers in vector
{
    std::default_random_engine randomEng(seed);
    std::uniform_int_distribution<int> distr(0, numbers.size() - 1);
    for(unsigned int i = 0; i < numbers.size(); ++i)
    {
        unsigned int random_index = distr(randomEng);
        unsigned int temp = numbers.at(i);
        numbers.at(i) = numbers.at(random_index);
        numbers.at(random_index) = temp;
    }
}

