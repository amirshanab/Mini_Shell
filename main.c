#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <wait.h>
#include <stdlib.h>
#include <fcntl.h>

//amir
int countLines(char *fname);

void StringFun(const char *s, int *WC);

void writeToFile(char *userIn, char *path);

void remove_spaces(char *s);

void printHistory(char *path);

int checkForPipes(char *userIn);

void arrFun(char *userIn, char **command);

int changString(char *userIn);

void onePipeEx(char *userIn, int checkForExp, int no_hup);

void twoPipeEx(char *userIn, int checkForExp, int no_hup);

int explanation(char *userIn);

void execute();

void clear_history(char *path);

void handler();


static int numOfPid = 1;

void handler() {// a function that takes care of killing the process after it's done in the background.
    int st;
    waitpid(-1, &st, WNOHANG);
}

void clear_history(char *path) {// a function to clear the history file

    FILE *fp1;
    fp1 = fopen(path, "w");
    fclose(fp1);
}

int countLines(char *fname) {// count lines in file
    int lineCount = 0;
    char c;
    FILE *fp1;
    fp1 = fopen(fname, "r");
    if (fp1 != NULL) {// if the file is not null
        for (c = (char) getc(fp1); c != EOF; c = (char) getc(fp1)) {// counts the lines in the file
            if (c == '\n') // Increment count if this character is newline
                lineCount = lineCount + 1;
        }
        fclose(fp1);
    } else {
        fprintf(stderr, "could not open file");
    }
    return lineCount;
}

int explanation(char *userIn) {// a function that runs over the whole string input and checks if the user is trying to call a command from the history.

    for (int i = 0; i < strlen(userIn); i++) {
        if (userIn[i] == '!') {
            return 1;
        }
    }
    return 0;

}

void StringFun(const char *s, int *WC) {//a function to count the number of words in the input
    int word = 0, i, letters = 0;
    for (i = 0; s[i] != '\n'; i++) {
        if (s[i] != ' ' &&
            s[i] != '\n') {//if the char at that index is not a space and the char next to it is not a new line
            letters++;// adds one to the letters counter
        }
        if (letters > 0 && s[i] == ' ' && s[i + 1] != ' ' ||
            letters > 0 && s[i + 1] == '\n')// if we have letters and the char at the index is a space or a new line
            word++;// adds one to the word counter
    }//count letters and words.
    *WC = word;
}

void writeToFile(char *userIn, char *path) {// a function that takes in the users input and places it in the history file.
    FILE *fp1;
    fp1 = fopen(path, "a+");
    fprintf(fp1, "%s", userIn);
    fclose(fp1);
}

void remove_spaces(char *s) {// a function that removes the spaces from the end of a string

    unsigned long i = (strlen(s) - 1);
    if (s[i] == ' ' || s[i] == '\n') {
        while (s[i] == ' ' || s[i] == '\n') {
            s[i] = '\0';
            i--;
        }
    }
    s[i + 1] = '\n';
    s[i + 2] = '\0';
}

void printHistory(char *path) {// a function to print out the history
    FILE *fileRead;
    fileRead = fopen(path, "a+");
    if (fileRead != NULL) {
        char history[512];
        int counter = 1;
        while (fgets(history, 512, fileRead)) {
            printf("%d %s", counter, history);// prints out the file context with the number of the line.
            counter++;
        }
        fclose(fileRead);
    }
}

int checkForPipes(char *userIn) {// a function that counts the amount of pipes in the user input
    int count = 0;
    for (int i = 0; i < strlen(userIn); i++) {

        if (userIn[i] == '|')//  count how many pipes the user is using.
            count++;
    }
    return count;
}

void arrFun(char *userIn, char **command) {// a function to split the string into an array.
    int z = 0;
    int startingIndex = 0;
    int i = 0;
    while (userIn[i] == ' ')
        i++;
    for (; i < strlen(userIn) - 1; i++) {// going over the input and placing every word in it into the string array
        if (userIn[i] == ' ' || userIn[i + 1] == '\n') {
            if (userIn[i + 1] == '\n') {
                i++;
            }
            command[z] = calloc((1 + (i - startingIndex)), sizeof(char));//giving each word n+1 size for the '\0'
            if (command[z] == NULL) {// if the malloc failed.
                for (int y = 0; y < z; y++) {// free the allocated before and exit.
                    free(command[y]);
                }
                fprintf(stderr, "malloc failed");
                exit(1);
            }
            strncpy(command[z], &userIn[startingIndex],
                    (i - startingIndex));// place the input into the newly allocated array.
            z++;
            startingIndex = i + 1;
        }
        if (userIn[startingIndex] == ' ') {
            while (userIn[startingIndex] == ' ') {
                startingIndex++;
            }
            i = startingIndex;
        }
    }

}

int changString(char *userIn) {// a function that changes the user input from !(number) to the command from the history file
    char filname[] = "file.txt";
    FILE *fp1;
    fp1 = fopen(filname, "a+");
    int k = 0;
    while (userIn[k] == ' ') {
        k++;
    }
    if (userIn[k] == '!') {// check if he's trying to call something from the history
        char InputFile[512];
        char number[strlen(userIn) - k + 1];// to take the number that the user wants to run
        strncpy(number, &userIn[k + 1], (strlen(userIn) - k));// cut it
        number[strlen(userIn) - k] = '\0';
        int x = atoi(number);//turn it into an int
        if (x >= 0) {//making sure that the number inputted by the user is a legal number.
            int lines = countLines(filname);
            if (x > lines) {// checking if the number is bigger than the amount of lines we have in the file.
                fprintf(stderr, "the number is greater than the amount of lines\n");
                return -1;
            } else {// if the number is a legal number :-
                rewind(fp1);// set the pointer to the start of the file
                for (int i = 0; i < x; i++) {
                    fgets(InputFile, 512, fp1);// go over each line until we get to the desired one
                }
                strcpy(userIn, InputFile);// change the input of the user to the command from the history file.
                userIn[strlen(userIn) - 1] = '\0';
                // resend the string to the function that counts the word in it (because we changed it from !x to a command)
            }
        } else {// if the number is illegal it prints out this message.
            fprintf(stderr, "error converting the number,please use a positive number.\n");
            return -1;
        }
    }

    fclose(fp1);
    return 0;
}

void onePipeEx(char *userIn, int checkForExp, int no_hup) {// a function that takes care of the one pipe.
    /*
     * this function takes in the user input that has one pipe in it
     * then splits the string into two arrays and sends each array to a child process
     * inside the child process each array is sent to the exevp function to execute the commands in it.
     */

    int amper = 0;
    if (userIn[strlen(userIn) - 2] == '&') {
        amper = 1;
        userIn[strlen(userIn) - 2] = ' ';
        remove_spaces(userIn);
    }

    int fWC = 0;
    int sWC = 0;
    int index = 0;

    char *token = strtok(userIn, "|");
    char firstC[strlen(token) + 2];
    memset(firstC, (int) '\0', strlen(token) + 2);
    strcpy(firstC, token);

    if (no_hup == 1) {
        index = 6;
    }

    token = strtok(NULL, "|");
    char secondC[strlen(token) + 2];
    memset(secondC, (int) '\0', strlen(token) + 2);
    strcpy(secondC, token);
    if (checkForExp > 0 && changString(&firstC[index]) == -1) {// checking if the string before the pipe had any explanation mark (calling from history)
        return;
    }
    if (checkForExp > 0 && changString(secondC) == -1) {// checking if the string after the pipe had any explanation mark (calling from history)
        return;
    }
    if (strncmp(firstC, "nohup ", 6) == 0) {
        no_hup = 1;
    }

    char clean[strlen(firstC) + strlen(secondC) + 5];// combining the two strings into one.
    sprintf(clean, "%s|%s", firstC, secondC);// putting the split up strings into one string.
    if (checkForPipes(clean) > 0) {
        if (amper == 1) {// if the original input had an & at the end we add it to the newly made string.
            clean[strlen(clean) - 1] = '&';
            remove_spaces(clean);
        }

        if (checkForPipes(clean) == 2) {// if the newly created string has 2 pipes instead of one we send it to the function that handles the two pipes
            remove_spaces(clean);
            twoPipeEx(clean, checkForExp, no_hup);
            return;
        }
        if (checkForPipes(clean) > 2) {// if the newly created string has more than 2 pipes.
            fprintf(stderr, "can't use more than two pipes.\n");//print an error message.
            return;//go back to the main screen.
        }

    }

    writeToFile(clean, "file.txt");// writing the string into the history.
    remove_spaces(firstC);
    remove_spaces(secondC);

    StringFun(firstC, &fWC);
    StringFun(secondC, &sWC);

    char *fCommand[fWC + 1];// creating an array for the first command before the pipe
    arrFun(firstC, fCommand);
    fCommand[fWC] = NULL;
    StringFun(secondC, &sWC);
    char *sCommand[sWC + 1];// creating an array for the second command after the pipe
    arrFun(secondC, sCommand);
    sCommand[sWC] = NULL;


    if (strcmp(fCommand[no_hup], "cd") == 0 && fWC == 1) {// if the first command is cd
        fprintf(stderr, "this function is not available yet.\n");//print out an error message
        return;// go back to the main screen
    }

    if (strcmp(sCommand[0], "cd") == 0 && sWC == 1) {// if the second command is cd
        fprintf(stderr, "this function is not available yet.\n");// print out an error message
        return;//go back to the main screen
    }


    if (checkForExp == 1) {// if one or both of the commands where called from the history
        for (int i = 0; i < fWC; i++) {// print out what commands are going to  be running.
            printf("%s ", fCommand[i]);
        }
        printf("| ");
        for (int i = 0; i < sWC; i++) {
            printf("%s ", sCommand[i]);
        }
        printf("\n");
    }

    int pipefd[2];
    int status;
    pid_t leftpid, rightpid;// creating two child process.
    if (pipe(pipefd) < 0) {// error
        perror("pipe failed");
        for (int p = 0; p <= fWC; p++) {// free the command array
            free(fCommand[p]);
        }
        for (int p = 0; p <= sWC; p++) {// free the command array
            free(sCommand[p]);
        }
        exit(EXIT_FAILURE);
    }
    leftpid = fork();
    if (leftpid < 0) {// error
        perror("fork failed");
        for (int p = 0; p <= fWC; p++) {// free the command array
            free(fCommand[p]);
        }
        for (int p = 0; p <= sWC; p++) {// free the command array
            free(sCommand[p]);
        }
        exit(EXIT_FAILURE);
    }
    if (leftpid == 0) {// first child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        if (execvp(fCommand[no_hup], &fCommand[no_hup]) == -1) {
            perror("execvp error");//print error message
            exit(1);
        }
        close(pipefd[1]);
        exit(0);
    } else {
        rightpid = fork();
        if (rightpid < 0) {// error
            perror("fork failed");
            for (int p = 0; p <= fWC; p++) {// free the command array
                free(fCommand[p]);
            }
            for (int p = 0; p <= sWC; p++) {// free the command array
                free(sCommand[p]);
            }
            exit(1);
        }
        if (rightpid == 0) {// second child process
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            if (execvp(sCommand[0], sCommand) == -1) {
                perror("execvp error");//print error message
                exit(1);
            }
            close(pipefd[0]);
            exit(0);
        }
    }
    close(pipefd[0]);
    close(pipefd[1]);
    if (amper != 1) {// if & is inputted at the end of the user input.
        wait(&status);
        wait(&status);
    }
    if (amper == 1) {//print out the process pid.
        printf("[%d] %d\n", numOfPid, rightpid);
        numOfPid++;
    }

    for (int p = 0; p <= fWC; p++) {// free the command array
        free(fCommand[p]);
    }
    for (int p = 0; p <= sWC; p++) {// free the command array
        free(sCommand[p]);
    }
    fWC = 0;
    sWC = 0;
}

void twoPipeEx(char *userIn, int checkForExp, int no_hup) {
    /*
     * this function takes care of a two pipe input
     * splits the user input into 3 strings
     * creates 3 arrays and places each string into one of them
     * then the 3 arrays are sent off to the execvp function to run the commands.
     */
    int amper = 0;
    if (userIn[strlen(userIn) - 2] == '&') {
        amper = 1;
        userIn[strlen(userIn) - 2] = ' ';
        remove_spaces(userIn);
    }
    int fWC = 0;
    int sWC = 0;
    int tWC = 0;
    int WC = 0;
    int index = 0;

    StringFun(userIn, &WC);

    char *token = strtok(userIn, "|");
    char firstC[strlen(token)];
    strcpy(firstC, token);
    if (no_hup == 1) {
        index = 6;
    }
    if (changString(&firstC[index]) == -1) {// checking if the user is calling a command from the history file
        return;
    }
    token = strtok(NULL, "|");
    char secondC[strlen(token) + 1];
    strcpy(secondC, token);
    if (changString(secondC) == -1) {// checking if the user is calling a command from the history file.
        return;

    }

    token = strtok(NULL, "|");
    char thirdC[strlen(token)];
    strcpy(thirdC, token);
    if (changString(thirdC) == -1) {// checking if the user is calling for a command from the history file.
        return;
    }
    if (strncmp(firstC, "nohup ", 6) == 0) {
        no_hup = 1;
    }
    char clean[strlen(firstC) + strlen(secondC) + strlen(thirdC) + 6]; // combining the 3 strings into one.
    sprintf(clean, "%s|%s|%s", firstC, secondC, thirdC);
    if (amper == 1) {// if & was in the original input we added back to the newly created array.
        clean[strlen(clean) - 1] = '&';
    }
    remove_spaces(clean);
    if (checkForPipes(clean) > 2) {// making sure that the input only has 2 pipes
        fprintf(stderr, "can't use more than two pipes.\n");// if we count more than 2 we print out an error message and go back to the main screen.
        return;
    }

    writeToFile(clean, "file.txt");// send the newly created sting to the history file.
    remove_spaces(firstC);
    remove_spaces(secondC);
    remove_spaces(thirdC);

    StringFun(firstC, &fWC);
    StringFun(secondC, &sWC);
    StringFun(thirdC, &tWC);

    char *fCommand[fWC + 1];// creating an array for the first command
    arrFun(firstC, fCommand);
    fCommand[fWC] = NULL;
    char *sCommand[sWC + 1];// creating an array for the second command
    arrFun(secondC, sCommand);
    sCommand[sWC] = NULL;
    char *tCommand[tWC + 1];// creating an array for the third command
    arrFun(thirdC, tCommand);
    tCommand[tWC] = NULL;

    if (checkForExp == 1) {// if one of the commands was called from the history we print our the commands that are going to be executed.
        for (int i = 0; i < fWC; i++) {
            printf("%s ", fCommand[i]);
        }
        printf("| ");
        for (int i = 0; i < sWC; i++) {
            printf("%s ", sCommand[i]);
        }
        printf("| ");
        for (int i = 0; i < tWC; i++) {
            printf("%s ", tCommand[i]);
        }
        printf("\n");
    }

    if (strcmp(fCommand[no_hup], "cd") == 0 && fWC == no_hup + 1) {// if the first command was cd
        fprintf(stderr, "this function is not available yet.\n");
        return;
    }

    if (strcmp(sCommand[0], "cd") == 0 && sWC == 1) {// if the second command was cd
        fprintf(stderr, "this function is not available yet.\n");
        return;
    }

    if (strcmp(tCommand[0], "cd") == 0 && tWC == 1) {// if the third command was cd
        fprintf(stderr, "this function is not available yet.\n");
        return;
    }

    int pipefd[2];
    int pipefd2[2];
    int status;
    pid_t leftpid, middlepid, rightpid;
    pipe(pipefd);
    pipe(pipefd2);
    leftpid = fork();
    if (leftpid < 0) {//error
        for (int p = 0; p <= fWC; p++) {// free the command array
            free(fCommand[p]);
        }
        for (int p = 0; p <= sWC; p++) {// free the command array
            free(sCommand[p]);
        }
        for (int p = 0; p <= tWC; p++) {// free the command array
            free(tCommand[p]);
        }
        close(pipefd[0]);
        close(pipefd[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        exit(1);
    }
    if (leftpid == 0) {// first child process.
        close(pipefd[0]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        dup2(pipefd[1], STDOUT_FILENO);
        if (execvp(fCommand[no_hup], &fCommand[no_hup]) == -1) {
            perror("execvp error");//print error message
            exit(EXIT_FAILURE);
        }
        close(pipefd[1]);
        exit(0);
    }
    middlepid = fork();
    if (middlepid < 0) {//error
        for (int p = 0; p <= fWC; p++) {// free the command array
            free(fCommand[p]);
        }
        for (int p = 0; p <= sWC; p++) {// free the command array
            free(sCommand[p]);
        }
        for (int p = 0; p <= tWC; p++) {// free the command array
            free(tCommand[p]);
        }
        close(pipefd[0]);
        close(pipefd[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        exit(1);
    } else if (middlepid == 0) {// middle child process
        close(pipefd[1]);
        close(pipefd2[0]);
        dup2(pipefd[0], STDIN_FILENO);
        dup2(pipefd2[1], STDOUT_FILENO);
        if (execvp(sCommand[0], sCommand) == -1) {
            perror("execvp error");//print error message
            exit(1);
        }
        close(pipefd[0]);
        close(pipefd2[1]);
    }
    rightpid = fork();
    if (rightpid < 0) {//error
        for (int p = 0; p <= fWC; p++) {// free the command array
            free(fCommand[p]);
        }
        for (int p = 0; p <= sWC; p++) {// free the command array
            free(sCommand[p]);
        }
        for (int p = 0; p <= tWC; p++) {// free the command array
            free(tCommand[p]);
        }
        close(pipefd[0]);
        close(pipefd[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        exit(1);
    } else if (rightpid == 0) {// third child process.
        close(pipefd[0]);
        close(pipefd[1]);
        close(pipefd2[1]);
        dup2(pipefd2[0], STDIN_FILENO);
        if (execvp(tCommand[0], tCommand) == -1) {
            perror("execvp error");//print error message
            exit(1);
        }
        close(pipefd2[0]);
    } else {
        close(pipefd[0]);
        close(pipefd[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        if (amper != 1) {
            wait(&status);
            wait(&status);
            wait(&status);
        }
        if (amper == 1) {// if the command is being run in the background we print out its pid.
            printf("[%d] %d\n", numOfPid, rightpid);
            numOfPid++;
        }
    }
    /*
     * in the fathers process :-
     * freeing the arrays that where sent to the execvp function.
     * resetting all the values that where used.
     */
    for (int p = 0; p <= fWC; p++) {// free the command array
        free(fCommand[p]);
    }
    for (int p = 0; p <= sWC; p++) {// free the command array

        free(sCommand[p]);
    }
    for (int p = 0; p <= tWC; p++) {// free the command array
        free(tCommand[p]);
    }
    fWC = 0;
    sWC = 0;
    WC = 0;
}

void execute() {
    int count = 0;
    int totalWord = 0;

    char userIn[512];
    memset(userIn, (int) '\0', 512);
    char filname[] = "file.txt";
    char cwd[PATH_MAX];
    memset(cwd, (int) '\0', PATH_MAX);
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
    }// get directory
    FILE *fp1;
    fp1 = fopen(filname, "a+");
    while (1) {
        int WC = 0;
        int amper = 0;
        int no_hup = 0;
        int index = 0;
        printf("%s>", cwd);// print out the input reciver
        fgets(userIn, 512, stdin);// save the user input
        fflush(stdin);
        StringFun(userIn, &WC);//send to a function that counts the words.

        int checkForExp = explanation(userIn);

        if (checkForExp == 1 && WC == 1 && changString(userIn) != -1)

            if (userIn[0] == ' ' || userIn[strlen(userIn) - 2] == ' ') {// if there are spaces before or after the input.
                if (WC != 0) {
                    writeToFile(userIn, filname);
                    count += 1;
                    totalWord += WC;
                }
                fprintf(stderr, "Space detected at the start or at the end of the sentence\n");
                continue;
            }

        if (userIn[strlen(userIn) - 2] == '&' && checkForPipes(userIn) == 0) {
            amper = 1;
            userIn[strlen(userIn) - 2] = ' ';
            remove_spaces(userIn);
            StringFun(userIn, &WC);
        }
        if (strncmp(userIn, "nohup ", 6) == 0) {
            no_hup = 1;
            signal(SIGHUP, SIG_IGN);
        }
        if (no_hup == 1) {
            index = 6;
        }

        if (checkForPipes(userIn) == 0 && changString(&userIn[index]) == -1) {
            continue;
        }
        remove_spaces(userIn);
        if (userIn[strlen(userIn) - 2] == '&' && checkForPipes(userIn) == 0) {
            amper = 1;
            userIn[strlen(userIn) - 2] = ' ';
            remove_spaces(userIn);
            StringFun(userIn, &WC);
        }
        if (checkForPipes(userIn) > 0) {
            if (checkForPipes(userIn) == 1) {
                onePipeEx(userIn, checkForExp, no_hup);
                continue;

            } else if (checkForPipes(userIn) == 2) {
                twoPipeEx(userIn, checkForExp, no_hup);
                continue;
            }
            if (checkForPipes(userIn) > 2) {
                fprintf(stderr, "can't use more than two pipes.\n");
            }
            continue;

        }

        if (strncmp(userIn, "done", 4) == 0 && strlen(userIn) == 5) {// if the user inputted the word done
            printf("Num of commands: %d\n", count + 1);//print the number of commands from the user
            printf("total number of words in all commands: %d\n", totalWord);// the number of words from the user
            fclose(fp1);
            exit(0);
        } else if (strncmp(userIn, "history -c", 10) == 0 && strlen(userIn) == 11) {
            printf("history cleared\n");
            clear_history(filname);
        } else if (strncmp(userIn, "cd", 2) == 0 && strlen(userIn) == 3) {
            // if the user inputted "cd"
            fprintf(stderr, "this function is not available (yet)\n");
            count += 1;
            totalWord += 1;
        } else if (strncmp(userIn, "history", 7) == 0) {// if the input is history
            writeToFile(userIn, filname);// calls the function that prints out the history
            printHistory(filname);
            count += 1;
            totalWord += 1;
        } else {// if the user inputted anything other than the ones listed above
            if (fp1 == NULL) {
                fprintf(stderr, "could not open file");
                break;
            }// if the file pointer is null
            if (WC == 0)
                continue;
            if (strncmp(userIn, "history", 7) == 0) {// after changing the input from !x to the command from the history file we have to check if the command is history.
                printHistory(filname);
                count += 1;
                totalWord += 1;
                continue;
            }
            StringFun(userIn, &WC);
            char *command[WC + 1];
            command[WC] = NULL;// last index in the array is set to null// the string array to be sent to the execvp function
            arrFun(userIn, command);
            if (amper == 1) {
                userIn[strlen(userIn) - 1] = '&';
                remove_spaces(userIn);
            }
            writeToFile(userIn, filname);// place the input in file(write to file)

            if (checkForExp == 1) {
                printf("%s ", userIn);
            }
            int fp3;
            if (no_hup == 1) {
                fp3 = open("nohup.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                if (fp3 < 0) {
                    perror("could not create file\n");
                }
            }
            pid_t child = fork();
            if (child < 0) {
                perror("fork failed");
                for (int p = 0; p <= WC; p++) {// free the command array
                    free(command[p]);
                }
                exit(1);
            }
            if (child == 0) {//child process
                if (no_hup == 1) {
                    dup2(fp3, STDOUT_FILENO);
                }
                if (execvp(command[no_hup], &command[no_hup]) == -1) {//if the execvp function does not work :-
                    perror("execvp error");//print error message
                    exit(EXIT_FAILURE);
                }

                exit(0);
            }
            if (amper == 1) {
                printf("[%d] %d\n", numOfPid, child);
                numOfPid++;
            }
            if (no_hup == 1) {
                close(fp3);
            }
            if (amper != 1) {
                wait(NULL);//wait for the child process to finish
            }
            for (int p = 0; p <= WC; p++) {// free the command array
                free(command[p]);
            }
            count += 1;
            totalWord += WC;
        }
    }
}

int main() {

    signal(SIGCHLD, handler);
    execute();

    return 0;
}