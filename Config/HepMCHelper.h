// -*- C++ -*-
//
// HepMCHelper_HepMC.h is a part of ThePEG - A multi-purpose Monte Carlo event generator
// Copyright (C) 2002-2019 The Herwig Collaboration
//
// ThePEG is licenced under version 3 of the GPL, see COPYING for details.
// Please respect the MCnet academic guidelines, see GUIDELINES for details.
//
//
// This is a helper header to implement HepMC conversions
//
#include "ThePEG/Vectors/HepMCTraits.h"
#ifdef  HAVE_HEPMC3
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/Version.h"   
#include "HepMC3/WriterAscii.h"
#include "HepMC3/WriterHEPEVT.h"
#include "HepMC3/WriterAsciiHepMC2.h"
#ifdef HAVE_HEPMC3_WRITERROOT_H
#include "HepMC3/WriterRoot.h"
#endif
#ifdef HAVE_HEPMC3_WRITERROOTTREE_H
#include "HepMC3/WriterRootTree.h"
#endif
namespace HepMC3 {
using PdfInfo=GenPdfInfo;
using Polarization=std::pair<double,double>;
}
namespace HepMC=HepMC3;
#else
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "HepMC/Version.h"
#include "HepMC/IO_BaseClass.h"
#include "HepMC/IO_GenEvent.h"
#include "HepMC/IO_AsciiParticles.h"
namespace HepMC {
#undef GenParticlePtr
#undef GenVertexPtr
typedef GenParticle * GenParticlePtr;
typedef GenVertex * GenVertexPtr;
}
#endif
namespace ThePEG {
/**
 * Struct for HepMC conversion
 */
#ifndef HAVE_HEPMC3
template<> 
struct HepMCTraits<HepMC::GenEvent> 
  : public HepMCTraitsBase<HepMC::GenEvent,
			   HepMC::GenParticle,
			   HepMC::GenParticle *,
			   HepMC::GenVertex,
			   HepMC::GenVertex *,
			   HepMC::Polarization,
			   HepMC::PdfInfo>
{
#else
  // This is version 3!
template<> 
struct HepMCTraits<HepMC::GenEvent> 
  : public HepMCTraitsBase<HepMC::GenEvent,
			   HepMC::GenParticle,
			   HepMC::GenParticlePtr,
			   HepMC::GenVertex,
			   HepMC::GenVertexPtr,
			   HepMC::Polarization,
			   HepMC::PdfInfo>
{

  /** Create an event object with number \a evno and \a weight. */
  static EventT * newEvent(long evno, double weight,
			   const map<string,double>& optionalWeights) {
    EventT * e = new EventT(HepMC::Units::GEV, HepMC::Units::MM);
    e->set_event_number(evno);
    e->set_event_number(evno);
    //std::vector<std::string> wnames;
    std::vector<double> wvalues;
    
    //wnames.push_back("Default");
    wvalues.push_back(weight);
    for ( map<string,double>::const_iterator w = optionalWeights.begin();
	  w != optionalWeights.end(); ++w ) {
    //wnames.push_back(w->first);
    wvalues.push_back(w->second);
    }
    //e->run_info()->set_weight_names(wnames);
    e->weights()=wvalues;    
  return e;
  }

  /** Create a new vertex. */
  static VertexPtrT newVertex() {
    return std::make_shared<VertexT>(VertexT());
  }

  /** Set the \a scale, \f$\alpha_S\f$ (\a aS) and \f$\alpha_{EM}\f$
      (\a aEM) for the event \a e. The scale will be scaled with \a
      unit before given to the GenEvent. */
  static void setScaleAndAlphas(EventT & e, Energy2 scale,
				double aS,  double aEM, Energy unit) {
    e.add_attribute("event_scale",std::make_shared<HepMC::DoubleAttribute>(sqrt(scale)/unit));
    e.add_attribute("mpi",std::make_shared<HepMC::IntAttribute>(-1));//Please fix it later, once ThePEG authors respond
    e.add_attribute("signal_process_id",std::make_shared<HepMC::IntAttribute>(0));//Please fix it later, once ThePEG authors respond
    e.add_attribute("alphaQCD",std::make_shared<HepMC::DoubleAttribute>(aS));
    e.add_attribute("alphaQED",std::make_shared<HepMC::DoubleAttribute>(aEM));
  }

  /** Set the colour line (with index \a indx) to \a coline for
      particle \a p. */
  static void setColourLine(ParticleT & p, int indx, int coline) {
   p.add_attribute("flow"+std::to_string(indx),std::make_shared<HepMC::IntAttribute>(coline));
  }

  /** Add an incoming particle, \a p, to the vertex, \a v. */
  static void addIncoming(VertexT & v, ParticlePtrT p) {
    v.add_particle_in(p);
  }

  /** Add an outgoing particle, \a p, to the vertex, \a v. */
  static void addOutgoing(VertexT & v, ParticlePtrT p) {
    v.add_particle_out(p);
  }

  /** Set the primary vertex, \a v, for the event \a e. */
  static void setSignalProcessVertex(EventT & e, VertexPtrT v) {
    e.add_vertex(v);
    e.add_attribute("signal_process_vertex",
                    std::make_shared<HepMC::IntAttribute>(v->id()));
  }

  /** Set a vertex, \a v, for the event \a e. */
  static void addVertex(EventT & e, VertexPtrT v) {
    e.add_vertex(v);
  }

  /** Set the beam particles for the event.*/
  static void setBeamParticles(EventT & e, ParticlePtrT p1, ParticlePtrT p2) {
    //    e.set_beam_particles(p1,p2);
    p1->set_status(4);
    p2->set_status(4);
    e.set_beam_particles(p1, p2);
  }

  /** Create a new particle object with momentum \a p, PDG number \a
      id and status code \a status. The momentum will be scaled with
      \a unit which according to the HepMC documentation should be
      GeV. */
  static ParticlePtrT newParticle(const Lorentz5Momentum & p,
                                    long id, int status, Energy unit) {
    // Note that according to the documentation the momentum is stored in a
    // HepLorentzVector in GeV (event though the CLHEP standard is MeV).
    HepMC::FourVector p_scalar(p.x()/unit, p.y()/unit, p.z()/unit, p.e()/unit);
    ParticlePtrT genp = std::make_shared<ParticleT>(ParticleT(p_scalar, id, status));
    genp->set_generated_mass(p.mass()/unit);
    return genp;
  }

  /** Set the polarization directions, \a the and \a phi, for particle
      \a p. */
  static void setPolarization(ParticleT & genp, double the, double phi) {
    genp.add_attribute("theta",std::make_shared<HepMC::DoubleAttribute>(the));
    genp.add_attribute("phi",std::make_shared<HepMC::DoubleAttribute>(phi));
   }

  /** Set the position \a p for the vertex, \a v. The length will be
      scaled with \a unit which normally should be millimeters. */
  static void setPosition(VertexT & v, const LorentzPoint & p, Length unit) {
    HepMC::FourVector v_scaled(p.x()/unit, p.y()/unit, p.z()/unit, p.e()/unit);
    v.set_position(v_scaled);
  }

#endif

};
}
