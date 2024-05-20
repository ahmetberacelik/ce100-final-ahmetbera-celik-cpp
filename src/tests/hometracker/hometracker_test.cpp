//#define ENABLE_HOMETRACKER_TEST  // Uncomment this line to enable the Hometracker tests

#include "gtest/gtest.h"
#include "../../hometracker/header/hometracker.h"  // Adjust this include path based on your project structure

class HometrackerTest : public ::testing::Test {
protected:
	std::ostringstream out;
	std::istringstream in;
	void SetUp() override {
		out.str("");
		out.clear();
	}

	void TearDown() override {
		// Clean up test data
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