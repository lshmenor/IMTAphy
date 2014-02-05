/******************************************************************************
 * Glue                                                                       *
 * __________________________________________________________________________ *
 *                                                                            *
 * Copyright (C) 2005-2006                                                    *
 * Lehrstuhl fuer Kommunikationsnetze (ComNets)                               *
 * Kopernikusstr. 16, D-52074 Aachen, Germany                                 *
 * phone: ++49-241-80-27910 (phone), fax: ++49-241-80-22242                   *
 * email: wns@comnets.rwth-aachen.de                                          *
 * www: http://wns.comnets.rwth-aachen.de                                     *
 ******************************************************************************/

#include <GLUE/Glue.hpp>

#include <WNS/pyconfig/View.hpp>

using namespace glue;

STATIC_FACTORY_REGISTER_WITH_CREATOR(
	Glue,
	wns::module::Base,
	"Glue",
	wns::PyConfigViewCreator);

Glue::Glue(const wns::pyconfig::View& _pyConfigView) :
		wns::module::Module<Glue>(_pyConfigView)
{
}

Glue::~Glue()
{
}

void
Glue::configure()
{
} // configure


void
Glue::startUp()
{
} // startUp


void
Glue::shutDown()
{
} // shutDown



