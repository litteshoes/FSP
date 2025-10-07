#include "for_environment.h"
#include "for_misc.h"
#include "for_io.h"
#include <iostream>
#include <cmath>

Environment environment;

void Environment::DoVariableClimate() {
    try {
        HecPointer hec = FirstHec;
        while (hec != NULL) {
            PlotPointer plot = hec->FirstPlot;
            while (plot != NULL) {
                std::string fn = N_Par.Climate_File[0];
                if (fn.size() == 0 || fn == "None") {
                    std::cerr << "ERROR: Climate_File is empty/None" << std::endl;
                    throw std::runtime_error("Climate_File empty");
                }
                std::string pardir = fileNames.getParFileDirAbsolutePath();
                std::string full = makeNicePathForYourOS(pardir + directorySeparator + fn);
                if (!readClimateFileToPlot(full, plot)) {
                    std::cerr << "ERROR: Failed to read climate file '" << fn << "'" << std::endl;
                    throw std::runtime_error("readClimateFileToPlot failed");
                }
                plot = plot->next;
            }
            hec = hec->next;
        }
        initialized = true;
    } catch (...) {
        initialized = false;
        throw;
    }
}

// helper: compute annual window [startIdx, endIdx) for daily vectors
static void getAnnualIndices(int totalDays, double currentTimeYear, int &i0, int &i1) {
    // currentTimeYear is years since start; take floor as current integer year
    int year = static_cast<int>(std::floor(currentTimeYear + 1e-9));
    const int daysPerYear = 365; // assume 365; if PET或CO2长度更大也截取
    i0 = year * daysPerYear;
    i1 = i0 + daysPerYear;
    if (i0 < 0) i0 = 0;
    if (i1 > totalDays) i1 = totalDays;
    if (i0 >= i1) { i0 = 0; i1 = totalDays; }
}

bool Environment::calculate_reduction_factors() {
    if (!initialized) return false;

    HecPointer hec = FirstHec;
    while (hec != NULL) {
        PlotPointer plot = hec->FirstPlot;
        while (plot != NULL) {
            // annual window indices based on current model time T.T
            int totalN = static_cast<int>(plot->temperature.size());
            totalN = std::min(totalN, (int)plot->daylength.size());
            totalN = std::min(totalN, (int)plot->irradiance.size());
            totalN = std::min(totalN, (int)plot->precipitation.size());
            totalN = std::min(totalN, (int)plot->pet.size());
            totalN = std::min(totalN, (int)plot->CO2_concentration.size());
            if (totalN <= 0) {
                std::cerr << "ERROR: Climate vectors are empty or inconsistent in length." << std::endl;
                throw std::runtime_error("Climate data empty");
            }
            int i0=0, i1=totalN;
            getAnnualIndices(totalN, T.T, i0, i1);
            if (i1 <= i0) {
                std::cerr << "ERROR: Annual window is empty for current year." << std::endl;
                throw std::runtime_error("Empty annual window");
            }

            // --- Vegetation-period indexing (based on temperature) ---
            int a0t = 0, a1t = totalN;
            getAnnualIndices(totalN, T.T, a0t, a1t);
            const bool haveTemp = (totalN > 0);
            const double tmin_vp = (double)N_Par.Temperature_min;
            int veg_count = 0;
            double sum_dl_vp = 0.0;
            double sum_ir_vp = 0.0;
            if (haveTemp && (N_Par.Veg_period_ON || N_Par.Daylength_ON || N_Par.variable_Irradiance_ON)) {
                for (int i = a0t; i < a1t; ++i) {
                    if (plot->temperature[i] >= tmin_vp) {
                        ++veg_count;
                        if (i < (int)plot->daylength.size()) sum_dl_vp += plot->daylength[i];
                        if (i < (int)plot->irradiance.size()) sum_ir_vp += plot->irradiance[i];
                    }
                }
            }

            // Daylength mean (unweighted arithmetic mean for equivalence of total daylight seconds)
            double sum_dl = 0.0;
            for (int i = i0; i < i1; ++i) sum_dl += plot->daylength[i];
            double dl_mean = sum_dl / (double)(i1 - i0);
            plot->mean_daylength = (float)dl_mean;

            // Irradiance equivalent (energy-weighted by daylight seconds)
            double sumS = 0.0;
            double sumI = 0.0;
            for (int i = i0; i < i1; ++i) {
                double S = std::max(0.0, plot->daylength[i]) * 3600.0;
                sumS += S;
                sumI += plot->irradiance[i] * S;
            }
            if (sumS <= 0.0) {
                std::cerr << "ERROR: Sum of daylight seconds is zero in annual window." << std::endl;
                throw std::runtime_error("Zero daylight seconds");
            }
            double ir_mean = sumI / sumS;
            if (plot->mean_light_above_canopy.size() != static_cast<size_t>(MAXGRP))
                plot->mean_light_above_canopy.assign(MAXGRP, static_cast<float>(ir_mean));
            else
                for (int i = 0; i < MAXGRP; ++i) plot->mean_light_above_canopy[i] = static_cast<float>(ir_mean);
            // also provide total mean above-canopy irradiance for floor calculation
            plot->total_light_above_canopy = static_cast<float>(ir_mean);

            // Temperature reduction (daily Gaussian → energy-weighted annual mean)
            if (plot->temp_photosynthese_reduction.size() != static_cast<size_t>(MAXGRP))
                plot->temp_photosynthese_reduction.assign(MAXGRP, 1.0);
            double mu = (double)N_Par.Temperature_opt;
            double sig = std::max(1e-3, (double)N_Par.Temperature_sig);
            double sumFT = 0.0;
            for (int i = i0; i < i1; ++i) {
                double S = std::max(0.0, plot->daylength[i]) * 3600.0;
                double Td = plot->temperature[i];
                double fT = std::exp(-0.5 * std::pow((Td - mu) / sig, 2.0));
                sumFT += fT * S;
            }
            double fT_eq = sumFT / sumS;
            for (int i = 0; i < MAXGRP; ++i) plot->temp_photosynthese_reduction[i] = (float)fT_eq;

            // Vegetation period: count annual days above threshold
            if (!plot->temperature.empty()) {
                int days = 0;
                for (int i = a0t; i < a1t; ++i) if (plot->temperature[i] >= tmin_vp) ++days;
                plot->length_of_vegetation_periode = days > 0 ? days : 365;
            } else {
                plot->length_of_vegetation_periode = 365;
            }

            // Temperature effect on respiration (Q10) - energy-weighted
            {
                double q10 = (N_Par.Temperature_Q10 > 0.0) ? (double)N_Par.Temperature_Q10 : 2.0;
                double tref = (double)N_Par.Temperature_reference;
                double sumRQ = 0.0;
                for (int i = i0; i < i1; ++i) {
                    double S = std::max(0.0, plot->daylength[i]) * 3600.0;
                    double Td = plot->temperature[i];
                    double r = std::pow(q10, (Td - tref) / 10.0);
                    sumRQ += r * S;
                }
                plot->temp_resp_reduction = (float)(sumRQ / sumS);
            }

            // CO2: energy-weighted mean ratio to reference
            double cref = std::max(1e-6, (double)N_Par.CO2_reference_concentration);
            double sumC = 0.0;
            for (int i = i0; i < i1; ++i) {
                double S = std::max(0.0, plot->daylength[i]) * 3600.0;
                sumC += (plot->CO2_concentration[i] / cref) * S;
            }
            plot->CO2_inc_GPP = (float)(sumC / sumS);

            // --- Water limitation (daily P/PET → energy-weighted annual mean) ---
            {
                double pr_sum = 0.0;
                double pet_sum = 0.0;
                double sumFW = 0.0;
                for (int i = i0; i < i1; ++i) {
                    double S = std::max(0.0, plot->daylength[i]) * 3600.0;
                    double P = plot->precipitation[i];
                    double E = plot->pet[i];
                    pr_sum += P;
                    pet_sum += E;
                    double fw = (E > 1e-6) ? std::min(1.0, std::max(0.0, P / E)) : 1.0;
                    sumFW += fw * S;
                }
                plot->PR = (float)pr_sum;
                plot->PET = (float)pet_sum;
                plot->water_reduction = (float)(sumFW / sumS);
            }

            plot = plot->next;
        }
        hec = hec->next;
    }
    return true;
}


