/*!
 * @file       http.hpp
 * @brief      Declares elements of the HTTP protocol
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date       March 24, 2020
 * @copyright  Copyright &copy; 2020 Eddie Carle. This project is released under
 *             the GNU Lesser General Public License Version 3.
 */
/*******************************************************************************
* Copyright (C) 2020 Eddie Carle [eddie@isatec.ca]                             *
*                                                                              *
* This file is part of fastcgi++.                                              *
*                                                                              *
* fastcgi++ is free software: you can redistribute it and/or modify it under   *
* the terms of the GNU Lesser General Public License as  published by the Free *
* Software Foundation, either version 3 of the License, or (at your option)    *
* any later version.                                                           *
*                                                                              *
* fastcgi++ is distributed in the hope that it will be useful, but WITHOUT ANY *
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS    *
* FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for     *
* more details.                                                                *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with fastcgi++.  If not, see <http://www.gnu.org/licenses/>.           *
*******************************************************************************/

#ifndef FASTCGIPP_HTTP_HPP
#define FASTCGIPP_HTTP_HPP

#include <string>
#include <ostream>
#include <istream>
#include <iterator>
#include <map>
#include <vector>
#include <memory>
#include <ctime>

#include "fastcgi++/protocol.hpp"
#include "fastcgi++/address.hpp"

//! Topmost namespace for the fastcgi++ library
namespace Fastcgipp
{
    //! Defines classes and functions relating to the HTTP protocol
    namespace Http
    {
        //! Holds a file uploaded from the client
        /*!
         * The actual name associated with the file is omitted from the class
         * so it can be linked in an associative container.
         *
         * @tparam charT Type of character to use in the value string (char or
         *               wchar_t)
         *
         * @date    May 3, 2017
         * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        template<class charT> struct File
        {
            //! Filename
            std::basic_string<charT> filename;

            //! Content Type
            std::basic_string<charT> contentType;

            //! Size of file
            size_t size;

            //! File data
            mutable std::unique_ptr<char[]> data;

            //! Move constructor
            File(File&& x):
                filename(std::move(x.filename)),
                contentType(std::move(x.contentType)),
                size(x.size),
                data(std::move(x.data))
            {}

            File() {}
        };

        //! The HTTP request method as an enumeration
        enum class RequestMethod
        {
            ERROR=0,
            HEAD=1,
            GET=2,
            POST=3,
            PUT=4,
            DELETE=5,
            TRACE=6,
            OPTIONS=7,
            CONNECT=8
        };

        //! Some textual labels for RequestMethod
        extern const std::array<const char* const, 9> requestMethodLabels;

        template<class charT, class Traits>
        inline std::basic_ostream<charT, Traits>& operator<<(
                std::basic_ostream<charT, Traits>& os,
                const RequestMethod requestMethod)
        {
            return os << requestMethodLabels[static_cast<int>(requestMethod)];
        }

        //! Data structure of HTTP environment data
        /*!
         * This structure contains all HTTP environment data for each
         * individual request. The data is processed from FastCGI parameter
         * records.
         *
         * @tparam charT Character type to use for strings
         *
         * @date    March 24, 2020
         * @author  Eddie Carle &lt;eddie@isatec.ca&gt;
         */
        template<class charT> struct Environment
        {
            //! Hostname of the server
            std::basic_string<charT> host;

            //! Origin server
            std::basic_string<charT> origin;

            //! User agent string
            std::basic_string<charT> userAgent;

            //! Content types the client accepts
            std::basic_string<charT> acceptContentTypes;

            //! Languages the client accepts
            std::vector<std::string> acceptLanguages;

            //! Character sets the clients accepts
            std::basic_string<charT> acceptCharsets;
	  
            //! Http authorization string
            std::basic_string<charT> authorization;
	  
            //! Referral URL
            std::basic_string<charT> referer;

            //! Content type of data sent from client
            std::basic_string<charT> contentType;

            //! HTTP root directory
            std::basic_string<charT> root;

            //! Filename of script relative to the HTTP root directory
            std::basic_string<charT> scriptName;

            //! REQUEST_METHOD
            RequestMethod requestMethod;

            //! REQUEST_URI
            std::basic_string<charT> requestUri;

            //! Path information
            std::vector<std::basic_string<charT>> pathInfo;

            //! The etag the client assumes this document should have
            unsigned etag;

            //! How many seconds the connection should be kept alive
            unsigned keepAlive;

            //! Length of content to be received from the client (post data)
            unsigned contentLength;

            //! IP address of the server
            Address serverAddress;

            //! IP address of the client
            Address remoteAddress;

            //! TCP port used by the server
            uint16_t serverPort;

            //! TCP port used by the client
            uint16_t remotePort;

            //! Timestamp the client has for this document
            std::time_t ifModifiedSince;

            //! Container with all other enironment variables
            std::map<
                std::basic_string<charT>,
                std::basic_string<charT>> others;

            //! Container with all url-encoded cookie data
            std::multimap<
                std::basic_string<charT>,
                std::basic_string<charT>> cookies;

            //! Container with all url-encoded GET data
            std::multimap<
                std::basic_string<charT>,
                std::basic_string<charT>> gets;

            //! Container of non-file POST data
            std::multimap<
                std::basic_string<charT>,
                std::basic_string<charT>> posts;

            //! Container of file POST data
            std::multimap<
                std::basic_string<charT>,
                File<charT>> files;

            //! Parses FastCGI parameter data into the data structure
            /*!
             * This function will take the body of a FastCGI parameter record
             * and parse the data into the data structure. data should equal
             * the first character of the records body with size being it's
             * content length.
             *
             * @param[in] data Start of parameter data
             * @param[in] dataEnd 1+ the last byte of parameter data
             */
            void fill(
                    const char* data,
                    const char* dataEnd);

            //! Consolidates POST data into a single buffer
            /*!
             * This function will take arbitrarily divided chunks of raw http
             * post data and consolidate them into m_postBuffer.
             *
             * @param[in] start Start of post data.
             * @param[in] end 1+ the last byte of post data
             */
            void fillPostBuffer(
                    const char* start,
                    const char* end);

            //! Attempts to parse the POST buffer
            /*!
             * If the content type is recognized, this function will parse the
             * post buffer and return true. If it isn't recognized, it will
             * return false.
             *
             * @return True if successfully parsed. False otherwise.
             */
            bool parsePostBuffer();

            //! Get the post buffer
            const std::vector<char>& postBuffer() const
            {
                return m_postBuffer;
            }

            //! Clear the post buffer
            void clearPostBuffer()
            {
                m_postBuffer.clear();
                m_postBuffer.shrink_to_fit();
            }

            Environment():
                requestMethod(RequestMethod::ERROR),
                etag(0),
                keepAlive(0),
                contentLength(0),
                serverPort(0),
                remotePort(0),
                ifModifiedSince(0)
            {}
        private:
            //! Parses "multipart/form-data" http post data
            inline void parsePostsMultipart();

            //! Parses "application/x-www-form-urlencoded" post data
            inline void parsePostsUrlEncoded();

            //! Raw string of characters representing the post boundary
            std::vector<char> boundary;

            //! Buffer for processing post data
            std::vector<char> m_postBuffer;
        };

        //! Convert a char array to a std::wstring
        /*!
         * @param[in] start First byte in char array
         * @param[in] end 1+ last byte of the array (no null terminator)
         * @param[out] string Reference to the wstring that should be modified
         */
        void vecToString(
                const char* start,
                const char* end,
                std::wstring& string);

        //! Convert a char string to a std::string
        /*!
         * @param[in] start First byte in char string
         * @param[in] end 1+ last byte of the string (no null terminator)
         * @param[out] string Reference to the string that should be modified
         */
        inline void vecToString(
                const char* start,
                const char* end,
                std::string& string)
        {
            string.assign(start, end);
        }

        //! Convert a char string to an integer
        /*!
         * This function is very similar to std::atoi() except that it takes
         * start/end values of a non null terminated char string instead of a
         * null terminated string. The first character must be either a number
         * or a minus sign (-). As soon as the end is reached or a non
         * numerical character is reached, the result is tallied and returned.
         *
         * @param[in] start Pointer to the first byte in the string
         * @param[in] end Pointer to the last byte in the string + 1
         * @return Integer value represented by the string
         * @tparam charT Character type
         */
        template<class charT> int atoi(const charT* start, const charT* end);

        //! Convert a char string to a float
        /*!
         * This function is very similar to std::atof() except that it takes
         * start/end values of a non null terminated char string instead of a
         * null terminated string. The first character must be either a number
         * or a minus sign (-). As soon as the end is reached or a non
         * numerical character is reached, the result is tallied and returned.
         *
         * @param[in] start Pointer to the first byte in the string
         * @param[in] end Pointer to the last byte in the string + 1
         * @return Float value represented by the string
         * @tparam charT Character type
         */
        template<class charT> float atof(const charT* start, const charT* end);

        //! Decodes a url-encoded string into a multimap container
        /*!
         * @param[in] data Data to decode
         * @param[in] dataEnd +1 last byte to decode
         * @param[out] output Container to output data into
         * @param[in] fieldSeparator String that signifies field separation
         */
        template<class charT> void decodeUrlEncoded(
                const char* data,
                const char* dataEnd,
                std::multimap<
                    std::basic_string<charT>,
                    std::basic_string<charT>>& output,
                const char* const fieldSeparator="&");

        //! Convert a string with percent escaped byte values to their values
        /*!
         * Since converting a percent escaped string to actual values can only
         * make it shorter, it is safe to assume that the return value will
         * always be smaller than size. It is thereby a safe move to make the
         * destination block of memory the same size as the source.
         *
         * @param[in] start Iterator to the first character in the percent
         *                  escaped string
         * @param[in] end Iterator to +1 the last character in the percent
         *                escaped string
         * @param[out] destination Pointer to the section of memory to write the
         *                         converted string to
         * @return Iterator to +1 the last character written
         */
        char* percentEscapedToRealBytes(
                const char* start,
                const char* end,
                char* destination);

        //! List of characters in order for Base64 encoding.
        extern const std::array<const char, 64> base64Characters;

        //! Convert a binary container of data to a Base64 encoded container.
        /*!
         * If destination is a fixed size container, it should have a size of
         * at least ((end-start-1)/3 + 1)*4 not including null terminators if
         * used and assuming integer arithmetic.
         *
         * @param[in] start Iterator to start of binary data.
         * @param[in] end Iterator to end of binary data.
         * @param[out] destination Iterator to start of Base64 destination.
         *
         * @tparam In Input iterator type. Should be dereferenced to type char.
         * @tparam Out Output iterator type. Should be dereferenced to type
         *             char.
         *
         * @return Iterator to last position written+1 (The normal end()
         *                  iterator).
         */
        template<class In, class Out>
        Out base64Encode(In start, In end, Out destination);

        //! Convert a Base64 encoded container to a binary container.
        /*!
         * If destination is a fixed size container, it should have a size of
         * at least (end-start)*3/4 not including null terminators if used.
         *
         * @param[in] start Iterator to start of Base64 data.
         * @param[in] end Iterator to end of Base64 data.
         * @param[out] destination Iterator to start of binary destination.
         *
         * @tparam In Input iterator type. Should be dereferenced to type char.
         * @tparam Out Output iterator type. Should be dereferenced to type
         *             char.
         *
         * @return Iterator to last position written+1 (The normal end()
         *                  iterator). If the return value equals destination,
         *                  an error occurred.
         */
        template<class In, class Out>
        Out base64Decode(In start, In end, Out destination);

    }
}

template<class In, class Out>
Out Fastcgipp::Http::base64Decode(In start, In end, Out destination)
{
    Out dest=destination;

    for(int buffer=0, bitPos=-8, padStart=0; start!=end || bitPos>-6; ++dest)
    {
        if(bitPos==-8)
        {
            bitPos=18;
            padStart=-9;
            buffer=0;
            while(bitPos!=-6)
            {
                if(start==end) return destination;
                int value=*start++;
                if(value >= 'A' && 'Z' >= value) value -= 'A';
                else if(value >= 'a' && 'z' >= value) value -= 'a' - 26;
                else if(value >= '0' && '9' >= value) value -= '0' - 52;
                else if(value == '+') value = 62;
                else if(value == '/') value = 63;
                else if(value == '=') { padStart=bitPos; break; }
                else return destination;

                buffer |= value << bitPos;
                bitPos-=6;
            }
            bitPos=16;
        }

        *dest = (buffer >> bitPos) & 0xff;
        bitPos-=8;
        if(padStart>=bitPos)
        {
            if( (padStart-bitPos)/6 )
                return dest;
            else
                return ++dest;
        }
    }

    return dest;
}

template<class In, class Out>
Out Fastcgipp::Http::base64Encode(In start, In end, Out destination)
{
    for(int buffer, bitPos=-6, padded; start!=end || bitPos>-6; ++destination)
    {
        if(bitPos==-6)
        {
            bitPos=16;
            buffer=0;
            padded=-6;
            while(bitPos!=-8)
            {
                if(start!=end)
                    buffer |= static_cast<int>(
                            *reinterpret_cast<const unsigned char*>(
                                start++)) << bitPos;
                else padded+=6;
                bitPos-=8;
            }
            bitPos=18;
        }

        if(padded == bitPos)
        {
            *destination='=';
            padded-=6;
        }
        else *destination=base64Characters[ (buffer >> bitPos)&0x3f ];
        bitPos -= 6;
    }

    return destination;
}

#endif
