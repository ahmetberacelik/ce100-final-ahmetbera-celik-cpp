/**
 * @file hometracker.h
 *
 * @brief Provides functions for math. utilities
 */
#ifndef HOMETRACKER_H
#define HOMETRACKER_H
#include <iostream>
#include <cstdio>
#include <cstring>
#include <climits>
#include "../../des/header/des.h"
const int MAX_NEIGHBORS = 10;
typedef struct{
    char username[50];
    char reminderText[100];
    int daysAfter;
}Reminder;

typedef struct{
    char username[50];
    char password[50];
}User;

struct UtilityUsage {
    char username[50];
    double electricity = 0;
    double water = 0;
    double gas = 0;
};

struct Node {
    char username[50];
    double electricity;
    double water;
    double gas;
    Node* neighbors[10];
    int neighborCount;

    // Default constructor
    Node() : electricity(0), water(0), gas(0), neighborCount(0) {
        username[0] = '\0';  // Initialize username with an empty string
    }

    // Constructor with username parameter
    Node(const char* uname) : electricity(0), water(0), gas(0), neighborCount(0) {
        strcpy(username, uname);
    }
};

typedef struct {
    int u, v, weight;
} Edge;

const double ELECTRICITY_PRICE_PER_KWH = 0.145;
const double WATER_PRICE_PER_CUBIC_METER = 1.30;
const double GAS_PRICE_PER_CUBIC_METER = 0.75;

void clearScreen();

bool mainMenu(bool authenticationResult, std::istream& in, std::ostream& out);

des_block_t getFixedPassphrase();

void encryptUserCredentials(const char* input, char* output);

void decryptUserCredentials(const char* input, char* output);

void hashPassword(const char* password, char* outputHash);

int saveUser(const User* user, const char* filename);

int authenticateUser(const char* username, const char* password, const char* filename);

bool userAuthentication(std::istream& in, std::ostream& out);

bool saveUtilityUsage(const UtilityUsage& usage, const char* filename);

int loadUtilityUsages(const char* filename, UtilityUsage* usages, int maxUsages);

int lcs(char* X, char* Y, int m, int n);

void BFS(Node* startNode, std::ostream& out);

void DFS(Node* startNode, std::ostream& out);

bool loadGraph(const char* filename);

bool viewUtilityUsages(std::istream& in, std::ostream& out, int searchType);

int findPath(int source, int sink, int parent[], Node nodes[], int numNodes);

int fordFulkerson(Node nodes[], int numNodes, int source, int sink);

int edmondsKarp(Node nodes[], int numNodes, int source, int sink);

bool calculateAndShowMaximumFlow(std::istream& in, std::ostream& out);

void dijkstra(int source, int dist[], int prev[]);

void bellmanFord(int source, int dist[]);

void initializeCosts();

void primMST(std::ostream& out);

int find(int i);

void union1(int i, int j);

void kruskalMST(std::ostream& out);

bool utilityLogging(std::istream& in, std::ostream& out, bool localGuestMode);

bool calculateAndShowExpenses(std::istream& in, std::ostream& out, const char* activeUser, bool guestMode);

void showTrendAnalysis(std::istream& in, std::ostream& out);

int saveReminder(const Reminder* reminder, const char* filename);

int loadReminders(const char* username, const char* filename, Reminder* reminders, int maxReminders);

bool printReminders(const Reminder* reminders, int reminderCount, std::istream& in, std::ostream& out);

bool ReminderSetup(std::istream& in, std::ostream& out, bool localGuestMode);

#endif // HOMETRACKER_H