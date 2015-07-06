// -*- C++ -*-
#ifndef ARIADNE5_DYME_H
#define ARIADNE5_DYME_H
//
// This is the declaration of the DYME class.
//

#include "Ariadne/Cascade/EmitterBase.h"
#include "RRGluonEmission.h"
#include "ISGtoQEmitter.h"

namespace Ariadne5 {

using namespace ThePEG;

/**
 * The DYME class implements the emission of gluons and initial-state
 * splitting of gluons into quarks according to the leading-order
 * tree-level matrix element for deeply inelastic, neutral-current,
 * lepton-proton scattering.
 *
 * @see \ref DYMEInterfaces "The interfaces"
 * defined for DYME.
 */
class DYME: public ISGtoQEmitter {

public:

  /** @name Standard constructors and destructors. */
  //@{
  /**
   * The default constructor.
   */
  DYME();

  /**
   * The destructor.
   */
  virtual ~DYME();
  //@}

  /** @name Virtual functions to be overridden by sub-classes. */
  //@{
  /**
   * Return true if and only if this model can handle the given
   * Emitter.
   */
  virtual bool canHandle(const DipoleBase &) const;

  /**
   * If the given EmissionModel overlaps with this model for the given
   * Emitter, return true if this model should take precedence. Must
   * only be called for Emitters for which canHandle() is true.
   */
  virtual bool overrides(const EmitterBase &, DipoleBase &) const;

  /**
   * Generate the a phase space point for an emission corresponding to
   * this model. Must only be called for Emitters for which
   * canHandle() is true.
   */
  virtual EmPtr generate(const DipoleBase &, Energy rhomin, Energy rhomax) const;

  /**
   * Perform an emission previously generated for this Emitter. Must
   * only be called for Emitters for which canHandle() is true.
   * @return true if the emission was successful
   */
  virtual bool perform(const Emission &) const;

  /**
   * Reverse a previously performed emission. Sub-classes which has
   * signalled that they can revert an emission but fails to do so,
   * must throw a Exception::runerror.
   */
  virtual void revert(const Emission & emission) const;
  //@}

public:

  /**
   * Generate the a phase space point for a gluon emission.
   */
  virtual EmPtr
  generateG(tRemParPtr rsoft, tRemParPtr rhard, const QCDDipole & d,
	    Energy rhomin, Energy rhomax) const;

  /**
   * We don't have to implement the whole generation for quark
   * emission, only (p)reweight the base class method.
   */
  virtual double reweight(const Emission & emission) const;

  /**
   * We don't have to implement the whole generation for quark
   * emission, only (p)reweight the base class method.
   */
  virtual double preweight(const Emission & emission) const;

public:

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
  virtual IBPtr clone() const;

  /** Make a clone of this object, possibly modifying the cloned object
   * to make it sane.
   * @return a pointer to the new object.
   */
  virtual IBPtr fullclone() const;
  //@}


// If needed, insert declarations of virtual function defined in the
// InterfacedBase class here (using ThePEG-interfaced-decl in Emacs).


private:

  /**
   * The assignment operator is private and must never be called.
   * In fact, it should not even be implemented.
   */
  DYME & operator=(const DYME &);

};

}

#endif /* ARIADNE5_DYME_H */
