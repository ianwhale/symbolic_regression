#include<set>
#include<cmath>
#include<stack>
#include<memory>
#include<string>
#include<sstream>
#include<iostream>
#include "evaluation.h"
#include "individual.h"

using std::endl; using std::cout;
using std::string; using std::make_shared; using std::shared_ptr;


// void print_stack(std::stack<node_ptr> s) {
//     std::stack<node_ptr> c = s; // Copy
//     cout << "Stack: ";
//     while (! c.empty()) {
//         cout << c.top()->value << " ";
//         c.pop();
//     }
//     cout << endl;
// }

/**
 * Builds the tree based on the given reverse polish notation string.
 */
void RPNTree::build_tree(const string & rpn_string) {
    std::stack<node_ptr> node_stack;

    string current_val;
    std::stringstream ss(rpn_string);

    node_ptr a, b;
    node_ptr parent;

    while(getline(ss, current_val, ' ')) {
        if (Evaluation::is_operation(current_val)) {
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
void RPNTree::get_ab(std::stack<node_ptr> & node_stack, node_ptr & a, node_ptr & b) {
    a = node_stack.top();
    node_stack.pop();
    b = node_stack.top();
    node_stack.pop();
}

/**
 * Recurseively copies the current subtree.
 * @param  current node_ptr
 * @param  parent  node_ptr
 * @return         node_ptr
 */
node_ptr RPNTree::copy_of(node_ptr current, node_ptr parent) {
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
void RPNTree::in_order_traversal(node_ptr node, string & out) {
    if (node != nullptr) {
        if (! node->is_root() && Evaluation::is_operation(node->value)) {
            out += "( ";
        }

        RPNTree::in_order_traversal(node->left, out);
        out += node->value + " ";
        RPNTree::in_order_traversal(node->right, out);

        if (! node->is_root() && Evaluation::is_operation(node->value)) {
            out += ") ";
        }
    }
}

/**
 * In order traversal of the tree.
 * @return string, representing the traversal (formatted with parentheses).
 */
string RPNTree::in_order() const {
    string out = "";
    RPNTree::in_order_traversal(this->root, out);
    return out;
}

/**
 * Post order traversal on a tree.
 * @param node current node.
 * @param out  output string.
 */
void RPNTree::post_order_traversal(node_ptr node, string & out) {
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
string RPNTree::post_order() const {
    string out = "";
    RPNTree::post_order_traversal(this->root, out);
    return out;
}

/**
 * Counts the number of nodes.
 * @param  node node_ptr, current node.
 * @return      int
 */
int num_nodes_recursive(const node_ptr node) {
    int count = 1;

    if (node->left != nullptr) {
        count += num_nodes_recursive(node->left);
    }
    if (node->right != nullptr) {
        count += num_nodes_recursive(node->right);
    }

    return count;
}

/**
 * Return the number of nodes in the tree;
 * @return int
 */
int RPNTree::num_nodes() {
    return num_nodes_recursive(this->root);
}



/**
 * Returns a pointer to the node at an index.
 * More specifically, this would be the index of a symbol in the RPN string.
 * Source:
 *  https://articles.leetcode.com/binary-tree-post-order-traversal/
 * @param  idx int
 * @return     node_ptr | nullptr, nullptr will be returned in error.
 */
node_ptr RPNTree::node_at(int idx) {
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

/**
 * Ostream definition for printing individuals.
 */
std::ostream & operator<<(std::ostream & os, const Individual & indv) {
    os << "RPN: " << indv.get_tree()->get_rpn_string() <<
        "\nInfix: " << indv.get_tree()->get_infix_string() << endl;
    return os;
}
