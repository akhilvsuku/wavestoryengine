#include "CompareHandler.h"
#include "../common/Logger.h"
#include <cstdio>

CompareHandler::CompareHandler() {}

void CompareHandler::handle(const httplib::Request& req, httplib::Response& res) {
    Logger::getInstance()->log(Logger::Level::INFO, "CompareHandler::handle");

    std::string text1, text2;
    if (req.has_param("text1")) text1 = req.get_param_value("text1");
    if (req.has_param("text2")) text2 = req.get_param_value("text2");

    if (text1.empty() || text2.empty()) {
        res.status = 400;
        res.set_content("{\"error\":\"text1 and text2 are required\"}", "application/json");
        Logger::getInstance()->log(Logger::Level::WARNING, "CompareHandler: missing text1 or text2");
        return;
    }

    try {
        auto diffs = dmp_.diff_main(text1, text2);
        dmp_.diff_cleanupSemantic(diffs);
        res.set_content(serializeDiffs(diffs), "application/json");
        Logger::getInstance()->log(Logger::Level::INFO, "CompareHandler: diff complete");
    } catch (const std::string& e) {
        Logger::getInstance()->log(Logger::Level::ERRR, "CompareHandler: diff_main threw: " + e);
        res.status = 500;
        res.set_content("{\"error\":\"diff computation failed\"}", "application/json");
    } catch (...) {
        Logger::getInstance()->log(Logger::Level::ERRR, "CompareHandler: unknown exception in diff_main");
        res.status = 500;
        res.set_content("{\"error\":\"internal server error\"}", "application/json");
    }
}

std::string CompareHandler::serializeDiffs(const std::vector<Diff>& diffs) const {
    std::string out;
    out.reserve(diffs.size() * 32);
    out += "{\"diffs\":[";

    for (size_t i = 0; i < diffs.size(); ++i) {
        if (i > 0) out += ',';

        const char* op = nullptr;
        switch (diffs[i].operation) {
            case INSERT: op = "INSERT"; break;
            case DELETE: op = "DELETE"; break;
            case EQUAL:  op = "EQUAL";  break;
        }

        out += "{\"operation\":\"";
        out += op;
        out += "\",\"text\":\"";
        out += jsonEscape(diffs[i].text);
        out += "\"}";
    }

    out += "]}";
    return out;
}

std::string CompareHandler::jsonEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size());

    for (unsigned char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (c < 0x20) {
                    // Control characters: encode as \uXXXX
                    char buf[8];
                    std::snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned>(c));
                    out += buf;
                } else {
                    out += static_cast<char>(c);
                }
        }
    }

    return out;
}
