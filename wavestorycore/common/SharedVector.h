#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <utility> // For std::forward

template <typename T>
class SharedVector {
private:
    std::vector<T> data;
    mutable std::mutex mtx; // Mutex for thread safety

public:
    // Default constructor
    SharedVector() = default;

    // Copy constructor (Thread-Safe Deep Copy)
    SharedVector(const SharedVector& other) {
        std::lock_guard<std::mutex> lock(other.mtx);
        data = other.data;
    }

    // Move constructor
    SharedVector(SharedVector&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.mtx);
        data = std::move(other.data);
    }

    // Copy assignment operator
    SharedVector& operator=(const SharedVector& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lockThis(mtx);
            std::lock_guard<std::mutex> lockOther(other.mtx);
            data = other.data;
        }
        return *this;
    }

    // Move assignment operator
    SharedVector& operator=(SharedVector&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lockThis(mtx);
            std::lock_guard<std::mutex> lockOther(other.mtx);
            data = std::move(other.data);
        }
        return *this;
    }

    // Push an element (universal reference for perfect forwarding)
    template <typename U>
    void push_back(U&& value) {
        std::lock_guard<std::mutex> lock(mtx);
        data.push_back(std::forward<U>(value));
    }

    // Emplace an element directly
    template <typename... Args>
    void emplace_back(Args&&... args) {
        std::lock_guard<std::mutex> lock(mtx);
        data.emplace_back(std::forward<Args>(args)...);
    }

    // Remove last element
    void pop_back() {
        std::lock_guard<std::mutex> lock(mtx);
        if (!data.empty()) {
            data.pop_back();
        }
    }

    // Get element at index
    T get(size_t index) const {
        std::lock_guard<std::mutex> lock(mtx);
        if (index < data.size()) {
            return data[index];
        }
        throw std::out_of_range("Index out of range");
    }

    //T getandpop_front() const {
    //    std::lock_guard<std::mutex> lock(mtx);
    //    if (0 < data.size()) {
    //        T elem = std::move(data[0]);
    //        //pop_front();
    //        return elem;
    //    }
    //    throw std::out_of_range("Index out of range");
    //}

    T pop_front() {
        std::lock_guard<std::mutex> lock(mtx);
        if (!data.empty()) {
            T elem = std::move(data[0]);
            data.erase(data.begin()); // Removes the first element
            return elem;
        }
        throw std::out_of_range("Index out of range");
    }

    // Get the current size of vector
    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return data.size();
    }

    // Print vector (for debugging)
    void print() const {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Vector: ";
        for (const auto& item : data) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
};
