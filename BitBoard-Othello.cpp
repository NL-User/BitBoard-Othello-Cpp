#include "pch.h"
#include <stdio.h>
#include <chrono>
#include <fstream>

using namespace std;

const int kBoardSize = 8; // 何×何か
const int kCellsCount = kBoardSize * kBoardSize;
const bool kIsBackgroundBlack = true;

using ull_int = unsigned long long int;

int Count_Bits(ull_int bits) {
	bits = (bits & 0x5555555555555555) + (bits >> 1 & 0x5555555555555555);
	bits = (bits & 0x3333333333333333) + (bits >> 2 & 0x3333333333333333);
	bits = (bits & 0x0f0f0f0f0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f0f0f0f0f);
	bits = (bits & 0x00ff00ff00ff00ff) + (bits >> 8 & 0x00ff00ff00ff00ff);
	bits = (bits & 0x0000ffff0000ffff) + (bits >> 16 & 0x0000ffff0000ffff);
	return (bits & 0x00000000ffffffff) + (bits >> 32 & 0x00000000ffffffff);
}


class Board {
public:
	Board(ull_int, ull_int, bool);
	~Board();
	void View();
	int Get_Board_Piece_Count();

private:
	ull_int Get_Legal_Move();

	ull_int black_board, white_board;
	bool is_black_turn;
};

Board::Board(ull_int black_ = 0, ull_int white_ = 0, bool turn_ = true) {
	if (black_ == 0 || white_ == 0) {
		black_board = 0x0000000810000000;
		white_board = 0x0000001008000000;
	} else {
		black_board = black_;
		white_board = white_;
	}

	is_black_turn = turn_;
}

Board::~Board() {
}

void Board::View() {
	ull_int ligal_move = Get_Legal_Move();

	printf("%sのターン\n%dターン目\n\n  ", is_black_turn ? "黒" : "白", Get_Board_Piece_Count() - 3);
	for (int i = 97; i < 105; i++) {
		printf("  %c", i);
	}
	printf("\n");
	for (int i = kBoardSize; i > 0; i--) {
		printf(" %d", kBoardSize - i + 1);
		for (int j = 1; j <= kBoardSize; j++) {
			printf(" ");
			if (black_board >> (kBoardSize*i - j) & 0x1) {
				printf((kIsBackgroundBlack) ? "〇" : "●");
			} else if (white_board >> (kBoardSize*i - j) & 0x1) {
				printf((kIsBackgroundBlack) ? "●" : "〇");
			} else if (ligal_move >> (kBoardSize*i - j) & 0x1) {
				printf("△");
			} else {
				printf("－");
			}
		}
		printf("\n");
	}

}

int Board::Get_Board_Piece_Count() {
	return Count_Bits(black_board) + Count_Bits(white_board);
}

ull_int Board::Get_Legal_Move() {
	ull_int player, opponent;
	if (is_black_turn) {
		player = black_board;
		opponent = white_board;
	} else {
		player = black_board;
		opponent = white_board;
	}

	ull_int blank = ~(player | opponent);
	
	ull_int ligal_move = 0;
	ull_int tmp, mask;

	for (int i = 0; i < 2; i++) {
		for (int d : {1, 7, 8, 9}) {
			if (d == 1) {
				// 横
				mask = opponent & 0x7e7e7e7e7e7e7e7e;
			} else if (d == 8) {
				// 縦
				mask = opponent & 0x00ffffffffffff00;
			} else {
				// 斜め
				mask = opponent & 0x007e7e7e7e7e7e00;
			}

			if (i == 0) {
				tmp = mask & (player << d);
				tmp |= mask & (tmp << d);
				tmp |= mask & (tmp << d);
				tmp |= mask & (tmp << d);
				tmp |= mask & (tmp << d);
				tmp |= mask & (tmp << d);
				ligal_move |= blank & (tmp << d);
			} else {
				tmp = mask & (player >> d);
				tmp |= mask & (tmp >> d);
				tmp |= mask & (tmp >> d);
				tmp |= mask & (tmp >> d);
				tmp |= mask & (tmp >> d);
				tmp |= mask & (tmp >> d);
				ligal_move |= blank & (tmp >> d);
			}
		}
	}

	return ligal_move;
}

int main() {
	chrono::system_clock::time_point start_time, end_time;

	start_time = chrono::system_clock::now();

	Board* board = new Board();
	board->View();

	end_time = chrono::system_clock::now();

	printf("処理時間:%dミリ秒\n\n", (int)chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count());

	ofstream file("ull-int.txt", ios::app);
	file << (int)chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << ",";
	delete board;
	//_getwch();
}