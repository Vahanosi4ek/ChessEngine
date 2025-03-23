#include "test.h"

#include <chrono>

int perft(Board& board, const int& depth) {
    int res = 0;
    if (depth == 0) return 1;
    else {
        Board b = board;
        MoveList moves = board.gen_legal_moves();
        for (Move move : moves) {
            board.make_move(move);
            res += perft(board, depth - 1);
            board.undo_move(move);
            // Useful for debugging
            // if (!(board == b)) {
            //     std::cout << "Not equal" << std::endl;
            //     std::cout << b;
            //     std::cout << board;
            //     assert(false);
            // }
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
        board.undo_move(move);
    }

    std::cout << "Total leaf nodes: " << res << std::endl;

    return res;
}

int test_time_movegen(Board& board, bool print) {
    auto start = std::chrono::high_resolution_clock::now();
    if (print) std::cout << "Started timing..." << std::endl;
    
    board.load_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    TEST(1, perft(board, 4) == 197281, "Starting position", print);

    board.load_from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    TEST(2, perft(board, 3) == 97862, "Complex position - caught bugs for castling and promotion", print);

    board.load_from_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    TEST(3, perft(board, 5) == 674624, "Simple endgames - caught bugs for en passant pins", print);

    board.load_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    TEST(4, perft(board, 4) == 422333, "Very complex position - caught bugs overall", print);

    board.load_from_fen("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
    TEST(5, perft(board, 4) == 422333, "Same as last, but mirrored - good for catching mirroring bugs", print);

    board.load_from_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    TEST(6, perft(board, 3) == 62379, "Complex position with high nodes - good for testing performance", print);

    board.load_from_fen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    TEST(7, perft(board, 3) == 89890, "Bonus - good for catching bugs overall", print);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (print) std::cout << "Tests took " << duration.count() << " milliseconds" << std::endl;

    return duration.count();
}

int test_startpos_perft(Board& board, bool print) {
    auto start = std::chrono::high_resolution_clock::now();
    if (print) std::cout << "Started timing..." << std::endl;
    
    board.load_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    TEST(1, perft(board, 6) == 119060324, "Starting position", print);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (print) std::cout << "Tests took " << duration.count() << " milliseconds" << std::endl;

    return duration.count();
}

float repeated_tests(Board& board, int times) {
    int total_duration = 0;
    int cur = 0;

    for (int i = 0; i < times; i++) {
        cur = test_time_movegen(board, false);
        std::cout << "Iteration " << (i + 1) << ": " << cur << " milliseconds" << std::endl;
        total_duration += cur;
    }

    std::cout << "Average time spent over " << times << " iterations: " << total_duration / times << " milliseconds" << std::endl;

    return total_duration / times;
}