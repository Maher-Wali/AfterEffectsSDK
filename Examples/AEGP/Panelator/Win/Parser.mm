#include "Parser.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstring>

namespace Parser {

    // Helper function to convert AEGP_ColorVal to hex string (e.g., #FFFFFF)
    std::string ColorToHex(const AEGP_ColorVal& color) {
        int r = static_cast<int>(color.redF * 255.0f + 0.5f);
        int g = static_cast<int>(color.greenF * 255.0f + 0.5f);
        int b = static_cast<int>(color.blueF * 255.0f + 0.5f);

        r = std::max(0, std::min(255, r));
        g = std::max(0, std::min(255, g));
        b = std::max(0, std::min(255, b));

        std::stringstream ss;
        ss << "#" << std::hex << std::uppercase << std::setfill('0')
            << std::setw(2) << r
            << std::setw(2) << g
            << std::setw(2) << b;
        return ss.str();
    }
}