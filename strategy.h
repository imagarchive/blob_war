#ifndef __STRATEGY_H
#define __STRATEGY_H

#include "common.h"
#include "bidiarray.h"
#include "move.h"


/** Move class
 */
struct movementEval
{
    movementEval(){}
    
    movementEval(const movementEval & mv)
            :m(mv.m), eavl(mv.eval) {}
    movementEval(const movement& mov, const Sint32 ev)
            :m(mov), eavl(ev){}
    movementEval(const Sint32 ev)
            :m(movement{0,0,0,0}), eavl(ev){}
    
    movementEval& operator=(const movementEval& mv)
        {m = mv.m; eval = mv.eval; return *this;}
        
    
    movement m;
    Sint32 eval;
};

class Strategy {

private:
    //! array containing all blobs on the board
    bidiarray<Sint16> _blobs;
    //! an array of booleans indicating for each cell whether it is a hole or not.
    const bidiarray<bool>& _holes;
    //! Current player
    Uint16 _current_player;
    //! max level
    Uint16 _max_level = 1;
    
    //! Call this function to save your best move.
    //! Multiple call can be done each turn,
    //! Only the last move saved will be used.
    void (*_saveBestMove)(movement&);

    // implantation of minmax
    // return the evaluation of the board
    // and save the best move coresponding to it
    movementEval min_max(Uint16 level, int sign);    

public:
        // Constructor from a current situation
    Strategy (bidiarray<Sint16>& blobs, 
              const bidiarray<bool>& holes,
              const Uint16 current_player,
              void (*saveBestMove)(movement&))
            : _blobs(blobs),_holes(holes), _current_player(current_player), _saveBestMove(saveBestMove)
        {
        }
    
              
    
        // Copy constructor
    Strategy (const Strategy& St)
            : _blobs(St._blobs), _holes(St._holes),_current_player(St._current_player) 
        {}
    
        // Destructor
    ~Strategy() {}
    
        /** 
         * Apply a move to the current state of blobs
         * Assumes that the move is valid
         */
    void applyMove (const movement& mv);


        /** 
         * Apply a move that cancel mv to the current state of blobs
         * Assumes that the move is valid
         */
    void removeMove (const movement& mv);

        /**
         * Compute the vector containing every possible moves
         */
    vector<movement>& computeValidMoves (vector<movement>& valid_moves) const;

        /**
         * Estimate the score of the current state of the game
         */
    Sint32 estimateCurrentScore () const;

        /**
         * Find the best move.
         */
    void computeBestMove ();

};

#endif
