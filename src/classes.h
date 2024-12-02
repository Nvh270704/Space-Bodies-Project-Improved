// src/classes.h

#ifndef CLASS_H
#define CLASS_H

#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "platform_config.h"  // For conditional compilation
#include "json.hpp"          // Ensure json.hpp is included

using namespace std;
using json = nlohmann::json;

// Struct to represent Close Approach Data
struct CloseApproachData {
    string date;
    double relative_velocity; // in km/s
    double miss_distance;     // in km
};

// Base class for Space Bodies (e.g., Planets, Asteroids)
class SpaceBody {
public:
    SpaceBody(const string& name, double diameter, double mass);
    virtual void printInfo() const;
    double calculateSurfaceGravity() const;
    double calculateEscapeVelocity() const;
    double getMass() const;
    double getDiameter() const;
    string getName() const; // Getter for name
    virtual ~SpaceBody();

protected:
    string name;
    double diameter;  // in kilometers
    double mass;      // in kilograms
};

// Derived class for Planets
class Planet : public SpaceBody {
public:
    Planet(const string& name, double diameter, double mass);
    void printInfo() const override;
    void handleImpact(const Asteroid& asteroid); // New method for handling impacts
    ~Planet();
};

// Derived class for Asteroids
class Asteroid : public SpaceBody {
public:
    Asteroid(const json& asteroidData);
    Asteroid(const Asteroid& other);
    void printInfo() const override;
    double calculateImpactEnergy() const;
    Asteroid operator+(const Asteroid& other) const;
    ~Asteroid();

    // Getter Methods
    string getID() const;
    string getNasaJplUrl() const;
    double getAbsoluteMagnitude() const;
    vector<CloseApproachData> getCloseApproachData() const;
    double getRelativeVelocityKmPerS() const;
    double getMissDistanceKm() const;

private:
    string id;
    string nasa_jpl_url;
    double absolute_magnitude;
    double minDiameterKm;
    double maxDiameterKm;
    bool isDangerous;
    vector<CloseApproachData> closeApproachDataList; // Store multiple close approaches

    static double calculateMass(const json& asteroidData);
};

#endif
