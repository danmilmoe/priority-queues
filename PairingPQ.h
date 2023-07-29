// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E


#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure
            //       to initialize it here.
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
            {}

            // Description: Allows access to the element at that Node's
            //              position. There are two versions, getElt() and a
            //              dereference operator, use whichever one seems
            //              more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data
            // members of this Node class from within the PairingPQ class.
            // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
            // function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *parent;
    }; // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        rootptr = nullptr;
        numNodes = 0;
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        rootptr = nullptr;
        numNodes = 0;
        while (start != end) {
            push(*start);
            start++;
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare } {
        rootptr = nullptr;
        numNodes = 0;
        std::deque<Node*> dq;
        Node* current = other.rootptr;
        dq.push_back(current);
        while (!dq.empty()) {
            current = dq.front();
            dq.pop_front();
            if (current->child != nullptr) {
                dq.push_back(current->child);
            }
            if (current->sibling != nullptr) {
                dq.push_back(current->sibling);
            }
            push(current->elt);
        }


    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        PairingPQ temp(rhs);

        std::swap(this->numNodes, temp.numNodes);
        std::swap(this->rootptr, temp.rootptr);

        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        std::deque<Node*> dq;
        if (rootptr == nullptr) {
            return;
        }
        Node* current = rootptr;
        dq.push_back(current);
        while (!dq.empty()) {
            current = dq.front();
            dq.pop_front();
            if (current->child != nullptr) {
                dq.push_back(current->child);
            }
            if (current->sibling != nullptr) {
                dq.push_back(current->sibling);
            }
            delete current;
        }

    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant. You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        std::deque<Node*> dq;
        //how to get root to point at null and current to point at what root used to point at
        Node* current = rootptr;
        dq.push_back(current);
        while (!dq.empty()) {
            current = dq.front();
            dq.pop_front();
            if (current->child != nullptr) {
                dq.push_back(current->child);
            }
            if (current->sibling != nullptr) {
                dq.push_back(current->sibling);
            }
            current->sibling = nullptr;
            current->child = nullptr;
            current->parent = nullptr;
            if (current == rootptr) {
                continue;
            }
            Node* meldReturn = meld(rootptr, current);
            rootptr = meldReturn;
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely in
    //              the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    //       an element when the pairing heap is empty. Though you are
    //       welcome to if you are familiar with them, you do not need to use
    //       exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        Node* p = rootptr->child;
        delete rootptr;
        std::deque<Node*> dq;
        if (p == nullptr) {
            rootptr = nullptr;
        }
        else if (p->sibling == nullptr) {
            rootptr = p;
        }
        else {
            //pushing values into deque
            Node* current = p;
            while (current != nullptr) {
                dq.push_back(current);
                current = current->sibling;
            }
            Node* current1;
            Node* current2;
            Node* meldReturn;
            while (dq.size() > 1) {
                current1 = dq.front();
                dq.pop_front();
                current2 = dq.front();
                dq.pop_front();
                current1->parent = nullptr;
                current2->parent = nullptr;
                current1->sibling = nullptr;
                current2->sibling = nullptr;
                meldReturn = meld(current1, current2);
                dq.push_back(meldReturn);
            }
            rootptr = dq.front();
        }
        numNodes--;

    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return rootptr->elt;
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return numNodes;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return rootptr == nullptr;
    } // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value. Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //               extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        node->elt = new_value;
        if (node->parent == nullptr) {
            return;
        }
        else {
            //2 cases updated node is leftmost or not leftmost
            Node* parent = node->parent;
            //if leftmost
            if (parent->child == node) {
                //if not only node in row
                if (node->sibling != nullptr) {
                    parent->child = node->sibling;
                }
                node->parent = nullptr;
                node->sibling = nullptr;
                Node* meldReturn = meld(rootptr, node);
                rootptr = meldReturn;
            }
            else {
                Node* previous = parent->child;
                while (previous->sibling != node) {
                    previous = previous->sibling;
                }
                previous->sibling = node->sibling;
                node->parent = nullptr;
                node->sibling = nullptr;
                Node* meldReturn = meld(rootptr, node);
                rootptr = meldReturn;
            }

        }

    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    // Runtime: O(1)
    Node* addNode(const TYPE &val) {
        Node* add = new Node(val);
        if (rootptr == nullptr) {
            rootptr = add;
        }
        else {
            Node* meldReturn = meld(rootptr, add);
            rootptr = meldReturn; 
        }
        numNodes++;
        return add;
    } // addNode()


private:
    Node* rootptr;
    size_t numNodes;
    Node* meld(Node* pa, Node* pb) {
        //should maintian invariance that pa and pb should have no parents, no siblings so they must be root Nodes
        if (this->compare(pa->elt, pb->elt)) {
            if (pb->child != nullptr) {
                pa->sibling = pb->child;
            }
            pb->child = pa;
            pa->parent = pb;
            return pb;
        }
        else {
            if (pa->child != nullptr) {
                pb->sibling = pa->child;
            }
            pa->child = pb;
            pb->parent = pa;
            return pa;
        }

    }
};


#endif // PAIRINGPQ_H