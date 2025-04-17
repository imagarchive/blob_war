#include "strategy.h"

#include <cstdint>
#include <limits>
#include <filesystem>

void Strategy::applyMove (const movement& mv) {
        // To be completed...
}

void Strategy::removeMove (const movement& mv){
    applyMove(movement(mv.nx,mv.ny,mv.ox,mv.oy));
}

Sint32 Strategy::estimateCurrentScore () const {
        // To be completed...
    return 0;
}

vector<movement>& Strategy::computeValidMoves (vector<movement>& valid_moves) const {

    movement mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) {
                //iterate on possible destinations
                for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (_blobs.get(mv.nx, mv.ny) == -1) {
                            valid_moves.push_back(mv);
                        }
                    }
                }
            }
        }
    }

    return valid_moves;
}

void Strategy::computeBestMove () {
/*
    std::ofstream outfile ("out.txt", std::ios::out | std::ios::trunc);

    outfile << "\n" << "computeBestMove" << "\n";
    outfile.close();*/

    // TODO increase max level
    /*

    _max_level = 1;
    while (_max_level < 2){
        _max_level += 1;
        movement result = min_max(0,-1).m;
        _saveBestMove(result);
    }
    return;

    */

    // To be improved...

    //The following code finds a valid move.
    movement mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) {
                //iterate on possible destinations
                for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (_blobs.get(mv.nx, mv.ny) == -1) goto end_choice;
                    }
                }
            }
        }
    }

end_choice:
     _saveBestMove(mv);
     return;
}

movementEval Strategy::min_max(Uint16 level, int sign)
{
    if (level == _max_level) {
        return movementEval(estimateCurrentScore());
    }

    std::vector<movement> validMoves = {};
    validMoves = computeValidMoves(validMoves);

    std::cout << "\n" << validMoves.size() << "\n";

    // TODO: check
    movementEval eval(sign * std::numeric_limits<std::int32_t>::max());

    for(const movement& mv : validMoves){
        applyMove(mv);

        movementEval moveEval = min_max(level + 1, -sign);

        if ((sign * eval.eval) > (sign * moveEval.eval)) {
            eval = moveEval;
            eval.m = mv;
        }

        removeMove(mv);
    }

    return eval;
}

