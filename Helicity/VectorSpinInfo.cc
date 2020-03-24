// -*- C++ -*-
//
// VectorSpinInfo.cc is a part of ThePEG - Toolkit for HEP Event Generation
// Copyright (C) 2003-2019 Peter Richardson, Leif Lonnblad
//
// ThePEG is licenced under version 3 of the GPL, see COPYING for details.
// Please respect the MCnet academic guidelines, see GUIDELINES for details.
//
//
// This is the implementation of the non-inlined, non-templated member
// functions of the VectorSpinInfo class.
//
// Author: Peter Richardson
//

#include "VectorSpinInfo.h"
#include "ThePEG/Utilities/DescribeClass.h"

using namespace ThePEG;
using namespace ThePEG::Helicity;

EIPtr VectorSpinInfo::clone() const {
  tcSpinPtr temp=this;
  return const_ptr_cast<SpinPtr>(temp);
}

// The following static variable is needed for the type
// description system in ThePEG.
DescribeNoPIOClass<VectorSpinInfo,SpinInfo>
describeThePEGVectorSpinInfo("ThePEG::VectorSpinInfo", "libThePEG.so");

void VectorSpinInfo::Init() {}

void VectorSpinInfo::transform(const LorentzMomentum & m,
			       const LorentzRotation & r) {
  if(isNear(m)) {
    for(unsigned int ix=0;ix<3;++ix) _currentstates[ix].transform(r.one());
    SpinInfo::transform(m,r);
  }
}
