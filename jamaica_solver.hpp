/*
 * jamaica_solver.hpp
 *
 * program to solve jamaica game
 * these class generate formulas and try it whether it is correct one
 *
 *  Created on: 2019/07/03
 *      Author: lum1narie
 */

#ifndef JAMAICA_SOLVER_HPP_
#define JAMAICA_SOLVER_HPP_

#include <algorithm>

#include "RPN.hpp"

/// progress of making RPN_formula in Round_robin_formula_generator
class RPN_formula_progress {
public:
    /// progress, generating formula
    RPN_formula progress_formula;
    /// NUMBER operand remains unused
    std::vector<std::shared_ptr<RPN_operand>> rest_num_operand;

    /**
     * calculate how many number will remain in calculator stack when evaluate progress_formula
     *
     * @return number of numbers which will be remain after evaluate progress_formula
     */
    int get_current_stack_number();
    /// defualt constructor
    RPN_formula_progress();
    /**
     * @param formula to be a source
     * @param operands to be a source
     */
    RPN_formula_progress(const RPN_formula&,
            const std::vector<std::shared_ptr<RPN_operand>>&);
    /**
     * copy constructor
     * @param progress to be a source
     */
    RPN_formula_progress(const RPN_formula_progress&);
    const RPN_formula_progress& operator=(const RPN_formula_progress&);
};

/**
 * make RPN_formula in round robin algorythm
 * generate all of possible formulas including numbers in num_operands exactly once
 * (formula from {1, 2, 3, 3} include exactly one 1, one 2, and two 3)
 * and some operators in operands
 * ex. {1, 2, 3} -> 1 2 3 + +, 1 2 3 + -, or 3 1 2 + + and so on
 *
 * this class will make formulas in depth-first search
 */
class Round_robin_formula_generator {
private:
    /// stack of RPN_formula_progress on construction
    std::deque<std::unique_ptr<RPN_formula_progress>> progress_stack;
    /**
     * operands used in formula
     * each operands can be used several times, or can be used 0 time
     */
    std::vector<std::shared_ptr<RPN_operand>> operands;
    /**
     * numbers used in formula
     * each number used only once
     */
    std::vector<std::shared_ptr<RPN_operand>> num_operands;
public:
    /**
     * @param source of num_operands
     * @param source of operands
     */
    Round_robin_formula_generator(
            const std::vector<std::shared_ptr<RPN_operand>>&,
            const std::vector<std::shared_ptr<RPN_operand>>&);
    /**
     * generate next valid formula
     * return 0 length formula if no more formulas are left
     * @return one of RPN style fomrula, or 0 length formula if no more left
     */
    RPN_formula generate_next();
};

/// solver of Jamaica problem
class Jamaica_solver {
private:
    /**
     * operands enable in question
     * default value is PLUS, MINUS, MULTI, DIVIDE
     */
    std::vector<std::shared_ptr<RPN_operand>> operands;
    /**
     * judge formula if correct
     * @param supposed answer
     * @param formula to be judged
     * @param RPN_calculator object used to calculate formula, supposed to be modified
     * @return result of formula evaluation is supposed answer
     */
    bool judge_formula(int, const RPN_formula&, RPN_calculator&);
    /**
     * search until find next answer, generated by given generator
     * return 0 length formula if no answer found
     *
     * @param supposed answer
     * @param generator used in generate answer, supposed to be modified
     * @return next answer, or 0 length formula if no answer found
     */
    RPN_formula get_next_answer(int, Round_robin_formula_generator&);
    /**
     * RPN_calculator object used in judge_formula()
     * user will never touch
     */
    RPN_calculator calculator;
public:
    /// default constructor
    Jamaica_solver();
    /**
     * solve jamaica question to get all answers
     * @param black dice sum of jamaica question
     * @param white dice numbers of jamaica question
     * @return formulas, the answer of jamaica question
     */
    std::vector<std::unique_ptr<RPN_formula>> solve_jamaica(int,
            const std::vector<int>&);
    /**
     * solve jamaica question to get only one answer
     * @param black dice sum of jamaica question
     * @param white dice numbers of jamaica question
     * @return formula, one of the answer of jamaica question
     */
    RPN_formula solve_first_answer(int, const std::vector<int>&);

    /// get copy of value of operands
    const std::vector<std::unique_ptr<RPN_operand>> get_operands() const;
    /// set operands by copying value
    void set_operands(const std::vector<std::unique_ptr<RPN_operand>>&);
};

#endif /* JAMAICA_SOLVER_HPP_ */
