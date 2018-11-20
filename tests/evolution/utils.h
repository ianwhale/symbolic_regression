#pragma once

#include "../../gp/individual.h"
#include "../../gp/evolution.h"

/**
 * Resets the static distributions in the Evolution namespace.
 */
void reset_distributions() {
    Evolution::COIN_FLIP.reset();
    Evolution::EPHEMERAL_RANDOM_CONSTANTS.reset();
    Evolution::RAND.reset();
}

/**
 * Get the depth of a tree starting at some node.
 * @param current node_ptr
 * @param depth   int
 */
int get_depth(node_ptr current) {
    if (current == nullptr) {
        return 0;
    }
    else {
        int left_depth = get_depth(current->left);
        int right_depth = get_depth(current->right);

        if (left_depth > right_depth) {
            return left_depth + 1;
        }
        else {
            return right_depth + 1;
        }
    }
}
