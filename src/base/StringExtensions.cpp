#include "../base/StringExtensions.h"
#include "../math/MathAll.h"

namespace BR2 {


std::string operator+(const std::string& str, const char& rhs) {
    return str + std::to_string(rhs);
}
std::string operator+(const std::string& str, const int8_t& rhs) {
    return str + std::to_string(rhs);
}
std::string operator+(const std::string& str, const int16_t& rhs) {
    return str + std::to_string(rhs);
}
std::string operator+(const std::string& str, const int32_t& rhs) {
    return str + std::to_string(rhs);
}
std::string operator+(const std::string& str, const int64_t& rhs) {
    return str + std::to_string(rhs);
}

std::string operator+(const std::string& str, const uint8_t& rhs) {
    return str + std::to_string(rhs);
}
std::string operator+(const std::string& str, const uint16_t& rhs) {
    return str + std::to_string(rhs);
}
std::string operator+(const std::string& str, const uint32_t& rhs) {
    return str + std::to_string(rhs);
}
std::string operator+(const std::string& str, const uint64_t& rhs) {
    return str + std::to_string(rhs);
}

std::string operator+(const std::string& str, const double& rhs) {
    return str + std::to_string(rhs);
}
std::string operator+(const std::string& str, const float& rhs) {
    return str + std::to_string(rhs);
}

std::string operator+(const std::string& str, const vec2& rhs) {
    return str + rhs.toString();
}
std::string operator+(const std::string& str, const vec3& rhs) {
    return str + rhs.toString();
}
std::string operator+(const std::string& str, const vec4& rhs) {
    return str + rhs.toString();
}

std::string operator+(const std::string& str, const ivec2& rhs) {
    return str + rhs.toString();
}
std::string operator+(const std::string& str, const ivec3& rhs) {
    return str + rhs.toString();
}
std::string operator+(const std::string& str, const ivec4& rhs) {
    return str + rhs.toString();
}

std::string operator+(const std::string& str, const uvec2& rhs) {
    return str + rhs.toString();
}
std::string operator+(const std::string& str, const uvec3& rhs) {
    return str + rhs.toString();
}
std::string operator+(const std::string& str, const uvec4& rhs) {
    return str + rhs.toString();
}

std::string operator+(const std::string& str, const mat3& rhs) {
    return str + rhs.toString();
}
std::string operator+(const std::string& str, const mat4& rhs) {
    return str + rhs.toString();
}







//Added this on 8/5 hopefully doing away with TStr
void StringExtensions::Test() {

    std::string st = "";


#define Test(s, a) \
   { \
    s = std::string(" "); s += #a; s += ": "; \
    std::cout << (s + a) << std::endl; \
    }

    /*
        s = std::string(" "); s += #a; s += ": "; \
    s += a; \
    std::cout << (s) << std::endl; \
    */

    //Signed
    char _char_t = -100;
    Test(st, _char_t);

    int8_t _int8_t = -100;
    Test(st, _int8_t);

    int16_t _int16_t = -30000;
    Test(st, _int16_t);

    int32_t _int32_t = -2000000000;
    Test(st, _int32_t);

    int64_t _int64_t = -4000000000000000000;
    Test(st, _int64_t);

    //Unsigned
    unsigned char _uchar_t = 200;
    Test(st, _uchar_t);

    uint8_t _uint8_t = 200;
    Test(st, _uint8_t);

    uint16_t _uint16_t = 30000;
    Test(st, _uint16_t);

    uint32_t _uint32_t = 4000000000;
    Test(st, _uint32_t);

    uint64_t _uint64_t = 9000000000000000000;
    Test(st, _uint64_t);

    //Float
    float _float_t = 5.0f;
    Test(st, _float_t);

    double _double_t = 3.14159265358979323846;
    Test(st, _double_t);

    //Class
    std::string _std_string_t = "std::string";
    Test(st, _std_string_t);

    const char* _char_ptr_t = "char*";
    Test(st, _char_ptr_t);


    std::string test;

    test = test + 10;
    std::cout << test << std::endl;

    test = test + 5.5;
    test += 5.5;
    std::cout << test << std::endl;

    test = test + " hi " + 2.4 + " I'm gay " + 3.14 + 10;
    std::cout << test << std::endl;

    std::cin.get();

}









}//ns Game
