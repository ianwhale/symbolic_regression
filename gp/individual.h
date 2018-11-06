#pragma once

#include<string>
#include<memory>
#include<stack>

typedef shared_ptr<RPNNode> node_ptr;
typdef shared_ptr<RPNTree> tree_ptr;

class Individual {
public:
    Individual(string _rpn_string) {
        this.tree = make_shared<RPNTree>(_rpn_string);
    }

    Individual crossover(Individual * other) {

    }

    tree_ptr getTree() {return this.tree;}

private:
    tree_ptr tree = nullptr;
}


class RPNTree {
public:
    /**
     * Constructor, builds tree.
     * @param _rpn_string string, reverse polish notation string.
     */
    RPNTree(string _rpn_string) : rpn_string(_rpn_string) {this.buildTree();}

    /**
     * Builds the tree based on the given reverse polish string.
     */
    void buildTree() {}

    /**
     * In order traversal of the tree.
     * @return string, representing the traversal (formatted with parentheses).
     */
    string inOrder() {}

    /**
     * Post order traversal of the tree (this is just the rpn string
     * @return string, represeting the traversal.
     */
    string postOrder() {return this.rpn_string;}

    struct RPNNode {
        RPNNode(string _value, node_ptr _left, node_ptr _right) :
            value(_value), left(_left), right(_right) {}

        string value;
        node_ptr left;
        node_ptr right;
    };

private:
    string rpn_string;
    shared_ptr<RPNNode> root = nullptr;
};
