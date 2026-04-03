#pragma once
#include "../httplib/httplib.h"
// arpa/nameser.h (pulled in transitively by httplib.h on Linux) may #define DELETE
// as ns_uop_delete, which conflicts with diff_match_patch's Operation enum.
// Undef here so the dmp enum declaration succeeds.
#ifdef DELETE
#  undef DELETE
#endif
#include "../dmp/diff_match_patch.h"
#include <string>
#include <vector>

/**
 * CompareHandler
 *
 * Handles POST /compare requests.
 * Accepts form-encoded text1 and text2, computes a character-level diff
 * using diff_match_patch::diff_main + diff_cleanupSemantic, and returns
 * a JSON array of { operation, text } objects.
 *
 * Usage (registered in receivermain::receiver_controller):
 *   m_svr.Post("/compare", [this](const httplib::Request& req, httplib::Response& res) {
 *       m_compare->handle(req, res);
 *   });
 */
class CompareHandler {
public:
    CompareHandler();

    /**
     * Handle an incoming POST /compare request.
     * Reads text1 and text2 from form params, runs the diff, writes JSON response.
     */
    void handle(const httplib::Request& req, httplib::Response& res);

private:
    diff_match_patch dmp_;

    /**
     * Serialize a vector of Diff objects to a JSON string.
     * Format: {"diffs":[{"operation":"INSERT","text":"..."},...]}
     */
    std::string serializeDiffs(const std::vector<Diff>& diffs) const;

    /**
     * Escape a UTF-8 string for safe embedding inside a JSON double-quoted value.
     * Handles: ", \, \n, \r, \t, and control characters U+0000–U+001F.
     */
    static std::string jsonEscape(const std::string& s);
};
