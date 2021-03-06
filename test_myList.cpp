/*
Test cases to test the functionality of myList
*/

#include <ranges>
#include <algorithm>

#include "catch.hpp"
#include "myList.hpp"



TEST_CASE("List creation"){

    SECTION("Create list with one element"){
        auto head = myList::make_list(1);
        REQUIRE(head->value == 1 );
        REQUIRE(head->next  == nullptr );
    }

    SECTION("Create list with two elements"){
        auto head = myList::make_list(1, 2);

        auto currentNodePtr = head.get();
        REQUIRE(currentNodePtr->value == 1 );
        REQUIRE(currentNodePtr->next  != nullptr );

        currentNodePtr = currentNodePtr->next;
        REQUIRE(currentNodePtr->value == 2 );
        REQUIRE(currentNodePtr->next  == nullptr );
    }

    SECTION("Create list with a lot of elements"){
        auto head = myList::make_list(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

        auto currentNodePtr = head.get();
        int expectedValue = 1;
        while(currentNodePtr != nullptr){
            REQUIRE(currentNodePtr->value == expectedValue );
            currentNodePtr = currentNodePtr->next;
            ++expectedValue;
        }
    }
}


TEST_CASE("advance"){
    auto list = myList::make_list(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    auto head = list.get();

    SECTION("Advance zero steps"){
        auto sameAsHeadPtr = advance(head, 0);
        REQUIRE(sameAsHeadPtr == head);
    }

    SECTION("Advance one steps"){
        auto nextNodePtr = advance(head, 1);
        REQUIRE(nextNodePtr == head->next);
    }

    SECTION("Advance more steps"){
        auto steps   = GENERATE(range(1,15));
        auto nodePtr = advance(head, steps);
        REQUIRE(nodePtr->value == steps+1);
    }

    SECTION("Advance multiple times"){
        auto firstStep  = GENERATE(range(1,5));
        auto secondStep = GENERATE(range(1,5));
        auto nodePtr       = advance(head,    firstStep );
        auto secondNodePtr = advance(nodePtr, secondStep);
        REQUIRE(secondNodePtr->value == firstStep+secondStep+1);
    }

    SECTION("Advance beyond list"){
        auto nodePtr = advance(head, 16);
        REQUIRE(nodePtr == nullptr);

        auto anotherNodePtr = advance(head, 17);
        REQUIRE(anotherNodePtr == nullptr);
    }
}


TEST_CASE("reverse list"){
    SECTION("Reverse list with one element"){
        auto list = myList::make_list(1);
        auto head = list.get();

        auto lastElementPtr = myList::reverse_list(list);
        auto newHead = list.get();

        REQUIRE(newHead == head );
        REQUIRE(newHead == lastElementPtr );

        REQUIRE(newHead->value == 1 );
        REQUIRE(newHead->next  == nullptr );
    }

    SECTION("Reverse list with two elements"){
        auto list = myList::make_list(1, 2);
        auto head = list.get();

        auto lastElementPtr = myList::reverse_list(list);
        REQUIRE(head == lastElementPtr);

        auto currentNodePtr = list.get();
        REQUIRE(currentNodePtr->value == 2 );
        REQUIRE(currentNodePtr->next  != nullptr );

        currentNodePtr = currentNodePtr->next;
        REQUIRE(currentNodePtr->value == 1 );
        REQUIRE(currentNodePtr->next  == nullptr );

        REQUIRE(currentNodePtr == lastElementPtr);
    }

    SECTION("Reverse list with a lot of elements"){
        auto list = myList::make_list(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        auto head = list.get();

        auto lastElementPtr = myList::reverse_list(list);
        REQUIRE(head == lastElementPtr);

        auto currentNodePtr = list.get();
        int expectedValue = 16;
        while(currentNodePtr->next != nullptr){
            REQUIRE(currentNodePtr->value == expectedValue);
            currentNodePtr = currentNodePtr->next;
            --expectedValue;
        }
        REQUIRE(currentNodePtr->value == expectedValue);

        REQUIRE(currentNodePtr == lastElementPtr);
    }
}

TEST_CASE("split_after"){
    SECTION("Split after k-th element"){
        auto k = GENERATE(range(1,10));
        auto list = myList::make_list(1,2,3,4,5,6,7,8,9,10);
        INFO("Splitting after the " << k << "-th element.");
        auto listIt = myList::advance(list.get(), k-1);
        auto nextList = myList::split_after(listIt);

        auto testValues = [](auto listIt, auto expectedValue, auto maxElementValue){
            while(expectedValue < maxElementValue){
                CHECK(listIt->value == expectedValue);
                REQUIRE(listIt->next != nullptr);
                listIt = myList::advance(listIt, 1);
                ++expectedValue;
            }
            CHECK(listIt->value == expectedValue);
            REQUIRE(listIt->next == nullptr);
        };

        testValues(    list.get(), 1  , k );
        testValues(nextList.get(), k+1, 10);
    }

    SECTION("Split after last element"){
        auto list = myList::make_list(1,2,3);
        auto listIt = myList::advance(list.get(), 2);
        auto newList = myList::split_after(listIt);

        REQUIRE( newList.get() == nullptr );
        CHECK( listIt->value == 3 );
        REQUIRE( listIt->next == nullptr );
    }
}

auto groupsAreReversedAsExpected(auto& currentNodePtr, const auto size, const auto k){
    const auto numberOfGroups = size/k;
    auto groupIsReversed = [&currentNodePtr, k](const auto group){
        auto hasExpectedValue = [&currentNodePtr, k, group](const auto nodeInGroup){
            const auto expectedValue = (group+1)*k-nodeInGroup;
            const auto currentValue  = currentNodePtr->value;
            currentNodePtr = currentNodePtr->next;
            return expectedValue == currentValue;
        };
        auto nodesInGroup = std::views::iota(0, k);
        return std::ranges::all_of(nodesInGroup, hasExpectedValue);
    };
    auto groups = std::views::iota(0, numberOfGroups);
    return std::ranges::all_of(groups, groupIsReversed);
}

auto remainingElementsStayUnmodified(auto& currentNodePtr, const auto size, const auto k){
    const auto numberOfGroups = size/k;
    auto hasExpectedValue = [&currentNodePtr](const auto expectedValue){
        const auto currentValue = currentNodePtr->value;
        currentNodePtr = currentNodePtr->next;
        return expectedValue == currentValue;
    };
    auto remainingElements = std::views::iota( std::min(numberOfGroups*k+1, size), size );
    return std::ranges::all_of(remainingElements, hasExpectedValue);
}

auto valuesAreReversedAsExpected(myList::List<int>& reversedList, const int size, int k){
    if( k==0 ){
        k=1;
    }
    auto currentNodePtr = reversedList.get();
    const auto groupsReversed         = groupsAreReversedAsExpected    (currentNodePtr, size, k);
    const auto lastElementsUnmodified = remainingElementsStayUnmodified(currentNodePtr, size, k);

    return groupsReversed && lastElementsUnmodified;
}

template<typename... ListValues>
auto test_reverse_group(unsigned int k, ListValues&&... values){
    auto list = myList::make_list(std::forward<ListValues>(values)...);

    std::cout << "Testing reverse_groups with k = " << k << '\n';
    std::cout << "Before: ";
    myList::print(list);

    myList::reverse_groups(list, k);
    std::cout << "After:  ";
    myList::print(list);
    std::cout << '\n';

    const auto size = sizeof...(ListValues);
    const auto reverse_group_successful = valuesAreReversedAsExpected(list, size, k);

    return reverse_group_successful;
}

TEST_CASE("Reverse_group"){
    SECTION("Reverse_group with one element"){
        REQUIRE( test_reverse_group(0, 1) );
        REQUIRE( test_reverse_group(1, 1) );
    }

    SECTION("Reverse_group with two elements"){
        REQUIRE( test_reverse_group(0, 1, 2) );
        REQUIRE( test_reverse_group(1, 1, 2) );
        REQUIRE( test_reverse_group(2, 1, 2) );
    }

    SECTION("Reverse_group with nine elements"){
        auto k = GENERATE(range(0,10));
        REQUIRE( test_reverse_group(k, 1, 2, 3, 4, 5, 6, 7, 8, 9) );
    }

    SECTION("Reverse_group with a lot of elements"){
        auto k = GENERATE(range(0,17));
        REQUIRE( test_reverse_group(k, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16) );
    }
}