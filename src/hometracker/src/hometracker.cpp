#include "../header/hometracker.h"
#include <iostream>

bool guestMode = false; /**< Boolean variable to indicate whether the program is in guest mode (false by default). */
char active_user[50];

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 * @brief Displays the main menu options and handles user input.
 *
 * @param authenticationResult Indicates whether the user is authenticated.
 * @return True if the user chooses to exit the program, otherwise false.
 */
bool mainMenu(bool authenticationResult, std::istream& in, std::ostream& out) {
    if (!authenticationResult) {
        return false;
    }
    int choice;
    while (true) {
        clearScreen();
        out << "+-------------------------------------+\n";
        out << "|            MAIN MENU                |\n";
        out << "+-------------------------------------+\n";
        out << "| 1. Utility Logging                  |\n";
        out << "| 2. Expense Calculation              |\n";
        out << "| 3. Trend Analysis                   |\n";
        out << "| 4. Reminder Setup                   |\n";
        out << "| 5. Exit                             |\n";
        out << "+-------------------------------------+\n";
        out << "Please select an option: ";

        if (!(in >> choice)) {
            in.clear();
            in.get();
            out << "Invalid input, please enter a number.\n";
            continue;
        }
        in.get();

        switch (choice) {
        case 1:
            out << "Hi\n";
            break;
        case 2:
            out << "Hi\n";
            break;
        case 3:
            out << "Hi\n";
            break;
        case 4:
            ReminderSetup(in, out, guestMode);
            break;
        case 5:
            out << "Exiting program...Press enter!\n";
            in.get();
            return true;
        default:
            out << "Invalid option, please try again.\n";
            in.get();
        }
    }
}
/**
 * @brief Saves user information to a binary file.
 *
 * @param user Pointer to the User structure containing user information.
 * @param filename Name of the file to save the user information.
 * @return 1 if the user information is successfully saved, otherwise 0.
 */
int saveUser(const User* user, const char* filename) {
    FILE* file = fopen(filename, "ab");
    fwrite(user, sizeof(User), 1, file);
    fclose(file);
    return 1;
}
/**
 * @brief Authenticates a user by checking the provided username and password against stored records.
 *
 * @param username The username entered by the user.
 * @param password The password entered by the user.
 * @param filename Name of the file containing user records.
 * @return 1 if authentication is successful, -1 if the file cannot be opened, otherwise 0.
 */
int authenticateUser(const char* username, const char* password, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return -1;
    }
    User user;
    while (fread(&user, sizeof(User), 1, file)) {
        if (strcmp(user.username, username) == 0 && strcmp(user.password, password) == 0) {
            fclose(file);
            return 1;
        }
    }
}
/**
 * @brief Handles user authentication, login, registration, and guest mode selection.
 *
 * @return True if the user successfully logs in or registers, otherwise false.
 */
bool userAuthentication(std::istream& in, std::ostream& out) {
    clearScreen();
    const char* filename = "Users.bin";
    User user1 = { "Ahmet Bera Celik", "qwerty" };
    saveUser(&user1, filename);
    User user2 = { "Enes Koy", "123456" };
    saveUser(&user2, filename);
    User user3 = { "Ugur Coruh", "asdasd" };
    saveUser(&user3, filename);

    int choice;
    int right_to_try = 3;
    char temp_username[50];
    char temp_password[50];

    while (true) {
        clearScreen();
        out << "+---------------------------+\n";
        out << "|  LOGIN AND REGISTER MENU  |\n";
        out << "+---------------------------+\n";
        out << "| 1. Login                  |\n";
        out << "| 2. Register               |\n";
        out << "| 3. Guest Mode             |\n";
        out << "| 4. Exit Program           |\n";
        out << "+---------------------------+\n";
        out << "Please select an option: ";
        in >> choice;
        in.get();

        switch (choice) {
        case 1:
            out << "Please enter your username: ";
            in.getline(temp_username, 50);

            out << "Please enter your password: ";
            in.getline(temp_password, 50);

            if (authenticateUser(temp_username, temp_password, filename) == 1) {
                clearScreen();
                out << "Welcome " << temp_username << "\n";
                strcpy(active_user, temp_username);
                in.get();
                return true;
            }
            else {
                out << "You entered wrong username or password. Please try again.\n";
                right_to_try--;
                if (right_to_try == 0) {
                    out << "You have run out of login attempts. See you...\n";
                    in.get();
                    return false;
                }
            }
            in.get();
            break;

        case 2:
            out << "Please enter your username: ";
            in.getline(temp_username, 50);

            out << "Please enter your password: ";
            in.getline(temp_password, 50);

            User newUser;
            strcpy(newUser.username, temp_username);
            strcpy(newUser.password, temp_password);
            saveUser(&newUser, filename);
            clearScreen();
            out << "User registered successfully.\n" << "Welcome " << temp_username << "\n";
            strcpy(active_user, temp_username);
            in.get();
            return true;

        case 3:
            guestMode = true;
            return true;

        case 4:
            out << "Exiting program...Press enter!\n";
            in.get();
            return false;

        default:
            out << "Invalid option, please try again.\n";
            in.get();
        }
    }
}

int saveReminder(const Reminder* reminder, const char* filename) {
    FILE* file = fopen(filename, "ab");
    fwrite(reminder, sizeof(Reminder), 1, file);
    fclose(file);
    return 1;
}

int loadReminders(const char* username, const char* filename, Reminder* reminders, int maxReminders) {
    FILE* file = fopen(filename, "rb");
    int reminderCount = 0;
    while (fread(&reminders[reminderCount], sizeof(Reminder), 1, file)) {
        if (strcmp(reminders[reminderCount].username, username) == 0) {
            reminderCount++;
            if (reminderCount >= maxReminders) {
                break;
            }
        }
    }
    fclose(file);
    return reminderCount;
}

void printReminders(const Reminder* reminders, int reminderCount, std::istream& in, std::ostream& out) {
    if (reminderCount == 0) {
        out << "You have no reminders.\n";
        in.get();
        return;
    }

    out << "+------------------------------------+\n";
    out << "|        Upcoming Reminders          |\n";
    out << "+------------------------------------+\n";
    for (int i = 0; i < reminderCount; i++) {
        out << "Reminder " << i + 1 << ":\n";
        out << "Username: " << reminders[i].username << "\n";
        out << "Reminder Text: " << reminders[i].reminderText << "\n";
        out << "Days After: " << reminders[i].daysAfter << "\n";
        out << "--------------------------------------\n";
    }
    in.get();
}


bool ReminderSetup(std::istream& in, std::ostream& out, bool localGuestMode) {
    if (localGuestMode) {
        out << "Guest mode does not have permission to set reminders.\n";
        in.get();
        return false;
    }

    const char* filename = "Reminders.bin";
    Reminder reminders[100];
    int reminderCount = 0;

    int choice;
    while (true) {
        clearScreen();
        out << "+---------------------------------+\n";
        out << "|       REMINDER SETUP MENU       |\n";
        out << "+---------------------------------+\n";
        out << "| 1. Set Payment Reminders        |\n";
        out << "| 2. View Upcoming Reminders      |\n";
        out << "| 3. Back to Main Menu            |\n";
        out << "+---------------------------------+\n";
        out << "Please select an option: ";
        if (!(in >> choice)) {
            in.clear();
            in.get();
            out << "Invalid input, please enter a number.\n";
            continue;
        }
        in.get();
        switch (choice) {
        case 1:
            out << "Enter reminder text: ";
            in.getline(reminders[reminderCount].reminderText, 100);

            out << "Enter number of days after which to remind: ";
            if (!(in >> reminders[reminderCount].daysAfter)) {
                in.clear();
                in.get();
                out << "Invalid input for days, please enter a number.\n";
                break;
            }
            in.get();

            strcpy(reminders[reminderCount].username, active_user); // Set active user for reminder

            if (saveReminder(&reminders[reminderCount], filename)) {
                out << "Reminder saved successfully.\n";
                reminderCount++;
            }
            in.get();
            break;
        case 2:
            reminderCount = loadReminders(active_user, filename, reminders, 100); // Load reminders for active user
            printReminders(reminders, reminderCount, in, out);
            break;
        case 3:
            return false;
        default:
            out << "Invalid option, please try again.\n";
        }
    }
}
