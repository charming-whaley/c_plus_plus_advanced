#ifndef FUNCTIONTEMPLATESEXAMPLES_H
#define FUNCTIONTEMPLATESEXAMPLES_H

#include <iostream>
#include <concepts>
#include <limits>
#include <type_traits>

/*
 * В данном файле хранится код с практики лекции по шаблонам функций
 * Здесь можно найти код для функции gcd, которая ищет наибольший общий делитель двух чисел
 *
 * Сначала напишем кастомную функцию abs, после чего перейдем уже к gcd
 */

// Кастомная реализация abs(_)

template<std::signed_integral T>
constexpr T abs(T value) noexcept { // <- реализация кастомного abs для целых чисел
    if (value == std::numeric_limits<T>::min())
        return std::make_unsigned_t<T>(value);
    return value < 0 ? -value : value;
}

template<std::floating_point T>
constexpr T abs(T value) noexcept { // <- реализация кастомного abs для чисел с плавающей точкой
    return std::fabs(value);
}

template<typename T>
concept signed_number = std::signed_integral<T> || std::floating_point<T>; // <- концепт для обобщения T в absolute (работает только с C++20+)

template<signed_number T>
constexpr auto absoluteValue(T value) noexcept { // <- общий кастомный abs
    return abs<T>(value);
}

// Кастомная реализация функции gcd(_, _). Сначала тело такой функции

template<std::signed_integral T>
constexpr T gcdBody(const T& firstValue, const T& secondValue) {
    if (secondValue == 0)
        throw std::invalid_argument("Fatal error: division modulo 0 is not accepted");
    T quotient = firstValue % secondValue;
    if (quotient < 0)
        quotient += absoluteValue(secondValue);
    return quotient;
}

template<std::floating_point T>
constexpr T gcdBody(const T& firstValue, const T& secondValue) {
    if (secondValue == 0)
        throw std::invalid_argument("Fatal error: division modulo 0 is not accepted");
    T quotient = firstValue % secondValue;
    if (quotient < 0)
        quotient += absoluteValue(secondValue);
    return quotient;
}

template<signed_number T>
constexpr auto gcd(const T& firstValue, const T& secondValue) {
    const auto quotient = gcdBody<T>(firstValue, secondValue);
    return quotient == 0 ? secondValue : gcd<T>(secondValue, quotient);
}

#endif
