#include "strategy.h"


void Strategy::applyMove (const movement& mv) {
        // To be completed...
}

void Strategy::removeMove (const movement& mv){
    applyMove(mv.nx,mv.ny,mv.ox,mv.oy);
}

Sint32 Strategy::estimateCurrentScore () const {
        // To be completed...
    return 0;
}

vector<movement>& Strategy::computeValidMoves (vector<movement>& valid_moves) const {
        // To be completed...
    return valid_moves;
}

void Strategy::computeBestMove () {
    // TODO increase max level
    /*
    _max_level = 1;

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

movementEval min_max(Uint16 level, int sign){
    if(level == _max_level) { return movementEval(estimateCurrentScore()); }
    
    vector<movement>& validMoves = computeValidMoves();
    
    movementEval eval(sign * __INT32_MAX__); // TODO check
    for(movement& mv : validMoves){
        applyMove(mv);
        
        movementEval moveEval = min_max(level + 1, -i);
        if(i * eval.eval > i * moveEval.eval){
            eval = moveEval;
        }
        removeMove(mv);
    }
    return eval;
}

