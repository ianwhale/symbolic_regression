#pragma once

#include<string>
#include<memory>
#include<stack>

using std::string; using std::make_shared; using std::shared_ptr;

// Forward references for typedefs.
class RPNNode; class RPNTree; class Individual;

typedef shared_ptr<RPNNode> node_ptr;
typedef shared_ptr<RPNTree> tree_ptr;
typedef shared_ptr<Individual> indv_ptr;


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

    tree_ptr get_tree() const {return this->tree;}

private:
    tree_ptr tree = nullptr;
};


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
    RPNTree(string _rpn_string) : rpn_string(_rpn_string) { this->buildTree(); }

    /**
     * Copy constructor.
     * @param other RPNTree, to be copied.
     */
    RPNTree(const RPNTree & other) {
        this->rpn_string = other.rpn_string;
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
     * In order traversal of the tree.
     * @return string, representing the traversal (formatted with parentheses).
     */
    string in_order() {}

    /**
     * Post order traversal of the tree (this is just the rpn string).
     * @return string, represeting the traversal.
     */
    string post_order() { return this->rpn_string; }

    /**
     * Return the number of nodes in the
     * @return [description]
     */
    int num_nodes() {}

    node_ptr get_root() const { return this->root; }
    string get_rpn_string() const { return this->rpn_string; }

private:
    /**
     * Builds the tree based on the given reverse polish notation string.
     */
    void buildTree() {}

    string rpn_string = "";
    node_ptr root = nullptr;
};
