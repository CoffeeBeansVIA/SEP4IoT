#include "pch.h"

class ProdTest : public ::testing::Test {
protected:
	void SetUp() override {}
	void TearDown() override {}
};

TEST_F(ProdTest, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}