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


#include "include/hamcast_logging.h"
#include "include/utils/addr_storage.hpp"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <stdlib.h>

void addr_storage::clean(){
    HC_LOG_TRACE("");

    //memset(&m_addr,0, sizeof(m_addr));
    ((struct sockaddr_in6*)&m_addr)->sin6_family = AF_UNSPEC;
    ((struct sockaddr_in6*)&m_addr)->sin6_port = 0;
    ((struct sockaddr_in6*)&m_addr)->sin6_flowinfo = 0;
    ((struct sockaddr_in6*)&m_addr)->sin6_scope_id = 0;
}

int addr_storage::get_addr_len(int addr_family) const{
    HC_LOG_TRACE("");

    switch(addr_family){
    case AF_INET: return sizeof(struct sockaddr_in);
    case AF_INET6: return sizeof(struct sockaddr_in6);
    default:
        HC_LOG_ERROR("Unknown address family");
        return 0;
    }
}

addr_storage::addr_storage(){
    HC_LOG_TRACE("");

    clean();
}

addr_storage::addr_storage(int addr_family){
    HC_LOG_TRACE("");

    memset(&m_addr,0, sizeof(m_addr));
    m_addr.ss_family = addr_family;
}

addr_storage::addr_storage(const std::string& addr)
{
    HC_LOG_TRACE("");
    *this = addr;
}

addr_storage::addr_storage(const struct sockaddr_storage& addr){
    HC_LOG_TRACE("");

    *this = addr;
}

addr_storage::addr_storage(const addr_storage& addr){
    HC_LOG_TRACE("");

    *this = addr;
}

addr_storage::addr_storage(const struct in_addr& addr){
    HC_LOG_TRACE("");

    *this = addr;
}

addr_storage::addr_storage(const struct in6_addr& addr){
    HC_LOG_TRACE("");

    *this = addr;
}

addr_storage::addr_storage(const struct sockaddr& addr){
    HC_LOG_TRACE("");

    *this = addr;
}

addr_storage::addr_storage(const struct sockaddr_in6& addr){
    HC_LOG_TRACE("");

    *this = addr;
}

addr_storage::addr_storage(const struct sockaddr_in& addr){
    HC_LOG_TRACE("");

    *this = addr;
}

std::ostream& operator <<(std::ostream& s, const addr_storage a){
    HC_LOG_TRACE("");

    s << a.to_string();
    return s;
}

addr_storage& addr_storage::operator=(const addr_storage& s){
    HC_LOG_TRACE("");

    if(this != &s){
        this->m_addr = s.m_addr;
    }

    return *this;
}

addr_storage& addr_storage::operator=(const struct sockaddr_storage& s){
    HC_LOG_TRACE("");

    this->m_addr = s;
    return *this;
}

addr_storage& addr_storage::operator=(const std::string& s){
    HC_LOG_TRACE("");

    clean();

    if(s.find_first_of(':')==std::string::npos){ //==> IPv4
        m_addr.ss_family=AF_INET;
        if(inet_pton(m_addr.ss_family, s.c_str(), (void*)&(((struct sockaddr_in*)(&m_addr))->sin_addr))<1){
            HC_LOG_ERROR("failed to convert string to sockaddr_storage:" << s);
        }
    }else{ //==> IPv6
        m_addr.ss_family=AF_INET6;
        if(inet_pton(m_addr.ss_family, s.c_str(), (void*)&(((struct sockaddr_in6*)(&m_addr))->sin6_addr))<1){
            HC_LOG_ERROR("failed to convert string to sockaddr_storage:" << s);
        }
    }

    return *this;
}

addr_storage& addr_storage::operator=(const struct in_addr& s){
    HC_LOG_TRACE("");

    clean();

    m_addr.ss_family = AF_INET;
    ((sockaddr_in*)(&m_addr))->sin_addr = s;
    return *this;
}

addr_storage& addr_storage::operator=(const struct in6_addr& s){
    HC_LOG_TRACE("");

    clean();

    m_addr.ss_family = AF_INET6;
    ((sockaddr_in6*)(&m_addr))->sin6_addr = s;
    return *this;
}

addr_storage& addr_storage::operator=(const struct sockaddr& s){
    HC_LOG_TRACE("");

    clean();
    memcpy(&m_addr,&s,get_addr_len(s.sa_family));

    return *this;
}

addr_storage& addr_storage::operator=(const struct sockaddr_in& s){
    HC_LOG_TRACE("");

    clean();

    *this = *(const struct sockaddr*)&s;
    return *this;
}

addr_storage& addr_storage::operator=(const struct sockaddr_in6& s){
    HC_LOG_TRACE("");

    clean();

    *this = *(const struct sockaddr_in*)&s;
    return *this;
}

bool addr_storage::operator==(const addr_storage& addr) const{
    HC_LOG_TRACE("");

    std::string a, b;
    a = this->to_string();
    b = addr.to_string();

    if(a.empty()){
        return false;
    }else if(a.compare(b) == 0){
        return true;
    }else{
        return false;
    }
}

bool addr_storage::operator!=(addr_storage& addr) const{
    HC_LOG_TRACE("");

    return !(*this == addr);
}


bool operator< (const addr_storage& addr1, const addr_storage& addr2){
    HC_LOG_TRACE("");

    if(addr1.m_addr.ss_family == AF_INET && addr2.m_addr.ss_family == AF_INET){
        return  ntohl(((sockaddr_in*)(&addr1))->sin_addr.s_addr) < ntohl(((sockaddr_in*)(&addr2))->sin_addr.s_addr);
    }else if(addr1.m_addr.ss_family == AF_INET6 && addr2.m_addr.ss_family == AF_INET6){
        const uint8_t* a1 = ((const sockaddr_in6*)&addr1.m_addr)->sin6_addr.s6_addr;
        const uint8_t* a2 = ((const sockaddr_in6*)&addr2.m_addr)->sin6_addr.s6_addr;

        for(unsigned int i=0; i < sizeof(struct in6_addr)/sizeof(uint8_t); i++){
            if(a1[i]> a2[i]){
                return false;
            }else if(a1[i]<a2[i]){
                return true;
            }
        }
        return false;
    }else{
        HC_LOG_ERROR("incompatible ip versions");
        return false;
    }
}

int addr_storage::get_addr_family() const{
    HC_LOG_TRACE("");

    return this->m_addr.ss_family;
}

int addr_storage::get_port() const{
    HC_LOG_TRACE("");

    return ((sockaddr_in*)&m_addr)->sin_port;
}

addr_storage& addr_storage::set_port(int port){
    HC_LOG_TRACE("");

    ((sockaddr_in*)&m_addr)->sin_port = port;
    return *this;
}

addr_storage& addr_storage::set_port(const string& port){
    HC_LOG_TRACE("");

    set_port(atoi(port.c_str()));
    return *this;
}

int addr_storage::get_addr_len() const{
    HC_LOG_TRACE("");

    return get_addr_len(get_addr_family());
}

const struct sockaddr_storage& addr_storage::get_sockaddr_storage() const{
    HC_LOG_TRACE("");

    return m_addr;
}

const struct in_addr& addr_storage::get_in_addr() const{
    HC_LOG_TRACE("");

    return ((const struct sockaddr_in*)(&m_addr))->sin_addr;
}

const struct in6_addr& addr_storage::get_in6_addr() const{
    HC_LOG_TRACE("");

    return ((const struct sockaddr_in6*)(&m_addr))->sin6_addr;
}

const struct sockaddr& addr_storage::get_sockaddr() const{
    HC_LOG_TRACE("");

    return *((const struct sockaddr*)&m_addr);
}

const struct sockaddr_in& addr_storage::get_sockaddr_in() const{
    HC_LOG_TRACE("");

    return *((const struct sockaddr_in*)(&m_addr));
}

const struct sockaddr_in6& addr_storage::get_sockaddr_in6() const{
    HC_LOG_TRACE("");

    return *((const struct sockaddr_in6*)(&m_addr));
}


std::string addr_storage::to_string() const{
    HC_LOG_TRACE("");

    int af = m_addr.ss_family;
    if(af == AF_INET){
        char addressBuffer[INET_ADDRSTRLEN];

        if(inet_ntop(af,(const void*)&(((const struct sockaddr_in*)(&m_addr))->sin_addr),addressBuffer, sizeof(addressBuffer)) == nullptr){
            HC_LOG_ERROR("failed to convert sockaddr_storage");
            return std::string();
        }else{
            return std::string(addressBuffer);
        }
    }else if(af == AF_INET6){
        char addressBuffer[INET6_ADDRSTRLEN];
        if(inet_ntop(af,(const void*)&(((const struct sockaddr_in6*)(&m_addr))->sin6_addr),addressBuffer, sizeof(addressBuffer)) == nullptr){
            HC_LOG_ERROR("failed to convert sockaddr_storage");
            return std::string();
        }else{
            return std::string(addressBuffer);
        }

    }else{
        HC_LOG_ERROR("wrong address family");
        return std::string();
    }

}

addr_storage& addr_storage::mask(const addr_storage& s){
    HC_LOG_TRACE("");

    if(this->m_addr.ss_family == AF_INET && s.m_addr.ss_family == AF_INET){
        ((sockaddr_in*)(&m_addr))->sin_addr.s_addr &= ((sockaddr_in*)(&s))->sin_addr.s_addr;
        return *this;
    }else {
        HC_LOG_ERROR("incompatible ip versions");
    }

    return *this;
}

void addr_storage::test_addr_storage_old(){
    HC_LOG_TRACE("");

    using namespace std;
    std::string addr4 = "251.0.0.224";
    std::string addr6 = "ff02:231:abc::1";

    struct sockaddr_storage sockaddr4;
    struct sockaddr_storage sockaddr6;
    struct in_addr in_addr4;
    struct in6_addr in_addr6;
    struct in6_addr in_addr6tmp;

    addr_storage s4;
    addr_storage s6;
    addr_storage s4_tmp;
    addr_storage s6_tmp;
    addr_storage s4_1;
    addr_storage s6_1;



    cout << "-- string in addr_storage, cout stream, sockaddr_storage to string --" << endl;
    s4 = addr4;
    s6 = addr6;

    cout <<"addr4: str<" << addr4 << "> addr_storage<" << s4 << "> ==>" << (addr4.compare(s4.to_string())==0? "OK!" : "FAILED!") << endl;
    cout << "addr6: str<" << addr6 << "> addr_storage<" << s6 << "> ==>" << (addr6.compare(s6.to_string())==0? "OK!" : "FAILED!") << endl;

    cout << "-- sockaddr_storage to addr_storage --" << endl;
    sockaddr4 = s4.get_sockaddr_storage();
    sockaddr6 = s6.get_sockaddr_storage();
    s4_1 = sockaddr4;
    s6_1 = sockaddr6;
    cout << "addr4: str<" << addr4 << "> addr_storage<" << s4_1 << "> ==>" << (addr4.compare(s4_1.to_string())==0? "OK!" : "FAILED!") << endl;
    cout << "addr6: str<" << addr6 << "> addr_storage<" << s6_1 << "> ==>" << (addr6.compare(s6_1.to_string())==0? "OK!" : "FAILED!") << endl;

    cout << "-- equivalent addresses --" << endl;
    s4_tmp = "Hallo ich bin bob";
    s6_tmp = "ich: auch";

    cout << "s4_tmp: str<" << s4_tmp << "> == s6_tmp<" << s6_tmp << "> ==>" << ((s4_tmp != s6_tmp)? "OK!" : "FAILED!") << endl;
    cout << "s6_1: str<" << s6_1 << "> s6_1<" << s6_1 << "> ==>" << (s6_1 == s6_1? "OK!" : "FAILED!") << endl;


    cout << "-- struct in_addr and in6_addr --" << endl;
    in_addr4 = s4.get_in_addr();
    in_addr6 = s6.get_in6_addr();

    if(!inet_pton(AF_INET6, addr6.c_str(),(void*)&in_addr6tmp)>0){
        cout << "Error convert " << addr6 <<" to in6_addr FAILED! " << endl;
    }

    cout << "addr_storage to struct in_addr ==>" << (in_addr4.s_addr == inet_addr(addr4.c_str())? "OK!": "FAILED!") << endl;
    cout << "addr_storage to struct in6_addr ==>" << (IN6_ARE_ADDR_EQUAL(&in_addr6,&in_addr6tmp)? "OK!": "FAILED!") << endl;
    cout << "struct in_addr to addr_storage ==>" << ((addr_storage(in_addr4).to_string().compare(addr4)==0)? "OK!": "FAILED!") <<endl;
    cout << "struct in6_addr to addr_storage ==>" << ((addr_storage(in_addr6).to_string().compare(addr6)==0)? "OK!": "FAILED!") <<endl;

    cout << "-- ipv4 mask --" << endl;
    s6_tmp = "141.22.26.0";
    s4 = "141.22.26.249";
    s6 = "255.255.254.0";
    s4_tmp = s4;
    s4_tmp.mask(s6);
    cout << s4 << " mask with " << s6 << " ==> " << s4_tmp << " ==>"  << (s4_tmp == s6_tmp? "OK!" : "FAILED!") << endl;
    s4 = "141.22.27.155";
    s6 = "255.255.254.0";
    s4_tmp = s4;
    s4_tmp.mask(s6);
    cout << s4 << " mask with " << s6 << " ==> " << s4_tmp << " ==>"  << (s4_tmp == s6_tmp? "OK!" : "FAILED!")<< endl;
    s4 = "141.22.27.142";
    s6 = "255.255.254.0";
    s4_tmp = s4;
    s4_tmp.mask(s6);
    cout << s4 << " mask with " << s6 << " ==> " << s4_tmp << " ==>"  << (s4_tmp == s6_tmp? "OK!" : "FAILED!")<< endl;

    cout << "-- less then --" << endl;
    s4 = "141.22.26.249";
    s6 = "255.255.254.0";
    cout << s4 << " is less then " << s6  << ": " << (s4<s6? "true ==>OK!" : "false ==>FAILED!") << endl;
    cout << s6 << " is less then " << s4  << ": " << (s6<s4? "true ==>FAILED!" : "false ==>OK!") << endl;
    s4 = "fe80::5e26:aff:fe23:8dc0";
    s6 = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff";
    cout << s4 << " is less then " << s6  << ": " << (s4<s6? "true ==>OK!" : "false ==>FAILED!") << endl;
    cout << s6 << " is less then " << s4  << ": " << (s6<s4? "true ==>FAILED!" : "false ==>OK!") << endl;
    s4 = "0:0:0:0:ffff:ffff:ffff:ffff";
    s6 = "ffff:ffff:ffff:ffff::0";
    cout << s4 << " is less then " << s6  << ": " << (s4<s6? "true ==>OK!" : "false ==>FAILED!") << endl;
    cout << s6 << " is less then " << s4  << ": " << (s6<s4? "true ==>FAILED!" : "false ==>OK!") << endl;

}

void addr_storage::test_addr_storage(){
    HC_LOG_TRACE("");

    //string
    //sockaddr_storage
    //in_addr
    //in6_addr
    //sockaddr
    //sockaddr_in
    //sockaddr_in6

    const string s4("1.2.3.4");
    const string s6("1:2:3::4");
    const addr_storage a4(s4);
    const addr_storage a6(s6);

    cout << "const string s4(\"1.2.3.4\");" << endl;
    cout << "const string s6(\"1:2:3::4\");" << endl;
    cout << "const addr_storage a4(s4);" << endl;
    cout << "const addr_storage a6(s6);" << endl;
    cout << "-------------------------------" << endl;

    cout << "a4.to_string().compare(s4) ==> ";
    if(a4.to_string().compare(s4)==0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "a6.to_string().compare(s6) ==> ";
    if(a6.to_string().compare(s6)==0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "addr_storage(a4.get_sockaddr_storage()).to_string().compare(s4) ==> ";
    if(addr_storage(a4.get_sockaddr_storage()).to_string().compare(s4) == 0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "addr_storage(a6.get_sockaddr_storage()).to_string().compare(s6) ==> ";
    if(addr_storage(a6.get_sockaddr_storage()).to_string().compare(s6) == 0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "addr_storage(a4.get_sockaddr()).to_string().compare(s4) ==> ";
    if(addr_storage(a4.get_sockaddr()).to_string().compare(s4) == 0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "addr_storage(a6.get_sockaddr()).to_string().compare(s6) ==> ";
    if(addr_storage(a4.get_sockaddr()).to_string().compare(s4) == 0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "addr_storage(a4.get_sockaddr_in()).to_string().compare(s4) ==> ";
    if(addr_storage(a4.get_sockaddr_in()).to_string().compare(s4) == 0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "addr_storage(a6.get_sockaddr_in6()).to_string().compare(s6) ==> ";
    if(addr_storage(a6.get_sockaddr_in6()).to_string().compare(s6) == 0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "-------------------------------" << endl;
    cout << "addr_storage a4a(s4);" << endl;
    cout << "addr_storage a6a(s6);" << endl;
    cout << "const int port = 123;" << endl;

    addr_storage a4a(s4);
    addr_storage a6a(s6);
    const int port = 123;

    cout << "a4a.set_port(port).get_port()  == port ==> ";
    if(a4a.set_port(port).get_port() == port){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "a6a.set_port(port).get_port()  == port ==> ";
    if(a6a.set_port(port).get_port() == port){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "a4a.to_string().compare(s4) ==> ";
    if(a4a.to_string().compare(s4)==0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "a6.to_string().compare(s6) ==> ";
    if(a6a.to_string().compare(s6)==0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "-------------------------------" << endl;
    cout << "const int bad_port = 0xFFFFFF;" << endl;
    const int bad_port = 0xFFFFFF;


    cout << "a4a.set_port(bad_port).get_port()  != bad_port ==> ";
    if(a4a.set_port(bad_port).get_port() != bad_port){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "a6a.set_port(port).get_port()  != port ==> ";
    if(a6a.set_port(bad_port).get_port() != bad_port){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "a4a.to_string().compare(s4) ==> ";
    if(a4a.to_string().compare(s4)==0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "a6a.to_string().compare(s6) ==> ";
    if(a6a.to_string().compare(s6)==0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "-------------------------------" << endl;
    cout << "const string sport = \"123\";" << endl;
    a4a = s4;
    a6a = s6;

    const string sport = "123";

    cout << "a4a.set_port(port).get_port()  == port ==> ";
    if(a4a.set_port(port).get_port() == port){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "a6a.set_port(port).get_port()  == port ==> ";
    if(a6a.set_port(port).get_port() == port){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "a4a.to_string().compare(s4) ==> ";
    if(a4a.to_string().compare(s4)==0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

    cout << "a6.to_string().compare(s6) ==> ";
    if(a6a.to_string().compare(s6)==0){
        cout << "OK!" << endl;
    }else{
        cout << "FAILED!" << endl;
    }

}

