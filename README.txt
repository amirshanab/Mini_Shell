
Shell simulator
authored by Amir Abu Shanab.
208586214
==Description==
ex3.c:-
this is a program that takes in the user's input and runs the commands that can be by ending them to the execvp function.
it does all that using the help of 14 cultural functions that communicate with each other to get the job done.
==Program Database==
1) The file that was created by the program to save the inputs of the user, and it can be accessed by giving the command "history".
2)the file that was created by the program to save the output of the program when using the command "nohup".
== Functions==
14 functions :-
1) StringFun – this function takes the user's input (string) and two int variables (word count and letter count) and runs it through the string,
 counting the number of words and letters.
2) countLine: This function retrieves the name of the file (data file) and counts the number of lines in it.
3) writeToFile – this function accepts user input and the file name and writes the string entered by the user to the file.
4) printHistory – this is a function that takes the file name and prints out the context of the file as well as the line numbers.
5) remove_spaces - this is a function that takes in a string input and removes the spaces at the end of the sentence.
6) checkForPipes - this is a function that takes in a string input from the program and counts the amount of pipes in it "|".
7) arrFun - this is a function that creates a 2D array out of the string that it takes in.
8) changString - this is a function that takes in a string that has the char "!" in it and takes the number that's after that char
  and changes the input to the right input from the history file.
9) onePipeEx - this function takes in the user's input that has only one pipe in it and splits that string into two, send each to the arrFun to create
   array out of them, then send each array to the execvp function to run the commands.
10) twoPipeEx - this function is exactly like the function before it only this one takes in the string with two pipes in it and splits them into
   3 arrays than are sent to the execvp function to run.
11) explanation -  this function takes in the user's input and counts the amount of "!" in it.
12) execute - this is the main function in this program, and its in charge of running the commands with no pipes in them.
13) clear_history -  this function was added by me and it was not a part of the  homework, all it does is delete the history list
    after running the command "history -c".
14) handler - this function takes care of killing the child process that was running in the background after is done.
==Program Files==
ex3.c.
==How to compile? ==
gcc ex3.c -o run
run: ./run
== Input: ==
A string by the user (up to 3 commands)
==Output: ==
The output depends on the user input and the amount of commands that can be executed by the execvp function, and some other functions depending on the input.