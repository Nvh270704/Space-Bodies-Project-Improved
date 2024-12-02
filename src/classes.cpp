// src/classes.cpp

#include "classes.h"

// SpaceBody Implementations
SpaceBody::SpaceBody(const string& name, double diameter, double mass)
    : name(name), diameter(diameter), mass(mass)
{
    if (diameter <= 0) {
        throw std::invalid_argument("Diameter must be positive.");
    }
    if (mass <= 0) {
        throw std::invalid_argument("Mass must be positive.");
    }
}

void SpaceBody::printInfo() const {
    cout << "Name: " << name << ", Diameter: " << diameter << " km, Mass: " << mass << " kg" << endl;
}

double SpaceBody::calculateSurfaceGravity() const {
    const double G = 6.67430e-11; // m^3 kg^-1 s^-2
    double radius_m = (diameter * 1000) / 2.0;
    return (G * mass) / (radius_m * radius_m); // m/s^2
}

double SpaceBody::calculateEscapeVelocity() const {
    const double G = 6.67430e-11; // m^3 kg^-1 s^-2
    double radiusMeters = (diameter * 1000) / 2.0;
    double escapeVelocity_m_s = sqrt((2 * G * mass) / radiusMeters);
    return escapeVelocity_m_s / 1000.0; // km/s
}

double SpaceBody::getMass() const {
    return mass;
}

double SpaceBody::getDiameter() const {
    return diameter;
}

string SpaceBody::getName() const {
    return name;
}

SpaceBody::~SpaceBody() {
    cout << "Space body " << name << " memory freed." << endl;
}

// Planet Implementations
Planet::Planet(const string& name, double diameter, double mass)
    : SpaceBody(name, diameter, mass) {}

void Planet::printInfo() const {
    cout << "Planet Name: " << name << ", Mass: " << mass << " kg, Diameter: " << diameter << " km" << endl;
    cout << "Surface Gravity: " << calculateSurfaceGravity() << " m/s^2" << endl;
    cout << "Escape Velocity: " << calculateEscapeVelocity() << " km/s" << endl;
}

void Planet::handleImpact(const Asteroid& asteroid) {
    // Example: Reduce planet's mass by asteroid's mass
    mass -= asteroid.getMass();

    // Additional logic can be implemented here
    // For example, logging the impact, updating other attributes, etc.

    cout << "Impact occurred! New mass of " << name << ": " << mass << " kg" << endl;
}

Planet::~Planet() {
    cout << "Planet " << name << " memory freed." << endl;
}

// Asteroid Implementations
Asteroid::Asteroid(const json& asteroidData)
    : SpaceBody(
        asteroidData.at("name").get<string>(),
        asteroidData.at("estimated_diameter").at("kilometers").at("estimated_diameter_min").get<double>(),
        calculateMass(asteroidData)
      ),
      id(asteroidData.at("id").get<string>()),
      nasa_jpl_url(asteroidData.at("nasa_jpl_url").get<string>()),
      absolute_magnitude(asteroidData.at("absolute_magnitude_h").get<double>()),
      isDangerous(asteroidData.at("is_potentially_hazardous_asteroid").get<bool>())
{
    auto diameter = asteroidData.at("estimated_diameter").at("kilometers");
    minDiameterKm = diameter.at("estimated_diameter_min").get<double>();
    maxDiameterKm = diameter.at("estimated_diameter_max").get<double>();

    auto close_approaches = asteroidData.at("close_approach_data");
    for (const auto& approach : close_approaches) {
        CloseApproachData data;
        data.date = approach.at("close_approach_date").get<string>();
        data.relative_velocity = stod(approach.at("relative_velocity").at("kilometers_per_second").get<string>());
        data.miss_distance = stod(approach.at("miss_distance").at("kilometers").get<string>());
        // Adjust miss_distance as per original logic
        data.miss_distance = max(data.miss_distance / 2.0, 6371.0 * 2); // EARTH_RADIUS is 6371 km
        closeApproachDataList.push_back(data);
    }

    mass = calculateMass(asteroidData);
}

Asteroid::Asteroid(const Asteroid& other)
    : SpaceBody(other.name, other.diameter, other.mass),
      id(other.id),
      nasa_jpl_url(other.nasa_jpl_url),
      absolute_magnitude(other.absolute_magnitude),
      minDiameterKm(other.minDiameterKm),
      maxDiameterKm(other.maxDiameterKm),
      isDangerous(other.isDangerous),
      closeApproachDataList(other.closeApproachDataList)
{
    cout << "Asteroid " << name << " copied." << endl;
}

void Asteroid::printInfo() const {
    cout << "Asteroid ID: " << id << endl;
    cout << "Name: " << name << endl;
    cout << "NASA JPL URL: " << nasa_jpl_url << endl;
    cout << "Absolute Magnitude (H): " << absolute_magnitude << endl;
    cout << "Diameter (Min): " << minDiameterKm << " km, Max: " << maxDiameterKm << " km" << endl;
    cout << "Is Potentially Hazardous: " << (isDangerous ? "Yes" : "No") << endl;
    for (const auto& approach : closeApproachDataList) {
        cout << "Close Approach Date: " << approach.date << endl;
        cout << "Relative Velocity: " << approach.relative_velocity << " km/s" << endl;
        cout << "Miss Distance: " << approach.miss_distance << " km" << endl;
    }
    cout << "Mass: " << mass << " kg" << endl;
    cout << "Surface Gravity: " << calculateSurfaceGravity() << " m/s^2" << endl;
    cout << "Impact Energy: " << calculateImpactEnergy() << " megatons of TNT" << endl;
}

double Asteroid::calculateImpactEnergy() const {
    double velocity_m_s = relativeVelocityKmPerS * 1000.0;
    double energy_joules = 0.5 * mass * pow(velocity_m_s, 2);
    double energy_megatons = energy_joules / 4.184e15;
    return energy_megatons;
}

Asteroid Asteroid::operator+(const Asteroid& other) const {
    Asteroid combined(*this); // Use copy constructor

    combined.name = name + " & " + other.name;
    combined.minDiameterKm += other.minDiameterKm;
    combined.maxDiameterKm += other.maxDiameterKm;
    combined.mass += other.mass;
    combined.relativeVelocityKmPerS += other.relativeVelocityKmPerS;
    combined.missDistanceKm += other.missDistanceKm;

    combined.isDangerous = ((combined.minDiameterKm > 280) || (combined.relativeVelocityKmPerS > 5.0));
    return combined;
}

Asteroid::~Asteroid() {
    cout << "Asteroid " << name << " memory freed." << endl;
}

double Asteroid::calculateMass(const json& asteroidData) {
    const double density = 3000.0; // kg/m^3
    double diameterMin_m = asteroidData.at("estimated_diameter").at("kilometers").at("estimated_diameter_min").get<double>() * 1000.0;
    double diameterMax_m = asteroidData.at("estimated_diameter").at("kilometers").at("estimated_diameter_max").get<double>() * 1000.0;

    double radiusMin = diameterMin_m / 2.0;
    double radiusMax = diameterMax_m / 2.0;

    double volumeMin = (4.0 / 3.0) * M_PI * pow(radiusMin, 3);
    double volumeMax = (4.0 / 3.0) * M_PI * pow(radiusMax, 3);

    double avgVolume = (volumeMin + volumeMax) / 2.0;

    double massCalculated = density * avgVolume;
    return massCalculated;
}

// Getter Implementations for Asteroid
string Asteroid::getID() const {
    return id;
}

string Asteroid::getNasaJplUrl() const {
    return nasa_jpl_url;
}

double Asteroid::getAbsoluteMagnitude() const {
    return absolute_magnitude;
}

vector<CloseApproachData> Asteroid::getCloseApproachData() const {
    return closeApproachDataList;
}

double Asteroid::getRelativeVelocityKmPerS() const {
    // Assuming you want the first approach's velocity
    if (!closeApproachDataList.empty()) {
        return closeApproachDataList[0].relative_velocity;
    }
    return 0.0;
}

double Asteroid::getMissDistanceKm() const {
    // Assuming you want the first approach's distance
    if (!closeApproachDataList.empty()) {
        return closeApproachDataList[0].miss_distance;
    }
    return 0.0;
}
