//
// Created by zvone on 27-Sep-19.
//

#ifndef PG1_2019_VECTOR_H
#define PG1_2019_VECTOR_H

struct Vector2 {
  union {
    struct {
      float x; /*!< První složka vektoru. */
      float y; /*!< Druhá složka vektoru. */
    };
    struct {
      float u; /*!< První složka vektoru. */
      float v; /*!< Druhá složka vektoru. */
    };
    float data[2];  /*!< Pole složek vektoru. */
    
  };
  
  explicit Vector2(const float *v);
  
  Vector2(float fx, float fy) : x{fx}, y{fy} {}
  
  Vector2(const Vector2 &v) : Vector2(v.x, v.y) {}
  
  explicit Vector2(float fxy = 0.f) : Vector2(fxy, fxy) {}
  
  void Print();
  
  explicit Vector2(float *array);
  
  friend Vector2 operator-(const Vector2 &v);
  
  friend Vector2 operator+(const Vector2 &u, const Vector2 &v);
  
  friend Vector2 operator-(const Vector2 &u, const Vector2 &v);
  
  friend Vector2 operator*(const Vector2 &v, const float a);
  
  friend Vector2 operator*(const float a, const Vector2 &v);
  
  friend Vector2 operator*(const Vector2 &u, const Vector2 &v);
  
  friend Vector2 operator/(const Vector2 &v, const float a);
  
  friend void operator+=(Vector2 &u, const Vector2 &v);
  
  friend void operator-=(Vector2 &u, const Vector2 &v);
  
  friend void operator*=(Vector2 &v, const float a);
  
  friend void operator/=(Vector2 &v, const float a);
};

struct Vector3 {
  union  // anonymní unie
  {
    struct {
      float x; /*!< První složka vektoru. */
      float y; /*!< Druhá složka vektoru. */
      float z; /*!< Třetí složka vektoru. */
    };
    struct {
      float u; /*!< První složka vektoru. */
      float v; /*!< Druhá složka vektoru. */
      float w; /*!< Třetí složka vektoru. */
    };
    
    struct {
      float r; /*!< První složka vektoru. */
      float g; /*!< Druhá složka vektoru. */
      float b; /*!< Třetí složka vektoru. */
    };
    
    float data[3]{}; /*!< Pole složek vektoru. */
    
    
  };
  
  //! Obecný konstruktor.
  /*!
  Inicializuje složky vektoru podle zadaných hodnot parametrů,
  \f$\mathbf{v}=(x,y,z)\f$.

  \param x první složka vektoru.
  \param y druhá složka vektoru.
  \param z třetí složka vektoru.
  */
  Vector3(const float x, const float y, const float z) : x(x), y(y), z(z) {}
  
  
  /**
   * @brief Konstruktor pro vytvoření vektoru z jedné hodnoty.
   * @param xyz První, druhá a třetí složka vektoru.
   */
  Vector3(const float xyz = 0.f) : x(xyz), y(xyz), z(xyz) {}
  
  //! Konstruktor z pole.
  /*!
  Inicializuje složky vektoru podle zadaných hodnot pole,

  \param v ukazatel na první složka vektoru.
  */
  Vector3(const float *v);
  
  Vector3(const Vector3 &v) : x{v.x}, y{v.y}, z{v.z} {};
  
  Vector3(const Vector2 &v, float z = 0.f) : x{v.x}, y{v.y}, z{z} {};
  
  //! L2-norma vektoru.
  /*!
  \return x Hodnotu \f$\mathbf{||v||}=\sqrt{x^2+y^2+z^2}\f$.
  */
  float L2Norm() const;
  
  //! Druhá mocnina L2-normy vektoru.
  /*!
  \return Hodnotu \f$\mathbf{||v||^2}=x^2+y^2+z^2\f$.
  */
  float SqrL2Norm() const;
  
  //! Normalizace vektoru.
  /*!
  Po provedení operace bude mít vektor jednotkovou délku.
  */
  void Normalize();
  
  //! Vektorový součin.
  /*!
  \param v vektor \f$\mathbf{v}\f$.

  \return Vektor \f$(\mathbf{u}_x \mathbf{v}_z - \mathbf{u}_z \mathbf{v}_y,
  \mathbf{u}_z \mathbf{v}_x - \mathbf{u}_x \mathbf{v}_z,
  \mathbf{u}_x \mathbf{v}_y - \mathbf{u}_y \mathbf{v}_x)\f$.
  */
  Vector3 CrossProduct(const Vector3 &v) const;
  
  Vector3 Abs() const;
  
  Vector3 Max(const float a = 0) const;
  
  //! Skalární součin.
  /*!
  \return Hodnotu \f$\mathbf{u}_x \mathbf{v}_x + \mathbf{u}_y \mathbf{v}_y + \mathbf{u}_z \mathbf{v}_z)\f$.
  */
  float DotProduct(const Vector3 &v) const;
  
  //! Index největší složky vektoru.
  /*!
  \param absolute_value index bude určen podle absolutní hodnoty složky

  \return Index největší složky vektoru.
  */
  char LargestComponent(const bool absolute_value = false);
  
  void Print();
  
  Vector3 reflect(const Vector3 &direction);
  
  // --- operátory ------
  
  friend Vector3 operator-(const Vector3 &v);
  
  friend Vector3 operator+(const Vector3 &u, const Vector3 &v);
  
  friend Vector3 operator-(const Vector3 &u, const Vector3 &v);
  
  friend Vector3 operator*(const Vector3 &v, const float a);
  
  friend Vector3 operator*(const float a, const Vector3 &v);
  
  friend Vector3 operator*(const Vector3 &u, const Vector3 &v);
  
  friend Vector3 operator/(const Vector3 &v, const float a);
  
  friend void operator+=(Vector3 &u, const Vector3 &v);
  
  friend void operator-=(Vector3 &u, const Vector3 &v);
  
  friend void operator*=(Vector3 &v, const float a);
  
  friend void operator/=(Vector3 &v, const float a);
};

struct Vector4 {
  union {
    struct {
      float r; /*!< První složka vektoru. */
      float g; /*!< Druhá složka vektoru. */
      float b; /*!< Třetí složka vektoru. */
      float a; /*!< Čtvrtá složka vektoru. */
    };
    struct {
      float x; /*!< První složka vektoru. */
      float y; /*!< Druhá složka vektoru. */
      float z; /*!< Třetí složka vektoru. */
      float w; /*!< Čtvrtá složka vektoru. */
    };
    float data[4]{}; /*!< Pole složek vektoru. */
  };
  
  Vector4(float fr, float fg, float fb, float fa) : r{fr}, g{fg}, b{fb}, a{fa} {}
  
  explicit Vector4(const Vector2 &v, float fb = 0.f, float fa = 0.f) : Vector4(v.x, v.y, fb, fa) {}
  
  explicit Vector4(const Vector3 &v, float fa = 0.f) : Vector4(v.x, v.y, v.z, fa) {}
  
  Vector4(const Vector4 &v) : Vector4(v.r, v.g, v.b, v.a) {}
  
  explicit Vector4(float rgba = 0.f) : Vector4(rgba, rgba, rgba, rgba) {}
  
  explicit Vector4(float *array);
  
  Vector3 getRgb(){return Vector3(r,g,b);}
  
  void Print();
  
  friend Vector4 operator-(const Vector4 &v);
  
  friend Vector4 operator+(const Vector4 &u, const Vector4 &v);
  
  friend Vector4 operator-(const Vector4 &u, const Vector4 &v);
  
  friend Vector4 operator*(const Vector4 &v, const float a);
  
  friend Vector4 operator*(const float a, const Vector4 &v);
  
  friend Vector4 operator*(const Vector4 &u, const Vector4 &v);
  
  friend Vector4 operator/(const Vector4 &v, const float a);
  
  friend void operator+=(Vector4 &u, const Vector4 &v);
  
  friend void operator-=(Vector4 &u, const Vector4 &v);
  
  friend void operator*=(Vector4 &v, const float a);
  
  friend void operator/=(Vector4 &v, const float a);
};

using Color4f = Vector4;
using Color3f = Vector3;
using Vertex3f = Vector3;
using Normal3f = Vector3;
using Coord2f = Vector2;

#endif //PG1_2019_VECTOR_H
