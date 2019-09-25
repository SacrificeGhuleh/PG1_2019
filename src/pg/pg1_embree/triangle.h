#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "vertex.h"

class Surface; // dopředná deklarace třídy

/*! \class Triangle
\brief A class representing single triangle in 3D.

\author Tomáš Fabián
\version 1.1
\date 2013-2018
*/
class Triangle
{
public:	
	//! Výchozí konstruktor.
	/*!
	Inicializuje všechny složky trojúhelníku na hodnotu nula.
	*/
	Triangle() { }

	//! Obecný konstruktor.
	/*!
	Inicializuje trojúhelník podle zadaných hodnot parametrů.

	\param v0 první vrchol trojúhelníka.
	\param v1 druhý vrchol trojúhelníka.
	\param v2 třetí vrchol trojúhelníka.
	\param surface ukazatel na plochu, jíž je trojúhelník členem.
	*/
	Triangle( const Vertex & v0, const Vertex & v1, const Vertex & v2, Surface * surface = NULL );

	//! I-tý vrchol trojúhelníka.
	/*!
	\param i index vrcholu trojúhelníka.

	\return I-tý vrchol trojúhelníka.
	*/
	Vertex vertex( const int i );
	
	//! Ukazatel na síť, jíž je trojúhelník členem.
	/*!
	\return Ukazatel na síť.
	*/	
	Surface * surface();

protected:

private:
	Vertex vertices_[3]; /*!< Vrcholy trojúhelníka. Nic jiného tu nesmí být, jinak padne VBO v OpenGL! */	
};

#endif
