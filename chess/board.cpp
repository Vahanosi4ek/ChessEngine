#include "board.h"

#include <sstream>
#include <iostream>

Bitboard knight_attacks[SQ_ALL];
Bitboard king_attacks[SQ_ALL];

constexpr Bitboard get_knight_attacks(Square sq) {
    Bitboard bb = 0ull;

    if (get_row(sq) != 0 && get_row(sq) != 1 && get_col(sq) != 0) bb |= Square(sq - 17);
    if (get_row(sq) != 0 && get_col(sq) != 1 && get_col(sq) != 0) bb |= Square(sq - 10);
    if (get_row(sq) != 7 && get_col(sq) != 1 && get_col(sq) != 0) bb |= Square(sq + 6);
    if (get_row(sq) != 7 && get_row(sq) != 6 && get_col(sq) != 0) bb |= Square(sq + 15);
    if (get_row(sq) != 7 && get_row(sq) != 6 && get_col(sq) != 7) bb |= Square(sq + 17);
    if (get_row(sq) != 7 && get_col(sq) != 6 && get_col(sq) != 7) bb |= Square(sq + 10);
    if (get_row(sq) != 0 && get_col(sq) != 6 && get_col(sq) != 7) bb |= Square(sq - 6);
    if (get_row(sq) != 0 && get_row(sq) != 1 && get_col(sq) != 7) bb |= Square(sq - 15);

    return bb;
}

Bitboard get_bishop_attacks(Square sq, Bitboard blockers) {
    int row, col;
    Bitboard bb = 0ull;
    for (row = get_row(sq) - 1, col = get_col(sq) - 1; row >= 0 && col >= 0; row--, col--) {
        bb |= Square(row * 8 + col);
        if (blockers & Square(row * 8 + col)) break;
    }
    for (row = get_row(sq) + 1, col = get_col(sq) - 1; row <= 7 && col >= 0; row++, col--) {
        bb |= Square(row * 8 + col);
        if (blockers & Square(row * 8 + col)) break;
    }
    for (row = get_row(sq) + 1, col = get_col(sq) + 1; row <= 7 && col <= 7; row++, col++) {
        bb |= Square(row * 8 + col);
        if (blockers & Square(row * 8 + col)) break;
    }
    for (row = get_row(sq) - 1, col = get_col(sq) + 1; row >= 0 && col <= 7; row--, col++) {
        bb |= Square(row * 8 + col);
        if (blockers & Square(row * 8 + col)) break;
    }

    return bb;
}

Bitboard get_rook_attacks(Square sq, Bitboard blockers) {
    int row, col;
    Bitboard bb = 0ull;
    for (row = get_row(sq) - 1, col = get_col(sq); row >= 0; row--) {
        bb |= Square(row * 8 + col);
        if (blockers & Square(row * 8 + col)) break;
    }
    for (row = get_row(sq), col = get_col(sq) - 1; col >= 0; col--) {
        bb |= Square(row * 8 + col);
        if (blockers & Square(row * 8 + col)) break;
    }
    for (row = get_row(sq) + 1, col = get_col(sq); row <= 7; row++) {
        bb |= Square(row * 8 + col);
        if (blockers & Square(row * 8 + col)) break;
    }
    for (row = get_row(sq), col = get_col(sq) + 1; col <= 7; col++) {
        bb |= Square(row * 8 + col);
        if (blockers & Square(row * 8 + col)) break;
    }

    return bb;
}

Bitboard get_king_attacks(Square sq) {
    int row = get_row(sq);
    int col = get_col(sq);

    Bitboard bb = 0ull;

    if (row > 0 && col > 0) bb |= Square(row * 8 + col - 9);
    if (           col > 0) bb |= Square(row * 8 + col - 1);
    if (row < 7 && col > 0) bb |= Square(row * 8 + col + 7);
    if (row < 7           ) bb |= Square(row * 8 + col + 8);
    if (row < 7 && col < 7) bb |= Square(row * 8 + col + 9);
    if (           col < 7) bb |= Square(row * 8 + col + 1);
    if (row > 0 && col < 7) bb |= Square(row * 8 + col - 7);
    if (row > 0           ) bb |= Square(row * 8 + col - 8);

    return bb;
}

void init() {
    for (int i = 0; i < 64; i++) {
        knight_attacks[i] = get_knight_attacks(Square(i));
        king_attacks[i] = get_king_attacks(Square(i));
    }
}

Board::Board() {
    for (int i = 0; i < PIECE_TYPE_ALL; i++)
        by_type[i] = 0ull;
    for (int i = 0; i < COLOR_ALL; i++)
        by_color[i] = 0ull;

    init();
}

void Board::set_piece(Square sq, Piece p) {
    rem_piece(sq);
    if (p != NO_PIECE) {
        set_square(by_color[get_color(p)], sq);
        set_square(by_type[get_type(p)], sq);
    }
}

Piece Board::get_piece(Square sq) const {
    if (get_square(by_type[PAWN], sq)) return make_piece(get_color(sq), PAWN);
    if (get_square(by_type[KNIGHT], sq)) return make_piece(get_color(sq), KNIGHT);
    if (get_square(by_type[BISHOP], sq)) return make_piece(get_color(sq), BISHOP);
    if (get_square(by_type[ROOK], sq)) return make_piece(get_color(sq), ROOK);
    if (get_square(by_type[QUEEN], sq)) return make_piece(get_color(sq), QUEEN);
    if (get_square(by_type[KING], sq)) return make_piece(get_color(sq), KING);

    return NO_PIECE;
}

Board& Board::load_from_fen(const std::string& fen) {
    std::istringstream ss(fen);
    int row = 7;
    int col = 0;
    char cur;

    ss >> std::noskipws;

    // TODO: add error detection
    while ((ss >> cur) && !isspace(cur)) {
        if (cur == '/') { row--; col = 0; }
        else if (cur == 'P') { set_piece(row, col, W_PAWN); col++; }
        else if (cur == 'N') { set_piece(row, col, W_KNIGHT); col++; }
        else if (cur == 'B') { set_piece(row, col, W_BISHOP); col++; }
        else if (cur == 'R') { set_piece(row, col, W_ROOK); col++; }
        else if (cur == 'Q') { set_piece(row, col, W_QUEEN); col++; }
        else if (cur == 'K') { set_piece(row, col, W_KING); col++; }
        else if (cur == 'p') { set_piece(row, col, B_PAWN); col++; }
        else if (cur == 'n') { set_piece(row, col, B_KNIGHT); col++; }
        else if (cur == 'b') { set_piece(row, col, B_BISHOP); col++; }
        else if (cur == 'r') { set_piece(row, col, B_ROOK); col++; }
        else if (cur == 'q') { set_piece(row, col, B_QUEEN); col++; }
        else if (cur == 'k') { set_piece(row, col, B_KING); col++; }
        else { // int
            for (int i = 0; i < cur - '0'; i++) {
                set_piece(row, col, NO_PIECE);
                col++;
            }
        }
    }

    ss >> cur;
    side_to_move = cur == 'w' ? WHITE : BLACK;
    ss >> cur;

    castling_rights.clear_all(); 
    while ((ss >> cur) && !isspace(cur)) {
        if (cur == '-') { ss >> cur; break; }
        else if (cur == 'K') { castling_rights.set_white_can_00(); }
        else if (cur == 'k') { castling_rights.set_black_can_00(); }
        else if (cur == 'Q') { castling_rights.set_white_can_000(); }
        else if (cur == 'q') { castling_rights.set_black_can_000(); }
    }

    ss >> cur;
    if (cur == '-') {
        en_passant_sq = NO_SQ;
    } else {
        col = cur - 'a';
        ss >> cur;
        row = cur - '1';
        en_passant_sq = Square(row * 8 + col);
    }
    ss >> cur;

    ss >> std::skipws >> rule50_half_moves >> move_counter;

    return *this;
}

Board& Board::make_move(Move move) {
    history.push_back(*this);  // Store current board state

    // Check for capture or pawn move (for 50 rule)
    if (get_piece(move.from) == W_PAWN || (!get_piece(~(by_color[WHITE] | by_color[BLACK]), move.to))) rule50_half_moves = 0;
    else rule50_half_moves++;

    Square temp_en_passant_sq;
    // Update En passant square
    if (get_piece(move.from) == W_PAWN && (8 <= move.from) && (move.from <= 15) && (24 <= move.to) && (move.to <= 31))
        temp_en_passant_sq = Square(move.from + 8);
    else if (get_piece(move.from) == B_PAWN && (48 <= move.from) && (move.from <= 55) && (32 <= move.to) && (move.to <= 39))
        temp_en_passant_sq = Square(move.from - 8);
    else
        temp_en_passant_sq = NO_SQ;

    // Castling
    // No need to check for castling rights because otherwise move would
    // be illegal anyways (make_move doesn't check for legality)
    // Also, no need to update rights because one of the next blocks
    // will do so anyways
    // Also, only moves rook because king move is implied on the
    // next block
    if (get_piece(move.from) == W_KING && (move.from == SQ_E1) && (move.to == SQ_G1))
        { set_piece(SQ_F1, W_ROOK); rem_piece(SQ_H1); }
    if (get_piece(move.from) == W_KING && (move.from == SQ_E1) && (move.to == SQ_C1))
        { set_piece(SQ_D1, W_ROOK); rem_piece(SQ_A1); }
    if (get_piece(move.from) == B_KING && (move.from == SQ_E8) && (move.to == SQ_G8))
        { set_piece(SQ_F8, B_ROOK); rem_piece(SQ_H8); }
    if (get_piece(move.from) == B_KING && (move.from == SQ_E8) && (move.to == SQ_C8))
        { set_piece(SQ_D8, B_ROOK); rem_piece(SQ_A8); }

    if (move.promotion == 0) {
        // Check for en passant
        if ((move.to == en_passant_sq) && get_piece(move.from) == W_PAWN) {
            rem_piece(Square(move.to - 8));
            rem_piece(Square(move.to - 8));
        }
        else if ((move.to == en_passant_sq) && get_piece(move.from) == B_PAWN) {
            rem_piece(Square(move.to + 8));
            rem_piece(Square(move.to + 8));
        }

        set_piece(move.to, get_piece(move.from));
    }
    // Check for promotion
    else if (move.promotion == 1) set_piece(move.to, side_to_move == WHITE ? W_KNIGHT : B_KNIGHT);
    else if (move.promotion == 2) set_piece(move.to, side_to_move == WHITE ? W_BISHOP : B_BISHOP);
    else if (move.promotion == 3) set_piece(move.to, side_to_move == WHITE ? W_ROOK : B_ROOK);
    else if (move.promotion == 4) set_piece(move.to, side_to_move == WHITE ? W_QUEEN : B_QUEEN);
    rem_piece(move.from);


    // !! Update !! (NOT CHECK!!) castling rights
    if ((get_piece(SQ_H1) != W_ROOK || get_piece(SQ_E1) != W_KING) && castling_rights.get_white_can_00())
        castling_rights.reset_white_can_00();
    if ((get_piece(SQ_A1) != W_ROOK || get_piece(SQ_E1) != W_KING) && castling_rights.get_white_can_000())
        castling_rights.reset_white_can_000();
    if ((get_piece(SQ_H8) != B_ROOK || get_piece(SQ_E8) != B_KING) && castling_rights.get_black_can_00())
        castling_rights.reset_black_can_00();
    if ((get_piece(SQ_A8) != B_ROOK || get_piece(SQ_E8) != B_KING) && castling_rights.get_black_can_000())
        castling_rights.reset_black_can_000();

    // Increment every time black moves
    move_counter += side_to_move;

    // Flip turn
    side_to_move = Color(1 - int(side_to_move));

    // Set actual ep square
    en_passant_sq = temp_en_passant_sq;

    return *this;
}

void Board::undo_move() {
    if (history.empty()) {
        std::cerr << "Undo move error: no previous state!" << std::endl;
    }

    *this = history.back();  // Restore previous state
}

bool Board::is_square_attacked_by(Square sq, Color side) {
    Bitboard pawn_attackers = 0ull;

    bool res = false;

    if (side == WHITE) {
        pawn_attackers |= Square(sq - 7); 
        pawn_attackers |= Square(sq - 9);
    } else {
        pawn_attackers |= Square(sq + 7); 
        pawn_attackers |= Square(sq + 9); 
    }

    if (pawn_attackers & by_color[side] & by_type[PAWN])
        res = true;

    else if (knight_attacks[sq] & by_color[side] & by_type[KNIGHT])
        res = true;

    else if (king_attacks[sq] & by_color[side] & by_type[KING])
        res = true;

    else if (get_bishop_attacks(sq, by_color[WHITE] | by_color[BLACK]) & by_color[side] & (by_type[BISHOP] | by_type[QUEEN]))
        res = true;

    else if (get_rook_attacks(sq, by_color[WHITE] | by_color[BLACK]) & by_color[side] & (by_type[ROOK] | by_type[QUEEN]))
        res = true;

    return res;
}

// !! ONLY CHECK FOR CHECKS IS THE CASTLING MIDDLE SQUARE !!
MoveList Board::gen_pseudolegal_moves_sq(Square sq) {
    Piece piece = get_piece(sq);
    Bitboard all = by_color[WHITE] | by_color[BLACK];
    Bitboard attacks_bb;
    MoveList res;

    if (piece == W_PAWN) {
        if (~all & (sq + 8)) {
            if (get_row(sq) == 6) {
                res.push_back(Move(sq, Square(sq + 8), 1));
                res.push_back(Move(sq, Square(sq + 8), 2));
                res.push_back(Move(sq, Square(sq + 8), 3));
                res.push_back(Move(sq, Square(sq + 8), 4));
            } else {
                res.push_back(Move(sq, Square(sq + 8)));
            }
            if (get_row(sq) == 1 && (~all & (sq + 16))) {
                res.push_back(Move(sq, Square(sq + 16)));
            }
        }
        if (get_col(sq) != 0 && (by_color[BLACK] & (sq + 7))) {
            if (get_row(sq) == 6) {
                res.push_back(Move(sq, Square(sq + 7), 1));
                res.push_back(Move(sq, Square(sq + 7), 2));
                res.push_back(Move(sq, Square(sq + 7), 3));
                res.push_back(Move(sq, Square(sq + 7), 4));
            } else {
                res.push_back(Move(sq, Square(sq + 7)));
            }
        }
        if (get_col(sq) != 7 && (by_color[BLACK] & (sq + 9))) {
            if (get_row(sq) == 6) {
                res.push_back(Move(sq, Square(sq + 9), 1));
                res.push_back(Move(sq, Square(sq + 9), 2));
                res.push_back(Move(sq, Square(sq + 9), 3));
                res.push_back(Move(sq, Square(sq + 9), 4));
            } else {
                res.push_back(Move(sq, Square(sq + 9)));
            }
        }
        if (sq + 7 == en_passant_sq && get_col(sq) != 0)
            res.push_back(Move(sq, Square(sq + 7)));
        if (sq + 9 == en_passant_sq && get_col(sq) != 7)
            res.push_back(Move(sq, Square(sq + 9)));
    }
    else if (piece == B_PAWN) {
        if (~all & (sq - 8)) {
            if (get_row(sq) == 1) {
                res.push_back(Move(sq, Square(sq - 8), 1));
                res.push_back(Move(sq, Square(sq - 8), 2));
                res.push_back(Move(sq, Square(sq - 8), 3));
                res.push_back(Move(sq, Square(sq - 8), 4));
            } else {
                res.push_back(Move(sq, Square(sq - 8)));
            }
            if (get_row(sq) == 6 && get_piece(sq - 16) == NO_PIECE) {
                res.push_back(Move(sq, Square(sq - 16)));
            }
        }
        if (get_col(sq) != 7 && (by_color[WHITE] & (sq - 7))) {
            if (get_row(sq) == 1) {
                res.push_back(Move(sq, Square(sq - 7), 1));
                res.push_back(Move(sq, Square(sq - 7), 2));
                res.push_back(Move(sq, Square(sq - 7), 3));
                res.push_back(Move(sq, Square(sq - 7), 4));
            } else {
                res.push_back(Move(sq, Square(sq - 7)));
            }
        }
        if (get_col(sq) != 0 && (by_color[WHITE] & (sq - 9))) {
            if (get_row(sq) == 1) {
                res.push_back(Move(sq, Square(sq - 9), 1));
                res.push_back(Move(sq, Square(sq - 9), 2));
                res.push_back(Move(sq, Square(sq - 9), 3));
                res.push_back(Move(sq, Square(sq - 9), 4));
            } else {
                res.push_back(Move(sq, Square(sq - 9)));
            }
        }
        if (sq - 7 == en_passant_sq && get_col(sq) != 7)
            res.push_back(Move(sq, Square(sq - 7)));
        if (sq - 9 == en_passant_sq && get_col(sq) != 0)
            res.push_back(Move(sq, Square(sq - 9)));
    }
    else if (piece == W_KNIGHT) {
        attacks_bb = knight_attacks[sq];
        attacks_bb &= ~by_color[WHITE];

        while (attacks_bb) {
            res.push_back(Move(sq, Square(lsb(attacks_bb))));
            attacks_bb &= attacks_bb - 1;
        }
    }
    else if (piece == B_KNIGHT) {
        attacks_bb = knight_attacks[sq];
        attacks_bb &= ~by_color[BLACK];

        while (attacks_bb) {
            res.push_back(Move(sq, Square(lsb(attacks_bb))));
            attacks_bb &= attacks_bb - 1;
        }
    }
    else if (piece == W_BISHOP) {
        attacks_bb = get_bishop_attacks(sq, all);
        attacks_bb &= ~by_color[WHITE];

        while (attacks_bb) {
            res.push_back(Move(sq, Square(lsb(attacks_bb))));
            attacks_bb &= attacks_bb - 1;
        }
    }
    else if (piece == B_BISHOP) {
        attacks_bb = get_bishop_attacks(sq, all);
        attacks_bb &= ~by_color[BLACK];

        while (attacks_bb) {
            res.push_back(Move(sq, Square(lsb(attacks_bb))));
            attacks_bb &= attacks_bb - 1;
        }
    }
    else if (piece == W_ROOK) {
        attacks_bb = get_rook_attacks(sq, all);
        attacks_bb &= ~by_color[WHITE];

        while (attacks_bb) {
            res.push_back(Move(sq, Square(lsb(attacks_bb))));
            attacks_bb &= attacks_bb - 1;
        }
    }
    else if (piece == B_ROOK) {
        attacks_bb = get_rook_attacks(sq, all);
        attacks_bb &= ~by_color[BLACK];

        while (attacks_bb) {
            res.push_back(Move(sq, Square(lsb(attacks_bb))));
            attacks_bb &= attacks_bb - 1;
        }
    }
    else if (piece == W_QUEEN) {
        attacks_bb = get_bishop_attacks(sq, all);
        attacks_bb |= get_rook_attacks(sq, all);
        attacks_bb &= ~by_color[WHITE];

        while (attacks_bb) {
            res.push_back(Move(sq, Square(lsb(attacks_bb))));
            attacks_bb &= attacks_bb - 1;
        }
    }
    else if (piece == B_QUEEN) {
        attacks_bb = get_bishop_attacks(sq, all);
        attacks_bb |= get_rook_attacks(sq, all);
        attacks_bb &= ~by_color[BLACK];

        while (attacks_bb) {
            res.push_back(Move(sq, Square(lsb(attacks_bb))));
            attacks_bb &= attacks_bb - 1;
        }
    }
    else if (piece == W_KING) {
        attacks_bb = king_attacks[sq];
        attacks_bb &= ~by_color[WHITE];

        while (attacks_bb) {
            res.push_back(Move(sq, Square(lsb(attacks_bb))));
            attacks_bb &= attacks_bb - 1;
        }

        // Castling
        // No need to check for destination because legal move detection will do so for us
        if (castling_rights.get_white_can_00() && (~all & SQ_F1) && (~all & SQ_G1) && !is_square_attacked_by(SQ_F1, BLACK) && !is_square_attacked_by(SQ_E1, BLACK))
            res.push_back(Move(SQ_E1, SQ_G1));
        if (castling_rights.get_white_can_000() && (~all & SQ_D1) && (~all & SQ_C1) && (~all & SQ_B1) && !is_square_attacked_by(SQ_D1, BLACK) && !is_square_attacked_by(SQ_E1, BLACK))
            res.push_back(Move(SQ_E1, SQ_C1));
    }
    else if (piece == B_KING) {
        attacks_bb = king_attacks[sq];
        attacks_bb &= ~by_color[BLACK];

        while (attacks_bb) {
            res.push_back(Move(sq, Square(lsb(attacks_bb))));
            attacks_bb &= attacks_bb - 1;
        }

        // Castling
        if (castling_rights.get_black_can_00() && (~all & SQ_F8) && (~all & SQ_G8) && !is_square_attacked_by(SQ_F8, WHITE) && !is_square_attacked_by(SQ_E8, WHITE))
            res.push_back(Move(SQ_E8, SQ_G8));
        if (castling_rights.get_black_can_000() && (~all & SQ_D8) && (~all & SQ_C8) && (~all & SQ_B8) && !is_square_attacked_by(SQ_D8, WHITE) && !is_square_attacked_by(SQ_E8, WHITE))
            res.push_back(Move(SQ_E8, SQ_C8));
    }

    return res;
}

MoveList Board::gen_pseudolegal_moves() {
    MoveList res;

    for (int i = 0; i < 64; i++) {
        for (Move move : gen_pseudolegal_moves_sq(Square(i))) {
            if (side_to_move == WHITE ? is_white(Square(i)) : is_black(Square(i)))
                res.push_back(move);
        }
    }

    return res;
}

// TODO: implement board history
MoveList Board::gen_legal_moves() {
    MoveList res;
    Color safe_king_color = side_to_move;

    for (Move move : gen_pseudolegal_moves()) {
        make_move(move);
        if (!is_square_attacked_by(Square(lsb(by_type[KING] & by_color[safe_king_color])), side_to_move))
            res.push_back(move);
        undo_move();
    }

    return res;
}


std::ostream& operator<<(std::ostream& os, Board board) {
    std::string piece_to_char = "PNBRQKpnbrqkX ";
    std::string int_to_char = " 1234567";

    os << "  +---+---+---+---+---+---+---+---+\n8 ";

    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col <= 7; col++) {
            os << "| " << piece_to_char[board.get_piece(row, col)] << " ";
        }
        os << "|\n  +---+---+---+---+---+---+---+---+\n" << int_to_char[row] << " ";
    }

    os << "  a   b   c   d   e   f   g   h\n";

    return os;
}

std::ostream& operator<<(std::ostream& os, Move move) {
    std::string prom_table = " nbrq";
    os << std::string(1, (move.from % 8) + 'a') << std::string(1, (int)(move.from / 8) + '1')
       << std::string(1, (move.to   % 8) + 'a') << std::string(1, (int)(move.to   / 8) + '1') << prom_table[move.promotion];

    return os;
}
