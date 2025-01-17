/*--------------------------------------------------------------*/
/* 																*/
/*					execute_firespread_event								*/
/*																*/
/*	execute_firespread_event.c - creates a patch object					*/
/*																*/
/*	NAME														*/
/*	execute_firespread_event.c - creates a patch object					*/
/*																*/
/*	SYNOPSIS													*/
/*																*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*																*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*--------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "rhessys.h"

// test comment
void execute_firespread_event(
									 struct	world_object *world,
									 struct	command_line_object	*command_line,
									 struct date	current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/


	void *alloc(size_t, char *, char *);


	void	compute_fire_effects(
		struct patch_object *,
		double,
		struct	command_line_object	*);


	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	struct fire_object **fire_grid;
	struct patch_fire_object **patch_fire_grid;
	struct patch_object *patch;
//	struct node_fire_wui_dist *tmp_node;
	int i,j,p,c,layer;
	double pspread;
	double mean_fuel_veg=0,mean_fuel_litter=0,mean_soil_moist=0,mean_fuel_moist=0,mean_relative_humidity=0,
		mean_wind_direction=0,mean_wind=0,mean_z=0,mean_temp=0,mean_et=0,mean_pet=0,mean_understory_et=0,mean_understory_pet=0;
	double denom_for_mean=0;

	patch_fire_grid=world[0].patch_fire_grid;
	fire_grid = world[0].fire_grid;

// add code here to define understory et and pet to calculate understory deficit. The definition of understory here
// will be the same as that for fire effects below

// default value to determine if you are in the understory, compare layer height to stratum-level height threshold (canopy_strata_upper[0].defaults[0][0].understory_height_thresh,
// compared to patch[0].canopy_strata[(patch[0].layers[layer+1].strata[c])].epv.height

	/*--------------------------------------------------------------*/
	/* update fire grid variables			*/
	/* first reset the values				*/
	/*--------------------------------------------------------------*/
	printf("In WMFire\n");
	for  (i=0; i< world[0].num_fire_grid_row; i++) {
  	  for (j=0; j < world[0].num_fire_grid_col; j++) {
		  world[0].fire_grid[i][j].fire_size=0; // reset grid to no fire
		if(world[0].patch_fire_grid[i][j].occupied_area==0)
		{
			  if(world[0].defaults[0].fire[0].fire_in_buffer==0)
			  {
				    world[0].fire_grid[i][j].fuel_veg = 0.0; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
				    world[0].fire_grid[i][j].fuel_litter = 0.0;
				    world[0].fire_grid[i][j].fuel_moist = 100.0;
				    world[0].fire_grid[i][j].soil_moist = 100.0;
				    world[0].fire_grid[i][j].relative_humidity = 100.0;
				    world[0].fire_grid[i][j].wind_direction = 0;
				    world[0].fire_grid[i][j].wind = 0.0;
				    world[0].fire_grid[i][j].z=0.0; // mk: add so we can calculate the current elevation as the weighted mean elevation of the patches
				    world[0].fire_grid[i][j].temp=0.0;
				    world[0].fire_grid[i][j].et=1.0; // mk: should be 1 so that the deficit in the buffer is zero
				    world[0].fire_grid[i][j].pet=1.0;
				    world[0].fire_grid[i][j].understory_et=0.0;
				    world[0].fire_grid[i][j].understory_pet=1.0;
				    world[0].fire_grid[i][j].ign_available=0;

				     if(world[0].defaults[0].fire[0].fire_verbose==1)

					  printf("No fire in buffer\n");

			  }
			  else // if denom_for_mean==0, then this initializes the buffer, otherwise the mean is filled in below
			  {
				    world[0].fire_grid[i][j].fuel_veg = 10.0; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
				    world[0].fire_grid[i][j].fuel_litter = 10.0;
				    world[0].fire_grid[i][j].fuel_moist = 0;
				    world[0].fire_grid[i][j].soil_moist = 0;
				    world[0].fire_grid[i][j].relative_humidity = 0;
				    world[0].fire_grid[i][j].wind_direction = world[0].basins[0][0].hillslopes[0][0].zones[0][0].wind_direction;//patches[0].zone[0].wind_direction;// or pull the wind direction from the default wind
				    world[0].fire_grid[i][j].wind = 50.0;
				    world[0].fire_grid[i][j].z=world[0].patch_fire_grid[i][j].elev; // mk: add so we can calculate the current elevation as the weighted mean elevation of the patches
				    world[0].fire_grid[i][j].temp=0.0;
		  		    world[0].fire_grid[i][j].et=0.0;
				    world[0].fire_grid[i][j].pet=0.0;
				    world[0].fire_grid[i][j].understory_et=0.0;
				    world[0].fire_grid[i][j].understory_pet=0.0;

				    world[0].fire_grid[i][j].ign_available=0;
			  }
		}
		else
		{
		    world[0].fire_grid[i][j].fuel_veg = 0.0; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
		    world[0].fire_grid[i][j].fuel_litter = 0.0;
		    world[0].fire_grid[i][j].fuel_moist = 0.0;
		    world[0].fire_grid[i][j].soil_moist = 0.0;
		    world[0].fire_grid[i][j].relative_humidity = 0.0;
		    world[0].fire_grid[i][j].wind_direction = 0.0;
		    world[0].fire_grid[i][j].wind = 0.0;
		    world[0].fire_grid[i][j].z=0.0; // mk: add so we can calculate the current elevation as the weighted mean elevation of the patches
		    world[0].fire_grid[i][j].temp=0.0;
		    world[0].fire_grid[i][j].et=0.0;
		    world[0].fire_grid[i][j].pet=0.0;
		    world[0].fire_grid[i][j].understory_et=0.0;
		    world[0].fire_grid[i][j].understory_pet=0.0;
		    world[0].fire_grid[i][j].ign_available=1;	/* then make this available for ignition */
		 if(world[0].defaults[0].fire[0].fire_verbose==1)
			printf("Initialize ws patches\n");
		}
//    printf("checking num patches. row %d col %d numPatches %d\n",i,j,patch_fire_grid[i][j].num_patches);
		for (p=0; p < world[0].patch_fire_grid[i][j].num_patches; ++p) { // should just be 1 now...
                         if(world[0].defaults[0].fire[0].fire_verbose==1)
			{
                                printf("Patch p: %d, i: %d, j:%d\n",p,i,j);
			}

			patch = world[0].patch_fire_grid[i][j].patches[p]; //So this is patch family now? points to patch family
			if(world[0].defaults[0].fire[0].fire_verbose==1)
				printf("Patch p1 %lf\n", patch[0].litter_cs.litr1c); 
			patch = world[0].patch_fire_grid[i][j].patches[p]; //So this is patch family now? points to patch family
			world[0].fire_grid[i][j].fuel_litter += (patch[0].litter_cs.litr1c +	patch[0].litter_cs.litr2c +	// This sums the litter pools
				patch[0].litter_cs.litr3c +	patch[0].litter_cs.litr4c) * patch_fire_grid[i][j].prop_patch_in_grid[p];
			if(world[0].defaults[0].fire[0].fire_verbose==1)
				printf("grif litter %\f\n",world[0].fire_grid[i][j].fuel_litter);

			if( patch[0].litter.rain_capacity!=0)	// then update the fuel moisture, otherwise don't change it
			    world[0].fire_grid[i][j].fuel_moist += (patch[0].litter.rain_stored / patch[0].litter.rain_capacity) *
							patch_fire_grid[i][j].prop_patch_in_grid[p];
/*			fire_grid[i][j].fuel_moist += (patch[0].litter.rain_stored / patch[0].litter.rain_capacity) *
						patch_fire_grid[i][j].prop_patch_in_grid[p];
*/
			 if(world[0].defaults[0].fire[0].fire_verbose==1)
				printf("Patch p: %d\n",p);
	
	// this is the canopy fuels

			for ( layer=0 ; layer<patch[0].num_layers; layer++ ){
				for ( c=0 ; c<patch[0].layers[layer].count; c++ ){
		
						if(world[0].defaults[0].fire[0].fire_verbose==1)				
							printf("Layers: %d, count: %d\n",layer,c);

					world[0].fire_grid[i][j].fuel_veg += (patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
						* patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.leafc) *
							patch_fire_grid[i][j].prop_patch_in_grid[p] ;
              world[0].fire_grid[i][j].fuel_litter +=(patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cover_fraction
                * patch[0].canopy_strata[(patch[0].layers[layer].strata[c])][0].cs.dead_leafc) *
                patch_fire_grid[i][j].prop_patch_in_grid[p]; // adds standing dead grass to fuel litter for firespread
					}
				}
		 if(world[0].defaults[0].fire[0].fire_verbose==1)
			printf("pixel veg and prop patch in grid: %lf\t%lf\n",world[0].fire_grid[i][j].fuel_veg,patch_fire_grid[i][j].prop_patch_in_grid[p]);
			

			world[0].fire_grid[i][j].soil_moist += patch[0].rootzone.S * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];	//soil moisture, divided by proportion of the patch in that grid cell;

			world[0].fire_grid[i][j].wind += patch[0].zone[0].wind * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].wind_direction += patch[0].zone[0].wind_direction * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].relative_humidity += patch[0].zone[0].relative_humidity * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].z += patch[0].z*patch_fire_grid[i][j].prop_patch_in_grid[p]; // elevation
			world[0].fire_grid[i][j].temp += patch[0].zone[0].metv.tavg*patch_fire_grid[i][j].prop_patch_in_grid[p];// temperature? mk
			world[0].fire_grid[i][j].et += patch[0].fire.et * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			world[0].fire_grid[i][j].pet += patch[0].fire.pet * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			if(patch[0].fire.understory_et==0&&patch[0].fire.understory_pet==0) // means no understory present, then use overall et and pet for deficit calculation for this patch
			{
				world[0].fire_grid[i][j].understory_et = world[0].fire_grid[i][j].et;
				world[0].fire_grid[i][j].understory_pet = world[0].fire_grid[i][j].pet;
			}
			else
			{
				world[0].fire_grid[i][j].understory_et += patch[0].fire.understory_et * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
				world[0].fire_grid[i][j].understory_pet += patch[0].fire.understory_pet * world[0].patch_fire_grid[i][j].prop_patch_in_grid[p];
			}
		 if(world[0].defaults[0].fire[0].fire_verbose==1)
			printf("patch pet, patch et: %lf\t%lf\n",patch[0].fire.pet,patch[0].fire.et);

		}
		if(world[0].patch_fire_grid[i][j].occupied_area>0&&world[0].defaults[0].fire[0].fire_in_buffer==1) // if allowing fire into the buffer (on raster grid outside of watershed boundaries), then fill with mean field values within watershed boundary
		{ // this loop fills sums to calculate the mean value across watershed
			denom_for_mean+=1;
			mean_fuel_veg+=world[0].fire_grid[i][j].fuel_veg; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
			mean_fuel_litter+=world[0].fire_grid[i][j].fuel_litter;
			mean_fuel_moist+=world[0].fire_grid[i][j].fuel_moist;
			mean_soil_moist+=world[0].fire_grid[i][j].soil_moist;
			mean_relative_humidity+=world[0].fire_grid[i][j].relative_humidity;
			mean_wind_direction+=world[0].fire_grid[i][j].wind_direction;
			mean_wind+=world[0].fire_grid[i][j].wind;
			mean_temp+=world[0].fire_grid[i][j].temp;
			mean_et+=world[0].fire_grid[i][j].et;
			mean_pet+=world[0].fire_grid[i][j].pet;
			mean_understory_et+=world[0].fire_grid[i][j].understory_et;
			mean_understory_pet+=world[0].fire_grid[i][j].understory_pet;
		//	printf("et: %f  pet: %f  ",world[0].fire_grid[i][j].et,world[0].fire_grid[i][j].pet);
		}

		world[0].fire_grid[i][j].et=world[0].fire_grid[i][j].et*1000; // convert to mm
		world[0].fire_grid[i][j].pet=world[0].fire_grid[i][j].pet*1000; // convert to mm
		world[0].fire_grid[i][j].understory_et=world[0].fire_grid[i][j].understory_et*1000; // convert to mm
		world[0].fire_grid[i][j].understory_pet=world[0].fire_grid[i][j].understory_pet*1000; // convert to mm


	}
	}
//	printf("denom: %lf\t",denom_for_mean);
	if(denom_for_mean>0&&world[0].defaults[0].fire[0].fire_in_buffer==1) // so here we calculate the mean value
	{
//		printf("in denom if\n");
		mean_fuel_veg=mean_fuel_veg/denom_for_mean;
		mean_fuel_litter=mean_fuel_litter/denom_for_mean;
		mean_fuel_moist=mean_fuel_moist/denom_for_mean;
		mean_soil_moist=mean_soil_moist/denom_for_mean;
		mean_relative_humidity=mean_relative_humidity/denom_for_mean;
		mean_wind_direction=mean_wind_direction/denom_for_mean;
		mean_wind=mean_wind/denom_for_mean;
		mean_temp=mean_temp/denom_for_mean;
		mean_et=mean_et/denom_for_mean;
		mean_pet=mean_pet/denom_for_mean;
		mean_understory_et=mean_understory_et/denom_for_mean;
		mean_understory_pet=mean_understory_pet/denom_for_mean;
	//	printf("mean et: %f  mean pet: %f  ",mean_et,mean_pet);

	//	printf("mean pet, mean et: %lf\t%lf\n",mean_pet,mean_et);
	//	printf("mean wind: %lf, mean direction %lf \n",mean_wind,mean_wind_direction);
		for  (i=0; i< world[0].num_fire_grid_row; i++) {
		  for (j=0; j < world[0].num_fire_grid_col; j++) {
			  if(world[0].patch_fire_grid[i][j].occupied_area==0) // and here we fill in the buffer
			  {

				world[0].fire_grid[i][j].fuel_veg = mean_fuel_veg; // this should work to initialize the grid, so if none of the patches overlap a grid point the fuel is zero and fire doesn't spread
				world[0].fire_grid[i][j].fuel_litter = mean_fuel_litter;
				world[0].fire_grid[i][j].fuel_moist = mean_fuel_moist;
				world[0].fire_grid[i][j].soil_moist = mean_soil_moist;
				world[0].fire_grid[i][j].relative_humidity = mean_relative_humidity;
				world[0].fire_grid[i][j].wind_direction = mean_wind_direction;
				world[0].fire_grid[i][j].wind = mean_wind;
				world[0].fire_grid[i][j].temp=mean_temp;
				world[0].fire_grid[i][j].z=world[0].patch_fire_grid[i][j].elev;
				world[0].fire_grid[i][j].et=mean_et*1000; // convert to mm
				world[0].fire_grid[i][j].pet=mean_pet*1000; // convert to mm
				world[0].fire_grid[i][j].understory_et=mean_understory_et*1000; // convert to mm
				world[0].fire_grid[i][j].understory_pet=mean_understory_pet*1000; // convert to mm
	//	printf("in denom if take 2 update values\n");
			  }
		     }
		}
	}

	/*--------------------------------------------------------------*/
	/* Call WMFire	 						*/
	/*--------------------------------------------------------------*/
	printf("calling WMFire: month %ld year %ld  cell res %lf  nrow %d ncol % d\n",current_date.month,current_date.year,command_line[0].fire_grid_res,world[0].num_fire_grid_row,world[0].num_fire_grid_col);
// needs to return fire size, not just grid--create structure that includes fire size, or a 12-member array of fire sizes, and/or a tally of fires > 1000 acres
	world[0].fire_grid=WMFire(command_line[0].fire_grid_res,world[0].num_fire_grid_row,world[0].num_fire_grid_col,current_date.year,current_date.month,world[0].fire_grid,*(world[0].defaults[0].fire));
 	printf("Finished calling WMFire\n");
	/*--------------------------------------------------------------*/
	/* update biomass after fire					*/
	/*--------------------------------------------------------------*/

	// if(world[0].fire_grid[0][0].fire_size>0) // only do this if there was a fire
	for  (i=0; i< world[0].num_fire_grid_row; i++) {
  		for (j=0; j < world[0].num_fire_grid_col; j++) {
			for (p=0; p < patch_fire_grid[i][j].num_patches; ++p) {
				patch = world[0].patch_fire_grid[i][j].patches[p];

				patch[0].burn = world[0].fire_grid[i][j].burn * world[0].patch_fire_grid[i][j].prop_grid_in_patch[p];
				pspread = world[0].fire_grid[i][j].burn * world[0].patch_fire_grid[i][j].prop_grid_in_patch[p];
// so I think here we could flag whether to turn salient fire on in wui; convert fire size in pixels to ha, assuming the cell_res is in m
				/* (if pspread>0&world[0].fire_grid[0][0].fire_size*command_line[0].fire_grid_res*command_line[0].fire_grid_res*0.0001>=400) // also need a flag with the fire size to trigger event, because fire > 400 ha
				{
					// linked list loop
					for(w=0;w<3;w++) # for each level of salience, 1 = <= 3 km, 2 = <=5 km; 3=<=10 km
					{
						tmp_node=world[0].fire_grid[i][j].wuiList[w] // where wuiList[w] is the linked list of patches within w index of this pixel
						while(tmp_node!=NULL)
						{
							if(tmp_node->patch.wuiFire==0||(i+1)<tmp_node.patch.wuiFire) // then this pixel is closer to the wui and should activate a more salient fire
								tmp_node->patch.wuiFire=i+1; // then flag this wuiPatch with a salient fire event
							tmp_node=tmp_node->next;
						}
					}
				}

				*/
				if(world[0].defaults[0].fire[0].calc_fire_effects==1)
				{
					compute_fire_effects(
						patch,
						pspread,
						command_line);
				}

			}
		}
	}
	return;
} /*end execute_firespread_event.c*/


// ----------printing code samples -----------
//	printf("in execute_firespread_event_1\n");
//	printf("*********************\n");
//	printf("Understory Height = %f\n", height_under);






/*

// For 	Local variable definition.
struct fire_veg_loss_struct	fire_veg_effects;

// RHESSys.h
/*----------------------------------------------------------*/
/*      Define post fire vegetation loss structure.    	    */
/*----------------------------------------------------------*/
/*struct fire_veg_loss_struct
{
	double pspread
	double layer_upper_height
	double layer_lower_height
	double layer_lower_c
	double understory_litter_c
};*/



