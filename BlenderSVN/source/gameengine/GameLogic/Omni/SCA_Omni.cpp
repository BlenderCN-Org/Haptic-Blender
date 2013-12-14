/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): snailrose.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file gameengine/GameLogic/Omni/SCA_Omni.cpp
 *  \ingroup gamelogic
 */

#ifdef WITH_SDL
#  include <SDL.h>
#endif

#ifdef WITH_H3D
#	include <HAPI/AnyHapticsDevice.h>
#	include <HAPI/GodObjectRenderer.h>
#	include <HAPI/FrictionSurface.h>
#	include <HAPI/HapticTriangleTree.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <cstring>



#include "SCA_Omni.h"

#include "BLI_path_util.h"



SCA_Omni::SCA_Omni(short int index)
	:
	m_omniindex(0),
	m_prec(3200),
	m_isinit(0)
	{
#ifdef WITH_H3D
#endif
}

SCA_Omni::~SCA_Omni()

{
#ifdef WITH_H3D

#endif
}
SCA_Omni *SCA_Omni::m_instance[OMNIINDEX_MAX];
int SCA_Omni::m_omninum = 0;
int SCA_Omni::m_refCount = 0;

SCA_Omni *SCA_Omni::GetInstance( short int omniindex )
{

	if (omniindex < 0 || omniindex >= OMNIINDEX_MAX) {
		Omni_ECHO("Error-invalid Omni index: " << omniindex);
		return NULL;
	}

	 if (m_refCount == 0) 
	 {
		 int i;


		
		for (i=0; i<1; i++) { //OMNIINDEX_MAX
			 m_instance[i] = new SCA_Omni(i);
			 m_instance[i]->CreateOmniDevice();
		}
		 m_refCount = 1;
	 }
	else
	{
		m_refCount++;
	}
	return m_instance[0];
}

void SCA_Omni::ReleaseInstance()
{

	 m_Omni.disableDevice();
	 m_Omni.releaseDevice();
	 printf("Omni Released\n");
	
}

void SCA_Omni::cSetPrecision(int val)
{
	m_prec = val;
}



void SCA_Omni::SetHapticObject(char * infile)//input some vector list
{

	//if (infile.size() == strlen(infile.c_str()))
	//	printf("Posisble problem?\n");
	std::ifstream ifile(infile, ios::in);
	//printf("File should be: %s\n",infile);
	vector< HAPI::Collision::Triangle > triangles; 
	int i =0;
	Vec3 v0, v1, v2; 
	string line;

	if (!ifile) {
	//  cerr << "Can't open input file " << infile.c_str() << endl;
	}
	if(ifile.is_open())
	{
		while (getline(ifile, line))
			{
				//printf("Should be in the file reading\n");
				std::istringstream iss(line);

				if (!(iss >> v0.x >> v0.z >> v0.y)) { break; } // error
				v0.z=-v0.z;
				printf("V1 %f %f %f\n",v0.x,v0.y,v0.z);

				getline(ifile, line);
				std::istringstream iss2(line);

				if (!(iss2 >> v1.x >> v1.z >> v1.y)) { break; } // error
				v1.z=-v1.z;
				printf("V2 %f %f %f\n",v1.x,v1.y,v1.z);

				getline(ifile, line);
				std::istringstream iss3(line);

				if (!(iss3 >> v2.x >> v2.z >> v2.y)) { break; } // error
				v2.z=-v2.z;
				printf("V3 %f %f %f\n",v2.x,v2.y,v2.z);

				// process pair (a,b)
			
		   // get v0, v1 and v2 from your data structure
		   // create HAPI triangle
			   HAPI::Collision::Triangle t( v0, v1, v2);

			   triangles.push_back( t );
			   printf("Size of triangles %d\n",triangles.size());
		   }
	   ifile.close();
	}
	else
	{
		cout << infile << endl;
		printf("Unable to open file\n");
	}
	//printf("Testing after file read %f %f %f\n",v0.x,v0.y,v0.z);
	

	// Create the haptic shape to be rendered.
	HAPI::Collision::AABBTree *the_tree = new HAPI::Collision::AABBTree( triangles );


	HAPI::HAPISurfaceObject * my_surface = new HAPI::FrictionSurface();//new HAPI::FrictionSurface(2.0, 0, 0, 0, true, true);

	HAPI::HapticTriangleTree *tree_shape;

	// touch only outside (FRONT , also possible for inside (BACK) or both (will be something like BACK_AND_FRONT check the enum ;))
	tree_shape = new HAPI::HapticTriangleTree( the_tree, my_surface, HAPI::Collision::FRONT );
		
	// Transfer shape to the haptics loop.
	m_Omni.addShape( tree_shape );

	m_Omni.transferObjects();
	
	printf("Added new mesh!\n");
}

//INITIALIZE HAPTIC DEVICE HERE
//bool 
bool SCA_Omni::CreateOmniDevice(void)
{
	

		printf("Starting omni creationg\n");
		m_Omni.setHapticsRenderer( new GodObjectRenderer() );
		   if(m_Omni.initDevice() != HAPIHapticsDevice::SUCCESS ) {
			 //cerr << m_private->m_Omni->getLastErrorMsg() << endl;
			 m_isinit = false;
			 printf("Device not found!\n");
			 return false;
		   }
		   printf("Device should be created\n");
	   m_isinit = true;
		if(m_Omni.enableDevice() != HAPIHapticsDevice::SUCCESS){
			printf("Device enabling failed!\n");
		}
		printf("Device is enabled (unless prev statment!\n");
		return true;

}


void SCA_Omni::DestroyOmniDevice(void)
{
	 m_Omni.disableDevice();
	 m_Omni.releaseDevice();
	 printf("Omni destroyed\n");
}

int SCA_Omni::Connected(void)
{

	if (m_isinit)// && m_private->m_Omni != NULL)
		 return 1;
	return 0;
}



const char *SCA_Omni::GetName()
{

	return "This does nothing?";

}
