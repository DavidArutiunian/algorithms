/*
24. Имеются  расписания вылетов самолетов в  ряде  аэропор-
тов.  Требуется по  начальному  и  конечному  пунктам  методом
поиска  в ширину сформировать и выдать дерево возможных путей.
Проиллюстрировать этапы поиска (11).
Арутюнян Давид ПС-23
CLion 2018.3
gcc version 8.1.0
*/

#include <algorithm>
#include <iostream>
#include <string_view>

#include "GraphView.h"

std::string_view path(char* argv[])
{
    std::string_view path = argv[1];
    if (path.empty()) {
        throw std::runtime_error("No file provided!");
    }
    return path;
}

int main(int argc, char* argv[])
{
    GraphView graph;
    std::string from;
    std::string to;
    std::string type = "BFS";
    try {
        graph.parse(path(argv));
        graph.print();
        std::cout << std::endl;

        std::cout << "From: ";
        std::cin >> from;
        std::cout << std::endl;

        std::cout << "To: ";
        std::cin >> to;
        std::cout << std::endl;

        std::cout
            << "Going from "
            << from
            << " to "
            << to << "..."
            << std::endl;

        std::cout << std::endl
                  << "Found: " << std::endl;
        graph.search(from, to, type);
    } catch (std::exception& ex) {
        graph.error(ex);
    }
}
