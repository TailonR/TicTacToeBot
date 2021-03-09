//
// Created by renew on 3/9/2021.
//

#ifndef TICTACTOEBOT_TURNOUTCOME_H
#define TICTACTOEBOT_TURNOUTCOME_H
class TurnOutcome {
public:
    TurnOutcome(int d, int s, int bpps, bool p);
    int depth;
    int step;
    int boardProcessingSpeed;
    bool pruning;
};

bool operator<(const TurnOutcome &lhs, const TurnOutcome &rhs);

#endif //TICTACTOEBOT_TURNOUTCOME_H
