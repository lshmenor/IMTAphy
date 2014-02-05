/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef DLL_TESTS_FUTESTBASE_HPP
#define DLL_TESTS_FUTESTBASE_HPP

#include <DLL/Layer2.hpp>
#include <DLL/StationManager.hpp>
#include <DLL/UpperConvergence.hpp> 
#include <WNS/ldk/fun/Main.hpp>
#include <WNS/ldk/tools/Stub.hpp>
#include <WNS/node/tests/Stub.hpp>
#include <WNS/CppUnit.hpp>

namespace dll { namespace tests {

  /**
   * @brief Base class for dll::FU tests.
   *
   * This class sets up a layer, fun, and two stubs.
   * Deriving classes set up the FU chain they wish to
   * test, which is then placed between the two stubs.
   */
  class FUTestBase :
    public wns::TestFixture
  {
  public:

    FUTestBase();

    virtual
    ~FUTestBase();

  protected:

    /**
     * @brief Returns a Compound carrying a FakePDU
     */
    wns::ldk::CompoundPtr newFakeCompound();

    /**
     * @brief Sends a compound through the upper stub.
     *
     * @return Last compound sent by the lower stub.
     */
    wns::ldk::CompoundPtr sendCompound(const wns::ldk::CompoundPtr& compound);

    /**
     * @brief Let the lower stub receive a compound.
     *
     * @return Last compound received by the upper stub.
     */
    wns::ldk::CompoundPtr receiveCompound(const wns::ldk::CompoundPtr& compound);

    /**
     * @brief Number of calls to sendCompound.
     */
    unsigned int compoundsAccepted() const;

    /**
     * @brief Number of compounds sent by the lower stub.
     */
    unsigned int compoundsSent() const;

    /**
     * @brief Number of calls to receiveCompound.
     */
    unsigned int compoundsReceived() const;

    /**
     * @brief Number of compounds received by the upper stub.
     */
    unsigned int compoundsDelivered() const;

    /**
     * @brief Return the FUN.
     */
    wns::ldk::fun::FUN* getFUN();

    /**
     * @brief Return the lower stub.
     */
    wns::ldk::tools::StubBase* getLowerStub() const;

    /**
     * @brief Return the upper stub.
     */
    wns::ldk::tools::StubBase* getUpperStub() const;


    virtual void prepare();
    virtual void cleanup();

    /**
     * @brief Set up the FU chain to test.
     *
     * This method sets up the FUs which should be tested,
     * adds them to the FUN and connects them. They must
     * not be connected to the stubs.
     */
    virtual void setUpTestFUs() = 0;

    /**
     * @brief Tear down the FU chain to test.
     *
     * @note Note that FUs that have been added to a FUN don't
     * need to be deleted.
     */
    virtual void tearDownTestFUs() = 0;

  private:
    /**
     * @brief Returns the FU that should be connected to the upper stub.
     */
    virtual wns::ldk::FunctionalUnit* getUpperTestFU() const = 0;

    /**
     * @brief Returns the FU that should be connected to the lower stub.
     */
    virtual wns::ldk::FunctionalUnit* getLowerTestFU() const = 0;

    /**
     * @brief Return a new instance of the upper stub.
     */
    virtual wns::ldk::tools::StubBase* newUpperStub();

    /**
     * @brief Return a new instance of the lower stub.
     */
    virtual wns::ldk::tools::StubBase* newLowerStub();

    //Needed for the configuration of dll::Layer2
    virtual std::string getStationType() const = 0;
    virtual std::string getStationID() const = 0;
    virtual std::string getRing() const = 0;
    virtual std::string getUpperConvergenceName() const = 0;

    //Dependencies to other entities
    dll::StationManager* stationManager;
    dll::NoUpperConvergence* upperConvergence;
    wns::node::tests::Stub* node;
    dll::ILayer2* layer;
    wns::ldk::fun::Main* fun;

    wns::ldk::tools::StubBase* upperStub;
    wns::ldk::tools::StubBase* lowerStub;

    unsigned int accepted;
    unsigned int received;
  };

}//tests
}//dll

#endif // WNS_LDK_TESTS_FUTESTBASE_HPP
