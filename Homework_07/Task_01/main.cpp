#include <cassert>
#include <iostream>

#define HW7_USE_RECURSION
#define HW7_OPT_MEMORY
#define HW7_RUN_TESTS

#ifdef HW7_USE_RECURSION
// Важно: ако използваме рекурсивно попълване на данните може много лесно да стигнем до Stack
// Overflow (понеже за всяка колона която проверяваме трабва да създадем стекова рамка по време
// на разгръщане на рекурсията). Затова броя на колоните е от решаващо значения (и размера на
// default-ния стек, който ни дава компилатора). 
static constexpr auto MAX_MATRIX_DIM = 1024;
#else
// При итеративното решение големината на входната матрицата няма значение
static constexpr auto MAX_MATRIX_DIM = 4096;
#endif

namespace detail {
// Алокира масив от цели числа и връща указател към него
int* allocArray(const int arrSize) {
    int* arr = new (std::nothrow) int[arrSize]{};
    assert(arr && "Failed to allocate memory");
    return arr;
}

// Освобождава паметта на подадения масив
void freeArray(int*& arr) {
    delete[] arr;
    arr = nullptr;
}

// Преалокира масива _arr_ с новия размер и почиства след себе си. Задължително новия размер трябва
// да бъде по-малък от стария, в обратния случай няма да работи правилно
void shrinkToFit(int*& arr, const int newSize) {
    int* newArr = allocArray(newSize);
    for (int i = 0; i < newSize; i++) {
        newArr[i] = arr[i];
    }
    freeArray(arr);
    arr = newArr;
}

// Генерира sparse масив с произволни числа в диапазона [0, 100) и връща указател към него
int* genSparseArray(const int numRows, const int numCols) {
    std::srand(42);
    int* sparseArr = allocArray(numRows * numCols);
    for (int i = 0; i < numRows; i++) {
        const int nonZeroCols = std::rand() % std::max(numCols >> 1, 1);
        for (int j = 0; j < nonZeroCols; j++) {
            const int colIdx = std::rand() % numCols;
            sparseArr[i * numCols + colIdx] = std::rand() % 100;
        }
    }
    return sparseArr;
}

// Принтира 1D масив
void printArray(const int* arr, const int size) {
    for (int i = 0; i < size; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

// Принтира 1D масив като 2D матрица
void printMatrix(const int* mat, const int numRows, const int numCols) {
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            std::cout << mat[i * numCols + j] << " ";
        }
        std::cout << std::endl;
    }
}
}  // namespace detail

// Съдържа в себе си всичко необходимо за пълноценна работа на една
// компресирана матрица, БЕЗ да се пази копие на оригиналната.
struct SparseMatrix {
    int* data = nullptr;  // Пази компресираните данни
    int* offsets = nullptr;  // Пази изместванията на редовете спрямо първия ред
    int* indices = nullptr;  // Пази линейните индекси на оригиналните данни
    int numRows = 0;
    int numCols = 0;
    int dataSize = 0;  // Размера на масивите _data_ и _indices_
};

// Попълва ред от оригиналната sparse матрица в компресирания вариант. Прави го по тривиалния начин,
// като преплъзва настоящия ред спрямо предишния докато намери подходящо място. Времевата сложност в
// най-лошия случай е O((numCols ^ 2) / 2), т.е O(numCols ^ 2), не ползва допълнителна памет
void fillSparseRow(SparseMatrix& sm, const int* mat, const int matSize, const int numCols,
                   const int currRow) {
    assert(matSize >= currRow * numCols && "Matrix size exceeded");
    // Попълва първия ред и връща
    if (currRow == 0) {
        sm.offsets[currRow] = 0;
        for (int i = 0; i < numCols; i++) {
            sm.data[i] = mat[i];
            if (sm.data[i] != 0) {
                sm.indices[i] = i;
            }
        }
        return;
    }

    const int dataStart = sm.offsets[currRow - 1];  // От къде започва предишния ред
    const int matStart = currRow * numCols;  // От къде започва реда на матрицата
    int rowOffset = 0;  // Брои отместавенето на настоящия ред спрямо предишния
    int dataIdx = dataStart;  // Ходи по компресираните данни
    int matIdx = matStart;  // Ходи по матрацата от където четем данните
    int currColIdx = 0;  // Брояч в диапазона [0, numCols)

    // Изчисляваме отместването на настоящия ред спрямо предния
    while (currColIdx < numCols) {
        if (sm.data[dataIdx] > 0 && mat[matIdx] > 0) {
            // Трябва да преплъзнем настоящия ред спрямо предния с 1
            rowOffset++;
            currColIdx = rowOffset;
            dataIdx = dataStart + rowOffset;
            matIdx = matStart;  // Връщаме в началото на реда
        } else {
            dataIdx++;
            matIdx++;
            currColIdx++;
        }
    }

    // Попълваме данните за настоящия ред
    for (int i = 0; i < numCols; i++) {
        sm.data[dataStart + rowOffset + i] += mat[matStart + i];
        if (mat[matStart + i] != 0) {
            sm.indices[dataStart + rowOffset + i] = matStart + i;
        }
    }

    // Попълваме отместването за настоящия ред
    sm.offsets[currRow] = sm.offsets[currRow - 1] + rowOffset;
}

// Рекурсивна функция която попълва ред от оригиналната sparse матрица в компресирания вариант.
// Прави го като обикаля по елементите на настощия ред и едновреммено с това пише в компресираните
// данни. При достигане на 2 ненулеви елемента един под друг - backtrack-ва до началото на реда
// (като почиства след себе си) и започва с променен офсет. Времевата сложност е O(numCols ^ 2),
// ползва допълнително памет (за стековите рамки) пропорционална на брая на колоните
void fillSparseRowRec(SparseMatrix& sm, const int* mat, const int dataStart, const int matStart,
                      const int currColIdx, int& offset, bool& mustExit) {
    if (currColIdx == sm.numCols) {  // Ако сме попълнили всички колони - излизаме
        mustExit = true;
        return;
    }

    const int currDataValue = sm.data[dataStart + offset + currColIdx];
    const int currMatValue = mat[matStart + currColIdx];
    if (currDataValue == 0 || currMatValue == 0) {  // Един от двата елемента е нула
        sm.data[dataStart + offset + currColIdx] += currMatValue;
        fillSparseRowRec(sm, mat, dataStart, matStart, currColIdx + 1, offset, mustExit);
        if (!mustExit) {  // Backtrack-ваме и почистваме (освен ако не трябва да излезнем)
            sm.data[dataStart + offset + currColIdx] -= currMatValue;
        }
    }

    // Ако сме се върнали в началото - увеличаваме отмественето и започваме отначало
    if (currColIdx == 0 && !mustExit) {
        offset++;
        fillSparseRowRec(sm, mat, dataStart, matStart, currColIdx, offset, mustExit);
    }
}

// Попълва ред от оригиналната sparse матрица в компресирания вариант. За всеки ред различен от
// първия извиква горната рекурсивна функция, която смята отместването на настоящия ред спрямо
// предходия и едновременно с това попълва данните в компресирана матрица
void fillSparseRowRec(SparseMatrix& sm, const int* mat, const int matSize, const int numCols,
                      const int currRow) {
    assert(matSize >= currRow * numCols && "Matrix size exceeded");
    // Попълва първия ред и връща
    if (currRow == 0) {
        sm.offsets[currRow] = 0;
        for (int i = 0; i < numCols; i++) {
            sm.data[i] = mat[i];
            if (sm.data[i] != 0) {
                sm.indices[i] = i;
            }
        }
        return;
    }

    const int dataStart = sm.offsets[currRow - 1];  // От къде започва предния ред
    const int matStart = currRow * numCols;  // От къде започва реда на матрицата
    int rowOffset = 0;  // Брои отместавенето на настоящия ред спрямо предишния
    bool exit = false;  // Помощна променлива която ни служи за излизане от рекурсията

    // Изчисляваме отместването и попълваме кампресираните данни
    fillSparseRowRec(sm, mat, dataStart, matStart, 0, rowOffset, exit);

    // Попълваме индексите на ненулевите елементи
    for (int i = 0; i < numCols; i++) {
        if (mat[matStart + i] != 0) {
            sm.indices[dataStart + rowOffset + i] = matStart + i;
        }
    }

    // Попълваме отместването за настоящия ред
    sm.offsets[currRow] = sm.offsets[currRow - 1] + rowOffset;
}

// Алокира памет за компресираното представяне + каквато помощна информация е необходима,
// извършва компресирането и връща структура, съдържаща всички данни.
// ВАЖНО: не можем да сметнем предварително колко памет ще е нужна за компресирането
// => за целите на това домашно може да алокирате масив с размер numRows*numCols, и да попълвате в
// него.
SparseMatrix makeSparse(const int* mat, const int numRows, const int numCols) {
    SparseMatrix sm;
    sm.data = detail::allocArray(numRows * numCols);
    sm.offsets = detail::allocArray(numRows);
    sm.indices = detail::allocArray(numRows * numCols);
    sm.numRows = numRows;
    sm.numCols = numCols;
    sm.dataSize = numRows * numCols;

    const int matSize = numRows * numCols;
    for (int i = 0; i < numRows; i++) {
#ifdef HW7_USE_RECURSION
        fillSparseRowRec(sm, mat, matSize, numCols, i);
#else
        fillSparseRow(sm, mat, matSize, numCols, i);
#endif
    }

#ifdef HW7_OPT_MEMORY
    sm.dataSize = sm.offsets[numRows - 1] + numCols;
    detail::shrinkToFit(sm.data, sm.dataSize);
    detail::shrinkToFit(sm.indices, sm.dataSize);
#endif
    return sm;
}

// Връща елемента на дадената позиция в оригиналната матрица.
int get(const SparseMatrix& sm, const int row, const int col) {
    assert(row >= 0 && row < sm.numRows && "Row is out of bounds");
    assert(col >= 0 && col < sm.numCols && "Col is out of bounds");
    const int currIdx = row * sm.numCols + col;
    if (sm.indices[sm.offsets[row] + col] != currIdx) {
        return 0;
    }
    return sm.data[sm.offsets[row] + col];
}

// Освобождава паметта, алокирана в makeSparse() и съхранена в член-данните на sm.
void freeSparse(SparseMatrix& sm) {
    detail::freeArray(sm.data);
    detail::freeArray(sm.offsets);
    detail::freeArray(sm.indices);
}

namespace tests {
void testEasy(const int* mat, const int numRows, const int numCols) {
    SparseMatrix sm = makeSparse(mat, numRows, numCols);
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            assert(mat[i * numCols + j] == 0 || mat[i * numCols + j] == get(sm, i, j));
        }
    }
    freeSparse(sm);
}

void testHard(const int* mat, const int numRows, const int numCols) {
    SparseMatrix sm = makeSparse(mat, numRows, numCols);
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            // Проблем: ако mat[i][j] == 0, в компресираното представяне може
            // тази дупка да е била запълнена от елемент от различен ред :(
            // Как можем да ги различим с възможно най-малко допълнителна информация, съхранявана в
            // SparseMatrix обекта?
            assert(mat[i * numCols + j] == get(sm, i, j));
        }
    }
    freeSparse(sm);
}
}  // namespace tests

void runTests(const int* mat, const int numRows, const int numCols) {
    std::cout << "Running tests for correct compression for matrix with dims " << numRows << " x "
              << numCols << "..." << std::endl;
    tests::testEasy(mat, numRows, numCols);
    std::cout << "[EASY TEST RUN SUCCESSFULLY]" << std::endl;
    tests::testHard(mat, numRows, numCols);
    std::cout << "[HARD TEST RUN SUCCESSFULLY]" << std::endl;
}

int main() {
#ifdef HW7_RUN_TESTS
    const int rows = MAX_MATRIX_DIM;
    const int cols = MAX_MATRIX_DIM;
    std::cout << "Start generating sparse matrix with dims " << rows << " x " << cols << "...\n";
    int* arr = detail::genSparseArray(rows, cols);
    runTests(arr, rows, cols);
    detail::freeArray(arr);
#endif

    // Създава произволена sparse матрица като от входа взема само броя на редовете и колоните
    int numRows = 0, numCols = 0;
    std::cout << "Enter number of rows: ";
    std::cin >> numRows;
    std::cout << "Enter number of cols: ";
    std::cin >> numCols;
    assert(numRows > 0 && numCols >> 0 && "Failed to read positive int");
    std::cout << "Generating random sparse matrix with dims " << numRows << " x " << numCols
              << "..." << std::endl;

    // Генерира произволна sparse матрица
    int* arr2 = detail::genSparseArray(numRows, numCols);
    std::cout << "Generated sparse matrix" << std::endl;
    detail::printMatrix(arr2, numRows, numCols);

    // Стартира тестове за коректност
    runTests(arr2, numRows, numCols);

    // Създава копресирана sparse матрица и и принтира данните
    SparseMatrix sm = makeSparse(arr2, numRows, numCols);
    std::cout << "Compressed data" << std::endl;
    detail::printArray(sm.data, sm.dataSize);
    std::cout << "Row offsets" << std::endl;
    detail::printArray(sm.offsets, sm.numRows);
    std::cout << "Original data indices" << std::endl;
    detail::printArray(sm.indices, sm.dataSize);

    // Освобождаваме паметта
    detail::freeArray(arr2);
    freeSparse(sm);

    return 0;
}
