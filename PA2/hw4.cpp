#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#endif /* __PROGTEST__ */

class CComponenet{
public:
//    CComponenet()=default;
//    CComponenet(long vel): velik(vel) {};

    virtual ~CComponenet() {}
    virtual void print(std::ostream& os, bool slash, bool last, bool flag) const = 0;

    virtual std::shared_ptr<CComponenet> copy() const=0;
protected:
//    long velik;
};



class CComputer{
    std::string computerName;
    std::vector<std::shared_ptr<CComponenet>> components;
    std::vector<std::string> address;

public:
    CComputer(std::string name): computerName(name){}

    const std::string& getName() const {
        return computerName;
    }

    virtual ~CComputer(){}
    CComputer(const CComputer& other): computerName(other.computerName), components(other.components), address(other.address) {}

    CComputer& operator=(const CComputer& a){
        if(this!=&a){
            computerName=a.computerName;
            components=a.components;
            address=a.address;
        }
        return *this;
    }


    template <typename T>
    CComputer& addComponent(const T& comp){
        components.push_back(comp.copy());
        return *this;
    }

    CComputer& addAddress(const std::string& addr){
        address.push_back(addr);
        return *this;
    }

    void print(std::ostream& os, bool flag = true, bool isLastComputer=false)const{
        if(flag){
            os <<"Host: "<< computerName << "\n";

            if (!address.empty()){
                for (size_t i = 0; i < address.size(); ++i) {
                    os << "+-"<<  address[i] << "\n";
                }
            }

            if (!components.empty()){
                for (size_t i = 0; i < components.size(); ++i) {

                    components[i]->print(os, isLastComputer ,(i == components.size() - 1), true);
                    os << "\n";
                }
            }
        }
        else{
            os << (isLastComputer ? "\\-Host: " : "+-Host: ") << computerName << "\n";

            if (!address.empty()){
                for (size_t i = 0; i < address.size(); ++i) {
                    os << (isLastComputer ? "  +-" : "| +-") <<  address[i] << "\n";
                }
            }

            if (!components.empty()){
                for (size_t i = 0; i < components.size(); ++i){

                    components[i]->print(os, isLastComputer ,(i == components.size() - 1), false);
                    os << "\n";
                }
            }
        }

    }

    friend std::ostream& operator<<(std::ostream& os, const CComputer& computer){
        computer.print(os, true, false);
        return os;
    }
};




class CNetwork{
    std::string networkName;
    std::vector<CComputer> computers;

public:
    CNetwork(const std::string& name): networkName(name){}

    virtual ~CNetwork() {}

    CNetwork(const CNetwork& other): networkName(other.networkName), computers(other.computers){}

    CNetwork& operator=(const CNetwork& a){
        if (this != &a) {
            networkName = a.networkName;
            computers = a.computers;
        }
        return *this;
    }

    CNetwork& addComputer(CComputer computer){
        computers.push_back(computer);
        return *this;
    }

    CComputer* findComputer(const std::string& computerName) {
        auto it = std::find_if(computers.begin(), computers.end(),[&computerName](const CComputer& c) { return c.getName() == computerName; });
        return (it != computers.end()) ? &(*it) : nullptr;
    }

    void print(std::ostream& os) const {
        os << "Network: " << networkName << "\n";
        for (size_t i = 0; i < computers.size(); ++i) {
            bool isLastComputer = (i == computers.size() - 1);
            computers[i].print(os,false, isLastComputer);
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const CNetwork& network){
        network.print(os);
        return os;
    }
};


class CCPU :public CComponenet{
    int cislo_jader;
    int frekvenci;
public:
    CCPU(int cislo_jader, int frekvenci):cislo_jader(cislo_jader), frekvenci(frekvenci){}

    virtual ~CCPU(){}

    CCPU(const CCPU& other): cislo_jader(other.cislo_jader), frekvenci(other.frekvenci){}

    CCPU& operator=(const CCPU& a){
        if(this!=&a){
            cislo_jader=a.cislo_jader;
            frekvenci=a.frekvenci;
        }
        return *this;
    }

    void print(std::ostream& os, bool slash, bool lastComp, bool flag) const override {
        if(flag==false){
            if(slash==false && lastComp==false){
                os<<"| +-";
            }
            else if(slash==false && lastComp==true){
                os<<"| \\-";
            }
            else if(slash==true && lastComp==false){
                os<<"  +-";
            }
            else if(slash==true && lastComp==true){
                os<<"  \\-";
            }

        }
        else{
            if(lastComp==false){
                os<<"+-";
            }
            else {
                os<<"\\-";
            }
        }
        os << "CPU, " << cislo_jader << " cores @ " << frekvenci << "MHz";
    }

    std::shared_ptr<CComponenet> copy() const override{
        return std::make_shared<CCPU>(*this) ;
    }
};


class CMemory:public CComponenet{
    int memory;
public:
    CMemory(int pamet): memory(pamet){}

    virtual ~CMemory(){}

    CMemory(const CMemory& other): memory(other.memory){}

    CMemory& operator=(const CMemory& a){
        if(this!=&a){
            memory=a.memory;
        }
        return *this;
    }
    void print(std::ostream& os, bool slash, bool lastComp, bool flag) const override {
        if(!flag){
            if(slash==false && lastComp==false){
                os<<"| +-";
            }
            else if(slash==false && lastComp==true){
                os<<"| \\-";
            }
            else if(slash==true && lastComp==false){
                os<<"  +-";
            }
            else if(slash==true && lastComp==true){
                os<<"  \\-";
            }
        }
        else{
            if(lastComp==false){
                os<<"+-";
            }
            else {
                os<<"\\-";
            }
        }
        os << "Memory, " << memory << " MiB";

    }
    std::shared_ptr<CComponenet> copy() const override{
        return std::make_shared<CMemory>(*this) ;
    }
};


class CDisk: public CComponenet{
public:
    enum Type{SSD, MAGNETIC};

    CDisk(Type typ, int velikost): typ(typ), velikost(velikost){}
    virtual ~CDisk(){};

    CDisk(const CDisk& other): typ(other.typ), velikost(other.velikost), partition(other.partition){}
    CDisk& operator=(const CDisk& a){
        if (this !=&a){
            typ=a.typ;
            velikost=a.velikost;
            partition=a.partition;
        }
        return *this;
    }

    CDisk& addPartition(int partitionSize, const std::string& id) {
        partition.push_back(make_pair(partitionSize, id));
        return *this;
    }
    void print(std::ostream& os, bool slash, bool lastComp, bool flag) const override {
        if(!flag){
            if(slash==false && lastComp==false){
                os<<"| +-";
            }
            else if(slash==false && lastComp==true){
                os<<"| \\-";
            }
            else if(slash==true && lastComp==false){
                os<<"  +-";
            }
            else if(slash==true && lastComp==true){
                os<<"  \\-";
            }
            os << (typ == SSD ? "SSD" : "HDD") << ", " << velikost << " GiB\n";
            for (size_t i = 0; i < partition.size(); ++i) {
                if(lastComp==false && slash==false){
                    os << "| | " << (i == partition.size() - 1 ? "\\-" : "+-")
                       << "[" << i << "]: " << partition[i].first << " GiB, " << partition[i].second;
                    if (i != partition.size() - 1) {
                        os << "\n";
                    }
                }
                else if(lastComp==true && slash==false){
                    os << "|   " << (i == partition.size() - 1 ? "\\-" : "+-")
                       << "[" << i << "]: " << partition[i].first << " GiB, " << partition[i].second;
                    if (i != partition.size() - 1) {
                        os << "\n";
                    }
                }
                else if(lastComp==false && slash==true){
                    os << "  | " << (i == partition.size() - 1 ? "\\-" : "+-")
                       << "[" << i << "]: " << partition[i].first << " GiB, " << partition[i].second;
                    if (i != partition.size() - 1) {
                        os << "\n";
                    }
                }
                else if(lastComp==true && slash==true){
                    os << "    " << (i == partition.size() - 1 ? "\\-" : "+-")
                       << "[" << i << "]: " << partition[i].first << " GiB, " << partition[i].second;
                    if (i != partition.size() - 1) {
                        os << "\n";
                    }
                }
            }
        }
        else{
            if(lastComp==false){
                os<<"+-";
            }
            else {
                os<<"\\-";
            }
            os << (typ == SSD ? "SSD" : "HDD") << ", " << velikost << " GiB\n";
            for (size_t i = 0; i < partition.size(); ++i) {
                if(!lastComp){
                    os << "| " << (i == partition.size() - 1 ? "\\-" : "+-")
                       << "[" << i << "]: " << partition[i].first << " GiB, " << partition[i].second;
                    if (i != partition.size() - 1) {
                        os << "\n";
                    }
                }
                else{
                    os << "  " << (i == partition.size() - 1 ? "\\-" : "+-")
                       << "[" << i << "]: " << partition[i].first << " GiB, " << partition[i].second;
                    if (i != partition.size() - 1) {
                        os << "\n";
                    }
                }
            }
        }


    }

    std::shared_ptr<CComponenet> copy() const override {
        return std::make_shared<CDisk>(*this) ;
    }

private:
    Type typ;
    int velikost;
    std::vector<std::pair<int, std::string>> partition;
};

#ifndef __PROGTEST__
template<typename T_>
std::string toString ( const T_ & x )
{
    std::ostringstream oss;
    oss << x;
    return oss . str ();
}

int main ()
{
    CNetwork n ( "FIT network" );
    n . addComputer (
            CComputer ( "progtest.fit.cvut.cz" ) .
                    addAddress ( "147.32.232.142" ) .
                    addComponent ( CCPU ( 8, 2400 ) ) .
                    addComponent ( CCPU ( 8, 1200 ) ) .
                    addComponent ( CDisk ( CDisk::MAGNETIC, 1500 ) .
                    addPartition ( 50, "/" ) .
                    addPartition ( 5, "/boot" ).
                    addPartition ( 1000, "/var" ) ) .
                    addComponent ( CDisk ( CDisk::SSD, 60 ) .
                    addPartition ( 60, "/data" )  ) .
                    addComponent ( CMemory ( 2000 ) ).
                    addComponent ( CMemory ( 2000 ) ) ) .
            addComputer (
            CComputer ( "courses.fit.cvut.cz" ) .
                    addAddress ( "147.32.232.213" ) .
                    addComponent ( CCPU ( 4, 1600 ) ) .
                    addComponent ( CMemory ( 4000 ) ).
                    addComponent ( CDisk ( CDisk::MAGNETIC, 2000 ) .
                    addPartition ( 100, "/" )   .
                    addPartition ( 1900, "/data" ) ) ) .
            addComputer (
            CComputer ( "imap.fit.cvut.cz" ) .
                    addAddress ( "147.32.232.238" ) .
                    addComponent ( CCPU ( 4, 2500 ) ) .
                    addAddress ( "2001:718:2:2901::238" ) .
                    addComponent ( CMemory ( 8000 ) ) );

    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: courses.fit.cvut.cz\n"
             "| +-147.32.232.213\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "\\-Host: imap.fit.cvut.cz\n"
             "  +-147.32.232.238\n"
             "  +-2001:718:2:2901::238\n"
             "  +-CPU, 4 cores @ 2500MHz\n"
             "  \\-Memory, 8000 MiB\n" );
    CNetwork x = n;
    auto c = x . findComputer ( "imap.fit.cvut.cz" );
    assert ( toString ( *c ) ==
             "Host: imap.fit.cvut.cz\n"
             "+-147.32.232.238\n"
             "+-2001:718:2:2901::238\n"
             "+-CPU, 4 cores @ 2500MHz\n"
             "\\-Memory, 8000 MiB\n" );
    c -> addComponent ( CDisk ( CDisk::MAGNETIC, 1000 ) .
            addPartition ( 100, "system" ) .
            addPartition ( 200, "WWW" ) .
            addPartition ( 700, "mail" ) );
    assert ( toString ( x ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: courses.fit.cvut.cz\n"
             "| +-147.32.232.213\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "\\-Host: imap.fit.cvut.cz\n"
             "  +-147.32.232.238\n"
             "  +-2001:718:2:2901::238\n"
             "  +-CPU, 4 cores @ 2500MHz\n"
             "  +-Memory, 8000 MiB\n"
             "  \\-HDD, 1000 GiB\n"
             "    +-[0]: 100 GiB, system\n"
             "    +-[1]: 200 GiB, WWW\n"
             "    \\-[2]: 700 GiB, mail\n" );
    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: courses.fit.cvut.cz\n"
             "| +-147.32.232.213\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "\\-Host: imap.fit.cvut.cz\n"
             "  +-147.32.232.238\n"
             "  +-2001:718:2:2901::238\n"
             "  +-CPU, 4 cores @ 2500MHz\n"
             "  \\-Memory, 8000 MiB\n" );



    
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
