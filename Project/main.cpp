// C++ system includes
#include <fstream>
#include <sstream>
#include <string>

// Own includes
#include "Utils.h"

static constexpr auto MAX_CIRCUITS = 100;

struct IntegratedCircuit {
    std::string name = "";
    std::string expr = "";
    CharVector tokenizedExpr;
    CharVector arguments;
};

struct CircuitInput {
    std::string circuitName = "";
    IntVector args;
};

struct CircuitStorage {
    IntegratedCircuit* circuits = nullptr;
    int size = 0;
    int capacity = 0;
};

namespace utils {
std::string getFileName(std::istream& istream) {
    std::string fileName;
    std::getline(istream >> std::ws, fileName);
    fileName = fileName.substr(fileName.find_first_of('\"') + 1, fileName.find_last_of('\"') - 1);
    return fileName;
}

CharVector tokenizeExpression(const std::string& expr) {
    CharVector tokens = makeCharVector(100);
    for (const auto ch : expr) {
        if (ch == ' ' || ch == '\"') {
            continue;
        }
        pushToCharVector(tokens, ch);
    }
    return tokens;
}

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

// Замества аргументите на логичиския израз с конкретни цифрови стойности взети от RUN команда или
// генерирани автоматично. Връща резултата като масив от char-ове.
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

CharVector convertInfixToPostfix(const CharVector& infixTokens) {
    CharVector operators = makeCharVector(infixTokens.capacity);
    CharVector postfixExpr = makeCharVector(infixTokens.capacity);

    for (int i = 0; i < infixTokens.size; i++) {
        const char token = infixTokens.data[i];
        if (std::isdigit(token)) {
            pushToCharVector(postfixExpr, token);
        } else if (token == '(' || token == '!' || token == '&' || token == '|') {
            pushToCharVector(operators, token);
        } else if (token == ')') {
            char op = getCharVectorBack(operators);
            while (op != '(') {
                popFromCharVector(operators);
                pushToCharVector(postfixExpr, op);
                op = getCharVectorBack(operators);
            }
            popFromCharVector(operators);
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
    assert(getStackSize(exprStack) == 1 && "Something is wrond");
    return peekStack(exprStack);
}
}  // namespace utils

IntegratedCircuit makeIntegratedCircuit() {
    IntegratedCircuit circuit;
    circuit.tokenizedExpr = makeCharVector(100);
    circuit.arguments = makeCharVector(100);
    return circuit;
}

void freeIntegratedCicruit(IntegratedCircuit& circuit) {
    clearCharVector(circuit.tokenizedExpr);
    clearCharVector(circuit.arguments);
    circuit.name = "";
    circuit.expr = "";
}

CircuitInput makeCircuitInput() {
    CircuitInput input;
    input.args = makeIntVector(100);
    return input;
}

void freeCircuitInput(CircuitInput& input) {
    clearIntVector(input.args);
    input.circuitName = "";
}

CircuitStorage makeCircuitStorage(const int capacity) {
    CircuitStorage storage;
    storage.circuits = new IntegratedCircuit[capacity];
    storage.capacity = capacity;
    return storage;
}

void freeCircuitStorage(CircuitStorage& storage) {
    for (int i = 0; i < storage.size; i++) {
        freeIntegratedCicruit(storage.circuits[i]);
    }
    delete[] storage.circuits;
    storage.circuits = nullptr;
    storage.capacity = 0;
    storage.size = 0;
}

void addCircuit(CircuitStorage& storage, const IntegratedCircuit& circuit) {
    assert(storage.size < storage.capacity && "Circuit storage capacity exceeded");
    // Копираме интегралната схема
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

IntegratedCircuit* findCircuit(const CircuitStorage& storage, const std::string& name) {
    for (int i = 0; i < storage.size; i++) {
        if (storage.circuits[i].name == name) {
            return &storage.circuits[i];
        }
    }
    return nullptr;
}

bool hasCircuit(const CircuitStorage& storage, const std::string& name) {
    for (int i = 0; i < storage.size; i++) {
        if (storage.circuits[i].name == name) {
            return true;
        }
    }
    return false;
}

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
    circuit.tokenizedExpr = utils::tokenizeExpression(circuit.expr);
    if (!utils::validateCircuit(circuit)) {
        freeIntegratedCicruit(circuit);
    }

    return circuit;
}

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

int runCircuit(const IntegratedCircuit& circuit, const CircuitInput& input) {
    CharVector inputInfixExpr = utils::getInputExpr(circuit, input);
    CharVector postfixExpr = utils::convertInfixToPostfix(inputInfixExpr);
    const int result = utils::evaluatePostfixExpr(postfixExpr);
    // Освобождаваме паметта
    clearCharVector(inputInfixExpr);
    clearCharVector(postfixExpr);
    return result;
}

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

TruthTable parseTruthTable(const std::string& file) {
    TruthTable table = makeTruthTable(100);
    std::ifstream inputFile(file, std::ios::in);
    assert(inputFile.is_open() && "Input file is not open");

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

void runFindCommand(const TruthTable& table) {
    for (int i = 0; i < table.rows; i++) {
        // Вземаме резултата на ф-та за настоящия ред
        const int res = table.data[i * table.cols + table.cols - 1];
        if (1 == res) {
            // Копираме входовете на ф-ята
            int* currInput = utils::allocIntArray(table.cols - 1);
            for (int k = 0; k < table.cols; k++) {
                
            }
            // Освобождаваме паметта 
            utils::freeIntArray(currInput);
        }
    }
}

int main() {
    std::cout << "Console simulator of Digital Integrated Circuits\nEnter command: ";
    CircuitStorage storage = makeCircuitStorage(MAX_CIRCUITS);

    std::string input;
    while (std::getline(std::cin, input) && input != "EXIT") {
        std::istringstream istream(input);
        std::string command;
        istream >> command;
        if (command == "DEFINE") {
            IntegratedCircuit circuit = parseIntegratedCircuit(istream);
            if (circuit.name.empty()) {
                std::cerr << "Invalid expression entered. Skip DEFINE command.\nEnter command: ";
                freeIntegratedCicruit(circuit);
                continue;
            }
            if (hasCircuit(storage, circuit.name)) {
                std::cerr << "Integrated circuit with name " << circuit.name
                          << " already exist. Skip DEFINE command." << std::endl;
            } else {
                addCircuit(storage, circuit);
            }
            // Освобождаваме паметта
            freeIntegratedCicruit(circuit);
        } else if (command == "RUN") {
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
        } else if (command == "ALL") {
            std::string circuitName;
            istream >> circuitName;
            IntegratedCircuit* circuit = findCircuit(storage, circuitName);
            if (!circuit) {
                std::cerr << "Circuit with name " << circuitName
                          << " does NOT exist.\nSkip ALL command." << std::endl;
            } else {
                runAllCommand(*circuit);
            }
        } else if (command == "FIND") {
            const std::string fileName = utils::getFileName(istream);
            TruthTable table = parseTruthTable(fileName);
            runFindCommand(table);
            // Освобождаваме паметта
            freeTruthTable(table);
        }
        std::cout << "Enter command: ";
    }

    freeCircuitStorage(storage);

    return 0;
}
