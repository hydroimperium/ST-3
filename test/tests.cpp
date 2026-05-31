// Copyright 2026 Klimov M.D.
#include "TimedDoor.h"
#include <chrono>
#include <stdexcept>
#include <thread>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    void SetUp() override {
        door = new TimedDoor(0);
    }

    void TearDown() override {
        delete door;
    }

    TimedDoor* door;
};

TEST_F(TimedDoorTest, InitialStateClosed) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeOut) {
    EXPECT_EQ(door->getTimeOut(), 0);
}

TEST_F(TimedDoorTest, ThrowStateThrowsException) {
    EXPECT_THROW(door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, UnlockThrowsExceptionWhenTimeoutZero) {
    EXPECT_THROW(door->unlock(), std::runtime_error);
}

TEST(TimerTest, TimerCallsTimeout) {
    MockTimerClient client;
    EXPECT_CALL(client, Timeout()).Times(1);
    Timer t;
    t.tregister(0, &client);
}

TEST(TimerTest, TimerSleeps) {
    using std::chrono::steady_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    auto start = steady_clock::now();
    Timer t;
    MockTimerClient client;
    EXPECT_CALL(client, Timeout()).Times(1);
    t.tregister(50, &client);
    auto end = steady_clock::now();
    auto elapsed = duration_cast<milliseconds>(end - start).count();
    EXPECT_GE(elapsed, 40);
}

TEST(MockDoorTest, MockDoorLock) {
    MockDoor door;
    EXPECT_CALL(door, lock()).Times(1);
    door.lock();
}

TEST(MockDoorTest, MockDoorUnlock) {
    MockDoor door;
    EXPECT_CALL(door, unlock()).Times(1);
    door.unlock();
}

TEST(MockDoorTest, MockDoorIsOpened) {
    MockDoor door;
    EXPECT_CALL(door, isDoorOpened()).WillOnce(::testing::Return(true));
    EXPECT_TRUE(door.isDoorOpened());
}
