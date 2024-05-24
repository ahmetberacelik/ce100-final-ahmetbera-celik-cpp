//#define ENABLE_HOMETRACKER_TEST  // Uncomment this line to enable the Hometracker tests

#include "gtest/gtest.h"
#include "../../hometracker/header/hometracker.h"  // Adjust this include path based on your project structure
#include <fstream>
class HometrackerTest : public ::testing::Test {
protected:
	std::ostringstream out;
	std::istringstream in;
	void SetUp() override {
        out.str("");
        out.clear();
	}

	void TearDown() override {
		remove("test_utility_usages.bin");
	}
	void simulateUserInput(const std::string& input) {
		in.str(input);
		in.clear();
	}
};

TEST_F(HometrackerTest, LoginGuestModeTest) {

	simulateUserInput("3\n");

	bool authResult = userAuthentication(in, out);

	EXPECT_TRUE(authResult);
}

TEST_F(HometrackerTest, UserAuthenticationTestCorrectLogin) {
    simulateUserInput("1\nEnes Koy\n123456\n\n");


    bool authResult = userAuthentication(in, out);

    EXPECT_TRUE(authResult);
}

TEST_F(HometrackerTest, UserAuthenticationTestIncorrectLogin) {
    simulateUserInput("1\nInvalid User\n123456\n\n1\nInvalid User\n123456\n\n1\nInvalid User\n123456\n\n");


    bool authResult = userAuthentication(in, out);

    EXPECT_FALSE(authResult);
}

TEST_F(HometrackerTest, UserAuthenticationRegisterTest) {
    simulateUserInput("2\nTestUser\n123456\n\n");


    bool authResult = userAuthentication(in, out);

    EXPECT_TRUE(authResult);
}

TEST_F(HometrackerTest, ExitUserAuthenticationTest) {

	simulateUserInput("4\n");

	bool authResult = userAuthentication(in, out);

	EXPECT_FALSE(authResult);
}

TEST_F(HometrackerTest, UserAuthenticationInvalidInputTest) {
	simulateUserInput("invalid\n\n454\n\n4\n");


	bool authResult = userAuthentication(in, out);

	EXPECT_FALSE(authResult);
}

TEST_F(HometrackerTest, TestSaveUtilityUsage) {
    UtilityUsage usage = { "test_user", 150.0, 120.0, 80.0 };
    EXPECT_TRUE(saveUtilityUsage(usage, "test_utility_usages.bin"));

    // Read back to verify
    UtilityUsage usages[100];
    int count = loadUtilityUsages("test_utility_usages.bin", usages, 100);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(strcmp(usages[0].username, "test_user"), 0);
    EXPECT_EQ(usages[0].electricity, 150.0);
    EXPECT_EQ(usages[0].water, 120.0);
    EXPECT_EQ(usages[0].gas, 80.0);
}

TEST_F(HometrackerTest, TestLoadUtilityUsages) {
    UtilityUsage usage = { "test_user", 150.0, 120.0, 80.0 };
    saveUtilityUsage(usage, "test_utility_usages.bin");
    UtilityUsage usages[100];
    int count = loadUtilityUsages("test_utility_usages.bin", usages, 100);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(strcmp(usages[0].username, "test_user"), 0);
}

TEST_F(HometrackerTest, TestBFS) {
    std::ostringstream out;
    Node startNode("test_user");
    Node neighbor1("neighbor1");
    Node neighbor2("neighbor2");

    startNode.neighbors[0] = &neighbor1;
    startNode.neighbors[1] = &neighbor2;
    startNode.neighborCount = 2;

    BFS(&startNode, out);
    std::string expected_output = "Username: test_user, Electricity: 0, Water: 0, Gas: 0\nUsername: neighbor1, Electricity: 0, Water: 0, Gas: 0\n";
    EXPECT_EQ(out.str(), expected_output);
}

TEST_F(HometrackerTest, TestDFS) {
    std::ostringstream out;
    Node startNode("test_user");
    Node neighbor1("neighbor1");
    Node neighbor2("neighbor2");

    startNode.neighbors[0] = &neighbor1;
    startNode.neighbors[1] = &neighbor2;
    startNode.neighborCount = 2;

    DFS(&startNode, out);
    std::string expected_output = "Username: test_user, Electricity: 0, Water: 0, Gas: 0\nUsername: neighbor2, Electricity: 0, Water: 0, Gas: 0\n";
    EXPECT_EQ(out.str(), expected_output);
}

TEST_F(HometrackerTest, LCS_EmptyStrings) {
    char X[] = "";
    char Y[] = "";

    int result = lcs(X, Y, 0, 0);

    EXPECT_EQ(result, 0);
}

TEST_F(HometrackerTest, LCS_NormalUse) {
    char X[] = "AGGTAB";
    char Y[] = "GXTXAYB";
    int m = strlen(X);
    int n = strlen(Y);

    int result = lcs(X, Y, m, n);

    EXPECT_EQ(result, 4); // Expected LCS length is 4 ("GTAB")
}

TEST_F(HometrackerTest, ViewUtilityUsages_NoNodes) {
    int nodeCount = 0;
    simulateUserInput("1\n");  // Search type input is irrelevant here as nodeCount is 0

    bool result = viewUtilityUsages(in, out, 1);

    EXPECT_FALSE(result);
    EXPECT_EQ(out.str(), "No utility data available for the current user.\n");
}



TEST_F(HometrackerTest, SaveUtilityUsage_NewUser) {
    UtilityUsage usage = { "new_user", 100.0, 50.0, 30.0 };
    EXPECT_TRUE(saveUtilityUsage(usage, "test_utility_usages.bin"));

    UtilityUsage usages[100];
    int count = loadUtilityUsages("test_utility_usages.bin", usages, 100);
    EXPECT_EQ(count, 1);
    EXPECT_STREQ(usages[0].username, "new_user");
    EXPECT_EQ(usages[0].electricity, 100.0);
    EXPECT_EQ(usages[0].water, 50.0);
    EXPECT_EQ(usages[0].gas, 30.0);
}

TEST_F(HometrackerTest, SaveUtilityUsage_UpdateExistingUser) {
    UtilityUsage usage = { "existing_user", 200.0, 100.0, 60.0 };
    saveUtilityUsage(usage, "test_utility_usages.bin");  // First save

    // Update the usage
    usage.electricity = 300.0;
    usage.water = 150.0;
    usage.gas = 90.0;
    EXPECT_TRUE(saveUtilityUsage(usage, "test_utility_usages.bin"));

    UtilityUsage usages[100];
    int count = loadUtilityUsages("test_utility_usages.bin", usages, 100);
    EXPECT_EQ(count, 1);
    EXPECT_STREQ(usages[0].username, "existing_user");
    EXPECT_EQ(usages[0].electricity, 300.0);
    EXPECT_EQ(usages[0].water, 150.0);
    EXPECT_EQ(usages[0].gas, 90.0);
}

TEST_F(HometrackerTest, LoadUtilityUsages_EmptyFile) {
    UtilityUsage usages[100];
    int count = loadUtilityUsages("empty_test_utility_usages.bin", usages, 100);
    EXPECT_EQ(count, 0);
}

TEST_F(HometrackerTest, UtilityLogging_GuestMode) {
    bool localGuestMode = true;
    simulateUserInput("1\n");

    bool result = utilityLogging(in, out, localGuestMode);

    EXPECT_FALSE(result);
    EXPECT_NE(out.str().find("Guest mode does not have permission to log utility."), std::string::npos);
}

TEST_F(HometrackerTest, UtilityLoggingTest) {
    bool localGuestMode = false;
    simulateUserInput("1\n100\n2\n100\n3\n100\n4\n1\n\n4\n2\n\n4\n4545\n\n5\n1\n\n5\n2\n\n5\n515515\n\n5\n3\n\n6\n\n7\n\n8\n");

    bool result = utilityLogging(in, out, localGuestMode);

    EXPECT_TRUE(result);
}

TEST_F(HometrackerTest, UtilityLoggingInvalidTest) {
    bool localGuestMode = false;
    simulateUserInput("invalid\n\n454545\n\n8\n");

    bool result = utilityLogging(in, out, localGuestMode);

    EXPECT_TRUE(result);
}

TEST_F(HometrackerTest, CalculateAndShowExpenses_GuestMode) {
    bool guestMode = true;

    bool result = calculateAndShowExpenses(in, out, "guest_user", guestMode);

    EXPECT_FALSE(result);
    EXPECT_NE(out.str().find("Guest mode does not have permission to access expense calculation."), std::string::npos);
}

TEST_F(HometrackerTest, CalculateAndShowExpenses) {
    bool guestMode = false;

    bool result = calculateAndShowExpenses(in, out, "test_user", guestMode);

    EXPECT_FALSE(result);
}

TEST_F(HometrackerTest, MainMenuTestReturnFalse) {
    bool authenticationResult = false;

    bool result = mainMenu(authenticationResult, in, out);

    EXPECT_FALSE(result);
}

TEST_F(HometrackerTest, MainMenuTest) {
    bool authenticationResult = true;

    simulateUserInput("1\n8\n2\n\n3\n\n4\n3\n5\n\n");

    bool result = mainMenu(authenticationResult, in, out);

    EXPECT_TRUE(result);
}

TEST_F(HometrackerTest, MainMenuInvalidTest) {
    bool authenticationResult = true;

    simulateUserInput("invalid\n\n454545\n\n5\n\n");

    bool result = mainMenu(authenticationResult, in, out);

    EXPECT_TRUE(result);
}

TEST_F(HometrackerTest, ReminderSetupInvalidTest) {
    bool guestMode = false;

    simulateUserInput("invalid\n\n454545\n\n3\n");

    bool result = ReminderSetup(in, out, guestMode);

    EXPECT_TRUE(result);
}

TEST_F(HometrackerTest, ReminderSetupTest) {
    bool guestMode = false;

    simulateUserInput("1\ntest reminder\n10\n\n2\n\n3\n");

    bool result = ReminderSetup(in, out, guestMode);

    EXPECT_TRUE(result);
}

TEST_F(HometrackerTest, ReminderSetupTestInvalidInput) {
    bool guestMode = false;

    simulateUserInput("1\ntest reminder\nday\n\n3\n");

    bool result = ReminderSetup(in, out, guestMode);

    EXPECT_TRUE(result);
}


/**
 * @brief The main function of the test program.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 * @return int The exit status of the program.
 */
int main(int argc, char** argv) {
#ifdef ENABLE_HOMETRACKER_TEST
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
#else
	return 0;
#endif
}