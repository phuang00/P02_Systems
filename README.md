# Battleship
By Peihua Huang, Jackie Lin, and Yifan Wang (period 10)

# Proposal
## Description
A terminal re-creation of the famous Battleship game.
## User Interface
Two different terminal sessions will act as Player 1 and Player 2. Each player will have to first run a setup program that allows them to place their ships on a 10x10 board. They will each have five ships (of lengths 1-5) to place. After they finish setting up, each player can then run a play program that allows them to start playing. When both players are running the play program, the game begins. Their own board will be displayed on their screen, along with an empty board to keep track of the opponent's ships. Each player will input one coordinate per turn into the terminal to try to hit each other's ships. The results of these attacks will be displayed on the boards. The first player to sink all five of the opponent's ships wins.
## Technical Design
### Topics Used
* Shared memory
* Semaphores
* Memory allocation
* Piping
* Signals
### Task Delegation
* Setup
  * Initializing shared memory, semaphores (Jenny)
  * Displaying board/ships (Yifan)
  * Take in terminal input to place ships (Jackie)
* Gameplay
  * Setting up turn-based gameplay using semaphores (Jackie)
  * Displaying and updating both boards (Jenny)
  * Take in terminal input for coordinates (Yifan)
  * Signals to handle errors (all)
* Help Menu (all)
  * Everyone will update the help menu with instructions as project moves forward
### Data Structures
* Arrays (Board)
  * used to keep track of what coordinates have been inputted already on the board
    * labels such as "hit", "empty", "missed"...etc.
* Structs
  * Ship structs will be used to keep track of the coordinates of a ship
    * will be used to determine if ship has been sunk yet
  * Struct will also be used for piping coordinates back and forth
### Algorithms
* Check when a ship has been sunk (as opposed to just being hit)
* Check if all of a player's ships have been sunk (end of game)
* Check for faulty coordinates
  * not formatted correctly
  * out-of-bounds
  * duplicate
## Timeline
* Setup
  * Set up shared memory and semaphores
  * Design a struct for storing information about boards and ships
  * Write methods to display board in terminal
  * Allow for terminal input to place ships on board
  * Implement signaling to allow for faulty inputs
  * Update help menu to reflect instructions for setting up
* Gameplay
  * Set up turn-based playing
  * Allow for terminal input for coordinates
  * Set up piping between processes
  * Write algorithm for faulty coordinates
  * Write algorithm to check if ship has been hit
  * Write methods to update and display boards
  * Write algorithm to check if game is over
  * Update help menu to reflect instructions for playing
