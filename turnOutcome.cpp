//
// Created by renew on 3/9/2021.
//

#include "turnOutcome.h"

TurnOutcome::TurnOutcome(int d, int s, int bpps, bool p): depth(d), step(s), boardProcessingSpeed(bpps), pruning(p){}

bool operator<(const TurnOutcome &lhs, const TurnOutcome &rhs) {
    if(lhs.step > rhs.step)
        return true;
    else if (lhs.step == rhs.step && lhs.depth > rhs.depth)
        return true;
    return false;

}
