/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <WNS/testing/TestTool.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#include <sys/types.h>
#include <regex.h>

bool
wns::testing::compareFile(const std::string filename,
                          const std::string regex)
{
    // and the efficiency award goes tooooo... the following code!
    std::ifstream file(filename.c_str());
    std::stringstream fileContent;
    std::string line;
    while(std::getline(file, line))
        fileContent << line << std::endl;

    return compareString(fileContent.str(), regex);
}

bool
wns::testing::compareString(const std::string content,
                            const std::string regex,
                            bool matchEntireContent)
{
    regex_t preg;

    // we compile the regular expression and then try to match
    // the file content against it. we make sure that the whole file
    // was matched, by looking at the match position in pmatch.
    // since regular expressions match greedy, rm_so equals 0 and rm_eo
    // equals the file length iff there exists a match of the
    // expression agains the complete file content.
    // REG_NEWLINE is necessary to make the '.' not match a newline.
    // as consequence you have to make sure to match every line of the
    // file explicitly.
    // a possibly empty sequence of comment lines may be skipped using
    // "(#.*\n)*".
    // note, that regular expression special characters have to be
    // escaped twice. the first escape for c string literal escaping,
    // the second for the regular expression compiler: to match a '(',
    // you have to write '\\('.
    int errorCode = regcomp(&preg, regex.c_str(),
                            REG_EXTENDED | REG_NEWLINE);
    if(errorCode) {
        const int stringLength = 1024;
        char errorString[stringLength];

        regerror(errorCode, &preg, errorString, stringLength);
        std::cerr << errorString << std::endl;
        return false;
    }

    regmatch_t pmatch;
    int result;
    result = regexec(&preg, content.c_str(), 1, &pmatch, 0);
    regfree(&preg);

    if(REG_NOMATCH == result)
        return false;

    if (matchEntireContent == true)
    {
        if(pmatch.rm_so != 0)
            return false;

        if(pmatch.rm_eo != (int) /* 8) */ content.size())
            return false;
    }

    return true;
} // compareFile

bool
wns::testing::matchInFile(const std::string filename, const std::string regex)
{
    std::ifstream file(filename.c_str());
    std::stringstream fileContent;
    std::string line;
    while(std::getline(file, line))
        fileContent << line << std::endl;

    return matchInString(fileContent.str(), regex);
}

bool
wns::testing::matchInFile(const std::string filename, std::vector<std::string> regexps)
{
    std::ifstream file(filename.c_str());
    std::stringstream fileContent;
    std::string line;
    while(std::getline(file, line))
        fileContent << line << std::endl;

    return matchInString(fileContent.str(), regexps);
}

bool
wns::testing::matchInString(const std::string content, std::string regex)
{
    return compareString(content, regex, false);
}

bool
wns::testing::matchInString(const std::string content, std::vector<std::string> regexps)
{
    std::vector<std::string>::const_iterator iter = regexps.begin();
    std::vector<std::string>::const_iterator end  = regexps.end();
    bool allMatched = true;
    for ( ; iter != end; ++iter)
    {
        bool thisMatches = matchInString(content, (*iter));
        if (thisMatches == false)
        {
            std::cerr << "Failed to match regexp '" << (*iter) << "'" << std::endl;
        }
        allMatched = allMatched && thisMatches;
    }
    return allMatched;
}
