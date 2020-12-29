/*!
 * @file       log.cpp
 * @brief      Defines the Fastcgipp debugging/logging facilities
 * @author     Eddie Carle &lt;eddie@isatec.ca&gt;, modified by Tobias Flaig &lt;dev@tobias-flaig.de&gt;
 * @date       May 18, 2016
 * @copyright  Copyright &copy; 2016 Eddie Carle. This project is released under
 *             the GNU Lesser General Public License Version 3.
 */

/*******************************************************************************
* Copyright (C) 2016 Eddie Carle [eddie@isatec.ca]                             *
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

#include "fastcgi++/log.hpp"

#include <iomanip>
#include <iostream>
#include <ctime>
#include <codecvt>
#include <cstring>
#include <array>
#include <sstream>

#include <unistd.h>
#include <limits.h>
#include <sys/types.h>

//! Topmost namespace for the fastcgi++ library
namespace Fastcgipp
{
    //! Contains the Fastcgipp debugging/logging mechanism
    namespace Logging
    {
        std::string getHostname()
        {
            char buffer[HOST_NAME_MAX+2];
            gethostname(buffer, sizeof(buffer));
            return(std::string(
                    buffer,
                    buffer+std::strlen(buffer)));

        }

        std::string getProgram()
        {
            std::ostringstream ss;
            ss << std::string(
                    program_invocation_short_name,
                    program_invocation_short_name
                    +std::strlen(program_invocation_short_name));

            ss << '[' << getpid() << ']';
            return ss.str();
        }

        std::array<std::string, 6> levels
        {{
            "[info] ",
            "[fail] ",
            "[error] ",
            "[warning] ",
            "[debug] ",
            "[diagnostic] "
        }};
    }
}

std::ostream* Fastcgipp::Logging::logstream(&std::cerr);
std::mutex Fastcgipp::Logging::mutex;
bool Fastcgipp::Logging::suppress(false);
std::string Fastcgipp::Logging::hostname(Fastcgipp::Logging::getHostname());
std::string Fastcgipp::Logging::program(Fastcgipp::Logging::getProgram());

void Fastcgipp::Logging::header(Level level)
{
    const std::time_t now = std::time(nullptr);
    *logstream
        << std::put_time(std::localtime(&now), "%b %d %H:%M:%S ")
        << hostname << ' ' << program << ": [fastcgi] " << levels[level];
}
