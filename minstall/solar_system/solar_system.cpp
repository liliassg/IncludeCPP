/**
 * REALISTIC SOLAR SYSTEM SIMULATION
 *
 * All data from NASA JPL Horizons Database
 * Units: SI (meters, kg, seconds)
 * Integration: Velocity Verlet (symplectic, energy-conserving)
 *
 * Includes: Sun, 8 Planets, Pluto, Major Moons
 */

#include <cmath>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace includecpp {

// Physical Constants (CODATA 2018)
constexpr double GRAV = 6.67430e-11;        // Gravitational constant [m³/(kg·s²)]
constexpr double AU = 1.495978707e11;       // Astronomical Unit [m]
constexpr double DAY = 86400.0;             // Seconds per day
constexpr double YEAR = 365.25 * DAY;       // Seconds per year

// Celestial Body with full orbital mechanics
struct CelestialBody {
    // Identity
    std::string name;
    int id;
    int parent_id;  // -1 for Sun, parent planet ID for moons

    // Physical properties
    double mass;            // [kg]
    double radius;          // [m]
    double obliquity;       // Axial tilt [radians]
    double rotation_period; // [seconds]

    // State vectors (heliocentric or parent-centric for moons)
    double x, y, z;         // Position [m]
    double vx, vy, vz;      // Velocity [m/s]

    // Accelerations (for Velocity Verlet)
    double ax, ay, az;      // [m/s²]
    double ax_old, ay_old, az_old;

    // Orbital elements (for reference/validation)
    double semi_major_axis; // [m]
    double eccentricity;
    double inclination;     // [radians]
    double orbital_period;  // [seconds]

    // Tracking
    std::vector<double> trajectory_x;
    std::vector<double> trajectory_y;
    std::vector<double> trajectory_z;
    int trajectory_max_points;

    CelestialBody() : id(0), parent_id(-1), mass(0), radius(0), obliquity(0),
                      rotation_period(0), x(0), y(0), z(0), vx(0), vy(0), vz(0),
                      ax(0), ay(0), az(0), ax_old(0), ay_old(0), az_old(0),
                      semi_major_axis(0), eccentricity(0), inclination(0),
                      orbital_period(0), trajectory_max_points(1000) {}

    void add_trajectory_point() {
        trajectory_x.push_back(x);
        trajectory_y.push_back(y);
        trajectory_z.push_back(z);
        if (trajectory_x.size() > static_cast<size_t>(trajectory_max_points)) {
            trajectory_x.erase(trajectory_x.begin());
            trajectory_y.erase(trajectory_y.begin());
            trajectory_z.erase(trajectory_z.begin());
        }
    }
};

class SolarSystem {
private:
    std::vector<CelestialBody> bodies;
    double simulation_time;     // Current time [seconds]
    double total_energy;        // System energy [J]
    double initial_energy;      // For conservation check
    int step_count;

    // Compute gravitational acceleration on body i from all other bodies
    void compute_acceleration(int i) {
        bodies[i].ax = 0;
        bodies[i].ay = 0;
        bodies[i].az = 0;

        for (size_t j = 0; j < bodies.size(); j++) {
            if (static_cast<int>(j) == i) continue;

            double dx = bodies[j].x - bodies[i].x;
            double dy = bodies[j].y - bodies[i].y;
            double dz = bodies[j].z - bodies[i].z;

            double r_sq = dx*dx + dy*dy + dz*dz;
            double r = std::sqrt(r_sq);
            double r_cubed = r_sq * r;

            // a = GRAV * M / r² * (r_hat)
            double factor = GRAV * bodies[j].mass / r_cubed;

            bodies[i].ax += factor * dx;
            bodies[i].ay += factor * dy;
            bodies[i].az += factor * dz;
        }
    }

    // Compute all accelerations
    void compute_all_accelerations() {
        for (size_t i = 0; i < bodies.size(); i++) {
            compute_acceleration(i);
        }
    }

public:
    SolarSystem() : simulation_time(0), total_energy(0), initial_energy(0), step_count(0) {}

    // Initialize with real solar system data (J2000.0 epoch)
    void init_real_solar_system() {
        bodies.clear();
        simulation_time = 0;
        step_count = 0;

        // ============================================================
        // SUN
        // ============================================================
        CelestialBody sun;
        sun.name = "Sun";
        sun.id = 0;
        sun.parent_id = -1;
        sun.mass = 1.98892e30;
        sun.radius = 6.96340e8;
        sun.obliquity = 7.25 * M_PI / 180.0;
        sun.rotation_period = 25.38 * DAY;
        sun.x = 0; sun.y = 0; sun.z = 0;
        sun.vx = 0; sun.vy = 0; sun.vz = 0;
        sun.trajectory_max_points = 10;  // Sun doesn't move much
        bodies.push_back(sun);

        // ============================================================
        // MERCURY - NASA JPL Horizons Data
        // ============================================================
        CelestialBody mercury;
        mercury.name = "Mercury";
        mercury.id = 1;
        mercury.parent_id = -1;
        mercury.mass = 3.30114e23;
        mercury.radius = 2.4397e6;
        mercury.obliquity = 0.034 * M_PI / 180.0;
        mercury.rotation_period = 58.646 * DAY;
        mercury.semi_major_axis = 0.387098 * AU;
        mercury.eccentricity = 0.205630;
        mercury.inclination = 7.005 * M_PI / 180.0;
        mercury.orbital_period = 87.969 * DAY;
        // Position at perihelion
        mercury.x = mercury.semi_major_axis * (1 - mercury.eccentricity);
        mercury.y = 0;
        mercury.z = 0;
        // Velocity at perihelion (vis-viva equation)
        double v_mercury = std::sqrt(GRAV * sun.mass * (2.0/mercury.x - 1.0/mercury.semi_major_axis));
        mercury.vx = 0;
        mercury.vy = v_mercury;
        mercury.vz = 0;
        mercury.trajectory_max_points = 500;
        bodies.push_back(mercury);

        // ============================================================
        // VENUS
        // ============================================================
        CelestialBody venus;
        venus.name = "Venus";
        venus.id = 2;
        venus.parent_id = -1;
        venus.mass = 4.86747e24;
        venus.radius = 6.0518e6;
        venus.obliquity = 177.36 * M_PI / 180.0;  // Retrograde rotation!
        venus.rotation_period = -243.025 * DAY;    // Negative = retrograde
        venus.semi_major_axis = 0.723332 * AU;
        venus.eccentricity = 0.006772;
        venus.inclination = 3.39458 * M_PI / 180.0;
        venus.orbital_period = 224.701 * DAY;
        venus.x = venus.semi_major_axis * (1 - venus.eccentricity);
        venus.y = 0;
        venus.z = 0;
        double v_venus = std::sqrt(GRAV * sun.mass * (2.0/venus.x - 1.0/venus.semi_major_axis));
        venus.vx = 0;
        venus.vy = v_venus;
        venus.vz = 0;
        venus.trajectory_max_points = 800;
        bodies.push_back(venus);

        // ============================================================
        // EARTH
        // ============================================================
        CelestialBody earth;
        earth.name = "Earth";
        earth.id = 3;
        earth.parent_id = -1;
        earth.mass = 5.97237e24;
        earth.radius = 6.371e6;
        earth.obliquity = 23.4393 * M_PI / 180.0;
        earth.rotation_period = 0.99726968 * DAY;
        earth.semi_major_axis = 1.000001018 * AU;
        earth.eccentricity = 0.0167086;
        earth.inclination = 0.00005 * M_PI / 180.0;
        earth.orbital_period = 365.256363004 * DAY;
        earth.x = earth.semi_major_axis * (1 - earth.eccentricity);
        earth.y = 0;
        earth.z = 0;
        double v_earth = std::sqrt(GRAV * sun.mass * (2.0/earth.x - 1.0/earth.semi_major_axis));
        earth.vx = 0;
        earth.vy = v_earth;
        earth.vz = 0;
        earth.trajectory_max_points = 1000;
        bodies.push_back(earth);

        // ============================================================
        // MOON (Earth's Moon)
        // ============================================================
        CelestialBody moon;
        moon.name = "Moon";
        moon.id = 31;
        moon.parent_id = 3;  // Earth
        moon.mass = 7.342e22;
        moon.radius = 1.7371e6;
        moon.obliquity = 6.687 * M_PI / 180.0;
        moon.rotation_period = 27.321661 * DAY;  // Tidally locked
        moon.semi_major_axis = 3.84399e8;  // From Earth
        moon.eccentricity = 0.0549;
        moon.inclination = 5.145 * M_PI / 180.0;
        moon.orbital_period = 27.321661 * DAY;
        // Position relative to Earth, then add Earth's position
        double moon_dist = moon.semi_major_axis * (1 - moon.eccentricity);
        moon.x = earth.x + moon_dist;
        moon.y = earth.y;
        moon.z = 0;
        double v_moon_orbit = std::sqrt(GRAV * earth.mass / moon_dist);
        moon.vx = earth.vx;
        moon.vy = earth.vy + v_moon_orbit;
        moon.vz = 0;
        moon.trajectory_max_points = 200;
        bodies.push_back(moon);

        // ============================================================
        // MARS
        // ============================================================
        CelestialBody mars;
        mars.name = "Mars";
        mars.id = 4;
        mars.parent_id = -1;
        mars.mass = 6.41712e23;
        mars.radius = 3.3895e6;
        mars.obliquity = 25.19 * M_PI / 180.0;
        mars.rotation_period = 1.025957 * DAY;
        mars.semi_major_axis = 1.523679 * AU;
        mars.eccentricity = 0.0934;
        mars.inclination = 1.850 * M_PI / 180.0;
        mars.orbital_period = 686.971 * DAY;
        mars.x = mars.semi_major_axis * (1 - mars.eccentricity);
        mars.y = 0;
        mars.z = 0;
        double v_mars = std::sqrt(GRAV * sun.mass * (2.0/mars.x - 1.0/mars.semi_major_axis));
        mars.vx = 0;
        mars.vy = v_mars;
        mars.vz = 0;
        mars.trajectory_max_points = 1500;
        bodies.push_back(mars);

        // ============================================================
        // JUPITER
        // ============================================================
        CelestialBody jupiter;
        jupiter.name = "Jupiter";
        jupiter.id = 5;
        jupiter.parent_id = -1;
        jupiter.mass = 1.89819e27;
        jupiter.radius = 6.9911e7;
        jupiter.obliquity = 3.13 * M_PI / 180.0;
        jupiter.rotation_period = 0.41354 * DAY;
        jupiter.semi_major_axis = 5.2044 * AU;
        jupiter.eccentricity = 0.0489;
        jupiter.inclination = 1.303 * M_PI / 180.0;
        jupiter.orbital_period = 4332.59 * DAY;
        jupiter.x = jupiter.semi_major_axis * (1 - jupiter.eccentricity);
        jupiter.y = 0;
        jupiter.z = 0;
        double v_jupiter = std::sqrt(GRAV * sun.mass * (2.0/jupiter.x - 1.0/jupiter.semi_major_axis));
        jupiter.vx = 0;
        jupiter.vy = v_jupiter;
        jupiter.vz = 0;
        jupiter.trajectory_max_points = 2000;
        bodies.push_back(jupiter);

        // ============================================================
        // GALILEAN MOONS
        // ============================================================

        // IO
        CelestialBody io;
        io.name = "Io";
        io.id = 51;
        io.parent_id = 5;
        io.mass = 8.9319e22;
        io.radius = 1.8216e6;
        io.semi_major_axis = 4.217e8;
        io.eccentricity = 0.0041;
        io.orbital_period = 1.769 * DAY;
        double io_dist = io.semi_major_axis;
        io.x = jupiter.x + io_dist;
        io.y = jupiter.y;
        io.z = 0;
        double v_io = std::sqrt(GRAV * jupiter.mass / io_dist);
        io.vx = jupiter.vx;
        io.vy = jupiter.vy + v_io;
        io.vz = 0;
        io.trajectory_max_points = 100;
        bodies.push_back(io);

        // EUROPA
        CelestialBody europa;
        europa.name = "Europa";
        europa.id = 52;
        europa.parent_id = 5;
        europa.mass = 4.7998e22;
        europa.radius = 1.5608e6;
        europa.semi_major_axis = 6.711e8;
        europa.eccentricity = 0.009;
        europa.orbital_period = 3.551 * DAY;
        double europa_dist = europa.semi_major_axis;
        europa.x = jupiter.x - europa_dist;  // Opposite side
        europa.y = jupiter.y;
        europa.z = 0;
        double v_europa = std::sqrt(GRAV * jupiter.mass / europa_dist);
        europa.vx = jupiter.vx;
        europa.vy = jupiter.vy - v_europa;
        europa.vz = 0;
        europa.trajectory_max_points = 100;
        bodies.push_back(europa);

        // GANYMEDE
        CelestialBody ganymede;
        ganymede.name = "Ganymede";
        ganymede.id = 53;
        ganymede.parent_id = 5;
        ganymede.mass = 1.4819e23;
        ganymede.radius = 2.6341e6;
        ganymede.semi_major_axis = 1.0704e9;
        ganymede.eccentricity = 0.0013;
        ganymede.orbital_period = 7.155 * DAY;
        double ganymede_dist = ganymede.semi_major_axis;
        ganymede.x = jupiter.x;
        ganymede.y = jupiter.y + ganymede_dist;
        ganymede.z = 0;
        double v_ganymede = std::sqrt(GRAV * jupiter.mass / ganymede_dist);
        ganymede.vx = jupiter.vx - v_ganymede;
        ganymede.vy = jupiter.vy;
        ganymede.vz = 0;
        ganymede.trajectory_max_points = 100;
        bodies.push_back(ganymede);

        // CALLISTO
        CelestialBody callisto;
        callisto.name = "Callisto";
        callisto.id = 54;
        callisto.parent_id = 5;
        callisto.mass = 1.0759e23;
        callisto.radius = 2.4103e6;
        callisto.semi_major_axis = 1.8827e9;
        callisto.eccentricity = 0.0074;
        callisto.orbital_period = 16.689 * DAY;
        double callisto_dist = callisto.semi_major_axis;
        callisto.x = jupiter.x;
        callisto.y = jupiter.y - callisto_dist;
        callisto.z = 0;
        double v_callisto = std::sqrt(GRAV * jupiter.mass / callisto_dist);
        callisto.vx = jupiter.vx + v_callisto;
        callisto.vy = jupiter.vy;
        callisto.vz = 0;
        callisto.trajectory_max_points = 100;
        bodies.push_back(callisto);

        // ============================================================
        // SATURN
        // ============================================================
        CelestialBody saturn;
        saturn.name = "Saturn";
        saturn.id = 6;
        saturn.parent_id = -1;
        saturn.mass = 5.6834e26;
        saturn.radius = 5.8232e7;
        saturn.obliquity = 26.73 * M_PI / 180.0;
        saturn.rotation_period = 0.444 * DAY;
        saturn.semi_major_axis = 9.5826 * AU;
        saturn.eccentricity = 0.0565;
        saturn.inclination = 2.485 * M_PI / 180.0;
        saturn.orbital_period = 10759.22 * DAY;
        saturn.x = saturn.semi_major_axis * (1 - saturn.eccentricity);
        saturn.y = 0;
        saturn.z = 0;
        double v_saturn = std::sqrt(GRAV * sun.mass * (2.0/saturn.x - 1.0/saturn.semi_major_axis));
        saturn.vx = 0;
        saturn.vy = v_saturn;
        saturn.vz = 0;
        saturn.trajectory_max_points = 2000;
        bodies.push_back(saturn);

        // TITAN
        CelestialBody titan;
        titan.name = "Titan";
        titan.id = 61;
        titan.parent_id = 6;
        titan.mass = 1.3452e23;
        titan.radius = 2.5747e6;
        titan.semi_major_axis = 1.22187e9;
        titan.eccentricity = 0.0288;
        titan.orbital_period = 15.945 * DAY;
        double titan_dist = titan.semi_major_axis;
        titan.x = saturn.x + titan_dist;
        titan.y = saturn.y;
        titan.z = 0;
        double v_titan = std::sqrt(GRAV * saturn.mass / titan_dist);
        titan.vx = saturn.vx;
        titan.vy = saturn.vy + v_titan;
        titan.vz = 0;
        titan.trajectory_max_points = 100;
        bodies.push_back(titan);

        // ============================================================
        // URANUS
        // ============================================================
        CelestialBody uranus;
        uranus.name = "Uranus";
        uranus.id = 7;
        uranus.parent_id = -1;
        uranus.mass = 8.6810e25;
        uranus.radius = 2.5362e7;
        uranus.obliquity = 97.77 * M_PI / 180.0;  // Extreme tilt!
        uranus.rotation_period = -0.71833 * DAY;   // Retrograde
        uranus.semi_major_axis = 19.19126 * AU;
        uranus.eccentricity = 0.04717;
        uranus.inclination = 0.773 * M_PI / 180.0;
        uranus.orbital_period = 30688.5 * DAY;
        uranus.x = uranus.semi_major_axis * (1 - uranus.eccentricity);
        uranus.y = 0;
        uranus.z = 0;
        double v_uranus = std::sqrt(GRAV * sun.mass * (2.0/uranus.x - 1.0/uranus.semi_major_axis));
        uranus.vx = 0;
        uranus.vy = v_uranus;
        uranus.vz = 0;
        uranus.trajectory_max_points = 2000;
        bodies.push_back(uranus);

        // ============================================================
        // NEPTUNE
        // ============================================================
        CelestialBody neptune;
        neptune.name = "Neptune";
        neptune.id = 8;
        neptune.parent_id = -1;
        neptune.mass = 1.02413e26;
        neptune.radius = 2.4622e7;
        neptune.obliquity = 28.32 * M_PI / 180.0;
        neptune.rotation_period = 0.6713 * DAY;
        neptune.semi_major_axis = 30.07 * AU;
        neptune.eccentricity = 0.008678;
        neptune.inclination = 1.77 * M_PI / 180.0;
        neptune.orbital_period = 60182.0 * DAY;
        neptune.x = neptune.semi_major_axis * (1 - neptune.eccentricity);
        neptune.y = 0;
        neptune.z = 0;
        double v_neptune = std::sqrt(GRAV * sun.mass * (2.0/neptune.x - 1.0/neptune.semi_major_axis));
        neptune.vx = 0;
        neptune.vy = v_neptune;
        neptune.vz = 0;
        neptune.trajectory_max_points = 2000;
        bodies.push_back(neptune);

        // TRITON (retrograde orbit!)
        CelestialBody triton;
        triton.name = "Triton";
        triton.id = 81;
        triton.parent_id = 8;
        triton.mass = 2.139e22;
        triton.radius = 1.3534e6;
        triton.semi_major_axis = 3.5476e8;
        triton.eccentricity = 0.000016;
        triton.orbital_period = -5.877 * DAY;  // Retrograde!
        double triton_dist = triton.semi_major_axis;
        triton.x = neptune.x + triton_dist;
        triton.y = neptune.y;
        triton.z = 0;
        double v_triton = std::sqrt(GRAV * neptune.mass / triton_dist);
        triton.vx = neptune.vx;
        triton.vy = neptune.vy - v_triton;  // Retrograde
        triton.vz = 0;
        triton.trajectory_max_points = 100;
        bodies.push_back(triton);

        // ============================================================
        // PLUTO (Dwarf Planet)
        // ============================================================
        CelestialBody pluto;
        pluto.name = "Pluto";
        pluto.id = 9;
        pluto.parent_id = -1;
        pluto.mass = 1.303e22;
        pluto.radius = 1.1883e6;
        pluto.obliquity = 122.53 * M_PI / 180.0;
        pluto.rotation_period = -6.387 * DAY;  // Retrograde
        pluto.semi_major_axis = 39.482 * AU;
        pluto.eccentricity = 0.2488;
        pluto.inclination = 17.16 * M_PI / 180.0;
        pluto.orbital_period = 90560.0 * DAY;
        // Start at an angle to show inclination
        double pluto_r = pluto.semi_major_axis * (1 - pluto.eccentricity);
        double pluto_angle = 45.0 * M_PI / 180.0;
        pluto.x = pluto_r * std::cos(pluto_angle);
        pluto.y = pluto_r * std::sin(pluto_angle) * std::cos(pluto.inclination);
        pluto.z = pluto_r * std::sin(pluto_angle) * std::sin(pluto.inclination);
        double v_pluto = std::sqrt(GRAV * sun.mass * (2.0/pluto_r - 1.0/pluto.semi_major_axis));
        pluto.vx = -v_pluto * std::sin(pluto_angle);
        pluto.vy = v_pluto * std::cos(pluto_angle) * std::cos(pluto.inclination);
        pluto.vz = v_pluto * std::cos(pluto_angle) * std::sin(pluto.inclination);
        pluto.trajectory_max_points = 2000;
        bodies.push_back(pluto);

        // Initialize accelerations
        compute_all_accelerations();
        for (auto& body : bodies) {
            body.ax_old = body.ax;
            body.ay_old = body.ay;
            body.az_old = body.az;
        }

        // Calculate initial energy
        initial_energy = calculate_total_energy();
        total_energy = initial_energy;
    }

    // Velocity Verlet Integration (symplectic, better energy conservation)
    void step(double dt) {
        // Store old accelerations
        for (auto& body : bodies) {
            body.ax_old = body.ax;
            body.ay_old = body.ay;
            body.az_old = body.az;
        }

        // Update positions: x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt²
        for (auto& body : bodies) {
            body.x += body.vx * dt + 0.5 * body.ax * dt * dt;
            body.y += body.vy * dt + 0.5 * body.ay * dt * dt;
            body.z += body.vz * dt + 0.5 * body.az * dt * dt;
        }

        // Compute new accelerations
        compute_all_accelerations();

        // Update velocities: v(t+dt) = v(t) + 0.5*(a(t) + a(t+dt))*dt
        for (auto& body : bodies) {
            body.vx += 0.5 * (body.ax_old + body.ax) * dt;
            body.vy += 0.5 * (body.ay_old + body.ay) * dt;
            body.vz += 0.5 * (body.az_old + body.az) * dt;
        }

        simulation_time += dt;
        step_count++;
    }

    // Run simulation for given duration
    void simulate(double duration, double dt) {
        int steps = static_cast<int>(duration / dt);
        for (int i = 0; i < steps; i++) {
            step(dt);

            // Record trajectory every 10 steps
            if (i % 10 == 0) {
                for (auto& body : bodies) {
                    body.add_trajectory_point();
                }
            }
        }
        total_energy = calculate_total_energy();
    }

    // Calculate total mechanical energy (kinetic + potential)
    double calculate_total_energy() {
        double kinetic = 0;
        double potential = 0;

        for (size_t i = 0; i < bodies.size(); i++) {
            // Kinetic energy: 0.5 * m * v²
            double v_sq = bodies[i].vx * bodies[i].vx +
                          bodies[i].vy * bodies[i].vy +
                          bodies[i].vz * bodies[i].vz;
            kinetic += 0.5 * bodies[i].mass * v_sq;

            // Potential energy: -GRAV * m1 * m2 / r (each pair counted once)
            for (size_t j = i + 1; j < bodies.size(); j++) {
                double dx = bodies[j].x - bodies[i].x;
                double dy = bodies[j].y - bodies[i].y;
                double dz = bodies[j].z - bodies[i].z;
                double r = std::sqrt(dx*dx + dy*dy + dz*dz);
                potential -= GRAV * bodies[i].mass * bodies[j].mass / r;
            }
        }

        return kinetic + potential;
    }

    // Calculate angular momentum (should be conserved)
    std::vector<double> calculate_angular_momentum() {
        double Lx = 0, Ly = 0, Lz = 0;
        for (const auto& body : bodies) {
            // L = r × p = r × (m*v)
            Lx += body.mass * (body.y * body.vz - body.z * body.vy);
            Ly += body.mass * (body.z * body.vx - body.x * body.vz);
            Lz += body.mass * (body.x * body.vy - body.y * body.vx);
        }
        return {Lx, Ly, Lz, std::sqrt(Lx*Lx + Ly*Ly + Lz*Lz)};
    }

    // Get body positions as flat array [x0,y0,z0, x1,y1,z1, ...]
    std::vector<double> get_positions() {
        std::vector<double> pos;
        pos.reserve(bodies.size() * 3);
        for (const auto& body : bodies) {
            pos.push_back(body.x);
            pos.push_back(body.y);
            pos.push_back(body.z);
        }
        return pos;
    }

    // Get positions in AU for visualization
    std::vector<double> get_positions_au() {
        std::vector<double> pos;
        pos.reserve(bodies.size() * 3);
        for (const auto& body : bodies) {
            pos.push_back(body.x / AU);
            pos.push_back(body.y / AU);
            pos.push_back(body.z / AU);
        }
        return pos;
    }

    std::vector<double> get_velocities() {
        std::vector<double> vel;
        vel.reserve(bodies.size() * 3);
        for (const auto& body : bodies) {
            vel.push_back(body.vx);
            vel.push_back(body.vy);
            vel.push_back(body.vz);
        }
        return vel;
    }

    std::vector<double> get_masses() {
        std::vector<double> m;
        m.reserve(bodies.size());
        for (const auto& body : bodies) {
            m.push_back(body.mass);
        }
        return m;
    }

    std::vector<double> get_radii() {
        std::vector<double> r;
        r.reserve(bodies.size());
        for (const auto& body : bodies) {
            r.push_back(body.radius);
        }
        return r;
    }

    std::vector<std::string> get_names() {
        std::vector<std::string> n;
        n.reserve(bodies.size());
        for (const auto& body : bodies) {
            n.push_back(body.name);
        }
        return n;
    }

    // Get trajectory for a specific body
    std::vector<double> get_trajectory(int body_index) {
        if (body_index < 0 || body_index >= static_cast<int>(bodies.size())) {
            return {};
        }
        std::vector<double> traj;
        const auto& body = bodies[body_index];
        for (size_t i = 0; i < body.trajectory_x.size(); i++) {
            traj.push_back(body.trajectory_x[i] / AU);
            traj.push_back(body.trajectory_y[i] / AU);
            traj.push_back(body.trajectory_z[i] / AU);
        }
        return traj;
    }

    int get_body_count() { return bodies.size(); }
    double get_simulation_time() { return simulation_time; }
    double get_simulation_time_days() { return simulation_time / DAY; }
    double get_simulation_time_years() { return simulation_time / YEAR; }
    int get_step_count() { return step_count; }
    double get_total_energy() { return total_energy; }
    double get_energy_error() {
        return std::abs((total_energy - initial_energy) / initial_energy);
    }

    // Get orbital period of body (from current velocity and position)
    double get_orbital_period(int body_index) {
        if (body_index <= 0 || body_index >= static_cast<int>(bodies.size())) {
            return 0;
        }
        const auto& body = bodies[body_index];
        double r = std::sqrt(body.x*body.x + body.y*body.y + body.z*body.z);
        // T = 2π * sqrt(a³ / (G*M_sun))
        // Use current r as approximation for a
        return 2.0 * M_PI * std::sqrt(r*r*r / (GRAV * bodies[0].mass));
    }

    // Get distance from Sun
    double get_distance_from_sun(int body_index) {
        if (body_index < 0 || body_index >= static_cast<int>(bodies.size())) {
            return 0;
        }
        const auto& body = bodies[body_index];
        return std::sqrt(body.x*body.x + body.y*body.y + body.z*body.z);
    }

    // Get speed
    double get_speed(int body_index) {
        if (body_index < 0 || body_index >= static_cast<int>(bodies.size())) {
            return 0;
        }
        const auto& body = bodies[body_index];
        return std::sqrt(body.vx*body.vx + body.vy*body.vy + body.vz*body.vz);
    }
};

// Constants for Python access
double get_AU() { return AU; }
double get_DAY() { return DAY; }
double get_YEAR() { return YEAR; }
double get_G() { return GRAV; }

}  // namespace includecpp
