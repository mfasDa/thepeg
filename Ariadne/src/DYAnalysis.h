// -*- C++ -*-
#ifndef THEPEG_DYAnalysis_H
#define THEPEG_DYAnalysis_H
//
// This is the declaration of the DYAnalysis class.
//

#include "ThePEG/Handlers/AnalysisHandler.h"

namespace Ariadne {

using namespace ThePEG;

/**
 * The DYAnalysis class collects a number of histograms related to
 * Drell-Yan production.
 *
 */
class DYAnalysis: public AnalysisHandler {

public:

  /** @name Standard constructors and destructors. */
  //@{
  /**
   * The default constructor.
   */
  inline DYAnalysis();

  /**
   * The copy constructor.
   */
  inline DYAnalysis(const DYAnalysis &);

  /**
   * The destructor.
   */
  virtual ~DYAnalysis();
  //@}

public:

  /** @name Virtual functions required by the AnalysisHandler class. */
  //@{
  /**
   * Analyze a given Event. Note that a fully generated event
   * may be presented several times, if it has been manipulated in
   * between. The default version of this function will call transform
   * to make a lorentz transformation of the whole event, then extract
   * all final state particles and call analyze(tPVector) of this
   * analysis object and those of all associated analysis objects. The
   * default version will not, however, do anything on events which
   * have not been fully generated, or have been manipulated in any
   * way.
   * @param event pointer to the Event to be analyzed.
   * @param ieve the event number.
   * @param loop the number of times this event has been presented.
   * If negative the event is now fully generated.
   * @param state a number different from zero if the event has been
   * manipulated in some way since it was last presented.
   */
  virtual void analyze(tEventPtr event, long ieve, int loop, int state);

  /**
   * Transform the event to the desired Lorentz frame and return the
   * corresponding LorentzRotation.
   * @param event a pointer to the Event to be transformed.
   * @return the LorentzRotation used in the transformation.
   */
  virtual LorentzRotation transform(tEventPtr event) const;

  /**
   * Analyze the given vector of particles. The default version calls
   * analyze(tPPtr) for each of the particles.
   * @param particles the vector of pointers to particles to be analyzed
   */
  virtual void analyze(const tPVector & particles);

  /**
   * Analyze the given particle.
   * @param particle pointer to the particle to be analyzed.
   */
  virtual void analyze(tPPtr particle);
  //@}

public:

  /** @name Functions used by the persistent I/O system. */
  //@{
  /**
   * Function used to write out object persistently.
   * @param os the persistent output stream written to.
   */
  void persistentOutput(PersistentOStream & os) const;

  /**
   * Function used to read in object persistently.
   * @param is the persistent input stream read from.
   * @param version the version number of the object when written.
   */
  void persistentInput(PersistentIStream & is, int version);
  //@}

  /**
   * The standard Init function used to initialize the interfaces.
   * Called exactly once for each class by the class description system
   * before the main function starts or
   * when this class is dynamically loaded.
   */
  static void Init();

protected:

  /** @name Clone Methods. */
  //@{
  /**
   * Make a simple clone of this object.
   * @return a pointer to the new object.
   */
  inline virtual IBPtr clone() const;

  /** Make a clone of this object, possibly modifying the cloned object
   * to make it sane.
   * @return a pointer to the new object.
   */
  inline virtual IBPtr fullclone() const;
  //@}

protected:

  /** @name Standard Interfaced functions. */
  //@{
  /**
   * Initialize this object after the setup phase before saving and
   * EventGenerator to disk.
   * @throws InitException if object could not be initialized properly.
   */
  virtual void doinit() throw(InitException);

  /**
   * Initialize this object. Called in the run phase just before
   * a run begins.
   */
  virtual void doinitrun();

  /**
   * Finalize this object. Called in the run phase just after a
   * run has ended. Used eg. to write out statistics.
   */
  virtual void dofinish();
  //@}

private:

  tH1DPtr histPt, histEta, histY, histYQ, histYG, histMass, histCPt;
  tH1DPtr histNG, histPTG, histPTQ;

private:

  /**
   * The static object used to initialize the description of this class.
   * Indicates that this is a concrete class with persistent data.
   */
  static ClassDescription<DYAnalysis> initDYAnalysis;

  /**
   * The assignment operator is private and must never be called.
   * In fact, it should not even be implemented.
   */
  DYAnalysis & operator=(const DYAnalysis &);

};

}

// CLASSDOC OFF

#include "ThePEG/Utilities/ClassTraits.h"

namespace ThePEG {

/** @cond TRAITSPECIALIZATIONS */

/** This template specialization informs ThePEG about the
 *  base classes of DYAnalysis. */
template <>
struct BaseClassTrait<Ariadne::DYAnalysis,1> {
  /** Typedef of the first base class of DYAnalysis. */
  typedef AnalysisHandler NthBase;
};

/** This template specialization informs ThePEG about the name of
 *  the DYAnalysis class and the shared object where it is defined. */
template <>
struct ClassTraits<Ariadne::DYAnalysis>
  : public ClassTraitsBase<Ariadne::DYAnalysis> {
  /** Return a platform-independent class name */
  static string className() { return "Ariadne::DYAnalysis"; }
  /** Return the name of the shared library be loaded to get
   *  access to the DYAnalysis class and every other class it uses
   *  (except the base class). */
  static string library() { return "DYAnalysis.so"; }
};

/** @endcond */

}

#include "DYAnalysis.icc"
#ifndef ThePEG_TEMPLATES_IN_CC_FILE
// #include "DYAnalysis.tcc"
#endif

#endif /* THEPEG_DYAnalysis_H */
