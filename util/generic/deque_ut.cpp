#include "deque.h"

#include <library/unittest/registar.h>

#include <util/generic/yexception.h>

class TDequeTest: public TTestBase {
        UNIT_TEST_SUITE(TDequeTest);
            UNIT_TEST(TestDeque1);
            UNIT_TEST(TestAt);
            UNIT_TEST(TestInsert);
            UNIT_TEST(TestErase);
            UNIT_TEST(TestAutoRef);
        UNIT_TEST_SUITE_END();

    protected:
        void TestDeque1();
        void TestInsert();
        void TestErase();
        void TestAt();
        void TestAutoRef();
};

UNIT_TEST_SUITE_REGISTRATION(TDequeTest);

void TDequeTest::TestDeque1() {
    ydeque<int> d;
    d.push_back(4);
    d.push_back(9);
    d.push_back(16);
    d.push_front(1);

    UNIT_ASSERT(d[0] == 1);
    UNIT_ASSERT(d[1] == 4);
    UNIT_ASSERT(d[2] == 9);
    UNIT_ASSERT(d[3] == 16);

    d.pop_front();
    d[2] = 25;

    UNIT_ASSERT(d[0] == 4);
    UNIT_ASSERT(d[1] == 9);
    UNIT_ASSERT(d[2] == 25);

    //Some compile time tests:
    ydeque<int>::iterator dit = d.begin();
    ydeque<int>::const_iterator cdit(d.begin());

    UNIT_ASSERT((dit - cdit) == 0);
    UNIT_ASSERT((cdit - dit) == 0);
    UNIT_ASSERT((dit - dit) == 0);
    UNIT_ASSERT((cdit - cdit) == 0);
    UNIT_ASSERT(!((dit < cdit) || (dit > cdit) || (dit != cdit) || !(dit <= cdit) || !(dit >= cdit)));
}

void TDequeTest::TestInsert() {
    ydeque<int> d;
    d.push_back(0);
    d.push_back(1);
    d.push_back(2);

    UNIT_ASSERT(d.size() == 3);

    ydeque<int>::iterator dit;

    //Insertion before begin:
    dit = d.insert(d.begin(), 3);
    UNIT_ASSERT(dit != d.end());
    UNIT_ASSERT(*dit == 3);
    UNIT_ASSERT(d.size() == 4);
    UNIT_ASSERT(d[0] == 3);

    //Insertion after begin:
    dit = d.insert(d.begin() + 1, 4);
    UNIT_ASSERT(dit != d.end());
    UNIT_ASSERT(*dit == 4);
    UNIT_ASSERT(d.size() == 5);
    UNIT_ASSERT(d[1] == 4);

    //Insertion at end:
    dit = d.insert(d.end(), 5);
    UNIT_ASSERT(dit != d.end());
    UNIT_ASSERT(*dit == 5);
    UNIT_ASSERT(d.size() == 6);
    UNIT_ASSERT(d[5] == 5);

    //Insertion before last element:
    dit = d.insert(d.end() - 1, 6);
    UNIT_ASSERT(dit != d.end());
    UNIT_ASSERT(*dit == 6);
    UNIT_ASSERT(d.size() == 7);
    UNIT_ASSERT(d[5] == 6);

    //Insertion of several elements before begin
    d.insert(d.begin(), 2, 7);
    UNIT_ASSERT(d.size() == 9);
    UNIT_ASSERT(d[0] == 7);
    UNIT_ASSERT(d[1] == 7);

    //Insertion of several elements after begin
    //There is more elements to insert than elements before insertion position
    d.insert(d.begin() + 1, 2, 8);
    UNIT_ASSERT(d.size() == 11);
    UNIT_ASSERT(d[1] == 8);
    UNIT_ASSERT(d[2] == 8);

    //There is less elements to insert than elements before insertion position
    d.insert(d.begin() + 3, 2, 9);
    UNIT_ASSERT(d.size() == 13);
    UNIT_ASSERT(d[3] == 9);
    UNIT_ASSERT(d[4] == 9);

    //Insertion of several elements at end:
    d.insert(d.end(), 2, 10);
    UNIT_ASSERT(d.size() == 15);
    UNIT_ASSERT(d[14] == 10);
    UNIT_ASSERT(d[13] == 10);

    //Insertion of several elements before last:
    //There is more elements to insert than elements after insertion position
    d.insert(d.end() - 1, 2, 11);
    UNIT_ASSERT(d.size() == 17);
    UNIT_ASSERT(d[15] == 11);
    UNIT_ASSERT(d[14] == 11);

    //There is less elements to insert than elements after insertion position
    d.insert(d.end() - 3, 2, 12);
    UNIT_ASSERT(d.size() == 19);
    UNIT_ASSERT(d[15] == 12);
    UNIT_ASSERT(d[14] == 12);
}

void TDequeTest::TestAt() {
    ydeque<int> d;
    ydeque<int> const& cd = d;

    d.push_back(10);
    UNIT_ASSERT(d.at(0) == 10);
    d.at(0) = 20;
    UNIT_ASSERT(cd.at(0) == 20);

    for (;;) {
        try {
            d.at(1) = 20;
            UNIT_ASSERT(false);
        } catch (...) {
            return;
        }
    }
}

void TDequeTest::TestAutoRef() {
    int i;
    ydeque<int> ref;
    for (i = 0; i < 5; ++i) {
        ref.push_back(i);
    }

    ydeque<ydeque<int> > d_d_int(1, ref);
    d_d_int.push_back(d_d_int[0]);
    d_d_int.push_back(ref);
    d_d_int.push_back(d_d_int[0]);
    d_d_int.push_back(d_d_int[0]);
    d_d_int.push_back(ref);

    for (i = 0; i < 5; ++i) {
        UNIT_ASSERT(d_d_int[i] == ref);
    }
}

void TDequeTest::TestErase() {
    ydeque<int> dint;
    dint.push_back(3);
    dint.push_front(2);
    dint.push_back(4);
    dint.push_front(1);
    dint.push_back(5);
    dint.push_front(0);
    dint.push_back(6);

    ydeque<int>::iterator it(dint.begin() + 1);
    UNIT_ASSERT(*it == 1);

    dint.erase(dint.begin());
    UNIT_ASSERT(*it == 1);

    it = dint.end() - 2;
    UNIT_ASSERT(*it == 5);

    dint.erase(dint.end() - 1);
    UNIT_ASSERT(*it == 5);

    dint.push_back(6);
    dint.push_front(0);

    it = dint.begin() + 2;
    UNIT_ASSERT(*it == 2);

    dint.erase(dint.begin(), dint.begin() + 2);
    UNIT_ASSERT(*it == 2);

    it = dint.end() - 3;
    UNIT_ASSERT(*it == 4);

    dint.erase(dint.end() - 2, dint.end());
    UNIT_ASSERT(*it == 4);
}