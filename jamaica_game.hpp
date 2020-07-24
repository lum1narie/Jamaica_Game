/*
 * jamaica_game.hpp
 *
 *  Created on: 2019/08/16
 *      Author: lum1narie
 */

#ifndef JAMAICA_GAME_HPP_
#define JAMAICA_GAME_HPP_

#include "RPN.hpp"
#include "jamaica_solver.hpp"
#include <random>
#include <time.h>

class Jamaica_game {
private:
    /// random generator
    std::mt19937 mt;
    /// answer of jamaica game
    std::vector<std::unique_ptr<RPN_formula>> answers;
    /// white dices of jamaica game, numbers in formula
    std::vector<int> white_dices;
    /// black dices of jamaica game, sum of formula
    int black_dices_sum;
    /// reroll dices
    void reroll();
public:
    /// default constructor
    Jamaica_game();
    /// play single jamaica game
    void play();
//    /**
//     * @return answers of jamica game
//     */
//    const std::vector<std::unique_ptr<RPN_formula>>& get_answers() const;
};

#endif /* JAMAICA_GAME_HPP_ */
