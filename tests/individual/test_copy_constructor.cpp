#include <memory>
#include <string>
#include "../../gp/individual.h"
#include "../../third-party/Catch2/single_include/catch2/catch.hpp"


using std::make_shared; using std::string;

/**
 * Make a tree of specific structure for testing.
 *                  root
 *                 /    \
 *                1      6
 *               / \    / \
 *              2   3  7   8
 *                 / \
 *                4   5
 * In-order traversal: 2, 1, 4, 3, 5, root, 7, 8, 6.
 * @return node_ptr, root node.
 */
node_ptr make_test_tree() {
    node_ptr root = make_shared<RPNNode>();
    root->value = "root";

    // Build left subtree first.
    node_ptr node_1 = make_shared<RPNNode>();
    node_1->value = "1";
    node_1->parent = root;

    root->left = node_1;

    node_ptr node_2 = make_shared<RPNNode>();
    node_2->value = "2";
    node_2->parent = node_1;

    node_ptr node_3 = make_shared<RPNNode>();
    node_3->value = "3";
    node_3->parent = node_1;

    node_1->left = node_2;
    node_1->right = node_3;

    node_ptr node_4 = make_shared<RPNNode>();
    node_4->value = "4";
    node_4->parent = node_3;

    node_ptr node_5 = make_shared<RPNNode>();
    node_5->value = "5";
    node_5->parent = node_3;

    node_3->left = node_4;
    node_3->right = node_5;

    // Build right subtree.
    node_ptr node_6 = make_shared<RPNNode>();
    node_6->value = "6";
    node_6->parent = root;

    root->right = node_6;

    node_ptr node_7 = make_shared<RPNNode>();
    node_7->value = "7";
    node_7->parent = node_6;

    node_ptr node_8 = make_shared<RPNNode>();
    node_8->value = "8";
    node_8->parent = node_6;

    node_6->left = node_7;
    node_6->right = node_8;

    return root;
}

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
}
