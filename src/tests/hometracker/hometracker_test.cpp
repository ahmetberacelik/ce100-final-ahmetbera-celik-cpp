//#define ENABLE_HOMETRACKER_TEST  // Uncomment this line to enable the Hometracker tests

#include "gtest/gtest.h"
#include "../../hometracker/header/hometracker.h"  // Adjust this include path based on your project structure

using namespace Coruh::Hometracker;

class HometrackerTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Setup test data
	}

	void TearDown() override {
		// Clean up test data
	}
};

TEST_F(HometrackerTest, TestAdd) {
	double result = Hometracker::add(5.0, 3.0);
	EXPECT_DOUBLE_EQ(result, 8.0);
}

TEST_F(HometrackerTest, TestSubtract) {
	double result = Hometracker::subtract(5.0, 3.0);
	EXPECT_DOUBLE_EQ(result, 2.0);
}

TEST_F(HometrackerTest, TestMultiply) {
	double result = Hometracker::multiply(5.0, 3.0);
	EXPECT_DOUBLE_EQ(result, 15.0);
}

TEST_F(HometrackerTest, TestDivide) {
	double result = Hometracker::divide(6.0, 3.0);
	EXPECT_DOUBLE_EQ(result, 2.0);
}

TEST_F(HometrackerTest, TestDivideByZero) {
	EXPECT_THROW(Hometracker::divide(5.0, 0.0), std::invalid_argument);
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