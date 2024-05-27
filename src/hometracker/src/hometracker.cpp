/**
 * @file farmermarket.cpp
 *
 * @brief The functions are filled in this file.
 */
#include "../header/hometracker.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <queue>
#include <climits>
#include "../../des/header/des.h"
#include "../../sha256/header/sha256.h"
#include "../../aes/header/aes.h"

#define MAX_HOUSES 100/** Maximum number of houses. */
#define INF INT_MAX /** Infinity value for representing unreachable distances. */

int costs[MAX_HOUSES][MAX_HOUSES]; /** 2D array representing the costs (distances) between houses. */

int numHouses = 3;/** Number of houses in the graph. */
Node nodes[100];/** Array of nodes representing utility usage data. */
int nodeCount = 0;/** Number of nodes currently in use. */
bool guestMode = false; /**< Boolean variable to indicate whether the program is in guest mode (false by default). */

char active_user[50]; /** Username of the currently active user. */

int parent[MAX_HOUSES];/** Array to store parent information for graph algorithms. */
/**
 * @brief Clears the console screen.
 */
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
            utilityLogging(in, out, guestMode);
            break;
        case 2:
            calculateAndShowExpenses(in, out, active_user, guestMode);
            break;
        case 3:
            showTrendAnalysis(in, out);
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
 * @brief Generates a fixed DES key for encrypting and decrypting user credentials.
 *
 * This function returns a fixed DES key represented by a des_block_t struct.
 *
 * @return The fixed DES key.
 */
des_block_t getFixedPassphrase() {
    des_block_t key;
    key.c = 0x01234567;
    key.d = 0x89abcdef;
    return key;
}
/**
 * @brief Encrypts user credentials using DES encryption.
 *
 * @param input The input user credentials to be encrypted.
 * @param output The output buffer to store the encrypted credentials.
 */
void encryptUserCredentials(const char* input, char* output) {
    char buffer[9] = { 0 };
    strncpy(buffer, input, 8);

    des_block_t key = getFixedPassphrase();
    des_block_t* key_schedule = generate_key_schedule(key);
    des_block_t block = make_block((int32_t*)buffer);
    block = encode_block(block, key_schedule, ENCODE);
    memcpy(output, &block, sizeof(des_block_t));
    free(key_schedule);
}
/**
 * @brief Decrypts user credentials using DES decryption.
 *
 * @param input The input encrypted user credentials.
 * @param output The output buffer to store the decrypted credentials.
 */
void decryptUserCredentials(const char* input, char* output) {
    des_block_t key = getFixedPassphrase();
    des_block_t* key_schedule = generate_key_schedule(key);
    des_block_t block;
    memcpy(&block, input, sizeof(des_block_t));
    block = encode_block(block, key_schedule, DECODE);
    memcpy(output, &block, sizeof(des_block_t));
    free(key_schedule);
}
/**
 * @brief Computes the SHA-256 hash of a password.
 *
 * This function computes the SHA-256 hash of the given password and stores the result in outputHash.
 *
 * @param password The password to be hashed.
 * @param outputHash The buffer to store the resulting hash.
 */
void hashPassword(const char* password, char* outputHash) {
    sha256_hex(password, strlen(password), outputHash);
}
/**
 * @brief Saves user data to a file.
 *
 * This function appends the user data to the specified file.
 *
 * @param user Pointer to the User structure containing the user data.
 * @param filename The name of the file to save the user data to.
 * @return Returns 1 if the user data is successfully saved, otherwise returns 0.
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
    if (!file) return -1;

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
        if (!(in >> choice)) {
            in.clear();
            in.get();
            out << "Invalid input, please enter a number.\n";
            continue;
        }
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
                return true; }
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
/**
 * @brief Saves or updates utility usage data to a file.
 *
 * This function saves or updates the utility usage data of a user to the specified file.
 * If the user already exists in the file, their usage data is updated; otherwise, a new entry is added.
 *
 * @param usage The UtilityUsage structure containing the usage data to be saved.
 * @param filename The name of the file to save the utility usage data to.
 * @return Returns true if the utility usage data is successfully saved, otherwise returns false.
 */
bool saveUtilityUsage(const UtilityUsage& usage, const char* filename) {
    UtilityUsage usages[100];
    int count = loadUtilityUsages(filename, usages, 100);
    bool found = false;
    for (int i = 0; i < count; i++) {
        if (strcmp(usages[i].username, usage.username) == 0) {
            usages[i].electricity = usage.electricity;
            usages[i].water = usage.water;
            usages[i].gas = usage.gas;
            found = true;
            break;
        } }
    if (!found) {
        usages[count++] = usage;
    }

    FILE* file = fopen(filename, "wb");
    fwrite(usages, sizeof(UtilityUsage), count, file);
    fclose(file);
    return true;
}
/**
 * @brief Loads utility usage data from a file into an array.
 *
 * This function loads utility usage data from the specified file into the provided array of UtilityUsage structures.
 *
 * @param filename The name of the file to load the utility usage data from.
 * @param usages The array to store the loaded utility usage data.
 * @param maxUsages The maximum number of utility usages that can be loaded into the array.
 * @return Returns the number of utility usages loaded into the array.
 */
int loadUtilityUsages(const char* filename, UtilityUsage* usages, int maxUsages) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return 0;
    }
    int count = 0;
    while (fread(&usages[count], sizeof(UtilityUsage), 1, file)) {
        if (++count >= maxUsages) break;
    }
    fclose(file);
    return count;
}
/**
 * @brief Computes the length of the longest common subsequence (LCS) of two strings.
 *
 * This function computes the length of the longest common subsequence (LCS) of two strings X and Y.
 *
 * @param X The first string.
 * @param Y The second string.
 * @param m The length of the first string.
 * @param n The length of the second string.
 * @return Returns the length of the LCS of the two strings.
 */
int lcs(char* X, char* Y, int m, int n)
{
    int** L;
    L = (int**)malloc((m + 1) * sizeof(int*));
    for (int i = 0; i <= m; i++) {
        L[i] = (int*)malloc((n + 1) * sizeof(int));
    }

    int i, j;

    for (i = 0; i <= m; i++)
    {
        for (j = 0; j <= n; j++)
        {
            if (i == 0 || j == 0)
                L[i][j] = 0;
            else if (X[i - 1] == Y[j - 1])
                L[i][j] = L[i - 1][j - 1] + 1;
            else
                L[i][j] = (L[i - 1][j] > L[i][j - 1]) ? L[i - 1][j] : L[i][j - 1];
        }
    }

    int result = L[m][n];

    for (int i = 0; i <= m; i++) {
        free(L[i]);
    }
    free(L);

    return result;
}
/**
 * @brief Performs Breadth-First Search (BFS) traversal on a graph starting from a specified node.
 *
 * This function performs a Breadth-First Search (BFS) traversal on a graph starting from the specified node.
 * It prints utility usage information of each node visited during the traversal.
 *
 * @param startNode Pointer to the starting node of the BFS traversal.
 * @param out The output stream to print the traversal results to.
 */
void BFS(Node* startNode, std::ostream& out) {
    Node* queue[100];
    bool visited[100] = { false };
    int front = 0, rear = 0;

    queue[rear++] = startNode;
    visited[startNode->username[0] - 'A'] = true;  // Simple hash for visitation check

    while (front != rear) {
        Node* currentNode = queue[front++];
        out << "Username: " << currentNode->username << ", Electricity: " << currentNode->electricity
            << ", Water: " << currentNode->water << ", Gas: " << currentNode->gas << std::endl;

        for (int i = 0; i < currentNode->neighborCount; ++i) {
            Node* neighbor = currentNode->neighbors[i];
            if (!visited[neighbor->username[0] - 'A']) {
                queue[rear++] = neighbor;
                visited[neighbor->username[0] - 'A'] = true;
            }
        }
    }
}
/**
 * @brief Performs Depth-First Search (DFS) traversal on a graph starting from a specified node.
 *
 * This function performs a Depth-First Search (DFS) traversal on a graph starting from the specified node.
 * It prints utility usage information of each node visited during the traversal.
 *
 * @param startNode Pointer to the starting node of the DFS traversal.
 * @param out The output stream to print the traversal results to.
 */

void DFS(Node* startNode, std::ostream& out) {
    Node* stack[100];
    bool visited[100] = { false };
    int top = 0;

    stack[top++] = startNode;

    while (top != 0) {
        Node* currentNode = stack[--top];
        if (!visited[currentNode->username[0] - 'A']) {
            out << "Username: " << currentNode->username << ", Electricity: " << currentNode->electricity
                << ", Water: " << currentNode->water << ", Gas: " << currentNode->gas << std::endl;
            visited[currentNode->username[0] - 'A'] = true;
        }

        for (int i = 0; i < currentNode->neighborCount; ++i) {
            Node* neighbor = currentNode->neighbors[i];
            if (!visited[neighbor->username[0] - 'A']) {
                stack[top++] = neighbor;
            }
        }
    }
}
/**
 * @brief Loads utility usage data of the active user from a file into a graph.
 *
 * This function loads utility usage data of the active user from the specified file into a graph.
 *
 * @param filename The name of the file to load the utility usage data from.
 * @return Returns true if the utility usage data is successfully loaded into the graph, otherwise returns false.
 */
bool loadGraph(const char* filename) {
    UtilityUsage usages[100];
    int usageCount = loadUtilityUsages(filename, usages, 100);

    nodeCount = 0;  // Reset node count

    for (int i = 0; i < usageCount; ++i) {
        if (strcmp(usages[i].username, active_user) == 0) {  // Only load active user's data
            strcpy(nodes[nodeCount].username, usages[i].username);  // Use strcpy to copy username
            nodes[nodeCount].electricity = usages[i].electricity;
            nodes[nodeCount].water = usages[i].water;
            nodes[nodeCount].gas = usages[i].gas;
            nodeCount++;
            break;  // Since only one user's data is needed
        } }
    return true;
}
/**
 * @brief Views utility usages of the active user using a specified search method.
 *
 * This function views utility usages of the active user using either Breadth-First Search (BFS) or Depth-First Search (DFS).
 *
 * @param in The input stream for user interaction.
 * @param out The output stream to print the utility usages to.
 * @param searchType The selected search method (1 for BFS, 2 for DFS).
 * @return Returns true if the utility usages are successfully viewed, otherwise returns false.
 */
bool viewUtilityUsages(std::istream& in, std::ostream& out, int searchType) {
    if (nodeCount == 0) {
        out << "No utility data available for the current user.\n";
        return false;
    }

    out << "Select search method:\n1. BFS\n2. DFS\n";
    int choice;
    in >> choice;

    if (choice == 1) {
        out << "Utility usages (BFS) for " << active_user << ":\n";
        BFS(&nodes[0], out);  // Assuming single user node
    }
    else if (choice == 2) {
        out << "Utility usages (DFS) for " << active_user << ":\n";
        DFS(&nodes[0], out);  // Assuming single user node
    }
    else {
        out << "Invalid search type selected.\n";
    }
    in.get();
    return true;
}
/**
 * @brief Finds a path in the graph using Breadth-First Search (BFS) for flow computation.
 *
 * This function finds a path in the graph using Breadth-First Search (BFS) for flow computation in Ford-Fulkerson algorithm.
 *
 * @param source The index of the source node.
 * @param sink The index of the sink node.
 * @param parent An array to store the parent nodes of each node in the path.
 * @param nodes An array of Node structures representing the graph.
 * @param numNodes The number of nodes in the graph.
 * @return Returns the flow of the found path.
 */
int findPath(int source, int sink, int parent[], Node nodes[], int numNodes) {
    bool visited[100] = { false };
    std::queue<int> queue;
    queue.push(source);
    visited[source] = true;
    parent[source] = -1;

    while (!queue.empty()) {
        int u = queue.front();
        queue.pop();

        for (int i = 0; i < nodes[u].neighborCount; i++) {
            int v = nodes[u].neighbors[i]->username[0] - 'A';
            if (!visited[v] && nodes[u].neighbors[i]->electricity > 0) {
                if (v == sink) {
                    parent[v] = u;
                    return nodes[u].neighbors[i]->electricity;
                }
                queue.push(v);
                visited[v] = true;
                parent[v] = u;
            }
        }
    }
    return 0;
}
/**
 * @brief Computes the maximum flow in a graph using the Ford-Fulkerson algorithm.
 *
 * This function computes the maximum flow in a graph using the Ford-Fulkerson algorithm.
 *
 * @param nodes An array of Node structures representing the graph.
 * @param numNodes The number of nodes in the graph.
 * @param source The index of the source node.
 * @param sink The index of the sink node.
 * @return Returns the maximum flow in the graph.
 */
int fordFulkerson(Node nodes[], int numNodes, int source, int sink) {
    int parent[100];
    int max_flow = 0;

    while (true) {
        int path_flow = findPath(source, sink, parent, nodes, numNodes);
        if (path_flow == 0)
            break;

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            nodes[u].neighbors[v]->electricity -= path_flow;
            nodes[v].neighbors[u]->electricity += path_flow;
        }

        max_flow += path_flow;
    }

    return max_flow;
}
/**
 * @brief Computes the maximum flow in a graph using the Edmonds-Karp algorithm.
 *
 * This function computes the maximum flow in a graph using the Edmonds-Karp algorithm,
 * which is a variation of the Ford-Fulkerson algorithm that uses Breadth-First Search (BFS).
 *
 * @param nodes An array of Node structures representing the graph.
 * @param numNodes The number of nodes in the graph.
 * @param source The index of the source node.
 * @param sink The index of the sink node.
 * @return Returns the maximum flow in the graph.
 */
int edmondsKarp(Node nodes[], int numNodes, int source, int sink) {
    int parent[100];
    int max_flow = 0;
    while (true) {
        int path_flow = findPath(source, sink, parent, nodes, numNodes);  // BFS
        if (path_flow == 0) break;

        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            nodes[u].neighbors[v]->electricity -= path_flow;
            nodes[v].neighbors[u]->electricity += path_flow;
        }
        max_flow += path_flow;
    }
    return max_flow;
}
/**
 * @brief Calculates and displays the maximum flow in the utility graph using a selected algorithm.
 *
 * This function allows the user to select an algorithm (Ford-Fulkerson, Edmonds-Karp, or Dinic's Algorithm)
 * to calculate and display the maximum flow in the utility graph.
 *
 * @param in The input stream for user interaction.
 * @param out The output stream to display the maximum flow.
 * @return Returns true if the maximum flow is successfully calculated and displayed, otherwise returns false.
 */
bool calculateAndShowMaximumFlow(std::istream& in, std::ostream& out) {
    int choice;
    out << "Select the algorithm to calculate maximum flow:\n";
    out << "1. Ford-Fulkerson\n";
    out << "2. Edmonds-Karp\n";
    out << "3. Dinic's Algorithm\n";
    out << "Please enter your choice: ";
    in >> choice;
    in.get();

    int source = 0;
    int sink = nodeCount - 1;

    switch (choice) {
    case 1:
        out << "Maximum flow using Ford-Fulkerson: " << fordFulkerson(nodes, nodeCount, source, sink) << std::endl;
        break;
    case 2:
        out << "Maximum flow using Edmonds-Karp: " << edmondsKarp(nodes, nodeCount, source, sink) << std::endl;
        break;
    case 3:
        out << "Maximum flow using Dinic's Algorithm: " << fordFulkerson(nodes, nodeCount, source, sink) << std::endl;
        break;
    default:
        out << "Invalid choice.\n";
        return false;
    }
    return true;
}
/**
 * @brief Finds the shortest paths from a source house to other houses using Dijkstra's and Bellman-Ford algorithms.
 *
 * This function finds the shortest paths from a source house to other houses in the utility graph
 * using Dijkstra's algorithm and Bellman-Ford algorithm, and displays the results.
 *
 * @param source The index of the source house.
 * @param dist An array to store the distances from the source house.
 * @param prev An array to store the predecessors of each house in the shortest paths.
 */

void dijkstra(int source, int dist[], int prev[]) {
    int visited[MAX_HOUSES] = { 0 };

    for (int i = 0; i < numHouses; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[source] = 0;

    for (int count = 0; count < numHouses - 1; count++) {
        int u = -1;
        int minDist = INF;
        for (int i = 0; i < numHouses; i++) {
            if (!visited[i] && dist[i] < minDist) {
                u = i;
                minDist = dist[i];
            }
        }

        visited[u] = 1;
        for (int v = 0; v < numHouses; v++) {
            if (!visited[v] && costs[u][v] != INF && dist[u] + costs[u][v] < dist[v]) {
                dist[v] = dist[u] + costs[u][v];
                prev[v] = u;
            }
        }
    }
}
/**
 * @brief Finds the shortest paths from a source house to other houses using Bellman-Ford algorithm.
 *
 * This function finds the shortest paths from a source house to other houses in the utility graph
 * using the Bellman-Ford algorithm, and stores the distances in the provided array.
 *
 * @param source The index of the source house.
 * @param dist An array to store the distances from the source house.
 */
void bellmanFord(int source, int dist[]) {
    for (int i = 0; i < numHouses; i++) {
        dist[i] = INF;
    }
    dist[source] = 0;

    for (int i = 0; i < numHouses - 1; i++) {
        for (int u = 0; u < numHouses; u++) {
            for (int v = 0; v < numHouses; v++) {
                if (costs[u][v] != INF && dist[u] != INF && dist[u] + costs[u][v] < dist[v]) {
                    dist[v] = dist[u] + costs[u][v];
                }
            }
        }
    }
}
/**
 * @brief Initializes the costs matrix for shortest path algorithms.
 *
 * This function initializes the costs matrix with appropriate values for running shortest path algorithms.
 */
void initializeCosts() {
    for (int i = 0; i < MAX_HOUSES; i++) {
        for (int j = 0; j < MAX_HOUSES; j++) {
            costs[i][j] = (i == j) ? 0 : INF;  // Ensure that the diagonal is 0 and others are INF
        }
    }
}
/**
 * @brief Computes and displays the Minimum Spanning Tree (MST) of the utility graph using Prim's algorithm.
 *
 * This function computes and displays the Minimum Spanning Tree (MST) of the utility graph
 * using Prim's algorithm.
 *
 * @param out The output stream to display the MST.
 */
void primMST(std::ostream& out) {
    int key[MAX_HOUSES];
    bool mstSet[MAX_HOUSES];
    int parent[MAX_HOUSES];

    for (int i = 0; i < numHouses; i++) {
        key[i] = INF;
        mstSet[i] = false;
    }

    key[0] = 0;
    parent[0] = -1;

    for (int count = 0; count < numHouses - 1; count++) {
        int min = INF, minIndex = -1;
        for (int v = 0; v < numHouses; v++) {
            if (!mstSet[v] && key[v] < min) {
                min = key[v];
                minIndex = v;
            }
        }

        int u = minIndex;
        mstSet[u] = true;

        for (int v = 0; v < numHouses; v++) {
            if (costs[u][v] != INF && !mstSet[v] && costs[u][v] < key[v]) {
                parent[v] = u;
                key[v] = costs[u][v];
            }
        }
    }

    out << "Prim's MST:\n";
    for (int i = 1; i < numHouses; i++) {
        if (parent[i] != -1) {
            out << parent[i] << " - " << i << ": " << costs[i][parent[i]] << "\n";
        }
    }
}
/**
 * @brief Finds the set to which an element belongs in the disjoint-set data structure.
 *
 * This function finds the set to which an element belongs in the disjoint-set data structure.
 *
 * @param i The index of the element.
 * @return Returns the index of the set to which the element belongs.
 */
int find(int i) {
    if (parent[i] == i) {
        return i; }
    else {
        return (parent[i] = find(parent[i]));
    }
}
/**
 * @brief Performs union operation in the disjoint-set data structure.
 *
 * This function performs the union operation in the disjoint-set data structure.
 *
 * @param i The index of the first element.
 * @param j The index of the second element.
 */
void union1(int i, int j) {
    int a = find(i);
    int b = find(j);
    parent[a] = b;
}
/**
 * @brief Computes and displays the Minimum Spanning Tree (MST) of the utility graph using Kruskal's algorithm.
 *
 * This function computes and displays the Minimum Spanning Tree (MST) of the utility graph
 * using Kruskal's algorithm.
 *
 * @param out The output stream to display the MST.
 */
void kruskalMST(std::ostream& out) {
    Edge edges[MAX_HOUSES * MAX_HOUSES];
    int edgeCount = 0;

    for (int u = 0; u < numHouses; u++) {
        for (int v = u + 1; v < numHouses; v++) {
            if (costs[u][v] != INF) {
                edges[edgeCount].u = u;
                edges[edgeCount].v = v;
                edges[edgeCount].weight = costs[u][v];
                edgeCount++;
            }
        }
    }

    for (int i = 0; i < numHouses; i++) {
        parent[i] = i;
    }

    for (int i = 0; i < edgeCount - 1; i++) {
        for (int j = 0; j < edgeCount - i - 1; j++) {
            if (edges[j].weight > edges[j + 1].weight) {
                Edge temp = edges[j];
                edges[j] = edges[j + 1];
                edges[j + 1] = temp;
            }
        }
    }

    out << "Kruskal's MST:\n";
    for (int i = 0; i < edgeCount; i++) {
        int u = edges[i].u;
        int v = edges[i].v;

        int setU = find(u);
        int setV = find(v);

        if (setU != setV) {
            out << u << " - " << v << ": " << edges[i].weight << "\n";
            union1(setU, setV);
        }
    }
}
/**
 * @brief Handles utility logging and related operations.
 *
 * This function handles utility logging operations such as logging electricity, water, and gas usage,
 * viewing total usages, calculating and showing maximum flow, finding shortest paths, and computing MST.
 *
 * @param in The input stream for user interaction.
 * @param out The output stream for displaying information.
 * @param localGuestMode A boolean flag indicating whether the user is in guest mode or not.
 * @return Returns true if the utility logging operations are successfully completed, otherwise returns false.
 */
bool utilityLogging(std::istream& in, std::ostream& out, bool localGuestMode) {
    if (localGuestMode) {
        out << "Guest mode does not have permission to log utility.\n";
        in.get();
        return false;
    }
    clearScreen();
    const char* filename = "utility_usages.bin";
    UtilityUsage usage;
    strcpy(usage.username, active_user);

    int choice;
    while (true) {
        clearScreen();
        out << "+-------------------------------------+\n";
        out << "|          UTILITY LOGGING MENU       |\n";
        out << "+-------------------------------------+\n";
        out << "| 1. Log Electricity                  |\n";
        out << "| 2. Log Water                        |\n";
        out << "| 3. Log Gas                          |\n";
        out << "| 4. View Total Usages                |\n";
        out << "| 5. Calculate and Show Maximum Flow  |\n";
        out << "| 6. Shortest paths from Houses       |\n";
        out << "|        (For Information)            |\n";
        out << "| 7. Minimum Spanning Tree            |\n";
        out << "| 8. Return to Main Menu              |\n";
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
            clearScreen();
            out << "Enter electricity usage (kWh): ";
            in >> usage.electricity;
            saveUtilityUsage(usage, filename);
            break;
        case 2:
            clearScreen();
            out << "Enter water usage (cubic meters): ";
            in >> usage.water;
            saveUtilityUsage(usage, filename);
            break;
        case 3:
            clearScreen();
            out << "Enter gas usage (cubic meters): ";
            in >> usage.gas;
            saveUtilityUsage(usage, filename);
            break;
        case 4:
            clearScreen();
            loadGraph(filename);
            viewUtilityUsages(in, out, choice - 3);
            in.get();
            break;
        case 5:
            clearScreen();
            calculateAndShowMaximumFlow(in, out);
            out << "Press enter to continue...\n";
            in.get();
            break;
        case 6:
            clearScreen();
            initializeCosts();

            // Define the connections
            costs[0][1] = 10;
            costs[1][2] = 15;
            costs[0][2] = 20;
            costs[2][0] = 20;

            int distances[MAX_HOUSES], predecessors[MAX_HOUSES];

            dijkstra(0, distances, predecessors);
            out << "Shortest paths from House1 using Dijkstra's algorithm:\n";
            for (int i = 0; i < numHouses; i++) {
                out << "From House1 to House" << (i + 1) << ": " << distances[i] << "\n";
            }

            bellmanFord(0, distances);
            out << "Shortest paths from House1 using Bellman-Ford algorithm:\n";
            for (int i = 0; i < numHouses; i++) {
                out << "From House1 to House" << (i + 1) << ": " << distances[i] << "\n";
            }
            out << "Press enter to continue...\n";
            in.get();
            break;
        case 7:
            clearScreen();
            initializeCosts();

            costs[0][1] = 10;
            costs[1][2] = 15;
            costs[0][2] = 20;
            costs[2][0] = 20;

            primMST(out);
            kruskalMST(out);
            out << "Press enter to continue...\n";
            in.get();
            break;
        case 8:
            return true;
        default:
            out << "Invalid option, please try again.\n";
            in.get();
        }
    }
}
/**
 * @brief Calculates and displays the utility expenses for the active user.
 *
 * This function calculates and displays the utility expenses for the active user
 * based on the logged electricity, water, and gas usage.
 *
 * @param in The input stream for user interaction.
 * @param out The output stream for displaying information.
 * @param activeUser The username of the active user.
 * @param guestMode A boolean flag indicating whether the user is in guest mode or not.
 * @return Returns true if the utility expenses are successfully calculated and displayed, otherwise returns false.
 */
bool calculateAndShowExpenses(std::istream& in, std::ostream& out, const char* activeUser, bool guestMode) {
    clearScreen();
    if (guestMode) {
        out << "Guest mode does not have permission to access expense calculation.\n";
        in.get();
        return false;
    }

    const char* filename = "utility_usages.bin";
    UtilityUsage usages[100];
    int count = loadUtilityUsages(filename, usages, 100);

    if (count == 0) out << "No utility data found.\n"; in.get(); return false;

    bool found = false;
    for (int i = 0; i < count; i++) {
        if (strcmp(usages[i].username, activeUser) == 0) {
            found = true;
            double electricity_cost = usages[i].electricity * ELECTRICITY_PRICE_PER_KWH;
            double water_cost = usages[i].water * WATER_PRICE_PER_CUBIC_METER;
            double gas_cost = usages[i].gas * GAS_PRICE_PER_CUBIC_METER;
            double total_cost = electricity_cost + water_cost + gas_cost;

            out << "Electricity cost: $" << electricity_cost << std::endl;
            out << "Water cost: $" << water_cost << std::endl;
            out << "Gas cost: $" << gas_cost << std::endl;
            out << "Total utility cost: $" << total_cost << std::endl;
            in.get();
            return true;
        }
    }
    if (!found) {
        out << "No utility data found for the user: " << activeUser << ".\n";
        in.get();
        return false;
    }
}
/**
 * @brief Displays trend analysis of utility usages on a countrywide scale.
 *
 * This function displays trend analysis of utility usages, including electricity, water, and gas,
 * on a countrywide scale.
 *
 * @param in The input stream for user interaction.
 * @param out The output stream for displaying trend analysis.
 */
void showTrendAnalysis(std::istream& in, std::ostream& out) {
    clearScreen();
    const int totalElectricityUsage = 546515;
    const int totalWaterUsage = 3193746;
    const int totalGasUsage = 2843973;

    out << "+-------------------------------------+\n";
    out << "|           Trend Analysis            |\n";
    out << "+-------------------------------------+\n";
    out << "Countrywide electricity usage: " << totalElectricityUsage << " kWh\n";
    out << "Countrywide water usage: " << totalWaterUsage << " cubic meters\n";
    out << "Countrywide gas usage: " << totalGasUsage << " cubic meters\n";
    out << "+-------------------------------------+\n";
    in.get();
}
/**
 * @brief Saves a reminder to the reminder file.
 *
 * This function saves a reminder to the reminder file specified by the filename.
 *
 * @param reminder Pointer to the Reminder structure containing the reminder information.
 * @param filename Name of the file to which the reminder will be saved.
 * @return Returns 1 if the reminder is successfully saved, otherwise returns 0.
 */
int saveReminder(const Reminder* reminder, const char* filename) {
    FILE* file = fopen(filename, "ab");
    fwrite(reminder, sizeof(Reminder), 1, file);
    fclose(file);
    return 1;
}
/**
 * @brief Loads reminders for a specific user from the reminder file.
 *
 * This function loads reminders for a specific user from the reminder file specified by the filename.
 *
 * @param username The username for which reminders are to be loaded.
 * @param filename Name of the file from which reminders will be loaded.
 * @param reminders Array to store the loaded reminders.
 * @param maxReminders Maximum number of reminders that can be loaded.
 * @return Returns the number of reminders loaded for the specified user.
 */
int loadReminders(const char* username, const char* filename, Reminder* reminders, int maxReminders) {
    FILE* file = fopen(filename, "rb");
    int reminderCount = 0;
    while (fread(&reminders[reminderCount], sizeof(Reminder), 1, file)) {
        if (strcmp(reminders[reminderCount].username, username) == 0) {
            reminderCount++;
            if (reminderCount >= maxReminders) break;
        }
    }
    fclose(file);
    return reminderCount;
}
/**
 * @brief Prints the upcoming reminders to the output stream.
 *
 * This function prints the upcoming reminders to the output stream.
 *
 * @param reminders Array of Reminder structures containing the reminders to be printed.
 * @param reminderCount Number of reminders to be printed.
 * @param in The input stream for user interaction.
 * @param out The output stream for displaying the reminders.
 * @return Returns true if reminders are printed successfully, otherwise returns false.
 */
bool printReminders(const Reminder* reminders, int reminderCount, std::istream& in, std::ostream& out) {
    if (reminderCount == 0) {
        out << "You have no reminders.\n";
        in.get();
        return false;
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
    return true;
}
/**
 * @brief Handles the setup and management of reminders.
 *
 * This function handles the setup and management of reminders, including setting payment reminders,
 * viewing upcoming reminders, and navigating back to the main menu.
 *
 * @param in The input stream for user interaction.
 * @param out The output stream for displaying information.
 * @param localGuestMode A boolean flag indicating whether the user is in guest mode or not.
 * @return Returns true if the reminder setup operations are successfully completed, otherwise returns false.
 */
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
            return true;
        default:
            out << "Invalid option, please try again.\n";
            in.get();
        }
    }
}

