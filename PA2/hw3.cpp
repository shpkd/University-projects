#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <compare>


class CDate
{
  public:
    CDate ( int y, int m, int d )
      : m_Y ( y ), m_M ( m ), m_D ( d )
    {
    }

    std::strong_ordering operator <=> ( const CDate & other ) const = default;

    friend std::ostream& operator << ( std::ostream& os, const CDate & d )
    {
      return os << d . m_Y << '-' << d . m_M << '-' << d . m_D;
    }

  private:
    int m_Y;
    int m_M;
    int m_D;
};
enum class ESortKey
{
  NAME,
  BIRTH_DATE,
  ENROLL_YEAR
};
#endif /* __PROGTEST__ */
//---------------------функции для нормализации имени-------------------------------------
std::set<std::string> normalize_name(const std::string& name){
    std::istringstream iss(name);
    std::string part;
    std::set<std::string> normalized_words;

    //перевод в нижний регистр
    while (iss >> part) {
        std::transform(part.begin(), part.end(), part.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        normalized_words.insert(part);
    }

    return normalized_words;
}
//функция для нормализации имени -> перевод в нижний регистр -> вставка в сет

std::string normalize_name_string(const std::set<std::string>& name) {
    //объединение слов в одну строку
    std::string normalized_name;
    for(const auto& na : name){
        normalized_name += na;
    }

    return normalized_name;
}
//функция для нормализации имени -> перевод в нижний регистр -> сортировка по алфавиту -> вставка в стринг
//----------------------------------------------------------------------------------------------------

class CStudent
{
public:
    CStudent ( const std::string & name, const CDate& born, int enrolled)
            : just_name(name), birth(born), year(enrolled), csIndex(0){
        name_filtr=normalize_name(name);
        name_filtr_st= normalize_name_string(name_filtr);
    };
    //-------------------------операторы----------------------------
    bool operator == ( const CStudent  & other ) const{
        if ((birth <=> other.birth) == std::strong_ordering::equal) {
            return year == other.year && just_name == other.just_name;
        } else {
            return false;
        }
    };

    bool operator != ( const CStudent  & other ) const{
        return !(*this == other);
    };
    //---------------------------getter------------------------------
    void setIndex(int index){
        csIndex= index;
    }

    int getIndex() const {
        return csIndex;
    }

    const std::set<std::string>& getName() const {
        return name_filtr;
    }

    std::string getNameSt()const{
        return name_filtr_st;
    }

    std::string getJustName()const{
        return just_name;
    }

    CDate getBirthDate() const {
        return birth;
    }

    int getEnrollYear() const {
        return year;
    }
    //----------------------------------------------------------------
private:
    std::string just_name;
    CDate birth;
    int year;
    std::set<std::string> name_filtr;
    std::string name_filtr_st;
    int csIndex;//индекс добавления студентов

};


class CFilter
{
public:
    CFilter()
            : name_f(),
              born_before(0,0,0),
              born_after(0,0,0),
              year_before(-1),
              year_after(-1),
              name_flag(false),
              born_bef_flag(false),
              born_aft_flag(false),
              year_bef_flag(false),
              year_aft_flag(false) {}

    CFilter& name(const std::string & name) {
        name_f.insert( normalize_name_string(normalize_name(name)));
        name_flag = true;
        return *this;
    }

    CFilter& bornBefore(const CDate& date) {
        born_before = date;
        born_bef_flag = true;
        return *this;
    }

    CFilter& bornAfter(const CDate& date) {
        born_after = date;
        born_aft_flag = true;
        return *this;
    }

    CFilter& enrolledBefore(int year) {
        year_before = year;
        year_bef_flag = true;
        return *this;
    }

    CFilter& enrolledAfter(int year) {
        year_after = year;
        year_aft_flag = true;
        return *this;
    }


    bool match(const CStudent& student) const {
        // если не указан ни один фильтр возврат всех студентов
        if (!name_flag && !born_bef_flag && !born_aft_flag && !year_bef_flag && !year_aft_flag){
            return true;
        }



        // фильтр для день рождения
        if (born_bef_flag && student.getBirthDate() >= born_before){
            return false;
        }
        if (born_aft_flag && student.getBirthDate() <= born_after){
            return false;
        }

        // фильтр поступления
        if (year_bef_flag && student.getEnrollYear() >= year_before){
            return false;
        }
        if (year_aft_flag && student.getEnrollYear() <= year_after){
            return false;
        }
        // фильтр для имени
        if (name_flag && name_f.find(student.getNameSt()) == name_f.end()){
            return false;
        }
        return true; // студент прошел все указанные фильтры
    }

private:
    std::set<std::string> name_f;
    CDate born_before, born_after;
    int year_before, year_after;
    bool name_flag, born_bef_flag, born_aft_flag, year_bef_flag, year_aft_flag;

    friend class CStudyDept;
};

class CSort
{
public:
    CSort():criteria(){};

    CSort& addKey ( ESortKey key, bool ascending ){
        criteria.push_back({key, ascending});
        return *this;
    };

    void sortOpt(std::list<CStudent>& students)const{
        //нет критериев сортировки->возвращаем список как есть
        if (criteria.empty()) {
            students.sort([](const CStudent& a, const CStudent& b) {
                return a.getIndex() < b.getIndex();
            });
        }

        students.sort([this](const CStudent& a, const CStudent& b){
            for(size_t i = 0; i < criteria.size(); ++i){
                if (criteria[i].key == ESortKey::NAME){
                    if (a.getJustName() != b.getJustName()){
                        return criteria[i].ascend ? (a.getJustName() < b.getJustName()) : (a.getJustName() >= b.getJustName());
                    }
                }
                else if (criteria[i].key == ESortKey::BIRTH_DATE){
                    if (a.getBirthDate() != b.getBirthDate()){
                        return criteria[i].ascend ? (a.getBirthDate() < b.getBirthDate()) : (a.getBirthDate() >= b.getBirthDate());
                    }
                }
                else if (criteria[i].key == ESortKey::ENROLL_YEAR){
                    if (a.getEnrollYear() != b.getEnrollYear()) {
                        return criteria[i].ascend ? (a.getEnrollYear() < b.getEnrollYear()) : (a.getEnrollYear() >= b.getEnrollYear());
                    }
                }
            }
            return a.getIndex() < b.getIndex();
        });
    }
private:
    struct Scriterion{
        ESortKey key;
        bool ascend;
    };
    std::vector<Scriterion> criteria;
    // todo
};

struct FullStudent{
    bool operator()(const std::shared_ptr<CStudent>& lhs, const std::shared_ptr<CStudent>& rhs)const{
        if (lhs->getJustName() != rhs->getJustName()){
            return lhs->getJustName() < rhs->getJustName();
        }
        if (lhs->getBirthDate() != rhs->getBirthDate()){
            return lhs->getBirthDate() < rhs->getBirthDate();
        }
        return lhs->getEnrollYear() < rhs->getEnrollYear();
    }
};

//-----------------------------------------------------------------------------------



class CStudyDept
{
public:
    CStudyDept (): index(0){};

    bool addStudent ( const CStudent  & x ){
        auto student = std::make_shared<CStudent>(x);
        student->setIndex(++index);

        auto it = studentsFull.find(student);
        if (it != studentsFull.end()) {
            return false;
        }

        studentsFull.insert(student);
        return true;
    };

    bool delStudent ( const CStudent  & x ){
        auto student = std::make_shared<CStudent>(x);

        auto itFull = studentsFull.find(student);
        if (itFull == studentsFull.end()){
            return false;
        }
        studentsFull.erase(itFull);

        return true;
    };


    std::list<CStudent> search ( const CFilter& flt, const CSort& sortOpt) const{
        std::list<CStudent> result;
        for(const auto& student_ptr : studentsFull){
            if (flt.match(*student_ptr)) {
                result.push_back(*student_ptr);
            }
        }

        sortOpt.sortOpt(result);
        return result;
    };

    std::set<std::string> suggest ( const std::string & name ) const{
        std::set<std::string> words = normalize_name(name);
        std::set<std::string> results;

        for(auto studentIt = studentsFull.begin(); studentIt != studentsFull.end(); ++studentIt){
            const auto& student_words = (*studentIt)->getName();
            if(std::includes(student_words.begin(), student_words.end(),words.begin(),words.end())){
                results.insert((*studentIt)->getJustName());
            }
        }
        return results;
    };
private:
    int index;
    std::set<std::shared_ptr<CStudent>, FullStudent> studentsFull;
    // todo
};


#ifndef __PROGTEST__
int main ( void )
{
  CStudyDept x0;

  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2010 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2010 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1997, 6, 17), 2010 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1997, 6, 17), 2010 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1980, 4, 11), 2016 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1980, 4, 11), 2016 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2016 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1980, 4, 11), 2016 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2010 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2010 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "James Bond", CDate ( 1997, 6, 17), 2016 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "James Bond", CDate ( 1997, 6, 17), 2016 ) ) );
  assert ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) != CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2016 ) );
  assert ( ! ( CStudent ( "James Bond", CDate ( 1980, 4, 11), 2010 ) == CStudent ( "Peter Peterson", CDate ( 1997, 6, 17), 2016 ) ) );

  assert ( x0 . addStudent ( CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ) ) );
  assert ( x0 . addStudent ( CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ) ) );
  assert ( x0 . addStudent ( CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 ) ) );
  assert ( x0 . addStudent ( CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ) ) );
  assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
  assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ) ) );
  assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ) ) );
  assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ) ) );
  assert ( x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ) ) );
  assert ( x0 . addStudent ( CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ) ) );

  assert ( x0 . search ( CFilter (), CSort () ) == (std::list<CStudent>
  {
    CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
    CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
    CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 ),
    CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 )
  }) );


  assert ( x0 . search ( CFilter (), CSort () . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
  {
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
    CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
    CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
    CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
    CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 )
  }) );


  assert ( x0 . search ( CFilter (), CSort () . addKey ( ESortKey::NAME, false ) ) == (std::list<CStudent>
  {
    CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 ),
    CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
    CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
    CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 )
  }) );


  assert ( x0 . search ( CFilter (), CSort () . addKey ( ESortKey::ENROLL_YEAR, false ) . addKey ( ESortKey::BIRTH_DATE, false ) . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
  {
    CStudent ( "Peter John Taylor", CDate ( 1984, 1, 17), 2017 ),
    CStudent ( "John Peter Taylor", CDate ( 1983, 7, 13), 2014 ),
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
    CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 ),
    CStudent ( "Peter Taylor", CDate ( 1982, 2, 23), 2011 )
  }) );


  assert ( x0 . search ( CFilter () . name ( "james bond" ), CSort () . addKey ( ESortKey::ENROLL_YEAR, false ) . addKey ( ESortKey::BIRTH_DATE, false ) . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
  {
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 )
  }) );


  assert ( x0 . search ( CFilter () . bornAfter ( CDate ( 1980, 4, 11) ) . bornBefore ( CDate ( 1983, 7, 13) ) . name ( "John Taylor" ) . name ( "james BOND" ), CSort () . addKey ( ESortKey::ENROLL_YEAR, false ) . addKey ( ESortKey::BIRTH_DATE, false ) . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
  {
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
    CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 )
  }) );

  assert ( x0 . search ( CFilter () . name ( "james" ), CSort () . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
  {
  }) );

  assert ( x0 . suggest ( "peter" ) == (std::set<std::string>
  {
    "John Peter Taylor",
    "Peter John Taylor",
    "Peter Taylor"
  }) );

  assert ( x0 . suggest ( "bond" ) == (std::set<std::string>
  {
    "Bond James",
    "James Bond"
  }) );

  assert ( x0 . suggest ( "peter joHn" ) == (std::set<std::string>
  {
    "John Peter Taylor",
    "Peter John Taylor"
  }) );
  assert ( x0 . suggest ( "peter joHn bond" ) == (std::set<std::string>
  {
  }) );
  assert ( x0 . suggest ( "pete" ) == (std::set<std::string>
  {
  }) );
  assert ( x0 . suggest ( "peter joHn PETER" ) == (std::set<std::string>
  {
    "John Peter Taylor",
    "Peter John Taylor"
  }) );
  assert ( ! x0 . addStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
  assert ( x0 . delStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );

  assert ( x0 . search ( CFilter () . bornAfter ( CDate ( 1980, 4, 11) ) . bornBefore ( CDate ( 1983, 7, 13) ) . name ( "John Taylor" ) . name ( "james BOND" ), CSort () . addKey ( ESortKey::ENROLL_YEAR, false ) . addKey ( ESortKey::BIRTH_DATE, false ) . addKey ( ESortKey::NAME, true ) ) == (std::list<CStudent>
  {
    CStudent ( "James Bond", CDate ( 1982, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 8, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 17), 2013 ),
    CStudent ( "Bond James", CDate ( 1981, 7, 16), 2013 ),
    CStudent ( "James Bond", CDate ( 1981, 7, 16), 2012 ),
    CStudent ( "John Taylor", CDate ( 1981, 6, 30), 2012 )
  }) );
  assert ( ! x0 . delStudent ( CStudent ( "James Bond", CDate ( 1981, 7, 16), 2013 ) ) );
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */