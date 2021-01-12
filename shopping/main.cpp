/* Shopping : Template code
 *
 * Desc:
 *  This program reads information about stores, their location
 * and product information from a text file and saves them into
 * a data structure. User can then access the stored information
 * using different commands.
 *
 * Name: Paul Ahokas
 * Student number: 283302
 * UserID: 1693
 * E-Mail: paul.ahokas@tuni.fi
 *
 * Notes about the program and it's implementation:
 * This game has been implemented on an empty base.
 * */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>

using namespace std;

// Data structure that will contain product information.
struct Product {
    string product_name;
    double price;
};

vector<string> split(const string& row, const char delimiter, bool ignore_empty = false){
    // Splits given string by given delimiter and adds words to a vector
    // Returns a vector containing words

    vector<string> result;
    string line = row;

    while(line.find(delimiter) != string::npos)
    {
        string new_part = line.substr(0, line.find(delimiter));
        line = line.substr(line.find(delimiter)+1, line.size());
        if(not (ignore_empty and new_part.empty()))
        {
            result.push_back(new_part);
        }
    }
    if(not (ignore_empty and line.empty()))
    {
        result.push_back(line);
    }
    return result;
}
bool compare_alphabet(const Product &a, const Product &b){
    // Compares products alphabetically, used for sorting a vector containing structs
    return a.product_name < b.product_name;
}

void data_struct(map <string, map <string, vector<Product>>>& stores,
                 vector <string>& products,vector<string> parts){
    // Saves line info from text file into the data structures.
    // Vector "parts" contains info that will be stored into
    // data structures "stores" and "product".

    string market_name = parts.at(0);
    string location = parts.at(1);
    string product_name = parts.at(2);
    double product_price;
    if (parts.at(3) == "out-of-stock"){
        product_price = 0;
    }
    else{
        product_price = stod(parts.at(3));
    }

    Product product;
        product.product_name = product_name;
        product.price = product_price;

    stores[market_name][location];
    bool isDuplicate = false;
    // Saves latest price, if same product given twice
    for(auto& i : stores[market_name][location]){
        if(i.product_name == product_name){
            i.price = product_price;
            isDuplicate = true;
        }
    }
    if (not isDuplicate){
        stores[market_name][location].push_back(product);

        // Sorts the vector containing structs
        sort(stores[market_name][location].begin(), stores[market_name][location].end(),
             compare_alphabet);
    }
        // Stores product name to vector "products" if its not in it already
    if (find(products.begin(), products.end(), product_name) == products.end()){
        products.push_back(product_name);
    }
}
void print_selection(map <string, map <string, vector<Product>>> stores, vector<string> command){
    // Prints out market's selection
    // Vector "command" contains given market name and location
    // "stores" is our data structure

    for(auto product : stores[command[1]][command[2]]){

        //Product is out of stock
        if(product.price == 0){
            cout << product.product_name << " out of stock" <<endl;
        }
        else{
            cout << fixed << setprecision(2);
            cout << product.product_name << " " << product.price <<endl;
        }
    }
}
void print_cheapest(map <string, map <string, vector<Product>>> stores, vector<string> command){
    // Prints out cheapest price of given product and its location(s) if it's part of the selection
    // Vector "command" contains product's name
    // "stores" is our data structure

    double cheapest_price = SIZE_MAX;
    vector <string> cheapest_location;
    bool is_product_in_selection = false;

    // Going through our data structure and storing cheapest price
    for(auto store_pair : stores){
        for(auto location_pair : store_pair.second){
            for(auto product : location_pair.second){

                if(product.product_name == command[1]){
                    string location = store_pair.first + " " + location_pair.first;

                    // If new cheapest price, clear vector and set new price
                    if(cheapest_price > product.price and product.price != 0){

                        cheapest_price = product.price;
                        cheapest_location.clear();
                        cheapest_location.push_back(location);
                    }
                    // Cheapest price matches with location's price
                    else if(cheapest_price == product.price){
                        cheapest_location.push_back(location);
                    }
                    // Checking if given product is in selection
                    if(command[1] == product.product_name){
                        is_product_in_selection = true;
                    }
                }
            }
        }
    }
    if(not is_product_in_selection){
        cout << "The product is not part of product selection" <<endl;
    }
    else if(cheapest_location.size() == 0){
        cout << "The product is temporarily out of stock everywhere" <<endl;
    }
    else{
        cout << fixed << setprecision(2);
        cout << cheapest_price << " euros" <<endl;
        for(string location : cheapest_location){
            // Printing out cheapest locations
            cout << location <<endl;
        }
    }
}
bool doesExist(map <string, map <string, vector<Product>>> stores, vector<string> command,
               bool checkLocation = false){
    // Checks if chain or store exists in data structure.
    // Vector "command" contains given market name and its location.
    // "stores" is our data structure

    if(stores.find(command[1]) == stores.end()){
        cout << "Error: unknown chain name" <<endl;
        return false;
    }
    if(checkLocation){
        if(stores[command[1]].find(command[2]) == stores[command[1]].end()){
            cout << "Error: unknown store" <<endl;
            return false;
        }
    }
    return true;
}

void user_interface(map <string, map <string, vector<Product>>> stores, vector <string> products){
    // User interface for entering commands and searching information
    // "stores" and "product" are our data structures

    vector <string> commands = {"chains", "stores", "selection", "cheapest", "products"};
    while(true){

        cout << "> ";
        string user_input;
        getline(cin, user_input);

        vector<string> command = split(user_input, ' ');

        if(command[0] == "chains" && command.size() == 1){
            for(auto chain_pair : stores){
                cout << chain_pair.first <<endl;
            }
        }
        else if(command[0] == "stores" && command.size() == 2 ){
            if(doesExist(stores, command)){
                for(auto location_pair : stores[command[1]]){
                   cout << location_pair.first <<endl;
                }
            }
        }
        else if(command[0] == "selection" && command.size() == 3){
             if(doesExist(stores, command, true)){
                 print_selection(stores, command);
            }
        }
        else if(command[0] == "cheapest" && command.size() == 2){
            print_cheapest(stores, command);
        }
        else if(command[0] == "products" && command.size() == 1){
             for(string product : products){
                cout << product <<endl;
           }
        }
        else if(command[0] == "quit"){
            break;
        }
        else if (find(commands.begin(), commands.end(), command[0]) == commands.end()){
            cout << "Error: unknown command: "<< command[0] <<endl;
        }
        else{
            cout << "Error: error in command "<< command[0] <<endl;
        }
    }
}

int main()
{
    // Used data structures
    map <string, map <string, vector<Product>>> stores;
    // Vector "products" is used for storing each product
    vector <string> products;

    cout << "Input file: ";
    string input = "";
    getline(cin, input);

    ifstream input_file(input);
    if (not input_file){
        cout << "Error: the input file cannot be opened"<<endl;
        return EXIT_FAILURE;
    }
    else{

        string row;
        // Reading text file and storing in a vector
        while (getline(input_file, row)){

            vector<string> parts = split(row, ';', true);
            if(parts.size() != 4){
                cout << "Error: the input file has an erroneous line" << endl;
                return EXIT_FAILURE;
            }
            data_struct(stores, products, parts);
        }
        input_file.close();
        sort(products.begin(), products.end());
        user_interface(stores, products);
    }
    return EXIT_SUCCESS;
}
