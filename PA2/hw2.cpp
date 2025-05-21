#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <compare>
#include <stdexcept>
#endif /* __PROGTEST__ */

class CBigInt
{
    std::vector<int> numbers;
    bool negative= false;
public:
    // default constructor
    CBigInt()
        :negative(false){}
    // copying/assignment/destruction

    // int constructor
    CBigInt(int number){
        negative=number<0;
        if(number==0){
            numbers.push_back(0);
        }
        else{
            if(number<0){
                negative=true;
                number=-number;
            }
            while(number>0){
                numbers.push_back(number%10);
                number/=10;
            }
        }
    }

    // string constructor
    CBigInt(const std::string& str){
        if(str.empty()){
            throw std::invalid_argument("");
        }
        int index = 0;
        negative = (str[0] == '-');
        if(negative){
            index = 1;
        }
        for(int i = str.length()-1; i>=index; --i){
            char ch = str[i];
            if(std::isdigit(ch)){
                numbers.push_back(ch - '0');
            }
            else{
                throw std::invalid_argument("");
            }
        }
        if(numbers.empty()) {
            throw std::invalid_argument("");
        }

    }

    CBigInt& operator=(int number) {
        *this = CBigInt(number);
        return *this;
    }

    CBigInt& operator=(const std::string& str) {
        *this = CBigInt(str);
        return *this;
    }

    CBigInt& operator=(const CBigInt& other){
        if(this != &other){
            numbers = other.numbers;
            negative = other.negative;
        }
        return *this;
    }

    // operator +, any combination {CBigInt/int/string} + {CBigInt/int/string}
    friend CBigInt operator+(const CBigInt& a, const CBigInt& b);
    friend CBigInt operator+(const CBigInt& a, const std::string& b) {
        return a + CBigInt(b);
    }
    friend CBigInt operator+(const std::string& a, const CBigInt& b) {
        return CBigInt(a) + b;
    }
    friend CBigInt operator+(const std::string& a, const std::string& b) {
        return CBigInt(a) + CBigInt(b);
    }

    // operator *, any combination {CBigInt/int/string} * {CBigInt/int/string}
    friend CBigInt operator*(const CBigInt& a, const CBigInt& b);
    friend CBigInt operator*(const CBigInt& a, const std::string& b) {
        return a * CBigInt(b);
    }
    friend CBigInt operator*(const std::string& a, const CBigInt& b) {
        return CBigInt(a) * b;
    }
    friend CBigInt operator*(const std::string& a, const std::string& b) {
        return CBigInt(a) * CBigInt(b);
    }
    // operator +=, any of {CBigInt/int/string}
    CBigInt& operator+=(const CBigInt& num) {
        *this = *this + num;
        return *this;
    }
    CBigInt& operator+=(const std::string& str) {
        *this = *this + str;
        return *this;
    }
    // operator *=, any of {CBigInt/int/string}
    CBigInt& operator*=(const CBigInt& num) {
        *this = *this * num;
        return *this;
    }
    CBigInt& operator*=(const std::string& str) {
        *this = *this * str;
        return *this;
    }
    CBigInt& operator*=(const int num) {
        *this = *this * num;
        return *this;
    }
    // comparison operators, any combination {CBigInt/int/string} {<,<=,>,>=,==,!=} {CBigInt/int/string}
    //==
    friend bool operator==(const CBigInt& a, const CBigInt& b){
        return a.negative==b.negative && a.numbers==b.numbers;
    }
    friend bool operator==(const CBigInt& a, const std::string& str){
        return a==CBigInt(str);
    }
    friend bool operator==(const std::string& str, const CBigInt& b){
        return CBigInt(str)==b;
    }
    friend bool operator==(const std::string& a, const std::string& b){
        return CBigInt(a)==CBigInt(b);
    }
    //=!
    friend bool operator!=(const CBigInt& a, const CBigInt& b) {
        return !(a == b);
    }
    friend bool operator!=(const CBigInt& a, const std::string& str){
        return a!=CBigInt(str);
    }
    friend bool operator!=(const std::string& str, const CBigInt& b){
        return CBigInt(str)!=b;
    }
    friend bool operator!=(const std::string& a, const std::string& b){
        return CBigInt(a)!=CBigInt(b);
    }
    //<
    friend bool operator<(const CBigInt& a, const CBigInt& b) {
        if (a.negative != b.negative) {
            return a.negative;
        }
        int result = CBigInt::compare(a, b);
        return a.negative ? (result == 1) : (result == -1);
    }
    friend bool operator<(const CBigInt& a, const std::string& str){
        return a<CBigInt(str);
    }
    friend bool operator<(const std::string& str, const CBigInt& b){
        return CBigInt(str)<b;
    }
    friend bool operator<(const std::string& a, const std::string& b){
        return CBigInt(a)<CBigInt(b);
    }
    //>
    friend bool operator>(const CBigInt& a, const CBigInt& b) {
        return b<a;
    }
    friend bool operator>(const CBigInt& a, const std::string& str){
        return a>CBigInt(str);
    }
    friend bool operator>(const std::string& str, const CBigInt& b){
        return CBigInt(str)>b;
    }
    friend bool operator>(const std::string& a, const std::string& b){
        return CBigInt(a)>CBigInt(b);
    }
    //<=
    friend bool operator<=(const CBigInt& a, const CBigInt& b) {
        return !(a > b);
    }
    friend bool operator<=(const CBigInt& a, const std::string& str){
        return a<=CBigInt(str);
    }
    friend bool operator<=(const std::string& str, const CBigInt& b){
        return CBigInt(str)<=b;
    }
    friend bool operator<=(const std::string& a, const std::string& b){
        return CBigInt(a)<=CBigInt(b);
    }
    //>=
    friend bool operator>=(const CBigInt& a, const CBigInt& b) {
        return !(a < b);
    }
    friend bool operator>=(const CBigInt& a, const std::string& str){
        return a>=CBigInt(str);
    }
    friend bool operator>=(const std::string& str, const CBigInt& b){
        return CBigInt(str)>=b;
    }
    friend bool operator>=(const std::string& a, const std::string& b){
        return CBigInt(a)>=CBigInt(b);
    }

    // output operator <<
    friend std::ostream& operator<<(std::ostream& os,const CBigInt& num){
        if(num.negative){
            os<<'-';
        }
        bool zero=true;
        for(int i=num.numbers.size()-1;i>=0; --i){
            if(!zero || num.numbers[i] != 0){
                if (zero && num.numbers[i] != 0) {
                    zero = false;
                }
                os << num.numbers[i];
            }
        }
        if(zero){
            os<<'0';
        }
        return os;
    }
    // input operator >>
    friend std::istream& operator>>(std::istream& is, CBigInt &num){
        CBigInt result;
        char ch;

        while (std::isspace(is.peek())) is.get();

        if (is.peek() == '-' || is.peek() == '+') {
            ch = is.get();
            result.negative = (ch == '-');
        }

        bool read = false;
        while(std::isdigit(is.peek())){
            read = true;
            ch = is.get();
            result.numbers.insert(result.numbers.begin(), ch - '0');
        }

        if(!read) {
            is.setstate(std::ios::failbit);
        }
        else{
            result.normalize();
            num=result;
        }
        return is;
    }
private:
    //+metody
    static CBigInt add(const CBigInt& num1, const CBigInt& num2) {
        CBigInt number_result;
        int index=0;
        size_t max=std::max(num1.numbers.size(),num2.numbers.size());

        for(size_t i=0; i<max || index;++i){
            if(i==number_result.numbers.size()){
                number_result.numbers.push_back(0);
            }
            int number1=i < num1.numbers.size() ? num1.numbers[i] : 0;
            int number2=i < num2.numbers.size() ? num2.numbers[i] : 0;
            int sum=index+number1+number2;
            number_result.numbers[i]=sum%10;
            index=sum/10;
        }
        return number_result;
    }
    static CBigInt minus(const CBigInt& num1, const CBigInt& num2) {
        CBigInt number_result;
        int index = 0;
        size_t max=std::max(num1.numbers.size(),num2.numbers.size());

        for(size_t i=0; i<max; ++i){
            int number=i<num2.numbers.size()?num2.numbers[i]:0;
            int sub=num1.numbers[i]-number-index;
            if(sub<0){
                sub+=10;
                index=1;
            }
            number_result.numbers.push_back(sub);
        }
        return number_result;
    }
    static int compare(const CBigInt& num1, const CBigInt& num2) {
        if (num1.numbers.size() < num2.numbers.size()) return -1;
        if (num1.numbers.size() > num2.numbers.size()) return 1;

        for (int i = num1.numbers.size() - 1; i >= 0; --i) {
            if (num1.numbers[i] < num2.numbers[i]) return -1;
            if (num1.numbers[i] > num2.numbers[i]) return 1;
        }
        return 0;
    }
    static CBigInt simple(const CBigInt& a, const CBigInt& b){
        CBigInt result;
        size_t num1=a.numbers.size();
        size_t num2=b.numbers.size();
        result.numbers.resize(num1 + num2, 0);

        for(size_t i=0; i<num1;++i){
            int index=0;
            for(size_t j=0;j<num2 || index; ++j){
                int umnozeni = 0;
                if (j < num2) {
                    umnozeni = a.numbers[i] * b.numbers[j];
                }
                int odpoved=j<num2?umnozeni:0;
                int konec=odpoved+result.numbers[i+j]+index;
                result.numbers[i+j]=konec%10;
                index=konec/10;
            }
        }
        return result;
    }
    void normalize() {
        while (numbers.size() > 1 && numbers.back() == 0) {
            numbers.pop_back();
        }
        if (numbers.size() == 1 && numbers[0] == 0) {
            negative = false;
        }
    }
};

CBigInt operator+(const CBigInt& a, const CBigInt& b){
    CBigInt result;
    if(a.negative==b.negative){
        result=CBigInt::add(a,b);
        result.negative=a.negative;
    }
    else{
        if(CBigInt::compare(a,b)==0){
            result.numbers.push_back(0);
            result.negative=false;
        }
        else if(CBigInt::compare(a,b)==1){
            result=CBigInt::minus(a,b);
            result.negative=a.negative;
        }
        else if(CBigInt::compare(a,b)==-1){
            result=CBigInt::minus(b,a);
            result.negative=b.negative;
        }
    }
    result.normalize();
    return result;
}
CBigInt operator*(const CBigInt& a, const CBigInt& b){
    CBigInt result;
    if(a.negative==b.negative){
        result=CBigInt::simple(a,b);
        result.negative=false;
    }
    else{
        result=CBigInt::simple(a,b);
        result.negative=true;
    }
    result.normalize();
    return result;
}

#ifndef __PROGTEST__
static bool equal ( const CBigInt & x, const char val [] )
{
  std::ostringstream oss;
  oss << x;
  return oss . str () == val;
}
static bool equalHex ( const CBigInt & x, const char val [] )
{
  return true; // hex output is needed for bonus tests only
  // std::ostringstream oss;
  // oss << std::hex << x;
  // return oss . str () == val;
}

int main ()
{
  CBigInt a, b;
  std::istringstream is;

  a = 10;
  a += 20;
  assert ( equal ( a, "30" ) );
  a *= 5;
  assert ( equal ( a, "150" ) );
  b = a + 3;
  assert ( equal ( b, "153" ) );
  b = a * 7;
  assert ( equal ( b, "1050" ) );
  assert ( equal ( a, "150" ) );
  assert ( equalHex ( a, "96" ) );
  a = 10;
  a += -20;
  assert ( equal ( a, "-10" ) );
  a *= 5;
  assert ( equal ( a, "-50" ) );
  b = a + 73;
  assert ( equal ( b, "23" ) );
  b = a * -7;
  assert ( equal ( b, "350" ) );
  assert ( equal ( a, "-50" ) );
  assert ( equalHex ( a, "-32" ) );
  a = "12345678901234567890";
  a += "-99999999999999999999";
  assert ( equal ( a, "-87654321098765432109" ) );
  a *= "54321987654321987654";
  assert ( equal ( a, "-4761556948575111126880627366067073182286" ) );
  a *= 0;
  assert ( equal ( a, "0" ) );
  a = 10;
  b = a + "400";
  assert ( equal ( b, "410" ) );
  b = a * "15";
  assert ( equal ( b, "150" ) );
  assert ( equal ( a, "10" ) );
  assert ( equalHex ( a, "a" ) );
  is . clear ();
  is . str ( " 1234" );
  assert ( is >> b );
  assert ( equal ( b, "1234" ) );
  is . clear ();
  is . str ( " 12 34" );
  assert ( is >> b );
  assert ( equal ( b, "12" ) );
  is . clear ();
  is . str ( "999z" );
  assert ( is >> b );
  assert ( equal ( b, "999" ) );
  is . clear ();
  is . str ( "abcd" );
  assert ( ! ( is >> b ) );
  is . clear ();
  is . str ( "- 758" );
  assert ( ! ( is >> b ) );
  a = 42;
  try
  {
    a = "-xyz";
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::invalid_argument & e )
  {
    assert ( equal ( a, "42" ) );
  }
  a = "73786976294838206464";
  assert ( equal ( a, "73786976294838206464" ) );
  assert ( equalHex ( a, "40000000000000000" ) );
  assert ( a < "1361129467683753853853498429727072845824" );
  assert ( a <= "1361129467683753853853498429727072845824" );
  assert ( ! ( a > "1361129467683753853853498429727072845824" ) );
  assert ( ! ( a >= "1361129467683753853853498429727072845824" ) );
  assert ( ! ( a == "1361129467683753853853498429727072845824" ) );
  assert ( a != "1361129467683753853853498429727072845824" );
  assert ( ! ( a < "73786976294838206464" ) );
  assert ( a <= "73786976294838206464" );
  assert ( ! ( a > "73786976294838206464" ) );
  assert ( a >= "73786976294838206464" );
  assert ( a == "73786976294838206464" );
  assert ( ! ( a != "73786976294838206464" ) );
  assert ( a < "73786976294838206465" );
  assert ( a <= "73786976294838206465" );
  assert ( ! ( a > "73786976294838206465" ) );
  assert ( ! ( a >= "73786976294838206465" ) );
  assert ( ! ( a == "73786976294838206465" ) );
  assert ( a != "73786976294838206465" );
  a = "2147483648";
  assert ( ! ( a < -2147483648 ) );
  assert ( ! ( a <= -2147483648 ) );
  assert ( a > -2147483648 );
  assert ( a >= -2147483648 );
  assert ( ! ( a == -2147483648 ) );
  assert ( a != -2147483648 );
  a = "-12345678";
  assert ( ! ( a < -87654321 ) );
  assert ( ! ( a <= -87654321 ) );
  assert ( a > -87654321 );
  assert ( a >= -87654321 );
  assert ( ! ( a == -87654321 ) );
  assert ( a != -87654321 );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */