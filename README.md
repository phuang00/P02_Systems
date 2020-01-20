# Battleship
By Peihua Huang, Jackie Lin, and Yifan Wang (period 10)

## Description
A terminal re-creation of the famous Battleship game.

## Required Libraries
*None*

## User Instructions 
0. After downloading the project, find a friend to play Battleship with you!
1. In the terminal, type make. 
2. Type ./setup 1 to begin setting up your own board. Tell your friend to run ./setup 2!
3. You should see a 10x10 board on the terminal. You will now begin placing your five ships (lengths 1-5) down on the board. To place a ship, you must provide **3** values in the following order: column (A-J), row (0-9), and orientation (l - left, r - right, u - up, d - down). 
    - keep in mind that the first three characters of your input will be interpreted as the column, row, and orientation
    - you are allowed to separate your input by spaces
    - **order matters!** 
    - you may **not** place boats on top of each other!
    - if you exit the process before you finish placing your boats, **your progress will be erased!**
    - each boat will be represented by its number on the board (e.g. boat two will render as '2' on the board.)
    - '-' indicates that a boat has not been placed at that coordinate yet. 
4. After you finish placing down your boats, run ./play 1 to enter the game! When your friend is done, tell them to run ./play 2. Once both of you have entered, the game will begin!
5. Once the game has started, both you and your friend should see your own board and an empty version of each other's board to keep track of which coordinates you have hit already. Now, both of you will take turns inputting coordinates into the terminal to fire at. To input a coordinate, you must provide **2** values in the following order: column (A-J) and row (0-9).
    - keep in mind that the first two characters of your input will be interpreted as the column and row
    - you are allowed to separate your input by spaces
    - **order matters!**
    - you may **not** fire at a coordinate you have already fired at
    - if you exit the process during your turn, **your turn will be given to your opponent!**
    - 'O' indicates that the coordinate has been fired at already, but there was no ship present.
    - 'X' indicates that the coordinate has been fired at already, and there was a ship present.
    - '-' indicates that the coordinate has not been fired at yet. 
6. Once one of you has sunk all five of the other's ships, the game will come to an end. If you would like to play again, reset the game by running ./setup -r. When the terminal prompts you with the question, "Are you sure you want to reset the game? (y/n): ", type in y for yes, n for no. 
7. Have fun!

*(If you need to view these instructions again, run cat helpMenu in the terminal!)*
