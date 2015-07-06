// -*- C++ -*-
#ifndef ARIADNE_Dipole_H
#define ARIADNE_Dipole_H
//
// This is the declaration of the Dipole class.
//

#include "Ariadne/DipoleCascade/Emitter.h"
#include "Dipole.fh"
#include "Parton.fh"
#include "DipoleState.fh"

namespace Ariadne {

using namespace ThePEG;

/**
 * The Dipole class represents QCD dipoles between
 * <code>Parton</code>s and is able to perform gluon emissions or
 * gluon splittings into \f$q\\bar{q}\f$ pairs according to the Dipole
 * Cascade Model. The Dipole also defines the colour flow from the
 * incoming to the outgoing Parton, where the incoming Parton carries
 * anti-colour and the outgoing Parton carries colour.
 */
class Dipole: public Emitter {

public:

  /**
   * The DipoleState is a friend.
   */
  friend class DipoleState;

public:

  /** @name Standard constructors and destructors. */
  //@{
  /**
   * The default constructor.
   */
  inline Dipole();

  /**
   * The copy constructor.
   */
  inline Dipole(const Dipole &);

  /**
   * The destructor.
   */
  inline virtual ~Dipole();
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

public:

  /** @name Functions used to construct a history.*/
  //@{
  /**
   * Construct the previous state in the cascade associated
   * with the emitter. This will remove one dipole and set
   * the values in the current dipole corresponding to the
   * emission needed to produce the state before
   * the function was called. In the modified dipole set x1, x3,
   * flav, qmass, and pT2.
   * @returns a vector of possible reconstructions associated with
   * the emitter
   */
  virtual DipoleStateVector constructStep();

  /**
   * Returns the emission probability of the emission that was undone
   * in the constructStep method.
   */
  virtual double emissionProbability();

  /**
   * Returns the value of the alphaS at the scale of the undone
   * emission.
   */
  virtual double coupling();
  //@}

protected:
  /** @name Functions used to undo emissions. */
  //@{
  /**
   * Undo a gluon emission with the gluon in OPart.
   * Avoind double counting by only including OPart
   */
  bool undoOG();

  /**
   * Merges q1 and q2 into a gluon and returns the gluon.
   * q1 and q2 are removed from dipole state.
   */
  bool undoQ(tParPtr q2);
  //@}

protected:

  /**
   * Reset this dipole preparing for generating a new emission below
   * the scale \a pt2max.
   */
  virtual void reset(Energy2 pt2max);

  /**
   * Generate a gluon emission. Saves x1 and x3 in genVar.
   *
   * @param pt2min the minimum transverse momentum squared allowed.
   *
   * @param pt2max the minimum transverse momentum squared allowed.
   */
  virtual void genG(Energy2 pt2min, Energy2 pt2max);

  /**
   * Generate a \f$g\rightarrow q\bar{q}\f$ emission. Saves x1, x3 and
   * the quark mass in GeV in genVar.
   *
   * @param pt2min the minimum transverse momentum squared allowed.
   *
   * @param pt2max the minimum transverse momentum squared allowed.
   */
  virtual void genQ(Energy2 pt2min, Energy2 pt2max);

  /**
   * Perform an emission. Use the information previously generated by
   * genG(Energy2,Energy2) to perform the corresponding emission.
   *
   * @return the emitted gluon or null if the emission failed.
   */
  virtual tParPtr performG();

  /**
   * Perform an emission. Use the information previously generated by
   * genQ(Energy2,Energy2) to perform the corresponding emission.
   *
   * @return the quark which was assumed to have been emitted or null
   * if the emission failed.
   */
  virtual tParPtr performQ();

  /**
   * Create a gluon emitted from this Dipole and set its momentum to
   * \a pg. Also create the new dipole and connect them all together.
   */
  virtual ParPtr emitGluon(const Lorentz5Momentum & pg);

  /**
   * Create a \f$q\bar{q}\f$ pair by splitting a gluon in this Dipole
   * and set their momentum to \a p2 and \a p3. Also split the
   * corresponding string into two.
   */
  tParPtr splitGluon(const Lorentz5Momentum & p2,
		     const Lorentz5Momentum & p3);

public:

  /** @name Simple access functions. */
  //@{
  /**
   * The anti-colour-carrying parton in this dipole.
   */
  inline tParPtr iPart() const;

  /**
   * The previous dipole in the string. Returns iPart()->iDip().
   */
  tDipPtr prev() const;

  /**
   * The colour-carrying parton in this dipole.
   */
  inline tParPtr oPart() const;

  /**
   * The next dipole in the string. Returns oPart()->oDip().
   */
  tDipPtr next() const;

  /**
   * The colour index of this dipole. 0 means no index has been assigned.
   */
  inline int colourIndex() const;

  /**
   * Set the anti-colour-carrying parton in this dipole.
   */
  inline void iPart(tParPtr);

  /**
   * Set the colour-carrying parton in this dipole.
   */
  inline void oPart(tParPtr);

  /**
   * Set the colour index of this dipole. 0 means no index has been assigned.
   */
  inline void colourIndex(int);

  /**
   * Generte a colour index of this dipole with the given string
   * index.
   */
  void generateColourIndex();

  /**
   * Return the squared invariant mass of this dipole.
   */
  Energy2 sdip() const;

  /**
   * The particle type if this dipole comes directly from a decay of a
   * resonance. If not, null is returned.
   */
  inline tcPDPtr resonance() const;

  /**
   * The particle type if this dipole comes directly from a decay of a
   * resonance.
   */
  inline void resonance(tcPDPtr);
  //@}

public:

  /**
   * A class representing the normal gluon emission type.
   */
  struct FSGluon: public EmissionType {};

  /**
   * A class representing emissions of the type final state splitting
   * of gluons into quak-antiquark pair.
   */
  struct FSGtoQQ: public EmissionType {};

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

private:

  /**
   * The anti-colour-carrying parton in this dipole.
   */
  tParPtr theIPart;

  /**
   * The colour-carrying parton in this dipole.
   */
  tParPtr theOPart;

  /**
   * The colour index of this dipole. 0 means no index has been assigned.
   */
  int theColourIndex;

  /**
   * The particle type if this dipole comes directly from a decay of a
   * resonance.
   */
  tcPDPtr theResonance;

  /**
   * Factor separating the bare colour index ci from the string index
   * si.
   */
  static const int div = 1000;

protected:

  /**
   * The flavour of the last generated emission. 0 means no emission
   * has been generated, 21 means gluon, positive means the iPart()
   * has been split into the corresponding quark type, and negative
   * means that the oPart() has been split into the corresponding
   * quark type.
   */
  int flav;

public:

  /**
   * Print out debugging information on std::cerr.
   */
  virtual void debugme() const;

  /**
   * Check integrety of the emitter. Return false if error is found.
   */
  virtual bool checkIntegrety();

private:

  /**
   * The static object used to initialize the description of this class.
   * Indicates that this is a concrete class with persistent data.
   */
  static ClassDescription<Dipole> initDipole;

  /**
   * The assignment operator is private and must never be called.
   * In fact, it should not even be implemented.
   */
  Dipole & operator=(const Dipole &);

};

}

#include "ThePEG/Utilities/ClassTraits.h"

namespace ThePEG {

/** @cond TRAITSPECIALIZATIONS */

/** This template specialization informs ThePEG about the
 *  base classes of Dipole. */
template <>
struct BaseClassTrait<Ariadne::Dipole,1> {
  /** Typedef of the first base class of Dipole. */
  typedef Ariadne::Emitter NthBase;
};

/** This template specialization informs ThePEG about the name of
 *  the Dipole class and the shared object where it is defined. */
template <>
struct ClassTraits<Ariadne::Dipole>
  : public ClassTraitsBase<Ariadne::Dipole> {
  /** Return a platform-independent class name */
  static string className() { return "Ariadne::Dipole"; }
  /** Return the name of the shared library be loaded to get
   *  access to the Dipole class and every other class it uses
   *  (except the base class). */
  static string library() { return "libArCascade.so"; }
};

/** @endcond */

}

#include "Dipole.icc"
#ifndef ThePEG_TEMPLATES_IN_CC_FILE
// #include "Dipole.tcc"
#endif

#endif /* ARIADNE_Dipole_H */