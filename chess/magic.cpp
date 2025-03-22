#include "magic.h"
#include "board.h"

#include <random>

Bitboard rook_attacks[64][4096];
Bitboard bishop_attacks[64][512];

SMagic m_bishop_table[64];
SMagic m_rook_table  [64];

const Bitboard bishop_magics[64] = {
    2450526799455600804ull, // set this to 0 to regen magics
    100490965273411712ull,
    6055239232716013880ull,
    144291127384604684ull,
    4755890013980525568ull,
    23244294313771264ull,
    288237008659219456ull,
    4901086412658835524ull,
    1152921989955191553ull,
    6341085868731670630ull,
    580966605949796624ull,
    1155349366169407617ull,
    396892981631647744ull,
    12386016757571584ull,
    18015018060767362ull,
    9296067625250471968ull,
    74872412559114248ull,
    1738970617193038593ull,
    2630249663374312576ull,
    72127980029364224ull,
    6917599413584036864ull,
    3459908351692132368ull,
    2307039283250209313ull,
    145292765063819392ull,
    13827458500136968ull,
    10376458683491029024ull,
    25060352496929028ull,
    1126037412946048ull,
    4614500836947611648ull,
    577587753400680960ull,
    225215457873903632ull,
    13871105011993302152ull,
    2342085283825082440ull,
    158232951803365392ull,
    4591612114535456ull,
    9570427307557120ull,
    3512807988521897984ull,
    36169779324653568ull,
    4623160659281483268ull,
    2311481649512057984ull,
    54065255084461716ull,
    5333407695510241408ull,
    145522631947665920ull,
    1768788770896871712ull,
    1100602321536ull,
    449030253462144ull,
    2260682082877537ull,
    20266276724146692ull,
    1152974315545690664ull,
    36105797227806720ull,
    90108277573026312ull,
    2278463004248105ull,
    9904404826625ull,
    14411589245116882945ull,
    11605846814711971840ull,
    4574346333694242ull,
    506734158384464385ull,
    432346668572083736ull,
    79255048292448ull,
    9511612309728927872ull,
    26388950320144ull,
    1298202999708991512ull,
    54044305444077856ull,
    580999813464097024ull,
};

const Bitboard rook_magics[64] = {
    756607761056301088ull,
    72066665017254000ull,
    9331463376250900480ull,
    72620685993707520ull,
    72060012138334208ull,
    2449977988641763592ull,
    2310370265822920768ull,
    612489824812663040ull,
    1891934056233304096ull,
    12781906610176ull,
    35187728711685ull,
    562984480942144ull,
    12948261379805196ull,
    290482485337399812ull,
    9224009797152800898ull,
    7107252892762144ull,
    1153839047061307428ull,
    9295993714721423616ull,
    18718120848916736ull,
    7782255623936344128ull,
    4648277800055472384ull,
    5197299690172874816ull,
    657543180731826184ull,
    2342296217723342913ull,
    10378863379375751168ull,
    36559130738832ull,
    577604795226390593ull,
    1298165066759210048ull,
    587724151565977600ull,
    937316071164347526ull,
    108091614282383616ull,
    9223376470871916672ull,
    9236885103620128801ull,
    8815555642372ull,
    18086970849230856ull,
    1441503793786258816ull,
    2324138908553577488ull,
    4325777862441304243ull,
    9511622206644298885ull,
    283676151644832ull,
    1706485553831936ull,
    45617088303747080ull,
    1740087102185410560ull,
    1297107766338519050ull,
    3377979564492936ull,
    549823972544ull,
    9313470692563627136ull,
    4611687129083412484ull,
    72128168951353352ull,
    614882363919065120ull,
    29273440529679392ull,
    12695658346892493312ull,
    144326448927342656ull,
    2255098417906952ull,
    45117360150938752ull,
    2199560161296ull,
    311163938153025ull,
    14989149715139938561ull,
    60808224287244930ull,
    297272873863612421ull,
    72215945191882753ull,
    288235427205480465ull,
    76566159189738497ull,
    9223390745774310434ull,
};

Bitboard random_bb() {
    Bitboard u1, u2, u3, u4;
    u1 = (Bitboard)(random()) & 0xFFFF; u2 = (Bitboard)(random()) & 0xFFFF;
    u3 = (Bitboard)(random()) & 0xFFFF; u4 = (Bitboard)(random()) & 0xFFFF;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

Bitboard random_magic_bb() {
    return random_bb() & random_bb() & random_bb();
}

Bitboard bmask(Square sq) {
    int row, col;
    Bitboard bb = 0ull;
    for (row = get_row(sq) - 1, col = get_col(sq) - 1; row > 0 && col > 0; row--, col--)
        bb |= Square(row * 8 + col);
    for (row = get_row(sq) + 1, col = get_col(sq) - 1; row < 7 && col > 0; row++, col--)
        bb |= Square(row * 8 + col);
    for (row = get_row(sq) + 1, col = get_col(sq) + 1; row < 7 && col < 7; row++, col++)
        bb |= Square(row * 8 + col);
    for (row = get_row(sq) - 1, col = get_col(sq) + 1; row > 0 && col < 7; row--, col++)
        bb |= Square(row * 8 + col);

    return bb;
}

Bitboard rmask(Square sq) {
    int row, col;
    Bitboard bb = 0ull;
    for (row = get_row(sq) - 1, col = get_col(sq); row > 0; row--)
        bb |= Square(row * 8 + col);
    for (row = get_row(sq), col = get_col(sq) - 1; col > 0; col--)
        bb |= Square(row * 8 + col);
    for (row = get_row(sq) + 1, col = get_col(sq); row < 7; row++)
        bb |= Square(row * 8 + col);
    for (row = get_row(sq), col = get_col(sq) + 1; col < 7; col++)
        bb |= Square(row * 8 + col);

    return bb;
}

Bitboard transform(Bitboard bb, Bitboard magic, int bits) {
    return ((bb * magic) >> (64 - bits));
}

Bitboard index_to_occ(int index, Bitboard mask) {
    int size = count(mask);
    int i, j;
    Bitboard res = 0ull;

    for (i = 0; i < size; i++) {
        j = pop_lsb(mask);
        if (index & (1 << i)) res |= 1ull << j;
    }

    return res;
}

Bitboard is_magic_valid(Square sq, Bitboard magic, int bits, bool bishop) {
    Bitboard mask = bishop ? bmask(sq) : rmask(sq);

    Bitboard used[4096];
    Bitboard attacks[4096];
    Bitboard occupancies[4096];

    int index;

    for (int i = 0; i < (1 << bits); i++) {
        occupancies[i] = index_to_occ(i, mask);
        attacks[i] = bishop ? get_bishop_attacks(sq, occupancies[i]) : get_rook_attacks(sq, occupancies[i]);
    }

    for (int i = 0; i < (1 << bits); i++)
        used[i] = 0ull;

    for (int i = 0; i < (1 << bits); i++) {
        index = transform(occupancies[i], magic, bits);
        if (used[index] == 0ull) used[index] = attacks[i];
        else if (used[index] != attacks[i]) return false;
    }

    return true;
}

Bitboard find_magic(Square sq, int bits, bool bishop) {
    Bitboard magic;
    for (int i = 0; i < 10000000; i++) {
        magic = random_magic_bb();
        if (is_magic_valid(sq, magic, bits, bishop)) return magic;
    }

    std::cout << "Magic not found" << std::endl;
    return 0ull;
}

void init_magics(bool gen_magics) {
    Bitboard mask;
    std::cout << "Initializing bishop magics..." << std::endl;
    for (int sq = 0; sq < 64; sq++) {
        mask = bmask(Square(sq));
        std::cout << "Bishop square " << sq << std::endl;
        // TODO: Impl improving shifts
        m_bishop_table[sq].mask = mask;
        if (gen_magics)
            m_bishop_table[sq].magic = find_magic(Square(sq), 9, 1);
        else
            m_bishop_table[sq].magic = bishop_magics[sq];
        m_bishop_table[sq].shift = 64 - 9;
    }
    std::cout << "Initializing rook magics..." << std::endl;
    for (int sq = 0; sq < 64; sq++) {
        mask = rmask(Square(sq));
        std::cout << "Rook square " << sq << std::endl;
        // TODO: Impl improving shifts
        m_rook_table[sq].mask = mask;
        if (gen_magics)
            m_rook_table[sq].magic = find_magic(Square(sq), 12, 0);
        else
            m_rook_table[sq].magic = rook_magics[sq];
        m_rook_table[sq].shift = 64 - 12;
    }

    std::cout << "Magics found..." << std::endl;
    std::cout << "Filling attack tables..." << std::endl;

    Bitboard index;
    Bitboard occ;

    for (int sq = 0; sq < 64; sq++) {
        for (int occ_index = 0; occ_index < (1 << 9); occ_index++) {
            occ = index_to_occ(occ_index, m_bishop_table[sq].mask);
            index = transform(occ, m_bishop_table[sq].magic, 9);
            bishop_attacks[sq][index] = get_bishop_attacks(Square(sq), occ);
        }
    }

    for (int sq = 0; sq < 64; sq++) {
        for (int occ_index = 0; occ_index < (1 << 12); occ_index++) {
            occ = index_to_occ(occ_index, m_rook_table[sq].mask);
            index = transform(occ, m_rook_table[sq].magic, 12);
            rook_attacks[sq][index] = get_rook_attacks(Square(sq), occ);
        }
    }

    std::cout << "Done" << std::endl;
}

Bitboard fast_bishop_attacks(Square sq, Bitboard occ) {
    occ &= m_bishop_table[sq].mask;
    occ *= m_bishop_table[sq].magic;
    occ >>= m_bishop_table[sq].shift;
    return bishop_attacks[sq][occ];
}

Bitboard fast_rook_attacks(Square sq, Bitboard occ) {
    occ &= m_rook_table[sq].mask;
    occ *= m_rook_table[sq].magic;
    occ >>= m_rook_table[sq].shift;
    return rook_attacks[sq][occ];
}

void print_magics() {
    std::cout << "const Bitboard bishop_magics[64] = {" << std::endl;
    for (int i = 0; i < 64; i++) {
        std::cout << "    " << m_bishop_table[i].magic << "ull," << std::endl;
    }
    std::cout << "};" << std::endl << std::endl;

    std::cout << "const Bitboard rook_magics[64] = {" << std::endl;
    for (int i = 0; i < 64; i++) {
        std::cout << "    " << m_rook_table[i].magic << "ull," << std::endl;
    }
    std::cout << "};" << std::endl;
}