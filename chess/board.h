#pragma once

#include "bitboard.h"

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

enum Color {
    WHITE,
    BLACK,
    COLOR_ALL,
};

enum PieceType {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, PIECE_TYPE_ALL, NO_PIECE_TYPE,
};

enum Piece {
    W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
    PIECE_ALL, NO_PIECE,
};

struct Move {
    Square from;
    Square to;
    int promotion = 0; // 1-4 n-q;
};

using MoveList = std::vector<Move>;

class CastlingRights {
public:
    CastlingRights() : data(0) {}
    CastlingRights(int d) : data(d) {}
    CastlingRights& operator=(const CastlingRights&) = default;

    int get_data() const { return data; }
    void clear_all() { data = 0; }

    void set_white_can_00() { data |= 1; }
    void set_white_can_000() { data |= 2; }
    void set_black_can_00() { data |= 4; }
    void set_black_can_000() { data |= 8; }

    void reset_white_can_00() { data &= ~1; }
    void reset_white_can_000() { data &= ~2; }
    void reset_black_can_00() { data &= ~4; }
    void reset_black_can_000() { data &= ~8; }

    bool get_white_can_00() { return data & 1; }
    bool get_white_can_000() { return data & 2; }
    bool get_black_can_00() { return data & 4; }
    bool get_black_can_000() { return data & 8; }

private:
    int data;
};

struct UndoInfo {
    Piece captured_piece = NO_PIECE; // !!! NOPIECE FOR ENPASSANT !!!
    bool en_passant = false;
    Square en_passant_sq = NO_SQ;
    int rule50_half_moves = 0;
    CastlingRights castling_rights;
    bool promotion = false;
};

class Board {
public:
    Board();
    Board(const Board& other) = default;
    Board& operator=(const Board&) = default;

    Board& load_from_fen(const std::string& fen);

    void set_piece(int row, int col, Piece p) { set_piece(Square(row * 8 + col), p); }
    void set_piece(Square sq, Piece p);
    void rem_piece(int row, int col) { rem_piece(Square(row * 8 + col)); }
    void rem_piece(Square sq);
    Piece get_piece(int row, int col) const { return get_piece(Square(row * 8 + col)); }
    Piece get_piece(Square sq) const { return board[sq]; };
    static constexpr Piece make_piece(Color c, PieceType p) { return Piece(c * 6 + p); }
    static constexpr PieceType get_type(Piece p) { return PieceType(p % 6); }
    static constexpr Color get_color(Piece p) { return is_white(p) ? WHITE : BLACK; }
    static constexpr bool is_white(Piece p) { return p < 6; }
    static constexpr bool is_black(Piece p) { return (p >= 6) && (p <= 11); }
    PieceType get_type(Square sq) const { return get_type(get_piece(sq)); }
    Color get_color(Square sq) const { return is_white(sq) ? WHITE : BLACK; }
    bool is_white(Square sq) const { return get_square(by_color[WHITE], sq) ? true : false; }
    bool is_black(Square sq) const { return get_square(by_color[BLACK], sq) ? true : false; }
    bool is_white(int row, int col) const { return is_white(Square(row * 8 + col)); }
    bool is_black(int row, int col) const { return is_black(Square(row * 8 + col)); }
    Board& make_move(Move move);
    void undo_move(Move move);

    bool is_sq_piece(Square sq, Piece piece) const { return by_color[get_color(piece)] & by_type[get_type(piece)] & sq; }

    bool is_square_attacked_by(Square sq, Color side);

    MoveList gen_pseudolegal_moves();
    MoveList gen_legal_moves();

private:
    // Useful for undoing moves
    UndoInfo undo_info[1024];
    int cur_undo_index = -1;

    // Bitboards
    Bitboard by_color[COLOR_ALL];
    Bitboard by_type[PIECE_TYPE_ALL];

    // Board
    Piece board[SQ_ALL];

    // Loaded in from fen string
    Color side_to_move;
    Square en_passant_sq;
    int rule50_half_moves;
    int move_counter;
    CastlingRights castling_rights; // All castling rights
};

std::ostream& operator<<(std::ostream& os, Board board);

std::ostream& operator<<(std::ostream& os, Move move);

Bitboard get_bishop_attacks(Square sq, Bitboard blockers);
Bitboard get_rook_attacks(Square sq, Bitboard blockers);

void init();
