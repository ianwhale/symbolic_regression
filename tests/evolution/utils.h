#include "../../gp/evolution.h"

void reset_distributions() {
    Evolution::COIN_FLIP.reset();
    Evolution::EPHEMERAL_RANDOM_CONSTANTS.reset();
    Evolution::RAND.reset();
}
