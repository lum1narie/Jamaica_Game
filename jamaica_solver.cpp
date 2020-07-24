/*
 * jamaica_solver.cpp
 *
 *  Created on: 2019/07/03
 *      Author: lum1narie
 */

#include <chrono>
#include "jamaica_solver.hpp"

#include "RPN.hpp"

// implement Round_robin_formula_generator
// ========================================
RPN_formula_progress::RPN_formula_progress() {
    this->progress_formula = RPN_formula();
    this->rest_num_operand = std::vector<std::shared_ptr<RPN_operand>>();
}

RPN_formula_progress::RPN_formula_progress(const RPN_formula& formula,
        const std::vector<std::shared_ptr<RPN_operand>>& rest_num_operand) {
    this->progress_formula = formula;
    this->rest_num_operand = rest_num_operand;
}

RPN_formula_progress::RPN_formula_progress(
        const RPN_formula_progress& progress) {
    this->progress_formula = progress.progress_formula;
    this->rest_num_operand = progress.rest_num_operand;
}

int RPN_formula_progress::get_current_stack_number() {
    int number = 0;
    for (auto&& operand : progress_formula.get_formula_queue()) {
        Operate_code code = operand->get_code();
        number += get_return_stack_number(code)
                - get_required_stack_number(code);
    }
    return number;
}

const RPN_formula_progress& RPN_formula_progress::operator =(
        const RPN_formula_progress& another) {
    this->progress_formula = another.progress_formula;
    this->rest_num_operand = another.rest_num_operand;
    return *this;
}

Round_robin_formula_generator::Round_robin_formula_generator(
        const std::vector<std::shared_ptr<RPN_operand>>& num_operands,
        const std::vector<std::shared_ptr<RPN_operand>>& operands) {
    this->num_operands = num_operands;
    this->operands = operands;

    progress_stack = std::deque<std::unique_ptr<RPN_formula_progress>>();
    progress_stack.emplace_back(
            new RPN_formula_progress(RPN_formula(), num_operands));
}

RPN_formula Round_robin_formula_generator::generate_next() {
    while (progress_stack.size() > 0) {
        std::unique_ptr<RPN_formula_progress> current_progress = std::move(
                progress_stack.back());
        progress_stack.pop_back();

        if (current_progress->rest_num_operand.size() == 0
                && current_progress->get_current_stack_number() == 1) {
            // if brought progress_formula is completed one
            return current_progress->progress_formula;
        }

        // making next progress with one of remain num_operands
        for (auto&& number : current_progress->rest_num_operand) {
            progress_stack.emplace_back(
                    new RPN_formula_progress(*current_progress));

            RPN_formula_progress& next_progress = *(progress_stack.back());

            next_progress.progress_formula.add_operand(number);

            // remove number used now from next progress
            auto&& itr_find = std::find_if(
                    next_progress.rest_num_operand.begin(),
                    next_progress.rest_num_operand.end(),
                    [&number](const std::shared_ptr<RPN_operand>& p) {
                        return *(p) == *(number);});
            next_progress.rest_num_operand.erase(itr_find);
        }

        // making next progress with one of operands if count of stack numbers is enough
        if (current_progress->get_current_stack_number() >= 2) {
            for (auto&& operand : operands) {
                progress_stack.emplace_back(
                        new RPN_formula_progress(*current_progress));
                RPN_formula_progress& next_progress = *(progress_stack.back());
                next_progress.progress_formula.add_operand(operand);
            }
        }
    }

    // at last, return RPN_formula with empty formula
    return RPN_formula();
}

// ========================================

// implement Jamaica_solver
// ========================================
bool Jamaica_solver::judge_formula(int answer, const RPN_formula& formula,
        RPN_calculator& calc) {
    RPN_number result = RPN_number();
    calc.reset();
    calc.add_operand(formula);

    try {
        calc.solve();
        if (calc.get_numbers_stack_count() != 1) {
            return false;
        }

        result = calc.get_last_number();
    } catch (char const* e) {
        return false;
    }

    return result == answer;
}

Jamaica_solver::Jamaica_solver() {
// set PLUS, MINUS, MULTI, DIVIDE in operands as default
    operands = std::vector<std::shared_ptr<RPN_operand>>();
    operands.emplace_back(new RPN_operand(PLUS));
    operands.emplace_back(new RPN_operand(MINUS));
    operands.emplace_back(new RPN_operand(MULTI));
    operands.emplace_back(new RPN_operand(DIVIDE));

    calculator = RPN_calculator();
}

RPN_formula Jamaica_solver::get_next_answer(int sum,
        Round_robin_formula_generator& generator) {
    while (true) {
        RPN_formula generated_formula = generator.generate_next();

        // return empty formula if no answer left
        if (generated_formula.get_formula_queue().size() == 0) {
            return RPN_formula();
        }

        if (judge_formula(sum, generated_formula, calculator)) {
            return generated_formula;
        }
    }
}

std::vector<std::unique_ptr<RPN_formula>> Jamaica_solver::solve_jamaica(int sum,
        const std::vector<int>& numbers) {
    std::vector<std::shared_ptr<RPN_operand>> rpn_numbers = std::vector<
            std::shared_ptr<RPN_operand>>();
    for (auto number : numbers) {
        // convert vector<int> into vector<unique_ptr<RPN_number>>
        std::vector<std::unique_ptr<RPN_number>> args = std::vector<
                std::unique_ptr<RPN_number>>();
        args.emplace_back(new RPN_number(number));
        rpn_numbers.emplace_back(new RPN_operand(NUMBER, args));
    }

    Round_robin_formula_generator generator = Round_robin_formula_generator(
            rpn_numbers, operands);
    std::vector<std::unique_ptr<RPN_formula>> correct_formulas = std::vector<
            std::unique_ptr<RPN_formula>>();
    RPN_formula correct_formula;

    while (true) {
        correct_formula = get_next_answer(sum, generator);
        if (correct_formula.get_formula_queue().size() == 0) {
            // if no answer found
            break;
        } else {
            // if answer found
            correct_formulas.emplace_back(new RPN_formula(correct_formula));
        }
    }

    // sort then make answers unique
    std::sort(correct_formulas.begin(), correct_formulas.end(),
            [](const std::unique_ptr<RPN_formula>& p1,
                    const std::unique_ptr<RPN_formula> & p2)
                    {return *p1 < *p2;});
    correct_formulas.erase(
            std::unique(correct_formulas.begin(), correct_formulas.end(),
                    [](const std::unique_ptr<RPN_formula>& p1,
                            const std::unique_ptr<RPN_formula> & p2)
                            {return *p1 == *p2;}),
            correct_formulas.end());
    return correct_formulas;
}

RPN_formula Jamaica_solver::solve_first_answer(int sum,
        const std::vector<int>& numbers) {
    std::vector<std::shared_ptr<RPN_operand>> rpn_numbers = std::vector<
            std::shared_ptr<RPN_operand>>();
    for (auto number : numbers) {
        // convert vector<int> into vector<unique_ptr<RPN_number>>
        std::vector<std::unique_ptr<RPN_number>> args = std::vector<
                std::unique_ptr<RPN_number>>();
        args.emplace_back(new RPN_number(number));
        rpn_numbers.emplace_back(new RPN_operand(NUMBER, args));
    }

    Round_robin_formula_generator generator = Round_robin_formula_generator(
            rpn_numbers, operands);

    // get RPN_formula with 0 length formula when not found
    RPN_formula correct_formula = get_next_answer(sum, generator);

    return correct_formula;
}

const std::vector<std::unique_ptr<RPN_operand>> Jamaica_solver::get_operands() const {
    std::vector<std::unique_ptr<RPN_operand>> operands_unique = std::vector<
            std::unique_ptr<RPN_operand>>();
    for (auto&& operand : operands) {
        operands_unique.emplace_back(new RPN_operand(*operand));
    }
    return operands_unique;
}
void Jamaica_solver::set_operands(
        const std::vector<std::unique_ptr<RPN_operand>>& operands) {
    this->operands = std::vector<std::shared_ptr<RPN_operand>>();
    for (auto&& operand : operands) {
        this->operands.emplace_back(new RPN_operand(*operand));
    }
}
// ========================================

// test code

//int main(void) {
//    std::chrono::system_clock::time_point start, end;
//
//    start = std::chrono::system_clock::now();
//    Jamaica_solver j = Jamaica_solver();
//
//    std::vector<std::unique_ptr<RPN_formula>> fs = j.solve_jamaica(13,
//            std::vector<int> { 5, 2, 3, 4, 5 });
//    RPN_infix_presenter p;
//    for (auto&& f : fs) {
//        std::cout << f->to_string() << " \t " << p.get_present(*f) << std::endl;
//    }
//
//    end = std::chrono::system_clock::now();
//
//    double time = static_cast<double>(std::chrono::duration_cast<
//            std::chrono::microseconds>(end - start).count() / 1000.0);
//    std::cout << time << "ms expired";
//}
