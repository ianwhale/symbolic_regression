#pragma once

#include<cmath>
#include<stack>
#include<memory>
#include<string>
#include<sstream>
#include<iostream>
#include "evaluation.h"


using std::string; using std::make_shared; using std::shared_ptr;
using Evaluation::is_operation; using Evaluation::is_variable;

// Forward references for typedefs.
class RPNNode; class RPNTree; class Individual;

typedef shared_ptr<RPNNode> node_ptr;
typedef shared_ptr<RPNTree> tree_ptr;
typedef shared_ptr<Individual> indv_ptr;


/**
 * Struct to represent a RPNTree's nodes.
 */
struct RPNNode {

    /**
     * Default constructor.
     */
    RPNNode() {}

    /**
     * Constructor with values.
     * @param _value  string, value stored at node.
     * @param _left   node_ptr | nullptr, pointer to left child.
     * @param _right  node_ptr | nullptr, pointer to right child.
     * @param _parent node_ptr | nullptr, pointer to parent (nullptr at root).
     */
    RPNNode(string _value, node_ptr _left, node_ptr _right, node_ptr _parent) :
        value(_value), left(_left), right(_right), parent(_parent) {}

    bool is_root() const { return this->parent == nullptr; }

    bool is_leaf() const { return this->left == nullptr && this->right == nullptr; }

    string value;
    node_ptr parent = nullptr;
    node_ptr left = nullptr;
    node_ptr right = nullptr;
};


class RPNTree {
public:
    /**
     * Constructor, with root given.
     * @param _root node_ptr, root node.
     */
    RPNTree(node_ptr _root) : root(_root) {}

    /**
     * Constructor, builds tree.
     * @param _rpn_string string, reverse polish notation string.
     */
    RPNTree(const string & rpn_string) { this->build_tree(rpn_string); }

    /**
     * Copy constructor.
     * @param other RPNTree, to be copied.
     */
    RPNTree(const RPNTree & other) {
        this->root = copy_of(other.root, nullptr);
    }

    node_ptr copy_of(node_ptr current, node_ptr parent) {
        if (current == nullptr) {
            return nullptr;
        }

        // Make a copy of the current node.
        node_ptr new_current = make_shared<RPNNode>();
        new_current->value = current->value;
        new_current->parent = parent;

        // Recursively copy.
        new_current->left = copy_of(current->left, new_current);
        new_current->right = copy_of(current->right, new_current);

        return new_current;
    }

    /**
     * In order traversal on a tree.
     * @param node node_ptr, current node.
     * @param out  string,   output string.
     */
    static void in_order_traversal(node_ptr node, string & out) {
        if (node != nullptr) {
            if (! node->is_root() && is_operation(node->value)) {
                out += "( ";
            }

            RPNTree::in_order_traversal(node->left, out);
            out += node->value + " ";
            RPNTree::in_order_traversal(node->right, out);

            if (! node->is_root() && is_operation(node->value)) {
                out += ") ";
            }
        }
    }

    /**
     * In order traversal of the tree.
     * @return string, representing the traversal (formatted with parentheses).
     */
    string in_order() const {
        string out = "";
        RPNTree::in_order_traversal(this->root, out);
        return out;
    }

    /**
     * Post order traversal on a tree.
     * @param node current node.
     * @param out  output string.
     */
    static void post_order_traversal(node_ptr node, string & out) {
        if (node != nullptr) {
            RPNTree::post_order_traversal(node->left, out);
            RPNTree::post_order_traversal(node->right, out);
            out += node->value + " ";
        }
    }

    /**
     * Post order traversal of the tree (this is just the rpn string).
     * @return string, represeting the traversal.
     */
    string post_order() const {
        string out = "";
        RPNTree::post_order_traversal(this->root, out);
        return out;
    }

    /**
     * Return the number of nodes in the tree;
     * @return int
     */
    int num_nodes() {
        string out = this->post_order();

        string current_val;
        std::stringstream ss(out);
        int count = 0;

        while(getline(ss, current_val, ' ')) {
            count++;
        }

        return count;
    }



    /**
     * Returns a pointer to the node at an index.
     * More specifically, this would be the index of a symbol in the RPN string.
     * Source:
     *  https://articles.leetcode.com/binary-tree-post-order-traversal/
     * @param  idx int
     * @return     node_ptr | nullptr, nullptr will be returned in error.
     */
    node_ptr node_at(int idx) {
        if (this->root == nullptr) {
            return nullptr;
        }

        int count = 0;
        std::stack<node_ptr> node_stack;
        node_ptr current = this->root;
        node_ptr previous = nullptr;

        //
        // Do a post order traversal iteratively so we can stop at the desired node.
        //
        node_stack.push(current);
        while (! node_stack.empty()) {
            current = node_stack.top();

            if (previous == nullptr
                || previous->left == current
                || previous->right == current) {

                if (current->left != nullptr) {
                    node_stack.push(current->left);
                }
                else if (current->right != nullptr) {
                    node_stack.push(current->right);
                }
            }
            else if (current->left == previous) {
                if (current->right != nullptr) {
                    node_stack.push(current->right);
                }
            }
            else {
                if (count == idx) {
                    return current;
                }
                count++;
                node_stack.pop();
            }

            previous = current;
        }

        return nullptr;
    }

    node_ptr get_root() const { return this->root; }
    string get_rpn_string() const { return this->post_order(); }

private:
    /**
     * Builds the tree based on the given reverse polish notation string.
     */
    void build_tree(const string & rpn_string) {
        std::stack<node_ptr> node_stack;

        string current_val;
        std::stringstream ss(rpn_string);

        node_ptr a, b;
        node_ptr parent;

        while(getline(ss, current_val, ' ')) {
            if (is_operation(current_val)) {
                // Operation node, get two nodes and build a subtree.
                get_ab(node_stack, a, b);
                parent = make_shared<RPNNode>(current_val, b, a, nullptr);
                a->parent = parent;
                b->parent = parent;

                node_stack.push(parent);
            }
            else {
                // Variable or constant, push onto stack.
                a = make_shared<RPNNode>();
                a->value = current_val;

                node_stack.push(a);
            }
        }
        this->root = node_stack.top();
    }

    /**
     * Get two values off the stack.
     * @param stack stack<node_ptr> &
     * @param a     node_ptr &
     * @param b     node_ptr &
     */
    void get_ab(std::stack<node_ptr> & node_stack, node_ptr & a, node_ptr & b) {
        a = node_stack.top();
        node_stack.pop();
        b = node_stack.top();
        node_stack.pop();
    }

    node_ptr root = nullptr;
};

class Individual {
public:
    /**
     * Constructor with rpn string.
     * @param _rpn_string string, reverse polish notation function.
     */
    Individual(string _rpn_string) {
        this->tree = make_shared<RPNTree>(_rpn_string);
    }

    /**
     * Constructor with tree pointer.
     * @param _tree tree_ptr, pointer to tree representing RPN string.
     */
    Individual(tree_ptr _tree) : tree(_tree) {}

    /**
     * Copy constructor.
     * @param other Individual, to be copied.
     */
    Individual(const Individual & other) {
        // Make shared pointer by copy of the other Individual's RPNTree.
        this->tree = make_shared<RPNTree>(*(other.get_tree()));
    }

    /**
     * Returns a pointer to the node at an index.
     * Index refers to the order the node is found in a traversal of the tree.
     * @param  idx int
     * @return     node_ptr
     */
    node_ptr node_at(int idx) {
        return this->tree->node_at(idx);
    }

    tree_ptr get_tree() const { return this->tree; }

private:
    tree_ptr tree = nullptr;
    float fitness = HUGE_VALF;
};

/**
 * Ostream definition for printing individuals.
 */
std::ostream & operator<<(std::ostream & os, const Individual & indv) {
    os << indv.get_tree()->get_rpn_string() << endl;
    return os;
}
