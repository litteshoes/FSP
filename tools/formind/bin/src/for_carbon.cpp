////////////////////////////////////////////////////////////////////
//
// FORMIND – the forest model
// Contact: info@formind.org
// http://www.formind.org/
//
// Author: FORMIND model developer team
// Maintainer: FORMIND model developer team
// Copyright: Helmholtz Centre for Environmental Research - UFZ
//            and FORMIND model developer team.
// License: AGPL (>= 3)
//
///////////////////////////////////////////////////////////////////
//
// File						for_carbon.cpp
// Description				calculates carbon fluxes on plot level
//
///////////////////////////////////////////////////////////////////

#include "for_global.h"
#include "for_carbon.h"
#include "for_var.h"
#include <exception>

#ifdef underconstruction
#include "for_log.h"
#include "for_landslide.h"
#include "for_fire.h"
#include "grass/grass_dead.h"
#include "for_water.h"
#endif

/* !
 \brief		calculates carbon fluxes on plot level
 \details 	This module only consideres carbon fluxes which are NOT
 connected to forest growth. (The latter you will find in
 for_grow.cpp)
 \return    boolean true if no error occures
 */

bool CalculateCarbonFlux(void) {
	try {
		PlotPointer plot;
		HecPointer hec;

		if (!((N_Par.TreeListInput == 1 || N_Par.InitPools == 1) && (T.T == 0.0)))
		{

			hec = FirstHec;
			while (hec != NULL) {
				plot = hec->FirstPlot;
				// set plot cflux to zero for initial state of each time step.
				plot->C_flux = 0.0;
				while (plot != NULL) {
					// if climate module on: calculate evapotranspiration & decomposition
					// rate based on current climate conditions
					// if climate module off: actual evapotransporation is taken from
					// parameter file
#ifdef underconstruction
					if (N_Par.Water_ON)
						CalculateDecompRate(plot);
#endif
					// calculate overall decomposition rate
					// Sato (2007) equation 41
					plot->decomp_rate =
						 std::min(1.0, (pow(10.0, -1.4553 + 0.0014175 * plot->aet))
						 / 12.0);

					// calculate respiration for two soil pools (fast and slow)
					// Respirations (Sato, 2007)
					plot->Resp_DeadWood =
						 0.7 * plot->decomp_rate * plot->CPool_DeadWood;
					// [tC / T.D]
					plot->Resp_Soil_fast = plot->CPool_Soil_fast * (1.0 / 15.0);
					// [tC / T.D], see (Sato, 2007) for the factors 1/15 and 1/750
					plot->Resp_Soil_slow = plot->CPool_Soil_slow * (1.0 / 750.0);
					// [tC / T.D]

					// Carbon Fluxes
#ifdef underconstruction
					// if grassmind is on, seperate calculations of carbon fluxes:
					if (N_Par.GRASSMIND) {
						CalculateCarbonGrassFlux(plot);
					}
					else
#endif
					{
						// if grassmind off, normal case:
						plot->Cflux_to_DeadWood =
							 (plot->MB + plot->MBC + plot->MBF + plot->MBD) * ODM_TO_C;
						// [tC/yr * T.D]
					}

#ifdef underconstruction
					if ((Logging.DoIt)) {
						CalculateLoggingCFlux(plot);
					}
#endif

					// calculating flux rates deadwood to soil pools (fast and slow
					// decomposing; Sato, 2007)
					plot->Cflux_to_Soil_fast =
						 plot->CPool_DeadWood * 0.3 * plot->decomp_rate * 0.985;
					// [tC / T.D]
					plot->Cflux_to_Soil_slow =
						 plot->CPool_DeadWood * 0.3 * plot->decomp_rate * 0.015;
					// [tC / T.D]

					// calculating Carbon Pools
					// ---------------
					// Dead wood C-pool is the sum of C-flux to dead wood from living biomass
					// minus fast and slow C-fluxes and minus dead wood respiration
					plot->CPool_DeadWood +=
						 (plot->Cflux_to_DeadWood - plot->Cflux_to_Soil_fast * T.D -
						 plot->Cflux_to_Soil_slow * T.D - plot->Resp_DeadWood * T.D);
					// [tC] per patch

					// fast C-pool of soil is fast C-flux to soil from dead wood minus
					// fast soil respiration
					plot->CPool_Soil_fast +=
						 (plot->Cflux_to_Soil_fast - plot->Resp_Soil_fast) * T.D;
					// [tC] per patch

					// slow C-pool of soil is slow C-flux to soil from dead wood minus
					// slow soil respiration
					plot->CPool_Soil_slow +=
						 (plot->Cflux_to_Soil_slow - plot->Resp_Soil_slow) * T.D;
					// [tC] per patch

					// calculating total Carbo-Fluxes (respiration and net ecosystem
					// exchange
					// ---------------
					// Total respiration of living and dead biomass as well as fast
					// and slow soil respirations
					plot->R_total =
						 ((ODM_TO_C * plot->R_total_biomass) + plot->Resp_DeadWood +
						 plot->Resp_Soil_fast + plot->Resp_Soil_slow);
					// [tC/T.D] per patch

					// Net ecosystem exchange is gross primary production minus total
					// respiration (see above)
					plot->C_flux =
						 ((ODM_TO_C * (plot->PB + plot->BiomassNewTrees)) -
						 plot->R_total);
					// NEE = GPP - totalRespiration [tC/T.D] per patch

#ifdef underconstruction
					// if fire-module is on, additional carbon is emitted
					if (N_Par.Fire) {
						CalculateFireCFlux(plot);
					}

					// if landslide-module is on, additional carbon is emitted
					if (N_Par.Landslide) {
						CalculateLandslideCFlux(plot);
					}

					// if logging-module is on, additional carbon is emitted
					if ((Logging.DoIt)) {

						CalculateLoggingEmmissions(plot);
					}
#endif
					plot = plot->next;
				} // end plot loop

				hec = hec->next;
			} // end hec loop
			return true;
		}
}
catch (std::exception& e) {
	std::cerr << "ERROR: undefined error \tfile: " << __FILE__ <<
		 "\tfunction: CalculateCarbonFlux\t line:" << __LINE__ <<
		 "\t undefined error.\n Standard exception: " << e.what() << std::endl;
	return false;
}
return true;
}

// -----------------------------------------------------------------------------
/* !
 \brief		Initiate carbon Pools and fluxes on plot level.
 \details	Fluxes are set to 0, Pools are set to the values of N_Par.CPool_DeadWood,
 N_Par.CPool_Soil_fast, N_Par.CPool_Soil_slow in the PAR-file.
 \return    boolean true if no error occures
 */
void InitCarbonPlot(PlotPointer plot) {

if (!N_Par.Century_ON) {

	plot->CPool_DeadWood = N_Par.CPool_DeadWood / Switch.Maxplot;
	plot->CPool_Soil_fast = N_Par.CPool_Soil_fast / Switch.Maxplot;
	plot->CPool_Soil_slow = N_Par.CPool_Soil_slow / Switch.Maxplot;

	plot->Cflux_to_DeadWood = 0;
	plot->Cflux_to_Soil_fast = 0;
	plot->Cflux_to_Soil_slow = 0;
	plot->aet = N_Par.Cflux_aet;
	// aet = actual annual evapotranspiration in [mm]  if climate module is off
	plot->decomp_rate = 0;
	plot->Resp_DeadWood = 0;
	plot->Resp_Soil_fast = 0;
	plot->Resp_Soil_slow = 0;
	plot->R_total = 0;
	plot->C_flux = 0;
}

}

// -----------------------------------------------------------
// ----------------- end of for_carbon.cpp -------------------
// -----------------------------------------------------------
