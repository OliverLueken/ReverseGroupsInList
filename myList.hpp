#ifndef MYLIST_HPP
#define MYLIST_HPP

#include <algorithm>
#include <iostream>

namespace myList{
    template<typename ValueType>
    struct Node{
        ValueType value{};
        Node<ValueType>* next{nullptr};
    };

    /*
    Expects the head of a list and destroys every node in that list
    */
    template<typename ValueType>
    void delete_list(Node<ValueType>* head){
        auto currentNodePtr = head;
        while(currentNodePtr != nullptr){
            auto nextNodePtr = currentNodePtr->next;
            delete currentNodePtr;
            currentNodePtr = nextNodePtr;
        }
    }

    template<typename ValueType>
    using List = std::unique_ptr<
        Node<ValueType>,
        decltype(&delete_list<ValueType>)
    >;

    /*
    Creates a list with one element
    Returns the list with one Node containing the passed in value
    */
    template<typename ValueType>
    List<ValueType> make_list(ValueType&& value){
        return List<ValueType>{
            new Node<ValueType>{std::forward<ValueType>(value)},
            &delete_list<ValueType>
        };
    }

    /*
    Creates a list consisting of the elements passed in
    Returns the list
    */
    template<typename ValueType, typename... ValueTypes>
    List<ValueType> make_list(ValueType&& value, ValueTypes&&... values){
        auto head  = make_list(std::forward<ValueType>(value));
        head->next = make_list(std::forward<ValueTypes>(values)...).release();
        return head;
    }

    /*
    Prints the content of the list
    */
    template<typename ValueType>
    void print(const List<ValueType>& head){
        auto currentNodePtr = head.get();
        while(currentNodePtr != nullptr){
            std::cout << currentNodePtr->value << ' ';
            currentNodePtr = currentNodePtr->next;
        }
        std::cout << '\n';
    }

    /*
    Receives a pointer nodePtr to a list and a positive integer n and
    returns the pointer to the nth next element in the list
    returns nullptr if list contains less than n+1 elements
    */
    template<typename ValueType>
    Node<ValueType>* advance(Node<ValueType>* nodePtr, unsigned int n){
        while(n>0 && nodePtr != nullptr){
            nodePtr = nodePtr->next;
            --n;
        }
        return nodePtr;
    }

    /*
    Reverses the elements inside the list
    Returns a pointer to the last element of the reversed list
    Iterators to elements of the list stay valid
    */
    template<typename ValueType>
    Node<ValueType>* reverse_list(List<ValueType>& list){
        auto lastNodePtr = list.release();
        const auto lastElementPtr = lastNodePtr;
        auto currentNodePtr = lastNodePtr->next;

        lastNodePtr->next = nullptr;
        while(currentNodePtr != nullptr){
            auto nextNodePtr = currentNodePtr->next;
            currentNodePtr->next = lastNodePtr;

            lastNodePtr = currentNodePtr;
            currentNodePtr = nextNodePtr;
        }
        list = List<ValueType>{
            lastNodePtr,
            &delete_list
        };
        return lastElementPtr;
    }

    /*
    Removes the nodes following nodePtr from the list
    Returns a list beginning with the node that followed nodePtr
    */
    template<typename ValueType>
    List<ValueType> split_after(Node<ValueType>* nodePtr){
        if(nodePtr == nullptr){
            return List<ValueType>{
                nullptr,
                &delete_list
            };
        }
        auto newListHead = List<ValueType>{
            nodePtr->next,
            &delete_list
        };
        nodePtr->next = nullptr;
        return newListHead;
    }

    /*
    Merges two lists
    The second list will be empty afterwards
    */
    template<typename ValueType>
    void merge_lists(List<ValueType>& firstList, List<ValueType>& secondList){
        auto lastNodePtr = firstList.get();
        if( lastNodePtr == nullptr ){
            std::swap(firstList, secondList);
            return;
        }
        while(lastNodePtr->next != nullptr){
            lastNodePtr = lastNodePtr->next;
        }
        lastNodePtr->next = secondList.release();
    }


    /*
    Modifies a list such that each group of size k nodes are reversed.
    Nodes at the end of the list that do not fill a whole group of size k are not reversed.
    */
    template<typename ValueType>
    void reverse_groups(List<ValueType>& list, const unsigned int k){
        if( k<=1 ) return;

        auto head = list.get();
        auto currentGroupTail = advance(head, k-1);
        if(currentGroupTail == nullptr) return; //k is greater than size of list

        auto nextGroupHead = split_after(currentGroupTail);

        //reverse group
        currentGroupTail = reverse_list(list);

        //insert group
        merge_lists(list, nextGroupHead);
        auto previousGroupTail = currentGroupTail;
        currentGroupTail = advance(currentGroupTail, k);

        while(currentGroupTail != nullptr){
            nextGroupHead = split_after(currentGroupTail);
            auto currentGroupList = split_after(previousGroupTail);
            currentGroupTail = reverse_list(currentGroupList);
            merge_lists(currentGroupList, nextGroupHead);
            merge_lists(list, currentGroupList);
            previousGroupTail = currentGroupTail;
            currentGroupTail = advance(currentGroupTail, k);
        }
    }
}

#endif