#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

//to compile use below format
/* g++ -std=c++11 your_file.cpp -o your_program */

vector<string>command_history;
void display_history(){ 
    if (command_history.empty()){
        cout << "No commands in history" << endl;
    } else{
        cout<<"History of Commands:"<< endl;
        for (int i = 0; i < command_history.size(); i++ ){
            cout << i + 1 << ". " << command_history[i] << endl;
            }
    }
}

//clear history
void clear_history(){
    command_history.clear();
    cout << "Commands history cleared"<< endl;
}

#define max_children 3
//show process ID
#include <iostream>
#include <unistd.h> // for getpid() and fork() functions
#include <sys/wait.h> // for waitpid() function

#define MAX_CHILDREN 3 // maximum number of child processes to keep track of

void showpid() {
    pid_t pid = getpid(); // get process ID
    std::cout << "Process ID: " << pid << std::endl; // print process ID

    pid_t child_pids[MAX_CHILDREN] = {0}; // initialize child PID array to 0
    int pid_index = 0; // current index in child PID array

    // create 3 child processes
    for (int i = 0; i < MAX_CHILDREN; i++) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            std::cerr << "Failed to fork child process." << std::endl;
            break;
        }
        else if (child_pid == 0) {
            // child process code here
            // ...
            exit(0); // exit child process
        }
        else {
            // parent process code here
            child_pids[pid_index] = child_pid; // add child PID to array
            pid_index = (pid_index + 1) % MAX_CHILDREN; // update PID array index
        }
    }

    // wait for child processes to finish
    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (child_pids[i] != 0) {
            int status;
            waitpid(child_pids[i], &status, 0);
        }
    }

    // print last 3 child process IDs
    std::cout << "Last 3 child process IDs:\n ";
    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (child_pids[i] != 0) {
            std::cout << child_pids[i] << " ";
        }
    }
    std::cout << std::endl;
}


//clear screen
void clear_screen(){
    #ifdef _WIN32
        system("cls"); // for Windows
    #else
        system("clear"); // for Unix/Linux
    #endif
}

// Function to split a string into tokens
vector<string> split(const string& s, char delimiter){
    vector<string> tokens;
    string token;
    for (char c : s) {
        if (c == delimiter) {
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to execute a command
void execute_commands(vector<string>args){
    // Convert vector of strings to array of char pointers
    char** argv = new char*[args.size() + 1];
    for (int i = 0; i < args.size(); ++i) {
        argv[i] = new char[args[i].size() + 1];
        strcpy(argv[i], args[i].c_str());
    }
    argv[args.size()] = nullptr;
    // Execute the command
    pid_t shell_pid = getpid();
    pid_t pid = fork();
    pid_t pids[max_children] = {0};
    int pid_index = 0;
    if (pid == 0) {
        // Child process
        execvp(argv[0], argv);
        cerr << "Failed to execute command: " << strerror(errno) << endl;
        exit(1);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    } else {
        cerr << "Failed to fork process: " << strerror(errno) << endl;
    }

    // Free memory allocated for char pointers
    for (int i = 0; i < args.size(); ++i) {
        delete[] argv[i];
    }
    delete[] argv;
}

int main(){
    while (true) {
        // Display prompt and read input
        cout << "osclass> ";
        fflush(stdout);
        string input;
        getline(cin, input);
        // Split input into tokens
        vector<string> tokens = split(input, ' ');
        
        if (tokens[0] == "history"){
            display_history();
            continue;
        } else if(tokens[0] == "clearhistory"){
            clear_history();
            continue;
        } else if (tokens[0] == "showpid"){
            showpid();
            continue;
        } else {
            command_history.push_back(input);
        }

        // Check if input is empty
        if (tokens.empty()) {
            continue;
        }
        // Check for built-in commands
        if (tokens[0] == "cd") {
            if (tokens.size() < 2) {
                cerr << "cd: missing argument" << endl;
            } else if (chdir(tokens[1].c_str()) != 0) {
                cerr << "cd: " << strerror(errno) << endl;
            }
            //update the PWD environmental variable with setenv()
            char cwd[1024]; //cwd current w orking directory
            std::string new_pwd = getcwd(cwd, sizeof(cwd));
            if (setenv("PWD", new_pwd.c_str(), 1) == 0 ){
                std:: cout << "PWD updated to " << new_pwd << std::endl;
            }
        //clears screen
        } else if (tokens[0] == "clear"){
            clear_screen();
        } else if (tokens[0] == "exit"){
            break;
        } else {
            // Execute command
            execute_commands(tokens);
        } 
    }
    return 0;
}