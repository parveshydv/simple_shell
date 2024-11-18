# simple_shell

This project implements a basic shell interpreter (SSI) in C that mimics core functionality of a Unix shell. It supports foreground and background execution of commands, directory navigation, and dynamic prompts showing the current directory.

Features
* Foreground Execution: Execute commands like ls, ping, etc. in the foreground.
* Changing Directories: Supports cd for changing directories (absolute, relative, and special paths like ~ for home).
* Background Execution: Supports executing commands in the background with the bg command, and listing background jobs using bglist.
* Prompt: Displays the current directory in the prompt, updating when directories change.


How to Compile and Run


1. Clone the repository:

      git clone <repository_url>
      cd simple_shell



2. Compile using the provided Makefile:

      make


    
3. Run the shell:

     ./ssi



The prompt will appear, waiting for user input.


