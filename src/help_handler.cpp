/* Author:      Connor Schultz
 * Created:     October 6, 2020
 * Last edit:   October 16, 2020
 */

#include <iostream>
#include "help_handler.h"

void printhelp(std::string progname) {

    std::cout << "\nUsage: ";
    std::cout << progname << " [OPTION]...\n";
    std::cout << "Creates children and simulates resource management and";
    std::cout << " deadlock avoidance algorithms.\n";

    std::cout << "\n  -v        \tSets logging to verbose mode.";

    std::cout << "\n  -h        \tPrints this message\n\n";
}
