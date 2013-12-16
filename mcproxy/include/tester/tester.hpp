/*
 * This file is part of mcproxy.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * written by Sebastian Woelke, in cooperation with:
 * INET group, Hamburg University of Applied Sciences,
 * Website: http://mcproxy.realmv6.org/
 */

#ifndef TESTER_HPP
#define TESTER_HPP

#include "include/tester/config_map.hpp"
#include "include/utils/addr_storage.hpp"
#include "include/proxy/def.hpp"
#include "include/utils/mc_socket.hpp"

#include <memory>
#include <list>

#define TESTER_DEFAULT_CONIG_PATH "tester.ini"

class tester 
{
private:
    config_map m_config_map;

    void run(const std::string& to_do);

    addr_storage get_gaddr(const std::string& to_do);
    std::unique_ptr<const mc_socket> get_mc_socket(int addr_family);
    std::string get_if_name(const std::string& to_do);
    std::list<addr_storage> get_src_list(const std::string& to_do, int addr_family);
    mc_filter get_mc_filter(const std::string& to_do);
    int get_count(const std::string& to_do);
    std::string get_action(const std::string& to_do);
    std::string source_list_to_string(const std::list<addr_storage>& slist);
    int get_ttl(const std::string& to_do);
    int get_port(const std::string& to_do);
    std::string get_msg(const std::string& to_do);

    void receive_data(const std::unique_ptr<const mc_socket>& ms, int port);
public:
    tester(int arg_count, char* args[]);
};


#endif // TESTER_HPP