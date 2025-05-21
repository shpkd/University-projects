#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
#endif /* __PROGTEST__ */


std::string toLower(const std::string& owner_for){
    std::string result = owner_for;
    std::transform(result.begin(), result.end(), result.begin(),[&](unsigned char c) {return std::tolower(c);});
    return result;
}


struct Land{
    std::string city;
    std::string addr;
    std::string region;
    unsigned int id;
    std::string owner;
    std::string ownerLower;
    Land(std::string city, std::string addr, std::string region, unsigned int id, std::string owner)
            : city(city), addr(addr), region(region), id(id), owner(owner){
        ownerLower = toLower(owner);
    }
};

class CIterator
{
public:

    CIterator(const std::vector<Land> lands, size_t c_pos = 0)
    {
        this->lands=lands;
        this->position=c_pos;
    };

    bool atEnd () const{
        if(lands.empty() || lands.size() == 1){
            return true;
        }
        return position > (lands.size()-1);
    };

    void next (){
        ++position;
    };

    std::string city () const{
        return lands[position].city;
    };

    std::string addr () const{
        return lands[position].addr;
    };

    std::string region () const{
        return lands[position].region;
    };

    unsigned int id () const{
        return lands[position].id;
    };

    std::string owner () const{
        return lands[position].owner;
    };

private:
    std::vector<Land> lands;
    size_t position;
};

class CLandRegister
{
public:
    bool add (const std::string & city, const std::string & addr, const std::string & region, unsigned int id ){
        Land l={city, addr, region, id,""}, l_plug = {"", "", "", 0,""};
        auto  it_city=std::lower_bound(land_city.begin(),land_city.end(),l,CmpByCity);
        auto  it_region=std::lower_bound(land_region.begin(),land_region.end(),l,CmpByRegion);

        if((it_city!=land_city.end() && it_city->city==city && it_city->addr==addr)||(it_region!=land_region.end() && it_region->region==region && it_region->id==id)){
            return false;
        }
        else{
            land_city.insert(it_city,l);
            land_region.insert(it_region,l);

            auto it_owner = std::lower_bound(land_owner.begin(),land_owner.end(),l, CmpByOwner);
            if(it_owner != land_owner.end()) {
                it_owner->push_back(l);
            } else {
                land_owner.insert(it_owner,std::vector<Land>{l_plug, l});
            }

            return true;
        }
    };

    bool del (const std::string & city, const std::string & addr ){
        Land l={city, addr, "",0, ""};
        auto it_city=std::lower_bound(land_city.begin(),land_city.end(),l,CmpByCity);

        if(it_city!=land_city.end() && it_city->city==city && it_city->addr==addr){
            auto it_region=std::lower_bound(land_region.begin(),land_region.end(),*it_city, CmpByRegion);

            auto it_owner=std::lower_bound(land_owner.begin(),land_owner.end(),*it_city,CmpByOwner);
            auto& lands = *it_owner;
            auto it_city_in=std::find_if(lands.begin(),lands.end(),[&](const Land& l_in) {return l_in.city==city && l_in.addr==addr;});
            lands.erase(it_city_in);

            land_region.erase(it_region);
            land_city.erase(it_city);
            return true;
        }
        return false;
    };

    bool del ( const std::string & region, unsigned int id ){
        Land l={"", "", region, id, ""};
        auto it_region=std::lower_bound(land_region.begin(),land_region.end(),l, CmpByRegion);

        if(it_region!=land_region.end() && it_region->region==region && it_region->id==id){
            auto it_city=std::lower_bound(land_city.begin(),land_city.end(),*it_region, CmpByCity);

            auto it_owner=std::lower_bound(land_owner.begin(),land_owner.end(),*it_region,CmpByOwner);
            auto& lands = *it_owner;
            auto it_region_in=std::find_if(lands.begin(),lands.end(),[&](const Land& l_in){return l_in.region==region && l_in.id==id;});
            lands.erase(it_region_in);

            land_city.erase(it_city);
            land_region.erase(it_region);
            return true;
        }
        return false;
    };

    bool getOwner ( const std::string & city, const std::string & addr, std::string & owner ) const{
        Land l={city, addr, "",0,""};

        auto it_city=std::lower_bound(land_city.begin(),land_city.end(),l,CmpByCity);
        if(it_city!=land_city.end() && it_city->city==city && it_city->addr==addr){
            owner=it_city->owner;
            return true;
        }

        return false;
    };

    bool getOwner ( const std::string & region, unsigned int id, std::string & owner ) const{
        Land l={"", "", region,id,""};

        auto it_region=std::lower_bound(land_region.begin(),land_region.end(),l,CmpByRegion);
        if(it_region!=land_region.end() && it_region->region==region && it_region->id==id){
            owner=it_region->owner;
            return true;
        }

        return false;
    };

    bool newOwner ( const std::string  & city, const std::string & addr, const std::string & owner ){
        Land l = {city, addr, "", 0, ""};

        auto it_city = std::lower_bound(land_city.begin(), land_city.end(), l, CmpByCity);
        if (it_city != land_city.end() && it_city->city == city && it_city->addr == addr && it_city->owner != owner) {
            Land l_new = {it_city->city, it_city->addr, it_city->region, it_city->id, owner},
                    l_plug = {"", "", "", 0,owner};

            auto it_region = std::lower_bound(land_region.begin(), land_region.end(), l_new, CmpByRegion);
            it_region->owner = l_new.owner;
            it_region->ownerLower=l_new.ownerLower;

            auto it_owner = std::lower_bound(land_owner.begin(), land_owner.end(), *it_city, CmpByOwner);
            auto& lands = *it_owner;
            auto it_city_in = std::find_if(lands.begin(), lands.end(), [&](const Land& l_in) { return l_in.city == city && l_in.addr == addr; });
            lands.erase(it_city_in);

            auto it_owner_new = std::lower_bound(land_owner.begin(), land_owner.end(), l_new, CmpByOwner);
            if (it_owner_new != land_owner.end() && it_owner_new->front().ownerLower == l_new.ownerLower) {
                it_owner_new->push_back(l_new);
            } else {
                land_owner.insert(it_owner_new, std::vector<Land>{l_plug, l_new});
            }

            it_city->owner = l_new.owner;
            it_city->ownerLower=l_new.ownerLower;
            return true;
        }

        return false;
    };

    bool newOwner ( const std::string & region, unsigned int id, const std::string & owner ){
        Land l = {"", "", region, id, ""};

        auto it_region = std::lower_bound(land_region.begin(), land_region.end(), l, CmpByRegion);
        if (it_region != land_region.end() && it_region->region == region && it_region->id == id && it_region->owner != owner) {
            Land l_new = {it_region->city, it_region->addr, it_region->region, it_region->id, owner},
                    l_plug = {"", "", "", 0,owner};

            auto it_city = std::lower_bound(land_city.begin(), land_city.end(), l_new, CmpByCity);
            it_city->owner = l_new.owner;
            it_city->ownerLower=l_new.ownerLower;

            auto it_owner = std::lower_bound(land_owner.begin(), land_owner.end(), *it_region, CmpByOwner);
            auto& lands = *it_owner;
            auto it_region_in = std::find_if(lands.begin(), lands.end(), [&](const Land& l_in) { return l_in.region == region && l_in.id == id; });
            lands.erase(it_region_in);

            auto it_owner_new = std::lower_bound(land_owner.begin(), land_owner.end(), l_new, CmpByOwner);
            if (it_owner_new != land_owner.end() && it_owner_new->front().ownerLower == l_new.ownerLower) {
                it_owner_new->push_back(l_new);
            } else {
                land_owner.insert(it_owner_new, std::vector<Land>{l_plug, l_new});
            }

            it_region->owner = l_new.owner;
            it_region->ownerLower=l_new.ownerLower;
            return true;
        }

        return false;
    };

    size_t count ( const std::string & owner ) const{
        Land l={"","","",0,owner};

        auto it_owner=std::lower_bound(land_owner.begin(),land_owner.end(),l,CmpByOwner);
        if(it_owner!=land_owner.end() && it_owner->front().ownerLower == l.ownerLower && it_owner->size() > 1){
            return it_owner->size() - 1;
        }
        else{
            return 0;
        }
    };

    CIterator listByAddr () const{
        return CIterator(land_city, 0);
    };

    CIterator listByOwner  ( const std::string & owner ) const{
        Land l={"","","",0,owner};

        auto it_owner = std::lower_bound(land_owner.begin(), land_owner.end(), l, CmpByOwner);
        if (it_owner != land_owner.end() && it_owner->front().ownerLower == l.ownerLower && it_owner->size() > 1){
            return CIterator(*it_owner, 1);
        } else {
            std::vector<Land> empty;
            return CIterator(empty, 0);
        }
    };

private:
    static bool CmpByCity(const Land& a,const Land& b){
        return a.city!=b.city ? a.city<b.city : a.addr<b.addr;
    }
    static bool CmpByRegion(const Land& a, const Land& b){
        return a.region!=b.region ? a.region<b.region : a.id<b.id;
    }
    static bool CmpByOwner(const std::vector<Land>& g_owner, const Land& land){
//        if(g_owner.empty()){
//            return true;
//        }
        return g_owner.front().ownerLower<land.ownerLower;
    }
    std::vector <Land> land_city;
    std::vector <Land> land_region;
    std::vector <std::vector<Land>> land_owner;
};


#ifndef __PROGTEST__
static void test0 ()
{
    CLandRegister x;
    std::string owner;

    assert ( x . add ( "Prague", "Thakurova", "Dejvice", 12345 ) );
    assert ( x . add ( "Prague", "Evropska", "Vokovice", 12345 ) );
    assert ( x . add ( "Prague", "Technicka", "Dejvice", 9873 ) );
    assert ( x . add ( "Plzen", "Evropska", "Plzen mesto", 78901 ) );
    assert ( x . add ( "Liberec", "Evropska", "Librec", 4552 ) );
    CIterator i0 = x . listByAddr ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Liberec"
             && i0 . addr () == "Evropska"
             && i0 . region () == "Librec"
             && i0 . id () == 4552
             && i0 . owner () == "" );
    i0 . next ();

    assert ( ! i0 . atEnd ()
             && i0 . city () == "Plzen"
             && i0 . addr () == "Evropska"
             && i0 . region () == "Plzen mesto"
             && i0 . id () == 78901
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Evropska"
             && i0 . region () == "Vokovice"
             && i0 . id () == 12345
             && i0 . owner () == "" );

    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Technicka"
             && i0 . region () == "Dejvice"
             && i0 . id () == 9873
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Thakurova"
             && i0 . region () == "Dejvice"
             && i0 . id () == 12345
             && i0 . owner () == "" );
    i0 . next ();
    assert ( i0 . atEnd () );

    assert ( x . count ( "" ) == 5 );
    CIterator i1 = x . listByOwner ( "" );
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Prague"
             && i1 . addr () == "Thakurova"
             && i1 . region () == "Dejvice"
             && i1 . id () == 12345
             && i1 . owner () == "" );
    i1 . next ();
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Prague"
             && i1 . addr () == "Evropska"
             && i1 . region () == "Vokovice"
             && i1 . id () == 12345
             && i1 . owner () == "" );
    i1 . next ();
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Prague"
             && i1 . addr () == "Technicka"
             && i1 . region () == "Dejvice"
             && i1 . id () == 9873
             && i1 . owner () == "" );
    i1 . next ();
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Plzen"
             && i1 . addr () == "Evropska"
             && i1 . region () == "Plzen mesto"
             && i1 . id () == 78901
             && i1 . owner () == "" );
    i1 . next ();
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Liberec"
             && i1 . addr () == "Evropska"
             && i1 . region () == "Librec"
             && i1 . id () == 4552
             && i1 . owner () == "" );
    i1 . next ();
    assert ( i1 . atEnd () );

    assert ( x . count ( "CVUT" ) == 0 );
    CIterator i2 = x . listByOwner ( "CVUT" );
    assert ( i2 . atEnd () );

    assert ( x . newOwner ( "Prague", "Thakurova", "CVUT" ) );
    assert ( x . newOwner ( "Dejvice", 9873, "CVUT" ) );
    assert ( x . newOwner ( "Plzen", "Evropska", "Anton Hrabis" ) );
    assert ( x . newOwner ( "Librec", 4552, "Cvut" ) );
    assert ( x . getOwner ( "Prague", "Thakurova", owner ) && owner == "CVUT" );
    assert ( x . getOwner ( "Dejvice", 12345, owner ) && owner == "CVUT" );
    assert ( x . getOwner ( "Prague", "Evropska", owner ) && owner == "" );
    assert ( x . getOwner ( "Vokovice", 12345, owner ) && owner == "" );
    assert ( x . getOwner ( "Prague", "Technicka", owner ) && owner == "CVUT" );
    assert ( x . getOwner ( "Dejvice", 9873, owner ) && owner == "CVUT" );
    assert ( x . getOwner ( "Plzen", "Evropska", owner ) && owner == "Anton Hrabis" );
    assert ( x . getOwner ( "Plzen mesto", 78901, owner ) && owner == "Anton Hrabis" );
    assert ( x . getOwner ( "Liberec", "Evropska", owner ) && owner == "Cvut" );
    assert ( x . getOwner ( "Librec", 4552, owner ) && owner == "Cvut" );
    CIterator i3 = x . listByAddr ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Liberec"
             && i3 . addr () == "Evropska"
             && i3 . region () == "Librec"
             && i3 . id () == 4552
             && i3 . owner () == "Cvut" );
    i3 . next ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Plzen"
             && i3 . addr () == "Evropska"
             && i3 . region () == "Plzen mesto"
             && i3 . id () == 78901
             && i3 . owner () == "Anton Hrabis" );
    i3 . next ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Prague"
             && i3 . addr () == "Evropska"
             && i3 . region () == "Vokovice"
             && i3 . id () == 12345
             && i3 . owner () == "" );
    i3 . next ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Prague"
             && i3 . addr () == "Technicka"
             && i3 . region () == "Dejvice"
             && i3 . id () == 9873
             && i3 . owner () == "CVUT" );
    i3 . next ();
    assert ( ! i3 . atEnd ()
             && i3 . city () == "Prague"
             && i3 . addr () == "Thakurova"
             && i3 . region () == "Dejvice"
             && i3 . id () == 12345
             && i3 . owner () == "CVUT" );
    i3 . next ();
    assert ( i3 . atEnd () );

    assert ( x . count ( "cvut" ) == 3 );
    CIterator i4 = x . listByOwner ( "cVuT" );
    assert ( ! i4 . atEnd ()
             && i4 . city () == "Prague"
             && i4 . addr () == "Thakurova"
             && i4 . region () == "Dejvice"
             && i4 . id () == 12345
             && i4 . owner () == "CVUT" );
    i4 . next ();
    assert ( ! i4 . atEnd ()
             && i4 . city () == "Prague"
             && i4 . addr () == "Technicka"
             && i4 . region () == "Dejvice"
             && i4 . id () == 9873
             && i4 . owner () == "CVUT" );
    i4 . next ();
    assert ( ! i4 . atEnd ()
             && i4 . city () == "Liberec"
             && i4 . addr () == "Evropska"
             && i4 . region () == "Librec"
             && i4 . id () == 4552
             && i4 . owner () == "Cvut" );
    i4 . next ();
    assert ( i4 . atEnd () );

    assert ( x . newOwner ( "Plzen mesto", 78901, "CVut" ) );
    assert ( x . count ( "CVUT" ) == 4 );
    CIterator i5 = x . listByOwner ( "CVUT" );
    assert ( ! i5 . atEnd ()
             && i5 . city () == "Prague"
             && i5 . addr () == "Thakurova"
             && i5 . region () == "Dejvice"
             && i5 . id () == 12345
             && i5 . owner () == "CVUT" );
    i5 . next ();
    assert ( ! i5 . atEnd ()
             && i5 . city () == "Prague"
             && i5 . addr () == "Technicka"
             && i5 . region () == "Dejvice"
             && i5 . id () == 9873
             && i5 . owner () == "CVUT" );
    i5 . next ();
    assert ( ! i5 . atEnd ()
             && i5 . city () == "Liberec"
             && i5 . addr () == "Evropska"
             && i5 . region () == "Librec"
             && i5 . id () == 4552
             && i5 . owner () == "Cvut" );
    i5 . next ();
    assert ( ! i5 . atEnd ()
             && i5 . city () == "Plzen"
             && i5 . addr () == "Evropska"
             && i5 . region () == "Plzen mesto"
             && i5 . id () == 78901
             && i5 . owner () == "CVut" );
    i5 . next ();
    assert ( i5 . atEnd () );

    assert ( x . del ( "Liberec", "Evropska" ) );
    assert ( x . del ( "Plzen mesto", 78901 ) );
    assert ( x . count ( "cvut" ) == 2 );
    CIterator i6 = x . listByOwner ( "cVuT" );
    assert ( ! i6 . atEnd ()
             && i6 . city () == "Prague"
             && i6 . addr () == "Thakurova"
             && i6 . region () == "Dejvice"
             && i6 . id () == 12345
             && i6 . owner () == "CVUT" );
    i6 . next ();
    assert ( ! i6 . atEnd ()
             && i6 . city () == "Prague"
             && i6 . addr () == "Technicka"
             && i6 . region () == "Dejvice"
             && i6 . id () == 9873
             && i6 . owner () == "CVUT" );
    i6 . next ();
    assert ( i6 . atEnd () );

    assert ( x . add ( "Liberec", "Evropska", "Librec", 4552 ) );
}

static void test1 ()
{
    CLandRegister x;
    std::string owner;

    assert ( x . add ( "Prague", "Thakurova", "Dejvice", 12345 ) );
    assert ( x . add ( "Prague", "Evropska", "Vokovice", 12345 ) );
    assert ( x . add ( "Prague", "Technicka", "Dejvice", 9873 ) );
    assert ( ! x . add ( "Prague", "Technicka", "Hradcany", 7344 ) );
    assert ( ! x . add ( "Brno", "Bozetechova", "Dejvice", 9873 ) );
    assert ( !x . getOwner ( "Prague", "THAKUROVA", owner ) );
    assert ( !x . getOwner ( "Hradcany", 7343, owner ) );
    CIterator i0 = x . listByAddr ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Evropska"
             && i0 . region () == "Vokovice"
             && i0 . id () == 12345
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Technicka"
             && i0 . region () == "Dejvice"
             && i0 . id () == 9873
             && i0 . owner () == "" );
    i0 . next ();
    assert ( ! i0 . atEnd ()
             && i0 . city () == "Prague"
             && i0 . addr () == "Thakurova"
             && i0 . region () == "Dejvice"
             && i0 . id () == 12345
             && i0 . owner () == "" );
    i0 . next ();
    assert ( i0 . atEnd () );

    assert ( x . newOwner ( "Prague", "Thakurova", "CVUT" ) );
    assert ( ! x . newOwner ( "Prague", "technicka", "CVUT" ) );
    assert ( ! x . newOwner ( "prague", "Technicka", "CVUT" ) );
    assert ( ! x . newOwner ( "dejvice", 9873, "CVUT" ) );
    assert ( ! x . newOwner ( "Dejvice", 9973, "CVUT" ) );
    assert ( ! x . newOwner ( "Dejvice", 12345, "CVUT" ) );
    assert ( x . count ( "CVUT" ) == 1 );
    CIterator i1 = x . listByOwner ( "CVUT" );
    assert ( ! i1 . atEnd ()
             && i1 . city () == "Prague"
             && i1 . addr () == "Thakurova"
             && i1 . region () == "Dejvice"
             && i1 . id () == 12345
             && i1 . owner () == "CVUT" );
    i1 . next ();
    assert ( i1 . atEnd () );

    assert ( ! x . del ( "Brno", "Technicka" ) );
    assert ( ! x . del ( "Karlin", 9873 ) );
    assert ( x . del ( "Prague", "Technicka" ) );
    assert ( ! x . del ( "Prague", "Technicka" ) );
    assert ( ! x . del ( "Dejvice", 9873 ) );
}



int main ( void )
{
    test0 ();
    test1 ();


    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
