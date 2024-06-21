#include <cassert>
#include <iostream>

// Съдържа полезни структури и функции за работа с динамична памет

static constexpr auto MAX_STACK_SIZE = 100;

struct CharVector {
    char* data = nullptr;
    int size = 0;
    int capacity = 0;
};

struct IntVector {
    int* data = nullptr;
    int size = 0;
    int capacity = 0;
};

struct Stack {
    int data[MAX_STACK_SIZE]{};
    int top = -1;
};

struct TruthTable {
    int* data = nullptr;
    int size = 0;
    int capacity = 0;
    int rows = 0;
    int cols = 0;
};

namespace utils {
int* allocIntArray(const int arrSize) {
    int* arr = new (std::nothrow) int[arrSize]{};
    assert(arr && "Failed to allocate memory");
    return arr;
}

char* allocCharArray(const int arrSize) {
    char* arr = new (std::nothrow) char[arrSize]{};
    assert(arr && "Failed to allocate memory");
    return arr;
}

void freeIntArray(int*& arr) {
    delete[] arr;
    arr = nullptr;
}

void freeCharArray(char*& arr) {
    delete[] arr;
    arr = nullptr;
}

void printCharVector(const CharVector& vector) {
    for (int i = 0; i < vector.size; i++) {
        std::cout << vector.data[i] << " ";
    }
    std::cout << std::endl;
}

void printIntVector(const IntVector& vector) {
    for (int i = 0; i < vector.size; i++) {
        std::cout << vector.data[i] << " ";
    }
    std::cout << std::endl;
}

void reallocIntVector(IntVector& vector, const int newCapacity) {
    if (vector.capacity > newCapacity) {
        return;
    }

    int* newData = allocIntArray(newCapacity);
    for (int i = 0; i < vector.size; i++) {
        newData[i] = vector.data[i];
    }
    freeIntArray(vector.data);
    vector.data = newData;
    vector.capacity = newCapacity;
}

void reallocCharVector(CharVector& vector, const int newCapacity) {
    if (vector.capacity > newCapacity) {
        return;
    }

    char* newData = allocCharArray(newCapacity);
    for (int i = 0; i < vector.size; i++) {
        newData[i] = vector.data[i];
    }
    freeCharArray(vector.data);
    vector.data = newData;
    vector.capacity = newCapacity;
}

void printTruthTable(const TruthTable& table) {
    for (int i = 0; i < table.rows; i++) {
        for (int j = 0; j < table.cols; j++) {
            std::cout << table.data[i * table.cols + j] << " ";
        }
        std::cout << std::endl;
    }
}
}  // namespace utils

IntVector makeIntVector(const int capacity) {
    IntVector vector;
    vector.data = utils::allocIntArray(capacity);
    vector.capacity = capacity;
    return vector;
}

CharVector makeCharVector(const int capacity) {
    CharVector vector;
    vector.data = utils::allocCharArray(capacity);
    vector.capacity = capacity;
    return vector;
}

void clearIntVector(IntVector& vector) {
    utils::freeIntArray(vector.data);
    vector.capacity = 0;
    vector.size = 0;
}

void clearCharVector(CharVector& vector) {
    utils::freeCharArray(vector.data);
    vector.capacity = 0;
    vector.size = 0;
}

void pushToIntVector(IntVector& vector, const int value) {
    if (vector.size + 1 == vector.capacity) {
        utils::reallocIntVector(vector, vector.capacity * 2);
    }
    vector.data[vector.size++] = value;
}

void pushToCharVector(CharVector& vector, const char value) {
    if (vector.size + 1 == vector.capacity) {
        utils::reallocCharVector(vector, vector.capacity * 2);
    }
    vector.data[vector.size++] = value;
}

void popFromIntVector(IntVector& vector) {
    assert(vector.size > 0 && "Popping from empty IntVector");
    vector.data[--vector.size] = 0;
}

void popFromCharVector(CharVector& vector) {
    assert(vector.size > 0 && "Popping from empty CharVector");
    vector.data[--vector.size] = 0;
}

int getIntVectorBack(const IntVector& vector) {
    assert(vector.size > 0 && "IntVector is empty");
    return vector.data[vector.size - 1];
}

char getCharVectorBack(const CharVector& vector) {
    assert(vector.size > 0 && "CharVector is empty");
    return vector.data[vector.size - 1];
}

void pushToStack(Stack& stack, const int value) {
    assert(stack.top < MAX_STACK_SIZE && "Stack capacity exceeded");
    stack.data[++stack.top] = value;
}

void popFromStack(Stack& stack) {
    assert(stack.top >= 0 && "Popping from empty stack");
    stack.data[stack.top--] = 0;
}

int peekStack(const Stack& stack) {
    assert(stack.top >= 0 && "Peaking empty stack");
    return stack.data[stack.top];
}

int getStackSize(const Stack& stack) { return stack.top + 1; }

TruthTable makeTruthTable(const int capacity) {
    TruthTable table;
    table.data = utils::allocIntArray(capacity);
    table.capacity = capacity;
    return table;
}

void freeTruthTable(TruthTable& table) {
    utils::freeIntArray(table.data);
    table.capacity = 0;
    table.size = 0;
    table.rows = 0;
    table.cols = 0;
}

void pushToTruthTable(TruthTable& table, const int value) {
    if (table.size + 1 == table.capacity) {
        int* newData = utils::allocIntArray(table.capacity * 2);
        for (int i = 0; i < table.size; i++) {
            newData[i] = table.data[i];
        }
        utils::freeIntArray(table.data);
        table.data = newData;
        table.size++;
        table.capacity = table.capacity * 2;
        return;
    }
    table.data[table.size++] = value;
}
