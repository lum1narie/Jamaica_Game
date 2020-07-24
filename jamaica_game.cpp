/*
 * jamaica_game.cpp
 *
 *  Created on: 2019/08/16
 *      Author: lum1narie
 */

#include "jamaica_game.hpp"

void Jamaica_game::reroll() {
    std::uniform_int_distribution<> rand_dice(1, 6);
    black_dices_sum = rand_dice(mt) * 10 + rand_dice(mt);
    white_dices.clear();
    for (int i = 0; i < 5; ++i) {
        white_dices.push_back(rand_dice(mt));
    }
}

Jamaica_game::Jamaica_game() {
    mt = std::mt19937(time(NULL));
    black_dices_sum = 0;
    white_dices = std::vector<int>();
}

void Jamaica_game::play() {
    Jamaica_solver solver = Jamaica_solver();
    RPN_infix_presenter presenter = RPN_infix_presenter();
    std::string tmp_s;

    reroll();

    // show question
    std::cout << black_dices_sum << " [ ";
    for (auto number : white_dices) {
        std::cout << number << " ";
    }
    std::cout << "]" << std::endl;

    // solve
    answers = solver.solve_jamaica(black_dices_sum, white_dices);

    // wait for tapping enter
    std::cout << "tap enter to show answer...";
    getline(std::cin, tmp_s);

    // show all of answer
    for (auto&& answer : answers) {
        std::cout << presenter.get_present(*answer) << std::endl;
    }
    std::cout << answers.size() << " answers found." << std::endl;
}

int main(int argc, char **argv) {
    Jamaica_game jamaica_game = Jamaica_game();
    std::string tmp_s;
    while (true) {
        // give one question
        jamaica_game.play();

        // wait for tapping enter
        std::cout << "tap enter for next question...";
        getline(std::cin, tmp_s);
        std::cout << "========================================" << std::endl;
    }
}
