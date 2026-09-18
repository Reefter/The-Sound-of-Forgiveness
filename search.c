#include <stdio.h>
#include <stdlib.h>
#include "board.h"

int mateValue = 32000;
unsigned long long nodes = 0;

void OrderMoves(MoveList *moves, const Board *board) {
    int scores[220];

    for (int i = 0; i < moves->count; i++) {
        int startSq  = moves->moves[i][0];
        int targetSq = moves->moves[i][1];
        int promo    = moves->moves[i][2];

        int moveScore = 0;
        int movedPiece = board->sq[startSq];
        int targetPiece = board->sq[targetSq];

        if (targetPiece != NONE) {
            moveScore = 10 * get_piece_type(targetPiece) - get_piece_type(movedPiece);
        }

        if (promo != NONE) {
            moveScore += promo;
        }

        /*
        if (IsSquareAttacked(targetSq, enemyColor, board)) {
            moveScore -= get_piece_type(movedPiece);
        }
        */
        // Commented out cause removing it gains elo so regardless the moves will get ordered
        scores[i] = moveScore;
    }

    for (int i = 0; i < moves->count - 1; i++) {
        for (int j = i + 1; j < moves->count; j++) {
            if (scores[j] > scores[i]) {
                int tempScore = scores[i];
                scores[i] = scores[j];
                scores[j] = tempScore;

                int tempStart  = moves->moves[i][0];
                int tempTarget = moves->moves[i][1];
                int tempPromo  = moves->moves[i][2];

                moves->moves[i][0] = moves->moves[j][0];
                moves->moves[i][1] = moves->moves[j][1];
                moves->moves[i][2] = moves->moves[j][2];

                moves->moves[j][0] = tempStart;
                moves->moves[j][1] = tempTarget;
                moves->moves[j][2] = tempPromo;
            }
        }
    }
}

int SearchAllCaptures(int alpha, int beta, const Board *board) {
    nodes++;

    int best = Evaluate(board);

    if (best >= beta) {
        return best;
    }

    if (best > alpha) {
        alpha = best;
    }

    MoveList moves;
    GenerateMoves(board, &moves);
    OrderMoves(&moves, board);

    for (int move = 0; move < moves.count; move++) {
        int startSq  = moves.moves[move][0];
        int targetSq = moves.moves[move][1];
        int promo    = moves.moves[move][2];

        if (board->sq[targetSq] == NONE) {
            continue;
        }

        Board next = *board;
        MakeMove(&next, startSq, targetSq, promo);

        int score = -SearchAllCaptures(-beta, -alpha, &next);

        if (score > best) {
            best = score;
        }

        if (score > alpha) {
            alpha = score;
        }

        if (score >= beta) {
            break;
        }
    }

    return best;
}

int Search(int depth, int ply, const Board *board, int bestMove[3], int alpha, int beta) {
    nodes++;

    if (depth == 0) {
        return SearchAllCaptures(alpha, beta, board);
    }

    MoveList moves;
    GenerateMoves(board, &moves);
    OrderMoves(&moves, board);

    if (moves.count == 0) {
        int kingSq     = board->white_to_move ? board->whiteKingLocation : board->blackKingLocation;
        int enemyColor = board->white_to_move ? BLACK : WHITE;

        if (IsSquareAttacked(kingSq, enemyColor, board)) {
            return -mateValue + ply;
        }

        return 0;
    }

    int best = -INF;

    for (int i = 0; i < moves.count; i++) {
        int startSq  = moves.moves[i][0];
        int targetSq = moves.moves[i][1];
        int promo    = moves.moves[i][2];

        Board next = *board;
        MakeMove(&next, startSq, targetSq, promo);

        int childBestMove[3];
        int score = -Search(depth - 1, ply + 1, &next, childBestMove, -beta, -alpha);

        if (score > best) {
            best = score;
            bestMove[0] = startSq;
            bestMove[1] = targetSq;
            bestMove[2] = promo;
        }

        if (score > alpha) {
            alpha = score;
        }

        if (score >= beta) {
            break;
        }
    }

    return best;
}

unsigned long long Perft(int depth, const Board *board) {
    if (depth == 0) {
        return 1;
    }

    unsigned long long nodes = 0;
    MoveList moves;
    GenerateMoves(board, &moves);

    for (int i = 0; i < moves.count; i++) {
        int startSq  = moves.moves[i][0];
        int targetSq = moves.moves[i][1];
        int promo    = moves.moves[i][2];

        Board nextBoard = *board;
        MakeMove(&nextBoard, startSq, targetSq, promo);
        nodes += Perft(depth - 1, &nextBoard);
    }

    return nodes;
}

void PrintPerftTable(int maxDepth, const Board *board) {
    printf("Depth   Nodes\n");
    printf("---------------------\n");

    for (int d = 0; d <= maxDepth; d++) {
        printf("%-7d %llu\n", d, Perft(d, board));
    }
}