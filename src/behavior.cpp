//
//  behavior.cpp
//  
//
//  Created by Michael on 8/5/17.
//
//

#include "behavior.hpp"

int BehaviorPlanner::lanePlanner(double s, double d, vector<vector<double>> sensor_fusion) {
  int lane = laneCalc(d);
  curr_lane = lane;
  int new_lane;
  bool blocked = false;
  double distance = closestVehicle(s, lane, sensor_fusion)[0];
  // check if blocked, i.e. car is within 50 meters
  if (distance > 30) {
    new_lane = lane;
    target_vehicle_speed = 22.352 - 1;
    target_vehicle_s = 10000;
    return 0;
  } else {
    new_lane = laneScore(s, lane, sensor_fusion);
    target_vehicle_speed = closestVehicle(s, new_lane, sensor_fusion)[1];
    target_vehicle_s = closestVehicle(s, new_lane, sensor_fusion)[2];
  }
  
  if (new_lane == lane) {
    return 0;
  } else if (new_lane < lane) {
    return -4;
  } else {
    return 4;
  }
}

int BehaviorPlanner::laneCalc(double d) {
  // Check which lane the d-value comes from
  // Left is 0, middle is 1, right is 2
  int lane;
  if (d < 4) {
    lane = 0;
  } else if (d < 8) {
    lane = 1;
  } else {
    lane = 2;
  }
  return lane;
}

vector<double> BehaviorPlanner::closestVehicle(double s, int lane, vector<vector<double>> sensor_fusion) {
  double dist = 10000;
  double velocity;
  double target_s;
  int vehicle_lane;
  double vehicle_s;
  double vehicle_d;
  double vehicle_v;
  
  for(int vehicle = 0; vehicle < sensor_fusion.size(); vehicle++) {
    vehicle_s = sensor_fusion[vehicle][5];
    vehicle_d = sensor_fusion[vehicle][6];
    vehicle_v = sqrt(pow(sensor_fusion[vehicle][3], 2)+pow(sensor_fusion[vehicle][4], 2));
    vehicle_lane = laneCalc(vehicle_d);
    
    if (vehicle_lane == lane) { // if same lane
      if (vehicle_s > s) { // and ahead of my vehicle
        if (vehicle_s - s < dist) {
          dist = vehicle_s - s;
          velocity = vehicle_v;
        }
      }
    }
  }
  return {dist, velocity, target_s};
}

int BehaviorPlanner::laneScore(double s, int lane, vector<vector<double>> sensor_fusion) {
  // Calc each lane, only compare applicable lanes
  vector <double> scores = {0,0,0};
  vector <double> vehicle;
  
  for (int i = 0; i < 3; i++) {
    if (i == lane) {  // benefit to keeping lane
      scores[i] += 0.5;
    }
    vehicle = closestVehicle(s, i, sensor_fusion);
    if (vehicle[0] > 1000) {
      scores[i] += 5;
    } else {
      scores[i] += 1 - (10/vehicle[0]); // benefit for large open distance in lane
      scores[i] += 1 - (10/(vehicle[1])); // benefit for faster car speed in lane
    }
  }
  
  if (lane == 0) {
    return max_element(scores.begin(), scores.end() - 1) - scores.begin();
  } else if (lane == 1) {
    return max_element(scores.begin(), scores.end())  - scores.begin();
  } else {
    return max_element(scores.begin() + 1, scores.end())  - scores.begin();
  }
}
