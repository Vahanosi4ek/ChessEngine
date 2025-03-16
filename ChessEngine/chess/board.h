#pragma once

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

enum Square {
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_ALL, NO_SQ,
};

constexpr bool is_ok(Square sq) { return (SQ_A1 <= sq) && (sq <= SQ_H8); }

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

class Board {
public:
    Board() {};

    Board& load_from_fen(const std::string& fen);

    void set_piece(int row, int col, Piece p) { pieces[row * 8 + col] = p; }
    void set_piece(Square sq, Piece p) { pieces[int(sq)] = p; }
    Piece get_piece(int row, int col) const { return pieces[row * 8 + col]; }
    Piece get_piece(Square sq) const { return pieces[int(sq)]; }
    bool is_white(Piece p) const { return p < 6; }
    bool is_white(Square sq) const { return pieces[sq] < 6; }
    bool is_white(int row, int col) const { return is_white(Square(row * 8 + col)); }
    bool is_black(Piece p) const { return (p >= 6) && (p <= 11); }
    bool is_black(Square sq) const { return (pieces[sq] >= 6) && (pieces[sq] <= 11); }
    bool is_black(int row, int col) const { return is_black(Square(row * 8 + col)); }

    std::vector<int> get_indices(Piece p);
    Board& make_move(Move move);

    bool is_square_attacked_by(Square sq, Color side);

    MoveList gen_pseudolegal_moves_sq(Square sq);
    MoveList gen_pseudolegal_moves();
    MoveList gen_legal_moves();

private:
    // Loaded in from fen string
    Piece pieces[SQ_ALL];
    Color side_to_move;
    Square en_passant_sq;
    int rule50_half_moves;
    int move_counter;
    CastlingRights castling_rights; // All rights
};

std::ostream& operator<<(std::ostream& os, Board board);

std::ostream& operator<<(std::ostream& os, Move move);
