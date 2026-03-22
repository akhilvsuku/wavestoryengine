#pragma once
#include "httplib/httplib.h"
#include "auth/SignInSignUp.h"
class receivermain
{
	// HTTP
	httplib::Server m_svr;

	// HTTPS
   // httplib::SSLServer svr;
protected:

	void process_version(const httplib::Request& req, httplib::Response& res);
	void singin_signup(const httplib::Request& req, httplib::Response& res);
	void fetch_search(const httplib::Request& req, httplib::Response& res);
	void fetch_search_update(const httplib::Request& req, httplib::Response& res);
	void manage_account(const httplib::Request& req, httplib::Response& res);
	void manage_payment(const httplib::Request& req, httplib::Response& res);
	void manage_booking(const httplib::Request& req, httplib::Response& res);
	void delete_account(const httplib::Request& req, httplib::Response& res);
	void save_doc(const httplib::Request& req, httplib::Response& res);
	void load_doc(const httplib::Request& req, httplib::Response& res);


	SignInSignUp* m_auth;
public:
	receivermain();
	void receiver_controller();
	virtual ~receivermain();
};

