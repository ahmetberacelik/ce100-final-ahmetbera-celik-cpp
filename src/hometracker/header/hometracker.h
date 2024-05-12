/**
 * @file hometracker.h
 *
 * @brief Provides functions for math. utilities
 */
#ifndef HOMETRACKER_H
#define HOMETRACKER_H
#include <iostream>

typedef struct{
    char username[50];
    char reminderText[100];
    int daysAfter;
}Reminder;

typedef struct{
    char username[50];
    char password[50];
}User;

void clearScreen();

bool mainMenu(bool authenticationResult, std::istream& in, std::ostream& out);

int saveUser(const User* user, const char* filename);

int authenticateUser(const char* username, const char* password, const char* filename);

bool userAuthentication(std::istream& in, std::ostream& out);

int saveReminder(const Reminder* reminder, const char* filename);

int loadReminders(const char* username, const char* filename, Reminder* reminders, int maxReminders);

void printReminders(const Reminder* reminders, int reminderCount, std::istream& in, std::ostream& out);

bool ReminderSetup(std::istream& in, std::ostream& out, bool localGuestMode);

#endif // HOMETRACKER_H