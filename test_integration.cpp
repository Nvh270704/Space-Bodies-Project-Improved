// tests/test_integration.cpp

#include <gtest/gtest.h>
#include "src/classes.h"      // Includes SpaceBody, Planet, Asteroid classes
#include "src/planets.h"      // Includes predefinedPlanets
#include "src/get_data.h"     // Includes data fetching and processing functions
#include "json.hpp"
#include <fstream>
#include <stdexcept>

using json = nlohmann::json;

// Integration Test: Simulate asteroid impact on a planet
TEST(IntegrationTest, AsteroidImpactOnPlanet) {
    // Initialize Earth using predefinedPlanets
    PlanetData earthData = SolarSystem::predefinedPlanets[2]; // Earth is the third planet in the list
    Planet earth(earthData.name, earthData.diameter, earthData.mass);

    // Path to your JSON test data
    std::string filepath = "tests/data/asteroid_sample1.json";

    // Load asteroid JSON data from file
    std::ifstream file(filepath);
    ASSERT_TRUE(file.is_open()) << "Failed to open file: " << filepath;
    json asteroidData;
    file >> asteroidData;
    file.close();

    // Create Asteroid object
    Asteroid asteroid(asteroidData);

    // Calculate impact energy
    double impactEnergy = asteroid.calculateImpactEnergy();
    EXPECT_GT(impactEnergy, 1000.0) << "Impact energy should be greater than 1000 megatons";

    // Record Earth's original mass
    double originalMass = earth.getMass();

    // Simulate impact
    earth.handleImpact(asteroid);

    // Expected new mass
    double expectedMass = originalMass - asteroid.getMass();
    EXPECT_DOUBLE_EQ(earth.getMass(), expectedMass) << "Earth's mass should be reduced by asteroid's mass after impact";

    // Optionally, verify other effects of the impact if implemented
}
