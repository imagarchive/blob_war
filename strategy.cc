#include "strategy.h"

#include <cstdint>
#include <limits>
#include <string_view>

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

    for(Sint8 i = std::max(nx-1, 0); i < std::min(nx+2, 8) ; i++)
        for(Sint8 j = std::max(ny-1, 0) ; j < std::min(ny+2, 8)  ; j++) {
            if ((blobs.get(i, j)!=-1)&&(blobs.get(i, j)!=oldB)) {
                blobs.set(i, j, oldB);
            }
        }

}

void Strategy::removeMove (const movement& mv, bidiarray<Sint16>& blobs){
    applyMove(movement(mv.nx,mv.ny,mv.ox,mv.oy), blobs);
}

Sint32 Strategy::estimateCurrentScore (bidiarray<Sint16>& blobs, Uint16 player) const {
    bool end = true;
    Sint32 score = 0;
    for(Uint8 ox = 0 ; ox < 8 ; ox++) {
        for(Uint8 oy = 0 ; oy < 8 ; oy++) {
            if (_holes.get(ox, oy)) continue;
            if (blobs.get(ox, oy) == (int) _current_player) {
                score += 1;
            }else if (blobs.get(ox, oy) != -1){
                score -= 1;
            }else{
                end = false;
                if(_current_player == 1){continue;} // TODO remove
                for(Sint8 i = std::max(ox-2, 0); i < std::min(ox+3, 8) ; i++)
                    for(Sint8 j = std::max(oy-2, 0) ; j < std::min(oy+3, 8)  ; j++) {
                        Sint16 b = blobs.get(i, j);
                        if (b == (int) _current_player) {
                            score += 1;
                            i = 8;
                            j = 8;
                        }else if (b != -1){
                            score -= 1;
                            i = 8;
                            j = 8;
                        }
                    }
            }
        }
    }
    if(end){
        if(score > 0) {
            return std::numeric_limits<std::int32_t>::max()-1;
        }else if(score < 0){
            return std::numeric_limits<std::int32_t>::max()*(-1) + 1;
        }
    }
    return score;
}

vector<movement>& Strategy::computeValidMoves (vector<movement>& valid_moves, bidiarray<Sint16>& blobs) const {

    movement mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (blobs.get(mv.ox, mv.oy) != -1) {
                //iterate on possible destinations
                for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (blobs.get(mv.nx, mv.ny) == -1) {
                            valid_moves.push_back(mv);
                        }
                    }
                }
            }
        }
    }

    return valid_moves;
}

void Strategy::_find_other_player(){
    for(Uint8 ox = 0 ; ox < 8 ; ox++) {
        for(Uint8 oy = 0 ; oy < 8 ; oy++) {
            Sint16 b = _blobs.get(ox, oy);
            if (b != (int) _current_player && b != -1){
                _other_player = (Uint16) b;
                return;
            }
        }
    }
}

void Strategy::computeBestMove (std::string_view __strategy_type)
{
    _find_other_player();
  //The following code finds a valid move.
  movement mv(0,0,0,0);

  if (__strategy_type == "greedy") {
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
  } else if (__strategy_type == "min_max") {
    _max_level = 1;

    while (true) {
      movement result = min_max_para(_blobs,0,_current_player).m;
      _saveBestMove(result);
#ifdef DEBUG
      if(result.ox == result.oy && result.oy == result.nx && result.nx == result.ny && result.nx == 0){
        std::cout << "trouve rien  !";
      }
	    std::cout << "finsh level " << _max_level << std::endl;
#endif

      ++_max_level;
    }
  } else if (__strategy_type == "alpha_beta") {
    _max_level = 1;

    while (true) {
      movement result = alpha_beta_seq(_blobs,0,_current_player, -1 * std::numeric_limits<std::int32_t>::max(), std::numeric_limits<std::int32_t>::max()).m;
      _saveBestMove(result);
#ifdef DEBUG
      if(result.ox == result.oy && result.oy == result.nx && result.nx == result.ny && result.nx == 0){
        std::cout << "trouve rien  !";
      }
	    std::cout << "finsh level " << _max_level << std::endl;
#endif

      ++_max_level;
    }
  } else if (__strategy_type == "alpha_beta_par") {
    // TODO: implement alpha_beta_par
  }
}

movementEval Strategy::min_max_para(bidiarray<Sint16>& blobs, Uint16 level, Uint16 player)
{
    if(_max_level - level <= 3){//TODO test
        return min_max_seq(blobs, level, player);
    }

    if (level == _max_level) {
        return movementEval(estimateCurrentScore(blobs, player));
    }

    Sint16 sign = player == _current_player ? -1 : 1;
    
    std::vector<movement> validMoves = {};
    validMoves = computeValidMoves(validMoves, blobs);

    if(validMoves.size() == 0){
        return movementEval(estimateCurrentScore(blobs, player));
    }

    movementEval eval(sign * std::numeric_limits<std::int32_t>::max());

    #pragma omp parallel for
    for(const movement& mv : validMoves){

        if(blobs.get(mv.ox, mv.oy) != (int) player) { continue; }

        bidiarray<Sint16> blobs2 = blobs;
        applyMove(mv, blobs2);

        movementEval moveEval = min_max_seq(blobs2,level + 1, player == _current_player ? _other_player : _current_player);
        if ((sign * eval.eval) > (sign * moveEval.eval)) {
            eval = moveEval;
            eval.m = mv;
        }

        //removeMove(mv, blobs);
    }

    movement& result = eval.m;
    if(result.ox == 0 && result.ox == result.oy && result.oy == result.nx && result.nx == result.ny){
        bidiarray<Sint16> blobs2 = blobs;
        eval.eval = min_max_seq(blobs2,level + 1, player == _current_player ? _other_player : _current_player).eval;
    }

    return eval;
}

movementEval Strategy::min_max_seq(bidiarray<Sint16>& blobs, Uint16 level, Uint16 player)
{
    if (level == _max_level) {
        return movementEval(estimateCurrentScore(blobs, player));
    }

    Sint16 sign = player == _current_player ? -1 : 1;

    std::vector<movement> validMoves = {};
    validMoves = computeValidMoves(validMoves, blobs);

    if(validMoves.size() == 0){
        return movementEval(estimateCurrentScore(blobs, player));
    }

    movementEval eval(sign * std::numeric_limits<std::int32_t>::max());

    for(const movement& mv : validMoves){

        if(blobs.get(mv.ox, mv.oy) != (int) player) { continue; }

        bidiarray<Sint16> blobs2 = blobs;
        applyMove(mv, blobs2);

        movementEval moveEval = min_max_seq(blobs2,level + 1, player == _current_player ? _other_player : _current_player);
        if ((sign * eval.eval) > (sign * moveEval.eval)) {
            eval = moveEval;
            eval.m = mv;
        }

        //removeMove(mv, blobs);
    }
    movement& result = eval.m;
    if(result.ox == 0 && result.ox == result.oy && result.oy == result.nx && result.nx == result.ny){
        bidiarray<Sint16> blobs2 = blobs;
        eval.eval = min_max_seq(blobs2,level + 1, player == _current_player ? _other_player : _current_player).eval;
    }

    return eval;
}

movementEval Strategy::alpha_beta_seq(bidiarray<Sint16>& blobs, Uint16 level, Uint16 player, Sint32 alpha, Sint32 beta)
{
    if (level == _max_level) {
        return movementEval(estimateCurrentScore(blobs, player));
    }

    Sint16 sign = player == _current_player ? -1 : 1;

    std::vector<movement> validMoves = {};
    validMoves = computeValidMoves(validMoves, blobs);

    if(validMoves.size() == 0){
        return movementEval(estimateCurrentScore(blobs, player));
    }

    movementEval eval(sign * std::numeric_limits<std::int32_t>::max());

    if(player == _current_player) // on max
    {
        for(const movement& mv : validMoves){

            if(blobs.get(mv.ox, mv.oy) != (int) player) { continue; }

            bidiarray<Sint16> blobs2 = blobs;
            applyMove(mv, blobs2);

            movementEval moveEval = alpha_beta_seq(blobs2,level + 1, player == _current_player ? _other_player : _current_player, alpha, beta);
            if ((sign * eval.eval) > (sign * moveEval.eval)) {
                eval.eval = moveEval.eval;
                eval.m = mv;
                alpha = moveEval.eval;
                if(alpha >= beta){
                    return movementEval(beta);
                }
            }

            //removeMove(mv, blobs);
        }
        movement& result = eval.m;
        if(result.ox == 0 && result.ox == result.oy && result.oy == result.nx && result.nx == result.ny){
            bidiarray<Sint16> blobs2 = blobs;
            eval.eval = alpha_beta_seq(blobs2,level + 1, player == _current_player ? _other_player : _current_player, alpha, beta).eval;
        }
    }else
    {
        for(const movement& mv : validMoves){

            if(blobs.get(mv.ox, mv.oy) != (int) player) { continue; }

            bidiarray<Sint16> blobs2 = blobs;
            applyMove(mv, blobs2);

            movementEval moveEval = alpha_beta_seq(blobs2,level + 1, player == _current_player ? _other_player : _current_player, alpha, beta);
            if ((sign * eval.eval) > (sign * moveEval.eval)) {
                eval.eval = moveEval.eval;
                eval.m = mv;
                beta = moveEval.eval;
                if(alpha >= beta){
                    return movementEval(alpha);
                }
            }

            //removeMove(mv, blobs);
        }
        movement& result = eval.m;
        if(result.ox == 0 && result.ox == result.oy && result.oy == result.nx && result.nx == result.ny){
            bidiarray<Sint16> blobs2 = blobs;
            eval.eval = alpha_beta_seq(blobs2,level + 1, player == _current_player ? _other_player : _current_player, alpha, beta).eval;
        }
    }

    return eval;
}