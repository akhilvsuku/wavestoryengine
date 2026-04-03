#include "receivermain.h"
#include <filesystem>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include "Logger.h" 
// SignInSignUp class for authentication
#include "auth/SignInSignUp.h"
#include "auth/MySqlUserStore.h"

namespace fs = std::filesystem;

receivermain::receivermain()
{
    // initialize authentication handler with MySQL store
    auto store = std::make_unique<MySqlUserStore>("localhost", "dbuser", "dbpass", "dbname");
    m_auth = new SignInSignUp(std::move(store));
    m_compare = new CompareHandler();
}

void receivermain::receiver_controller()
{
    Logger::getInstance()->log(Logger::Level::INFO,"receivermain::receiver_controller");

    // CORS: allow requests from the frontend dev server and any local origin.
    // Handles the browser preflight OPTIONS request for POST /compare.
    m_svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin",  "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
    });

    m_svr.set_default_headers({
        {"Access-Control-Allow-Origin",  "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });

    m_svr.Get("/version", [this](const httplib::Request& req, httplib::Response& res) {
        process_version( req, res);
        });


    m_svr.Get("/fetch_search", [this](const httplib::Request& req, httplib::Response& res) {
        fetch_search(req, res);
        });
		
		
    m_svr.Get("/save_doc", [this](const httplib::Request& req, httplib::Response& res) {
        save_doc(req, res);
        });

    m_svr.Post("/singin_signup",
        [&](const httplib::Request& req, httplib::Response& res/*, const httplib::ContentReader& content_reader*/) {
            singin_signup(req, res);
        });

    m_svr.Post("/compare", [this](const httplib::Request& req, httplib::Response& res) {
        compare_doc(req, res);
    });

    m_svr.listen("0.0.0.0", 8080);
}

void receivermain::process_version(const httplib::Request& req, httplib::Response& res)
{
    Logger::getInstance()->log(Logger::Level::INFO,"receivermain::process_version");
    res.set_content("v1.0.02", "text/plain");
}

void receivermain::singin_signup(const httplib::Request& req, httplib::Response& res)
{
    Logger::getInstance()->log(Logger::Level::INFO,"receivermain::singin_signup");
    std::string str_uname = "";
    std::string str_pwrd = "";
    if (req.has_param("username")) {
        str_uname = req.get_param_value("username");
    }

    if (req.has_param("password")) {
        str_pwrd = req.get_param_value("password");
    }

    std::string str_resp = str_uname + str_pwrd;
    res.set_content(str_resp, "text/plain");
}

void receivermain::fetch_search(const httplib::Request& req, httplib::Response& res)
{
    Logger::getInstance()->log(Logger::Level::INFO,"receivermain::fetch_search");
    std::string str_uname = "";
    std::string str_pwrd = "";
    if (req.has_param("username")) {
        str_uname = req.get_param_value("username");
    }

    if (req.has_param("password")) {
        str_pwrd = req.get_param_value("password");
    }

    std::string str_resp = str_uname + str_pwrd;
        res.set_content(str_resp, "text/plain");
}

void receivermain::fetch_search_update(const httplib::Request& req, httplib::Response& res)
{
    Logger::getInstance()->log(Logger::Level::INFO,"receivermain::fetch_search_update");
}

void receivermain::manage_account(const httplib::Request& req, httplib::Response& res)
{
    Logger::getInstance()->log(Logger::Level::INFO,"receivermain::manage_account");
}

void receivermain::manage_payment(const httplib::Request& req, httplib::Response& res)
{
    Logger::getInstance()->log(Logger::Level::INFO,"receivermain::manage_payment");
}

void receivermain::manage_booking(const httplib::Request& req, httplib::Response& res)
{
    Logger::getInstance()->log(Logger::Level::INFO,"receivermain::manage_booking");
}

void receivermain::delete_account(const httplib::Request& req, httplib::Response& res)
{
    Logger::getInstance()->log(Logger::Level::INFO,"receivermain::delete_account");
}


void receivermain::save_doc(const httplib::Request& req, httplib::Response& res)
{ 
    Logger::getInstance()->log(Logger::Level::INFO,"receivermain::save_doc");
    const auto& files = req.files;

    auto it = files.find("file");
    if (it != files.end()) {
        const auto& file = it->second;

        std::ofstream out("/app/" + file.filename, std::ios::binary);
        if (out.is_open()) {
            out << file.content;
            out.close();

            res.set_content("File uploaded successfully: " + file.filename, "text/plain");
            Logger::getInstance()->log(Logger::Level::INFO,"File uploaded successfully.");
        } else {
            res.status = 500;
            res.set_content("Failed to save file.", "text/plain");
            Logger::getInstance()->log(Logger::Level::ERRR,"Failed to save file.");
        }
    } else {
        res.status = 400;
        res.set_content("No file found in the request.", "text/plain");
        Logger::getInstance()->log(Logger::Level::WARNING,"No file found in the request.");
    }
	
    /*     
        fs::path currentPath = fs::current_path();  // Gets the working directory
        fs::path canonicalPath = fs::canonical(currentPath);  // Resolves symlinks etc.
    */
}


void receivermain::load_doc(const httplib::Request& req, httplib::Response& res)
{
    Logger::getInstance()->log(Logger::Level::INFO,"receivermain::load_doc");
    std::string str_path = "";
	
    if (req.has_param("filename")) {
        str_path = req.get_param_value("filename");
    }
    std::string file_path = "/app/" + str_path;  // Your actual file path

    if (!std::filesystem::exists(file_path)) {
        res.status = 404;
        res.set_content("File not found", "text/plain");
        Logger::getInstance()->log(Logger::Level::WARNING,"File not found.");
        return;
    }

    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        res.status = 500;
        res.set_content("Failed to open file", "text/plain");
        Logger::getInstance()->log(Logger::Level::ERRR,"Failed to open file.");
        return;
    }

    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    res.set_content(file_content, "application/octet-stream");
    res.set_header("Content-Disposition", "attachment; filename=\"" + str_path + "\"");
}


void receivermain::compare_doc(const httplib::Request& req, httplib::Response& res)
{
    m_compare->handle(req, res);
}

receivermain::~receivermain()
{
    delete m_compare;
    delete m_auth;
}
