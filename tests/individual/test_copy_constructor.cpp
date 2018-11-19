#include <memory>
#include <string>
#include "utils.h"
#include "../../gp/individual.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"


using std::make_shared; using std::string;

/**
 * Traverse a tree in order.
 * @param node current node.
 * @param out  output string.
 */
void in_order_traversal(node_ptr node, string & out) {
    if (node != nullptr) {
        in_order_traversal(node->left, out);
        out += node->value + " ";
        in_order_traversal(node->right, out);
    }
}

/**
 * Takes two of the same trees and REQUIRE that they are different.
 * @param node_a node_ptr
 * @param node_b node_ptr
 */
void in_order_requires(node_ptr node_a, node_ptr node_b) {
    if (node_a != nullptr) {
        in_order_requires(node_a->left, node_b->left);
        REQUIRE(node_a != node_b);
        in_order_requires(node_a->right, node_b->right);
    }
}

TEST_CASE("Copy constructor", "[unit]") {
    const string expected = "2 1 4 3 5 root 7 6 8 ";

    node_ptr root = make_test_tree();
    string pre_copy = "";
    in_order_traversal(root, pre_copy);

    REQUIRE(pre_copy == expected);

    tree_ptr tree = make_shared<RPNTree>(root);
    Individual indv(tree);

    Individual indv_copy = indv; // Call copy constructor.

    string post_copy = "";
    in_order_traversal(indv_copy.get_tree()->get_root(), post_copy);

    REQUIRE(pre_copy == post_copy);

    // Be sure everything if different.
    in_order_requires(indv_copy.get_tree()->get_root(), indv.get_tree()->get_root());
}
