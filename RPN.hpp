/*
 * RPN.hpp
 *
 * classes for expressing RPN(Reverse Polish Notation) formula
 * and calculator to evaluate RPN formula
 *
 *  Created on: 2019/07/29
 *      Author: lum1narie
 */

#ifndef RPN_HPP_
#define RPN_HPP_

#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <iostream>
#include <memory>
#include <boost/rational.hpp>

/// type ID of operand
enum Operate_code {
    NUMBER, PLUS, MINUS, MULTI, DIVIDE, NONE
};

/// use in RPN formula as number
using RPN_number = boost::rational<int>;

/// operand of RPN_formula
class RPN_operand {
private:
    /// type of operand
    Operate_code code; // only modified in constructer
    /// arguments of operand
    std::vector<std::unique_ptr<RPN_number>> args; // only modified in constructer
public:
    /// default constructor, should not use
    RPN_operand();
    /**
     * if less number of arguments than operate code needs are given,
     * NONE will be set as operate code then all of arguments will be ignored
     *
     * @param operate code to set
     * @param arguments to set
     */
    RPN_operand(Operate_code, const std::vector<std::unique_ptr<RPN_number>>&);
    /**
     * constructor for an operand which have empty args
     * if operate code which needs 1 argument at least is given, NONE will be set as operate code
     * @param operate code to set
     */
    RPN_operand(Operate_code);
    /**
     * copy constructor
     * @param another operand to be a source
     */
    RPN_operand(const RPN_operand&);
    /**
     * get minimum size of args which is decided by code
     * if given args in constructer is less than this, the operand will be NONE operand
     * exceed number of args from this are supposed to be ignored when operand is evaluated
     *
     * @return minimum size of args
     */
    int get_required_args_number() const;
    /**
     * get string shows operator
     * ex. RPN_operand(PLUS) -> "+", RPN_operand(NUMBER, {RPN_number(3, 2)}) -> "3/2"
     *
     * integer number is shown as single number instead number with denominator
     * (in spite of result of RPN_number::operator<<)
     * ex. RPN_operand(NUMBER, {RPN_number(5, 1)} -> "5"
     * @return string shows operator
     */
    std::string to_string() const;
    Operate_code get_code() const;
    const std::vector<std::unique_ptr<RPN_number>>& get_args() const;
    bool operator <(const RPN_operand&) const;
    bool operator >(const RPN_operand&) const;
    bool operator ==(const RPN_operand&) const;
    const RPN_operand& operator=(const RPN_operand&);
    virtual ~RPN_operand();
};
/**
 * used in operator < > == of RPN_operand
 * at first, codes are compared
 * next, args are compared from front to back
 *
 * @param former RPN_operand
 * @param latter RPN_operand
 * @return difference of operands, positive means former is bigger, negative means smaller, 0 means same
 */
int comp_RPN_operand(const RPN_operand&, const RPN_operand&);

/**
 * return how many stack numbers given Operate_code require
 * (ex. PLUS: 2 (!! + !!), NUMBER: 0)
 *
 * @param Operate_code to check number
 * @return how many stack numbers given Operate_code require
 */
int get_required_stack_number(Operate_code);
/**
 * return how many stack numbers given Operate_code return
 * (ex. PLUS: 1(? + ? = !!), NUMBER: 1)
 *
 * @param Operate_code to check number
 * @return how many stack numbers given Operate_code return
 */
int get_return_stack_number(Operate_code);


/// RPN style formula
class RPN_formula {
private:
    /// queue of operands, the body of formula
    std::deque<std::shared_ptr<RPN_operand>> formula_queue;
public:
    /// default constructor, make formula without operand
    RPN_formula();
    /**
     * @param queue of operands to set
     */
    RPN_formula(const std::deque<std::shared_ptr<RPN_operand>>&);
    /**
     * copy constructor
     * @param another formula to be a source
     */
    RPN_formula(const RPN_formula&);
    /**
     * add single operand to the last of formula
     *
     * @param operand to add
     */
    void add_operand(const RPN_operand&);
    /**
     * copy single operand to the last of formula
     * pointer will be shared with caller
     *
     * @param pointer of operand to copy then add
     */
    void add_operand(std::shared_ptr<RPN_operand>);
    /**
     * copy operands to the last of formula
     * pointers will be shared with caller
     *
     * @param pointers of operand to copy then add
     */
    void add_operand(const std::deque<std::shared_ptr<RPN_operand>>&);
    /**
     * copy operand from another formula to the last of formula
     * pointers will be shared with given formula
     *
     * @param formula to be a source
     */
    void add_operand(const RPN_formula&);
    /// remove first operand
    void pop_front();
    /**
     * get first operand with removing it from formula
     *
     * @return first operand in formula
     */
    std::shared_ptr<RPN_operand> take_front();
    /// remove all operands
    void clear();
    std::string to_string() const;
    const std::deque<std::shared_ptr<RPN_operand>>& get_formula_queue() const;
    bool operator <(const RPN_formula&) const;
    bool operator >(const RPN_formula&) const;
    bool operator ==(const RPN_formula&) const;
    const RPN_formula& operator=(const RPN_formula&);
    virtual ~RPN_formula();
};
/**
 * used in operator < > == of RPN_formula
 * at first, length are compared
 * next, operands are compared from front to back
 *
 * @param former RPN_formula
 * @param latter RPN_formula
 * @return difference of formula, positive means former is bigger, negative means smaller, 0 means same
 */
int comp_RPN_formula(const RPN_formula&, const RPN_formula&);

// initial value of RPN_calculator.max_abs
#define INIT_RPN_EVAL_MAX_ABS 10000000

/// calculator of RPN formula
class RPN_calculator {
private:
    /// remain operands(or formula) to calculate
    RPN_formula formula;
    /// numbers stack of RPN calculator
    std::deque<std::unique_ptr<RPN_number>> numbers_stack;
    /**
     * max value of each evaluation
     * 0 or negative value means infinity
     */
    RPN_number max_abs;
    /**
     * move and get numbers from numbers_stack then remove them from the stack
     *
     * @param number of RPN_number to take
     * @return numbers from numbers_stack
     */
    std::vector<std::unique_ptr<RPN_number>> take_from_numbers_stack(int);
public:
    /// default constructor, create RPN_calculator with empty formula
    RPN_calculator();
    /**
     * make copy of formula into calculator
     *
     * @param formula to be a source
     */
    RPN_calculator(const RPN_formula&);
    /**
     * add single operande to formula
     *
     * @param operand to add
     */
    void add_operand(const RPN_operand&);
    /**
     * add copy of pointer of operand to formula
     *
     * @param operand to add
     */
    void add_operand(std::shared_ptr<RPN_operand>);
    /**
     * add copy of given formula to the formula in calculator
     *
     * @param formula to add
     */
    void add_operand(const RPN_formula&);
    /// read one operand from formula then evaluate it
    void procede_operand();
    /// evaluate all of operands in formula
    void solve();
    /// drop last number in stack
    void drop();
    /// clear all numbers in stack
    void clear();
    /// clear all elements in queue
    void flush_elements();
    /// clear all numbers and elements
    void reset();
    /// print all numbers in stack to console
    void print_stack() const;
    /// print all elements in queue to console
    void print_elements() const;
    /**
     * return last number in stack
     *
     * @throw if no number in stack
     */
    RPN_number get_last_number() const;
    /// return the size of stack
    int get_numbers_stack_count() const;
    /**
     * set maximum value of evaluation
     * set 0 or negative value means infinity
     *
     * @param maximum value (<=0 for inf)
     */
    void set_max_abs(RPN_number);
};

/// progress_stack of building infix notation
class RPN_infix_progress {
private:
    /// string represent formula in infix notation
    std::string formula_string;
    /// priority rank of last operand in formula, lower means high priority
    int prority_rank;
public:
    /// default constructor
    RPN_infix_progress();
    RPN_infix_progress(const std::string&, int);
    const std::string& get_formula_string() const;
    void set_formula_string(const std::string&);
    int get_priority_rank() const;
    void set_priority_rank(int);
};

/// get_present RPN formula as infix notation
class RPN_infix_presenter {
private:
    /// stack of RPN_infix_progress
    std::deque<std::unique_ptr<RPN_infix_progress>> progress_stack;

    /**
     * procede presentation for one operand
     * @param operand to procede
     */
    void procede_progress(const RPN_operand&);
    /**
     * move and get progresses from progress_stack then remove them from the stack
     *
     * @param number of progresses to take
     * @return progresses from stack
     */
    std::vector<std::unique_ptr<RPN_infix_progress>> take_from_progress_stack(int);
public:
    /// default constructor
    RPN_infix_presenter();
    /**
     * get infix notation of RPN formula
     * @param RPN formula to show
     * @return string of infix notation
     */
    std::string get_present(const RPN_formula&);
};

/**
 * get priority of Operate_code
 * lower number means high priority
 * NUMBER and NONE are 0
 *
 * @param code to check
 * @return priority of code
 */
int get_operand_priority(Operate_code);

#endif /* RPN_HPP_ */
