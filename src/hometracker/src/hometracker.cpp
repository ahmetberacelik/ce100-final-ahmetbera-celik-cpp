#include "../header/hometracker.h"
#include <stdexcept>

using namespace Coruh::Hometracker;

double Hometracker::add(double a, double b) {
    return a + b;
}

double Hometracker::subtract(double a, double b) {
    return a - b;
}

double Hometracker::multiply(double a, double b) {
    return a * b;
}

double Hometracker::divide(double a, double b) {
    if (b == 0) {
        throw std::invalid_argument("Division by zero is not allowed.");
    }
    return a / b;
}