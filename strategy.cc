#include "strategy.h"

#include <cstdint>
#include <limits>
#include <filesystem>

void Strategy::applyMove (const movement& mv, bidiarray<Sint16>& blobs) {

    Uint8 ox = mv.ox;
    Uint8 oy = mv.oy;
    Uint8 nx = mv.nx;
    Uint8 ny = mv.ny;
    Sint16 oldB = blobs.get(ox,oy);
    //first check if we need to create a new blob or to move an old one
    if (((ox - nx)*(ox - nx)<=1)&&((oy - ny)*(oy - ny)<=1)) {
        //it's a copy
        blobs.set(nx,ny,oldB);
    } else {
        //it's a move
        blobs.set(ox ,oy, -1);
        blobs.set(nx ,ny, oldB);
    }

    //now eventually change some neighbours colors

    for(Sint8 i = -1 ; i < 2 ; i++)
        for(Sint8 j = -1 ; j < 2 ; j++) {
            if (nx+i < 0) continue;
            if (nx+i > 7) continue;
            if (ny+j < 0) continue;
            if (ny+j > 7) continue;
            if ((blobs.get(nx+i, ny+j)!=-1)&&(blobs.get(nx+i, ny+j)!=oldB)) {
                blobs.set(nx+i, ny+j, oldB);
            }
        }
		
}

void Strategy::removeMove (const movement& mv, bidiarray<Sint16>& blobs){
    applyMove(movement(mv.nx,mv.ny,mv.ox,mv.oy), blobs);
}

Sint32 Strategy::estimateCurrentScore (bidiarray<Sint16>& blobs) const {
    Sint32 score = 0;
    for(Uint8 ox = 0 ; ox < 8 ; ox++) {
        for(Uint8 oy = 0 ; oy < 8 ; oy++) {
            if (blobs.get(ox, oy) == (int) _current_player) {
                score += 1;
            }else if (blobs.get(ox, oy) != -1){
                score -= 1;
            }
        }
    }
    return score;
}

vector<movement>& Strategy::computeValidMoves (vector<movement>& valid_moves, bidiarray<Sint16>& blobs) const {

    movement mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            //if (blobs.get(mv.ox, mv.oy) == (int) _current_player) {
                //iterate on possible destinations
                for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (blobs.get(mv.nx, mv.ny) == -1) {
                            valid_moves.push_back(mv);
                        }
                    }
                }
            //}
        }
    }

    return valid_moves;
}

void Strategy::_find_other_player(){
    for(Uint8 ox = 0 ; ox < 8 ; ox++) {
        for(Uint8 oy = 0 ; oy < 8 ; oy++) {
            Sint16 b = _blobs.get(ox, oy);
            if (b != (int) _current_player && b != -1){
                _other_player = b;
            }
        }
    }
}

void Strategy::computeBestMove () {

    _find_other_player();
    _max_level = 1;
    while (true){
        movement result = min_max_seq(_blobs,0,_current_player).m;
        _saveBestMove(result);
#ifdef DEBUG
	    cout << "finsh level " << _max_level << endl;
#endif
        _max_level += 1;
    }
    return;

    //*/

    // To be improved...

    //The following code finds a valid move.
    movement mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) { // blobs
                //iterate on possible destinations
                for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (_blobs.get(mv.nx, mv.ny) == -1) goto end_choice; // blobs
                    }
                }
            }
        }
    }

end_choice:
     _saveBestMove(mv);
     return;
}

movementEval Strategy::min_max_seq(bidiarray<Sint16>& blobs, Uint16 level, Uint16 player)
{
    Sint16 sign = player == _current_player ? -1 : 1;
    if (level == _max_level) {
        return movementEval(estimateCurrentScore(blobs));
    }

    std::vector<movement> validMoves = {};
    validMoves = computeValidMoves(validMoves, blobs);

    std::cout << validMoves.size();

    movementEval eval(sign * std::numeric_limits<std::int32_t>::max());

    for(const movement& mv : validMoves){
        if(blobs.get(mv.ox, mv.oy) != (int) player) { continue; }
        applyMove(mv, blobs);

        movementEval moveEval = min_max_seq(blobs,level + 1, player == _current_player ? _other_player : _current_player);

        if ((sign * eval.eval) > (sign * moveEval.eval)) {
            eval = moveEval;
            eval.m = mv;
        }

        removeMove(mv, blobs);
    }

    return eval;
}

