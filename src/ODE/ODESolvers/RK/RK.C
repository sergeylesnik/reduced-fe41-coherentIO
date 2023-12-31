/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | foam-extend: Open Source CFD
   \\    /   O peration     | Version:     4.1
    \\  /    A nd           | Web:         http://www.foam-extend.org
     \\/     M anipulation  | For copyright notice see file Copyright
-------------------------------------------------------------------------------
License
    This file is part of foam-extend.

    foam-extend is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    foam-extend is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Description
    Fifth-order Cash-Karp embedded Runge-Kutta scheme with error control and
    adjustive time-step size
    Numerical Recipes in C, Secn 16.2 page 714-722
    Alternative reference in Numerical Recipes in C++

\*---------------------------------------------------------------------------*/

#include "RK.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(Foam::RK, 0);

namespace Foam
{
    addToRunTimeSelectionTable(ODESolver, RK, ODE);

const scalar
    RK::safety=0.9, RK::pGrow=-0.2, RK::pShrink=-0.25, RK::errCon=1.89e-4;

const scalar
    RK::a2 = 0.2, RK::a3 = 0.3, RK::a4 = 0.6, RK::a5 = 1.0, RK::a6 = 0.875,
    RK::b21 = 0.2, RK::b31 = 3.0/40.0, RK::b32 = 9.0/40.0,
    RK::b41 = 0.3, RK::b42 = -0.9, RK::b43 = 1.2,
    RK::b51 = -11.0/54.0, RK::b52 = 2.5, RK::b53 = -70.0/27.0,
    RK::b54 = 35.0/27.0,
    RK::b61 = 1631.0/55296.0, RK::b62 = 175.0/512.0, RK::b63 = 575.0/13824.0,
    RK::b64 = 44275.0/110592.0, RK::b65 = 253.0/4096.0,
    RK::c1 = 37.0/378.0, RK::c3 = 250.0/621.0,
    RK::c4 = 125.0/594.0, RK::c6 = 512.0/1771.0,
    RK::dc1 = RK::c1 - 2825.0/27648.0, RK::dc3 = RK::c3 - 18575.0/48384.0,
    RK::dc4 = RK::c4 - 13525.0/55296.0, RK::dc5 = -277.00/14336.0,
    RK::dc6 = RK::c6 - 0.25;
};


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::RK::RK(ODE& ode)
:
    ODESolver(ode),
    yTemp_(ode.nEqns()),
    ak2_(ode.nEqns()),
    ak3_(ode.nEqns()),
    ak4_(ode.nEqns()),
    ak5_(ode.nEqns()),
    ak6_(ode.nEqns()),
    yErr_(ode.nEqns()),
    yTemp2_(ode.nEqns())
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::RK::solve
(
    const scalar x,
    const scalarField& y,
    const scalarField& dydx,
    const scalar h,
    scalarField& yout,
    scalarField& yerr
) const
{
    forAll(yTemp_, i)
    {
        yTemp_[i] = y[i] + b21*h*dydx[i];
    }

    ode_.derivatives(x + a2*h, yTemp_, ak2_);

    forAll(yTemp_, i)
    {
        yTemp_[i] = y[i] + h*(b31*dydx[i] + b32*ak2_[i]);
    }

    ode_.derivatives(x + a3*h, yTemp_, ak3_);

    forAll(yTemp_, i)
    {
        yTemp_[i] = y[i] + h*(b41*dydx[i] + b42*ak2_[i] + b43*ak3_[i]);
    }

    ode_.derivatives(x + a4*h, yTemp_, ak4_);

    forAll(yTemp_, i)
    {
        yTemp_[i] = y[i]
          + h*(b51*dydx[i] + b52*ak2_[i] + b53*ak3_[i] + b54*ak4_[i]);
    }

    ode_.derivatives(x + a5*h, yTemp_, ak5_);

    forAll(yTemp_, i)
    {
        yTemp_[i] = y[i]
          + h*
            (
                b61*dydx[i] + b62*ak2_[i] + b63*ak3_[i]
              + b64*ak4_[i] + b65*ak5_[i]
            );
    }

    ode_.derivatives(x + a6*h, yTemp_, ak6_);

    forAll(yout, i)
    {
        yout[i] = y[i]
          + h*(c1*dydx[i] + c3*ak3_[i] + c4*ak4_[i] + c6*ak6_[i]);
    }

    forAll(yerr, i)
    {
        yerr[i] =
            h*
            (
                dc1*dydx[i] + dc3*ak3_[i] + dc4*ak4_[i]
              + dc5*ak5_[i] + dc6*ak6_[i]
            );
    }
}


void Foam::RK::solve
(
    scalar& x,
    scalarField& y,
    scalarField& dydx,
    const scalar eps,
    const scalarField& yScale,
    const scalar hTry,
    scalar& hDid,
    scalar& hNext
) const
{
    scalar h = hTry;
    scalar maxErr = 0.0;

    for (;;)
    {
        solve(x, y, dydx, h, yTemp2_, yErr_);

        maxErr = 0.0;
        for (label i=0; i<ode_.nEqns(); i++)
        {
            maxErr = max(maxErr, mag(yErr_[i]/yScale[i]));
        }
        maxErr /= eps;

        if (maxErr <= 1.0)
        {
            break;
        }

        {
            scalar hTemp = safety*h*pow(maxErr, pShrink);
            h = (h >= 0.0 ? max(hTemp, 0.1*h) : min(hTemp, 0.1*h));
        }

        if (h < VSMALL)
        {
            FatalErrorIn("RK::solve")
                << "stepsize underflow"
                << exit(FatalError);
        }
    }

    hDid = h;

    x += h;
    y = yTemp2_;

    if (maxErr > errCon)
    {
        hNext = safety*h*pow(maxErr, pGrow);
    }
    else
    {
        hNext = 5.0*h;
    }
}


// ************************************************************************* //
