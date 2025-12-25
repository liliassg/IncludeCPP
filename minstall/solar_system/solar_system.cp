SOURCE(include/solar_system.cpp) solar_system

PUBLIC(
    solar_system CLASS(CelestialBody) {
        CONSTRUCTOR()
        METHOD(add_trajectory_point)
    }
    solar_system CLASS(SolarSystem) {
        CONSTRUCTOR()
        METHOD(calculate_angular_momentum)
        METHOD(calculate_total_energy)
        METHOD(get_body_count)
        METHOD(get_distance_from_sun, int)
        METHOD(get_energy_error)
        METHOD(get_masses)
        METHOD(get_names)
        METHOD(get_orbital_period, int)
        METHOD(get_positions)
        METHOD(get_positions_au)
        METHOD(get_radii)
        METHOD(get_simulation_time)
        METHOD(get_simulation_time_days)
        METHOD(get_simulation_time_years)
        METHOD(get_speed, int)
        METHOD(get_step_count)
        METHOD(get_total_energy)
        METHOD(get_trajectory, int)
        METHOD(get_velocities)
        METHOD(init_real_solar_system)
        METHOD(simulate, double, double)
        METHOD(step, double)
    }

    solar_system FUNCTION(get_AU)
    solar_system FUNCTION(get_DAY)
    solar_system FUNCTION(get_G)
    solar_system FUNCTION(get_YEAR)
)
