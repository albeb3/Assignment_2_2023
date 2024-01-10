#include <ros/ros.h>
#include <assignment_2_2023/Posvel.h> // Includi il tipo di messaggio personalizzato
#include <math.h>
#include <assignment_2_2023/PlanningAction.h>
#include "assignment_2_2023/Dist_vel.h"



// Dichiarazione delle variabili globali

int max_values=0;
float vel = 0.0;
int count = 0;
int ind = 0; // Indice corrente nell'array dei valori di velocità
float *velocity_values=nullptr; // Array per memorizzare i valori di velocitàInizializza tutti gli elementi dell'array a zero
float goal_x=0,goal_y=0;
float position_x, position_y, vel_x, vel_y;
float ave_speed,dist;

void target_position(const assignment_2_2023::PlanningActionGoal::ConstPtr &msg){
	goal_x=msg->goal.target_pose.pose.position.x;
	goal_y=msg->goal.target_pose.pose.position.y;

}

// Funzione ausiliaria per calcolare la distanza tra due punti
float distance(float position_x, float position_y, float goal_x, float goal_y){
    return sqrt(pow((goal_x - position_x), 2.0) + pow((goal_y - position_y), 2.0));
}


float average_speed(float v_x, float v_y){
    float sum = 0.0;

    // Memorizza i valori di velocità nell'array e aggiorna l'indice
    velocity_values[ind] = sqrt(pow(vel_x, 2.0) + pow(vel_y, 2.0));
    ind = (ind + 1) % max_values;
    

    // Calcola la somma dei valori di velocità nell'array
    for (int i = 0; i < max_values; ++i) {
        sum += velocity_values[i];
    }

    // Se l'array non è ancora pieno, restituisci la media basata su ind + 1
    if (count < max_values) {
        return sum / (ind + 1);
    } else {
        // Calcola la media dei valori di velocità nell'intero array
        return sum / max_values;
    }
}




// Callback per elaborare i dati del robot
void dataCallback(const assignment_2_2023::Posvel::ConstPtr& data){
    position_x = data->x; // Posizione x del robot
    position_y = data->y; // Posizione y del robot
    vel_x = data->vel_x; // Velocità x del robot
    vel_y = data->vel_y; // Velocità y del robot

    

    // Calcolo della distanza tra la posizione attuale del robot e la posizione desiderata
    dist = distance(position_x, position_y, goal_x, goal_y);

    // Calcolo della velocità media
    ave_speed = average_speed(vel_x, vel_y);

    // Stampa della distanza e della velocità media
    ROS_INFO("Distance to goal: %f", dist);
    ROS_INFO("Average speed: %f", ave_speed);

    // Aggiornamento del contatore
    count++;
   
}

bool send_info(assignment_2_2023::Dist_vel::Request &req, assignment_2_2023::Dist_vel::Response &res) {
    // Implementazione per ottenere le coordinate dell'ultimo goal inviato
    res.distance = dist;  // Assegna la coordinata x del goal
    res.average_velocity = ave_speed;  // Assegna la coordinata y del goal
    return true;  // Restituisce true per indicare che il servizio è stato gestito correttamente
}

int main(int argc, char **argv){
    // Inizializzazione del nodo
    ros::init(argc, argv, "dist_vel_service");
    ros::NodeHandle nh;
	ros::Subscriber sub = nh.subscribe("/posvel", 1, dataCallback);
	ros::Subscriber sub_goal = nh.subscribe<assignment_2_2023::PlanningActionGoal>("/reaching_goal/goal", 1, target_position);
	ros::ServiceServer service=nh.advertiseService("/dist_vel_service",send_info);
	// Creazione del subscriber e associare la callback per i dati del robot
    
    
	if (ros::param::get("/dist_vel_service/max_values", max_values)) {
    	
    	if (max_values > 0) {
        	velocity_values = new float[max_values];
    	} else {
        	ROS_ERROR("Invalid value for 'averaging_window'. It should be a positive integer.");
        	return 1; // Esci dal nodo con un codice di errore
    	}
	} else {
    	ROS_ERROR("Failed to retrieve 'averaging_window' parameter.");
    	return 1; // Esci dal nodo con un codice di errore
	}


   
    // Processare i callback dei messaggi
    ros::spin();
	delete[] velocity_values;
    return 0;
}

