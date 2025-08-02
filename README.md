# Graphical Calculator in C

This project is an **interactive graphical calculator developed in C language**, which allows you to write, solve, and graph equations with up to **two variables (x, y)**. The program offers a simple menu-driven interface where users can manage their equations, solve them using value tables, and generate graphs in **BMP format**.  
This was achieved through the use of **Expression Trees, Dynamic Memory, Abstract Data Types (ADT), and Good Programming Practices**.

## Enter Equations
Allows writing equations with up to two variables (x, y), supporting basic operations such as addition, subtraction, multiplication, division, as well as exponents and roots. The equations are stored in an array of up to 10 elements. If the limit is reached, the program will prompt the user to select which equation to replace.

Additionally, once an equation is entered, it is displayed on the screen and the user is given the option to continue editing it before saving, allowing real-time corrections.

## View Current Session Equations
Displays the equations currently loaded in the working session, making it easier to review them before solving or graphing.

## Save Equations and Reset Session
Allows saving the current session's equations to text files with names chosen by the user for future reference. After saving, the session is reset by clearing the loaded equations.

## Load Saved Equations
Displays the list of saved equation files (up to a maximum of 10) and allows the user to load one of them into the current session, replacing any previously loaded equations.

## Delete Saved Equations
Deletes all previously stored equation files, freeing up the workspace.

## Solve Equations
Provides two methods for solving equations:
- Manually entering a table of values.
- Entering a single value and automatically generating a table with a range of values (+5 and -5 around the entered value).

Additionally, it includes an algorithm that allows you to find the roots of the equations.

## Graph Equations
Allows selecting an equation from the current session and generating a BMP format graph representing its behavior.

In addition to generating the BMP file, the program can display the graph directly on the screen.

## Help
Includes a user guide explaining how to use each feature of the program, including how to correctly write exponents and roots.

## Exit
Closes the program.

## Requirements:
- C Compiler (GCC recommended)
- make (optional, if you want to automate the compilation)

## How to run the program:

### 1. Clone the repository:
bash
git clone https://github.com/Facundo-byte/Graphing-Calculator.git
cd Graphing-Calculator

### 2. Compile the program:
gcc -o calculator main.c -lm

  ### Project made in group with Bonfigli Leonardo, Deiriz Matias Ezequiel and Falce Ezequiel Pablo for the assignment Topicos de Programacion of UNLAM


