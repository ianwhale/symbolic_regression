#pragma once

#include<cmath>
#include<stack>
#include<memory>
#include<string>
#include<iostream>
#include<sstream>

using std::string; using std::make_shared; using std::shared_ptr;

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
     * @param _rpn_string string, rev[description]erse polish notation string.
     */
    RPNTree(const string & rpn_string) { this->build_tree(rpn_string); }

    /**
     * Copy constructor.
     * @param other RPNTree, to be copied.
     */
    RPNTree(const RPNTree & other) {
        this->root = copy_of(other.root, nullptr);
    }

    node_ptr copy_of(node_ptr current, node_ptr parent);
    static void in_order_traversal(node_ptr node, string & out);
    string in_order() const;
    static void post_order_traversal(node_ptr node, string & out);
    string post_order() const;
    int num_nodes();
    node_ptr node_at(int idx);

    node_ptr get_root() const { return this->root; }
    string get_rpn_string() const { return this->post_order(); }

    /**
     * Infix notation is the usual function notation.
     * Formatted for clarity on order of operations.
     * @return string
     */
    string get_infix_string() const { return this->in_order(); }

private:
    void build_tree(const string & rpn_string);
    void get_ab(std::stack<node_ptr> & node_stack, node_ptr & a, node_ptr & b);

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
    float get_fitness() const { return this->fitness; }
    void set_fitness(float f) { this->fitness = f; }
    friend std::ostream & operator<<(std::ostream & os, const Individual & indv);

private:
    tree_ptr tree = nullptr;
    float fitness = HUGE_VALF;
};
