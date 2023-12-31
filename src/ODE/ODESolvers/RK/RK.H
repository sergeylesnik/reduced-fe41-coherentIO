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

Class
    Foam::RK

Description
    Fifth-order Cash-Karp embedded Runge-Kutta scheme with error control and
    adjustive time-step size
    Numerical Recipes in C, Secn 16.2 page 714-722
    Alternative reference in Numerical Recipes in C++

SourceFiles
    RK.C

\*---------------------------------------------------------------------------*/

#ifndef RK_H
#define RK_H

#include "ODESolver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                                Class RK Declaration
\*---------------------------------------------------------------------------*/

class RK
:
    public ODESolver
{
    // Private data

        mutable scalarField yTemp_;
        mutable scalarField ak2_;
        mutable scalarField ak3_;
        mutable scalarField ak4_;
        mutable scalarField ak5_;
        mutable scalarField ak6_;

        mutable scalarField yErr_;
        mutable scalarField yTemp2_;

        static const scalar safety, pGrow, pShrink, errCon;

        static const scalar
            a2, a3, a4, a5, a6,
            b21, b31, b32, b41, b42, b43,
            b51, b52, b53, b54, b61, b62, b63, b64, b65,
            c1, c3, c4, c6,
            dc1, dc3, dc4, dc5, dc6;


    // Private Member Functions

        void solve
        (
            const scalar x,
            const scalarField& y,
            const scalarField& dydx,
            const scalar h,
            scalarField& yout,
            scalarField& yerr
        ) const;


public:

    //- Runtime type information
    TypeName("RK");


    // Constructors

        //- Construct from ODE
        RK(ODE& ode);


    // Destructor

        virtual ~RK()
        {}


    // Member Functions

        virtual void solve
        (
            scalar& x,
            scalarField& y,
            scalarField& dydx,
            const scalar eps,
            const scalarField& yScale,
            const scalar hTry,
            scalar& hDid,
            scalar& hNext
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
