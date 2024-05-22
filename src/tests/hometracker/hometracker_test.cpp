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

TEST_F(HometrackerTest, ExitUserAuthenticationTest) {

	simulateUserInput("4\n");

	bool authResult = userAuthentication(in, out);

	EXPECT_FALSE(authResult);
}

TEST_F(HometrackerTest, UserAuthenticationInvalidInputTest) {
	simulateUserInput("invalid\n\n4\n");


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