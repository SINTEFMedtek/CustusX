//===========================================================================
//
// File: cxInput2DUStemplate.h
//
// Created: 06.05.2008
//
/// \file
///       Templated input2DUS class
/// \author Ole Vegard Solberg
//===========================================================================

#ifndef _2DUS_TEMPLATE_H_
#define _2DUS_TEMPLATE_H_

#include "cxInput2DUS.h"

template <class Tsample>
class Input2DUSTemplate : public Input2DUS
{
public:
  typedef std::vector<Tsample> Beam;
  typedef std::vector<Beam> Frame;
  Input2DUSTemplate<Tsample>(const Input2DUS& echopac)
    :Input2DUS(echopac)
  {
  }
  std::vector<Frame> *volume;
  
  //test
  std::vector<Tsample*> *vol;
  
  ~Input2DUSTemplate<Tsample>()
  {
  }
};

#endif

//===========================================================================
// $Log:$
//
//===========================================================================