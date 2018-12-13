/**
 *  @file Board.cpp
 *  @brief contains implementation for Board classes
 */

#include "Board.h"
#include "Game.h"
#include "Globals.h"
#include <iostream>

using namespace std;

/**
 *  BoardImpl: class for board implementation
 */
class BoardImpl{
    public:
        BoardImpl(const Game& g);
        void clear();
        void block();
        void unblock();
        void display(bool showShots) const;
        bool placeShip(Point tl, int shipID, Direction d);
        bool removeShip(Point tl, int shipID, Direction d);
        bool attack(Point p, bool& hit, bool& destroyed, int& shipID);
        bool allDestroyed() const;
    private:
        char mArr[MAXROWS][MAXCOLS];    ///< Member array for rows and columns
        const Game& mGame;  ///< Member object referencing to game object
};

/**
 * BoardImpl constructor
 * @param g reference to game object
 * Loops through rows and columns and fills them with '.'
 */
BoardImpl::BoardImpl(const Game& g) : mGame(g){
    for(int i = 0; i < mGame.rows(); i++){
        for(int j = 0; j < mGame.cols(); j++){
            mArr[i][j] = '.';
        }
    }
}

/**
 *  Method for clearing the game screen
 *  Loops through rows and columns and fills them with '.'
 */
void BoardImpl::clear(){
    for(int i = 0; i < mGame.rows(); i++){
        for(int j = 0; j < mGame.cols(); j++){
            mArr[i][j] = '.';
        }
    }
}

/**
 * Method for blocking cells
 * loops through game's rows and columns
 * and blocks them with '#' symbol with 50% chance
 */
void BoardImpl::block(){
    for(int i = 0; i < mGame.rows(); i++){
        for(int j = 0; j < mGame.cols(); j++){
            if(randomInt(2) == 0){
                mArr[i][j] = '#';
            }
        }
    }
}

/**
 * Method for unblocking cells
 * loops through game's rows and columns and replaces '#' with '.'
 */
void BoardImpl::unblock(){
    for(int i = 0; i < mGame.rows(); i++){
        for(int j = 0; j < mGame.cols(); j++){
            if(mArr[i][j] == '#'){
                mArr[i][j] = '.';
            }
        }
    }
}

/**
 * Display method for showing game screen
 * @param showShots boolean to check if shots are visible or not
 */
void BoardImpl::display(bool showShots) const{
    cout << "  ";   ///< Empty spaces to line header numbers with cells
    ///Loops through columns to create line with header (numbers)
    for(int i = 0; i < mGame.cols(); i++){
        cout << i;
    }
    cout << endl;

    ///Loops through rows
    for(int i = 0; i < mGame.rows(); i++){
        cout << i;  ///< Creates header numbers for columns
        cout << " ";
        for(int j = 0; j < mGame.cols(); j++){
            ///Checks if shots are visible
            if(showShots){
                ///Checks if cells don't contain 'X' and 'o' and if cells contain alphabetic character
                if(isalpha(mArr[i][j]) && mArr[i][j] != 'X' && mArr[i][j] != 'o'){
                    cout << '.';    ///< If cell is alphabetic, turns it into '.'
                } else{ ///< If cell is not alphabetic or contains 'X' or 'o'
                    cout << mArr[i][j]; ///Prints the cell
                }
            } else {    ///< If showShots is false it prints the cell
                cout << mArr[i][j];
            }
        }
        cout << endl;
    }
}

/**
 * Method for placing a ship on cell
 * @param tl Point for top left cell
 * @param shipID integer used as ship's ID
 * @param d Direction where the ship is facing
 */
bool BoardImpl::placeShip(Point tl, int shipID, Direction d){
    bool answer = true; ///< Boolean for checking if ship placement is valid

    ///Checks if ID is invalid
    if(mGame.shipLength(shipID) == 0){
        answer = false;
    }

    ///Checks if cell is outside of board
    if(tl.r < 0 || tl.c < 0 || tl.r >= mGame.rows() || tl.c >= mGame.cols()){
        answer = false;
    }

    ///Checks if ship extends outside of board
    if(d == HORIZONTAL){
        if(tl.c + mGame.shipLength(shipID) - 1 >= mGame.cols()){
            answer = false;
        }
    } else {
        if(tl.r + mGame.shipLength(shipID) - 1 >= mGame.rows()){
            answer = false;
        }
    }
    ///Checks if ship has been placed before
    for(int i = 0; i < mGame.rows(); i++){
        for(int j = 0; j < mGame.cols(); j++){
            if(mArr[i][j] == mGame.shipSymbol(shipID)){
                answer = false;
            }
            break;
        }
    }

    ///Checks if ship overlaps another one
    if(d == HORIZONTAL){
        for(int i = 0; i < mGame.shipLength(shipID); i++){
            if(mArr[tl.r][tl.c + i] != '.'){
                answer = false;
                break;
            }
        }
    } else {
        for(int i = 0; i < mGame.shipLength(shipID); i++){
            if(mArr[tl.r + i][tl.c] != '.'){
                answer = false;
                break;
            }
        }
    }

    ///Places ship on game board only if ship placement is valid
    if(answer){
        if(d == HORIZONTAL){
            for(int i = 0; i < mGame.shipLength(shipID); i++){
                mArr[tl.r][tl.c + i] = mGame.shipSymbol(shipID);
            }
        } else {
            for(int i = 0; i < mGame.shipLength(shipID); i++){
                mArr[tl.r + i][tl.c] = mGame.shipSymbol(shipID);
            }
        }
    }
    return answer;
}

/**
 * Method for taking a ship away from grid
 * @param tl Point for top left cell
 * @param shipID integer used as ship's ID
 * @param d Direction where the ship is facing
 */
bool BoardImpl::removeShip(Point tl, int shipID, Direction d){
    bool answer = true;
    ///Checks if ID is invalid
    if(mGame.shipLength(shipID) == 0){
        answer = false;
    }

    ///Checks if ship is there
    if(d == HORIZONTAL){
        for(int i = 0; i < mGame.shipLength(shipID); i++){
            if(mArr[tl.r][tl.c + i] != mGame.shipSymbol(shipID)){
                answer = false;
                break;
            }
        }
    } else {
        for(int i = 0; i < mGame.shipLength(shipID); i++){
            if(mArr[tl.r + i][tl.c] != mGame.shipSymbol(shipID)){
                answer = false;
                break;
            }
        }
    }

    ///Clears the cell if ship is there
    if(answer){
        for(int i = 0; i < mGame.rows(); i++){
            for(int j = 0; j < mGame.cols(); j++){
                if(mArr[i][j] == mGame.shipSymbol(shipID)){
                    mArr[i][j] = '.';
                }
            }
        }
    }
    return answer;
}

/**
 * Method for shooting on a cell
 * @param p Point that has the cell
 * @param hit reference to boolean which indicates if ship was hit
 * @param destroyed reference to boolean which indicates if ship was destroyed
 * @param shipID reference to integer which returns ID of ship that was hit
 */
bool BoardImpl::attack(Point p, bool& hit, bool& destroyed, int& shipID){
    bool answer = true;

    ///Returns false if target is out of bounds
    if(p.r < 0 || p.c < 0 || p.r >= mGame.rows() || p.c >= mGame.cols()){
        answer = false;
    }

    ///Returns false if target has already been shot
    if(mArr[p.r][p.c] == 'X' || mArr[p.r][p.c] == 'o'){
        answer = false;
    }

    ///Checks if it's possible to attack target cell
    if(answer){
        ///Checks if target doesn't have a dot on it
        if(mArr[p.r][p.c] != '.'){
            hit = true;
            char shipSymbol = mArr[p.r][p.c];
            ///Updates array with hit mark
            mArr[p.r][p.c] = 'X';
            bool exists = false;

            for(int i = 0; i < mGame.rows(); i++){
                for(int j = 0; j < mGame.cols(); j++){
                    ///Checks if ship exists
                    if(mArr[i][j] == shipSymbol){
                        exists = true;
                    }
                }
            }
            ///Checks if ship doesn't exist, it has been sunk
            if(!exists){
                destroyed = true;
                ///Loop that finds the right ship
                for(int i = 0; i < mGame.nShips(); i++){
                    if(shipSymbol == mGame.shipSymbol(i)){
                        shipID = i;
                        break;
                    }
                }
            } else {
                destroyed = false;
            }
        } else {    ///< If hit was a miss
            hit = false;
            mArr[p.r][p.c] = 'o';
        }
    }
    return answer;
}

/**
 * Method for checking if all ships have been destroyed
 */
bool BoardImpl::allDestroyed() const{
    bool answer = true;

    ///Loops through rows and columns
    for(int i = 0; i < mGame.rows(); i++){
        for(int j = 0; j < mGame.cols(); j++){
            /**Checks if board contains other symbols than 'X', '.' or 'o'
             *If yes, then there are still ships left
             */
            if(mArr[i][j] != 'X' && mArr[i][j] != '.' && mArr[i][j] != 'o'){
                answer = false;
                break;
            }
        }
    }
    return answer;
}

/**
 * Delegates to board implementation
 * @param g reference to game object
 */
Board::Board(const Game& g){
    mImpl = new BoardImpl(g);
}

/**
 * Deletes board implementation
 */
Board::~Board(){
    delete mImpl;
}

/**
 * Delegates to board implementation's clear method
 */
void Board::clear(){
    mImpl->clear();
}

/**
 * Delegates to board implementation's block method
 */
void Board::block(){
    return mImpl->block();
}

/**
 * Delegates to board implementation's unblock method
 */
void Board::unblock(){
    return mImpl->unblock();
}

/**
 * Delegates to board implementation's display method
 * @param showShots boolean for showing shots
 */
void Board::display(bool showShots) const{
	mImpl->display(showShots);
}

/**
 * Delegates to board implementation's placeShip method
 * @param tl Point containing top left cell
 * @param shipID integer for ship id
 * @param d Direction the ship is facing
 */
bool Board::placeShip(Point tl, int shipID, Direction d){
	return mImpl->placeShip(tl, shipID, d);
}

/**
 * Delegates to board implementation's removeShip method
 * @param tl Point containing top left cell
 * @param shipID integer for ship id
 * @param d Direction the ship is facing
 */
bool Board::removeShip(Point tl, int shipID, Direction d){
	return mImpl->removeShip(tl, shipID, d);
}

/**
 * Delegates to board implementation's attack method
 * @param p Point that has the targeted cell
 * @param hit reference to boolean which indicates if ship was hit
 * @param destroyed reference to boolean which indicates if ship was destroyed
 * @param shipID reference to integer which returns ID of ship that was hit
 */
bool Board::attack(Point p, bool& hit, bool& destroyed, int& shipID){
	return mImpl->attack(p, hit, destroyed, shipID);
}

/**
 * Delegates to board implementation's allDestroyed method
 */
bool Board::allDestroyed() const{
	return mImpl->allDestroyed();
}