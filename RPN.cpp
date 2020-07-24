/*
 * RPN.cpp
 *
 *  Created on: 2019/07/29
 *      Author: lum1narie
 */

#include "RPN.hpp"

// implement RPN_operand
// ========================================
RPN_operand::RPN_operand() :
        RPN_operand(NONE, std::vector<std::unique_ptr<RPN_number>>()) {
}

RPN_operand::RPN_operand(Operate_code operate_code,
        const std::vector<std::unique_ptr<RPN_number>>& args) {
    this->code = operate_code;
    this->args = std::vector<std::unique_ptr<RPN_number>>();
    if (args.size() < (size_t) get_required_args_number()) {
        // if given args' size is not enough
        this->code = NONE;
    } else {
        this->code = operate_code;
        for (auto&& arg : args) {
            this->args.emplace_back(new RPN_number(*arg));
        }
    }
}

RPN_operand::RPN_operand(Operate_code operate_code) :
        RPN_operand(operate_code, std::vector<std::unique_ptr<RPN_number>>()) {
}

RPN_operand::RPN_operand(const RPN_operand& operand) {
    this->code = operand.get_code();
    this->args = std::vector<std::unique_ptr<RPN_number>>();
    // make new pointer for args then just copy value into it
    for (auto&& arg : operand.get_args()) {
        this->args.emplace_back(new RPN_number(*arg));
    }
}

int RPN_operand::get_required_args_number() const {
    switch (code) {
    case NUMBER:
        return 1;
        break;
    default:
        return 0;
        break;
    }
    return 0;
}

std::string RPN_operand::to_string() const {
    std::stringstream ss;
    switch (code) {
    case PLUS:
        return "+";
        break;
    case MINUS:
        return "-";
        break;
    case MULTI:
        return "*";
        break;
    case DIVIDE:
        return "/";
        break;
    case NUMBER:
        if (args[0]->denominator() == 1) {
            /* if number is same as integer (ex. 5/1),
             * return the string which shows single integer (ex. "5")
             */
            ss << args[0]->numerator();
        } else {
            /* if number is fractional,
             * return default string with bracket (ex. "(5/2)")
             */
            ss << "(" << *(args[0]) << ")";
        }
        return ss.str();
        break;
    default:
        return "";
        break;
    }
    return "";
}

const std::vector<std::unique_ptr<RPN_number>>& RPN_operand::get_args() const {
    return args;
}

Operate_code RPN_operand::get_code() const {
    return code;
}

int comp_RPN_operand(const RPN_operand& operand_1,
        const RPN_operand& operand_2) {
    Operate_code code_1 = operand_1.get_code();
    Operate_code code_2 = operand_2.get_code();
    if (code_1 != code_2) {
        // return difference of code if different
        return code_1 - code_2;
    }

    // return differnce of args as -1 or 1 if first difference is found from front
    for (int i = 0; i < operand_1.get_required_args_number(); ++i) {
        RPN_number arg_1 = *(operand_1.get_args()[i]);
        RPN_number arg_2 = *(operand_2.get_args()[i]);

        if (arg_1 < arg_2) {
            return -1;
        } else if (arg_1 > arg_2) {
            return 1;
        }
    }

    // if same
    return 0;
}

bool RPN_operand::operator <(const RPN_operand& another) const {
    return comp_RPN_operand(*this, another) < 0;
}

bool RPN_operand::operator >(const RPN_operand& another) const {
    return comp_RPN_operand(*this, another) > 0;
}

bool RPN_operand::operator ==(const RPN_operand& another) const {
    return comp_RPN_operand(*this, another) == 0;
}

const RPN_operand& RPN_operand::operator =(const RPN_operand& another) {
    code = another.code;
    args.clear();
    for (auto&& arg : another.get_args()) {
        this->args.emplace_back(new RPN_number(*arg));
    }
    return *this;
}

RPN_operand::~RPN_operand() = default;
// ========================================

// implement RPN_formula
// ========================================
RPN_formula::RPN_formula() {
    formula_queue = std::deque<std::shared_ptr<RPN_operand>>();
}

RPN_formula::RPN_formula(
        const std::deque<std::shared_ptr<RPN_operand>>& queue) {
    formula_queue = queue;
}

RPN_formula::RPN_formula(const RPN_formula& formula) {
    formula_queue = formula.get_formula_queue();
}

void RPN_formula::add_operand(const RPN_operand& operand) {
    formula_queue.emplace_back(new RPN_operand(operand));
}

void RPN_formula::add_operand(std::shared_ptr<RPN_operand> operand) {
    formula_queue.push_back(operand);
}

void RPN_formula::add_operand(
        const std::deque<std::shared_ptr<RPN_operand>>& queue) {
    for (auto operand : queue) {
        formula_queue.push_back(operand);
    }
}

void RPN_formula::add_operand(const RPN_formula& formula) {
    add_operand(formula.get_formula_queue());
}

void RPN_formula::pop_front() {
    formula_queue.pop_front();
    return;
}

std::shared_ptr<RPN_operand> RPN_formula::take_front() {
    std::shared_ptr<RPN_operand> front_operand = formula_queue.front();
    formula_queue.pop_front();
    return front_operand;
}

void RPN_formula::clear() {
    formula_queue.clear();
}

std::string RPN_formula::to_string() const {
    std::string s;
    for (auto&& operand : formula_queue) {
        s += operand->to_string() + " ";
    }

    if (s.size() != 0) {
        s.pop_back();
    }
    return s;
}

const std::deque<std::shared_ptr<RPN_operand>>& RPN_formula::get_formula_queue() const {
    return formula_queue;
}

int get_required_stack_number(Operate_code code) {
    switch (code) {
    case NUMBER:
        return 0;
        break;
    case PLUS:
    case MINUS:
    case MULTI:
    case DIVIDE:
        return 2;
        break;
    default:
        return 0;
        break;
    }
}

int get_return_stack_number(Operate_code code) {
    switch (code) {
    case NUMBER:
    case PLUS:
    case MINUS:
    case MULTI:
    case DIVIDE:
        return 1;
        break;
    default:
        return 0;
        break;
    }
}

int comp_RPN_formula(const RPN_formula& formula_1,
        const RPN_formula& formula_2) {
    const std::deque<std::shared_ptr<RPN_operand>>& queue_1 =
            formula_1.get_formula_queue();
    const std::deque<std::shared_ptr<RPN_operand>>& queue_2 =
            formula_2.get_formula_queue();
    int size_1 = queue_1.size();
    int size_2 = queue_2.size();

    if (size_1 != size_2) {
        // if size is different
        return size_1 - size_2;
    } else {
        for (int i = 0; i < size_1; ++i) {
            int comp_operand_result = comp_RPN_operand(*(queue_1[i]),
                    *(queue_2[i]));
            if (comp_operand_result != 0) {
                return comp_operand_result;
            }
        }
    }
    return 0;
}

bool RPN_formula::operator <(const RPN_formula& another) const {
    return comp_RPN_formula(*this, another) < 0;
}

bool RPN_formula::operator >(const RPN_formula& another) const {
    return comp_RPN_formula(*this, another) > 0;
}

bool RPN_formula::operator ==(const RPN_formula& another) const {
    return comp_RPN_formula(*this, another) == 0;
}

const RPN_formula& RPN_formula::operator =(const RPN_formula& another) {
    formula_queue = another.formula_queue;
    return *this;
}

RPN_formula::~RPN_formula() = default;
// ========================================

// implement RPN_calculator
// ========================================
std::vector<std::unique_ptr<RPN_number>> RPN_calculator::take_from_numbers_stack(
        int take_count) {
    if (take_count > get_numbers_stack_count()) {
        throw "numbers stack is too less for evaluation";
    }

    std::vector<std::unique_ptr<RPN_number>> taken_numbers = std::vector<
            std::unique_ptr<RPN_number>>(take_count);
    for (int i = take_count - 1; i >= 0; --i) {
        taken_numbers[i] = std::move(numbers_stack.back());
        numbers_stack.pop_back();
    }
    return taken_numbers;
}

RPN_calculator::RPN_calculator() {
    numbers_stack = std::deque<std::unique_ptr<RPN_number>>();
    max_abs = INIT_RPN_EVAL_MAX_ABS;
    formula = RPN_formula();
}
RPN_calculator::RPN_calculator(const RPN_formula& formula) :
        RPN_calculator() {
    this->formula = formula;
}

void RPN_calculator::add_operand(const RPN_operand& operand) {
    formula.add_operand(operand);
}

void RPN_calculator::add_operand(std::shared_ptr<RPN_operand> operand) {
    formula.add_operand(operand);
}

void RPN_calculator::add_operand(const RPN_formula& formula) {
    this->formula.add_operand(formula);
}

void RPN_calculator::procede_operand() {
    std::shared_ptr<RPN_operand> operand = formula.take_front();
    std::vector<std::unique_ptr<RPN_number>> numbers_from_stack;
    std::deque<std::unique_ptr<RPN_number>> results;

    switch (operand->get_code()) {
    case NUMBER:
        results.emplace_back(new RPN_number(*(operand->get_args()[0])));
        break;
    case PLUS:
        numbers_from_stack = take_from_numbers_stack(2);
        results.emplace_back(
                new RPN_number(
                        *(numbers_from_stack[0]) + *(numbers_from_stack[1])));
        break;
    case MINUS:
        numbers_from_stack = take_from_numbers_stack(2);
        results.emplace_back(
                new RPN_number(
                        *(numbers_from_stack[0]) - *(numbers_from_stack[1])));
        break;
    case MULTI:
        numbers_from_stack = take_from_numbers_stack(2);
        results.emplace_back(
                new RPN_number(
                        *(numbers_from_stack[0]) * *(numbers_from_stack[1])));
        break;
    case DIVIDE:
        numbers_from_stack = take_from_numbers_stack(2);
        if (*(numbers_from_stack[1]) == 0) {
            throw "divided by zero";
        }
        results.emplace_back(
                new RPN_number(
                        *(numbers_from_stack[0]) / *(numbers_from_stack[1])));
        break;
    default:
        return;
        break;
    }

    for (auto&& result : results) {
        // when at least one of results excced max_abs
        if (max_abs > 0 && max_abs < boost::abs(*result)) {
            throw("result value is too large");
        }
        numbers_stack.push_back(std::move(result));
    }

    return;
}

void RPN_calculator::solve() {
    while (formula.get_formula_queue().size() > 0) {
        procede_operand();
    }
}

void RPN_calculator::drop() {
    numbers_stack.pop_back();
}

void RPN_calculator::clear() {
    numbers_stack.clear();
}

void RPN_calculator::flush_elements() {
    formula.clear();
}

void RPN_calculator::reset() {
    clear();
    flush_elements();
}

void RPN_calculator::print_stack() const {
    for (auto&& number : numbers_stack) {
        std::cout << *number << " ";
    }
    std::cout << std::endl;
}

void RPN_calculator::print_elements() const {
    std::cout << formula.to_string() << std::endl;
}

RPN_number RPN_calculator::get_last_number() const {
    if (numbers_stack.size() == 0) {
        throw("no number in stack");
    } else {
        return *(numbers_stack.back());
    }
}

int RPN_calculator::get_numbers_stack_count() const {
    return numbers_stack.size();
}

void RPN_calculator::set_max_abs(RPN_number max_abs) {
    this->max_abs = max_abs;
}
// ========================================

// ========================================
RPN_infix_progress::RPN_infix_progress() {
    formula_string = "";
    prority_rank = 0;
}
RPN_infix_progress::RPN_infix_progress(const std::string& str, int priority) {
    formula_string = str;
    prority_rank = priority;
}
const std::string& RPN_infix_progress::get_formula_string() const {
    return formula_string;
}
void RPN_infix_progress::set_formula_string(const std::string& formula_string) {
    this->formula_string = formula_string;
}
int RPN_infix_progress::get_priority_rank() const {
    return prority_rank;
}
void RPN_infix_progress::set_priority_rank(int least_priority) {
    this->prority_rank = least_priority;
}
// ========================================

// ========================================
RPN_infix_presenter::RPN_infix_presenter() {
    progress_stack = std::deque<std::unique_ptr<RPN_infix_progress>>();
}

void RPN_infix_presenter::procede_progress(const RPN_operand& operand) {
    std::vector<std::unique_ptr<RPN_infix_progress> > taken_progress;
    std::string formula_string = "";
    int operand_priority = get_operand_priority(operand.get_code());
    int next_worst_priority = 0;

    switch (operand.get_code()) {
    case NUMBER:
        // notion for single number
        formula_string = operand.to_string();
        next_worst_priority = get_operand_priority(operand.get_code());
        progress_stack.emplace_back(
                new RPN_infix_progress(formula_string, next_worst_priority));
        break;
    case PLUS:
    case MINUS:
    case MULTI:
    case DIVIDE:
        taken_progress = take_from_progress_stack(2);
        // notion for left operands and given operand
        if (operand_priority >= taken_progress[0]->get_priority_rank()) {
            // ... ?
            formula_string = taken_progress[0]->get_formula_string() + " " +  operand.to_string();
            next_worst_priority = operand_priority;
        } else {
            // (...) ?
            formula_string = "(" + taken_progress[0]->get_formula_string() + ") " + operand.to_string();
            next_worst_priority = taken_progress[0]->get_priority_rank();
        }
        // TODO: rework priority
        next_worst_priority = operand_priority;

        // notion for right operands
        if (taken_progress[1]->get_priority_rank() == get_operand_priority(NUMBER)) {
            // ... ? ...
            formula_string += " " + taken_progress[1]->get_formula_string();
        } else {
            // ... ? (...)
            formula_string += " (" + taken_progress[1]->get_formula_string() + ")";
        }
        progress_stack.emplace_back(
                        new RPN_infix_progress(formula_string, next_worst_priority));
        break;
    default:
        break;
    }
}

std::vector<std::unique_ptr<RPN_infix_progress> > RPN_infix_presenter::take_from_progress_stack(
        int take_count) {
    if (take_count > (int)progress_stack.size()) {
        throw "progress stack is too less for evaluation";
    }

    std::vector<std::unique_ptr<RPN_infix_progress>> taken_progress =
            std::vector<std::unique_ptr<RPN_infix_progress>>(take_count);
    for (int i = take_count - 1; i >= 0; --i) {
        taken_progress[i] = std::move(progress_stack.back());
        progress_stack.pop_back();
    }
    return taken_progress;
}

std::string RPN_infix_presenter::get_present(const RPN_formula& formula) {
    progress_stack.clear();
    for (auto&& operand : formula.get_formula_queue()) {
        procede_progress(*operand);
    }
    return progress_stack.front()->get_formula_string();
}
// ========================================

int get_operand_priority(Operate_code code) {
    switch (code) {
    case NUMBER:
        return 0;
        break;
    case MULTI:
    case DIVIDE:
        return 1;
        break;
    case PLUS:
    case MINUS:
        return 2;
        break;
    default:
        return 0;
        break;
    }
}
