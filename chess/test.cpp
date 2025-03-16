#include "test.h"

#include <chrono>

int perft(Board& board, const int& depth) {
    int res = 0;
    if (depth == 0) return 1;
    else {
        MoveList moves = board.gen_legal_moves();
        for (Move move : moves) {
            board.make_move(move);
            res += perft(board, depth - 1);
            board.undo_move();
        }
    }

    return res;
}

int dump_perft(Board& board, const int& depth) {
    int res = 0;
    int cur_res = 0;
    for (Move move : board.gen_legal_moves()) {
        board.make_move(move);
        cur_res = perft(board, depth - 1);
        res += cur_res;
        std::cout << move << ": " << cur_res << std::endl;
        board.undo_move();
    }

    std::cout << "Total leaf nodes: " << res << std::endl;

    return res;
}

void test_time_movegen() {
    Board board;

    auto start = std::chrono::high_resolution_clock::now();
    
    board.load_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    TEST(1, perft(board, 4) == 197281, "Starting position");

    board.load_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    TEST(2, perft(board, 3) == 97862, "Complex position - caught bugs for castling and promotion");

    board.load_from_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    TEST(3, perft(board, 5) == 674624, "Simple endgames - caught bugs for en passant pins");

    board.load_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    TEST(4, perft(board, 4) == 422333, "Very complex position - caught bugs overall");

    board.load_from_fen("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
    TEST(5, perft(board, 4) == 422333, "Same as last, but mirrored - good for catching mirroring bugs");

    board.load_from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    TEST(6, perft(board, 3) == 62379, "Complex position with high nodes - good for testing performance");

    board.load_from_fen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    TEST(7, perft(board, 3) == 89890, "Bonus - good for catching bugs overall");

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Tests took " << duration.count() << " milliseconds" << std::endl;
}