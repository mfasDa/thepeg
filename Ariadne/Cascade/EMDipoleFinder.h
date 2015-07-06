// -*- C++ -*-
#ifndef Ariadne5_EMDipoleFinder_H
#define Ariadne5_EMDipoleFinder_H
//
// This is the declaration of the EMDipoleFinder class.
//

#include "ThePEG/Handlers/HandlerBase.h"
#include "EMDipoleFinder.fh"
#include "EMDipole.fh"
#include "DipoleState.fh"

namespace Ariadne5 {

using namespace ThePEG;

/**
 * The EMDipoleFinder class and its sub-classes are responsible for
 * identifying and introducing of electro-magnetic dipoles in the
 * setup phase of a given DipoleState.
 *
 * @see \ref EMDipoleFinderInterfaces "The interfaces"
 * defined for EMDipoleFinder.
 */
class EMDipoleFinder: public HandlerBase {

public:

  /** @name Standard constructors and destructors. */
  //@{
  /**
   * The default constructor.
   */
  EMDipoleFinder();

  /**
   * The destructor.
   */
  virtual ~EMDipoleFinder();
  //@}

public:

  /**
   * Find, create and return the electro-magnetic dipoles in the given
   * DipoleState.
   */
  virtual vector<tEMDipPtr> findDipoles(DipoleState &) const;

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
  EMDipoleFinder & operator=(const EMDipoleFinder &);

};

}

#endif /* Ariadne5_EMDipoleFinder_H */