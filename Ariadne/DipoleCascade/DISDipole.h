// -*- C++ -*-
#ifndef ARIADNE_DISDipole_H
#define ARIADNE_DISDipole_H
//
// This is the declaration of the DISDipole class.
//

#include "ExtendedDipole.h"
#include "DISDipole.fh"
#include "HardRemnant.fh"

namespace Ariadne {

using namespace ThePEG;

/**
 * The DISDipole class represents the QCD dipole that performs the first
 * emission in a DIS process.
 */
class DISDipole: public ExtendedDipole {

public:

  /** @name Standard constructors and destructors. */
  //@{
  /**
   * The default constructor.
   */
  inline DISDipole();

  /**
   * The copy constructor.
   */
  inline DISDipole(const DISDipole &);

  /**
   * The destructor.
   */
  virtual ~DISDipole();
  //@}

public:

  /** @name The main virtual functions to be overridden by sub-classes. */
  //@{
  /**
   * Initialize this dipole. Given then incoming and outgoing partons,
   * \a ip and \a op, set up this dipole. If \a respectScales is true
   * the dipoles will get a maximum scale set by the minimum
   * scale of the original partons.
   */
  virtual void init(tParPtr ip, tParPtr op, bool respectScale = false);

  /**
   * Generate the variables needed to perform an emission. The
   * variables should be saved so that the emission can be performed
   * by a subsequent call to perform().
   *
   * @param pt2min the minimum transverse momentum squared allowed.
   *
   * @param pt2max the minimum transverse momentum squared allowed.
   *
   * @return the transverse momentum squared of the generated
   * emission. If pt2min is returned, no emission was generated.
   */
  virtual Energy2 generate(Energy2 pt2min, Energy2 pt2max);

  /**
   * Perform an emission. Use the information previously generated by
   * generate(Energy2,Energy2) to perform the corresponding emission.
   *
   * @return the parton which is considered to have been emitted, or
   * the null pointer if the emission failed.
   */
  virtual tParPtr perform();
  //@}


protected:

  /**
   * Generate a gluon emission. Saves x1, x3 and rotation angle in
   * genVar.
   *
   * @param pt2min the minimum transverse momentum squared allowed.
   *
   * @param pt2max the minimum transverse momentum squared allowed.
   */
  virtual void genG(Energy2 pt2min, Energy2 pt2max);

  /**
   * Implemented to disable the quark anti-quark splitting.
   */
  virtual void genQ(Energy2 pt2min, Energy2 pt2max);

  /**
   * Generate a gluon emission from a dipole between the remnant and the
   * hard remant. Saves rapidity in genVar. Uses the perform method for
   * initial state gluons to perform the emission.
   *
   * @param pt2min the minimum transverse momentum squared allowed.
   *
   * @param pt2max the minimum transverse momentum squared allowed.
   */
  void genDISG(Energy2 pt2min, Energy2 pt2max);

  /**
   * Generate boson gluon fusion. Saves xi, z and the quark mass in GeV
   * in genVar.
   *
   * @param pt2min the minimum transverse momentum squared allowed.
   *
   * @param pt2max the minimum transverse momentum squared allowed.
   */
  void genBGF(Energy2 pt2min, Energy2 pt2max);

  /**
   * Perform an initial state quark emission. This contains the common
   * parts of performSeaQ and performQtoGQ. This is an extended version
   * that takes into account hard remnants.
   */
  virtual tParPtr performInitialQ();

  /**
   * Calculate y = P.q/P.l, where q, P and l are the momentum of the
   * virtual photon, incoming proton and incoming lepton respectivly.
   */
  double y();

public:

  /** @name Simple access functions. */
  //@{
  /**
   * The hard remnant.
   */
  inline tHardRemPtr hardRem() const;

  /**
   * The soft remnant.
   */
  inline tSoftRemPtr softRem() const;

  /**
   * Set the hard remnant.
   */
  inline void hardRem(tHardRemPtr);

  /**
   * Set the soft remnant.
   */
  inline void softRem(tSoftRemPtr);
  //@}

private:

  /**
   * The hard remnant.
   */
  tHardRemPtr theHardRem;

  /**
   * The soft remnant.
   */
  tSoftRemPtr theSoftRem;

  /**
   * The cached y value.
   */
  double theY;

protected:

  /** @name Functions relating to the DipoleState to which this belongs. */
  //@{
  /**
   * Return a simple clone of this object. Should be implemented as
   * <code>return new_ptr(*this);</code> by a derived class.
   */
  virtual ClonePtr clone() const;

  /**
   * Fill the provided set with all pointers to CloneBase objects used
   * in this object.
   */
  virtual void fillReferences(CloneSet &) const;

  /**
   * Rebind pointers to other CloneBase objects. Called after a number
   * of interconnected CloneBase objects have been cloned, so that
   * the cloned objects will refer to the cloned copies afterwards.
   *
   * @param trans a TranslationMap relating the original objects to
   * their respective clones.
   */
  virtual void rebind(const TranslationMap & trans);
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

public:

  /**
   * Print out debugging information on std::cerr.
   */
  virtual void debugme() const;

private:

  /**
   * The static object used to initialize the description of this class.
   * Indicates that this is a concrete class with persistent data.
   */
  static ClassDescription<DISDipole> initDISDipole;

  /**
   * The assignment operator is private and must never be called.
   * In fact, it should not even be implemented.
   */
  DISDipole & operator=(const DISDipole &);

};

}

#include "ThePEG/Utilities/ClassTraits.h"

namespace ThePEG {

/** @cond TRAITSPECIALIZATIONS */

/** This template specialization informs ThePEG about the
 *  base classes of DISDipole. */
template <>
struct BaseClassTrait<Ariadne::DISDipole,1> {
  /** Typedef of the first base class of DISDipole. */
  typedef Ariadne::ExtendedDipole NthBase;
};

/** This template specialization informs ThePEG about the name of
 *  the DISDipole class and the shared object where it is defined. */
template <>
struct ClassTraits<Ariadne::DISDipole>
  : public ClassTraitsBase<Ariadne::DISDipole> {
  /** Return a platform-independent class name */
  static string className() { return "Ariadne::DISDipole"; }
  /**
   * The name of a file containing the dynamic library where the class
   * DISDipole is implemented. It may also include several, space-separated,
   * libraries if the class DISDipole depends on other classes (base classes
   * excepted). In this case the listed libraries will be dynamically
   * linked in the order they are specified.
   */
  static string library() { return "libArCascade.so"; }
};

/** @endcond */

}

#include "DISDipole.icc"
#ifndef ThePEG_TEMPLATES_IN_CC_FILE
// #include "DISDipole.tcc"
#endif

#endif /* ARIADNE_DISDipole_H */
