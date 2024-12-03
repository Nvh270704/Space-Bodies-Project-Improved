// tests/test_asteroid.cpp

#include <gtest/gtest.h>
#include "src/classes.h"      // Includes SpaceBody and Asteroid classes
#include "json.hpp"
#include <fstream>
#include <stdexcept>

using json = nlohmann::json;

// Test Fixture for Asteroid
class AsteroidTest : public ::testing::Test {
protected:
    // Helper function to load JSON from a file
    json loadJSONFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filepath);
        }
        json j;
        file >> j;
        return j;
    }
};

// Test Asteroid constructor with valid JSON from file
TEST_F(AsteroidTest, ConstructorValidJSONFromFile) {
    // Path to your JSON test data
    std::string filepath = "tests/data/asteroid_sample1.json";
    
    // Load JSON data
    json asteroidData = loadJSONFromFile(filepath);
    
    // Create Asteroid object
    Asteroid asteroid(asteroidData);

    // Assertions
    EXPECT_EQ(asteroid.getName(), "154229 (2002 JN97)");
    EXPECT_EQ(asteroid.getID(), "2154229");
    EXPECT_DOUBLE_EQ(asteroid.getDiameter(), 1.2547435637); // Assuming getDiameter returns minDiameterKm
    EXPECT_FALSE(asteroid.isDangerous());
    EXPECT_EQ(asteroid.getNasaJplUrl(), "https://ssd.jpl.nasa.gov/tools/sbdb_lookup.html#/?sstr=2154229");
    EXPECT_DOUBLE_EQ(asteroid.getAbsoluteMagnitude(), 16.63);
    EXPECT_DOUBLE_EQ(asteroid.getRelativeVelocityKmPerS(), 19.7506484939);
    EXPECT_DOUBLE_EQ(asteroid.getMissDistanceKm(), 31976921.0783012395); // 63953842.156602479 / 2.0
}

// Test Asteroid constructor with hazardous asteroid JSON from file
TEST_F(AsteroidTest, ConstructorHazardousAsteroidFromFile) {
    // Path to your JSON test data
    std::string filepath = "tests/data/asteroid_sample2.json";
    
    // Load JSON data
    json asteroidData = loadJSONFromFile(filepath);
    
    // Create Asteroid object
    Asteroid asteroid(asteroidData);

    // Assertions
    EXPECT_EQ(asteroid.getName(), "230111 (2001 BE10)");
    EXPECT_EQ(asteroid.getID(), "2230111");
    EXPECT_DOUBLE_EQ(asteroid.getDiameter(), 0.3703010782);
    EXPECT_TRUE(asteroid.isDangerous());
    EXPECT_EQ(asteroid.getNasaJplUrl(), "https://ssd.jpl.nasa.gov/tools/sbdb_lookup.html#/?sstr=2230111");
    EXPECT_DOUBLE_EQ(asteroid.getAbsoluteMagnitude(), 19.28);
    EXPECT_DOUBLE_EQ(asteroid.getRelativeVelocityKmPerS(), 16.6547709893);
    EXPECT_DOUBLE_EQ(asteroid.getMissDistanceKm(), 37023507.7746987215); // 74047015.549397443 / 2.0
}

// Test Asteroid constructor with incomplete JSON (missing estimated_diameter)
TEST_F(AsteroidTest, ConstructorIncompleteJSON) {
    // Create JSON data missing "estimated_diameter"
    json incompleteData = {
        {"id", "12345"},
        {"neo_reference_id", "12345"},
        {"name", "IncompleteAsteroid"},
        {"nasa_jpl_url", "https://ssd.jpl.nasa.gov/tools/sbdb_lookup.html#/?sstr=12345"},
        {"absolute_magnitude_h", 22.0},
        {"is_potentially_hazardous_asteroid", false},
        {"close_approach_data", {
            {
                {"close_approach_date", "2024-10-10"},
                {"relative_velocity", {{"kilometers_per_second", "5.0"}}},
                {"miss_distance", {{"kilometers", "500000"}}},
                {"orbiting_body", "Earth"}
            }
        }}
    };

    // Expect constructor to throw std::out_of_range or similar due to missing fields
    EXPECT_THROW(Asteroid asteroid(incompleteData), std::out_of_range);
}

// Test Copy Constructor
TEST_F(AsteroidTest, CopyConstructor) {
    // Path to your JSON test data
    std::string filepath = "tests/data/asteroid_sample1.json";
    
    // Load JSON data
    json asteroidData = loadJSONFromFile(filepath);
    
    // Create original Asteroid object
    Asteroid original(asteroidData);
    
    // Create copy using copy constructor
    Asteroid copy = original;
    
    // Assertions to ensure deep copy
    EXPECT_EQ(copy.getName(), original.getName());
    EXPECT_EQ(copy.getID(), original.getID());
    EXPECT_DOUBLE_EQ(copy.getDiameter(), original.getDiameter());
    EXPECT_EQ(copy.isDangerous(), original.isDangerous());
    EXPECT_EQ(copy.getNasaJplUrl(), original.getNasaJplUrl());
    EXPECT_DOUBLE_EQ(copy.getAbsoluteMagnitude(), original.getAbsoluteMagnitude());
    EXPECT_DOUBLE_EQ(copy.getRelativeVelocityKmPerS(), original.getRelativeVelocityKmPerS());
    EXPECT_DOUBLE_EQ(copy.getMissDistanceKm(), original.getMissDistanceKm());
}

// Test calculateImpactEnergy with known values
TEST_F(AsteroidTest, CalculateImpactEnergy) {
    // Given:
    // mass = density * average volume
    // density = 3000 kg/m^3
    // diameter_min = 0.5 km, diameter_max = 1.0 km
    // average diameter = 0.75 km => radius = 375 m
    // volume = (4/3) * π * r^3 ≈ 2.2e8 m^3
    // mass ≈ 3000 * 2.2e8 = 6.6e11 kg
    // velocity = 5 km/s = 5000 m/s
    // energy = 0.5 * m * v^2 ≈ 0.5 * 6.6e11 * 25e6 = 8.25e18 J
    // energy in megatons = 8.25e18 / 4.184e15 ≈ 1973.6 MT

    // Create JSON data
    json asteroidData = {
        {"id", "67890"},
        {"neo_reference_id", "67890"},
        {"name", "EnergyTestAsteroid"},
        {"nasa_jpl_url", "https://ssd.jpl.nasa.gov/tools/sbdb_lookup.html#/?sstr=67890"},
        {"absolute_magnitude_h", 20.0},
        {"estimated_diameter", {
            {"kilometers", {
                {"estimated_diameter_min", 0.75},
                {"estimated_diameter_max", 0.75}
            }}
        }},
        {"is_potentially_hazardous_asteroid", true},
        {"close_approach_data", {
            {
                {"close_approach_date", "2024-12-31"},
                {"relative_velocity", {{"kilometers_per_second", "5.0"}}},
                {"miss_distance", {{"kilometers", "1000000"}}},
                {"orbiting_body", "Earth"}
            }
        }}
    };

    // Create Asteroid object
    Asteroid asteroid(asteroidData);

    // Calculate impact energy
    double impactEnergy = asteroid.calculateImpactEnergy();

    EXPECT_NEAR(impactEnergy, 1973.6, 50.0); // Allowing a tolerance of 50 MT
}

// Test operator+ by combining two asteroids
TEST_F(AsteroidTest, OperatorPlus) {
    // Create first asteroid
    json asteroidData1 = {
        {"id", "11111"},
        {"neo_reference_id", "11111"},
        {"name", "AsteroidOne"},
        {"nasa_jpl_url", "https://ssd.jpl.nasa.gov/tools/sbdb_lookup.html#/?sstr=11111"},
        {"absolute_magnitude_h", 25.0},
        {"estimated_diameter", {
            {"kilometers", {
                {"estimated_diameter_min", 0.5},
                {"estimated_diameter_max", 1.0}
            }}
        }},
        {"is_potentially_hazardous_asteroid", true},
        {"close_approach_data", {
            {
                {"close_approach_date", "2024-10-10"},
                {"relative_velocity", {{"kilometers_per_second", "10.0"}}},
                {"miss_distance", {{"kilometers", "1000000"}}},
                {"orbiting_body", "Earth"}
            }
        }}
    };
    Asteroid asteroid1(asteroidData1);

    // Create second asteroid
    json asteroidData2 = {
        {"id", "22222"},
        {"neo_reference_id", "22222"},
        {"name", "AsteroidTwo"},
        {"nasa_jpl_url", "https://ssd.jpl.nasa.gov/tools/sbdb_lookup.html#/?sstr=22222"},
        {"absolute_magnitude_h", 22.0},
        {"estimated_diameter", {
            {"kilometers", {
                {"estimated_diameter_min", 0.7},
                {"estimated_diameter_max", 1.2}
            }}
        }},
        {"is_potentially_hazardous_asteroid", true},
        {"close_approach_data", {
            {
                {"close_approach_date", "2024-12-15"},
                {"relative_velocity", {{"kilometers_per_second", "12.5"}}},
                {"miss_distance", {{"kilometers", "7500000"}}},
                {"orbiting_body", "Earth"}
            }
        }}
    };
    Asteroid asteroid2(asteroidData2);

    // Combine asteroids using operator+
    Asteroid combined = asteroid1 + asteroid2;

    // Assertions
    EXPECT_EQ(combined.getName(), "AsteroidOne & AsteroidTwo");
    EXPECT_DOUBLE_EQ(combined.getDiameter(), 1.2); // 0.5 + 0.7
    EXPECT_DOUBLE_EQ(combined.getMass(), asteroid1.getMass() + asteroid2.getMass());
    EXPECT_DOUBLE_EQ(combined.getRelativeVelocityKmPerS(), 22.5); // 10.0 + 12.5
    EXPECT_DOUBLE_EQ(combined.getMissDistanceKm(), 4250000.0); // (1000000 / 2) + (7500000 / 2)
    EXPECT_TRUE(combined.isDangerous());
}

// Test calculateSurfaceGravity inherited from SpaceBody
TEST_F(AsteroidTest, CalculateSurfaceGravity) {
    // Create JSON data
    json asteroidData = {
        {"id", "33333"},
        {"neo_reference_id", "33333"},
        {"name", "GravityTestAsteroid"},
        {"nasa_jpl_url", "https://ssd.jpl.nasa.gov/tools/sbdb_lookup.html#/?sstr=33333"},
        {"absolute_magnitude_h", 18.0},
        {"estimated_diameter", {
            {"kilometers", {
                {"estimated_diameter_min", 0.6},
                {"estimated_diameter_max", 1.2}
            }}
        }},
        {"is_potentially_hazardous_asteroid", true},
        {"close_approach_data", {
            {
                {"close_approach_date", "2024-11-20"},
                {"relative_velocity", {{"kilometers_per_second", "7.0"}}},
                {"miss_distance", {{"kilometers", "2000000"}}},
                {"orbiting_body", "Earth"}
            }
        }}
    };
    Asteroid asteroid(asteroidData);
    double gravity = asteroid.calculateSurfaceGravity();
    EXPECT_GT(gravity, 0.0); // Ensure gravity is positive
}

// Test calculateEscapeVelocity inherited from SpaceBody
TEST_F(AsteroidTest, CalculateEscapeVelocity) {
    // Create JSON data
    json asteroidData = {
        {"id", "44444"},
        {"neo_reference_id", "44444"},
        {"name", "EscapeVelocityAsteroid"},
        {"nasa_jpl_url", "https://ssd.jpl.nasa.gov/tools/sbdb_lookup.html#/?sstr=44444"},
        {"absolute_magnitude_h", 19.5},
        {"estimated_diameter", {
            {"kilometers", {
                {"estimated_diameter_min", 0.8},
                {"estimated_diameter_max", 1.6}
            }}
        }},
        {"is_potentially_hazardous_asteroid", true},
        {"close_approach_data", {
            {
                {"close_approach_date", "2024-12-05"},
                {"relative_velocity", {{"kilometers_per_second", "9.0"}}},
                {"miss_distance", {{"kilometers", "3000000"}}},
                {"orbiting_body", "Earth"}
            }
        }}
    };
    Asteroid asteroid(asteroidData);
    double escapeVelocity = asteroid.calculateEscapeVelocity();
    EXPECT_GT(escapeVelocity, 0.0); // Ensure escape velocity is positive
}

// Test Impact Energy with zero velocity
TEST_F(AsteroidTest, ImpactEnergyZeroVelocity) {
    // Create JSON data with zero velocity
    json asteroidData = {
        {"id", "55555"},
        {"neo_reference_id", "55555"},
        {"name", "ZeroVelocityAsteroid"},
        {"nasa_jpl_url", "https://ssd.jpl.nasa.gov/tools/sbdb_lookup.html#/?sstr=55555"},
        {"absolute_magnitude_h", 21.0},
        {"estimated_diameter", {
            {"kilometers", {
                {"estimated_diameter_min", 0.4},
                {"estimated_diameter_max", 0.8}
            }}
        }},
        {"is_potentially_hazardous_asteroid", false},
        {"close_approach_data", {
            {
                {"close_approach_date", "2025-01-10"},
                {"relative_velocity", {{"kilometers_per_second", "0.0"}}},
                {"miss_distance", {{"kilometers", "1000000"}}},
                {"orbiting_body", "Earth"}
            }
        }}
    };
    Asteroid asteroid(asteroidData);
    double impactEnergy = asteroid.calculateImpactEnergy();
    EXPECT_DOUBLE_EQ(impactEnergy, 0.0);
}

// Test Asteroid with multiple close approaches
TEST_F(AsteroidTest, ConstructorMultipleCloseApproachData) {
    // Create JSON data with multiple close approaches
    json asteroidData = {
        {"id", "66666"},
        {"neo_reference_id", "66666"},
        {"name", "MultipleApproachAsteroid"},
        {"nasa_jpl_url", "https://ssd.jpl.nasa.gov/tools/sbdb_lookup.html#/?sstr=66666"},
        {"absolute_magnitude_h", 17.5},
        {"estimated_diameter", {
            {"kilometers", {
                {"estimated_diameter_min", 0.9},
                {"estimated_diameter_max", 1.8}
            }}
        }},
        {"is_potentially_hazardous_asteroid", true},
        {"close_approach_data", {
            {
                {"close_approach_date", "2024-11-01"},
                {"relative_velocity", {{"kilometers_per_second", "8.5"}}},
                {"miss_distance", {{"kilometers", "4000000"}}},
                {"orbiting_body", "Earth"}
            },
            {
                {"close_approach_date", "2025-02-20"},
                {"relative_velocity", {{"kilometers_per_second", "10.0"}}},
                {"miss_distance", {{"kilometers", "5000000"}}},
                {"orbiting_body", "Earth"}
            }
        }}
    };
    Asteroid asteroid(asteroidData);

    // Assertions
    EXPECT_EQ(asteroid.getName(), "MultipleApproachAsteroid");
    EXPECT_EQ(asteroid.getID(), "66666");
    EXPECT_DOUBLE_EQ(asteroid.getDiameter(), 0.9);
    EXPECT_TRUE(asteroid.isDangerous());
    EXPECT_EQ(asteroid.getNasaJplUrl(), "https://ssd.jpl.nasa.gov/tools/sbdb_lookup.html#/?sstr=66666");
    EXPECT_DOUBLE_EQ(asteroid.getAbsoluteMagnitude(), 17.5);

    // Verify close approach data
    std::vector<CloseApproachData> approaches = asteroid.getCloseApproachData();
    ASSERT_EQ(approaches.size(), 2);

    // First approach
    EXPECT_EQ(approaches[0].date, "2024-11-01");
    EXPECT_DOUBLE_EQ(approaches[0].relative_velocity, 8.5);
    EXPECT_DOUBLE_EQ(approaches[0].miss_distance, 2000000.0); // 4000000 / 2.0 vs 6371.0 * 2 = 12742, so 2000000 > 12742

    // Second approach
    EXPECT_EQ(approaches[1].date, "2025-02-20");
    EXPECT_DOUBLE_EQ(approaches[1].relative_velocity, 10.0);
    EXPECT_DOUBLE_EQ(approaches[1].miss_distance, 2500000.0); // 5000000 / 2.0 vs 12742, so 2500000 > 12742
}

// Main function for running tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
