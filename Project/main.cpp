// C++ system includes
#include <fstream>
#include <sstream>
#include <string>

// Own includes
#include "Utils.h"

static constexpr auto MAX_CIRCUITS = 100;

// Съдържа данните на интегрална схема
struct IntegratedCircuit {
    std::string name = "";
    std::string expr = "";
    CharVector tokenizedExpr;
    CharVector arguments;
};

// Съдържа аргументите за вход на интегрална схема
struct CircuitInput {
    std::string circuitName = "";  // Таргетната ис
    IntVector args;
};

// Пази всички интегрални схеми в програмата
struct CircuitStorage {
    IntegratedCircuit* circuits = nullptr;
    int size = 0;
    int capacity = 0;
};

namespace utils {
// Парсва име на файл
std::string getFileName(std::istream& istream) {
    std::string fileName;
    std::getline(istream >> std::ws, fileName);
    fileName = fileName.substr(fileName.find_first_of('\"') + 1, fileName.find_last_of('\"') - 1);
    return fileName;
}

// Принтира данните за дадена ис
void printCircuit(const IntegratedCircuit& circuit) {
    const int argSize = circuit.arguments.size;
    std::cout << circuit.name << "(";
    for (int i = 0; i < argSize - 1; i++) {
        std::cout << circuit.arguments.data[i] << ", ";
    }
    std::cout << circuit.arguments.data[argSize - 1] << ") " << circuit.expr << "\n";
}

// Проверява дали входовете на ис са същите като тези които се използват в логическия израз
bool validateCircuit(const IntegratedCircuit& circuit) {
    for (int i = 0; i < circuit.tokenizedExpr.size; i++) {
        const auto token = circuit.tokenizedExpr.data[i];
        if (token == '&' || token == '|' || token == '!' || token == '(' || token == ')') {
            continue;
        }
        int j = 0;
        for (; j < circuit.arguments.size; j++) {
            const auto arg = circuit.arguments.data[j];
            if (token == arg) {
                break;
            }
        }

        if (j == circuit.arguments.size) {
            std::cerr << "Found token {" << token << "} that is not valid operator or operand.\n";
            return false;
        }
    }
    return true;
}

// Замества входовете на логичиския израз с конкретни цифрови стойности взети от RUN команда или
// генерирани автоматично
CharVector getInputExpr(const IntegratedCircuit& circuit, const CircuitInput& input) {
    CharVector inputExpr = makeCharVector(100);
    for (int i = 0; i < circuit.tokenizedExpr.size; i++) {
        const char token = circuit.tokenizedExpr.data[i];
        pushToCharVector(inputExpr, token);
    }

    for (int i = 0; i < input.args.size; i++) {
        for (int j = 0; j < circuit.tokenizedExpr.size; j++) {
            if (circuit.tokenizedExpr.data[j] == circuit.arguments.data[i]) {
                inputExpr.data[j] = (char)input.args.data[i] + '0';
            }
        }
    }
    return inputExpr;
}

// Превръща входен израз на ис в токени
void tokenizeExpression(CharVector& tokens, const std::string& expr) {
    for (const auto ch : expr) {
        if (ch == ' ' || ch == '\"') {
            continue;
        }
        pushToCharVector(tokens, ch);
    }
}

// Връща предходността на логическите оператори
int getPrecedence(const char op) {
    switch (op) {
    case '!':
        return 3;
    case '&':
        return 2;
    case '|':
        return 1;
    default:
        std::cerr << "Unknown operator found: " << op << std::endl;
    }
    return -1;
}

// Превръща инфиксен запис на логически израз в постфиксен (използва Shunting Yard алгоритъма)
CharVector convertInfixToPostfix(const CharVector& infixTokens) {
    CharVector operators = makeCharVector(infixTokens.capacity);
    CharVector postfixExpr = makeCharVector(infixTokens.capacity);

    for (int i = 0; i < infixTokens.size; i++) {
        const char token = infixTokens.data[i];
        if (std::isdigit(token)) {
            pushToCharVector(postfixExpr, token);
        } else if (token == '(') {
            pushToCharVector(operators, token);
        } else if (token == ')') {
            char op = getCharVectorBack(operators);
            while (op != '(') {
                popFromCharVector(operators);
                pushToCharVector(postfixExpr, op);
                assert(operators.size > 0 && "Mismatch in parenthesis found");
                op = getCharVectorBack(operators);
            }
            popFromCharVector(operators);
        } else if (token == '!' || token == '&' || token == '|') {
            if (operators.size == 0) {
                pushToCharVector(operators, token);
                continue;
            }
            char op = getCharVectorBack(operators);
            while (op != '(' && operators.size > 0 && getPrecedence(op) > getPrecedence(token)) {
                popFromCharVector(operators);
                pushToCharVector(postfixExpr, op);
                if (operators.size > 0) {
                    op = getCharVectorBack(operators);
                }
            }
            pushToCharVector(operators, token);
        }
    }

    while (operators.size > 0) {
        const char op = getCharVectorBack(operators);
        pushToCharVector(postfixExpr, op);
        popFromCharVector(operators);
    }

    // Освобождаваме паметта
    clearCharVector(operators);
    return postfixExpr;
}

// Изпълнява логически израз в постфиксен запис
int evaluatePostfixExpr(const CharVector& postfixExpr) {
    Stack exprStack;
    for (int i = 0; i < postfixExpr.size; i++) {
        const char token = postfixExpr.data[i];
        if (std::isdigit(token)) {
            pushToStack(exprStack, token - '0');
        } else {
            switch (token) {
            case '!': {
                assert(getStackSize(exprStack) >= 1);
                const int operand = peekStack(exprStack);
                popFromStack(exprStack);
                pushToStack(exprStack, !operand);
            } break;
            case '&': {
                assert(getStackSize(exprStack) >= 2);
                const int operand2 = peekStack(exprStack);
                popFromStack(exprStack);
                const int operand1 = peekStack(exprStack);
                popFromStack(exprStack);
                pushToStack(exprStack, operand1 && operand2);
            } break;
            case '|': {
                assert(getStackSize(exprStack) >= 2);
                const int operand2 = peekStack(exprStack);
                popFromStack(exprStack);
                const int operand1 = peekStack(exprStack);
                popFromStack(exprStack);
                pushToStack(exprStack, operand1 || operand2);
            } break;
            default:
                std::cerr << "Unknown token found: " << token << std::endl;
                assert(false);
            }
        }
    }
    assert(getStackSize(exprStack) == 1 && "Something is wrong");
    return peekStack(exprStack);
}
}  // namespace utils

// Прави нова ис
IntegratedCircuit makeIntegratedCircuit() {
    IntegratedCircuit circuit;
    circuit.tokenizedExpr = makeCharVector(100);
    circuit.arguments = makeCharVector(100);
    return circuit;
}

// Освобождава паметта на дадената ис
void freeIntegratedCircuit(IntegratedCircuit& circuit) {
    clearCharVector(circuit.tokenizedExpr);
    clearCharVector(circuit.arguments);
    circuit.name = "";
    circuit.expr = "";
}

// Прави нов вход за ис
CircuitInput makeCircuitInput() {
    CircuitInput input;
    input.args = makeIntVector(100);
    return input;
}

// Освобождава паметта на дадения вход
void freeCircuitInput(CircuitInput& input) {
    clearIntVector(input.args);
    input.circuitName = "";
}

// Прави хранилище за ис
CircuitStorage makeCircuitStorage(const int capacity) {
    CircuitStorage storage;
    storage.circuits = new IntegratedCircuit[capacity];
    storage.capacity = capacity;
    return storage;
}

// Освобождава паметта за всчки ис в хронилището
void freeCircuitStorage(CircuitStorage& storage) {
    for (int i = 0; i < storage.size; i++) {
        freeIntegratedCircuit(storage.circuits[i]);
    }
    delete[] storage.circuits;
    storage.circuits = nullptr;
    storage.capacity = 0;
    storage.size = 0;
}

// Прави дълбоко копие на дадена ис в хранилището
void addCircuit(CircuitStorage& storage, const IntegratedCircuit& circuit) {
    assert(storage.size < storage.capacity && "Circuit storage capacity exceeded");
    // Правим нова ис
    storage.circuits[storage.size] = makeIntegratedCircuit();
    auto& storageCircuit = storage.circuits[storage.size];
    // Копираме аргументите (входа) на схемата
    for (int i = 0; i < circuit.arguments.size; i++) {
        const char currArg = circuit.arguments.data[i];
        pushToCharVector(storageCircuit.arguments, currArg);
    }
    // Копираме логическия израз
    for (int i = 0; i < circuit.tokenizedExpr.size; i++) {
        const char currToken = circuit.tokenizedExpr.data[i];
        pushToCharVector(storageCircuit.tokenizedExpr, currToken);
    }
    // Копираме останалите данни
    storageCircuit.name = circuit.name;
    storageCircuit.expr = circuit.expr;
    // Коригираме размера на хранилището
    storage.size++;
}

// Принтираме всички налични ис
void printStorage(const CircuitStorage& storage) {
    for (int i = 0; i < storage.size; i++) {
        utils::printCircuit(storage.circuits[i]);
    }
}

// Търсим ис в хранилището и я връщаме при нейното наличие
IntegratedCircuit* findCircuit(const CircuitStorage& storage, const std::string& name) {
    for (int i = 0; i < storage.size; i++) {
        if (storage.circuits[i].name == name) {
            return &storage.circuits[i];
        }
    }
    return nullptr;
}

// Проверяваме дали ис се съдържа в хранилището
bool hasCircuit(const CircuitStorage& storage, const std::string& name) {
    for (int i = 0; i < storage.size; i++) {
        if (storage.circuits[i].name == name) {
            return true;
        }
    }
    return false;
}

// Парсваме ис от стандартния вход
IntegratedCircuit parseIntegratedCircuit(std::istream& istream) {
    IntegratedCircuit circuit = makeIntegratedCircuit();

    std::getline(istream >> std::ws, circuit.name, '(');
    char arg;
    while (istream.get(arg) && arg != ')') {
        if (arg == ' ' || arg == ',') {
            continue;
        }
        pushToCharVector(circuit.arguments, arg);
    }

    std::string expression;
    std::getline(istream, expression);
    circuit.expr = expression.substr(expression.find_first_of("\""));

    utils::tokenizeExpression(circuit.tokenizedExpr, circuit.expr);
    if (!utils::validateCircuit(circuit)) {
        freeIntegratedCircuit(circuit);
    }

    return circuit;
}

// Парсваме вход за ис
CircuitInput parseRunCommand(std::istream& istream) {
    CircuitInput input = makeCircuitInput();
    std::getline(istream >> std::ws, input.circuitName, '(');

    char arg;
    while (istream.get(arg) && arg != ')') {
        if (arg == ' ' || arg == ',') {
            continue;
        }
        if (std::isdigit(arg)) {
            pushToIntVector(input.args, arg - '0');
        } else {
            assert(false && "Invalid argument parsed for RUN command");
        }
    }

    return input;
}

// Изпълняваме ис с дадения вход
int runCircuit(const IntegratedCircuit& circuit, const CircuitInput& input) {
    CharVector inputInfixExpr = utils::getInputExpr(circuit, input);
    CharVector postfixExpr = utils::convertInfixToPostfix(inputInfixExpr);
    const int result = utils::evaluatePostfixExpr(postfixExpr);
    // Освобождаваме паметта
    clearCharVector(inputInfixExpr);
    clearCharVector(postfixExpr);
    return result;
}

// Принтираме всички възможни комбинации за вход на ис заедно с резултата
void printAll(const IntegratedCircuit& circuit, CircuitInput& input, const int currIdx) {
    if (currIdx == input.args.size) {
        for (int i = 0; i < input.args.size - 1; i++) {
            std::cout << input.args.data[i] << " | ";
        }
        std::cout << input.args.data[input.args.size - 1] << " | res: ";
        const int res = runCircuit(circuit, input);
        std::cout << res << std::endl;
        return;
    }

    for (int i = 0; i <= 1; i++) {
        input.args.data[currIdx] = i;
        printAll(circuit, input, currIdx + 1);
    }
}

// Изпълнява командата ALL
void runAllCommand(IntegratedCircuit& circuit) {
    CircuitInput input = makeCircuitInput();
    input.circuitName = circuit.name;
    for (int i = 0; i < circuit.arguments.size; i++) {
        pushToIntVector(input.args, 0);
    }
    std::cout << "Execute " << circuit.name << " " << circuit.expr << std::endl;
    printAll(circuit, input, 0);
    // Освобождаваме паметта
    freeCircuitInput(input);
}

// Парсва таблица на истинност от даден файл
TruthTable parseTruthTable(const std::string& file) {
    TruthTable table = makeTruthTable(100);
    std::ifstream inputFile(file, std::ios::in);
    // assert(inputFile.is_open() && "Input file is not open");
    if (!inputFile.is_open()) {
        std::cerr << "Failed to parse truth table for file with name " << file << ".\n";
        std::cerr
            << "Maybe the file name is wrong or the file is missing from the working directory.\n";
        freeTruthTable(table);
        return table;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream istream(line);
        int entry = -1;
        int colCounter = 0;
        while (istream >> entry) {
            colCounter++;
            pushToTruthTable(table, entry);
        }
        table.rows++;
        table.cols = colCounter;
    }
    return table;
}

// Прави синтез по 1 за дадения вход
std::string synthLogicFuncByOne(const int* input, const int inputSize) {
    std::string result = "(";
    for (int i = 0; i < inputSize; i++) {
        const char arg = i + 49 + '0';
        if (input[i] == 0) {
            result.append("!");
        }
        result += arg;
        if (i != inputSize - 1) {
            result += " & ";
        }
    }
    result += ")";
    return result;
}

// Изпълнява командата FIND
std::string runFindCommand(const TruthTable& table) {
    std::string logicFunc = "\"";
    for (int i = 0; i < table.rows; i++) {
        // Вземаме резултата на ф-та за настоящия ред
        const int res = table.data[i * table.cols + table.cols - 1];
        if (1 == res) {
            // Копираме входовете на ф-ята
            int* currInput = utils::allocIntArray(table.cols - 1);
            for (int k = 0; k < table.cols - 1; k++) {
                currInput[k] = table.data[i * table.cols + k];
            }
            // Синтезираме по 1
            logicFunc += synthLogicFuncByOne(currInput, table.cols - 1);
            logicFunc += " | ";
            // Освобождаваме паметта
            utils::freeIntArray(currInput);
        }
    }

    if (!logicFunc.empty()) {
        logicFunc = logicFunc.substr(0, logicFunc.find_last_of(')') + 1);
        logicFunc.append("\"");
    }
    return logicFunc;
}

int main() {
    std::cout << "Console simulator of Digital Integrated Circuits\nEnter command: ";
    CircuitStorage storage = makeCircuitStorage(MAX_CIRCUITS);

    std::string input;
    while (std::getline(std::cin, input) && input != "EXIT") {
        std::istringstream istream(input);
        std::string command;
        istream >> command;
        // Въвеждаме интегрална схема
        if (command == "DEFINE") {
            IntegratedCircuit circuit = parseIntegratedCircuit(istream);
            if (circuit.name.empty()) {
                std::cerr << "Invalid expression entered. Skip DEFINE command.\nEnter command: ";
                freeIntegratedCircuit(circuit);
                continue;
            }
            if (hasCircuit(storage, circuit.name)) {
                std::cerr << "Integrated circuit with name " << circuit.name
                          << " already exist. Skip DEFINE command." << std::endl;
            } else {
                addCircuit(storage, circuit);
            }
            // Освобождаваме паметта
            freeIntegratedCircuit(circuit);
        }
        // Изпълняваме интегрална схема по име и входни параметри
        else if (command == "RUN") {
            CircuitInput input = parseRunCommand(istream);
            IntegratedCircuit* circuit = findCircuit(storage, input.circuitName);
            if (!circuit) {
                std::cerr << "Circuit with name " << input.circuitName
                          << " does NOT exist.\nSkip RUN command." << std::endl;
            } else {
                const int res = runCircuit(*circuit, input);
                std::cout << res << std::endl;
            }
            // Освобождаваме паметта
            freeCircuitInput(input);
        }
        // Изпълняваме дадена интегрална схема с всички възможни входове
        else if (command == "ALL") {
            std::string circuitName;
            istream >> circuitName;
            IntegratedCircuit* circuit = findCircuit(storage, circuitName);
            if (!circuit) {
                std::cerr << "Circuit with name " << circuitName
                          << " does NOT exist.\nSkip ALL command." << std::endl;
            } else {
                runAllCommand(*circuit);
            }
        }
        // Изчисляваме интегрална схема по дадена таблица на истинност от файл
        else if (command == "FIND") {
            const std::string fileName = utils::getFileName(istream);
            TruthTable table = parseTruthTable(fileName);
            if (!table.data) {
                std::cerr << "Skip FIND command.\nEnter command: ";
                continue;
            }
            utils::printTruthTable(table);
            const std::string logicFunc = runFindCommand(table);
            std::cout << logicFunc << std::endl;
            // Освобождаваме паметта
            freeTruthTable(table);
        }
        // Принтираме всички налични интеглани схеми
        else if (command == "PRINT") {
            printStorage(storage);
        }
        std::cout << "Enter command: ";
    }

    // Програмата би трябвало да leak-ва само служебни байтове
    // всички CharVector-и IntVector-и и структури които ги съдържат се почистват правилно
    freeCircuitStorage(storage);

    return 0;
}
