#ifndef UTILS_H_
#define UTILS_H_

#define MAT_ELEM(mat, type, x, y) (reinterpret_cast<(type) *>( ( mat ).data + \
  ( mat ).step * ( y ) + ( mat ).elemSize() * ( x ) ))

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(type_name) \
  type_name( const type_name & ); \
  void operator=( const type_name & )

#define SAFE_DELETE(p) { \
  if ( (p) != NULL ) \
  { \
    delete (p); \
    (p) = NULL; \
  } \
}

#define SAFE_DELETE_ARRAY(p) { \
  if ( (p) != NULL ) \
  { \
    delete [] (p); \
    (p) = NULL; \
  } \
}

/*! \fn float template<typename T> void SafeDeleteVectorItems( std::vector<T> v )
\brief Dealokuje všechny prvky typu T vektoru v.
\param v Standardní vektor.
*/
template<typename T>
void SafeDeleteVectorItems(std::vector<T> v) {
  while (v.size() > 0) {
    T item = v.back();
    v.pop_back();
    SAFE_DELETE(item)
  }
}

namespace utils {
  /*! \fn void swap( T & a, T & b )
  \brief Prohodí hodnoty a, b.
  \param a Prvni T hodnota.
  \param b Druha T hodnota.
  */
  template<typename T>
  void swap(T &a, T &b) {
    const T tmp = a;
    a = b;
    b = tmp;
  }
}

/*! \fn float Random( const float range_min, const float range_max )
\brief Vrátí pseudonáhodné číslo s normálním rozdělením v intervalu <range_min, range_max).
\param range_min Dolní mez intervalu.
\param range_max Horní mez intervalu.
\return Pseudonáhodné číslo.
*/
float Random(float range_min = 0.0f, float range_max = 1.0f);

/*! \fn long long GetFileSize64( const char * file_name )
\brief Vrátí velikost souboru v bytech.
\param file_name Úplná cesta k souboru
*/
long long GetFileSize64(const char *file_name);

/*! \fn void PrintTime( double t )
\brief Vytiskne na stdout čas ve formátu Dd:Mm:Ss.
\param t čas v sekundách.
*/
void PrintTime(double t, char *buffer);

/*! \fn std::string TimeToString( const double t )
\brief Převede čas v sekundách do řetězce ve formátu Dd:Mm:Ss nebo Ss.m nebo m.u
\param t čas v sekundách.
*/
std::string TimeToString(const double t);

/*! \fn char * LTrim( char * s )
\brief Ořeže řetězec o bílé znaky zleva.
\param s ukazatel na řetězec
\return Ukazatel na novou pozici v tomtéž řetězci
*/
char *LTrim(char *s);

/*! \fn char * RTrim( char * s )
\brief Ořeže řetězec o bílé znaky zprava.
\param s ukazatel na řetězec
\return Ukazatel na novou pozici v tomtéž řetězci
*/
char *RTrim(char *s);

/*! \fn char * Trim( char * s )
\brief Ořeže řetězec o bílé znaky z obou stran.
\param s ukazatel na řetězec
\return Ukazatel na novou pozici v tomtéž řetězci
*/
char *Trim(char *s);


template<class T_TYPE>
T_TYPE c_linear(T_TYPE c_srgb, float gamma = 2.4f);

template<class T_TYPE>
T_TYPE c_srgb(T_TYPE c_linear, float gamma = 2.4f);

template<>
float c_linear<float>(float c_srgb, float gamma);

template<>
float c_srgb<float>(float c_linear, float gamma);

template<>
glm::vec3 c_linear<glm::vec3>(glm::vec3 c_srgb, float gamma);

template<>
glm::vec3 c_srgb<glm::vec3>(glm::vec3 c_linear, float gamma);

template<>
glm::vec4 c_linear<glm::vec4>(glm::vec4 c_srgb, float gamma);

template<>
glm::vec4 c_srgb<glm::vec4>(glm::vec4 c_linear, float gamma);

#endif
