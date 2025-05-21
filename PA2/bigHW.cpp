#ifndef __PROGTEST__

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <array>
#include <utility>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <variant>
#include <optional>
#include <compare>
#include <charconv>
#include <span>
#include <utility>
#include "expression.h"

constexpr unsigned SPREADSHEET_CYCLIC_DEPS = 0x01;
constexpr unsigned SPREADSHEET_FUNCTIONS = 0x02;
constexpr unsigned SPREADSHEET_FILE_IO = 0x04;
constexpr unsigned SPREADSHEET_SPEED = 0x08;
constexpr unsigned SPREADSHEET_PARSER = 0x10;


using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>;

#endif /* __PROGTEST__ */

using namespace std;

class CPos {
    string numReplaceToLett(int num);
    int lettReaplaceToNum(std::string_view str);

    bool correct(std::string_view str, size_t &digitPos);

public:
    CPos(std::string_view str);

    CPos(int row, int col);

    bool operator==(CPos rhs) const;

    bool operator<(CPos rhs) const;

    int row = 0;
    int col = 0;

    bool isRowABS = false;
    bool isColABS = false;
};


string CPos::numReplaceToLett(int num) {
    std::string result;
    if (num >= 0) {
        int alph = num / 26;
        int rem = num % 26;
        result = ('A' + rem);
        if (alph > 0) {
            result = numReplaceToLett(alph - 1) + result;
        }
    }
    return result;
}

int CPos::lettReaplaceToNum(std::string_view str) {
    int result = 0;
    auto len = str.length();
    for (size_t i = 0; i < len; ++i) {
        auto c = str.at(len - i - 1);
        result += static_cast<int>(c - 'A' + 1) * std::pow(27, i);
    }
    return result;
}

bool CPos::correct(std::string_view str, size_t &digitPos) {
    bool seen_digit = false;
    bool seen_letter = false;
    bool valid = true;

    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
        if (std::isdigit(c)) {
            if (!seen_digit) {
                seen_digit = true;
                digitPos = i;
            }
            if (!seen_letter) {
                valid = false;
                break;
            }
        } else if (std::isalpha(c)) {
            if (seen_digit) {
                valid = false;
                break;
            }
            seen_letter = true;
        } else if (c != '$') {
            valid = false;
            break;
        }
    }

    return valid && seen_digit && seen_letter;
}


CPos::CPos(std::string_view str) {
    size_t digitPos = str.find_first_of("0123456789");
    if (digitPos == std::string_view::npos || !correct(str, digitPos)) {
        throw std::invalid_argument("Invalid cell identifier.");
    }

    isColABS = (str.find('$') != std::string_view::npos && std::isalpha(str[str.find('$') + 1]));

    isRowABS = str[digitPos - 1] == '$';

    std::string colL;
    colL.reserve(digitPos);
    for (size_t j = 0; j < digitPos; ++j) {
        if (std::isalpha(str[j])) {
            colL.push_back(std::toupper(str[j]));
        }
    }

    col = lettReaplaceToNum(colL);
    row = std::stoi(std::string(str.substr(digitPos)));
}

CPos::CPos(int row, int col) : row(row), col(col) {}

bool CPos::operator==(CPos rhs) const {
    return row == rhs.row && col == rhs.col;
}

bool CPos::operator<(CPos rhs) const {
    return std::tie(row, col) < std::tie(rhs.row, rhs.col);
}


class CSpreadsheet;

class MyBuilder;

class MainNode {
    string contents;
public:

    virtual CValue evaluate() = 0;

    virtual shared_ptr<MainNode> clone() const = 0;

    virtual ~MainNode() = default;
};

class NumNode : public MainNode {
    double value;
public:
    NumNode(double val);

    shared_ptr<MainNode> clone() const override;

    CValue evaluate() override;
};

class StrNode : public MainNode {
    std::string value;
public:
    StrNode(std::string val);

    shared_ptr<MainNode> clone() const override;

    CValue evaluate() override;
};

class RefNode : public MainNode {
    shared_ptr<MyBuilder> reference;
    CSpreadsheet &spreadsheet;
public:
    RefNode(shared_ptr<MyBuilder> reference, CSpreadsheet &spreadsheet);

    shared_ptr<MainNode> clone() const override;

    CValue evaluate() override;
};

class BinNode : public MainNode {
    std::shared_ptr<MainNode> left, right;
    std::function<CValue(CValue, CValue)> operation;
public:
    BinNode(std::shared_ptr<MainNode> l, std::shared_ptr<MainNode> r, std::function<CValue(CValue, CValue)> func);

    shared_ptr<MainNode> clone() const override;

    CValue evaluate() override;
};

class UnarNode : public MainNode {
    std::shared_ptr<MainNode> operand;
    std::function<CValue(CValue)> operation;
public:
    UnarNode(std::shared_ptr<MainNode> opnd, std::function<CValue(CValue)> func);

    shared_ptr<MainNode> clone() const override;

    CValue evaluate() override;
};

struct Offset {
    int w = 0;
    int h = 0;
};

class MyBuilder : public CExprBuilder {
private:
    stack<shared_ptr<MainNode>> m_stack;
    CSpreadsheet &spreadsheet;
    string contents;
    bool isBeingEvaluated = false;
    Offset offset = {0, 0};

public:
    MyBuilder(CSpreadsheet &spreadsheet, string contents, int w = 0, int h = 0);

    MyBuilder(MyBuilder &other);

    MyBuilder &operator=(MyBuilder &other);

    Offset GetOffset() const;

    void opAdd() override;

    void opSub() override;

    void opMul() override;

    void opDiv() override;

    void opPow() override;

    void opNeg() override;

    void opEq() override;

    void opNe() override;

    void opLt() override;

    void opLe() override;

    void opGt() override;

    void opGe() override;

    void valNumber(double num) override;

    void valString(std::string val) override;

    void valReference(std::string ref) override;

    void valRange(std::string range) override;

    void funcCall(std::string fnName, int paramCount) override;

    CValue evaluateExpression();

    string GetText() const;

    CValue GetValue();

    MyBuilder &setCell(string contents, int col = 0, int row = 0);
};


class CSpreadsheet {
public:
    static unsigned capabilities() {
        return SPREADSHEET_CYCLIC_DEPS |
               SPREADSHEET_FUNCTIONS | SPREADSHEET_FILE_IO | SPREADSHEET_SPEED;
    }

    CSpreadsheet() = default;

    CSpreadsheet(const CSpreadsheet &other);

    CSpreadsheet &operator=(const CSpreadsheet &other);

    bool load(std::istream &is);

    bool save(std::ostream &os) const;

    bool setCell(CPos pos,
                 std::string contents, int w = 0, int h = 0);

    CValue getValue(CPos pos);

    shared_ptr<MyBuilder> getCell(CPos pos);

    void copyRect(CPos dst, CPos src, int w, int h);

private:
    map<CPos, shared_ptr<MyBuilder>> cells;
    std::set<CPos> non_empty_cells;
};

std::string escapeJSON(const std::string &input) {
    std::string output;
    for (char c: input) {
        switch (c) {
            case '\n':
                output += "\\n";
                break;
            case '\\':
                output += "\\\\";
                break;
            case '"':
                output += "\\\"";
                break;
            default:
                output += c;
        }
    }
    return output;
}

std::string unescapeJSON(const std::string &input) {
    std::string output;
    bool escape = false;

    for (char c: input) {
        if (escape) {
            switch (c) {
                case 'n':
                    output += '\n';
                    break;
                case '\\':
                    output += '\\';
                    break;
                case '"':
                    output += '"';
                    break;
                default:
                    output += c;
            }
            escape = false;
        } else if (c == '\\') {
            escape = true;
        } else {
            output += c;
        }
    }
    return output;
}

bool CSpreadsheet::load(std::istream &is) {
    if (!is)
        return false;

    std::string line;
    while (std::getline(is, line)) {
        std::istringstream streaming(line);
        std::string split;
        std::vector<std::string> parts;

        int partsCount = 0;
        while (partsCount < 4 && std::getline(streaming, split, ':')) {
            parts.push_back(unescapeJSON(split));
            partsCount++;
        }
        string remaining = "";
        while (std::getline(streaming, split, ':')) {
            remaining += split;
        }
        parts.push_back(unescapeJSON(remaining));

        if (parts.size() != 5)
            return false;

        Offset rowCol = {-1,-1};
        Offset offsetSecond = {0,0};

        try {
            rowCol.w = std::stoi(parts[0]);
            rowCol.h = std::stoi(parts[1]);
            offsetSecond.w = std::stoi(parts[2]);
            offsetSecond.h = std::stoi(parts[3]);
            CPos pos(rowCol.w, rowCol.h);
            auto cell = getCell(pos);
            cell->setCell( parts[4], offsetSecond.w, offsetSecond.h);
            parseExpression(parts[4],*cell);
        }
        catch (const std::invalid_argument &e) {
            return false;
        }

    }
    return true;
}

bool CSpreadsheet::save(std::ostream &os) const {
    if (!os)
        return false;

    for (const auto &cell: cells) {
        if (cell.second->GetText().empty()) {
            continue;
        }
        os << cell.first.row << ':' << cell.first.col << ':' << cell.second->GetOffset().w << ':'
           << cell.second->GetOffset().h << ':' << escapeJSON(cell.second->GetText()) << '\n';
    }
    return true;
}

bool CSpreadsheet::setCell(CPos pos,
                           std::string contents, int w, int h) {
    if (pos.col == 0 && pos.row == 0) {
        return false;
    }
    auto cellsIT = cells.find(pos);
    if (cellsIT == cells.end()) {
        try {
            auto customCExprBuilder = make_shared<MyBuilder>(*this, contents, w, h);
            parseExpression(contents, *customCExprBuilder);
            cells[pos] = customCExprBuilder;
        }
        catch (const std::invalid_argument &e) {
            return false;
        }
    } else {
        cellsIT->second->setCell(contents, w, h);
        parseExpression(contents, *cellsIT->second);
    }
    return true;
}


CValue CSpreadsheet::getValue(CPos pos) {
    auto cellsIT = cells.find(pos);
    if (cellsIT == cells.end()) {
        return CValue();
    } else {
        if (cellsIT->second == nullptr) {
            return CValue();
        }
        return cellsIT->second->GetValue();
    }
}

void CSpreadsheet::copyRect(CPos dst,
                            CPos src,
                            int w = 1,
                            int h = 1) {
    if (w <= 0 || h <= 0) return;
    if (dst == src && w == 1 && h == 1) return;

    map<CPos, std::pair<std::string, Offset>> cellData;

    for (int dy = 0; dy < h; ++dy) {
        for (int dx = 0; dx < w; ++dx) {
            CPos srcPos(src.row + dy, src.col + dx);
            std::string text = "";
            Offset offset = {0, 0};
            auto it = cells.find(srcPos);
            if (it != cells.end()) {
                text = it->second->GetText();
                offset = it->second->GetOffset();
            }
            cellData[srcPos] = {text, offset};
        }
    }

    for (int dy = 0; dy < h; ++dy) {
        for (int dx = 0; dx < w; ++dx) {
            CPos srcPos(src.row + dy, src.col + dx);
            CPos dstPos(dst.row + dy, dst.col + dx);

            auto &cell = cellData[srcPos];
            if (!cell.first.empty()) {
                setCell(dstPos, cell.first, cell.second.w + (dstPos.col - srcPos.col),
                        cell.second.h + (dstPos.row - srcPos.row));
            }
        }
    }
}

std::shared_ptr<MyBuilder> CSpreadsheet::getCell(CPos pos) {
    auto &cell = cells[pos];
    if (!cell) {
        cell = std::make_shared<MyBuilder>(*this, "");
    }
    return cell;
}

CSpreadsheet &CSpreadsheet::operator=(const CSpreadsheet &other) {
    if (this != &other) {
        cells.clear();
        for (const auto &pair: other.cells) {
            if (pair.second) {
                setCell(pair.first, pair.second->GetText());
            }
        }
    }
    return *this;
}

CSpreadsheet::CSpreadsheet(const CSpreadsheet &other) {
    for (const auto &pair: other.cells) {
        if (pair.second) {
            cells[pair.first] = std::make_shared<MyBuilder>(*pair.second);
        }
    }
}

NumNode::NumNode(double val) : value(val) {}

CValue NumNode::evaluate() {
    return value;
}

std::shared_ptr<MainNode> NumNode::clone() const {
    return std::make_shared<NumNode>(*this);
}

StrNode::StrNode(std::string val) : value(std::move(val)) {}

CValue StrNode::evaluate() {
    return value;
}

std::shared_ptr<MainNode> StrNode::clone() const {
    return std::make_shared<StrNode>(*this);
}

RefNode::RefNode(shared_ptr<MyBuilder> reference, CSpreadsheet &spreadsheet) : reference(
        std::move(reference)), spreadsheet(spreadsheet) {}

CValue RefNode::evaluate() {
    return reference ? reference->GetValue() : CValue();
}

std::shared_ptr<MainNode> RefNode::clone() const {
    std::shared_ptr<MyBuilder> res;
    if (reference) {
        auto posText = reference->GetText();
        CPos pos(posText);
        res = spreadsheet.getCell(pos);
    }
    return std::make_shared<RefNode>(res, spreadsheet);
}

BinNode::BinNode(std::shared_ptr<MainNode> l, std::shared_ptr<MainNode> r,
                 std::function<CValue(CValue, CValue)> func)
        : left(std::move(l)), right(std::move(r)), operation(std::move(func)) {}

CValue BinNode::evaluate() {
    if (!left || !right) {
        return CValue();
    }

    CValue valueLeft = left->evaluate();
    CValue valueRight = right->evaluate();

    if (std::holds_alternative<monostate>(valueLeft) || std::holds_alternative<monostate>(valueRight)) {
        return CValue();
    }

    return operation(valueLeft, valueRight);
}

std::shared_ptr<MainNode> BinNode::clone() const {
    return std::make_shared<BinNode>(left->clone(), right->clone(), operation);
}

UnarNode::UnarNode(std::shared_ptr<MainNode> opnd, std::function<CValue(CValue)> func)
        : operand(std::move(opnd)), operation(std::move(func)) {}

CValue UnarNode::evaluate() {
    if (operand == nullptr) {
        return CValue();
    }
    auto value = operand->evaluate();
    if (!holds_alternative<double>(value)) {
        return CValue();
    }
    auto result = operation(get<double>(value));
    return result;
}

std::shared_ptr<MainNode> UnarNode::clone() const {
    return std::make_shared<UnarNode>(operand->clone(), operation);
}

string MyBuilder::GetText() const {
    return contents;
}

CValue MyBuilder::GetValue() {
    if (isBeingEvaluated) {
        return CValue();
    }
    isBeingEvaluated = true;
    try {
        auto result = evaluateExpression();
        isBeingEvaluated = false;
        return result;
    } catch (const std::invalid_argument &e) {
        isBeingEvaluated = false;
        return CValue();
    }
}

MyBuilder & MyBuilder::setCell(string contents, int col , int row ) {
    this->contents = std::move(contents);
    this->offset.w = col;
    this->offset.h = row;
    return *this;
}

void MyBuilder::opAdd() {

    auto right = m_stack.top();
    m_stack.pop();
    auto left = m_stack.top();
    m_stack.pop();

    auto addOperation = [](CValue left, CValue right) -> CValue {
        auto leftDouble = std::get_if<double>(&left);
        auto rightDouble = std::get_if<double>(&right);
        auto leftStr = std::get_if<std::string>(&left);
        auto rightStr = std::get_if<std::string>(&right);

        if (leftDouble && rightDouble) {
            return *leftDouble + *rightDouble;
        } else if (leftStr && rightStr) {
            return *leftStr + *rightStr;
        } else if (leftStr && rightDouble) {
            return *leftStr + std::to_string(*rightDouble);
        } else if (leftDouble && rightStr) {
            return std::to_string(*leftDouble) + *rightStr;
        }

        return CValue();
    };

    m_stack.push(std::make_shared<BinNode>(left, right, addOperation));
}

void MyBuilder::opSub() {
    auto right = m_stack.top();
    m_stack.pop();
    auto left = m_stack.top();
    m_stack.pop();

    auto subOperation = [](CValue left, CValue right) -> CValue {
        auto leftDouble = std::get_if<double>(&left);
        auto rightDouble = std::get_if<double>(&right);

        if (leftDouble && rightDouble) {
            return *leftDouble - *rightDouble;
        }

        return CValue();
    };

    m_stack.push(std::make_shared<BinNode>(left, right, subOperation));
}

void MyBuilder::opMul() {

    auto right = m_stack.top();
    m_stack.pop();
    auto left = m_stack.top();
    m_stack.pop();

    auto mulOperation = [](CValue left, CValue right) -> CValue {
        auto leftDouble = std::get_if<double>(&left);
        auto rightDouble = std::get_if<double>(&right);

        if (leftDouble && rightDouble) {
            return *leftDouble * *rightDouble;
        }

        return CValue();
    };

    m_stack.push(std::make_shared<BinNode>(left, right, mulOperation));
}

void MyBuilder::opDiv() {

    auto right = m_stack.top();
    m_stack.pop();
    auto left = m_stack.top();
    m_stack.pop();

    auto divOperation = [](CValue left, CValue right) -> CValue {
        auto leftDouble = std::get_if<double>(&left);
        auto rightDouble = std::get_if<double>(&right);

        if (leftDouble && rightDouble && *rightDouble != 0) {
            return *leftDouble / *rightDouble;
        }

        return CValue();
    };

    m_stack.push(std::make_shared<BinNode>(left, right, divOperation));
}

void MyBuilder::opPow() {

    auto right = m_stack.top();
    m_stack.pop();
    auto left = m_stack.top();
    m_stack.pop();

    auto powOperation = [](CValue left, CValue right) -> CValue {
        auto leftDouble = std::get_if<double>(&left);
        auto rightDouble = std::get_if<double>(&right);
        if (leftDouble && rightDouble) {
            return std::pow(*leftDouble, *rightDouble);
        }
        return CValue();
    };

    m_stack.push(std::make_shared<BinNode>(left, right, powOperation));
}

void MyBuilder::opNeg() {

    auto operand = m_stack.top();
    m_stack.pop();
    auto negOperation = [](CValue a) -> CValue {
        auto value = std::get_if<double>(&a);
        return value ? -(*value) : CValue();
    };

    m_stack.push(std::make_shared<UnarNode>(operand, negOperation));
}

void MyBuilder::opEq() {

    auto right = m_stack.top();
    m_stack.pop();
    auto left = m_stack.top();
    m_stack.pop();

    auto eqOperation = [](CValue left, CValue right) -> CValue {
        auto leftDouble = std::get_if<double>(&left);
        auto rightDouble = std::get_if<double>(&right);
        if (leftDouble && rightDouble) {
            return *leftDouble == *rightDouble ? 1.0 : 0.0;
        }
        auto leftStr = std::get_if<string>(&left);
        auto rightStr = std::get_if<string>(&right);
        if (leftStr && rightStr) {
            return *leftStr == *rightStr ? 1.0 : 0.0;
        }
        return CValue();
    };

    m_stack.push(std::make_shared<BinNode>(left, right, eqOperation));
}

void MyBuilder::opNe() {

    auto right = m_stack.top();
    m_stack.pop();
    auto left = m_stack.top();
    m_stack.pop();

    auto neOperation = [](CValue left, CValue right) -> CValue {
        auto leftDouble = std::get_if<double>(&left);
        auto rightDouble = std::get_if<double>(&right);
        auto leftStr = std::get_if<string>(&left);
        auto rightStr = std::get_if<string>(&right);

        if (leftDouble && rightDouble) {
            return *leftDouble != *rightDouble ? 1.0 : 0.0;
        } else if (leftStr && rightStr) {
            return *leftStr != *rightStr ? 1.0 : 0.0;
        }
        return CValue();
    };

    m_stack.push(std::make_shared<BinNode>(left, right, neOperation));
}

void MyBuilder::opLt() {

    auto right = m_stack.top();
    m_stack.pop();
    auto left = m_stack.top();
    m_stack.pop();

    auto ltOperation = [](CValue left, CValue right) -> CValue {
        auto leftDouble = std::get_if<double>(&left);
        auto rightDouble = std::get_if<double>(&right);
        auto leftStr = std::get_if<string>(&left);
        auto rightStr = std::get_if<string>(&right);

        if (leftDouble && rightDouble) {
            return *leftDouble < *rightDouble ? 1.0 : 0.0;
        } else if (leftStr && rightStr) {
            return *leftStr < *rightStr ? 1.0 : 0.0;
        }
        return CValue();
    };

    m_stack.push(std::make_shared<BinNode>(left, right, ltOperation));
}

void MyBuilder::opLe() {
    auto right = m_stack.top();
    m_stack.pop();
    auto left = m_stack.top();
    m_stack.pop();
    auto leOperation = [](CValue left, CValue right) -> CValue {
        if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
            if (std::get<double>(left) <= std::get<double>(right)) {
                return 1.0;
            }
            return 0.0;
        }
        if (std::holds_alternative<string>(left) && std::holds_alternative<string>(right)) {
            if (std::get<string>(left) <= std::get<string>(right)) {
                return 1.0;
            }
            return 0.0;
        }
        return CValue();
    };

    m_stack.push(std::make_shared<BinNode>(left, right, leOperation));
}

void MyBuilder::opGt() {
    if (m_stack.size() < 2) return;

    auto right = m_stack.top();
    m_stack.pop();
    auto left = m_stack.top();
    m_stack.pop();

    auto leOperation = [](CValue left, CValue right) -> CValue {
        auto leftDouble = std::get_if<double>(&left);
        auto rightDouble = std::get_if<double>(&right);
        auto leftStr = std::get_if<string>(&left);
        auto rightStr = std::get_if<string>(&right);

        if (leftDouble && rightDouble) {
            return *leftDouble > *rightDouble ? 1.0 : 0.0;
        }
        if (leftStr && rightStr) {
            return *leftStr > *rightStr ? 1.0 : 0.0;
        }

        return CValue();
    };

    m_stack.push(std::make_shared<BinNode>(left, right, leOperation));
}

void MyBuilder::opGe() {
    if (m_stack.size() < 2) return;

    auto right = m_stack.top();
    m_stack.pop();
    auto left = m_stack.top();
    m_stack.pop();

    auto geOperation = [](CValue left, CValue right) -> CValue {
        auto leftDouble = std::get_if<double>(&left);
        auto rightDouble = std::get_if<double>(&right);
        auto leftStr = std::get_if<string>(&left);
        auto rightStr = std::get_if<string>(&right);

        if (leftDouble && rightDouble) {
            return *leftDouble >= *rightDouble ? 1.0 : 0.0;
        }
        if (leftStr && rightStr) {
            return *leftStr >= *rightStr ? 1.0 : 0.0;
        }
        return CValue();
    };

    m_stack.push(std::make_shared<BinNode>(left, right, geOperation));

}

void MyBuilder::valNumber(double num) {
    m_stack.push(std::make_shared<NumNode>(num));
}

void MyBuilder::valString(std::string val) {
    m_stack.push(std::make_shared<StrNode>(val));
}

void MyBuilder::valReference(std::string ref) {
    auto pos = CPos(ref);
    if (!pos.isRowABS) {
        pos.row += offset.h;
    }
    if (!pos.isColABS) {
        pos.col += offset.w;
    }
    auto reference = spreadsheet.getCell(pos);
    m_stack.push(std::make_shared<RefNode>(reference, spreadsheet));
}

void MyBuilder::valRange(std::string range) {

}

void MyBuilder::funcCall(std::string fnName, int paramCount) {

}

CValue MyBuilder::evaluateExpression() {
    if (m_stack.empty()) {
        return CValue();
    }

    auto rootNode = m_stack.top();
    return rootNode->evaluate();
}

MyBuilder::MyBuilder(CSpreadsheet &spreadsheet, string contents, int w, int h) : spreadsheet(
        spreadsheet), contents(std::move(contents)) {
    this->offset = {w, h};
}


MyBuilder::MyBuilder(MyBuilder &other) : spreadsheet(other.spreadsheet) {
    std::stack<std::shared_ptr<MainNode>> tempStack;
    std::vector<std::shared_ptr<MainNode>> nodes;

    while (!other.m_stack.empty()) {
        auto node = other.m_stack.top();
        other.m_stack.pop();
        nodes.push_back(node->clone());
    }

    for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
        other.m_stack.push(*it);
        this->m_stack.push((*it)->clone());
    }
}

MyBuilder &MyBuilder::operator=(MyBuilder &other) {
    if (this != &other) {
        this->spreadsheet = other.spreadsheet;

        while (!this->m_stack.empty()) {
            this->m_stack.pop();
        }

        std::stack<std::shared_ptr<MainNode>> tempStack;
        std::vector<std::shared_ptr<MainNode>> nodes;

        while (!other.m_stack.empty()) {
            auto node = other.m_stack.top();
            other.m_stack.pop();
            nodes.push_back(node->clone());
        }

        for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
            other.m_stack.push(*it);
            this->m_stack.push((*it)->clone());
        }
    }
    return *this;
}

Offset MyBuilder::GetOffset() const {
    return offset;
}



#ifndef __PROGTEST__
bool                                   valueMatch                              ( const CValue                        & r,
                                                                                 const CValue                        & s )

{
    if ( r . index () != s . index () )
        return false;
    if ( r . index () == 0 )
        return true;
    if ( r . index () == 2 )
        return std::get<std::string> ( r ) == std::get<std::string> ( s );
    if ( std::isnan ( std::get<double> ( r ) ) && std::isnan ( std::get<double> ( s ) ) )
        return true;
    if ( std::isinf ( std::get<double> ( r ) ) && std::isinf ( std::get<double> ( s ) ) )
        return ( std::get<double> ( r ) < 0 && std::get<double> ( s ) < 0 )
               || ( std::get<double> ( r ) > 0 && std::get<double> ( s ) > 0 );
    return fabs ( std::get<double> ( r ) - std::get<double> ( s ) ) <= 1e8 * DBL_EPSILON * fabs ( std::get<double> ( r ) );
}
int main ()
{
    CSpreadsheet x0, x1;
    std::ostringstream oss;
    std::istringstream iss;
    std::string data;
    assert ( x0 . setCell ( CPos ( "A1" ), "10" ) );
    assert ( x0 . setCell ( CPos ( "A2" ), "20.5" ) );
    assert ( x0 . setCell ( CPos ( "A3" ), "3e1" ) );
    assert ( x0 . setCell ( CPos ( "A4" ), "=40" ) );
    assert ( x0 . setCell ( CPos ( "A5" ), "=5e+1" ) );
    assert ( x0 . setCell ( CPos ( "A6" ), "raw text with any characters, including a quote \" or a newline\n" ) );
    assert ( x0 . setCell ( CPos ( "A7" ), "=\"quoted string, quotes must be doubled: \"\". Moreover, backslashes are needed for C++.\"" ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A1" ) ), CValue ( 10.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A2" ) ), CValue ( 20.5 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A3" ) ), CValue ( 30.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A4" ) ), CValue ( 40.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A5" ) ), CValue ( 50.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A6" ) ), CValue ( "raw text with any characters, including a quote \" or a newline\n" ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A7" ) ), CValue ( "quoted string, quotes must be doubled: \". Moreover, backslashes are needed for C++." ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A8" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "AAAA9999" ) ), CValue() ) );
    assert ( x0 . setCell ( CPos ( "B1" ), "=A1+A2*A3" ) );
    assert ( x0 . setCell ( CPos ( "B2" ), "= -A1 ^ 2 - A2 / 2   " ) );
    assert ( x0 . setCell ( CPos ( "B3" ), "= 2 ^ $A$1" ) );
    assert ( x0 . setCell ( CPos ( "B4" ), "=($A1+A$2)^2" ) );
    assert ( x0 . setCell ( CPos ( "B5" ), "=B1+B2+B3+B4" ) );
    assert ( x0 . setCell ( CPos ( "B6" ), "=B1+B2+B3+B4+B5" ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B1" ) ), CValue ( 625.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B2" ) ), CValue ( -110.25 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B3" ) ), CValue ( 1024.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B4" ) ), CValue ( 930.25 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B5" ) ), CValue ( 2469.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B6" ) ), CValue ( 4938.0 ) ) );
    assert ( x0 . setCell ( CPos ( "A1" ), "12" ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B1" ) ), CValue ( 627.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B2" ) ), CValue ( -154.25 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B4" ) ), CValue ( 1056.25 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B5" ) ), CValue ( 5625.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B6" ) ), CValue ( 11250.0 ) ) );
    x1 = x0;
    assert ( x0 . setCell ( CPos ( "A2" ), "100" ) );
    assert ( x1 . setCell ( CPos ( "A2" ), "=A3+A5+A4" ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B1" ) ), CValue ( 3012.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B2" ) ), CValue ( -194.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B4" ) ), CValue ( 12544.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B5" ) ), CValue ( 19458.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B6" ) ), CValue ( 38916.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B1" ) ), CValue ( 3612.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B2" ) ), CValue ( -204.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B4" ) ), CValue ( 17424.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B5" ) ), CValue ( 24928.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B6" ) ), CValue ( 49856.0 ) ) );
    oss . clear ();
    oss . str ( "" );
    assert ( x0 . save ( oss ) );
    data = oss . str ();
    iss . clear ();
    iss . str ( data );
    assert ( x1 . load ( iss ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B1" ) ), CValue ( 3012.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B2" ) ), CValue ( -194.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B4" ) ), CValue ( 12544.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B5" ) ), CValue ( 19458.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B6" ) ), CValue ( 38916.0 ) ) );
    assert ( x0 . setCell ( CPos ( "A3" ), "4e1" ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B1" ) ), CValue ( 3012.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B2" ) ), CValue ( -194.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B4" ) ), CValue ( 12544.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B5" ) ), CValue ( 19458.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B6" ) ), CValue ( 38916.0 ) ) );
    oss . clear ();
    oss . str ( "" );
    assert ( x0 . save ( oss ) );
    data = oss . str ();
    for ( size_t i = 0; i < std::min<size_t> ( data . length (), 10 ); i ++ )
        data[i] ^=0x5a;
    iss . clear ();
    iss . str ( data );
    assert ( ! x1 . load ( iss ) );
    assert ( x0 . setCell ( CPos ( "D0" ), "10" ) );
    assert ( x0 . setCell ( CPos ( "D1" ), "20" ) );
    assert ( x0 . setCell ( CPos ( "D2" ), "30" ) );
    assert ( x0 . setCell ( CPos ( "D3" ), "40" ) );
    assert ( x0 . setCell ( CPos ( "D4" ), "50" ) );
    assert ( x0 . setCell ( CPos ( "E0" ), "60" ) );
    assert ( x0 . setCell ( CPos ( "E1" ), "70" ) );
    assert ( x0 . setCell ( CPos ( "E2" ), "80" ) );
    assert ( x0 . setCell ( CPos ( "E3" ), "90" ) );
    assert ( x0 . setCell ( CPos ( "E4" ), "100" ) );
    assert ( x0 . setCell ( CPos ( "F10" ), "=D0+5" ) );
    assert ( x0 . setCell ( CPos ( "F11" ), "=$D0+5" ) );
    assert ( x0 . setCell ( CPos ( "F12" ), "=D$0+5" ) );
    assert ( x0 . setCell ( CPos ( "F13" ), "=$D$0+5" ) );
    x0 . copyRect ( CPos ( "G11" ), CPos ( "F10" ), 1, 4 );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F10" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F11" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F12" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F13" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F14" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G10" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G11" ) ), CValue ( 75.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G12" ) ), CValue ( 25.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G13" ) ), CValue ( 65.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G14" ) ), CValue ( 15.0 ) ) );
    x0 . copyRect ( CPos ( "G11" ), CPos ( "F10" ), 2, 4 );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F10" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F11" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F12" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F13" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F14" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G10" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G11" ) ), CValue ( 75.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G12" ) ), CValue ( 25.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G13" ) ), CValue ( 65.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G14" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H10" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H11" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H12" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H13" ) ), CValue ( 35.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H14" ) ), CValue() ) );
    assert ( x0 . setCell ( CPos ( "F0" ), "-27" ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H14" ) ), CValue ( -22.0 ) ) );
    x0 . copyRect ( CPos ( "H12" ), CPos ( "H13" ), 1, 2 );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H12" ) ), CValue ( 25.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H13" ) ), CValue ( -22.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H14" ) ), CValue ( -22.0 ) ) );
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */