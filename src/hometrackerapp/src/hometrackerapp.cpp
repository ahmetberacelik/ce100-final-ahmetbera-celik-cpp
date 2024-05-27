/**
 * @file hometrackerapp.cpp
 * @brief A simple program to demonstrate the usage of the hometracker model class.
 *
 */
#include <iostream>
#include "../../hometracker/header/hometracker.h"  // Adjust this include path based on your project structure
 /**
  * @brief The entry point of the program.
  *
  * This function serves as the entry point of the program. It first performs user authentication
  * and then displays the main menu based on the authentication result.
  *
  * @return Returns 0 to indicate successful execution of the program.
  */
int main() {
    bool authenticationResult = userAuthentication(std::cin, std::cout);
    mainMenu(authenticationResult, std::cin, std::cout);
    return 0;

}
