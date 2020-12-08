//! See https://isatec.ca/fastcgipp/echo.html
//! [Request definition]
#include <fastcgi++/request.hpp>
#include <iomanip>

class Echo: public Fastcgipp::Request<wchar_t>
{
    //! [Request definition]
    //! [Max POST]
public:
    Echo():
        Fastcgipp::Request<wchar_t>(5*1024)
    {}

private:
    bool response()
    {
        //! [Max POST]
        //! [Encoding using]
        using Fastcgipp::Encoding;
        //! [Encoding using]

        //! [Header]
        out << L"Set-Cookie: echoCookie=" << Encoding::URL << L"<\"русский\">;"
            << Encoding::NONE << L"; path=/\n";
        out << L"Content-Type: text/html; charset=utf-8\r\n\r\n";
        //! [Header]

        //! [HTML]
        out <<
L"<!DOCTYPE html>\n"
L"<html>"
    L"<head>"
        L"<meta charset='utf-8' />"
        L"<title>fastcgi++: Echo</title>"
    L"</head>"
    L"<body>"
        L"<h1>Echo</h1>";
        //! [HTML]

        //! [Environment]
        out <<
        L"<h2>Environment Parameters</h2>"
        L"<p>"
            L"<b>FastCGI Version:</b> "
                << Fastcgipp::Protocol::version << L"<br />"
            L"<b>fastcgi++ Version:</b> " << Fastcgipp::version << L"<br />"
            L"<b>Hostname:</b> " << Encoding::HTML << environment().parameters.at(L"HTTP_HOST")
                << Encoding::NONE << L"<br />"
            L"<b>Origin Server:</b> " << Encoding::HTML << environment().parameters.at(L"HTTP_ORIGIN")
                << Encoding::NONE << L"<br />"
            L"<b>User Agent:</b> " << Encoding::HTML << environment().parameters.at(L"HTTP_USER_AGENT")
                << Encoding::NONE << L"<br />"
            L"<b>Accepted Content Types:</b> " << Encoding::HTML
                << environment().parameters.at(L"HTTP_ACCEPT") << Encoding::NONE
                << L"<br />"
            L"<b>Accepted Languages:</b> " << Encoding::HTML;
        if(!environment().acceptLanguages.empty())
        {
            auto language = environment().acceptLanguages.cbegin();
            while(true)
            {
                out << language->c_str();
                ++language;
                if(language == environment().acceptLanguages.cend())
                    break;
                out << ',';
            }
        }
        out << Encoding::NONE << L"<br />"
            L"<b>Accepted Characters Sets:</b> " << Encoding::HTML
                << environment().parameters.at(L"HTTP_ACCEPT_CHARSET") << Encoding::NONE << L"<br />"
            L"<b>Referer:</b> " << Encoding::HTML << environment().parameters.at(L"HTTP_REFERER")
                << Encoding::NONE << L"<br />"
            L"<b>Content Type:</b> " << Encoding::HTML
                << environment().contentType << Encoding::NONE << L"<br />"
            L"<b>Root:</b> " << Encoding::HTML << environment().parameters.at(L"DOCUMENT_ROOT")
                << Encoding::NONE << L"<br />"
            L"<b>Script Name:</b> " << Encoding::HTML
                << environment().parameters.at(L"SCRIPT_NAME") << Encoding::NONE << L"<br />"
            L"<b>Request URI:</b> " << Encoding::HTML
                << environment().parameters.at(L"REQUEST_URI") << Encoding::NONE << L"<br />"
            L"<b>Request Method:</b> " << environment().requestMethod
                << L"<br />"
            L"<b>Content Length:</b> " << environment().contentLength
                << L" bytes<br />"
            L"<b>Keep Alive Time:</b> " << environment().parameters.at(L"HTTP_KEEP_ALIVE")
                << L" seconds<br />"
            L"<b>Server Address:</b> " << environment().parameters.at(L"SERVER_ADDR")
                << L"<br />"
            L"<b>Server Port:</b> " << environment().parameters.at(L"SERVER_PORT") << L"<br />"
            L"<b>Client Address:</b> " << environment().parameters.at(L"REMOTE_ADDR") << L"<br />"
            L"<b>Client Port:</b> " << environment().parameters.at(L"REMOTE_PORT") << L"<br />"
            L"<b>Etag:</b> " << environment().parameters.at(L"HTTP_IF_MODIFIED_SINCE") << L"<br />"
        L"</p>";
        //! [Environment]

        //! [Other Environment]
        out <<
        L"<h2>All Environment Parameters</h2>";
        if(!environment().parameters.empty())
            for(const auto& other: environment().parameters)
                out << L"<b>" << Encoding::HTML << other.first << Encoding::NONE
                    << L":</b> " << Encoding::HTML << other.second
                    << Encoding::NONE << L"<br />";
        else
            out <<
        L"<p>No Other Environment Parameters</p>";
        //! [Other Environment]

        //! [GET Data]
        out <<
        L"<h2>GET Data</h2>";
        if(!environment().gets.empty())
            for(const auto& get: environment().gets)
                out << L"<b>" << Encoding::HTML << get.first << Encoding::NONE
                    << L":</b> " << Encoding::HTML << get.second
                    << Encoding::NONE << L"<br />";
        else
            out <<
        L"<p>No GET data</p>";
        //! [GET Data]

        //! [POST Data]
        out <<
        L"<h2>POST Data</h2>";
        if(!environment().posts.empty())
            for(const auto& post: environment().posts)
                out << L"<b>" << Encoding::HTML << post.first << Encoding::NONE
                    << L":</b> " << Encoding::HTML << post.second
                    << Encoding::NONE << L"<br />";
        else
            out <<
        L"<p>No POST data</p>";
        //! [POST Data]

        //! [Cookies]
        out <<
        L"<h2>Cookies</h2>";
        if(!environment().cookies.empty())
            for(const auto& cookie: environment().cookies)
                out << L"<b>" << Encoding::HTML << cookie.first
                    << Encoding::NONE << L":</b> " << Encoding::HTML
                    << cookie.second << Encoding::NONE << L"<br />";
        else
            out <<
        L"<p>No Cookies</p>";
        //! [Cookies]

        //! [Files]
        out <<
        L"<h2>Files</h2>";
        if(!environment().files.empty())
        {
            for(const auto& file: environment().files)
            {
                out <<
        L"<h3>" << Encoding::HTML << file.first << Encoding::NONE << L"</h3>"
        L"<p>"
            L"<b>Filename:</b> " << Encoding::HTML << file.second.filename
                << Encoding::NONE << L"<br />"
            L"<b>Content Type:</b> " << Encoding::HTML
                << file.second.contentType << Encoding::NONE << L"<br />"
            L"<b>Size:</b> " << file.second.size << L"<br />"
            L"<b>Data:</b>"
        L"</p>"
        L"<pre>";
                //! [Files]
                //! [Dump]
                dump(file.second.data.get(), file.second.size);
                out <<
        L"</pre>";
            }
        }
        else
            out <<
        L"<p>No files</p>";
        //! [Dump]

        //! [Form]
        out <<
        L"<h1>Form</h1>"
        L"<h3>multipart/form-data</h3>"
        L"<form action='?getVar=testing&amp;secondGetVar=tested&amp;"
            L"utf8GetVarTest=проверка&amp;enctype=multipart' method='post' "
            L"enctype='multipart/form-data' accept-charset='utf-8'>"
            L"Name: <input type='text' name='+= aquí está el campo' value='Él "
                L"está con un niño' /><br />"
            L"File: <input type='file' name='aFile' /> <br />"
            L"<input type='submit' name='submit' value='submit' />"
        L"</form>"
        L"<h3>application/x-www-form-urlencoded</h3>"
        L"<form action='?getVar=testing&amp;secondGetVar=tested&amp;"
            L"utf8GetVarTest=проверка&amp;enctype=url-encoded' method='post' "
            L"enctype='application/x-www-form-urlencoded' "
            L"accept-charset='utf-8'>"
            L"Name: <input type='text' name='+= aquí está el campo' value='Él "
                L"está con un niño' /><br />"
            L"File: <input type='file' name='aFile' /><br />"
            L"<input type='submit' name='submit' value='submit' />"
        L"</form>";
        //! [Form]

        //! [Finish]
        out <<
    L"</body>"
L"</html>";

        return true;
    }
};

#include <fastcgi++/manager.hpp>

int main()
{
    Fastcgipp::Manager<Echo> manager;
    manager.setupSignals();
    manager.listen();
    manager.start();
    manager.join();

    return 0;
}
//! [Finish]
