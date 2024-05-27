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
 /**
  * @brief Structure representing a reminder.
  *
  * This structure holds information about a reminder, including the username of the user, the reminder text,
  * and the number of days after which the reminder should be triggered.
  */
typedef struct {
    char username[50]; ///< Username of the user associated with the reminder.
    char reminderText[100]; ///< Text of the reminder.
    int daysAfter; ///< Number of days after which the reminder should be triggered.
} Reminder;
/**
 * @brief Structure representing a user.
 * 
 * This structure holds information about a user, including the username and password.
 */
typedef struct {
    char username[50]; ///< Username of the user.
    char password[50]; ///< Password of the user.
} User;
/**
 * @brief Structure representing utility usage.
 *
 * This structure holds information about utility usage, including the username of the user,
 * as well as the amount of electricity, water, and gas consumed.
 */
struct UtilityUsage {
    char username[50]; ///< Username of the user.
    double electricity; ///< Amount of electricity consumed.
    double water; ///< Amount of water consumed.
    double gas; ///< Amount of gas consumed.
};
/**
 * @brief Structure representing a node in a graph.
 *
 * This structure represents a node in a graph used for utility usage analysis. It contains
 * information about utility usage, including electricity, water, and gas consumption, as well
 * as connections to neighboring nodes.
 */
struct Node {
    char username[50];///< Username associated with the node.
    double electricity;///< Electricity consumption of the node.
    double water;///< Water consumption of the node.
    double gas;///< Gas consumption of the node.
    Node* neighbors[10];///< Array of pointers to neighboring nodes.
    int neighborCount;///< Number of neighboring nodes.

    /**
 * Initialize username with an empty string
 */
    Node() : electricity(0), water(0), gas(0), neighborCount(0) {
        username[0] = '\0';  
    }

    /**
 * Initialize username with an empty string
 */
    Node(const char* uname) : electricity(0), water(0), gas(0), neighborCount(0) {
        strcpy(username, uname);
    }
};
/**
 * @brief Structure representing an edge in a graph.
 *
 * This structure holds information about an edge in a graph, including the source vertex, destination vertex,
 * and the weight of the edge.
 */
typedef struct {
    int u; ///< Source vertex of the edge.
    int v; ///< Destination vertex of the edge.
    int weight; ///< Weight of the edge.
} Edge;

/**
 * @brief Price per unit of electricity (kWh).
 */
const double ELECTRICITY_PRICE_PER_KWH = 0.145;

/**
 * @brief Price per unit of water (cubic meter).
 */
const double WATER_PRICE_PER_CUBIC_METER = 1.30;

/**
 * @brief Price per unit of gas (cubic meter).
 */
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