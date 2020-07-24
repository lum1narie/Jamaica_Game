/*
 * main.cpp
 *
 *  Created on: 2019/08/16
 *      Author: lum1narie
 */
#include "RPN.hpp"
#include "jamaica_solver.hpp"

int main(int argc, char **argv) {
    Jamaica_solver solver = Jamaica_solver();
    RPN_infix_presenter presenter = RPN_infix_presenter();
    int sum;
    std::vector<int> numbers;

    std::string s;

    std::cout << "sum: ";
    std::getline(std::cin, s);
    sum = stoi(s);

    std::cout << "numbers: ";
    std::getline(std::cin, s);
    std::istringstream ss(s + "\n");
    while (true) {
        int t;
        ss >> t;
        if (!ss.good())
            break;
        numbers.push_back(t);
    }

    std::vector<std::unique_ptr<RPN_formula>> answers = solver.solve_jamaica(
            sum, numbers);

    std::cout << "========================================" << std::endl;
    // show all of answer
    for (auto&& answer : answers) {
        std::cout << presenter.get_present(*answer) << std::endl;
    }
    std::cout << answers.size() << " answers found." << std::endl;
}


