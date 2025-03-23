#include "magic.h"
#include "board.h"

#include <random>

Bitboard attack_table[ROOK_ATTACKS + BISHOP_ATTACKS];

SMagic m_bishop_table[64];
SMagic m_rook_table  [64];

const int RBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12,
};

const int BBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6,
};

const Bitboard bishop_magics[64] = {
    297457482031202818ull,
    1298216546004762888ull,
    2308130547484000256ull,
    633872749428938ull,
    326590413789995280ull,
    7327197765369880ull,
    573956381737092ull,
    306315435473637376ull,
    2413497833488512ull,
    35203767632417ull,
    36105797227806720ull,
    4611695931229208722ull,
    4611687187742720004ull,
    9008368694068484ull,
    2308095375980380160ull,
    283958173506050ull,
    2323998216116372484ull,
    9296713869336772864ull,
    144695799245308169ull,
    164391299204550660ull,
    19283252185399328ull,
    18155690318102784ull,
    324443895428030464ull,
    1749683779359805456ull,
    76631571100619264ull,
    286972804350465ull,
    4614016984957354000ull,
    9529621209969393952ull,
    18296972998885376ull,
    4616189895082512384ull,
    321334454652992ull,
    22806070185985089ull,
    599594893334560ull,
    6917691824628875808ull,
    68125744752099841ull,
    9227877837671235712ull,
    666533983412420736ull,
    441353324045338624ull,
    2329319679723776ull,
    4720899039560737024ull,
    4611835707701415938ull,
    72576904794241ull,
    288690385134290948ull,
    589621834547332ull,
    11831099200631783937ull,
    450365700897767808ull,
    5333389002316711425ull,
    4505012688392225ull,
    297872063160385792ull,
    586631376865329152ull,
    9799835031319085824ull,
    47287814358864008ull,
    289356312045748744ull,
    18033227679932928ull,
    149762306774597792ull,
    2454479941023056913ull,
    2310487623087425539ull,
    8791045168629328945ull,
    6755747342320640ull,
    9227875707366426629ull,
    4612248977272865290ull,
    292737558050898465ull,
    289427228935127556ull,
    7160727891600228370ull,
};

const Bitboard rook_magics[64] = {
    9259401388333481986ull,
    4629718010196664322ull,
    36046389622342528ull,
    72062060839569664ull,
    144117387234378784ull,
    4683748010554294528ull,
    36029346808332544ull,
    9295431005961847552ull,
    603623227142455304ull,
    77124420661018660ull,
    1171498990781366784ull,
    18295942206390304ull,
    9147971111224320ull,
    1267187788547072ull,
    585608693349876224ull,
    955607588892854528ull,
    4647857202206343272ull,
    9149587084288005ull,
    45037096192245792ull,
    9227875911494533185ull,
    11259548966847488ull,
    578995126639526912ull,
    1170975486746104368ull,
    38282795869701157ull,
    1477180970909048848ull,
    2314920595468324864ull,
    2305878195734315136ull,
    1441195871964233856ull,
    4622945176411181312ull,
    9241390835566641664ull,
    576462968507629832ull,
    13582344447606924ull,
    36169809527832608ull,
    4616189755497906240ull,
    2305860603555618816ull,
    9439686690403782656ull,
    9228157188852156416ull,
    581105115214119936ull,
    4828149286694814032ull,
    2323899487900144641ull,
    18049857767899136ull,
    9223654062661107744ull,
    9516248984046403600ull,
    1153765998257045536ull,
    577591050391027840ull,
    5066584209096708ull,
    1152939105383088648ull,
    4827859488876920833ull,
    180144604161573632ull,
    63067988042981952ull,
    35188671256320ull,
    2469116105206038656ull,
    2251817001949568ull,
    562984581857792ull,
    9227893237937669120ull,
    2305843628831998464ull,
    4647715366276964609ull,
    18014538100186369ull,
    2307340820539246849ull,
    360850989000950818ull,
    5231493951774524418ull,
    281767035013651ull,
    4611687186792841348ull,
    10696191184830530ull,
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

Bitboard find_magic(Square sq, int m, int bishop) {
    Bitboard mask, b[4096], a[4096], used[4096], magic;
    int i, j, k, n, fail;

    mask = bishop ? bmask(sq) : rmask(sq);
    n = count(mask);

    for(i = 0; i < (1 << n); i++) {
        b[i] = index_to_occ(i, mask);
        a[i] = bishop ? get_bishop_attacks(sq, b[i]) : get_rook_attacks(sq, b[i]);
    }
    for(k = 0; k < 100000000; k++) {
    magic = random_magic_bb();
    if(count((mask * magic) & 0xFF00000000000000ULL) < 6) continue;
    for(i = 0; i < 4096; i++) used[i] = 0ULL;
    for(i = 0, fail = 0; !fail && i < (1 << n); i++) {
        j = transform(b[i], magic, m);
        if(used[j] == 0ULL) used[j] = a[i];
        else if(used[j] != a[i]) fail = 1;
        }
        if(!fail) return magic;
    }
    printf("***Failed***\n");
    return 0ULL;
}

void init_magics(bool gen_magics) {
    Bitboard mask;
    Bitboard* ptr = (Bitboard*)&attack_table;
    std::cout << "Initializing bishop magics..." << std::endl;
    for (int sq = 0; sq < 64; sq++) {
        mask = bmask(Square(sq));
        std::cout << "Bishop sq " << sq << " for " << BBits[sq] << " bits" << std::endl;
        
        m_bishop_table[sq].ptr = ptr;
        m_bishop_table[sq].mask = mask;
        m_bishop_table[sq].shift = 64 - BBits[sq];
        if (gen_magics)
            m_bishop_table[sq].magic = find_magic(Square(sq), BBits[sq], 1);
        else
            m_bishop_table[sq].magic = bishop_magics[sq];

        ptr += 1 << BBits[sq];
    }
    std::cout << "Initializing rook magics..." << std::endl;
    for (int sq = 0; sq < 64; sq++) {
        mask = rmask(Square(sq));
        std::cout << "Rook sq " << sq << " for " << RBits[sq] << " bits" << std::endl;
        
        m_rook_table[sq].ptr = ptr;
        m_rook_table[sq].mask = mask;
        m_rook_table[sq].shift = 64 - RBits[sq];
        if (gen_magics)
            m_rook_table[sq].magic = find_magic(Square(sq), RBits[sq], 0);
        else
            m_rook_table[sq].magic = rook_magics[sq];

        ptr += 1 << RBits[sq];
    }

    std::cout << "Magics found..." << std::endl;
    std::cout << "Filling attack tables..." << std::endl;

    Bitboard index;
    Bitboard occ;

    for (int sq = 0; sq < 64; sq++) {
        for (int occ_index = 0; occ_index < (1 << BBits[sq]); occ_index++) {
            occ = index_to_occ(occ_index, m_bishop_table[sq].mask);
            index = transform(occ, m_bishop_table[sq].magic, BBits[sq]);
            m_bishop_table[sq].ptr[index] = get_bishop_attacks(Square(sq), occ);
        }
    }

    for (int sq = 0; sq < 64; sq++) {
        for (int occ_index = 0; occ_index < (1 << RBits[sq]); occ_index++) {
            occ = index_to_occ(occ_index, m_rook_table[sq].mask);
            index = transform(occ, m_rook_table[sq].magic, RBits[sq]);
            m_rook_table[sq].ptr[index] = get_rook_attacks(Square(sq), occ);
        }
    }

    std::cout << "Done" << std::endl;
}

Bitboard fast_bishop_attacks(Square sq, Bitboard occ) {
    Bitboard* aptr  = m_bishop_table[sq].ptr;
    occ            &= m_bishop_table[sq].mask;
    occ            *= m_bishop_table[sq].magic;
    occ           >>= m_bishop_table[sq].shift;
    return aptr[occ];
}

Bitboard fast_rook_attacks(Square sq, Bitboard occ) {
    Bitboard* aptr  = m_rook_table[sq].ptr;
    occ            &= m_rook_table[sq].mask;
    occ            *= m_rook_table[sq].magic;
    occ           >>= m_rook_table[sq].shift;
    return aptr[occ];
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