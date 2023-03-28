#include <stdio.h>
#include <stdlib.h>
//#include "adcs.c"
#include "adcs.h"
//#include "help_adcs.c"
#include "help_adcs.h"
#include "satutl.h"
#include<math.h>

//#include "igrf13.c"
#include "igrf13.h"
//#include "orbit_propagators_utils.c"
#include "orbit_propagators_utils.h"
//#include "sgp.c"
#include "sgp.h"
//#include "igrf13syn.c"
#include "igrf13syn.h"
#include "sgp4.h"
//#include "sgp4.c"

//per compilar i executar f6
int main_adcs() {
    
    int i;
    p_struct p;
   
    p.a[0][0]=1;
    p.a[0][1]=0;
    p.a[1][0]=0;
    p.a[1][1]=0.22;

    p.b[0]=10;
    p.b[1]=0.78;

    p.c[0]=2.0748099129e-06;
    p.c[1]=-0.0053818;

    p.d=0.0059;

    p.inertia[0]=0.025;
    p.inertia[1]=0.025;
    p.inertia[2]=0.025;

    p.accel_sat[0]=100;
    p.accel_sat[1]=100;
    p.accel_sat[2]=100;

    p.l[0]=0;
    p.l[1]=0;

    p.x_pitch[0]=0;
    p.x_pitch[1]=0;
    p.x_roll[0]=0;
    p.x_roll[1]=0;
    p.x_yaw[0]=0;
    p.x_yaw[1]=0;

    p.q_desired_state[0]=0;
    p.q_desired_state[1]=0;
    p.q_desired_state[2]=0;
    p.q_desired_state[3]=1;

    p.q_target_last[0]=1;
    p.q_target_last[1]=0;
    p.q_target_last[2]=0;
    p.q_target_last[3]=0;

    p.eci_vecor[0]=-1;
    p.eci_vecor[1]=0;
    p.eci_vecor[2]=0;

    p.body_vector[0]=0;
    p.body_vector[1]=0;
    p.body_vector[2]=1;

    p.max_angle=0.01;

    double q_ECI_Body[4]={1, 0, 0, 0};
    double torque[3];


    PID3Axis(&q_ECI_Body, &p, &torque);

    for(i=0;i<3;i++){
        printf("%e\n", torque[i]);
    }


    /*
    double bxyz[3];
    igrf13(2023.0, 500, 40, 0, &bxyz);

    printf("El valor del camp magentic es:\n");
    printf("En x:\t%e\n", bxyz[0]);
    printf("En y:\t%e\n", bxyz[1]);
    printf("En z:\t%e\n", bxyz[2]);
    */

    //coid per provar el sgp4
    /*
    satrec prova;
    char ho[70]="1 32765U 08017A   09209.18550013  .00001988  00000-0  53832-4 0  3469";
    char nn[70]="2 32765  13.0144 166.4925 0314115 276.2764  80.1808 14.81449833 69409";
    readTwoLine(&ho, &nn, &prova);
    double r[3], v[3];
    sgp4(&prova, 83.7, &r, &v);
    int i;
    printf("r es: \n");
    for(i=0;i<3;i++){
        printf("r %d = %f\n", i, r[i]);
    }
    printf("\n\nv es: \n");
    for(i=0;i<3;i++){
        printf("v %d = %f\n", i, v[i]);
    }
    */
    

    /*prova days2mdh
    double mon,da, hr,min, sec;
    days2mdh(2015, 153.2, &mon, &da, &hr,&min, &sec);
    printf("mont: %.0f\n",mon);
    printf("day: %.0f\n", da);
    printf("hour: %.0f\n", hr);
    printf("min: %.0f\n", min);
    printf("sec: %.0f\n", sec);
    */

   /*//prova jday
   double jj;
   jday(2009, 2, 28, 5, 39, 59, &jj);
   printf("%e", jj);*/
   //getgravc(84);
   //printf("%f", gstime(2080.3 + 2433281.5));

}

