/***************************************************************************
 *   Copyright (C) 2006 by Emmanuel Nuño                                   *
 *   emmanuel.nuno@upc.edu                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/



//mt library
#include <mt/mt.h>

//haptic library
#include <hardlab/haptic/haptic.h>


int main(int argc, char *argv[])
{

	haptic::Haptic myHaptic;
	myHaptic.calibrate();
	myHaptic.start();

	mt::Transform myPos;
	mt::Vector3 pos;

	Vect6 Force(6);

	Force[0]=0;
	Force[1]=0;
	Force[2]=0;
	Force[3]=0;
	Force[4]=0;
	Force[5]=0;

	const mt::Scalar K=-0.05;


	for (int i=0; i<10000; i++)
	{

		myHaptic.getPosition(myPos);
		pos=myPos.getTranslation();

		Force[0]=K*pos[0];
		Force[1]=K*pos[1];
		Force[2]=K*pos[2];




		//std::cout<<myPos<<std::endl;

		myHaptic.setForce(Force);

		Sleep(5);

	}

	myHaptic.stop();

	return 1;
}
