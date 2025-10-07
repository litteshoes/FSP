////////////////////////////////////////////////////////////////////
//
// FORMIND � the forest model
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
// File						main.cpp
// Description				Start of the model
//
///////////////////////////////////////////////////////////////////
#include <iostream>
#include "formind.h"
#include "for_var.h"
#include "for_misc.h"
#ifdef underconstruction
#include "for_sidar.h"
#endif

/* !
 \brief       executes the whole project
 \param       int, char (command line arguments of main function)
 \return      0 (end of the model)
 \details	  initializes formind model, imports *.par file, executes model,
 writes result files
 */
int main(int argc, char*argv[]) {
	int error = 0;
	Formind formind_model;
	formind_model.ReadCommandLineParams(argc, argv);

		error = formind_model.Start();
		if (!error) {
			formind_model.PrintInfo();
			portableTimer timer;
			timer.start();
			formind_model.Run();
			formind_model.WriteResults();
#ifdef underconstruction
			if (formind_model.UseLidar())
				LidarPostProcessing();
#endif
			timer.stop();
			std::cout <<
				 "=============== Simulation Successful =====================" <<
				 std::endl;
			std::cout << "Runtime: " << timer.getTimeDifference()
				 << " seconds" << std::endl;
		}
		else {
			std::cerr << "error: initializing formind model!" << std::endl;
			return (-1);
		}

	return (0);
}
