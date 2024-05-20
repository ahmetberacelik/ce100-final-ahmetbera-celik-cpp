#include "../header/hometracker.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <queue>
#define MAX_HOUSES 100
#define INF INT_MAX

int costs[MAX_HOUSES][MAX_HOUSES];
int numHouses = 3;
Node nodes[100];
int nodeCount = 0;
bool guestMode = false; /**< Boolean variable to indicate whether the program is in guest mode (false by default). */
char active_user[50];
int parent[MAX_HOUSES];

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
        }
    }
    if (!found) {
        usages[count++] = usage;
    }

    FILE* file = fopen(filename, "wb");
    fwrite(usages, sizeof(UtilityUsage), count, file);
    fclose(file);
    return true;
}

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
        }
    }
    return true;
}



bool viewUtilityUsages(std::istream& in, std::ostream& out, int searchType) {
    if (nodeCount == 0) {
        out << "No utility data available for the current user.\n";
        return false;
    }

    out << "Select search method:\n1. BFS\n2. DFS\n";
    int choice;
    std::cin >> choice;

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

void initializeCosts() {
    for (int i = 0; i < MAX_HOUSES; i++) {
        for (int j = 0; j < MAX_HOUSES; j++) {
            costs[i][j] = (i == j) ? 0 : INF;  // Ensure that the diagonal is 0 and others are INF
        }
    }
}


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

int find(int i) {
    if (parent[i] == i) {
        return i;
    }
    else {
        return (parent[i] = find(parent[i]));
    }
}

void union1(int i, int j) {
    int a = find(i);
    int b = find(j);
    parent[a] = b;
}

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
    return true;
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

