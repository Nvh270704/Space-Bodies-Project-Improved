// tests/test_planet.cpp

#include <gtest/gtest.h>
#include "src/planets.h"
#include "src/classes.h"
#include "json.hpp"

using json = nlohmann::json;

// Test Fixture for Planet
class PlanetTest : public ::testing::Test {
protected:
    // You can add common setup code here if needed
    void SetUp() override {}

    void TearDown() override {}
};

// Test Planet constructor and getters
TEST_F(PlanetTest, ConstructorAndGetters) {
    Planet mars("Mars", 6779.0, 6.4171e23); // Diameter in km, mass in kg
    EXPECT_EQ(mars.getName(), "Mars");
    EXPECT_DOUBLE_EQ(mars.getDiameter(), 6779.0);
    EXPECT_DOUBLE_EQ(mars.getMass(), 6.4171e23);
}

// Test calculateSurfaceGravity with Earth data
TEST_F(PlanetTest, CalculateSurfaceGravity_Earth) {
    Planet earth("Earth", 12742.0, 5.97237e24);
    double gravity = earth.calculateSurfaceGravity();
    EXPECT_NEAR(gravity, 9.807, 0.01); // Known surface gravity of Earth in m/s^2
}

// Test calculateEscapeVelocity with Earth data
TEST_F(PlanetTest, CalculateEscapeVelocity_Earth) {
    Planet earth("Earth", 12742.0, 5.97237e24);
    double escapeVelocity = earth.calculateEscapeVelocity();
    EXPECT_NEAR(escapeVelocity, 11.186, 0.001); // Known escape velocity of Earth in km/s
}

// Test Planet constructor with invalid diameter
TEST_F(PlanetTest, Constructor_InvalidDiameter) {
    // Assuming constructor throws std::invalid_argument for invalid diameter
    EXPECT_THROW(Planet("InvalidPlanet", -500.0, 5.97237e24), std::invalid_argument);
}

// Test Planet constructor with invalid mass
TEST_F(PlanetTest, Constructor_InvalidMass) {
    // Assuming constructor throws std::invalid_argument for invalid mass
    EXPECT_THROW(Planet("InvalidPlanet", 12742.0, -1.0), std::invalid_argument);
}

// Test calculateSurfaceGravity with extreme values
TEST_F(PlanetTest, CalculateSurfaceGravity_ExtremeValues) {
    // Very small planet
    Planet tinyPlanet("Tiny", 1.0, 1e15); // Diameter in km, mass in kg
    double gravityTiny = tinyPlanet.calculateSurfaceGravity();
    EXPECT_GT(gravityTiny, 0.0);

    // Very large planet
    Planet giantPlanet("Giant", 100000.0, 1e30); // Diameter in km, mass in kg
    double gravityGiant = giantPlanet.calculateSurfaceGravity();
    EXPECT_GT(gravityGiant, gravityTiny);
}

// Test calculateEscapeVelocity with extreme values
TEST_F(PlanetTest, CalculateEscapeVelocity_ExtremeValues) {
    // Very small planet
    Planet tinyPlanet("Tiny", 1.0, 1e15); // Diameter in km, mass in kg
    double escapeVelocityTiny = tinyPlanet.calculateEscapeVelocity();
    EXPECT_GT(escapeVelocityTiny, 0.0);

    // Very large planet
    Planet giantPlanet("Giant", 100000.0, 1e30); // Diameter in km, mass in kg
    double escapeVelocityGiant = giantPlanet.calculateEscapeVelocity();
    EXPECT_GT(escapeVelocityGiant, escapeVelocityTiny);
}

// Test Planet's handleImpact method (Assuming it's implemented)
TEST_F(PlanetTest, HandleImpact) {
    // Initialize Earth
    Planet earth("Earth", 12742.0, 5.97237e24);
    
    // Create a sample asteroid
    json asteroidData = {
        {"id", "999999"},
        {"neo_reference_id", "999999"},
        {"name", "ImpactAsteroid"},
        {"nasa_jpl_url", "https://ssd.jpl.nasa.gov/tools/sbdb_lookup.html#/?sstr=999999"},
        {"absolute_magnitude_h", 17.0},
        {"estimated_diameter", {
            {"kilometers", {
                {"estimated_diameter_min", 1.0},
                {"estimated_diameter_max", 2.0}
            }}
        }},
        {"is_potentially_hazardous_asteroid", true},
        {"close_approach_data", {
            {
                {"close_approach_date", "2025-01-01"},
                {"relative_velocity", {
                    {"kilometers_per_second", "10.0"}
                }},
                {"miss_distance", {
                    {"kilometers", "1000000"}
                }},
                {"orbiting_body", "Earth"}
            }
        }}
    };
    Asteroid asteroid(asteroidData);
    
    // Record Earth's original mass
    double originalMass = earth.getMass();
    
    // Simulate impact
    earth.handleImpact(asteroid);
    
    // Expected new mass
    double expectedMass = originalMass - asteroid.getMass();
    
    EXPECT_DOUBLE_EQ(earth.getMass(), expectedMass);
}

// Main function for running tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
