/*
 =========================    Cpp Console Table
 | Cpp | Console | Table |    version 1.2.0
 =========================    https://github.com/Oradle/CppConsoleTable

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2017 Oradle

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#ifndef _CPP_CONSOLE_TABLE_
#define _CPP_CONSOLE_TABLE_

#include <algorithm> // max
#include <iomanip> // setfill, setw, right, left
#include <iostream> // ostream, endl
#include <map> // map
#include <string> // string, to_string, const_iterator
#include <type_traits> // enable_if, is_arithmetic, is_same, remove_pointer
#include <vector> // vector

namespace samilton {
// force declaration
class ConsoleTable;
class ConsoleRow;

class ConsoleString {
public:
    ConsoleString(ConsoleRow* parent)
    {
        _parent = parent;
    }

    void clear()
    {
        if (_str != nullptr) {
            if (_lineCount > 1) {
                delete[] _str;
            } else {
                delete _str;
            }

            _str = nullptr;
            _lineCount = 1;
        }
    }

    ConsoleString& operator=(const std::string& val)
    {
        clear();
        _parseString(val);
        return *this;
    }

    ConsoleString& operator=(const char* val)
    {
        clear();
        _parseString(std::string(val));
        return *this;
    }

    template <class T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    ConsoleString& operator=(const T& val)
    {
        clear();
        _str = new std::string;

        *_str = std::to_string(val);
        return *this;
    }

    ConsoleString& operator=(const bool& val)
    {
        clear();
        _str = new std::string;

        if (val)
            *_str = "true";
        else
            *_str = "false";

        return *this;
    }

    ~ConsoleString()
    {
        clear();
    }

private:
    friend std::ostream& operator<<(std::ostream& stream, ConsoleTable& table);
    void _parseString(const std::string& val)
    {
        std::vector<std::string::const_iterator> lineEndingMarks;
        for (auto i = val.begin(); i < val.end(); ++i) {
            if (*i == '\n') {
                ++_lineCount;
                lineEndingMarks.push_back(i);
            }
        }

        if (_lineCount > 1) {
            _str = new std::string[_lineCount];

            _str[0].assign(val.begin(), lineEndingMarks[0]);
            for (size_t i = 1; i < lineEndingMarks.size(); ++i) {
                _str[i].assign(lineEndingMarks[i - 1] + 1, lineEndingMarks[i]);
            }
            _str[_lineCount - 1].assign(lineEndingMarks[_lineCount - 2] + 1, val.end());
        } else {
            _str = new std::string(val);
        }
    }

    ConsoleRow* _parent;
    std::string* _str = nullptr;
    size_t _lineCount = 1;
};

class ConsoleRow {
public:
    ConsoleRow(ConsoleTable* parent)
    {
        _parent = parent;
    }

    ~ConsoleRow()
    {
        for (auto& element : _rowData) {
            delete element.second;
        }
        _rowData.clear();
    }

    ConsoleString& operator[](const size_t column);

private:
    friend std::ostream& operator<<(std::ostream& stream, ConsoleTable& table);

    ConsoleTable* _parent;
    std::map<size_t, ConsoleString*> _rowData;
};

class ConsoleTable {
public:
    enum class Alignment {
        left = 0,
        centre,
        right
    };

    struct TableChars {
        char topRight = 187, topLeft = 201, downRight = 188, downLeft = 200;
        char topDownSimple = 205, topSeparation = 203, downSeparation = 202;
        char leftRightSimple = 186, leftSeparation = 204, rightSeparation = 185;
        char centreSeparation = 206;
    };

    ConsoleTable()
    {
        _alignment = Alignment::left;
        _leftIndent = _rightIndent = _rowSize = _columnSize = 0;
    }

    ConsoleTable(const Alignment& alignment)
    {
        _alignment = alignment;
        _leftIndent = _rightIndent = _rowSize = _columnSize = 0;
    }

    ConsoleTable(const size_t& leftIndent, const size_t& rightIndent, const Alignment& alignment = Alignment::left)
    {
        _leftIndent = leftIndent;
        _rightIndent = rightIndent;
        _alignment = alignment;
        _rowSize = _columnSize = 0;
    }

    ~ConsoleTable()
    {
        clear();
    }

    template <class T,
        class = typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<std::string, T>::value || std::is_same<char*, T>::value>::type>
    void addRow(const std::vector<T>& row)
    {
        const size_t tmp = _rowSize;
        for (size_t i = 0; i < row.size(); i++) {
            (*this)[tmp][i] = row[i];
        }
    }

    template <class T,
        class = typename std::enable_if<std::is_arithmetic<typename std::remove_pointer<T>::type>::value || std::is_same<std::string, typename std::remove_pointer<T>::type>::value>::type>
    void addRow(const T row, const size_t& size)
    {
        const size_t tmp = _rowSize;
        for (size_t i = 0; i < size; i++) {
            (*this)[tmp][i] = row[i];
        }
    }

    template <class T,
        class = typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<std::string, T>::value || std::is_same<char*, T>::value>::type>
    void addColumn(const std::vector<T>& column)
    {
        const size_t tmp = _columnSize;
        for (size_t i = 0; i < column.size(); i++) {
            (*this)[i][tmp] = column[i];
        }
    }

    template <class T,
        class = typename std::enable_if<std::is_arithmetic<typename std::remove_pointer<T>::type>::value || std::is_same<std::string*, T>::value>::type>
    void addColumn(const T column, const size_t& size)
    {
        const size_t tmp = _columnSize;
        for (size_t i = 0; i < size; i++) {
            (*this)[i][tmp] = column[i];
        }
    }

    template <class T,
        class = typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<std::string, T>::value || std::is_same<char*, T>::value>::type>
    void assign(const std::vector<std::vector<T>>& table)
    {
        clear();
        for (size_t i = 0; i < table.size(); i++)
            for (size_t j = 0; j < table[i].size(); j++)
                (*this)[i][j] = table[i][j];
    }

    template <class T,
        class = typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<std::string, typename std::remove_pointer<typename std::remove_pointer<T>::type>::type>::value>::type>
    void assign(T table, const size_t& rowSize, const size_t& columnSize)
    {
        clear();
        for (size_t i = 0; i < columnSize; i++)
            for (size_t j = 0; j < rowSize; j++)
                (*this)[i][j] = table[i][j];
    }

    void clear()
    {
        for (auto& element : _tableData) {
            delete element.second;
        }

        _tableData.clear();
        _columnSize = _rowSize = 0;
    }

    void setAlignment(const Alignment& alignment)
    {
        _alignment = alignment;
    }

    void setIndent(const size_t& leftIndent, const size_t& rightIndent)
    {
        _leftIndent = leftIndent;
        _rightIndent = rightIndent;
    }

    void setTableChars(const TableChars& chars)
    {
        _chars = chars;
    }

    template <class T,
        class = typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<std::string, T>::value || std::is_same<char*, T>::value>::type>
    ConsoleTable& operator=(const std::vector<std::vector<T>>& table)
    {
        assign(table);
        return *this;
    }

    ConsoleRow& operator[](const size_t row);

    friend std::ostream& operator>>(ConsoleTable& table, std::ostream& stream)
    {
        return stream << table;
    }

    friend std::ostream& operator<<(std::ostream& stream, ConsoleTable& table);

private:
    friend ConsoleString& ConsoleRow::operator[](const size_t column);

    static void _fillStreamByChar(std::ostream& stream, const char& fillChar, const size_t& lenght)
    {
        if (lenght > 0)
            stream << std::setfill(fillChar) << std::setw(lenght);
    }

    static void _fillStreamByChar(std::ostream& stream, const char& fillChar, const char& endChar, const size_t& lenght)
    {
        if (lenght > 0)
            stream << std::setfill(fillChar) << std::setw(lenght) << endChar;
    }

    TableChars _chars;
    std::map<size_t, ConsoleRow*> _tableData;
    Alignment _alignment;
    size_t _leftIndent, _rightIndent;
    size_t _rowSize, _columnSize;
};

inline ConsoleRow& ConsoleTable::operator[](const size_t row)
{
    try {
        return *_tableData.at(row);
    } catch (...) {
        _rowSize = std::max(_rowSize, row + 1);

        _tableData[row] = new ConsoleRow(this);
        return *_tableData[row];
    }
}

inline ConsoleString& ConsoleRow::operator[](const size_t column)
{
    try {
        return *_rowData.at(column);
    } catch (...) {
        _parent->_columnSize = std::max(_parent->_columnSize, column + 1);

        _rowData[column] = new ConsoleString(this);
        return *_rowData[column];
    }
}

inline std::ostream& operator<<(std::ostream& stream, ConsoleTable& table)
{
    // Return if table is empty
    if (table._tableData.empty())
        return stream;

    // Read width member and use it as indentation parameter if nonzero
    const auto tableIndentation = stream.width() > 0 ? stream.width() : 0;

    // Reset width to 0 for subsequent calls to this stream
    stream.width(0);

    // Calculation width of every column
    std::vector<size_t> columnWidth;
    for (size_t i = 0; i < table._columnSize; i++) {
        size_t tmp = 1;
        for (size_t j = 0; j < table._rowSize; j++) {
            if (table._tableData[j] != nullptr && table._tableData[j]->_rowData[i] != nullptr) {
                if (table._tableData[j]->_rowData[i]->_lineCount == 1) {
                    tmp = std::max(tmp, table._tableData[j]->_rowData[i]->_str->size());
                } else {
                    for (size_t k = 0; k < table._tableData[j]->_rowData[i]->_lineCount; ++k) {
                        tmp = std::max(tmp, table._tableData[j]->_rowData[i]->_str[k].size());
                    }
                }
            }
        }
        columnWidth.push_back(tmp);
    }

    // Calculation height of every row
    std::vector<size_t> rowHeight;
    for (size_t i = 0; i < table._rowSize; i++) {
        size_t tmp = 1;
        for (size_t j = 0; j < table._columnSize; j++) {
            if (table._tableData[i] != nullptr && table._tableData[i]->_rowData[j] != nullptr) {
                tmp = std::max(tmp, table._tableData[i]->_rowData[j]->_lineCount);
            }
        }
        rowHeight.push_back(tmp);
    }

    // Top border
    ConsoleTable::_fillStreamByChar(stream, ' ', ' ', static_cast<size_t>(tableIndentation));
    stream << std::right << table._chars.topLeft;
    ConsoleTable::_fillStreamByChar(stream, table._chars.topDownSimple, columnWidth[0] + 1 + table._leftIndent + table._rightIndent);

    if (table._columnSize != 1) {
        for (size_t i = 1; i < table._columnSize; i++) {
            stream << table._chars.topSeparation << std::setw(columnWidth[i] + 1 + table._leftIndent + table._rightIndent);
        }
    }
    stream << table._chars.topRight << std::endl;

    // Elements and middle borders
    for (size_t i = 0; i < table._rowSize; i++) {
        for (size_t k = 0; k < rowHeight[i]; ++k) {
            ConsoleTable::_fillStreamByChar(stream, ' ', ' ', static_cast<size_t>(tableIndentation));
            for (size_t j = 0; j < table._columnSize; j++) {
                if (table._tableData[i] != nullptr && table._tableData[i]->_rowData[j] != nullptr && k < table._tableData[i]->_rowData[j]->_lineCount) {
                    if (table._alignment == ConsoleTable::Alignment::centre) {
                        const size_t tmp = k < table._tableData[i]->_rowData[j]->_lineCount
                            ? columnWidth[j] - table._tableData[i]->_rowData[j]->_str[k].size()
                            : columnWidth[j] - table._tableData[i]->_rowData[j]->_str->size();

                        size_t leftAlignmentIndent, rightAlignmentIndent;
                        if (tmp % 2) {
                            leftAlignmentIndent = tmp / 2;
                            rightAlignmentIndent = tmp / 2 + 1;
                        } else {
                            leftAlignmentIndent = rightAlignmentIndent = tmp / 2;
                        }

                        stream << table._chars.leftRightSimple;
                        ConsoleTable::_fillStreamByChar(stream, ' ', ' ', table._leftIndent + leftAlignmentIndent);

                        if (k < table._tableData[i]->_rowData[j]->_lineCount)
                            stream << table._tableData[i]->_rowData[j]->_str[k];
                        else
                            stream << *table._tableData[i]->_rowData[j]->_str;

                        ConsoleTable::_fillStreamByChar(stream, ' ', ' ', table._rightIndent + rightAlignmentIndent);
                    } else {
                        if (table._alignment == ConsoleTable::Alignment::left)
                            stream << std::left;
                        else if (table._alignment == ConsoleTable::Alignment::right)
                            stream << std::right;

                        stream << table._chars.leftRightSimple;
                        ConsoleTable::_fillStreamByChar(stream, ' ', ' ', table._leftIndent);

                        ConsoleTable::_fillStreamByChar(stream, ' ', columnWidth[j]);

                        if (k < table._tableData[i]->_rowData[j]->_lineCount)
                            stream << table._tableData[i]->_rowData[j]->_str[k];
                        else
                            stream << *table._tableData[i]->_rowData[j]->_str;

                        ConsoleTable::_fillStreamByChar(stream, ' ', ' ', table._rightIndent);
                    }
                } else {
                    stream << table._chars.leftRightSimple;
                    ConsoleTable::_fillStreamByChar(stream, ' ', ' ', table._leftIndent);

                    ConsoleTable::_fillStreamByChar(stream, ' ', columnWidth[j]);
                    stream << ' ';

                    ConsoleTable::_fillStreamByChar(stream, ' ', ' ', table._rightIndent);
                }
            }
            stream << std::right << table._chars.leftRightSimple << std::endl;
        }

        // Down border
        ConsoleTable::_fillStreamByChar(stream, ' ', ' ', static_cast<size_t>(tableIndentation));
        if (i == table._rowSize - 1) {
            stream << table._chars.downLeft;
            ConsoleTable::_fillStreamByChar(stream, table._chars.topDownSimple, columnWidth[0] + 1 + table._leftIndent + table._rightIndent);

            if (table._columnSize != 1) {
                for (size_t j = 1; j < table._columnSize; j++) {
                    stream << table._chars.downSeparation << std::setw(columnWidth[j] + 1 + table._leftIndent + table._rightIndent);
                }
            }
            stream << table._chars.downRight << std::endl;
        } else {
            stream << table._chars.leftSeparation;
            ConsoleTable::_fillStreamByChar(stream, table._chars.topDownSimple, columnWidth[0] + 1 + table._leftIndent + table._rightIndent);

            if (table._columnSize != 1) {
                for (size_t j = 1; j < table._columnSize; j++) {
                    stream << table._chars.centreSeparation << std::setw(columnWidth[j] + 1 + table._leftIndent + table._rightIndent);
                }
            }
            stream << table._chars.rightSeparation << std::endl;
        }
    }

    return stream;
}
}

#endif // _CPP_CONSOLE_TABLE_
