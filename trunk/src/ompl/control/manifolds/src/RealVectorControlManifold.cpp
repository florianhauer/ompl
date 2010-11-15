/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2010, Rice University
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Rice University nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Ioan Sucan */

#include "ompl/control/manifolds/RealVectorControlManifold.h"
#include "ompl/util/Exception.h"
#include <cstring>
#include <limits>

void ompl::control::RealVectorControlUniformSampler::sample(Control *control)
{
    const unsigned int dim = manifold_->getDimension();
    const base::RealVectorBounds &bounds = static_cast<const RealVectorControlManifold*>(manifold_)->getBounds();
    
    RealVectorControlManifold::ControlType *rcontrol = static_cast<RealVectorControlManifold::ControlType*>(control);
    for (unsigned int i = 0 ; i < dim ; ++i)
	rcontrol->values[i] = rng_.uniformReal(bounds.low[i], bounds.high[i]);
}

void ompl::control::RealVectorControlManifold::setup(void)
{
    ControlManifold::setup();
    bounds_.check();
}

void ompl::control::RealVectorControlManifold::setBounds(const base::RealVectorBounds &bounds)
{
    bounds.check();
    if (bounds.low.size() != dimension_)
	throw Exception("Bounds do not match dimension of manifold");
    bounds_ = bounds;
}

unsigned int ompl::control::RealVectorControlManifold::getDimension(void) const
{
    return dimension_;
}

void ompl::control::RealVectorControlManifold::copyControl(Control *destination, const Control *source) const
{
    memcpy(static_cast<ControlType*>(destination)->values,
	   static_cast<const ControlType*>(source)->values, controlBytes_);    
}

bool ompl::control::RealVectorControlManifold::equalControls(const Control *control1, const Control *control2) const
{
    const double *s1 = static_cast<const ControlType*>(control1)->values;
    const double *s2 = static_cast<const ControlType*>(control2)->values;
    for (unsigned int i = 0 ; i < dimension_ ; ++i)
    {	 
	double diff = (*s1++) - (*s2++);
	if (fabs(diff) > std::numeric_limits<double>::epsilon())
	    return false;
    }
    return true;
}

ompl::control::ControlSamplerPtr ompl::control::RealVectorControlManifold::allocControlSampler(void) const 
{
    return ControlSamplerPtr(new RealVectorControlUniformSampler(this));
}

ompl::control::Control* ompl::control::RealVectorControlManifold::allocControl(void) const
{
    ControlType *rcontrol = new ControlType();
    rcontrol->values = new double[dimension_];
    return rcontrol;
}

void ompl::control::RealVectorControlManifold::freeControl(Control *control) const
{
    ControlType *rcontrol = static_cast<ControlType*>(control);
    delete[] rcontrol->values;
    delete rcontrol;
}

void ompl::control::RealVectorControlManifold::nullControl(Control *control) const
{
    ControlType *rcontrol = static_cast<ControlType*>(control);
    for (unsigned int i = 0 ; i < dimension_ ; ++i)
	rcontrol->values[i] = 0.0;
}

void ompl::control::RealVectorControlManifold::printControl(const Control *control, std::ostream &out) const
{
    out << "RealVectorControl [";
    if (control)
    {
	const ControlType *rcontrol = static_cast<const ControlType*>(control);
	for (unsigned int i = 0 ; i < dimension_ ; ++i)
	{
	    out << rcontrol->values[i];
	    if (i + 1 < dimension_) 
		out << ' ';
	}
    }
    else
	out << "NULL";
    out << ']' << std::endl;
}

void ompl::control::RealVectorControlManifold::printSettings(std::ostream &out) const
{
    out << "Real vector control manifold '" << name_ << "' with bounds: " << std::endl;
    out << "  - min: ";
    for (unsigned int i = 0 ; i < dimension_ ; ++i)
	out << bounds_.low[i] << " ";
    out << std::endl;    
    out << "  - max: ";
    for (unsigned int i = 0 ; i < dimension_ ; ++i)
	out << bounds_.high[i] << " ";
    out << std::endl;
}
