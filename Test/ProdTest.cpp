#include "gtest/gtest.h"
#include "fff.h"
#include "FreeRTOS_FFF_MocksDeclaration.h"

DEFINE_FFF_GLOBALS

extern "C" {
#include <stdio.h>
#include "../FreeRTOS/src/task.h"

// Header file from the production code project
#include <upLinkHandler.h>
}

class ProdTest : public ::testing::Test {
protected:
	void SetUp() override {}
	void TearDown() override {}
};

TEST_F(ProdTest, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}