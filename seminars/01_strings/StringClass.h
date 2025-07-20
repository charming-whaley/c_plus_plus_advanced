#ifndef STRINGCLASS_H
#define STRINGCLASS_H

#include <iostream>

class String {
    char* data;
    std::size_t size;
    std::size_t capacity;
public:
    explicit String(const char* data, const std::size_t size)
        : data(new char[size]), size(size), capacity(size + 5) {
        std::copy_n(data, size, this->data);
        this->data[size] = '\0';
    }

    String(const String& other)
        : data(new char[other.capacity]), size(other.size), capacity(other.capacity) {
        std::copy_n(other.data, other.size + 1, this->data);
    }

    String(String&& other) noexcept
        : data(other.data), size(other.size), capacity(other.capacity) {
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }

    String& operator=(const String& other) {
        if (this != &other) {
            delete[] this->data;
            this->data = new char[other.capacity];
            std::copy_n(other.data, other.size + 1, this->data);
            size = other.size;
            capacity = other.capacity;
        }

        return *this;
    }

    String& operator=(String&& other) noexcept {
        if (this != &other) {
            delete[] this->data;

            this->data = other.data;
            this->size = other.size;
            this->capacity = other.capacity;

            other.data = nullptr;
            other.size = 0;
            other.capacity = 0;
        }

        return *this;
    }

    ~String() {
        delete[] data;
    }

    [[nodiscard]] std::size_t length() const {
        return this->size;
    }

    [[nodiscard]] std::size_t getCapacity() const {
        return this->capacity;
    }

    [[nodiscard]] bool isEmpty() const {
        return this->size == 0;
    }

    void clear() {
        this->size = 0;
        this->capacity = 0;
        delete[] this->data;
    }

    void reserve(const std::size_t capacity) {
        if (capacity < this->capacity)
            throw std::invalid_argument("Fatal error: too small capacity for current String!");
        this->capacity = capacity;
    }

    void push_back(const char& character) {
        if (this->size + 1 >= capacity) {
            const auto newData = new char[this->capacity * 2];
            std::copy_n(this->data, this->size, newData);
            delete[] this->data;
            this->data = newData;
            this->capacity *= 2;
        }

        this->data[this->size] = character;
        ++this->size;
        this->data[this->size] = '\0';
    }

    void pop_back() {
        if (this->size == 0)
            throw std::out_of_range("Fatal error: index out of range!");
        --this->size;
        this->data[this->size] = '\0';
    }

    [[nodiscard]] char& front() const {
        return this->data[0];
    }

    [[nodiscard]] char& back() const {
        return this->data[this->size - 1];
    }

    char& operator[](std::size_t index) {
        if (index >= this->size)
            throw std::out_of_range("Fatal error: index out of range!");
        return this->data[index];
    }

    const char& operator[](std::size_t index) const {
        if (index >= this->size)
            throw std::out_of_range("Fatal error: index out of range!");
        return this->data[index];
    }

    friend std::ostream& operator<<(std::ostream& out, const String& string) {
        for (std::size_t i = 0; i < string.size; ++i)
            out << string.data[i];
        return out;
    }
};

#endif
