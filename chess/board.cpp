#include "board.h"
#include "magic.h"

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

    init_magics(false);
    // print_magics();
}

Board::Board() {
    for (int i = 0; i < PIECE_TYPE_ALL; i++)
        by_type[i] = 0ull;
    for (int i = 0; i < COLOR_ALL; i++)
        by_color[i] = 0ull;
    for (int i = 0; i < SQ_ALL; i++)
        board[i] = NO_PIECE;
}

void Board::rem_piece(Square sq) {
    if (get_piece(sq) != NO_PIECE) {
        clear_square(by_color[get_color(get_piece(sq))], sq);
        clear_square(by_type[get_type(get_piece(sq))], sq);
    }
    board[sq] = NO_PIECE;
}

void Board::set_piece(Square sq, Piece p) {
    rem_piece(sq);
    if (p != NO_PIECE) {
        set_square(by_color[get_color(p)], sq);
        set_square(by_type[get_type(p)], sq);
    }    
    board[sq] = p;
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
    cur_undo_index++;
    UndoInfo u;
    u.captured_piece = NO_PIECE;
    u.en_passant = false;
    u.promotion = true;
    u.en_passant_sq = en_passant_sq;
    u.rule50_half_moves = rule50_half_moves;
    u.castling_rights = castling_rights;

    // Check for capture or pawn move (for 50 rule)
    if (is_sq_piece(move.from, W_PAWN))
        rule50_half_moves = 0;
    if ((by_color[WHITE] | by_color[BLACK]) & move.to) {
        rule50_half_moves = 0;
        u.captured_piece = get_piece(move.to);
    }
    else {
        rule50_half_moves++;
    }

    Square temp_en_passant_sq;
    // Update En passant square
    if (is_sq_piece(move.from, W_PAWN) && (8 <= move.from) && (move.from <= 15) && (24 <= move.to) && (move.to <= 31)) {
        temp_en_passant_sq = Square(move.from + 8);
    }
    else if (is_sq_piece(move.from, B_PAWN) && (48 <= move.from) && (move.from <= 55) && (32 <= move.to) && (move.to <= 39)) {
        temp_en_passant_sq = Square(move.from - 8);
    }
    else
        temp_en_passant_sq = NO_SQ;

    // Castling
    // No need to check for castling rights because otherwise move would
    // be illegal anyways (make_move doesn't check for legality)
    // Also, no need to update rights because one of the next blocks
    // will do so anyways
    // Also, only moves rook because king move is implied on the
    // next block
    if (is_sq_piece(move.from, W_KING) && (move.from == SQ_E1) && (move.to == SQ_G1))
        { set_piece(SQ_F1, W_ROOK); rem_piece(SQ_H1); }
    if (is_sq_piece(move.from, W_KING) && (move.from == SQ_E1) && (move.to == SQ_C1))
        { set_piece(SQ_D1, W_ROOK); rem_piece(SQ_A1); }
    if (is_sq_piece(move.from, B_KING) && (move.from == SQ_E8) && (move.to == SQ_G8))
        { set_piece(SQ_F8, B_ROOK); rem_piece(SQ_H8); }
    if (is_sq_piece(move.from, B_KING) && (move.from == SQ_E8) && (move.to == SQ_C8))
        { set_piece(SQ_D8, B_ROOK); rem_piece(SQ_A8); }

    if (move.promotion == 0) {
        u.promotion = false;
        // Check for en passant
        if ((move.to == en_passant_sq) && is_sq_piece(move.from, W_PAWN)) {
            u.en_passant = true;
            rem_piece(Square(move.to - 8));
        }
        else if ((move.to == en_passant_sq) && is_sq_piece(move.from, B_PAWN)) {
            u.en_passant = true;
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
    if ((!is_sq_piece(SQ_H1, W_ROOK) || !is_sq_piece(SQ_E1, W_KING)) && castling_rights.get_white_can_00()) {
        castling_rights.reset_white_can_00();
    }
    if ((!is_sq_piece(SQ_A1, W_ROOK) || !is_sq_piece(SQ_E1, W_KING)) && castling_rights.get_white_can_000()) {
        castling_rights.reset_white_can_000();
    }
    if ((!is_sq_piece(SQ_H8, B_ROOK) || !is_sq_piece(SQ_E8, B_KING)) && castling_rights.get_black_can_00()) {
        castling_rights.reset_black_can_00();
    }
    if ((!is_sq_piece(SQ_A8, B_ROOK) || !is_sq_piece(SQ_E8, B_KING)) && castling_rights.get_black_can_000()) {
        castling_rights.reset_black_can_000();
    }

    // Increment every time black moves
    move_counter += side_to_move;

    // Flip turn
    side_to_move = Color(1 - int(side_to_move));

    // Set actual ep square
    en_passant_sq = temp_en_passant_sq;

    undo_info[cur_undo_index] = u;

    return *this;
}

void Board::undo_move(Move move) {
    UndoInfo u = undo_info[cur_undo_index];
    cur_undo_index--;
    side_to_move = Color(1 - side_to_move);
    en_passant_sq = u.en_passant_sq;
    castling_rights = u.castling_rights;
    rule50_half_moves = u.rule50_half_moves;

    if (side_to_move == BLACK) { move_counter--; }

    if (!u.promotion) {
        set_piece(move.from, get_piece(move.to));
        set_piece(move.to, u.captured_piece);

        if (u.en_passant) {
            if (side_to_move == WHITE)
                set_piece(move.to - 8, B_PAWN);
            else
                set_piece(move.to + 8, W_PAWN);
        }
    } else { 
        if (side_to_move == WHITE) {
            set_piece(move.from, W_PAWN);
            set_piece(move.to, u.captured_piece);
        } else {
            set_piece(move.from, B_PAWN);
            set_piece(move.to, u.captured_piece);
        }
    }

    if (get_piece(move.from) == W_KING && move.from == SQ_E1 && move.to == SQ_G1) {
        set_piece(SQ_H1, W_ROOK);
        rem_piece(SQ_F1);
    } else if (get_piece(move.from) == W_KING && move.from == SQ_E1 && move.to == SQ_C1) {
        set_piece(SQ_A1, W_ROOK);
        rem_piece(SQ_D1);
    } else if (get_piece(move.from) == B_KING && move.from == SQ_E8 && move.to == SQ_G8) {
        set_piece(SQ_H8, B_ROOK);
        rem_piece(SQ_F8);
    } else if (get_piece(move.from) == B_KING && move.from == SQ_E8 && move.to == SQ_C8) {
        set_piece(SQ_A8, B_ROOK);
        rem_piece(SQ_D8);
    }
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

    else if (fast_bishop_attacks(sq, by_color[WHITE] | by_color[BLACK]) & by_color[side] & (by_type[BISHOP] | by_type[QUEEN]))
        res = true;

    else if (fast_rook_attacks(sq, by_color[WHITE] | by_color[BLACK]) & by_color[side] & (by_type[ROOK] | by_type[QUEEN]))
        res = true;

    return res;
}

void make_promotion(Square f, Square d, MoveList& m) {
    m.push_back(Move(f, d, 1));
    m.push_back(Move(f, d, 2));
    m.push_back(Move(f, d, 3));
    m.push_back(Move(f, d, 4));
}

void moves_from_attack_bb(Square sq, Bitboard bb, MoveList& m) {
    while (bb) {
        m.push_back(Move(sq, Square(lsb(bb))));
        bb &= bb - 1;
    }
}

MoveList Board::gen_pseudolegal_moves() {
    MoveList res;

    Color us = side_to_move;
    Color them = Color(1 - side_to_move);

    Direction up       = (us == WHITE ? NORTH : SOUTH);
    Direction up_right  = (us == WHITE ? NORTH_EAST : SOUTH_WEST);
    Direction up_left   = (us == WHITE ? NORTH_WEST : SOUTH_EAST);

    Bitboard all = by_color[WHITE] | by_color[BLACK];
    Bitboard allies = by_color[us];
    Bitboard enemies = by_color[them];
    Bitboard our_pawns = by_type[PAWN] & by_color[us];

    Bitboard rel_rank_7 = us == WHITE ? Rank7 : Rank2;
    Bitboard rel_rank_3 = us == WHITE ? Rank3 : Rank6;
    Bitboard rel_not_rank_7 = us == WHITE ? ~Rank7 : ~Rank2;

    Square to;
    Square from;

    // Pawns regular moves
    Bitboard pawn_attacks_1 = shift(our_pawns & rel_not_rank_7, up) & ~all;
    Bitboard pawn_attacks_2 = shift(pawn_attacks_1 & rel_rank_3, up) & ~all;

    while (pawn_attacks_1) {
        to = pop_lsb(pawn_attacks_1);
        res.push_back(Move(to - up, to));
    }

    while (pawn_attacks_2) {
        to = pop_lsb(pawn_attacks_2);
        res.push_back(Move(to - up - up, to));
    }

    // Captures + en passant
    pawn_attacks_1 = shift(our_pawns & rel_not_rank_7, up_right) & (enemies | square_to_bb(en_passant_sq));
    pawn_attacks_2 = shift(our_pawns & rel_not_rank_7, up_left) & (enemies | square_to_bb(en_passant_sq)); 

    while (pawn_attacks_1) {
        to = pop_lsb(pawn_attacks_1);
        res.push_back(Move(to - up_right, to));
    }

    while (pawn_attacks_2) {
        to = pop_lsb(pawn_attacks_2);
        res.push_back(Move(to - up_left, to));
    }

    // Promotions
    pawn_attacks_1 = shift(our_pawns & rel_rank_7, up) & ~all;

    while (pawn_attacks_1) {
        to = pop_lsb(pawn_attacks_1);
        make_promotion(to - up, to, res);
    }

    pawn_attacks_1 = shift(our_pawns & rel_rank_7, up_right) & enemies;
    pawn_attacks_2 = shift(our_pawns & rel_rank_7, up_left) & enemies; 

    while (pawn_attacks_1) {
        to = pop_lsb(pawn_attacks_1);
        make_promotion(to - up_right, to, res);
    }

    while (pawn_attacks_2) {
        to = pop_lsb(pawn_attacks_2);
        make_promotion(to - up_left, to, res);
    }

    // Knight
    Bitboard our_knights = by_type[KNIGHT] & by_color[us];

    while (our_knights) {
        from = pop_lsb(our_knights);
        moves_from_attack_bb(from, knight_attacks[from] & ~allies, res);
    }

    // Bishops
    Bitboard our_bishops = by_type[BISHOP] & by_color[us];


    while (our_bishops) {
        from = pop_lsb(our_bishops);
        moves_from_attack_bb(from, fast_bishop_attacks(from, all) & ~allies, res);
    }

    // Rooks
    Bitboard our_rooks = by_type[ROOK] & by_color[us];

    while (our_rooks) {
        from = pop_lsb(our_rooks);
        moves_from_attack_bb(from, fast_rook_attacks(from, all) & ~allies, res);
    }

    // Queens
    Bitboard our_queens = by_type[QUEEN] & by_color[us];

    while (our_queens) {
        from = pop_lsb(our_queens);
        moves_from_attack_bb(from, (fast_rook_attacks(from, all) | fast_bishop_attacks(from, all)) & ~allies, res);
    }

    // King
    Bitboard our_king = by_type[KING] & by_color[us];

    from = pop_lsb(our_king);
    moves_from_attack_bb(from, king_attacks[from] & ~allies, res);

    if (side_to_move == WHITE) {
        if (castling_rights.get_white_can_00() && (~all & SQ_F1) && (~all & SQ_G1) && !is_square_attacked_by(SQ_F1, BLACK) && !is_square_attacked_by(SQ_E1, BLACK))
            res.push_back(Move(SQ_E1, SQ_G1));
        if (castling_rights.get_white_can_000() && (~all & SQ_D1) && (~all & SQ_C1) && (~all & SQ_B1) && !is_square_attacked_by(SQ_D1, BLACK) && !is_square_attacked_by(SQ_E1, BLACK))
            res.push_back(Move(SQ_E1, SQ_C1));
    } else {
        if (castling_rights.get_black_can_00() && (~all & SQ_F8) && (~all & SQ_G8) && !is_square_attacked_by(SQ_F8, WHITE) && !is_square_attacked_by(SQ_E8, WHITE))
            res.push_back(Move(SQ_E8, SQ_G8));
        if (castling_rights.get_black_can_000() && (~all & SQ_D8) && (~all & SQ_C8) && (~all & SQ_B8) && !is_square_attacked_by(SQ_D8, WHITE) && !is_square_attacked_by(SQ_E8, WHITE))
            res.push_back(Move(SQ_E8, SQ_C8));
    }

    return res;
}

MoveList Board::gen_legal_moves() {
    MoveList res;
    Color safe_king_color = side_to_move;

    for (Move move : gen_pseudolegal_moves()) {
        make_move(move);
        if (!is_square_attacked_by(Square(lsb(by_type[KING] & by_color[safe_king_color])), side_to_move))
            res.push_back(move);
        undo_move(move);
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
       << std::string(1, (move.to   % 8) + 'a') << std::string(1, (int)(move.to   / 8) + '1');

    if (move.promotion)
        os << prom_table[move.promotion];

    return os;
}

bool operator==(UndoInfo u, UndoInfo o) {
    bool res = true;

    res &= u.captured_piece == o.captured_piece;
    res &= u.en_passant == o.en_passant;
    res &= u.en_passant_sq == o.en_passant_sq;
    res &= u.rule50_half_moves == o.rule50_half_moves;
    res &= u.castling_rights.get_data() == o.castling_rights.get_data();
    res &= u.promotion == o.promotion;

    return res;
}